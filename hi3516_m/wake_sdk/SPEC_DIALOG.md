# 实时语音对话规格说明（实现版）

> **项目**：Hi3516DV300 (HiSpark Taurus) OpenHarmony LiteOS-A 小型系统
> **目标**：唤醒词 "Luna" 触发后，连接阿里云 Qwen-Audio-Realtime，实现端云实时语音对话
> **状态**：步骤 1-4 全部验证通过，端到端实时对话可用。下一步：Function Calling
> **更新**：2026-09-22

---

## 1. 背景与目标

### 1.1 现状

- **语音唤醒已完成**：`my_wake`（wake 目录）已验证可用。说 "Luna" 触发检测，score 0.987-0.993。16kHz/16bit/单声道 PCM 采集，TFLM 软推理，不依赖 media_server/ai_server。
- **录音参数已对齐**：wake_capture 采集格式为 16kHz/16bit/单声道 PCM——正好是 Qwen-Audio-Realtime 的输入格式，零转换。
- **端到端对话已跑通**：喊话 → 上传 PCM → 云端 server_vad → 收 AI 语音 delta → 24kHz 播放 → 回声抑制 + 插话打断，全链路工作。

### 1.2 目标

在 wake 目录内（**不复用 audio 目录代码**，可参考）实现：

1. 喊 "Luna" 唤醒后，自动连接阿里云 Qwen-Audio-Realtime
2. 持续上传 16kHz PCM 给云端（server_vad 模式）
3. 接收云端 24kHz PCM 回复，通过喇叭实时播放
4. 软件回声抑制 + 语音打断（AI 说话时用户插话能打断）
5. 对话超时或异常断开后回到唤醒监听状态

### 1.3 非目标

- 不复用 my_audio 代码（参考 audio_pcm.c 的 AO 实现但不依赖）
- 不做硬件 AEC（本板无硬件回声消除，用软件 energy gate）
- 不做 TTS/ASR（云端全包，我们只管 PCM 收发）
- 不做 UI（串口打印状态即可）
- 不做 WiFi 联网（同事已把板子联网，我们假设网络可达）

---

## 2. 阿里云 Qwen-Audio-Realtime 协议要点

### 2.1 连接

- **URL**：`wss://ws-qq7tg8jxbjmnqaty.cn-beijing.maas.aliyuncs.com/api-ws/v1/realtime?model=qwen-audio-3.1-realtime-plus`
- **域名 IP**（板上 DNS 不可用，预解析）：`47.94.20.201`
- **Host/SNI**：`ws-qq7tg8jxbjmnqaty.cn-beijing.maas.aliyuncs.com`
- **认证**：HTTP Header `Authorization: Bearer {DASHSCOPE_API_KEY}`
- **传输**：WebSocket over TLS（wss://），TLS 握手 + HTTP Upgrade + WebSocket 帧层
- **证书验证跳过**：`MBEDTLS_SSL_VERIFY_NONE`（不打包 cacert.pem）

### 2.2 音频格式

| 方向 | 格式 | 规格 |
|------|------|------|
| 输入（板子→云） | PCM | 16kHz / 16bit / 单声道 |
| 输出（云→板子） | PCM | 24kHz / 16bit / 单声道 |

输入格式与 wake_capture 完全一致，零转换。输出 24kHz 经 `HI_MPI_AO_EnableReSmp(24000)` 重采样到 16kHz 硬件播放（ACodec I2S1 采样率全局唯一，AI/AO 共享 16kHz）。

### 2.3 事件交互（server_vad 模式）

**客户端→服务端**：
| 事件 | 用途 | 时机 |
|------|------|------|
| `session.update` | 配置会话（音色/VAD/工具） | 建连后立即发，首次发音频前 |
| `input_audio_buffer.append` | 上传一段 PCM（base64） | 每 30ms 一帧（960 字节） |
| `response.cancel` | 取消当前回复 | 打断时 |
| `conversation.item.create` | 写回 Function Calling 结果 | 工具执行后 |
| `response.create` | 触发二轮推理 | Function Calling 结果写回后 |

**服务端→客户端**（当前已处理）：
| 事件 | 含义 | 我们的动作 |
|------|------|-----------|
| `session.created` | 会话已建 | 标记连接成功 |
| `input_audio_buffer.speech_started` | 云端检测到你开始说话 | 打断：清播放队列 + audio_suppressed + response.cancel |
| `response.created` | 云端开始生成回复 | is_responding=true |
| `response.audio.delta` | 一段 24kHz PCM（base64，~25800 字节） | 解码 → 塞播放队列 |
| `response.done` | 回复生成完毕 | 入哨兵节点（通知 play 线程本轮结束） |
| `conversation.item.input_audio_transcription.completed` | 用户说的话的文本 | 串口打印 [You] |
| `response.audio_transcript.done` | AI 回复的文本 | 串口打印 [AI] |
| `error` | 错误 | 打印（"no active response" 无害，忽略） |

