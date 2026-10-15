/*
 * wake_dialog.c - 端到端实时语音对话实现(步骤 4)
 *
 * 三线程状态机, 镜像 realtime_quickstart.py 的三个 asyncio task:
 *   - send 线程: WakeCaptureRead → base64 → 发 input_audio_buffer.append
 *   - recv 线程: wake_ws_recv_text → 解析 JSON → 分发(播放/打断/转写打印)
 *   - play 线程: 出播放队列 → WakePlaybackWrite
 *
 * 回声抑制(energy gate):
 *   AI 播放期间(is_playing), 麦克风能量低于阈值的不上传(回声比直接人声轻)。
 *   speech_started 触发打断: 清播放队列 + audio_suppressed + response.cancel。
 *
 * 无 cJSON: 手写 json_get_str 提取 type/delta/transcript 字段(结构固定)。
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

#include "wake_dialog.h"
#include "wake_capture.h"
#include "wake_playback.h"
#include "wake_ws.h"
#include "wake_base64.h"
#include "wifi_client.h"
#include "agent_bridge.h"
#include "wake_word.h"

/* WiFi 凭据 + API key(与 wake_main.cc 一致, 写死; 避免跨文件 C/C++ 链接问题) */
#define DIALOG_WIFI_SSID  "123456"
#define DIALOG_WIFI_PSK   "20010504"
#define DIALOG_API_KEY    "sk-ws-H.PIPHLHL.jG0u.MEUCIQDHrsS_H86Sa7dOmMlBWpR1gcd3baedf7WO6wZcwI5r0QIgI9XxbFBm9ki7f3g7cv2Kx4FuFqWFshwbc2uf3fj3vxk"

/* session.update 配置(server_vad + Function Calling tools)
 * - instructions: 话路区分守门员(动作/表情意图才调 control_robot, 闲聊不调)
 *   + reply 原样说(control_robot 返回的 reply 字段是 deepseek 生成的回复, 原样说出)
 * - tools: 注册 control_robot, 音频模型识别到肢体动作意图时自主调用
 * 官方协议: 模型发 function_call → 客户端执行(发 deepseek) → 写回 output → 发 response.create 二轮 */
static const char *SESSION_UPDATE_JSON =
    "{\"type\":\"session.update\",\"session\":{"
    "\"modalities\":[\"text\",\"audio\"],"
    "\"voice\":\"longanqian_v3.1\","
    "\"instructions\":\"你是装在机器人上的语音助手Luna。用户说话时先判断意图:若用户明确要求做肢体动作或切换表情(如跳舞/挥手/点头/走/转身/坐/换个表情/开心点等),调用control_robot工具(action填一个粗略动作名即可,精确动作由机器人大脑决定),调用后等结果再自然回应;若是问答/闲聊/知识/天气/时间/讲故事等,不要调用control_robot,直接自己语音回答;拿不准是不是动作意图时,不要调control_robot,先正常对话澄清。当control_robot工具返回结果时,把结果里reply字段的文字原样说给用户,不要改写或补充,这是机器人大脑生成的回复。语气口语化,不输出emoji和Markdown。\","
    "\"turn_detection\":{\"type\":\"server_vad\",\"threshold\":0.5,\"silence_duration_ms\":800},"
    "\"tools\":[{\"type\":\"function\",\"function\":{\"name\":\"control_robot\","
    "\"description\":\"控制机器人舵机执行肢体动作。用户要求跳舞/挥手/点头等动作时调用。\","
    "\"parameters\":{\"type\":\"object\",\"properties\":{\"action\":{\"type\":\"string\",\"description\":\"动作名:dance(跳舞)或wave(挥手)或nod(点头)\"}},\"required\":[\"action\"]}}}]"
    "}}";

#define DIALOG_RECV_CAP   65536   /* 接收缓冲, 容纳 25800 字节 audio.delta 帧 */
#define DIALOG_B64_CAP    45000   /* delta base64 解码缓冲(25800*4/3≈34400, 留余量) */
#define DIALOG_PCM_CAP    32000   /* delta 解码后 PCM 缓冲 */
#define DIALOG_JSON_CAP   2400    /* 上行 input_audio_buffer.append JSON 缓冲 */
#define DIALOG_B64_ENC_CAP 2200   /* 960B PCM → 1280 b64, 留余量 */

/* energy gate 运行时阈值(dBFS): AI 播放期间麦克风能量低于此值不上传(滤回声)。
 * 实测(vol=60): -18 最佳(挡住回声 + 插话顺畅)。
 * 越接近 0 越紧(更抗回声但更难插话); 越负越松(易插话但回声易泄漏致自言自语)。
 *
 * 可配: RunDialog 由命令行参数赋值(exec /bin/my_wake dialog -18), send_thread 读取。
 * 默认值 ENERGY_GATE_DBFS_DEFAULT 定义在 wake_dialog.h。 */
static volatile float g_gate_dbfs = ENERGY_GATE_DBFS_DEFAULT;

/* 播放队列上限(TTS 比实时快, 防堆积), 单位: 字节数。
 * 200KB 只够 ~8s, 长句(40字≈10s≈480KB)丢后半段 → 提到 2MB(≈42s) */
#define PLAY_QUEUE_MAX_BYTES  (2 * 1024 * 1024)

/* AI 播放结束后再静音上传 N 帧(每帧 30ms), 滤掉喇叭余响/回声拖尾。
 * 实测 300ms 不够: 问候/回复播完后回声尾巴仍被上传触发云端 speech_started →
 * 自言自语。提到 1000ms(33 帧), 覆盖喇叭余响 + 回声拖尾, 代价是播完后插话
 * 响应慢 1s(可接受, 播完才有插话需求)。 */
#define DIALOG_COOLDOWN_FRAMES  33  /* 33*30ms ≈ 1000ms */

/* 对话空闲超时: N 秒无任何云端事件就断开 WS 回唤醒监听。
 * 用户思考停顿不会太长, 10s 足够; 太长则断开后重连慢。 */
#define DIALOG_IDLE_TIMEOUT_MS  30000

/* 唤醒监听每秒打印一次 mic 电平 + score(诊断用) */
#define WAKE_LISTEN_LOG_FRAMES  33  /* 33*30ms ≈ 1s */
#define WAKE_STEPS_PER_READ     3   /* 每帧 480 点喂 3 次(每次 160 点@10ms) */

/* ================= 共享状态 ================= */

static volatile int g_running = 1;        /* 0 = 全部线程退出 */
static volatile int g_is_responding = 0;  /* 云端正在生成回复 */
static volatile int g_audio_suppressed = 0; /* 打断后丢弃残余音频 */
static volatile int g_is_playing = 0;     /* 正在播放 AI 音频 */
static volatile int g_echo_cooldown = 0;  /* 播放结束后的静音倒计时(滤回声余响) */
static volatile int g_dialog_idle = 0;    /* 1 = 对话空闲超时, recv 主动退出回唤醒 */
static volatile int g_flow_running = 1;   /* 0 = RunWakeFlow 主循环退出 */
/* 问候播放结束标志: play_thread 处理第一个哨兵(问候播放完)时置 1。
 * send_thread 启动后等此标志再开始上传 —— 问候期间不上传麦克风,
 * 云端 server_vad 不会误触发 speech_started 打断问候。8s 超时兜底。 */
