# Luna 语音机器人 —— 实现详解(当前代码版)

> **这份文档是给谁看的**:从未接触过这个项目的人,看完能理解"喊 Luna → 机器人回复 → 对话"完整链路怎么跑的,出了问题知道去哪查。
> **和 SPEC_DIALOG.md 的区别**:SPEC 是开发初期的设计文档,部分描述已过时;本文档严格按**当前代码**编写,以代码为唯一事实来源。
> **更新日期**:2026-09-24

---

## 目录

1. [一句话理解整个系统](#1-一句话理解整个系统)
2. [硬件与软件环境](#2-硬件与软件环境)
3. [整体架构:两条主线](#3-整体架构两条主线)
4. [主线一:喊 Luna 唤醒](#4-主线一喊-luna-唤醒)
5. [主线二:对话会话(三线程)](#5-主线二对话会话三线程)
6. [机器怎么"主动开口"](#6-机器怎么主动开口)
7. [回声抑制:为什么不插话](#7-回声抑制为什么不插话)
8. [Function Calling:让机器人做动作](#8-function-calling让机器人做动作)
9. [文件清单:每个文件干什么](#9-文件清单每个文件干什么)
10. [关键参数速查表](#10-关键参数速查表)
11. [和云端说什么:协议层](#11-和云端说什么协议层)
12. [常见问题定位手册](#12-常见问题定位手册)

---

## 1. 一句话理解整个系统

```
你喊 "Luna" → 板子上的小模型识别出来 → 连上阿里云 → Luna 主动说"你好"
            → 你说话,云转成回复语音,喇叭播放 → 空闲 10 秒回唤醒监听
```

板子只负责:**听(麦克风采集)**、**认唤醒词(本地小模型)**、**说(喇叭播放)**。听懂你说什么、想回复什么,全部由阿里云的大模型(Qwen-Audio)完成。板子和云端之间只传 PCM 音频流,不传文本。

---

## 2. 硬件与软件环境

| 项 | 值 |
|----|-----|
| 开发板 | Hi3516DV300(HiSpark Taurus) |
| 操作系统 | OpenHarmony LiteOS-A(小型系统) |
| 网络 | WiFi(连手机热点,SSID `iPhone`) |
| 云端 | 阿里云 Qwen-Audio-3.1-Realtime-Plus(实时语音对话大模型) |
| 唤醒 | 本地运行 microWakeWord 的 TFLM 模型,唤醒词 "Luna" |
| 外设 | 板载麦克风 + 喇叭;预留舵机(目前是空实现) |

**为什么选这个组合**:LiteOS-A 小型系统资源有限,装不下大模型,所以"听懂+回复"放云端,板子只做轻量的唤醒词识别 + 音频搬运。

---

## 3. 整体架构:两条主线

程序入口是 `exec /bin/my_wake flow`(可选参数 `-18` 是回声抑制阈值)。`flow` 命令做两件事,循环执行:

```
┌─────────────────────────────────────────────────────────┐
│  RunWakeFlow(主循环)                                    │
│                                                          │
│  ① 一次性初始化:WiFi + 麦克风 + 喇叭 + 唤醒模型          │
│                                                          │
│  while 运行中:                                           │
│  ┌──────────────────────────────────────────────┐       │
│  │ 【主线一:唤醒监听】                            │       │
│  │  不停采集麦克风 → 喂唤醒模型 → 等用户喊 Luna    │       │
│  │  喊到了 → 进入主线二                           │       │
│  └──────────────────────────────────────────────┘       │
│  ┌──────────────────────────────────────────────┐       │
│  │ 【主线二:对话会话】                            │       │
│  │  连云端 → Luna 主动开口 → 三线程收发对话       │       │
│  │  空闲 10 秒或断开 → 回主线一                    │       │
│  └──────────────────────────────────────────────┘       │
│  清理唤醒模型状态(防误触发),回循环开头                  │
└─────────────────────────────────────────────────────────┘
```

**关键设计**:麦克风和喇叭在两条主线之间**复用,不反复初始化**,所以从对话切回唤醒监听时声音不会断。

代码入口:`wake_dialog.c` 的 `RunWakeFlow`(约第 800 行)。

---

## 4. 主线一:喊 Luna 唤醒

### 4.1 这条线干什么

采集麦克风声音 → 喂给本地小模型 → 模型判断"这声音像不像 Luna" → 像就触发,进入对话。

### 4.2 唤醒词怎么识别出来的(`wake_word.cc`)

这是个**流式(streaming)检测器**,工作流程像流水线:

```
麦克风 PCM(16kHz,每 10ms 160 个采样点)
   │
   ▼ ① 提取声音特征(microfrontend)
   │   把声音转成"频谱图":30ms 一窗、10ms 一步、40 个频率通道
   │   做降噪 + 增益 + 取对数(模拟人耳听觉)
   │   得到一组数字(uint16)
   │
   ▼ ② 量化成模型能吃的格式
   │   uint16 × 0.0390625 → 转成 int8(模型训练时定的)
   │
   ▼ ③ 攒够 3 个 10ms 切片(=30ms)喂一次模型
   │   TFLM(TensorFlow Lite Micro)在板子上软推理
   │   输出一个 0~1 的概率值
   │
   ▼ ④ 滑动窗口平滑
   │   最近 5 次推理的概率求平均(避免单次抖动误判)
   │
   ▼ ⑤ 判定
       平均概率 > 0.98 → 触发!进入 1.5 秒不应期(防连触)
```

### 4.3 关键阈值(都在 `wake_word.cc` 开头)

| 参数 | 值 | 含义 | 调大的影响 |
|------|-----|------|-----------|
| `kProbabilityCutoff` | **0.98** | 概率超过多少算"喊了 Luna" | 更难唤醒(漏唤醒多) |
| `kSlidingWindow` | 5 | 平滑用最近几次推理 | 反应慢但更稳 |
| `kRefractoryUs` | 1.5 秒 | 触发后多久不再触发 | 防一句话触发多次 |

> **调参血泪史**(代码注释里记着):0.80 / 0.88 会把 "tuna"、"lunar"、"Tina"、"luba" 误判成 Luna;0.95 还会被 "luba" 触发;0.98 是当前权衡值,但仍有 18% 左右漏唤醒。误唤醒靠调阈值已经到头,要彻底解决得回训练集加"难负例"(hard negatives)。

### 4.4 唤醒监听的预热(`wake_listen_loop`,约第 740 行)

刚进入唤醒监听(尤其从对话切回来时),喇叭的余响、回声还没消停,这时喂给模型容易误触发(比如把回声里的"你好"当成 Luna)。所以:

- **丢掉前 33 帧(≈1 秒)**:不喂模型,让环境稳定
- 每秒打印 `mic X.X dBFS, score 0.XXX`,方便看电平和实时得分

### 4.5 从对话切回唤醒时为什么还要 reset

`RunWakeFlow` 在对话结束后调 `wake_word_reset()`([wake_dialog.c 约 812 行](#))。原因:对话期间唤醒模型没被喂数据,但它内部的滑动窗口、滤波器状态还残留着唤醒前的特征,切回监听后第一个声音(如"你好")容易误触发。reset 清空这些状态,让模型重新冷启动。

---

## 5. 主线二:对话会话(三线程)

这是系统的核心。进入对话后,起**三个线程**协同工作,模拟官方 Python demo 的三个异步任务。

### 5.1 为什么必须三个线程

如果单线程串行做"采集一帧(30ms)+ 发送 + 收回复 + 播放一帧(30ms)",一个周期要 60ms+,跟不上实时(每秒要处理 33 帧)。所以必须**并行**:

```
       ┌──────────────┐  麦克风音频(base64 编码)
麦克风 │  send 线程   │ ──────────────────────► 云端
  ▲    └──────────────┘                          │
  │                                              │ 云端 VAD
  │    ┌──────────────┐  云端回复(WS 帧)         │ 判断说话
  │    │  recv 线程   │ ◄────────────────────────┤
  │    └──────────────┘                          │
  │         │ 解码后 PCM 入播放队列                │
  │         ▼                                    │
  │    ┌──────────────┐  播放队列 PCM
喇叭 │  play 线程   │ ◄── 出队播放
  ▼    └──────────────┘
```

### 5.2 send 线程(上传麦克风,约第 237 行)

职责:不停采集麦克风 → base64 编码 → 发给云端。

核心循环(简化):
```
等问候播放完(g_greeting_played)才开始上传  ← 防止环境声打断问候
loop:
  采集一帧(480 点 / 960 字节 / 30ms)
  if AI 正在生成回复 OR 正在播放:
      丢弃这帧,不上传  ← 回声抑制(见第 7 节)
  if 还在冷却期(播放后 1 秒内):
      丢弃这帧,不上传  ← 滤喇叭余响
  else:
      base64 编码 → 拼 JSON → ws 发送
```

**它上传的事件**:`input_audio_buffer.append`(每 30ms 一次,最频繁)。

### 5.3 recv 线程(收云端回复,约第 400 行)

职责:非阻塞轮询收云端事件,按事件类型分发。

收到事件后的处理(看 `type` 字段):

| 收到事件 | 干什么 |
|---------|--------|
| `input_audio_buffer.speech_started` | 用户开始说话 → **打断**:清播放队列 + 标记暂停播放 + 取消云端回复 |
| `response.created` | 云端开始生成回复 → 标记"回复中" |
| `response.audio.delta` | 一段 24kHz PCM → base64 解码 → 塞播放队列 |
| `response.done` | 本轮回复生成完 → 入哨兵节点(通知 play 线程本轮结束) |
| `response.function_call_arguments.done` | 模型要调工具 → 执行 Function Calling(第 8 节) |
| `...input_audio_transcription.completed` | 你说的话的文本 → 打印 `[You] xxx` |
| `response.audio_transcript.done` | AI 回复的文本 → 打印 `[AI] xxx` |
| `error` | 打印(忽略无害的 "no active response") |

**空闲超时**:连续 10 秒没收到任何事件 → 主动断开,回唤醒监听。用非阻塞轮询(每 100ms 一次)就是为了能做这个超时判断。

### 5.4 play 线程(播放 AI 语音,约第 523 行)

职责:从播放队列取数据 → 喇叭播放。

```
loop:
  从队列取一个节点(队列空就阻塞等待)
  if 是哨兵节点(标志本轮播放完):
      标记不在播放 + 设冷却(触发 send 线程 1 秒静默)
      第一个哨兵 = 问候播完 → 放行 send 线程开始上传
  if 处于打断暂停状态:
      丢弃这帧(残余音频)
  else:
      标记正在播放 → 喇叭写出
```

### 5.5 播放队列(`wake_dialog.c`)

- 结构:单链表,每个节点存一段解码后的 PCM
- 哨兵节点(len = -1):由 `response.done` 入队,标志"这一整轮回复播完了"
- 上限 2MB:防 TTS 生成比实时快导致堆积,长句不丢后半段
- 用 mutex + cond 保护(send/recv/play 三线程都碰它)

### 5.6 三线程怎么退出

recv 线程因**空闲超时**或 **WS 断开**退出 → 主线程设全局退出标志 + 唤醒等待的线程 → join 三个线程 → 清队列 + 关 WS(**麦克风/喇叭不关,复用**)。

---

## 6. 机器怎么"主动开口"

进入对话后,Luna 不是等你说话,而是先主动打招呼。这套时序在 `run_dialog_session`(约第 570 行),**每步都要等云端确认,不能连发**:

```
1. WebSocket 建连(重试 3 次)
   TCP → TLS 握手 → WebSocket 升级
   等 session.created(确认连上了)

2. 发 session.update(配置会话)
   告诉云端:音色、VAD 参数、人设指令、注册了哪些工具
   等 session.updated 确认

3. 注入一条假对话:conversation.item.create("你好")
   等 conversation.item.created 确认
   ★ 关键:不等确认直接发 response.create 的话,
     云端会静默不回复(没报错也没回复),排查极费时间

4. 发 response.create(触发生成回复)
   云端开始生成 Luna 的问候语音

5. 起三线程,正式进入收发对话
```

### 6.1 session.update 里配了什么(约第 40 行)

```json
{
  "voice": "longanqian_v3.1",          // 音色:龙干浅(甜美)
  "instructions": "你叫Luna,是个装在机器人上的语音助手...",  // 人设
  "turn_detection": {                  // 云端 VAD 自动检测说话
    "type": "server_vad",
    "threshold": 0.5,
    "silence_duration_ms": 800
  },
  "tools": [ ... control_robot ... ]  // 注册 Function Calling 工具
}
```

`instructions` 是你自己写的业务文本,定义 Luna 的性格和能做什么;协议只规定字段名,内容随便填。

---

## 7. 回声抑制:为什么不插话

> ⚠️ **这是当前实现最重要的权衡,也是 SPEC 文档和代码不符的地方**。SPEC_DIALOG.md 4.1 说的是"按能量过滤、支持插话打断",但代码实际不是这样。

### 7.1 问题:回声导致自言自语

没有硬件回声消除(AEC)的板子上,喇叭播 Luna 的声音会被麦克风录回去。云端 VAD 收到这"回声",以为是用户在说话,于是:

```
Luna 说话 → 喇叭响 → 麦克风采到回声 → 云端以为用户插话
→ 发 speech_started → 打断 Luna → Luna 没说完
→ 残余回声又触发 → Luna 自言自语循环
```

### 7.2 当前方案:AI 说话时直接不传音频

send 线程([wake_dialog.c 约 289-300 行](#)):
```
if (AI 正在生成回复 OR 正在播放):
    丢弃麦克风帧,不上传
```

**原理**:云端 VAD 只能看客户端上传的音频。只要 AI 说话时不传,云端就收不到回声,也就不会误判用户插话。

**代价**:AI 说话时你插话**打不断**(因为没传音频,云端不知道你在说话)。必须等 Luna 说完才能接话。

> 代码注释原话:"代价:不支持打断(AI 说话时插不进话),需等 AI 说完。无耳机场景这是最稳方案。"

### 7.3 播放后的冷却

Luna 说完后,喇叭还有余响、回声拖尾,这时马上传音频又会被误判。所以播放结束后再**冷却 1 秒(33 帧)**不上传:

```
播放结束(哨兵节点) → g_echo_cooldown = 1
  → send 线程检测到 → 进入 33 帧冷却(≈1 秒不上传)
  → 冷却结束才恢复正常上传
```

### 7.4 问候期间的静默

进入对话时,Luna 先说问候。这时 send 线程**等问候播完**(`g_greeting_played` 标志)才开始上传,8 秒超时兜底。否则问候期间传的环境声会被云端误判打断问候。

### 7.5 唯一的"打断"入口:speech_started

虽然 AI 说话时打不断,但 `speech_started` 这套打断代码仍然在(约第 453 行),只是实际只在"轮到用户说话时"才触发,那时往往没东西可打断。完整链路见第 11 节。

---

## 8. Function Calling:让机器人做动作

Luna 不只能说话,还能"做动作"——通过 Function Calling 让模型决定调用本地工具。

### 8.1 触发条件

在 session.update 里注册了 `control_robot` 工具(约第 46 行),描述是"控制机器人舵机执行肢体动作,跳舞/挥手/点头时调用"。模型理解你的意图后,会主动发起函数调用。

### 8.2 完整链路(`handle_function_call`,约第 360 行)

```
你说:"Luna 跳个舞"
   ↓
云端模型识别意图 → 发 response.function_call_arguments.done
   (含 call_id / name="control_robot" / arguments={"action":"dance"})
   ↓
recv 线程收到 → handle_function_call():
   ① 从 arguments 里抠出 action="dance"
   ② 调 servo_run_action("dance")  ← 启动舵机(目前是空实现)
   ③ 发 conversation.item.create 写回执行结果
      {"type":"function_call_output","call_id":"xxx",
       "output":"{\"status\":\"started\",\"action\":\"dance\"}"}
   ④ 发 response.create 触发二轮推理
      → 模型基于"动作已启动"生成语音:"好的,我跳个舞~"
      → 这段语音走正常 audio.delta → 播放队列 → 喇叭
```

### 8.3 舵机控制(`servo_ctl.c`)——目前是空壳

```c
int servo_run_action(const char *action)
{
    printf("[servo] >>> run_action \"%s\" started (stub, no real servo)\n", action);
    return 0;   // 假装成功
}
```

当前只打印日志,不驱动真实硬件。等同事提供 PWM/GPIO 实现后,替换这个文件即可,**接口契约不变**(详见 `servo_ctl.h`)。

### 8.4 硬性约束:舵机函数必须非阻塞

`servo_run_action` 在 recv 线程里调用([servo_ctl.h](#) 强调),如果它阻塞(比如等动作做完才返回),会卡死 recv 线程,导致收不到云端事件、打断失效。所以必须"启动动作立即返回,动作在后台线程/定时器完成"。

---

## 9. 文件清单:每个文件干什么

```
wake/
├── wake_main.cc          # 程序入口 + 命令分发(flow/dialog/duplex/wstest/sendtest)
├── wake_capture.c/h      # 麦克风采集(16kHz/16bit/单声道,直调 HiSilicon AI 驱动)
├── wake_playback.c/h     # 喇叭播放(24kHz→16kHz 硬件重采样,直调 AO 驱动)
├── wake_word.cc/h        # 唤醒词识别(TFLM 流式模型 + microfrontend)
├── wake_word_model_data.c/h  # Luna 唤醒词模型权重(大文件)
├── wake_ws.c/h           # WebSocket 客户端(socket→TLS→WS 帧,全自封装)
├── wake_dialog.c/h       # ★核心:三线程状态机 + 回声抑制 + FC + 主循环
├── wake_base64.c/h       # base64 编解码(自实现,无第三方库)
├── servo_ctl.c/h         # 舵机控制(空壳,等真实实现)
├── tflm/                 # TensorFlow Lite Micro 源码(第三方)
├── microfrontend/        # 音频特征提取前端(第三方)
├── BUILD.gn              # 构建配置(-ldl + mbedtls_shared)
├── SPEC.md               # 旧设计文档(唤醒阶段)
├── SPEC_DIALOG.md        # 旧设计文档(对话阶段,部分已过时)
└── IMPLEMENTATION.md     # 本文档(当前代码实现版)
```

### 各文件详细职责

| 文件 | 职责 | 依赖 |
|------|------|------|
| `wake_main.cc` | 解析命令行参数,调对应子命令 | 全部模块 |
| `wake_capture.c` | 麦克风 PCM 采集,不依赖任何后台服务 | HiSilicon MPP 库 |
| `wake_playback.c` | 喇叭 PCM 播放 + 24k→16k 重采样 | HiSilicon MPP 库 + libhive_RES.so |
| `wake_word.cc` | "Luna" 唤醒词流式检测 | TFLM + microfrontend |
| `wake_ws.c` | 和云端建立加密连接、收发 WS 帧 | mbedTLS |
| `wake_dialog.c` | 对话全部逻辑(三线程/队列/回声/FC) | 上面所有模块 |
| `wake_base64.c` | PCM ↔ base64 转换 | 无 |
| `servo_ctl.c` | 舵机动作(占位) | 无 |

---

## 10. 关键参数速查表

### 10.1 唤醒相关(`wake_word.cc`)

| 参数 | 值 | 含义 |
|------|-----|------|
| `kProbabilityCutoff` | 0.98 | 概率超过此值才算喊了 Luna |
| `kSlidingWindow` | 5 | 平滑窗口(最近 5 次推理平均) |
| `kRefractoryUs` | 1.5 秒 | 触发后不应期 |
| `kFeatureSlicesPerInvoke` | 3 | 每次推理吃 3 个 10ms 切片 |

### 10.2 采集/播放(`wake_capture.h` / `wake_playback.h`)

| 参数 | 值 | 含义 |
|------|-----|------|
| `WAKE_MIC_VOL` | 60 | 麦克风增益(实测最佳) |
| `WAKE_SAMPLE_RATE` | 16000 Hz | 采集采样率 |
| `WAKE_PT_PER_FRM` | 480 | 每帧 480 点 = 30ms |
| `PLAY_SAMPLE_RATE` | 24000 Hz | 云端输出采样率(应用送入) |
| `PLAY_HW_SAMPLE_RATE` | 16000 Hz | 硬件实际播放采样率 |

### 10.3 对话相关(`wake_dialog.c`)

| 参数 | 值 | 含义 |
|------|-----|------|
| `DIALOG_COOLDOWN_FRAMES` | 33(≈1 秒) | 播放后冷却静音帧数 |
| `DIALOG_IDLE_TIMEOUT_MS` | 10000 | 对话空闲超时(回唤醒) |
| `PLAY_QUEUE_MAX_BYTES` | 2 MB | 播放队列上限 |
| `ENERGY_GATE_DBFS_DEFAULT` | -18.0 | 回声抑制阈值(命令行可覆盖) |
| `WAKE_WARMUP_FRAMES` | 33(≈1 秒) | 回唤醒后丢弃帧数 |

### 10.4 网络相关(`wake_ws.h`)

| 参数 | 值 | 含义 |
|------|-----|------|
| `QWEN_WS_IP` | 47.94.20.201 | 云端 IP(DNS 不可用,预解析) |
| `QWEN_WS_HOST` | ws-qq7tg8jxbjmnqaty... | 云端域名(SNI/Host) |
| `QWEN_WS_PORT` | 443 | 端口 |
| `WS_RX_BUF_SIZE` | 32768 | WS 接收缓冲 |

### 10.5 凭据(写死在代码里)

| 项 | 值 |
|----|-----|
| WiFi SSID | `iPhone` |
| WiFi PSK | `fx151231` |
| Qwen API Key | `sk-ws-H.PIPHLHL...`(在 `wake_main.cc` / `wake_dialog.c`) |

---

## 11. 和云端说什么:协议层

### 11.1 发送接口:只有一条路

应用层所有发给云端的内容,都只通过一个函数:

```c
wake_ws_send_text(json_string, length);   // wake_ws.c:464
```

调用链(从上到下):
```
wake_ws_send_text(json, len)        ← 应用层唯一入口
  → ws_send_frame(TEXT, data, len)  ← 按 RFC 6455 封 WS 帧(加掩码)
    → tls_write_all(frame, total)   ← mbedTLS 加密 + 重试循环
      → mbedtls_ssl_write           ← TLS 记录层加密
        → TCP socket write           → 47.94.20.201:443
```

### 11.2 上行发的事件清单

| 事件 | 何时发 | 位置 |
|------|--------|------|
| `session.update` | 建连后配置 | 约第 627 行 |
| `conversation.item.create`("你好") | 让 Luna 主动开口 | 约第 666 行 |
| `response.create` | 触发生成回复 | 约第 693 行 |
| `input_audio_buffer.append` | 每 30ms 上传麦克风 PCM | 约第 337 行(最频繁) |
| `response.cancel` | 打断时取消回复 | 约第 464 行 |
| `conversation.item.create`(FC output) | Function Calling 写回结果 | 约第 394 行 |
| `response.create`(FC 二轮) | 触发二轮推理 | 约第 401 行 |

### 11.3 协议层:哪些自封装,哪些遵循官方

| 层 | 是否自封装 | 说明 |
|----|-----------|------|
| 事件 JSON 结构 | ❌ 遵循官方 | type 名、字段名都是阿里云规定的,改一个字云端不认 |
| WebSocket 帧层 | ✅ 自封装 | 按 RFC 6455 手写(无 libwebsockets) |
| TLS 握手 | 半自封装 | 用 mbedTLS 库,但重试逻辑自写 |
| JSON 解析 | ✅ 自封装 | 手写 `json_get_str`,不用 cJSON |
| base64 编解码 | ✅ 自封装 | 自实现 RFC 4648 |
| PCM 采集/播放 | ✅ 自封装 | 直调 HiSilicon 驱动,不依赖 media_server |
| session.update 业务内容 | ✅ 自己写 | instructions/tools 是你的业务文本 |

### 11.4 JSON 解析的局限

没引 cJSON,手写的 `json_get_str`(约第 185 行)只能提取字符串字段,且**字段值里不能含双引号**。所以:

- Function Calling 的 `arguments` 是个 JSON 字符串,要**二次**用 `json_get_str` 抠里面的 `action`
- 写回 `function_call_output` 时,内层 JSON 的双引号要手动 `\"` 转义

---

## 12. 常见问题定位手册

### 12.1 喊 Luna 不响应 / 漏唤醒

| 查什么 | 怎么查 |
|--------|--------|
| 概率阈值是否过高 | 看 `kProbabilityCutoff=0.98`,每秒打印的 `score` 够不够 |
| warmup 丢帧 | 进入唤醒监听前 1 秒(33 帧)丢弃,别在这期间喊 |
| 麦克风电平 | `mic X.X dBFS` 是否正常(应 > -30dBFS) |

### 12.2 误唤醒(tuna/Tina/说"你好"就触发)

| 查什么 | 怎么查 |
|--------|--------|
| 阈值调参史 | 看 `wake_word.cc` 第 67-90 行注释 |
| 对话结束没 reset | `RunWakeFlow` 对话后是否调了 `wake_word_reset()` |
| warmup 不够 | 回唤醒后 1 秒内别急着判断 |

### 12.3 AI 不开口 / 没声音

| 查什么 | 怎么查 |
|--------|--------|
| 注入"你好"后等没等到 ack | `drain(inject)` 是否收到 `conversation.item.created` |
| 问候被环境声打断 | `g_greeting_played` 是否置位;send 线程问候等待日志 |
| response.create 发了没 | `response.create send=N` 日志 |

### 12.4 AI 说话时插话打不断

**这是预期行为,不是 bug**。当前回声抑制策略([wake_dialog.c 约 289 行](#))在 AI 说话时不传音频,云端收不到你的声音,自然不发 `speech_started`。要恢复打断能力需改回"始终上传+能量过滤",但会重新引入回声自言自语。

### 12.5 AI 自言自语

| 查什么 | 怎么查 |
|--------|--------|
| 冷却期够不够 | `DIALOG_COOLDOWN_FRAMES=33`(~1 秒)够不够挡余响 |
| 冷却是否真生效 | `[send] cooldown triggered` 和 `cooldown ended: skipped N frames` 日志 |
| 噪声底噪 | 冷却结束首个上传帧的 `mic=X.X dBFS` 是否过高 |

### 12.6 长句后半段丢失

播放队列满丢 delta。看日志 `[dialog] queue full`,考虑调大 `PLAY_QUEUE_MAX_BYTES`。

### 12.7 对话很快断开回唤醒

`DIALOG_IDLE_TIMEOUT_MS=10000`,如果云端长时间没事件(比如思考久),会被当成空闲超时。看 `idle 10s, back to wake listen` 日志。

### 12.8 WS 连不上

| 查什么 | 怎么查 |
|--------|--------|
| WiFi | `wifi connected, ip=...` 是否拿到 IP |
| IP 是否变了 | `47.94.20.201` 是预解析的,云端 IP 变了要更新 |
| TLS 握手 | `[ws] TLS handshake OK` 有没有 |
| 重试 | 建连重试 3 次,看 `ws connect try N failed` |

### 12.9 EnableReSmp 失败(播放初始化失败)

`HI_MPI_AO_EnableReSmp` 内部 dlopen 找不到 `.so`。看 `[my_wake] play: preload ... FAILED` 日志。根因和解法见 memory `mpp-dlopen-runtime-path.md`:在 `PreloadResampleLibs` 按绝对路径预加载 `/usr/lib/libsecurec.so` + `/usr/lib/libhive_RES.so`。

### 12.10 Function Calling 动作无响应

| 查什么 | 怎么查 |
|--------|--------|
| 模型有没有发起调用 | `[FC] call control_robot, ...` 日志 |
| 舵机函数是否阻塞 | `servo_run_action` 必须立即返回(契约见 servo_ctl.h) |
| 二轮 response.create 发了没 | `[FC] response.create sent` 日志 |

---

## 附:快速启动

```bash
# 确保板子已联网(WiFi 在 flow 里会自动连)
exec /bin/my_wake flow        # 唤醒→对话主循环(默认回声阈值 -18)
exec /bin/my_wake flow -15    # 自定义回声阈值(dBFS,越负越松)

# 调试子命令
exec /bin/my_wake             # 只检测唤醒,不对话
exec /bin/my_wake dialog      # 直接进对话(不经唤醒)
exec /bin/my_wake duplex      # AI+AO 双工验证
exec /bin/my_wake wstest      # WebSocket 连接验证
exec /bin/my_wake sendtest    # 单向上传验证
```

**预期现象**:喊" Luna",串口打印 `*** DETECTED ***`,接着 Luna 主动开口打招呼,然后可以正常对话;停顿 10 秒后自动回唤醒监听。