**Function Calling 新增事件（待处理）**：
| 事件 | 含义 | 我们的动作 |
|------|------|-----------|
| `response.function_call_arguments.done` | 模型决定调用工具，参数完整 | 解析 call_id/name/arguments → 执行函数 → 写回 output → 发 response.create |

---

## 3. 实际架构（三线程状态机）

### 3.1 线程模型

LiteOS-A 用户态 pthread 可用。三个线程 + 主线程：

| 线程 | 职责 | 阻塞点 |
|------|------|--------|
| **send 线程** | WakeCaptureRead(30ms) → energy gate → base64 → 发 input_audio_buffer.append | WakeCaptureRead |
| **recv 线程** | wake_ws_recv_text(阻塞) → 解析 JSON → 分发：audio.delta 入播放队列 / speech_started 打断 / 转写打印 / function_call 执行 | wake_ws_recv_text |
| **play 线程** | 出播放队列 → WakePlaybackWrite(30ms)；audio_suppressed 期间丢弃 | pthread_cond_wait + WakePlaybackWrite |
| **主线程** | 建连 + session.update + 起 3 线程 + join recv → g_running=0 → broadcast cond → join 其余 | pthread_join |

单线程不可行：AI GetFrame 和 AO SendFrame 各阻塞 30ms，串行 = 60ms/周期，跟不上实时。必须三线程。

### 3.2 共享状态（mutex 保护）

```c
static volatile int g_running = 1;          /* 0 = 全部线程退出 */
static volatile int g_is_responding = 0;    /* 云端正在生成回复 */
static volatile int g_audio_suppressed = 0; /* 打断后丢弃残余音频 */
static volatile int g_is_playing = 0;       /* 正在播放 AI 音频 */
static volatile int g_echo_cooldown = 0;    /* 播放结束后静音倒计时(滤回声余响) */
static volatile float g_gate_dbfs = -18.0f; /* energy gate 阈值(可配) */
```

- `g_mtx` + `g_cond` 保护播放队列（单链表，带哨兵节点 len=-1 表示本轮播放结束）
- 播放队列上限 `PLAY_QUEUE_MAX_BYTES = 2MB`（防 TTS 突发堆积，长句不丢后半段）

### 3.3 数据流

**上传方向**（16kHz）：
```
WakeCaptureRead(480pts/960B/30ms) ──base64──> JSON event ──ws_send()──> TLS ──> 云端
  ↑ AI 播放期间经 energy gate: 能量 < g_gate_dbfs 的帧不上传(滤回声)
```

**下行方向**（24kHz）：
```
云端 ──TLS──> ws_recv() ──> json_get_str 解析 type
  ──response.audio.delta──> base64解码 ──> 播放队列(malloc 链表节点)
  ──播放队列──> play_thread ──> WakePlaybackWrite(1440B/帧) ──> AO EnableReSmp(24k→16k) ──> 喇叭
```

### 3.4 JSON 解析（无 cJSON）

事件结构固定，手写 `json_get_str(json, key, out, cap)`：strstr 定位 `"key":"` 模式，拷贝到下一个 unescaped `"`。处理 UTF-8 中文（transcript 含中文但不含 `"`，安全）。不用 cJSON 避免动态库依赖。

---

## 4. 关键实现细节

### 4.1 回声抑制（energy gate，已调优）

无硬件 AEC 时，AI 说话的喇叭声会被麦克风录回，云端 VAD 误判为"用户说话"触发打断，AI 自言自语。

**方案：energy gate（始终上传 + 能量过滤）**
- AI 播放期间（`g_is_playing=true`），计算每帧 RMS 能量（dBFS）
- 能量 < `g_gate_dbfs` 的帧不上传（滤回声，回声比直接人声轻且失真）
- 能量 ≥ 阈值的帧上传（用户大声插话能打断）
- AI 播放结束后 `DIALOG_COOLDOWN_FRAMES = 10`（300ms）静音冷却，滤喇叭余响/回声拖尾