static volatile int g_greeting_played = 0;
/* 上次上传帧的麦克风电平(dBFS)。send_thread 每帧更新但不打印(避免刷屏),
 * speech_started 时读它打印一次 —— 确认触发对话那一刻的噪声/人声电平。 */
static volatile float g_last_mic_dbfs = -99.0f;
/* 用户最近一次语音转写文本(UTF-8)。recv_thread 在 transcription.completed 时更新,
 * fc_worker 读它填进 agent prompt 发给 deepseek。g_mtx 保护。
 *
 * item_id 关联(修 function-call 路径转写滞后, 实测日志可见):
 *   事件顺序 = speech_started → committed(带 item_id) → function_call_arguments.done
 *             → ... → transcription.completed(带同 item_id, 滞后于 function_call)。
 *   若 function_call 时直接读 g_last_user_transcript 会拿到上一句旧值
 *   (日志: 用户说"走两步", fc_worker 却拿到"现在也没")。
 *   故 committed 时存 item_id 并 ready=0; transcription.completed 匹配同 item_id 时 ready=1;
 *   fc_worker 轮询等 ready && item_id 匹配, 最多 3s 超时降级。 */
static char g_last_user_transcript[1024];
static char g_user_item_id[128];
static volatile int g_user_transcript_ready = 0;
/* function call busy 标志: fc_worker 运行期间拒绝并发(避免 deepseek 并发请求)。g_mtx 保护。 */
static volatile int g_fc_busy = 0;
static int g_dropped_deltas = 0;          /* 播放队列满丢弃的 delta 计数(诊断) */

/* ---- 端到端时延打点(用户需求: 测唤醒→播放 / 说完话→回复播放) ----
 * 用 gettimeofday 取单调毫秒(CLOCK_MONOTONIC 在 LiteOS-A 可能触发 syscall 256,
 * gettimeofday 是 syscall 78, 普遍支持)。各点在事件处理处打, 首帧播放时算差值打印。
 *   g_t_wake_ms         : 喊到 Luna 的时刻(wake_listen_loop 检测到时记)
 *   g_t_speech_stop_ms  : 用户说完话(speech_stopped, VAD 判定静音起点)
 *   g_t_committed_ms    : 云端提交用户音频(committed, 含 800ms 静音确认后)
 *   g_t_resp_created_ms : 云端开始生成回复(response.created)
 *   g_reply_first_played: 本轮回复首帧已播(0=未播, response.created 时清零)
 * 首帧播放(play_thread)时打印: wake→play / speech_stop→play / committed→play / resp_created→play */
static long g_t_wake_ms = 0;
static long g_t_speech_stop_ms = 0;
static long g_t_committed_ms = 0;
static long g_t_resp_created_ms = 0;
static volatile int g_reply_first_played = 0;

static long now_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long)tv.tv_sec * 1000L + (long)tv.tv_usec / 1000L;
}

static pthread_mutex_t g_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  g_cond = PTHREAD_COND_INITIALIZER;

/* 播放队列: 单链表节点, 存解码后的 PCM 字节 */
typedef struct PlayNode {
    int len;                 /* PCM 字节数; -1 = 哨兵(本轮播放结束) */
    struct PlayNode *next;
    unsigned char data[];
} PlayNode;

static PlayNode *g_play_head = NULL;
static PlayNode *g_play_tail = NULL;
static int g_play_queue_bytes = 0;

/* ================= 播放队列操作 ================= */

static void play_queue_clear(void)
{
    PlayNode *p = g_play_head;
    g_play_head = g_play_tail = NULL;
    g_play_queue_bytes = 0;
    while (p) {
        PlayNode *n = p->next;
        free(p);
        p = n;
    }
}

/* 入队 PCM 数据(len>0)或哨兵(len=-1)。返回 0 成功, -1 队列满丢弃 */
static int play_queue_push(const unsigned char *data, int len)
{
    pthread_mutex_lock(&g_mtx);
    if (len > 0 && g_play_queue_bytes + len > PLAY_QUEUE_MAX_BYTES) {
        g_dropped_deltas++;
        if (g_dropped_deltas % 10 == 1) {
            printf("[dialog] queue full (%d bytes), dropped %d deltas\n",
                   g_play_queue_bytes, g_dropped_deltas);
        }
        pthread_mutex_unlock(&g_mtx);
        return -1;  /* 队列满, 丢弃(避免 TTS 堆积导致大延迟) */
    }
    PlayNode *node = (PlayNode *)malloc(sizeof(PlayNode) + (len > 0 ? len : 0));
    if (node == NULL) {
        pthread_mutex_unlock(&g_mtx);
        return -1;
    }
    node->len = len;
    node->next = NULL;
    if (len > 0) {
        memcpy(node->data, data, len);
        g_play_queue_bytes += len;
    }
    if (g_play_tail) {
        g_play_tail->next = node;
    } else {
        g_play_head = node;
    }
    g_play_tail = node;
    pthread_cond_signal(&g_cond);
    pthread_mutex_unlock(&g_mtx);
    return 0;
}

/* 出队, 阻塞直到有数据或退出。返回节点(调用方 free), NULL=退出 */
static PlayNode *play_queue_pop(void)
{
    pthread_mutex_lock(&g_mtx);
    while (g_play_head == NULL && g_running) {
        pthread_cond_wait(&g_cond, &g_mtx);
    }
    PlayNode *node = g_play_head;
    if (node) {
        g_play_head = node->next;
        if (g_play_head == NULL) g_play_tail = NULL;
        if (node->len > 0) g_play_queue_bytes -= node->len;
    }
    pthread_mutex_unlock(&g_mtx);
    return node;
}

/* ================= JSON 最小字段提取 ================= */

/*
 * 提取 JSON 字符串字段值(简单实现, 处理 \" 转义)。
 * 在 json 中找 "key": "value"(冒号两侧允许空格), 拷贝 value 到 out。
 * 支持模型生成的带空格 JSON(如 {"action": "dance"}), 成功返回长度, 未找到 -1。
 */
