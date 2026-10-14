/* llm_client.c — LLM 问答客户端库实现(双通道)
 * llm_chat: 板 -> 客户网关 IP:443 (mbedTLS TLS1.2, SNI/Host=vhost, Bearer=key)
 * llm_ask : 板 -> PC 中继 /say (明文 HTTP, 免 key)
 * 注: 大缓冲一律 malloc(堆) — liteos_a 链接器对超大静态段会错位放置(实测页错误)
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "llm_client.h"

#define LLM_DEFAULT_MODEL   "deepseek-ai/DeepSeek-V4-Flash"
#define LLM_WIRE_DEBUG      1     /* 1=打印线上收发摘要(排查网关串线用, 排完改0) */
#define LLM_FD_TIMEOUT_SEC  5
#define LLM_RETRY_BUDGET    18    /* 18 x 5s = 90s 总预算 */
#define LLM_ACC_SIZE        12288
#define LLM_REQ_SIZE        10240
#define LLM_BODY_SIZE       9728
#define LLM_ESCJ_SIZE       9216
#define LLM_ASKREQ_SIZE     1200

static char g_relay_ip[32] = "192.168.1.1";
static int g_relay_port = 8443;
static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;

int llm_set_server(const char *ip, int port)
{
    if (ip != NULL) {
        if (inet_addr(ip) == INADDR_NONE) {
            return -1;
        }
        snprintf(g_relay_ip, sizeof(g_relay_ip), "%s", ip);
    }
    if (port > 0 && port < 65536) {
        g_relay_port = port;
    }
    return 0;
}

static const char *extract_content(const char *s, char *out, int out_size)
{
    const char *p = strstr(s, "\"content\":\"");
    if (p == NULL) {
        return NULL;
    }
    p += strlen("\"content\":\"");
    int o = 0;
    while (*p != '\0' && *p != '"' && o < out_size - 1) {
        if (*p == '\\' && *(p + 1) != '\0') {
            p++;
            switch (*p) {
            case 'n':  out[o++] = '\n'; break;
            case 't':  out[o++] = '\t'; break;
            case '"':  out[o++] = '"';  break;
            case '\\': out[o++] = '\\'; break;
            case 'r':  break;
            case 'u':
                if (p[1] && p[2] && p[3] && p[4]) {
                    p += 4;
                }
                break;
            default:   out[o++] = *p; break;
            }
        } else {
            out[o++] = *p;
        }
        p++;
    }
    out[o] = '\0';
    return out;
}

/* JSON 字符串转义(消息内嵌进 JSON body 用) */
static int json_escape(const char *src, char *dst, int cap)
{
    int o = 0;
    for (; *src != '\0' && o < cap - 8; src++) {
        switch (*src) {
        case '"':  dst[o++] = '\\'; dst[o++] = '"';  break;
        case '\\': dst[o++] = '\\'; dst[o++] = '\\'; break;
        case '\n': dst[o++] = '\\'; dst[o++] = 'n';  break;
        case '\r': dst[o++] = '\\'; dst[o++] = 'r';  break;
        case '\t': dst[o++] = '\\'; dst[o++] = 't';  break;
        default:
            if ((unsigned char)*src >= 0x20) {
                dst[o++] = *src;
            }
            break;
        }
    }
    dst[o] = '\0';
    return o;
}

/* 公共收尾: 从完整 HTTP 响应(acc)取正文填 out */
static int finish_reply(const char *acc, int acc_len, char *out, int out_len)
{
    const char *body = NULL;
    for (int i = 0; i + 4 <= acc_len; i++) {
        if (acc[i] == '\r' && acc[i + 1] == '\n' &&
            acc[i + 2] == '\r' && acc[i + 3] == '\n') {
            body = acc + i + 4;
            break;
        }
    }
    if (body == NULL) {
        return LLM_ERR_NORESP;
    }
    if (strncmp(acc, "HTTP/1.0 200", 12) != 0 && strncmp(acc, "HTTP/1.1 200", 12) != 0) {
        return LLM_ERR_STATUS;
    }
    char *cbuf = (char *)malloc(LLM_ACC_SIZE);
    if (cbuf == NULL) {
        return LLM_ERR_MEM;
    }
    const char *c = extract_content(body, cbuf, LLM_ACC_SIZE);
    if (c == NULL) {
        c = body;   /* 提取失败兜底: 原样给 body */
    }
    int clen = (int)strlen(c);
    int ret;
    if (clen >= out_len) {
        memcpy(out, c, (size_t)out_len - 1);
        out[out_len - 1] = '\0';
        ret = LLM_ERR_TOOSMALL;
    } else {
        memcpy(out, c, (size_t)clen);
        out[clen] = '\0';
        ret = clen;
    }
    free(cbuf);
    return ret;
}

