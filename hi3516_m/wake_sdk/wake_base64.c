/*
 * wake_base64.c - base64 编解码实现(RFC 4648 标准)
 */
#include <stddef.h>
#include "wake_base64.h"

static const char enc_tab[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* 解码表:合法字符 → 0-63,非法 → -1(等号 '=' 单独处理) */
static int dec_val(char c)
{
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
}

int wake_base64_encode(const unsigned char *data, int len, char *out, int out_cap)
{
    if (data == NULL || out == NULL || len < 0 || out_cap <= 0) {
        return -1;
    }
    int o = 0;
    int i = 0;
    /* 每次处理 3 字节 → 4 字符 */
    while (i + 3 <= len) {
        if (o + 4 >= out_cap) return -1;  /* +1 给 '\0' */
        unsigned v = ((unsigned)data[i] << 16) | ((unsigned)data[i+1] << 8) | data[i+2];
        out[o++] = enc_tab[(v >> 18) & 0x3f];
        out[o++] = enc_tab[(v >> 12) & 0x3f];
        out[o++] = enc_tab[(v >> 6) & 0x3f];
        out[o++] = enc_tab[v & 0x3f];
        i += 3;
    }
    /* 处理尾部 1 或 2 字节 */
    int rem = len - i;
    if (rem == 1) {
        if (o + 4 >= out_cap) return -1;
        unsigned v = (unsigned)data[i] << 16;
        out[o++] = enc_tab[(v >> 18) & 0x3f];
        out[o++] = enc_tab[(v >> 12) & 0x3f];
        out[o++] = '=';
        out[o++] = '=';
    } else if (rem == 2) {
        if (o + 4 >= out_cap) return -1;
        unsigned v = ((unsigned)data[i] << 16) | ((unsigned)data[i+1] << 8);
        out[o++] = enc_tab[(v >> 18) & 0x3f];
        out[o++] = enc_tab[(v >> 12) & 0x3f];
        out[o++] = enc_tab[(v >> 6) & 0x3f];
        out[o++] = '=';
    }
    out[o] = '\0';
    return o;
}

int wake_base64_decode(const char *b64, unsigned char *out, int out_cap)
{
    if (b64 == NULL || out == NULL || out_cap <= 0) {
        return -1;
    }
    int o = 0;
    int v = 0;
    int bits = 0;
    int pad = 0;
    for (const char *p = b64; *p != '\0'; p++) {
        char c = *p;
        if (c == '=') {
            pad++;
            continue;
        }
        if (c == '\n' || c == '\r' || c == ' ' || c == '\t') {
            continue;  /* 跳过空白 */
        }
        int d = dec_val(c);
        if (d < 0) return -1;
        v = (v << 6) | d;
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            if (o >= out_cap) return -1;
            out[o++] = (unsigned char)((v >> bits) & 0xff);
        }
    }
    return o;
}
