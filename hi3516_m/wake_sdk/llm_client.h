/* llm_client.h — 镜像内模块共用的 LLM 问答客户端库
 * 两种用法:
 *   1) llm_chat(): 客户自带网关+key — 板上直连 TLS 1.2, 不依赖 PC 中继
 *   2) llm_ask():  便捷接口 — 走 PC 中继(默认 192.168.1.1:8443 /say), 免 key
 * 链接: -lllmclient (动态) 或 libllmclient.a (静态, llm_chat 需另 -lmbedtls)
 */
#ifndef LLM_CLIENT_H
#define LLM_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

/* 错误码 */
#define LLM_ERR_CONNECT   (-1)   /* 连不上网关 */
#define LLM_ERR_SEND      (-2)   /* 发送失败 */
#define LLM_ERR_NORESP    (-3)   /* 无响应/超时(内部 90s) */
#define LLM_ERR_STATUS    (-4)   /* 网关返回非 200 */
#define LLM_ERR_TOOSMALL  (-5)   /* 输出缓冲区不够(out 内为截断内容) */
#define LLM_ERR_ARGS      (-6)   /* 参数非法 */
#define LLM_ERR_TLS       (-7)   /* TLS 握手失败 */
#define LLM_ERR_MEM       (-8)   /* 内存分配失败 */

/* 单条消息上限(字节): 超长部分发送前会被截断, 调用方应自查并告警 */
#define LLM_MSG_MAX       4096

#ifndef LLM_GATEWAY_T
#define LLM_GATEWAY_T
typedef struct {
    const char *ip;       /* 网关 IP(连接用, 必填) — 板上 DNS 解析不可用, 只能传 IP */
    const char *vhost;    /* 网关域名(Host 头/SNI 用, 必填, 可与 ip 相同) */
    int port;             /* 端口, 0 = 默认 443 */
    const char *api_key;  /* 客户的 Bearer key(必填) */
    const char *model;    /* 模型名, NULL = "deepseek-ai/DeepSeek-V4-Flash" */
} llm_gateway_t;
#endif

/* 客户主接口: 自带网关+key, 直连 TLS 问答
 * 链路: 板 -> 网关 IP:443 (TLS1.2, SNI/Host=vhost, Bearer=api_key)
 *       -> POST /v1/chat/completions {"model":..,"messages":[{"role":"user","content":msg}]}
 * message : 用户消息(UTF-8, 上限 LLM_MSG_MAX=4KB, 超长截断)
 * out      : 回复缓冲区(正文已从 JSON 提取, 建议 >= 4096)
 * 返回     : >=0 回复长度; 负数见 LLM_ERR_*
 */
int llm_chat(const llm_gateway_t *gw, const char *message, char *out, int out_len);

/* 便捷接口: 走 PC 中继(免 key, 有线网即可)
 * llm_set_server 可换中继地址(默认 192.168.1.1:8443)
 */
int llm_ask(const char *msg, char *out, int out_len);
int llm_set_server(const char *ip, int port);

#ifdef __cplusplus
}
#endif
#endif /* LLM_CLIENT_H */