static int json_get_str(const char *json, const char *key, char *out, int cap)
{
    /* 构造 "key" 模式(不带冒号, 冒号两侧可能有空格, 手动扫描) */
    char pattern[64];
    int kl = strlen(key);
    if (kl + 3 >= (int)sizeof(pattern)) return -1;
    pattern[0] = '"';
    memcpy(pattern + 1, key, kl);
    pattern[1 + kl] = '"';
    pattern[2 + kl] = '\0';

    const char *p = strstr(json, pattern);
    if (p == NULL) return -1;
    p += kl + 2;  /* 跳过 "key" */
    /* 跳过空白 → 期望冒号 → 跳过空白 → 期望开引号 */
    while (*p == ' ' || *p == '\t') p++;
    if (*p != ':') return -1;
    p++;
    while (*p == ' ' || *p == '\t') p++;
    if (*p != '"') return -1;
    p++;
    int n = 0;
    while (*p && *p != '"' && n < cap - 1) {
        if (*p == '\\') {  /* 跳过转义字符(如 \") */
            p++;
            if (*p == '\0') break;
        }
        out[n++] = *p++;
    }
    out[n] = '\0';
    return n;
}

/* ================= energy 计算 ================= */

static float rms_dbfs(const short *s, int n)
{
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        double v = (double)s[i] / 32768.0;
        sum += v * v;
    }
    double rms = sqrt(sum / n);
    return rms > 1e-9 ? (float)(20.0 * log10(rms)) : -120.0f;
}

/* ================= 三线程 ================= */

/* send 线程: 采集 → base64 → 上传。energy gate 回声抑制 */
static void *send_thread(void *arg)
{
    (void)arg;
    short capBuf[WAKE_PT_PER_FRM];
    char *b64 = (char *)malloc(DIALOG_B64_ENC_CAP);
    char *json = (char *)malloc(DIALOG_JSON_CAP);
    if (b64 == NULL || json == NULL) {
        printf("[dialog] send: malloc failed\n");
        free(b64); free(json);
        g_running = 0;
        pthread_cond_signal(&g_cond);
        return NULL;
    }

    int echoFrames = 0;        /* 连续低能量帧计数(诊断回声电平) */
    int cooldownLeft = 0;      /* 本地静音冷却剩余帧 */
    int cooldownSkipped = 0;   /* 本次冷却静音掉的帧数(诊断: 确认冷却是否真生效) */
    int inCooldown = 0;        /* 1=处于冷却期, 冷却结束首个上传帧打印一次诊断 */

    /* 问候期间不上传: 等问候播放结束(g_greeting_played)再开始, 否则云端 server_vad
     * 会把麦克风环境声/唤醒残响误判为"用户说话"→ speech_started → 打断问候(喇叭不出声)。
     * 超时 8s 兜底: 问候异常无哨兵时也能继续对话。 */
    if (!g_greeting_played) {
        int waited = 0;
        while (!g_greeting_played && g_running && waited < 8000) {
            usleep(100 * 1000);  /* 100ms 轮询 */
            waited += 100;
            if (waited % 1000 == 0) {  /* 每秒打印等待状态 */
                pthread_mutex_lock(&g_mtx);
                printf("[send] greeting wait %ds: played=%d responding=%d playing=%d queue=%dB\n",
                       waited / 1000, g_greeting_played, g_is_responding, g_is_playing,
                       g_play_queue_bytes);
                pthread_mutex_unlock(&g_mtx);
            }
        }
        if (g_greeting_played) {
            printf("[dialog] send: greeting done, start uploading\n");
        } else {
            printf("[dialog] send: greeting wait timeout, upload anyway\n");
        }
        /* 进入主循环前打印冷却状态, 确认 sentinel 设的 g_echo_cooldown 是否生效 */
        pthread_mutex_lock(&g_mtx);
        printf("[send] enter main loop: echo_cooldown=%d is_playing=%d\n",
               g_echo_cooldown, g_is_playing);
        pthread_mutex_unlock(&g_mtx);
    }

    while (g_running) {
        if (WakeCaptureRead(capBuf, WAKE_PT_PER_FRM) != 0) {
            continue;
        }

        /* 回声抑制(echo_suppression, 镜像 Qwen 文档 realtime_demo.py 第 494-502 行):
         * AI 正在生成回复(g_is_responding)或正在播放(g_is_playing)时, 麦克风完全
         * 静音不上传 —— 不判能量(嘈杂环境能量门控分不清人声/噪声/回声, 误放行致自言自语)。
         * 代价: 不支持打断(AI 说话时插不进话), 需等 AI 说完。无耳机场景这是最稳方案。
         * 播放结束后 cooldownLeft(1s) 内也不上传, 滤喇叭余响/回声拖尾。 */
        pthread_mutex_lock(&g_mtx);
        int active = g_is_responding || g_is_playing;
        int cd = g_echo_cooldown;
        pthread_mutex_unlock(&g_mtx);
        if (active) {
            continue;  /* AI 在生成/播放: 静音, 不上传 */
        }

        /* 播放结束后冷却: 喇叭余响/回声拖尾, 冷却期内不上传 */
        if (cooldownLeft > 0) {
            cooldownSkipped++;
            cooldownLeft--;
            continue;
        }
        if (cd > 0) {
            pthread_mutex_lock(&g_mtx);
            g_echo_cooldown--;
            pthread_mutex_unlock(&g_mtx);
            cooldownLeft = DIALOG_COOLDOWN_FRAMES;
            cooldownSkipped = 0;
            inCooldown = 1;  /* 冷却期开始; 冷却结束首个上传帧打印一次诊断 */
            printf("[send] cooldown triggered, cooldownLeft=%d\n", cooldownLeft);
            continue;
        }

        int bl = wake_base64_encode((const unsigned char *)capBuf, WAKE_FRAME_BYTES,
                                    b64, DIALOG_B64_ENC_CAP);
        if (bl <= 0) continue;
        int jl = snprintf(json, DIALOG_JSON_CAP,
            "{\"type\":\"input_audio_buffer.append\",\"audio\":\"%s\"}", b64);
        if (jl <= 0 || jl >= DIALOG_JSON_CAP) continue;
        /* 记录本帧 mic 电平(不逐帧打印, 避免刷屏); speech_started 时读 g_last_mic_dbfs 打印。
         * 冷却结束后的首个上传帧打印一次: 确认冷却静音帧数(skipped≈33≈1s 生效?)
         * + 冷却刚结束时的噪声底噪(判断是否噪声触发云端 VAD 致自言自语)。 */
        {
            float lvl = rms_dbfs(capBuf, WAKE_PT_PER_FRM);
            g_last_mic_dbfs = lvl;
            if (inCooldown) {
                printf("[send] cooldown ended: skipped %d frames (~%dms), first mic=%.1f dBFS\n",
                       cooldownSkipped, cooldownSkipped * 30, lvl);
                inCooldown = 0;
            }
        }
        if (wake_ws_send_text(json, jl) != 0) {
            printf("[dialog] send: ws send failed, exit\n");
            g_running = 0;
            break;
        }
    }

    free(b64);
    free(json);
    return NULL;
}

/* ================= Function Calling ================= */

