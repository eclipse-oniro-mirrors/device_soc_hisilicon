/*
 * wake_main.cc - 主程序
 *
 * 用法(串口 OHOS shell):
 *   exec /bin/my_wake              默认:语音唤醒监听(说 "Luna" 唤醒)
 *   exec /bin/my_wake duplex       步骤 1:AI(16kHz 采集) + AO(24kHz 播放) 双工验证
 *   exec /bin/my_wake wstest       步骤 2:WebSocket 连接验证
 *   exec /bin/my_wake sendtest     步骤 3:发 PCM 收转写验证
 *   exec /bin/my_wake dialog [-24] 步骤 4:端到端实时对话(可选 energy gate dBFS, 默认 -26)
 *
 * 步骤 1 验证(duplex):
 *   - WakeCaptureInit:16kHz 采集
 *   - WakePlaybackInit:24kHz 播放(重采样到 16kHz 硬件播放)
 *   - 主循环:边采集打印 mic 电平,边播放 24kHz 正弦波(10 秒)
 *   - 验证:喇叭出正弦波 + mic 电平正常(证明 AI+AO 双向同时工作)
 *
 * 步骤 2 验证(wstest):
 *   - wifi_connect_ap + wifi_wait_connected:连 WiFi(同事库)
 *   - wake_ws_connect:TCP→TLS→WebSocket 升级握手到阿里云 Qwen-Audio
 *   - 收 3 帧服务端事件(期待 session.created 等),打印后关闭
 *
 * 步骤 3 验证(sendtest):
 *   - 连 WiFi + WS,发 session.update(开 server_vad)
 *   - 采集 16kHz PCM,每帧(960B/30ms)base64 编码发 input_audio_buffer.append
 *   - 边发边收(wake_ws_try_recv 非阻塞 drain),打印转写/事件
 *   - 停采后继续 drain 10s,期待 conversation.item.input_audio_transcription.completed
 *     (你说的转写) 和 response.audio_transcript.done (AI 回复转写)
 *
 * 默认(唤醒监听):
 *   - WakeCaptureInit + wake_word_init
 *   - 循环:取帧 → wake_word_process → 打印 score → 检测到打印 DETECTED
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

#include "wake_capture.h"
#include "wake_playback.h"
#include "wake_word.h"
#include "wake_ws.h"
#include "wake_base64.h"
#include "wake_dialog.h"
#include "wifi_client.h"

#define WIFI_SSID "123456"
#define WIFI_PSK  "20010504"

/* 阿里云 DashScope API key(写死, 来自 默认业务空间-apiKey-7372424.csv) */
#define QWEN_API_KEY "sk-ws-H.PIPHLHL.jG0u.MEUCIQDHrsS_H86Sa7dOmMlBWpR1gcd3baedf7WO6wZcwI5r0QIgI9XxbFBm9ki7f3g7cv2Kx4FuFqWFshwbc2uf3fj3vxk"

/* ================= 步骤 1:双工验证 ================= */

#define TONE_HZ          440      /* A4 音符,容易辨认 */
#define TONE_SECONDS     10       /* 播放 10 秒 */
#define DUPLEX_LOG_MS    1000
#define DUPLEX_FRAMES_PER_STATUS (DUPLEX_LOG_MS / 30)

static float block_rms_dbfs(const short *samples, int count)
{
    double sum = 0.0;
    for (int i = 0; i < count; i++) {
        double v = (double)samples[i] / 32768.0;
        sum += v * v;
    }
    double rms = sqrt(sum / count);
    return rms > 1e-9 ? (float)(20.0 * log10(rms)) : -120.0f;
}

/* 24kHz 正弦波生成 */
static int GenTone24k(short **outBuf, int *outSamples)
{
    int samples = PLAY_SAMPLE_RATE * TONE_SECONDS;
    short *buf = (short *)malloc(samples * sizeof(short));
    if (buf == NULL) {
        printf("[my_wake] tone: malloc failed\n");
        return -1;
    }
    for (int i = 0; i < samples; i++) {
        double t = (double)i / PLAY_SAMPLE_RATE;
        buf[i] = (short)(0.3 * 32767.0 * sin(2.0 * M_PI * TONE_HZ * t));
    }
    *outBuf = buf;
    *outSamples = samples;
    printf("[my_wake] tone: %dHz, %ds, %d samples (%d bytes) @24kHz\n",
           TONE_HZ, TONE_SECONDS, samples, samples * 2);
    return 0;
}