/* ============ 通道1: llm_chat — 客户网关直连 TLS ============ */
static int llm_chat_locked(const llm_gateway_t *gw, const char *message, char *out, int out_len)
{
    char portstr[8];
    char *esc = (char *)malloc(LLM_MSG_MAX + 1);
    char *escj = (char *)malloc(LLM_ESCJ_SIZE);
    char *body = (char *)malloc(LLM_BODY_SIZE);
    char *req = (char *)malloc(LLM_REQ_SIZE);
    char *acc = (char *)malloc(LLM_ACC_SIZE);
    if (esc == NULL || escj == NULL || body == NULL || req == NULL || acc == NULL) {
        free(esc);
        free(escj);
        free(body);
        free(req);
        free(acc);
        return LLM_ERR_MEM;
    }

    int ret = LLM_ERR_TLS;
    int fd = -1;

    int mlen = (int)strlen(message);
    if (mlen > LLM_MSG_MAX) {
        mlen = LLM_MSG_MAX;
    }
    memcpy(esc, message, (size_t)mlen);
    esc[mlen] = '\0';
    json_escape(esc, escj, LLM_ESCJ_SIZE);

    int blen = snprintf(body, LLM_BODY_SIZE,
                        "{\"model\":\"%s\",\"temperature\":0.2,"
                        "\"messages\":[{\"role\":\"user\",\"content\":\"%s\"}]}",
                        (gw->model != NULL) ? gw->model : LLM_DEFAULT_MODEL, escj);
    if (blen <= 0 || blen >= LLM_BODY_SIZE) {
        ret = LLM_ERR_ARGS;
        goto buf_out;
    }

    mbedtls_net_context server_fd;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;

    mbedtls_net_init(&server_fd);
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    fd = server_fd.fd;

    const char *pers = "llm_client";
    if (mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                              (const unsigned char *)pers, strlen(pers)) != 0) {
        goto tls_out;
    }
    if (mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_STREAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT) != 0) {
        goto tls_out;
    }
    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
    if (mbedtls_ssl_setup(&ssl, &conf) != 0) {
        goto tls_out;
    }
    if (mbedtls_ssl_set_hostname(&ssl, gw->vhost) != 0) {
        goto tls_out;
    }
    snprintf(portstr, sizeof(portstr), "%d", (gw->port > 0) ? gw->port : 443);
    if (mbedtls_net_connect(&server_fd, gw->ip, portstr, MBEDTLS_NET_PROTO_TCP) != 0) {
        ret = LLM_ERR_CONNECT;
        goto tls_out;
    }
    fd = server_fd.fd;
    {
        struct timeval tv = { LLM_FD_TIMEOUT_SEC, 0 };
        setsockopt(server_fd.fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    }
    mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

    int budget = LLM_RETRY_BUDGET;
    while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
        if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
            if (--budget < 0) {
                ret = LLM_ERR_NORESP;
                goto tls_out;
            }
            continue;
        }
        ret = LLM_ERR_TLS;
        goto tls_out;
    }

    int rl = snprintf(req, LLM_REQ_SIZE,
                      "POST /v1/chat/completions HTTP/1.0\r\nHost: %s\r\n"
                      "Content-Type: application/json\r\nAuthorization: Bearer %s\r\n"
                      "Content-Length: %d\r\n\r\n%s",
                      gw->vhost, gw->api_key, blen, body);
    if (rl <= 0 || rl >= LLM_REQ_SIZE) {
        ret = LLM_ERR_SEND;
        goto tls_out;
    }
    size_t len = (size_t)rl;
    size_t sent = 0;
    budget = LLM_RETRY_BUDGET;
    while (sent < len) {
        ret = mbedtls_ssl_write(&ssl, (unsigned char *)req + sent, len - sent);
        if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
            if (--budget < 0) {
                ret = LLM_ERR_SEND;
                goto tls_out;
            }
            continue;
        }
        if (ret <= 0) {
            ret = LLM_ERR_SEND;
            goto tls_out;
        }
        sent += (size_t)ret;
    }

    int acc_len = 0;
    int n;
    budget = LLM_RETRY_BUDGET;
    for (;;) {
        n = mbedtls_ssl_read(&ssl, (unsigned char *)acc + acc_len,
                             LLM_ACC_SIZE - 1 - (size_t)acc_len);
        if (n == MBEDTLS_ERR_SSL_WANT_READ || n == MBEDTLS_ERR_SSL_WANT_WRITE) {
            if (--budget < 0) {
                ret = LLM_ERR_NORESP;
                goto tls_out;
            }
            continue;
        }
        if (n == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY || n <= 0) {
            break;
        }
        acc_len += n;
        if (acc_len >= LLM_ACC_SIZE - 1) {
            break;
        }
    }
    acc[acc_len] = '\0';
