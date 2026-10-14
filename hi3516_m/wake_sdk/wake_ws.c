/*
 * wake_ws.c - WebSocket 客户端实现(wss:// 阿里云 Qwen-Audio 实时语音)
 *
 * 链路: socket(TCP) → mbedTLS(TLS1.2) → WebSocket 帧层
 *
 * TLS 部分: 参考 applications/llm_sdk/llm_client.c (同事已验证的模式):
 *   - mbedtls_net_connect 用 IP(板上 DNS 不可用)
 *   - MBEDTLS_SSL_VERIFY_NONE 跳过证书验证(不用打包 cacert.pem)
 *   - WANT_READ/WANT_WRITE 循环重试
 *
 * WebSocket 部分: RFC 6455 自写
 *   - 握手: 发 HTTP Upgrade, 收 101
 *   - 发送帧: 客户端必须 mask(payload XOR 4字节 maskkey)
 *   - 接收帧: 服务端不 mask
 *
 * 读路径用内部缓冲 g_rx_buf + select 非阻塞填充(tls_fill_rx),
 * 支持单线程下"边发边收"(步骤3/4 需要):
 *   - wake_ws_recv_text : 阻塞收一帧(30s 超时)
 *   - wake_ws_try_recv  : 非阻塞尝试收一帧(无数据返回 0)
 * 写路径 tls_write_all 阻塞发送(单线程下发送不需要非阻塞)。
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/socket.h>   /* setsockopt, SOL_SOCKET, SO_RCVTIMEO */

#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"

#include "wake_ws.h"
#include "wake_base64.h"

#define WS_RECV_TIMEOUT_SEC  30   /* 阻塞 recv 超时 */
#define WS_HANDSHAKE_BUDGET  50   /* 握手/读写重试预算 */

/* 全局连接上下文(单连接, 简化设计) */
static mbedtls_net_context  g_server_fd;
static mbedtls_ssl_context  g_ssl;
static mbedtls_ssl_config   g_conf;
static mbedtls_entropy_context g_entropy;
static mbedtls_ctr_drbg_context g_ctr_drbg;
static int g_connected = 0;

/* 发送互斥锁: 三线程(send_thread 上传音频 / recv_thread 回 PONG + response.cancel
 * / fc_worker 写 function_call_output) 都会 ws_send_frame → tls_write_all,
 * 交替写 TLS 会错帧。锁整个帧发送(含 maskkey 生成, ctr_drbg 非线程安全)。 */
static pthread_mutex_t g_send_mtx = PTHREAD_MUTEX_INITIALIZER;

/* 接收缓冲区: 累积 TLS 解密后的字节, 支持跨多次 ssl_read 拼完整帧。
 * 用 malloc(静态大数组在 LiteOS-A 会 ldso 段错位) */
#define WS_RX_BUF_SIZE  32768
static unsigned char *g_rx_buf = NULL;
static int g_rx_len = 0;

/* ================= TLS 层(参考 llm_client.c) ================= */