static int RunDuplexTest(void)
{
    printf("[my_wake] === step 1: AI(16k) + AO(24k) duplex verify ===\n");

    printf("[my_wake] [1/3] capture init...\n");
    if (WakeCaptureInit() != 0) {
        printf("[my_wake] capture init FAILED\n");
        return 1;
    }
    printf("[my_wake] [1/3] capture init OK\n");

    printf("[my_wake] [2/3] playback init...\n");
    if (WakePlaybackInit() != 0) {
        printf("[my_wake] playback init FAILED\n");
        WakeCaptureDeinit();
        return 1;
    }
    printf("[my_wake] [2/3] playback init OK\n");

    printf("[my_wake] [3/3] gen tone...\n");
    short *toneBuf = NULL;
    int toneSamples = 0;
    if (GenTone24k(&toneBuf, &toneSamples) != 0) {
        WakePlaybackDeinit();
        WakeCaptureDeinit();
        return 1;
    }

    printf("[my_wake] duplex: playing %dHz tone + capturing mic (10s)...\n", TONE_HZ);
    printf("[my_wake] expect: speaker plays tone, log shows mic level\n");

    short capBuf[WAKE_PT_PER_FRM];
    int frameCount = 0;
    int toneSent = 0;
    int toneBytes = toneSamples * 2;
    int chunkBytes = PLAY_IN_FRAME_BYTES;
    int miss = 0;

    while (toneSent < toneBytes || frameCount < TONE_SECONDS * 33) {
        /* 送一帧播放数据(24kHz 侧),阻塞 ~30ms */
        if (toneSent < toneBytes) {
            unsigned char padBuf[PLAY_IN_FRAME_BYTES];
            memset(padBuf, 0, PLAY_IN_FRAME_BYTES);
            int remain = toneBytes - toneSent;
            int send = remain < chunkBytes ? remain : chunkBytes;
            memcpy(padBuf, (unsigned char *)toneBuf + toneSent, send);
            (void)WakePlaybackWrite(padBuf, chunkBytes);
            toneSent += send;
        } else {
            unsigned char silence[PLAY_IN_FRAME_BYTES];
            memset(silence, 0, sizeof(silence));
            (void)WakePlaybackWrite(silence, chunkBytes);
        }

        /* 读一帧采集数据,打印 mic 电平 */
        if (WakeCaptureRead(capBuf, WAKE_PT_PER_FRM) != 0) {
            if (++miss < 10) {
                continue;
            }
        }

        if (frameCount % DUPLEX_FRAMES_PER_STATUS == 0) {
            float level = block_rms_dbfs(capBuf, WAKE_PT_PER_FRM);
            short peak = 0;
            for (int i = 0; i < WAKE_PT_PER_FRM; i++) {
                short a = capBuf[i] < 0 ? (short)-capBuf[i] : capBuf[i];
                if (a > peak) peak = a;
            }
            int playSec = toneSent / (PLAY_SAMPLE_RATE * 2);
            printf("[my_wake] mic %.1f dBFS, peak %d | tone sent %d/%ds\n",
                   level, peak, playSec, TONE_SECONDS);
        }
        frameCount++;
    }

    printf("[my_wake] step 1 done\n");
    free(toneBuf);
    WakePlaybackDeinit();
    WakeCaptureDeinit();
    return 0;
}

/* ================= 默认:唤醒监听 ================= */

#define WAKE_STATUS_LOG_MS  1000
#define WAKE_FRAMES_PER_STATUS (WAKE_STATUS_LOG_MS / 30)
#define WAKE_STEPS_PER_READ  3