**最终实测参数（WAKE_MIC_VOL=60 下）**：
- `ENERGY_GATE_DBFS_DEFAULT = -18.0f`（实测最佳：挡住回声 + 插话顺畅）
- 阈值越接近 0 越紧（更抗回声但更难插话），越负越松（易插话但回声易泄漏）
- **命令行可配**：`exec /bin/my_wake dialog -18`（第二参数覆盖默认值）

> **重要**：改 WAKE_MIC_VOL 必须重新校准 gate——输入电平整体变化，回声电平和直接人声都会变。

### 4.2 打断处理

收到 `input_audio_buffer.speech_started`：
1. 清空播放队列（立即闭嘴）
2. `g_is_playing = 0`，`g_audio_suppressed = 1`，`g_echo_cooldown = 0`
3. 若 `g_is_responding`，发 `response.cancel`

收到下一个 `response.created` → `g_audio_suppressed = 0`（新回复正常播放）。

### 4.3 24kHz 播放通路（硬件重采样方案）

ACodec I2S1 采样率全局唯一，AI/AO 共享。**实际采用**：
- ACodec I2S1 = 16kHz（wake_capture 设定）
- AO 设备 `enSamplerate = AUDIO_SAMPLE_RATE_16000`（对齐 ACodec）
- `HI_MPI_AO_EnableReSmp(dev, chn, 24000)` 把应用送入的 24kHz PCM 重采样到 16kHz 播放
- `u32ClkSel = 0`：AI/AO 时钟分离，启用同步双工

**关键坑：EnableReSmp 运行时 dlopen**
- `HI_MPI_AO_EnableReSmp` 内部 `dlopen("libhive_RES.so")` + `dlopen("libsecurec.so")`（裸文件名）
- musl 默认仅搜 `/lib`，OHOS shell `execve(..., NULL)` 清环境变量，LD_LIBRARY_PATH 不传到 my_wake
- **修复**：WakePlaybackInit 中 EnableReSmp 之前，按绝对路径 `/usr/lib/libsecurec.so` + `/usr/lib/libhive_RES.so` 预 dlopen，`RTLD_GLOBAL`。musl 的"已加载"缓存按 shortname 匹配，后续裸名 dlopen 命中缓存。详见 memory `mpp-dlopen-runtime-path.md`。

### 4.4 TLS + WebSocket

- `mbedtls_net_connect` 用预解析 IP `47.94.20.201`，域名放 SNI/Host
- `MBEDTLS_SSL_VERIFY_NONE` 跳过证书验证（不打包 cacert.pem）
- WANT_READ/WANT_WRITE 重试循环
- 大缓冲 malloc（静态大数组触发 LiteOS-A ldso 段错位）

### 4.5 缓冲区大小（应对大 delta 帧）

`response.audio.delta` 单帧 ~25800 字节：
- `DIALOG_RECV_CAP = 65536`（接收缓冲，容纳 25800 字节 delta）
- `DIALOG_B64_CAP = 45000`（delta base64 解码缓冲）
- `DIALOG_PCM_CAP = 32000`（delta 解码后 PCM 缓冲）

### 4.6 采集常驻（方案B）

采集始终运行，唤醒只切换"是否上传"开关，声音无断裂。当前 dialog 子命令独立运行（采集在 WakeCaptureInit 启用，Deinit 关闭）；唤醒→对话流程合并待实现。

---

## 5. 命令接口

```
exec /bin/my_wake                  # 语音唤醒监听(说 "Luna" 唤醒)
exec /bin/my_wake duplex           # 步骤1:AI+AO 双工验证
exec /bin/my_wake wstest           # 步骤2:WebSocket 连接验证
exec /bin/my_wake sendtest         # 步骤3:发 PCM 收转写验证
exec /bin/my_wake dialog [-18]     # 步骤4:端到端实时对话(可选 energy gate dBFS, 默认 -18)
```

API Key 硬编码在 `wake_main.cc`（来自 `默认业务空间-apiKey-7372424.csv`，Qwen key，非同事的 SiliconFlow key）。
WiFi 凭据：SSID `iPhone`，PSK `fx151231`（热点）。

---

## 6. 文件布局与构建

### 6.1 目录结构