static int tls_connect(const char *ip, const char *host, int port)
{
    char portstr[8];
    int ret;

    mbedtls_net_init(&g_server_fd);
    mbedtls_entropy_init(&g_entropy);
    mbedtls_ctr_drbg_init(&g_ctr_drbg);
    mbedtls_ssl_init(&g_ssl);
    mbedtls_ssl_config_init(&g_conf);

    g_rx_buf = (unsigned char *)malloc(WS_RX_BUF_SIZE);
    if (g_rx_buf == NULL) {
        printf("[ws] malloc rx buf failed\n");
        return -1;
    }
    g_rx_len = 0;

    const char *pers = "wake_ws";
    if (mbedtls_ctr_drbg_seed(&g_ctr_drbg, mbedtls_entropy_func, &g_entropy,
                              (const unsigned char *)pers, strlen(pers)) != 0) {
        printf("[ws] ctr_drbg_seed failed\n");
        return -1;
    }
    if (mbedtls_ssl_config_defaults(&g_conf, MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_STREAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT) != 0) {
        printf("[ws] ssl_config_defaults failed\n");
        return -1;
    }
    /* 跳过证书验证(参考 llm_client.c:205, 板上不便打包 CA) */
    mbedtls_ssl_conf_authmode(&g_conf, MBEDTLS_SSL_VERIFY_NONE);
    mbedtls_ssl_conf_rng(&g_conf, mbedtls_ctr_drbg_random, &g_ctr_drbg);
    if (mbedtls_ssl_setup(&g_ssl, &g_conf) != 0) {
        printf("[ws] ssl_setup failed\n");
        return -1;
    }
    if (mbedtls_ssl_set_hostname(&g_ssl, host) != 0) {
        printf("[ws] set_hostname failed\n");
        return -1;
    }
    snprintf(portstr, sizeof(portstr), "%d", port);
    if (mbedtls_net_connect(&g_server_fd, ip, portstr, MBEDTLS_NET_PROTO_TCP) != 0) {
        printf("[ws] net_connect %s:%d failed\n", ip, port);
        return -1;
    }
    /* recv 超时(参考 llm_client.c:220) */
    struct timeval tv = { WS_RECV_TIMEOUT_SEC, 0 };
    setsockopt(g_server_fd.fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    mbedtls_ssl_set_bio(&g_ssl, &g_server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

    /* TLS 握手(WANT_READ/WANT_WRITE 重试, 参考 llm_client.c:226-236) */
    int budget = WS_HANDSHAKE_BUDGET;
    while ((ret = mbedtls_ssl_handshake(&g_ssl)) != 0) {
        if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
            if (--budget < 0) {
                printf("[ws] handshake timeout\n");
                return -1;
            }
            continue;
        }
        printf("[ws] handshake failed: -0x%x\n", -ret);
        return -1;
    }
    printf("[ws] TLS handshake OK (%s:%d)\n", ip, port);
    return 0;
}

/* TLS 阻塞写全部(处理 WANT_READ/WANT_WRITE) */
static int tls_write_all(const unsigned char *buf, int len)
{
    int sent = 0;
    int budget = WS_HANDSHAKE_BUDGET;
    while (sent < len) {
        int ret = mbedtls_ssl_write(&g_ssl, buf + sent, (size_t)(len - sent));
        if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
            if (--budget < 0) return -1;
            continue;
        }
        if (ret <= 0) return -1;
        sent += ret;
    }
    return sent;
}

/*
 * 非阻塞填充接收缓冲区。
 * timeout_ms=0 时不阻塞(select 0 超时); >0 时阻塞等待最多 timeout_ms。
 * 返回: >0 读到新字节(g_rx_len 增长), 0 = 无数据(超时), <0 = 错误/关闭。
 * 若缓冲区已有数据, 直接返回不读(避免覆盖未消费的字节)。
 */
static int tls_fill_rx(int timeout_ms)
{
    if (g_rx_buf == NULL) return -1;
    if (g_rx_len >= WS_RX_BUF_SIZE) return g_rx_len;  /* 满, 先消费 */

    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(g_server_fd.fd, &rfds);
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    int s = select(g_server_fd.fd + 1, &rfds, NULL, NULL, &tv);
    if (s < 0) {
        if (errno == EINTR) return 0;
        return -1;
    }
    if (s == 0) return 0;  /* 超时, 无数据 */

    int ret = mbedtls_ssl_read(&g_ssl, g_rx_buf + g_rx_len, WS_RX_BUF_SIZE - g_rx_len);
    if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
        return 0;  /* select 误报(部分 TLS 记录), 下次再试 */
    }
    if (ret <= 0) return -1;
    g_rx_len += ret;
    return ret;
}

/* 从缓冲区头部消费 n 字节 */
static void rx_consume(int n)
{
    if (n <= 0 || n > g_rx_len) return;
    memmove(g_rx_buf, g_rx_buf + n, g_rx_len - n);
    g_rx_len -= n;
}

/* ================= WebSocket 握手层 ================= */