static int RunWakeListen(void)
{
    printf("[my_wake] === wake word listen ===\n");

    printf("[my_wake] [1/2] capture init...\n");
    if (WakeCaptureInit() != 0) {
        printf("[my_wake] capture init FAILED\n");
        return 1;
    }
    printf("[my_wake] [1/2] capture init OK\n");

    printf("[my_wake] [2/2] wake_word init...\n");
    int ret = wake_word_init();
    if (ret != 0) {
        printf("[my_wake] wake_word init FAILED (ret=%d)\n", ret);
        return 1;
    }
    printf("[my_wake] [2/2] wake_word init OK\n");

    printf("[my_wake] listening (1s log)...\n");

    short buf[WAKE_PT_PER_FRM];
    int frameCount = 0;

    for (;;) {
        if (WakeCaptureRead(buf, WAKE_PT_PER_FRM) != 0) {
            printf("[my_wake] read failed, retry\n");
            continue;
        }

        bool detected = false;
        for (int step = 0; step < WAKE_STEPS_PER_READ; step++) {
            if (wake_word_process(buf + step * WAKE_WORD_SAMPLES_PER_STEP)) {
                detected = true;
            }
        }

        if (frameCount % WAKE_FRAMES_PER_STATUS == 0) {
            float level = block_rms_dbfs(buf, WAKE_PT_PER_FRM);
            float score = wake_word_last_probability();
            short peak = 0;
            for (int i = 0; i < WAKE_PT_PER_FRM; i++) {
                short a = buf[i] < 0 ? (short)-buf[i] : buf[i];
                if (a > peak) peak = a;
            }
            printf("[my_wake] mic %.1f dBFS, peak %d, score %.3f\n", level, peak, score);
        }

        if (detected) {
            printf("[my_wake] *** DETECTED! score %.3f ***\n", wake_word_last_probability());
            wake_word_reset();
        }
        frameCount++;
    }

    WakeCaptureDeinit();
    return 0;
}

/* ================= 步骤 2: WebSocket 连接验证 ================= */

#define WS_RECV_BUF  8192

static int RunWsTest(void)
{
    printf("[my_wake] === step 2: wss connect verify ===\n");

    printf("[my_wake] [1/3] wifi connect...\n");
    if (wifi_connect_ap(WIFI_SSID, WIFI_PSK) != 0) {
        printf("[my_wake] wifi_connect_ap FAILED\n");
        return 1;
    }
    char ip[16] = {0};
    if (wifi_wait_connected(20, ip, sizeof(ip)) != 0) {
        printf("[my_wake] wifi_wait_connected FAILED\n");
        return 1;
    }
    printf("[my_wake] [1/3] wifi connected, ip=%s\n", ip);

    printf("[my_wake] [2/3] ws connect %s:%d...\n", QWEN_WS_IP, QWEN_WS_PORT);
    if (wake_ws_connect(QWEN_WS_IP, QWEN_WS_HOST, QWEN_WS_PORT,
                        QWEN_WS_PATH, QWEN_API_KEY) != 0) {
        printf("[my_wake] ws connect FAILED\n");
        return 1;
    }
    printf("[my_wake] [2/3] ws connected\n");

    printf("[my_wake] [3/3] waiting for session.created...\n");
    char *buf = (char *)malloc(WS_RECV_BUF);
    if (buf == NULL) {
        printf("[my_wake] malloc FAILED\n");
        wake_ws_close();
        return 1;
    }

    /* 收 3 帧服务端事件, 打印(期待 session.created 等) */
    for (int i = 0; i < 3; i++) {
        int n = wake_ws_recv_text(buf, WS_RECV_BUF);
        if (n < 0) {
            printf("[my_wake] recv frame %d FAILED\n", i);
            break;
        }
        printf("[my_wake] [recv %d] (%d bytes):\n%s\n", i, n, buf);
    }

    free(buf);
    wake_ws_close();
    printf("[my_wake] step 2 done\n");
    return 0;
}

/* ================= 步骤 3: 单向上传验证(发 PCM, 收转写) ================= */

#define SEND_TEST_SECONDS  15      /* 采集并发送的时长 */
#define SEND_DRAIN_MS      10000   /* 停采后继续收事件的时间 */
#define SEND_RECV_CAP      16384   /* 事件接收缓冲(音频 delta 可能较大) */
#define SEND_B64_CAP       2200    /* 960B PCM → 1280 b64, 留余量 */
#define SEND_JSON_CAP      2400

/* session.update 配置(对齐 realtime_quickstart.py, 开启 server_vad) */
static const char *SESSION_UPDATE_JSON =
    "{\"type\":\"session.update\",\"session\":{\"modalities\":[\"text\",\"audio\"],"
    "\"voice\":\"longanqian_v3.1\",\"turn_detection\":{\"type\":\"server_vad\","
    "\"threshold\":0.5,\"silence_duration_ms\":800}}}";

