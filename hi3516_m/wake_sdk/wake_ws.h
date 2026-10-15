/*
 * wake_ws.h - WebSocket 客户端(wss:// 阿里云 Qwen-Audio 实时语音)
 *
 * 链路: socket(TCP) → mbedTLS(TLS1.2) → WebSocket 帧层
 *   - 板上 DNS 不可用, mbedtls_net_connect 用预解析 IP
 *   - 证书验证跳过(MBEDTLS_SSL_VERIFY_NONE), 不用打包 cacert.pem
 *   - WebSocket: 客户端发送 mask, 服务端发送不 mask
 *
 * 参考实现: applications/llm_sdk/llm_client.c (TLS 握手模式)
 * WebSocket 帧层: RFC 6455 自写
 */
#ifndef WAKE_WS_H
#define WAKE_WS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 阿里云 Qwen-Audio 连接参数(板上 DNS 不可用, IP 预解析) */
#define QWEN_WS_IP       "47.94.20.201"
#define QWEN_WS_HOST     "ws-qq7tg8jxbjmnqaty.cn-beijing.maas.aliyuncs.com"
#define QWEN_WS_PORT     443
#define QWEN_WS_PATH     "/api-ws/v1/realtime?model=qwen-audio-3.1-realtime-plus"

/* WebSocket 帧类型(RFC 6455 opcode) */
#define WS_OPCODE_TEXT    0x1
#define WS_OPCODE_BINARY  0x2
#define WS_OPCODE_CLOSE   0x8
#define WS_OPCODE_PING    0x9
#define WS_OPCODE_PONG    0xA

/*
 * 建立 wss:// 连接: TCP → TLS 握手 → WebSocket 升级握手。
 * api_key: DASHSCOPE_API_KEY(Bearer 认证用)
 * 成功返回 0, 失败返回 -1。
 */
int wake_ws_connect(const char *ip, const char *host, int port,
                    const char *path, const char *api_key);

/*
 * 发送文本帧(JSON 事件)。data 为 UTF-8 JSON 字符串(不含 '\0')。
 * 成功返回 0, 失败返回 -1。
 */
int wake_ws_send_text(const char *data, int len);

/*
 * 接收一帧文本。out 收文本内容, 返回长度; <0 失败/连接关闭。
 * 阻塞直到收到一帧(30s 超时)。out_cap 为 out 缓冲区大小。
 */
int wake_ws_recv_text(char *out, int out_cap);

/*
 * 非阻塞尝试收一帧。用于单线程"边发边收": 发完音频后 drain 服务端事件。
 * 返回: >0 收到一帧(len), 0 = 无数据, <0 失败/连接关闭。
 */
int wake_ws_try_recv(char *out, int out_cap);

/*
 * 关闭 WebSocket + TLS + socket。
 */
void wake_ws_close(void);

#ifdef __cplusplus
}
#endif

#endif /* WAKE_WS_H */