/*
 * 处理 response.function_call_arguments.done: 模型决定调用工具。
 * 流程(双模型, 对齐官方 realtime_fc_demo.py + prompt/agent_service.c):
 *   1. 从事件提取 call_id / name / arguments
 *   2. 若 control_robot: spawn fc_worker 线程(非阻塞!), 把用户转录发 agent_bridge
 *      (agent_bridge 组 prompt → llm_chat 发 deepseek → 解析四行协议 → 执行动作/表情
 *       → 返回 [SAY] 文本)
 *   3. fc_worker 把 [SAY] 写进 function_call_output → 发 conversation.item.create
 *   4. 发 response.create 触发二轮: 音频模型读 output.reply 原样说出来
 *
 * 关键: llm_chat 阻塞 3-8s, 必须在 worker 线程跑, 不能卡 recv_thread。
 * output 字段是 JSON 字符串, 嵌入外层 JSON 时双引号转义(用 \\\")。
 */

/* 清理文本用于嵌入 JSON output: 把 " \ \n \r \t 换成空格。
 * [SAY] 是 deepseek 的中文回复, 不会含这些; 此为防破坏 JSON 的保险。 */
static void sanitize_for_json(const char *src, char *dst, int cap)
{
    int n = 0;
    while (*src && n + 1 < cap) {
        char c = *src++;
        if (c == '"' || c == '\\' || c == '\n' || c == '\r' || c == '\t') c = ' ';
        dst[n++] = c;
    }
    dst[n] = '\0';
}

/* 发 function_call_output(写回结果) + response.create(触发二轮)。
 * output 是已构造的 layer-1 JSON 字符串(内部双引号已用 \" 转义)。
 * wake_ws_send_text 已在 wake_ws.c 内部加发送互斥锁, 此处多线程安全。 */
static void fc_send_result(const char *call_id, const char *output)
{
    char *msg = (char *)malloc(2048);
    if (msg != NULL) {
        int ml = snprintf(msg, 2048,
            "{\"type\":\"conversation.item.create\",\"item\":{"
            "\"type\":\"function_call_output\",\"call_id\":\"%s\",\"output\":\"%s\"}}",
            call_id, output);
        if (ml > 0 && ml < 2048) {
            wake_ws_send_text(msg, ml);
        }
        free(msg);
    }
    const char *rr = "{\"type\":\"response.create\",\"response\":{\"modalities\":[\"audio\",\"text\"]}}";
    wake_ws_send_text(rr, (int)strlen(rr));
    printf("[FC] response.create sent (round-2)\n");
}

/* fc_worker 线程: 在 recv_thread 之外跑 deepseek 问答 + 写回结果。
 * req(call_id/arguments/item_id) 堆分配, 本线程结束时 free。
 *
 * 转写等待(修 Bug: function-call 路径转写滞后):
 *   response.function_call_arguments.done 先到, conversation.item.input_audio_transcription.completed
 *   后到。若 function_call 时直接读 g_last_user_transcript 会拿到上一句旧值(实测: 用户说
 *   "走两步", fc_worker 却拿到上句"现在也没")。故此处用 item_id 轮询等匹配的转写就绪再发
 *   deepseek, 最多 3s; 超时降级用最新转写(可能旧)或 arguments 的 action。
 *   item_id 由 input_audio_buffer.committed 设置, transcription.completed 匹配后置 ready。 */
static void *fc_worker(void *arg)
{
    typedef struct {
        char call_id[128];
        char arguments[256];
        char item_id[128];
    } FcReq;
    FcReq *req = (FcReq *)arg;

    /* 轮询等待当前 user item 的转写就绪(同 item_id 且 ready=1)。
     * item_id 变化(用户又说了新一句)或超时则退出循环走降级。 */
    char transcript[1024] = {0};
    int ready = 0;
    int waited_ms = 0;
    while (g_running && waited_ms < 3000) {
        pthread_mutex_lock(&g_mtx);
        if (g_user_transcript_ready &&
            strcmp(g_user_item_id, req->item_id) == 0) {
            ready = 1;
            snprintf(transcript, sizeof(transcript), "%s", g_last_user_transcript);
        }
        pthread_mutex_unlock(&g_mtx);
        if (ready) break;
        usleep(50 * 1000);
        waited_ms += 50;
    }
    if (!ready) {
        /* 超时或 item 已变: 降级用最新转写(可能为上一句旧值); 仍空则下面用 action */
        pthread_mutex_lock(&g_mtx);
        snprintf(transcript, sizeof(transcript), "%s", g_last_user_transcript);
        pthread_mutex_unlock(&g_mtx);
    }

    /* action 参数(function call 自带, 转写全空时的最终降级) */
    char action[64] = {0};
    (void)json_get_str(req->arguments, "action", action, sizeof(action));
    const char *text = (transcript[0] != '\0') ? transcript :
                       (action[0] != '\0') ? action : NULL;

    printf("[FC] fc_worker start (item_id=%s ready=%d waited=%dms text=%s)\n",
           req->item_id, ready, waited_ms, text ? text : "(empty)");

    char say[1024] = {0};
    int r = agent_bridge_run(text, say, sizeof(say));

    char output[1300];
    if (r == 0) {
        char esc_say[1100];
        sanitize_for_json(say, esc_say, sizeof(esc_say));
        snprintf(output, sizeof(output),
                 "{\\\"reply\\\":\\\"%s\\\",\\\"status\\\":\\\"ok\\\"}", esc_say);
        printf("[FC] function_call_output: reply set, status=ok\n");
    } else {
        snprintf(output, sizeof(output),
                 "{\\\"status\\\":\\\"failed\\\",\\\"error\\\":\\\"agent_bridge %d\\\"}", r);
        printf("[FC] function_call_output: status=failed (agent_bridge %d)\n", r);
    }

    fc_send_result(req->call_id, output);

    pthread_mutex_lock(&g_mtx);
    g_fc_busy = 0;
    pthread_mutex_unlock(&g_mtx);
    printf("[FC] fc_worker done\n");
    free(req);
    return NULL;
}