#if LLM_WIRE_DEBUG
    printf("[llm] 线上发送 %dB body, 头60: %.60s\r\n", blen, body);
    printf("[llm]                尾60: %s\r\n", (blen > 60) ? body + blen - 60 : body);
    printf("[llm] 线上收到 %dB, 头72: %.72s\r\n", acc_len, acc);
#endif
    ret = finish_reply(acc, acc_len, out, out_len);

tls_out:
    mbedtls_net_free(&server_fd);
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
buf_out:
    (void)fd;
    free(esc);
    free(escj);
    free(body);
    free(req);
    free(acc);
    return ret;
}

int llm_chat(const llm_gateway_t *gw, const char *message, char *out, int out_len)
{
    if (gw == NULL || gw->ip == NULL || gw->vhost == NULL ||
        gw->api_key == NULL || message == NULL || out == NULL || out_len <= 1) {
        return LLM_ERR_ARGS;
    }
    pthread_mutex_lock(&g_lock);
    int r = llm_chat_locked(gw, message, out, out_len);
    pthread_mutex_unlock(&g_lock);
    return r;
}

/* ============ 通道2: llm_ask — PC 中继(免 key) ============ */
static int llm_ask_locked(const char *msg, char *out, int out_len)
{
    int ret;
    int fd = -1;
    char *req = (char *)malloc(LLM_ASKREQ_SIZE);
    char *acc = (char *)malloc(LLM_ACC_SIZE);
    if (req == NULL || acc == NULL) {
        free(req);
        free(acc);
        return LLM_ERR_MEM;
    }

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        ret = LLM_ERR_CONNECT;
        goto ask_out;
    }
    struct timeval tv = { LLM_FD_TIMEOUT_SEC * LLM_RETRY_BUDGET, 0 };
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons((unsigned short)g_relay_port);
    sa.sin_addr.s_addr = inet_addr(g_relay_ip);
    if (connect(fd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
        ret = LLM_ERR_CONNECT;
        goto ask_out;
    }

    int mlen = (int)strlen(msg);
    if (mlen > LLM_MSG_MAX) {
        mlen = LLM_MSG_MAX;
    }
    int rl = snprintf(req, LLM_ASKREQ_SIZE,
                      "POST /say HTTP/1.0\r\nHost: %s\r\nContent-Type: text/plain\r\n"
                      "Content-Length: %d\r\n\r\n%.*s",
                      g_relay_ip, mlen, mlen, msg);
    if (rl <= 0 || rl >= LLM_ASKREQ_SIZE || send(fd, req, (size_t)rl, 0) < 0) {
        ret = LLM_ERR_SEND;
        goto ask_out;
    }

    int acc_len = 0;
    char buf[2048];
    int n;
    while ((n = recv(fd, buf, sizeof(buf) - 1, 0)) > 0) {
        int copy = n;
        if (acc_len + copy > LLM_ACC_SIZE - 1) {
            copy = LLM_ACC_SIZE - 1 - acc_len;
        }
        memcpy(acc + acc_len, buf, (size_t)copy);
        acc_len += copy;
    }
    acc[acc_len] = '\0';
    ret = finish_reply(acc, acc_len, out, out_len);

ask_out:
    if (fd >= 0) {
        close(fd);
    }
    free(req);
    free(acc);
    return ret;
}

int llm_ask(const char *msg, char *out, int out_len)
{
    if (msg == NULL || out == NULL || out_len <= 1) {
        return LLM_ERR_ARGS;
    }
    pthread_mutex_lock(&g_lock);
    int r = llm_ask_locked(msg, out, out_len);
    pthread_mutex_unlock(&g_lock);
    return r;
}