static int ws_handshake(const char *host, const char *path, const char *api_key)
{
    /* 生成 16 字节随机 Sec-WebSocket-Key(base64 编码) */
    unsigned char key_raw[16];
    if (mbedtls_ctr_drbg_random(&g_ctr_drbg, key_raw, 16) != 0) {
        for (int i = 0; i < 16; i++) key_raw[i] = (unsigned char)(i * 7 + 13);
    }
    char key_b64[32];
    wake_base64_encode(key_raw, 16, key_b64, sizeof(key_b64));

    /* 构造 HTTP Upgrade 请求 */
    char *req = (char *)malloc(1024);
    if (req == NULL) return -1;
    int rl = snprintf(req, 1024,
        "GET %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: %s\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "Authorization: Bearer %s\r\n"
        "\r\n",
        path, host, key_b64, api_key);
    if (rl <= 0 || rl >= 1024) {
        free(req);
        return -1;
    }

    if (tls_write_all((unsigned char *)req, rl) < 0) {
        printf("[ws] send upgrade request failed\n");
        free(req);
        return -1;
    }
    free(req);

    /* 读 101 响应: 从 g_rx_buf 找 \r\n\r\n(头部结束)。
     * 剩余字节(session.created 帧等)留在 g_rx_buf 给帧解析器。 */
    int budget = WS_HANDSHAKE_BUDGET;
    int hdr_end = -1;
    for (;;) {
        for (int i = 0; i + 3 < g_rx_len; i++) {
            if (g_rx_buf[i] == '\r' && g_rx_buf[i+1] == '\n' &&
                g_rx_buf[i+2] == '\r' && g_rx_buf[i+3] == '\n') {
                hdr_end = i + 4;
                break;
            }
        }
        if (hdr_end >= 0) break;
        if (--budget < 0) {
            printf("[ws] handshake response timeout\n");
            return -1;
        }
        if (tls_fill_rx(2000) < 0) {
            printf("[ws] handshake recv failed\n");
            return -1;
        }
    }

    /* 检查 101 + Upgrade(复制出来 null-terminate 再 strstr) */
    char *hdr = (char *)malloc(hdr_end + 1);
    if (hdr == NULL) return -1;
    memcpy(hdr, g_rx_buf, hdr_end);
    hdr[hdr_end] = '\0';
    int ok = (strstr(hdr, "101") != NULL && strstr(hdr, "Upgrade") != NULL);
    if (!ok) {
        printf("[ws] upgrade failed, response:\n%.300s\n", hdr);
        free(hdr);
        return -1;
    }
    free(hdr);

    /* 消费头部, 剩余字节(若有)留给帧解析器 */
    rx_consume(hdr_end);
    printf("[ws] WebSocket upgrade OK\n");
    return 0;
}

/* ================= WebSocket 帧层(RFC 6455) ================= */

/*
 * 发送 WebSocket 帧(客户端必须 mask)。
 * opcode: WS_OPCODE_TEXT / WS_OPCODE_BINARY / WS_OPCODE_PONG / WS_OPCODE_CLOSE
 */
static int ws_send_frame(int opcode, const unsigned char *payload, int len)
{
    unsigned char hdr[14];
    int hlen = 0;
    hdr[0] = 0x80 | (opcode & 0x0f);  /* FIN=1 */

    unsigned char mask_bit = 0x80;
    if (len < 126) {
        hdr[1] = mask_bit | (unsigned char)len;
        hlen = 2;
    } else if (len < 65536) {
        hdr[1] = mask_bit | 126;
        hdr[2] = (len >> 8) & 0xff;
        hdr[3] = len & 0xff;
        hlen = 4;
    } else {
        hdr[1] = mask_bit | 127;
        memset(hdr + 2, 0, 4);
        hdr[6] = (len >> 24) & 0xff;
        hdr[7] = (len >> 16) & 0xff;
        hdr[8] = (len >> 8) & 0xff;
        hdr[9] = len & 0xff;
        hlen = 10;
    }

    /* 4 字节 mask key(随机) */
    unsigned char maskkey[4];
    if (mbedtls_ctr_drbg_random(&g_ctr_drbg, maskkey, 4) != 0) {
        maskkey[0] = 0x12; maskkey[1] = 0x34;
        maskkey[2] = 0x56; maskkey[3] = 0x78;
    }
    hdr[hlen++] = maskkey[0];
    hdr[hlen++] = maskkey[1];
    hdr[hlen++] = maskkey[2];
    hdr[hlen++] = maskkey[3];

    pthread_mutex_lock(&g_send_mtx);
    if (tls_write_all(hdr, hlen) < 0) { pthread_mutex_unlock(&g_send_mtx); return -1; }

    if (len > 0) {
        unsigned char *masked = (unsigned char *)malloc(len);
        if (masked == NULL) { pthread_mutex_unlock(&g_send_mtx); return -1; }
        for (int i = 0; i < len; i++) {
            masked[i] = payload[i] ^ maskkey[i & 3];
        }
        int ret = tls_write_all(masked, len);
        free(masked);
        if (ret < 0) { pthread_mutex_unlock(&g_send_mtx); return -1; }
    }
    pthread_mutex_unlock(&g_send_mtx);
    return 0;
}