static void handle_function_call(const char *call_id, const char *name, const char *arguments)
{
    /* fc_worker 请求(堆分配, 传给 detached 线程)。
     * 不在此处快照 transcript —— function_call 早于 transcription.completed,
     * 此刻 g_last_user_transcript 还是上一句旧值。改为快照 item_id,
     * fc_worker 按 item_id 轮询等转写就绪。 */
    typedef struct {
        char call_id[128];
        char arguments[256];
        char item_id[128];
    } FcReq;

    if (strcmp(name, "control_robot") != 0) {
        printf("[FC] unknown function: %s\n", name);
        char output[256];
        snprintf(output, sizeof(output),
                 "{\\\"error\\\":\\\"unknown function: %s\\\"}", name);
        fc_send_result(call_id, output);
        return;
    }

    /* 并发互斥: deepseek 请求耗时 3-8s, 期间再来一次 function call 直接拒绝
     * (避免并发请求打爆 deepseek, 且 reply 语义上一次只该有一个) */
    pthread_mutex_lock(&g_mtx);
    if (g_fc_busy) {
        pthread_mutex_unlock(&g_mtx);
        printf("[FC] busy, concurrent call rejected\n");
        fc_send_result(call_id, "{\\\"status\\\":\\\"busy\\\",\\\"error\\\":\\\"previous call in progress\\\"}");
        return;
    }
    g_fc_busy = 1;
    FcReq *req = (FcReq *)malloc(sizeof(FcReq));
    if (req == NULL) {
        pthread_mutex_unlock(&g_mtx);
        printf("[FC] malloc FcReq failed\n");
        fc_send_result(call_id, "{\\\"status\\\":\\\"failed\\\",\\\"error\\\":\\\"no memory\\\"}");
        return;
    }
    memset(req, 0, sizeof(*req));
    snprintf(req->call_id, sizeof(req->call_id), "%s", call_id);
    snprintf(req->arguments, sizeof(req->arguments), "%s", arguments);
    snprintf(req->item_id, sizeof(req->item_id), "%s", g_user_item_id);
    int ready_now = g_user_transcript_ready;
    pthread_mutex_unlock(&g_mtx);

    /* spawn detached worker: llm_chat 阻塞 3-8s, 不能在 recv_thread 同步跑
     * (否则期间 audio.delta 堆 WS 缓冲、play_thread 断粮、播放卡顿) */
    pthread_attr_t attr;
    pthread_t tid;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    int pc = pthread_create(&tid, &attr, fc_worker, req);
    pthread_attr_destroy(&attr);
    if (pc != 0) {
        printf("[FC] spawn fc_worker failed: %d\n", pc);
        free(req);
        pthread_mutex_lock(&g_mtx);
        g_fc_busy = 0;
        pthread_mutex_unlock(&g_mtx);
        fc_send_result(call_id, "{\\\"status\\\":\\\"failed\\\",\\\"error\\\":\\\"spawn failed\\\"}");
        return;
    }
    printf("[FC] fc_worker spawned (call_id=%s, item_id=%s, ready=%d)\n",
           call_id, req->item_id, ready_now);
}

/* recv 线程: 收 WS 帧 → 解析 → 分发 */
static void *recv_thread(void *arg)
{
    (void)arg;
    char *buf = (char *)malloc(DIALOG_RECV_CAP);
    char *type = (char *)malloc(128);
    char *b64 = (char *)malloc(DIALOG_B64_CAP);
    unsigned char *pcm = (unsigned char *)malloc(DIALOG_PCM_CAP);
    char *transcript = (char *)malloc(4096);
    if (buf == NULL || type == NULL || b64 == NULL || pcm == NULL || transcript == NULL) {
        printf("[dialog] recv: malloc failed\n");
        g_running = 0;
        return NULL;
    }

    /* 空闲超时: try_recv 轮询, 无事件累计 idle_ms; 收到任意事件清零。
     * 用阻塞版 wake_ws_recv_text 会卡 30s, 无法做 60s 空闲超时。
     * try_recv 非阻塞返回, 每 100ms 轮询一次, 期间能及时响应 g_running=0 */
    int idle_ms = 0;
    const int poll_ms = 100;
    const int idle_limit = DIALOG_IDLE_TIMEOUT_MS;

    while (g_running) {
        int n = wake_ws_try_recv(buf, DIALOG_RECV_CAP);
        if (n < 0) {
            printf("[dialog] recv: ws closed, exit\n");
            g_running = 0;
            break;
        }
        if (n == 0) {
            /* 无数据: 累计空闲时间, 超时则退出回唤醒 */
            idle_ms += poll_ms;
            if (idle_ms >= idle_limit) {
                printf("[dialog] idle %ds, back to wake listen\n", idle_limit / 1000);
                g_dialog_idle = 1;
                g_running = 0;
                break;
            }
            usleep(poll_ms * 1000);
            continue;
        }
        /* 收到事件: 清零空闲计时 */
        idle_ms = 0;
        buf[n] = '\0';

        if (json_get_str(buf, "type", type, 128) < 0) continue;
        printf("[recv] %s (%d bytes)\n", type, n);

        if (strcmp(type, "input_audio_buffer.speech_started") == 0) {                                                                                                                                                                                        
            /* 用户开始说话 → 打断 AI */
            pthread_mutex_lock(&g_mtx);
            printf("\n[dialog] --- speech_started: responding=%d playing=%d cooldown=%d greeting_played=%d mic=%.1f dBFS ---\n",
                   g_is_responding, g_is_playing, g_echo_cooldown, g_greeting_played, (double)g_last_mic_dbfs);
            play_queue_clear();
            g_is_playing = 0;
            g_audio_suppressed = 1;
            g_echo_cooldown = 0;
            pthread_mutex_unlock(&g_mtx);
            if (g_is_responding) {
                wake_ws_send_text("{\"type\":\"response.cancel\"}", 27);
            }
        } else if (strcmp(type, "response.created") == 0) {
            g_is_responding = 1;
            g_audio_suppressed = 0;
            g_t_resp_created_ms = now_ms();
            g_reply_first_played = 0;  /* 本轮回复首帧待播 */
            printf("[recv] response.created (responding=1)\n");
        } else if (strcmp(type, "response.done") == 0) {
            g_is_responding = 0;
            play_queue_push(NULL, -1);  /* 哨兵: 本轮播放结束 */
            printf("[recv] response.done -> push sentinel (greeting_played wait will release)\n");
        } else if (strcmp(type, "response.audio.delta") == 0) {
            if (g_audio_suppressed) {
                printf("[recv] audio.delta SUPPRESSED (dropped)\n");
                continue;
            }
            int dl = json_get_str(buf, "delta", b64, DIALOG_B64_CAP);
            if (dl <= 0) continue;
            int pl = wake_base64_decode(b64, pcm, DIALOG_PCM_CAP);
            if (pl <= 0) continue;
            play_queue_push(pcm, pl);
        } else if (strcmp(type, "response.audio.done") == 0) {
            /* 单条音频结束(response.done 也会来, 哨兵由 response.done 入) */
        } else if (strcmp(type, "response.function_call_arguments.done") == 0) {
            /* 模型决定调用工具: 提取 call_id/name/arguments → 执行 → 写回 → 二轮 */
            char call_id[128];
            char fname[64];
            char arguments[1024];
            if (json_get_str(buf, "call_id", call_id, sizeof(call_id)) >= 0 &&
                json_get_str(buf, "name", fname, sizeof(fname)) >= 0) {
                if (json_get_str(buf, "arguments", arguments, sizeof(arguments)) < 0) {
                    arguments[0] = '\0';
                }
                printf("[FC] call %s, call_id=%s, args=%s\n", fname, call_id, arguments);
                handle_function_call(call_id, fname, arguments);
            }
        } else if (strcmp(type, "input_audio_buffer.speech_stopped") == 0) {
            /* 用户说完话(VAD 判静音): 时延打点 "说完话→播放" 的起点。
             * 含 800ms 静音确认才到 committed, 故 speech_stop→play 比 committed→play 多 ~800ms */
            g_t_speech_stop_ms = now_ms();
            printf("[latency] speech stopped at %ld ms\n", g_t_speech_stop_ms);
        } else if (strcmp(type, "input_audio_buffer.committed") == 0) {
            /* 用户一句话音频提交: 记录 item_id, 标记转写未就绪(ready=0)。
             * transcription.completed 会带同 item_id 来, 届时 ready=1。
             * function_call 早于 transcription.completed 时, fc_worker 靠 item_id 等待。 */
            char citem[128];
            if (json_get_str(buf, "item_id", citem, sizeof(citem)) < 0) citem[0] = '\0';
            pthread_mutex_lock(&g_mtx);
            snprintf(g_user_item_id, sizeof(g_user_item_id), "%s", citem);
            g_user_transcript_ready = 0;
            pthread_mutex_unlock(&g_mtx);
            g_t_committed_ms = now_ms();  /* 时延打点: 云端拿到完整用户音频 */
            printf("[FC] audio committed (item_id=%s, ready=0)\n", citem);
        } else if (strcmp(type, "conversation.item.input_audio_transcription.completed") == 0) {
            if (json_get_str(buf, "transcript", transcript, 4096) >= 0) {
                printf("[You] %s\n", transcript);
                /* 存用户转录 + 标记当前 item 转写就绪。
                 * 提取本事件 item_id, 与 g_user_item_id 匹配则 ready=1,
                 * fc_worker 按 item_id 等到此刻即拿到正确转写(非上一句旧值)。 */
                char titem[128];
                if (json_get_str(buf, "item_id", titem, sizeof(titem)) < 0) titem[0] = '\0';
                pthread_mutex_lock(&g_mtx);
                snprintf(g_last_user_transcript, sizeof(g_last_user_transcript), "%s", transcript);
                int match = (titem[0] != '\0' && strcmp(titem, g_user_item_id) == 0);
                if (match) g_user_transcript_ready = 1;
                int rdy = g_user_transcript_ready;
                pthread_mutex_unlock(&g_mtx);
                printf("[FC] transcript done (item_id=%s match=%d ready=%d)\n", titem, match, rdy);
            }
        } else if (strcmp(type, "response.audio_transcript.done") == 0) {
            if (json_get_str(buf, "transcript", transcript, 4096) >= 0) {
                printf("[AI] %s\n", transcript);
            }
        } else if (strcmp(type, "error") == 0) {
            char msg[512];
            if (json_get_str(buf, "message", msg, sizeof(msg)) >= 0) {
                if (strstr(msg, "no active response") == NULL) {
                    printf("[Error] %s\n", msg);
                }
            }
        }
        /* 其余事件(session.created/updated, *.delta 转写增量等)忽略 */
    }

    free(buf); free(type); free(b64); free(pcm); free(transcript);
    return NULL;
}