/* 非阻塞 drain 服务端事件并打印(音频 delta 截断显示) */
static void DrainAndPrintEvents(char *buf, int cap)
{
    for (;;) {
        int n = wake_ws_try_recv(buf, cap);
        if (n <= 0) return;  /* 0=无数据, <0=关闭(后续 send 会失败) */
        if (n > 200) {
            buf[200] = '\0';
            printf("[recv] %s...(%d bytes)\n", buf, n);
        } else {
            printf("[recv] %s\n", buf);
        }
    }
}

static int RunSendTest(void)
{
    printf("[my_wake] === step 3: send PCM + recv transcription ===\n");

    char ip[16] = {0};
    printf("[my_wake] [1/4] wifi connect...\n");
    if (wifi_connect_ap(WIFI_SSID, WIFI_PSK) != 0) {
        printf("[my_wake] wifi_connect_ap FAILED\n");
        return 1;
    }
    if (wifi_wait_connected(20, ip, sizeof(ip)) != 0) {
        printf("[my_wake] wifi_wait_connected FAILED\n");
        return 1;
    }
    printf("[my_wake] [1/4] wifi connected, ip=%s\n", ip);

    printf("[my_wake] [2/4] ws connect...\n");
    if (wake_ws_connect(QWEN_WS_IP, QWEN_WS_HOST, QWEN_WS_PORT,
                        QWEN_WS_PATH, QWEN_API_KEY) != 0) {
        printf("[my_wake] ws connect FAILED\n");
        return 1;
    }
    printf("[my_wake] [2/4] ws connected\n");

    char *buf = (char *)malloc(SEND_RECV_CAP);
    char *b64 = (char *)malloc(SEND_B64_CAP);
    char *json = (char *)malloc(SEND_JSON_CAP);
    if (buf == NULL || b64 == NULL || json == NULL) {
        printf("[my_wake] malloc FAILED\n");
        free(buf); free(b64); free(json);
        wake_ws_close();
        return 1;
    }

    /* drain session.created */
    DrainAndPrintEvents(buf, SEND_RECV_CAP);

    /* 发 session.update */
    if (wake_ws_send_text(SESSION_UPDATE_JSON, strlen(SESSION_UPDATE_JSON)) != 0) {
        printf("[my_wake] send session.update FAILED\n");
        free(buf); free(b64); free(json);
        wake_ws_close();
        return 1;
    }
    printf("[my_wake] session.update sent\n");
    DrainAndPrintEvents(buf, SEND_RECV_CAP);

    printf("[my_wake] [3/4] capture init...\n");
    if (WakeCaptureInit() != 0) {
        printf("[my_wake] capture init FAILED\n");
        free(buf); free(b64); free(json);
        wake_ws_close();
        return 1;
    }
    printf("[my_wake] [3/4] capture OK, sending %ds of audio...\n", SEND_TEST_SECONDS);
    printf("[my_wake] >>> speak into the mic (15s) <<<\n");

    short capBuf[WAKE_PT_PER_FRM];
    int framesPerSec = 1000 / 30;  /* ~33 帧/秒 */
    int totalFrames = SEND_TEST_SECONDS * framesPerSec;
    int sendFail = 0;

    for (int f = 0; f < totalFrames; f++) {
        /* 边发边收: 每帧先 drain 服务端事件 */
        DrainAndPrintEvents(buf, SEND_RECV_CAP);

        if (WakeCaptureRead(capBuf, WAKE_PT_PER_FRM) != 0) {
            printf("[my_wake] capture read fail\n");
            continue;
        }

        /* base64 编码 960B PCM → b64 */
        int bl = wake_base64_encode((const unsigned char *)capBuf, WAKE_FRAME_BYTES,
                                    b64, SEND_B64_CAP);
        if (bl <= 0) {
            continue;
        }
        /* 拼 input_audio_buffer.append 事件 */
        int jl = snprintf(json, SEND_JSON_CAP,
            "{\"type\":\"input_audio_buffer.append\",\"audio\":\"%s\"}", b64);
        if (jl <= 0 || jl >= SEND_JSON_CAP) {
            continue;
        }
        if (wake_ws_send_text(json, jl) != 0) {
            printf("[my_wake] send append FAILED, conn closed?\n");
            sendFail = 1;
            break;
        }

        if (f % framesPerSec == 0) {
            printf("[my_wake] sent %ds/%d\n", f / framesPerSec, SEND_TEST_SECONDS);
        }
    }

    WakeCaptureDeinit();
    printf("[my_wake] [4/4] capture stopped, draining response %ds...\n",
           SEND_DRAIN_MS / 1000);

    /* 停采后继续收事件(转写/回复在 VAD 检测到静音后才来) */
    int drainIters = SEND_DRAIN_MS / 100;
    for (int i = 0; i < drainIters; i++) {
        DrainAndPrintEvents(buf, SEND_RECV_CAP);
        usleep(100 * 1000);  /* 100ms */
    }

    free(buf); free(b64); free(json);
    wake_ws_close();
    if (sendFail) {
        printf("[my_wake] step 3 aborted (send failed)\n");
        return 1;
    }
    printf("[my_wake] step 3 done\n");
    return 0;
}

