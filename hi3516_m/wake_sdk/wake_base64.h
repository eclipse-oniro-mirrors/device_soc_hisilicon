/*
 * wake_base64.h - base64 编解码(WebSocket + 阿里云音频传输用)
 *
 * 上传:PCM PCM → base64 字符串塞进 JSON
 * 下行:云端 delta(base64) → PCM 播放
 */
#ifndef WAKE_BASE64_H
#define WAKE_BASE64_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * base64 编码:data(len 字节)→ out(以 '\0' 结尾)。
 * 返回 out 长度(不含 '\0')。out 至少能装 4*ceil(len/3)+1 字节。
 */
int wake_base64_encode(const unsigned char *data, int len, char *out, int out_cap);

/*
 * base64 解码:b64(以 '\0' 结尾)→ out(裸字节)。
 * 返回解码字节数。out 至少能装 3*strlen(b64)/4 字节。
 * 非法字符返回 -1。
 */
int wake_base64_decode(const char *b64, unsigned char *out, int out_cap);

#ifdef __cplusplus
}
#endif

#endif /* WAKE_BASE64_H */