/*
 * 接收一帧 WebSocket 数据(服务端不 mask)。
 * timeout_ms: 每次等待数据的超时。
 * 返回: >0 payload 长度, 0 = 超时无完整帧, <0 = 错误/关闭/收到 close。
 * out_opcode 输出帧类型。
 * 自动处理 ping(回 pong)/pong/close, 数据帧返回 payload。
 */
static int ws_recv_frame(unsigned char *out, int out_cap, int *out_opcode, int timeout_ms)
{
    for (;;) {
        /* 确保至少 2 字节帧头 */
        while (g_rx_len < 2) {
            if (tls_fill_rx(timeout_ms) <= 0) return 0;
        }
        int opcode = g_rx_buf[0] & 0x0f;
        int masked = (g_rx_buf[1] >> 7) & 1;
        int len = g_rx_buf[1] & 0x7f;
        int hdr = 2;

        if (len == 126) {
            while (g_rx_len < 4) {
                if (tls_fill_rx(timeout_ms) <= 0) return 0;
            }
            len = (g_rx_buf[2] << 8) | g_rx_buf[3];
            hdr = 4;
        } else if (len == 127) {
            while (g_rx_len < 10) {
                if (tls_fill_rx(timeout_ms) <= 0) return 0;
            }
            len = (int)(((unsigned)g_rx_buf[6] << 24) | ((unsigned)g_rx_buf[7] << 16) |
                        ((unsigned)g_rx_buf[8] << 8) | (unsigned)g_rx_buf[9]);
            hdr = 10;
        }

        int payload_off = hdr + (masked ? 4 : 0);
        int total = payload_off + len;

        /* 超大帧(>缓冲): 不能整帧缓冲, 改为分块 drain, 只拷前面能放的。
         * 服务端帧不 mask(masked=0), 无需 unmask。 */
        if (total > WS_RX_BUF_SIZE) {
            printf("[ws] frame %d > buf %d, draining\n", len, WS_RX_BUF_SIZE);
            rx_consume(payload_off);  /* 先消费头+maskkey */
            int remaining = len;
            int copied = 0;
            while (remaining > 0) {
                if (g_rx_len == 0) {
                    if (tls_fill_rx(timeout_ms) <= 0) {
                        /* 未收完就超时: 部分帧已 drain, 输出已拷部分 */
                        break;
                    }
                }
                int take = g_rx_len < remaining ? g_rx_len : remaining;
                if (copied < out_cap - 1) {
                    int cpy = take;
                    if (copied + cpy > out_cap - 1) cpy = out_cap - 1 - copied;
                    memcpy(out + copied, g_rx_buf, cpy);
                    copied += cpy;
                }
                rx_consume(take);
                remaining -= take;
            }
            out[copied] = '\0';
            if (out_opcode) *out_opcode = opcode;
            if (opcode == WS_OPCODE_CLOSE) return -1;
            if (opcode == WS_OPCODE_PING) {
                ws_send_frame(WS_OPCODE_PONG, out, copied);
                continue;
            }
            if (opcode == WS_OPCODE_PONG) continue;
            return copied;
        }

        /* 等待整帧到齐 */
        while (g_rx_len < total) {
            if (tls_fill_rx(timeout_ms) <= 0) return 0;
        }

        unsigned char maskkey[4] = {0};
        if (masked) {
            memcpy(maskkey, g_rx_buf + hdr, 4);
        }

        int copy = len;
        if (copy > out_cap - 1) {
            printf("[ws] frame too large %d > %d, truncated\n", len, out_cap - 1);
            copy = out_cap - 1;
        }
        memcpy(out, g_rx_buf + payload_off, copy);
        if (masked) {
            for (int i = 0; i < copy; i++) out[i] ^= maskkey[i & 3];
        }
        out[copy] = '\0';
        rx_consume(total);

        if (out_opcode) *out_opcode = opcode;

        /* 控制帧处理 */
        if (opcode == WS_OPCODE_CLOSE) {
            /* close 帧前 2 字节 = 关闭码(1000 正常 / 1002 协议错 / 1003 不支持类型 等),
             * 其后是可选的 UTF-8 reason(云端报错文本, 排障关键) */
            int code = -1;
            if (copy >= 2) code = (out[0] << 8) | out[1];
            printf("[ws] received close frame (code=%d, len=%d)\n", code, copy);
            if (copy > 2) {
                char reason[128];
                int rl = copy - 2;

                if (rl > (int)sizeof(reason) - 1) {
                    rl = (int)sizeof(reason) - 1;
                }
                memcpy(reason, out + 2, (size_t)rl);
                reason[rl] = '\0';
                printf("[ws] close reason: %s\n", reason);
            }
            return -1;
        }
        if (opcode == WS_OPCODE_PING) {
            ws_send_frame(WS_OPCODE_PONG, out, copy);
            continue;  /* 继续等数据帧 */
        }
        if (opcode == WS_OPCODE_PONG) {
            continue;
        }
        return copy;
    }
}