/* ================= main ================= */

/*
 * 解析对话麦克风增益(0~0x7e)。优先级: 命令行第 arg_idx 参数 > 环境变量
 * WAKE_DIALOG_MIC_VOL > 唤醒校准默认值(WakeCaptureDefaultMicVol)。
 * 用法: flow/dialog 的第 3 个参数, 如 "flow -18 40" 表示 gate=-18dBFS, mic_vol=40。
 * 不传或非法则用默认(60), 不改变行为; 用户可调低(如 30~40)减小噪声底噪防自言自语。
 */
static unsigned int resolve_mic_vol(int argc, char **argv, int arg_idx)
{
    long v = -1;
    if (argc > arg_idx) {
        v = strtol(argv[arg_idx], NULL, 0);
    } else {
        const char *env = getenv("WAKE_DIALOG_MIC_VOL");
        if (env != NULL) {
            v = strtol(env, NULL, 0);
        }
    }
    if (v >= 0 && v <= 0x7e) {
        return (unsigned int)v;
    }
    return WakeCaptureDefaultMicVol();
}

int main(int argc, char **argv)
{
    /* 子命令(API key 已写死, 无需命令行参数):
     *   duplex         步骤 1:AI+AO 双工验证
     *   wstest         步骤 2:WebSocket 连接验证
     *   sendtest       步骤 3:发 PCM 收转写验证
     *   dialog [-18] [micvol]   步骤 4:端到端实时对话(可选 gate dBFS + 麦克风增益)
     *   flow [-18] [micvol]    唤醒→对话主循环(监听 Luna → 对话 → 空闲 30s 回唤醒)
     *                          micvol: 麦克风 PGA 增益(0~126), 默认 60; 调低(如 30~40)
     *                                  减小噪声底噪, 防云端 VAD 误判噪声致自言自语
     *   无参数         语音唤醒监听(只检测不对话)
     */
    if (argc >= 2 && strcmp(argv[1], "duplex") == 0) {
        return RunDuplexTest();
    }
    if (argc >= 2 && strcmp(argv[1], "wstest") == 0) {
        return RunWsTest();
    }
    if (argc >= 2 && strcmp(argv[1], "sendtest") == 0) {
        return RunSendTest();
    }
    if (argc >= 2 && strcmp(argv[1], "dialog") == 0) {
        /* 可选参数: [gate dBFS] [micvol], 如 "dialog -18 40"; 不带用默认 */
        float gate = ENERGY_GATE_DBFS_DEFAULT;
        if (argc >= 3) {
            gate = strtof(argv[2], NULL);
        }
        unsigned int micvol = resolve_mic_vol(argc, argv, 3);
        return RunDialog(gate, micvol);
    }
    if (argc >= 2 && strcmp(argv[1], "flow") == 0) {
        /* 唤醒→对话主循环: 监听 Luna → 对话(空闲 30s 回唤醒)→ 循环
         * 可选参数: [gate dBFS] [micvol], 如 "flow -18 40"; 不带用默认 */
        float gate = ENERGY_GATE_DBFS_DEFAULT;
        if (argc >= 3) {
            gate = strtof(argv[2], NULL);
        }
        unsigned int micvol = resolve_mic_vol(argc, argv, 3);
        return RunWakeFlow(gate, micvol);
    }
    return RunWakeListen();
}