/* play 线程: 出队 → 播放 */
static void *play_thread(void *arg)
{
    (void)arg;
    int play_seq = 0;
    while (g_running) {
        PlayNode *node = play_queue_pop();
        if (node == NULL) break;  /* g_running=0 退出 */
        if (node->len == -1) {
            /* 哨兵: 本轮播放结束 → 设冷却, 滤喇叭余响/回声拖尾 */
            pthread_mutex_lock(&g_mtx);
            g_is_playing = 0;
            g_echo_cooldown = 1;  /* 触发 send 线程冷却 */
            /* 第一个哨兵 = 问候播放结束: 唤醒 send_thread 开始上传(问候期间没上传) */
            if (!g_greeting_played) {
                g_greeting_played = 1;
                printf("[play] sentinel#%d -> greeting_played=1 (send_thread released)\n", play_seq);
            } else {
                printf("[play] sentinel#%d -> cooldown (round end)\n", play_seq);
            }
            play_seq++;
            pthread_mutex_unlock(&g_mtx);
            free(node);
            continue;
        }
        if (g_audio_suppressed) {
            printf("[play] node#%d len=%d SUPPRESSED (dropped)\n", play_seq, node->len);
            play_seq++;
            free(node);
            continue;  /* 打断后丢弃残余 */
        }
        pthread_mutex_lock(&g_mtx);
        g_is_playing = 1;
        int qb = g_play_queue_bytes;
        pthread_mutex_unlock(&g_mtx);
        printf("[play] node#%d len=%d queue=%dB -> WakePlaybackWrite\n", play_seq, node->len, qb);
        play_seq++;
        /* 时延打点: 本轮回复首帧即将送喇叭, 算各起点到此刻的差值。
         * - wake->play      : 喊 Luna 到喇叭出声(问候轮, 测一次后清零不再测)
         * - speech_stop->play: 用户说完话(VAD 静音)到喇叭出声(含 800ms 静音确认 + 云端处理)
         * - committed->play : 云端拿到完整音频到喇叭出声(纯云端处理, 排除静音确认)
         * - resp_created->play: 云端开始生成回复到喇叭出声(TTS + 网络, 排除 ASR) */
        if (!g_reply_first_played && g_t_resp_created_ms != 0) {
            g_reply_first_played = 1;
            long tnow = now_ms();
            if (g_t_wake_ms != 0) {
                printf("[latency] wake-detected -> first-play: %ld ms\n", tnow - g_t_wake_ms);
                g_t_wake_ms = 0;  /* 问候首播后清零, 后续轮不测唤醒延迟 */
            }
            if (g_t_speech_stop_ms != 0) {
                printf("[latency] speech-stop -> first-play: %ld ms\n", tnow - g_t_speech_stop_ms);
            }
            if (g_t_committed_ms != 0) {
                printf("[latency] committed -> first-play: %ld ms\n", tnow - g_t_committed_ms);
            }
            printf("[latency] response-created -> first-play: %ld ms\n", tnow - g_t_resp_created_ms);
        }
        WakePlaybackWrite(node->data, node->len);
        free(node);
    }
    return NULL;
}

/* ================= 对话主流程 ================= */

/*
 * 单次对话会话: WS 建连 + session.update + 三线程 + 空闲超时退出。
 * 假定采集器(WakeCaptureInit)和播放(WakePlaybackInit)已由调用方初始化。
 * 返回 0 = 正常(空闲超时/ws 关闭), 非 0 = 建连失败。
 * 退出后采集/播放/WS 由调用方清理(支持复用采集器回唤醒监听)。
 */