/* ================= 对外接口 ================= */

int wake_ws_connect(const char *ip, const char *host, int port,
                    const char *path, const char *api_key)
{
    if (g_connected) {
        printf("[ws] already connected\n");
        return 0;
    }
    printf("[ws] connecting %s:%d (host=%s)...\n", ip, port, host);
    if (tls_connect(ip, host, port) != 0) {
        return -1;
    }
    if (ws_handshake(host, path, api_key) != 0) {
        mbedtls_net_free(&g_server_fd);
        mbedtls_ssl_free(&g_ssl);
        mbedtls_ssl_config_free(&g_conf);
        mbedtls_ctr_drbg_free(&g_ctr_drbg);
        mbedtls_entropy_free(&g_entropy);
        free(g_rx_buf);
        g_rx_buf = NULL;
        return -1;
    }
    g_connected = 1;
    return 0;
}

int wake_ws_send_text(const char *data, int len)
{
    if (!g_connected) return -1;
    return ws_send_frame(WS_OPCODE_TEXT, (const unsigned char *)data, len);
}

/* 阻塞收一帧(30s 超时)。返回长度, <0 失败/关闭。 */
int wake_ws_recv_text(char *out, int out_cap)
{
    if (!g_connected) return -1;
    int budget = 60;  /* 最多 ~60 次 500ms 等待 = 30s */
    for (;;) {
        int opcode = 0;
        int n = ws_recv_frame((unsigned char *)out, out_cap, &opcode, 500);
        if (n < 0) return -1;
        if (n == 0) {
            if (--budget < 0) return -1;
            continue;
        }
        return n;
    }
}

/* 非阻塞尝试收一帧(无数据立即返回 0)。
 * 用于单线程"边发边收": 发完一帧音频后调它 drain 服务端事件。
 * 返回: >0 收到一帧, 0 = 无数据, <0 = 失败/关闭。 */
int wake_ws_try_recv(char *out, int out_cap)
{
    if (!g_connected) return -1;
    int opcode = 0;
    int n = ws_recv_frame((unsigned char *)out, out_cap, &opcode, 0);
    if (n < 0) return -1;
    return n;  /* 0 或 >0 */
}

void wake_ws_close(void)
{
    if (!g_connected) return;
    unsigned char close_payload[2] = {0x03, 0xe8};  /* 1000 normal closure */
    ws_send_frame(WS_OPCODE_CLOSE, close_payload, 2);
    mbedtls_ssl_close_notify(&g_ssl);
    mbedtls_net_free(&g_server_fd);
    mbedtls_ssl_free(&g_ssl);
    mbedtls_ssl_config_free(&g_conf);
    mbedtls_ctr_drbg_free(&g_ctr_drbg);
    mbedtls_entropy_free(&g_entropy);
    free(g_rx_buf);
    g_rx_buf = NULL;
    g_rx_len = 0;
    g_connected = 0;
    printf("[ws] closed\n");
}