```
device/soc/hisilicon/common/platform/mytest/wake/
├── wake_main.cc          # 主程序 + 子命令分发 + duplex 测试
├── wake_capture.c/h      # 16kHz 采集(WAKE_MIC_VOL=60)
├── wake_word.cc/h        # TFLM 唤醒(Luna)
├── wake_word_model_data.c/h  # Luna 模型
├── wake_playback.c/h     # 24kHz AO 播放 + 预 dlopen 重采样库
├── wake_ws.c/h           # WebSocket 客户端(socket+mbedTLS+WS帧)
├── wake_dialog.c/h       # 三线程状态机 + 回声抑制/打断 + 播放队列
├── wake_base64.c/h       # base64 编解码
├── tflm/                 # TFLM 源码
├── microfrontend/        # 音频前端
├── BUILD.gn              # -ldl + mbedtls_shared
└── SPEC_DIALOG.md        # 本文档
```

### 6.2 BUILD.gn 关键项

- sources 含 `wake_dialog.c` 等
- `ldflags` 含 `-ldl`（dlopen 符号，链接期）
- `deps` 含 `//third_party/mbedtls:mbedtls_shared`
- 不依赖 cJSON（手写 JSON 提取）

---

## 7. 实施步骤（全部完成）

### 步骤 1：24kHz 播放通路验证 ✅
440Hz 正弦波 + mic 电平，AI+AO 双工工作。

### 步骤 2：TLS + WebSocket 连接验证 ✅
session.created 已接收。

### 步骤 3：单向上传验证 ✅
上传 PCM → 云端转写 → AI 回复 delta 全部收到。

### 步骤 4：端到端实时对话 ✅
多轮中文/英文对话，转录正确，AI 语音播放，打断工作，回声抑制工作。最终参数 vol=60 + gate=-18 dBFS。

---

## 8. 下一步：Function Calling

### 8.1 协议（官方文档）

模型可根据对话上下文自主判断是否调用外部工具。

**1. 注册工具**（session.update 加 tools 字段）：
```json
{
  "type": "session.update",
  "session": {
    "tools": [{
      "type": "function",
      "function": {
        "name": "get_weather",
        "description": "查询指定城市天气",
        "parameters": {
          "type": "object",
          "properties": { "city": {"type":"string","description":"城市"} },
          "required": ["city"]
        }
      }
    }]
  }
}
```

**2. 事件序列**（模型决定调用工具时）：
```
response.created
response.output_item.added         (item.type=function_call)
conversation.item.created
response.function_call_arguments.delta   (参数增量,可多次)
response.function_call_arguments.done    (完整参数 JSON)  ← 客户端在此执行
response.output_item.done
response.done
```

**3. 执行工具并写回结果**：
```json
{
  "type": "conversation.item.create",
  "item": {
    "type": "function_call_output",
    "call_id": "call_xxx",
    "output": "{\"temperature\":18,\"condition\":\"晴\"}"
  }
}
```

**4. 触发二轮推理**：
```json
{"type":"response.create","response":{"modalities":["audio","text"]}}
```

二轮推理生成的 audio.delta 走正常播放路径。Function Call 部分不送 TTS 播报。一轮响应可含多个 function_call。

### 8.2 本板实现要点

- recv_thread 加 `response.function_call_arguments.done` 分支
- 用 `json_get_str` 提取 `call_id` / `name` / `arguments`（参数是 JSON 字符串，再用 json_get_str 提取具体字段）
- 本地执行函数表（name → C 函数指针）
- 发 `conversation.item.create` 写回 output
- 发 `response.create` 触发二轮（后续 audio.delta 自动入播放队列）
- 注意：function_call 期间 `g_is_responding` 状态流转，避免与打断逻辑冲突

---

## 9. 验收标准

1. **步骤1** ✅：喇叭出正弦波 + mic 电平正常
2. **步骤2** ✅：WS connected, session created
3. **步骤3** ✅：说话后打印 [You] / [AI]
4. **步骤4** ✅：喊话 → AI 语音回复 → 插话打断 → 回声不误触发

**实测指标**：
- 唤醒到可说话延迟 < 3 秒
- AI 首字语音延迟 < 2 秒
- 打断响应 < 500ms
- 回声 gate -18 dBFS 下：AI 能说完 + 插话顺畅

---

## 10. 参考文件索引

- `/code/3516/Qwen-Audio 实时语音对话.md` — 官方文档（含 Function Calling 完整示例 realtime_fc_demo.py）
- `wake_capture.c` — 16kHz 采集（vol=60）
- `wake_playback.c` — 24kHz 播放 + 预 dlopen
- `wake_dialog.c` — 三线程状态机（核心）
- `wake_ws.c` — WebSocket 客户端
- `audio/audio_pcm.c` — AO API 参考（不依赖）
- `device/soc/hisilicon/hi3516dv300/sdk_linux/sample/platform/common/sample_comm_audio.c` — HiSilicon 官方音频 sample