static int run_dialog_session(float gate_dbfs, unsigned int mic_vol)
{
    /* 重置共享状态(上一轮可能残留) */
    g_running = 1;
    g_is_responding = 0;
    g_audio_suppressed = 0;
    g_is_playing = 0;
    g_echo_cooldown = 0;
    g_dialog_idle = 0;
    g_greeting_played = 0;
    g_last_mic_dbfs = -99.0f;
    g_user_transcript_ready = 0;
    g_user_item_id[0] = '\0';
    g_last_user_transcript[0] = '\0';
    /* 时延打点: g_t_wake_ms 由 wake_listen_loop 检测时设置(保留进来), 其余清零 */
    g_t_speech_stop_ms = 0;
    g_t_committed_ms = 0;
    g_t_resp_created_ms = 0;
    g_reply_first_played = 0;
    g_gate_dbfs = gate_dbfs;

    /* 对话麦克风增益(可调): 降低增益减小噪声底噪, 减少云端 server_vad 把噪声误判
     * 为"用户说话" → speech_started → AI 回复噪声(自言自语)。
     * 命令行/exec env 可调; 默认=唤醒校准值。RunWakeFlow 对话结束会恢复唤醒增益。 */
    if (mic_vol == 0 || mic_vol > 0x7e) {
        mic_vol = WakeCaptureDefaultMicVol();
    }
    if (WakeCaptureSetMicVol(mic_vol) == 0) {
        printf("[dialog] mic vol=%u (wake default=%u)\n", mic_vol, WakeCaptureDefaultMicVol());
    }

    /* 1. WebSocket 连接(重试 3 次: WiFi 刚连上路由/ARP 未稳时首次 TCP 易失败) */
    printf("[dialog] ws connect...\n");
    int ws_ok = 0;
    for (int try = 0; try < 3; try++) {
        if (wake_ws_connect(QWEN_WS_IP, QWEN_WS_HOST, QWEN_WS_PORT,
                            QWEN_WS_PATH, DIALOG_API_KEY) == 0) {
            ws_ok = 1;
            break;
        }
        printf("[dialog] ws connect try %d failed, retry in 1s\n", try + 1);
        usleep(1000 * 1000);
    }
    if (!ws_ok) {
        printf("[dialog] ws connect FAILED after 3 tries\n");
        return 1;
    }

    /* drain session.created */
    char *tbuf = (char *)malloc(4096);
    if (tbuf) {
        int n = wake_ws_recv_text(tbuf, 4096);
        if (n > 0) {
            tbuf[n] = '\0';
            printf("[dialog] session.created received (%d bytes): %.120s\n", n, tbuf);
        } else {
            printf("[dialog] session.created drain FAILED n=%d\n", n);
        }
        free(tbuf);
    }

    /* 2. 发 session.update(含 instructions + tools) */
    int rsu = wake_ws_send_text(SESSION_UPDATE_JSON, strlen(SESSION_UPDATE_JSON));
    printf("[dialog] session.update sent ret=%d, energy gate %.0f dBFS\n",
           rsu, (double)g_gate_dbfs);
    if (rsu != 0) {
        printf("[dialog] send session.update FAILED\n");
        wake_ws_close();
        return 1;
    }

    /* drain 等 session.updated ack(确认 session.update 被云端接受) */
    {
        char *dbuf = (char *)malloc(4096);
        if (dbuf) {
            int got = 0;
            for (int waited = 0; waited < 5000 && !got; waited += 100) {
                int n = wake_ws_try_recv(dbuf, 4096);
                if (n < 0) { printf("[dialog] drain(session.update): ws closed\n"); break; }
                if (n == 0) { usleep(100 * 1000); continue; }
                dbuf[n] = '\0';
                char t2[64];
                if (json_get_str(dbuf, "type", t2, sizeof(t2)) > 0) {
                    printf("[dialog] drain(su) %s (%d bytes): %.200s\n", t2, n, dbuf);
                    if (strcmp(t2, "session.updated") == 0) got = 1;
                } else {
                    printf("[dialog] drain(su) ??? (%d bytes): %.200s\n", n, dbuf);
                }
            }
            printf("[dialog] drain(session.update) done, got_updated=%d\n", got);
            free(dbuf);
        }
    }

    /* 3. 唤醒后让 Luna 主动开口: 先注入一条 user 消息"你好", 等云端 ack, 再 response.create。
     * 关键: inject 和 response.create 不能背靠背发 —— 云端处理 response.create 时
     * 若 user message 尚未提交, 会静默不生成回复(无 response.created, 无报错)。
     * 必须 drain 等 conversation.item.created ack, 再发 response.create。 */
    const char *inject = "{\"type\":\"conversation.item.create\",\"item\":{"
        "\"type\":\"message\",\"role\":\"user\","
        "\"content\":[{\"type\":\"input_text\",\"text\":\"你好\"}]}}";
    int r1 = wake_ws_send_text(inject, (int)strlen(inject));
    printf("[dialog] inject 你好 send=%d (%zu bytes payload)\n", r1, strlen(inject));

    /* drain 等 conversation.item.created ack */
    {
        char *dbuf = (char *)malloc(4096);
        if (dbuf) {
            int got_item = 0;
            for (int waited = 0; waited < 5000 && !got_item; waited += 100) {
                int n = wake_ws_try_recv(dbuf, 4096);
                if (n < 0) { printf("[dialog] drain(inject): ws closed\n"); break; }
                if (n == 0) { usleep(100 * 1000); continue; }
                dbuf[n] = '\0';
                char t2[64];
                if (json_get_str(dbuf, "type", t2, sizeof(t2)) > 0) {
                    printf("[dialog] drain(inj) %s (%d bytes): %.200s\n", t2, n, dbuf);
                    if (strcmp(t2, "conversation.item.created") == 0) got_item = 1;
                } else {
                    printf("[dialog] drain(inj) ??? (%d bytes): %.200s\n", n, dbuf);
                }
            }
            printf("[dialog] drain(inject) done, got_item_created=%d\n", got_item);
            free(dbuf);
        }
    }

    const char *greet = "{\"type\":\"response.create\",\"response\":{\"modalities\":[\"audio\",\"text\"]}}";
    int r2 = wake_ws_send_text(greet, (int)strlen(greet));
    printf("[dialog] response.create send=%d (%zu bytes payload)\n", r2, strlen(greet));
    printf("[dialog] ready! Luna will greet you, then talk (idle %ds -> wake)\n",
           DIALOG_IDLE_TIMEOUT_MS / 1000);

    /* 4. 起三线程 */
    pthread_t t_send, t_recv, t_play;
    pthread_create(&t_send, NULL, send_thread, NULL);
    pthread_create(&t_recv, NULL, recv_thread, NULL);
    pthread_create(&t_play, NULL, play_thread, NULL);

    /* 主线程等 recv 退出(空闲超时/ws 关闭) */
    pthread_join(t_recv, NULL);
    g_running = 0;
    pthread_cond_broadcast(&g_cond);
    pthread_join(t_send, NULL);
    pthread_join(t_play, NULL);

    /* 5. 清理本轮 WS + 播放队列(采集器复用, 不 Deinit) */
    play_queue_clear();
    wake_ws_close();
    printf("[dialog] session ended%s\n", g_dialog_idle ? " (idle timeout)" : "");
    return 0;
}

/* ================= 唤醒 → 对话主循环 ================= */

/*
 * 唤醒监听循环: 采集 → 喂 wake_word → 检测到 "Luna" 返回。
 * 假定采集器已初始化(由 RunWakeFlow 调用前 init)。每秒打印 mic 电平+score。
 * 返回 0 = 检测到唤醒, 非 0 = 采集错误。
 */
static int wake_listen_loop(void)
{
    printf("[wake] listening for \"Luna\"...\n");

    short buf[WAKE_PT_PER_FRM];
    int frameCount = 0;

    /* 回唤醒监听后丢弃前 N 帧(1s): 对话刚结束喇叭余响/回声未消, 刚 reset 的模型
     * 冷启动, 喂入余响易误触发(说"你好"唤醒 Luna)。丢弃期让环境稳定后再检测。
     * 仅首次进入(frameCount 从 0 起)生效。 */
    const int WAKE_WARMUP_FRAMES = 33;  /* 33*30ms ≈ 1s */
    int warmup = WAKE_WARMUP_FRAMES;

    while (g_flow_running) {
        if (WakeCaptureRead(buf, WAKE_PT_PER_FRM) != 0) {
            printf("[wake] read failed, retry\n");
            continue;
        }

        /* 预热期: 丢弃音频不喂模型, 让喇叭余响/回声消退 */
        if (warmup > 0) {
            warmup--;
            if (warmup == 0) {
                printf("[wake] warmup done (1s), start detecting\n");
            }
            continue;
        }

        bool detected = false;
        for (int step = 0; step < WAKE_STEPS_PER_READ; step++) {
            if (wake_word_process(buf + step * WAKE_WORD_SAMPLES_PER_STEP)) {
                detected = true;
            }
        }

        if (frameCount % WAKE_LISTEN_LOG_FRAMES == 0) {
            float level = rms_dbfs(buf, WAKE_PT_PER_FRM);
            float score = wake_word_last_probability();
            printf("[wake] mic %.1f dBFS, score %.3f\n", level, score);
        }

        if (detected) {
            g_t_wake_ms = now_ms();
            printf("[wake] *** DETECTED! \"Luna\" ***\n");
            wake_word_reset();
            return 0;
        }
        frameCount++;
    }
    return 1;  /* g_flow_running=0, 退出 */
}

/*
 * 唤醒→对话主循环(用户用 exec /bin/my_wake flow 调用):
 *   1. 初始化采集器 + 播放 + 唤醒词(一次性)
 *   2. wake_listen_loop 等 "Luna"(采集器此时喂唤醒词)
 *   3. 检测到 → 切换到对话: run_dialog_session(Luna 主动开口 + 正常对话 + 空闲超时)
 *   4. 对话结束(超时/断开) → 回 2 继续监听(采集器复用, 无断裂)
 *   5. Ctrl+C 退出整个流程
 * gate_dbfs: energy gate 阈值, 由命令行传入。
 */
int RunWakeFlow(float gate_dbfs, unsigned int mic_vol)
{
    printf("[flow] === wake -> dialog flow (gate %.1f dBFS, mic_vol %u) ===\n",
           (double)gate_dbfs, mic_vol);

    /* 1. WiFi(假定已连, 幂等) */
    char ip[16] = {0};
    printf("[flow] wifi...\n");
    if (wifi_connect_ap(DIALOG_WIFI_SSID, DIALOG_WIFI_PSK) != 0) {
        printf("[flow] wifi_connect_ap FAILED\n");
        return 1;
    }
    if (wifi_wait_connected(20, ip, sizeof(ip)) != 0) {
        printf("[flow] wifi_wait_connected FAILED\n");
        return 1;
    }
    printf("[flow] wifi ok, ip=%s\n", ip);

    /* 2. 一次性 init: 采集 + 播放 + 唤醒词 */
    printf("[flow] capture + playback + wake_word init...\n");
    if (WakeCaptureInit() != 0) {
        printf("[flow] capture init FAILED\n");
        return 1;
    }
    if (WakePlaybackInit() != 0) {
        printf("[flow] playback init FAILED\n");
        WakeCaptureDeinit();
        return 1;
    }
    if (wake_word_init() != 0) {
        printf("[flow] wake_word init FAILED\n");
        WakePlaybackDeinit();
        WakeCaptureDeinit();
        return 1;
    }
    printf("[flow] init OK, say \"Luna\" to start a conversation\n");

    /* 3. 主循环: 唤醒监听 ↔ 对话会话 */
    while (g_flow_running) {
        /* 等唤醒词(采集器此时归唤醒监听) */
        if (wake_listen_loop() != 0) {
            break;  /* g_flow_running=0 */
        }

        /* 进入对话(采集器归对话上传; Luna 主动开口; 30s 空闲回唤醒) */
        printf("[flow] entering dialog...\n");
        run_dialog_session(gate_dbfs, mic_vol);
        printf("[flow] dialog ended, back to wake listen\n");
        /* 恢复唤醒校准增益(对话可能调低了), 保证唤醒灵敏度 */
        WakeCaptureSetMicVol(WakeCaptureDefaultMicVol());
        /* 对话期间采集器被 send_thread 独占, wake_word 未被喂新数据,
         * 但其 sliding window / pending features / frontend 滤波器状态残留着
         * 唤醒前的特征 → 回监听后第一个声音(如"你好")易误触发(score 0.974)。
         * reset 清空滑动窗口和 frontend, 重新冷启动检测。 */
        wake_word_reset();
        printf("[flow] wake_word reset, ready to listen\n");
    }

    /* 4. 清理 */
    WakePlaybackDeinit();
    WakeCaptureDeinit();
    printf("[flow] done\n");
    return 0;
}

/* 兼容旧接口: 独立对话(步骤4, 不带唤醒)。初始化→对话→清理 */
int RunDialog(float gate_dbfs, unsigned int mic_vol)
{
    g_gate_dbfs = gate_dbfs;
    printf("[dialog] === step 4: end-to-end realtime dialog (gate %.1f dBFS, mic_vol %u) ===\n",
           (double)gate_dbfs, mic_vol);

    char ip[16] = {0};
    printf("[dialog] [1/4] wifi...\n");
    if (wifi_connect_ap(DIALOG_WIFI_SSID, DIALOG_WIFI_PSK) != 0) {
        printf("[dialog] wifi_connect_ap FAILED\n");
        return 1;
    }
    if (wifi_wait_connected(20, ip, sizeof(ip)) != 0) {
        printf("[dialog] wifi_wait_connected FAILED (hotspot not connected?)\n");
        return 1;
    }
    printf("[dialog] [1/4] wifi ok, ip=%s\n", ip);

    printf("[dialog] [4/4] capture + playback init...\n");
    if (WakeCaptureInit() != 0) {
        printf("[dialog] capture init FAILED\n");
        return 1;
    }
    if (WakePlaybackInit() != 0) {
        printf("[dialog] playback init FAILED\n");
        WakeCaptureDeinit();
        return 1;
    }

    run_dialog_session(gate_dbfs, mic_vol);

    play_queue_clear();
    WakePlaybackDeinit();
    WakeCaptureDeinit();
    wake_ws_close();
    printf("[dialog] done\n");
    return 0;
}
