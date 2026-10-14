# 交付指南：Luna 唤醒 + 实时语音对话 + Function Calling

> **面向对象**：拿到这份代码、要在自己本地 OpenHarmony(Hi3516DV300 HiSpark Taurus, LiteOS-A)环境复现
> "喊 Luna 唤醒 → 实时语音对话 → 动作/表情意图触发 deepseek 决策" 这条完整链路的工程师。
>
> **作者**：fáng xīn。本文档与代码同步,复现过程中遇到问题对照本文档"故障排查"一节。

---

## 0. 这份交付能做什么

在 Hi3516DV300 板子上实现:

1. **语音唤醒**：离线喊"Luna"唤醒(不依赖云端,microWakeWord + TFLM 软推理,16kHz 采集)
2. **实时语音对话**：唤醒后连阿里云 Qwen-Audio-Realtime(WebSocket+TLS),全双工语音对话,
   喊"今天几号"云端直接答(延迟 ~170ms)
3. **Function Calling 动作编排**：用户说"走两步/跳个舞/换个开心的表情"等动作意图时,
   音频模型触发 `control_robot` 工具 → 板上把用户原话发给 deepseek 文本大模型 →
   deepseek 按四行协议 `[SAY]/[EMOJI]/[ACT]/[END]` 决策 → 执行动作(舵机 stub)+表情(写文件)+
   把 `[SAY]` 回复喂回音频模型说出来

**最终效果**：`exec /bin/my_wake flow` → 喊 Luna → Luna 说"你好呀,有什么我可以帮你的吗?" →
你说"走两步" → 喇叭说"好的,走两步给你看" + 日志显示 `[agent] act -> walk`(舵机 stub,接上真硬件即动)。

---

## 1. 前置条件(同事本地需已具备)

| 条件 | 说明 |
|------|------|
| OpenHarmony 源码树 | Hi3516DV300 HiSpark Taurus, LiteOS-A(非 Linux 内核版) |
| 构建工具 | `hb`(hb build)已装,能 `hb set` 选 `hispark_taurus@ipcamera_hispark_taurus` |
| HiSilicon SDK | `device/soc/hisilicon/hi3516dv300/sdk_liteos/`(含 `mpp/lib/*.a`、`include/`) |
| WiFi 客户端库 | `applications/llm_sdk/libwificlient.a` + `wifi_client.c/.h`(这是你的库,已在树里) |
| mbedtls | `third_party/mbedtls/`(TLS 用,已在树里) |
| 烧录工具 | HiTool 或 `rootfs_vfat.img` 烧到 flash 的方式 |
| 一台 WiFi 热点 | 板子要连网(手机热点即可);板子连云端 + deepseek 都走它 |
| 板子硬件 | HiSpark Taurus 板 + 麦克风(板载 AMIC 或外接)+ 喇叭(接 I2S1 DAO) |

> 若你的树里**没有** `device/soc/hisilicon/common/platform/mytest/` 目录,说明你还没接过这个模块,
> 本文档第 3 步会带你从零建。

---

## 2. 交付的文件清单

### 2.1 交付物:wake/ 文件夹压缩包

**交付方式**:把 `wake/` 整个文件夹打包成压缩包(如 `wake.zip` / `wake.tar.gz`)给同事。
压缩包内容:

```
wake/                               # ★ 整个目录(语音唤醒+对话+FC 全部代码)
├── BUILD.gn                        # wake 的构建(my_wake 可执行文件)
├── wake_main.cc                    # main:子命令分发(flow/dialog/sendtest/...)
├── wake_capture.c/.h               # AI 采集(16kHz,HI_MPI_AI + /dev/acodec)
├── wake_playback.c/.h              # AO 播放(24kHz 输入→EnableReSmp→16kHz 硬件)
├── wake_ws.c/.h                    # WebSocket+TLS 客户端(含三线程发送互斥锁)
├── wake_base64.c/.h                # base64 编解码(音频上行/下行用)
├── wake_dialog.c/.h                # ★ 实时对话(三线程状态机+回声抑制+FC 集成)
├── agent_bridge.c/.h               # ★ deepseek 桥接(能力表+四行协议解析+stub 执行)
├── wake_word.cc/.h                 # microWakeWord 封装( Luna 模型推理)
├── wake_word_model_data.c/.h       # Luna 唤醒词模型权重(5090 行, 391KB)
├── tflm/                           # ★ TensorFlow Lite for Micro(5.7M, vendored 副本)
│   ├── tensorflow/lite/micro/      #   核心解释器+kernels
│   ├── tensorflow/lite/signal/     #   signal 算子(microfrontend 依赖)
│   ├── tensorflow/lite/core/       #   core API
│   ├── tensorflow/lite/kernels/internal/  # 内部工具
│   └── third_party/                #   flatbuffers/gemmlowp/ruy/kissfft
├── microfrontend/                  # ★ 音频前端(236K, FFT+滤波器组,纯 C)
│   ├── tensorflow/lite/experimental/microfrontend/lib/
│   └── kissfft/
├── SPEC.md                         # 唤醒词移植 spec(步骤1-3)
├── SPEC_DIALOG.md                  # 实时对话 spec(步骤4)
├── SPEC_FC_AGENT.md                # ★ Function Calling spec(步骤5,与代码同步)
├── IMPLEMENTATION.md               # 实现笔记
└── DELIVERY_GUIDE.md               # 本文档
```

> **注意**:压缩包里只有 `wake/`,**不含**外层 `mytest/BUILD.gn` 聚合层文件。
> 同事需自己建这个聚合层(见第 3 节步骤 2),内容就 3 行,很简单。
>
> `tflm/` 有 5.7M,文件多(几百个 .cc),压缩包较大属正常。这是 vendored 的 TFLM 副本,
> 必须完整(构建时全编译 reference kernels)。

### 2.2 需要同事修改/新建的文件(不在压缩包里)

| 文件 | 操作 | 说明 |
|------|------|------|
| `device/soc/hisilicon/common/platform/mytest/BUILD.gn` | **新建**(若同事树里没有 mytest/ 目录) | 聚合层,把 wake 拉进构建链。内容见第 3 节步骤 2 |
| `device/soc/hisilicon/common/platform/BUILD.gn` | **改**:deps 列表加 `"mytest"` | 见第 3 节步骤 2 |
| `applications/llm_sdk/llm_client.c` | **改**:`LLM_MSG_MAX` 1024→4096 + REQ/BODY/ESCJ 同步放大 | 见第 3 节步骤 3,FC 的 agent prompt ~1.3KB 不截断 |

### 2.3 不用交付(同事树里已有 / 不依赖)

- `applications/llm_sdk/libwificlient.a`、`wifi_client.c/.h`、`wifi_wpa.c` —— WiFi 库(同事的)
- `applications/llm_sdk/llm_client.h` —— 接口头文件(若版本一致可不动)
- `applications/llm_sdk/libllmclient.a` —— 预编译库(**不用**,我们编译 llm_client.c 源码进 wake)
- `third_party/mbedtls/` —— TLS 库
- `device/soc/hisilicon/.../sdk_liteos/mpp/lib/*.a` —— MPP 静态库
- `prompt/` 目录 —— agent_bridge.c 已自包含,不运行时依赖它(只是移植参考)
- `mytest/audio/` 子目录 —— 独立的 my_audio 录音回放工具,与 wake 无代码依赖(wake 不引用它)

---

## 3. 复现步骤

### 步骤 1:解压源码

把交付的 `wake/` 压缩包解压到:
```
device/soc/hisilicon/common/platform/mytest/wake/
```

即解压后路径是 `mytest/wake/BUILD.gn`、`mytest/wake/wake_main.cc`、`mytest/wake/tflm/` 等。

> 若你的树里**没有** `mytest/` 目录,新建它即可(就一个空目录,放 wake 进去)。
> `tflm/` 解压慢属正常(5.7M,几百个 .cc)。

### 步骤 2:建聚合层 + 接入构建链

**(a) 新建 `mytest/BUILD.gn`**(若该文件已存在则跳过此步):

创建 `device/soc/hisilicon/common/platform/mytest/BUILD.gn`,内容:

```gn
# mytest/ - 板载语音唤醒+对话工具(my_wake)
group("mytest") {
  deps = [ "wake" ]
}
```

> 这个聚合层把 wake 子目录拉进构建链。压缩包里不含此文件(它在 wake/ 外层),
> 需同事自己建。就这 3 行。

**(b) 确认 platform 构建链引用了 mytest**:

打开 `device/soc/hisilicon/common/platform/BUILD.gn`,确认 `deps` 列表里有 `"mytest"`:

```gn
deps = [
    ...
    "mytest",    # ← 必须有这一行(没有就加)
    ...
]
```

### 步骤 3:修改 llm_client.c(关键)

打开 `applications/llm_sdk/llm_client.c`,把顶部常量放大:

```c
/* 原值(stock): */
#define LLM_MSG_MAX         1024
#define LLM_REQ_SIZE        3584
#define LLM_BODY_SIZE       3072
#define LLM_ESCJ_SIZE       2048

/* 改为(FC agent prompt ~1.3KB 不截断): */
#define LLM_MSG_MAX         4096
#define LLM_REQ_SIZE        6144
#define LLM_BODY_SIZE       5120
#define LLM_ESCJ_SIZE       8192
```

**为什么**:FC 的 agent prompt 固定部分(能力表+规则+示例)约 1.3KB,加上用户文字,
`LLM_MSG_MAX=1024` 会截断 → deepseek 看不到完整格式约束 → 不守四行协议。
预编译的 `libllmclient.a` 是 1024,但我们**编译 llm_client.c 源码进 wake**(见步骤 4),
不链接 .a,所以改源码常量即可。`llm_test`/`customer_test` 仍链接 .a,不受影响。

### 步骤 4:确认 wake/BUILD.gn(已配好,检查即可)

`mytest/wake/BUILD.gn` 已把 `llm_client.c` 编译进 wake(非链接 .a),并配好所有依赖:

```gn
executable("wake") {
  output_name = "my_wake"
  sources = [
    ...
    "agent_bridge.c",
    "//applications/llm_sdk/llm_client.c",   # ← 编译源码,非 .a
    ...
    # + TFLM 全套 kernels/signal/microfrontend 源码
  ]
  include_dirs = [
    ...
    "//third_party/mbedtls/include",
    "//applications/llm_sdk",
    "tflm", "tflm/third_party/...",
    "microfrontend", ...
  ]
  ldflags = [
    "-L$mpp_lib_dir",
    "-Wl,--start-group",
    "$mpp_lib_dir/libmpi.a", ..., "$mpp_lib_dir/libsecurec.a",
    "-Wl,--end-group",
    "-lm", "-ldl",
    _wifi_a,   # applications/llm_sdk/libwificlient.a
  ]
  deps = [ "//third_party/mbedtls:mbedtls_shared" ]
}
```

### 步骤 5:填入你的配置(必改)

#### 5.1 WiFi 热点

`wake_main.cc` 和 `wake_dialog.c` 顶部各有一组 WiFi 凭据(**两处都要改**):

```c
/* wake_main.cc */
#define WIFI_SSID "iPhone"      /* ← 改成你的热点名 */
#define WIFI_PSK  "fx151231"    /* ← 改成你的热点密码 */

/* wake_dialog.c */
#define DIALOG_WIFI_SSID  "iPhone"
#define DIALOG_WIFI_PSK   "fx151231"
```

> WiFi 冷启动连接偶尔首次失败(TCP/ARP 未稳),代码已内置 3 次重试。
> 先在板子上手动 `wifi_connect` 确认热点能连,再跑 my_wake。

#### 5.2 Qwen-Audio API Key(音频模型,必填你自己的)

`wake_main.cc` 和 `wake_dialog.c` 顶部:

```c
/* wake_main.cc */
#define QWEN_API_KEY "sk-ws-H.PIPHLHL...."   /* ← 换成你的阿里云 DashScope key */

/* wake_dialog.c */
#define DIALOG_API_KEY "sk-ws-H.PIPHLHL...." /* ← 同上 */
```

Qwen WebSocket 端点(已在 `wake_ws.h` 配好,一般不用改):
```c
#define QWEN_WS_IP   "47.94.20.201"
#define QWEN_WS_HOST "ws-qq7tg8jxbjmnqaty.cn-beijing.maas.aliyuncs.com"
#define QWEN_WS_PORT 443
#define QWEN_WS_PATH "/api-ws/v1/realtime?model=qwen-audio-3.1-realtime-plus"
```

#### 5.3 deepseek 网关(FC 用,SiliconFlow key)

`agent_bridge.c` 顶部:

```c
static const llm_gateway_t g_deepseek_gw = {
    .ip      = "47.102.215.139",
    .vhost   = "api.siliconflow.cn",
    .port    = 443,
    .api_key = "sk-jcojwurdcgxlgtrgaiuibttmopjzzxxrmnrfoieawqpkqwrj",  /* ← 用你自己的 SiliconFlow key */
    .model   = "deepseek-ai/DeepSeek-V4-Flash",
};
```

> 这个 key 是同事的 SiliconFlow key。你有自己的就用你自己的;没有就先沿用(仅测试用)。
> deepseek-ai/DeepSeek-V4-Flash 是 SiliconFlow 上的模型名,确认你的账号能调。

### 步骤 6:构建

```bash
hb set          # 选 hispark_taurus@ipcamera_hispark_taurus
hb build -f
```

构建成功后产物:
```
out/hispark_taurus/ipcamera_hispark_taurus/rootfs/bin/my_wake
out/hispark_taurus/ipcamera_hispark_taurus/rootfs_vfat.img   # 烧这个
```

### 步骤 7:烧录

把 `rootfs_vfat.img` 烧到板子 flash(HiTool / 你的烧录方式)。
烧好后 my_wake 在板子 `/bin/my_wake`,库在 `/usr/lib/`。启动运行见第 5 节。

---

## 4. 运行时依赖(rootfs 层面,构建自动处理,仅需了解)

### 4.1 重采样 .so 的 dlopen(已用代码兜底,不用改 init.cfg)

`wake_playback.c` 播放时调 `HI_MPI_AO_EnableReSmp(24000)`(24kHz→16kHz 硬件重采样),
MPP 内部会 `dlopen("libhive_RES.so")` 用裸文件名。问题:LiteOS-A 的 dlopen 默认只搜 `/lib`,
而 `libhive_RES.so` 在 `/usr/lib`。

**代码里的兜底方案**(已在 `wake_playback.c` 实现,无需改 init.cfg):
启动时先按**绝对路径** `dlopen("/usr/lib/libhive_RES.so", RTLD_NOW|RTLD_GLOBAL)` 预加载,
符号进全局表;之后 MPP 的裸名 dlopen 直接命中缓存,不再走路径搜索。

```
[my_wake] play: preloaded /usr/lib/libsecurec.so OK
[my_wake] play: preloaded /usr/lib/libhive_RES.so OK
```

> 若这两行打 FAILED:检查 rootfs 的 `/usr/lib/` 下有没有这两个 .so(标准构建产物里都有)。
> rootfs init.cfg 里通常有 `export LD_LIBRARY_PATH /storage/app/libs:/usr/lib`,也是保障。

### 4.2 表情命令文件(FC 用)

FC 路径里 `[EMOJI]` 执行会写 `/storage/data/emoji_cmd` 文件(一行表情名)。
这是给未来的 ui_display 进程消费的——**当前没有 ui_display,写了也无害**(文件通道是真的)。
接 ui_display 进程时,让它 tail 这个文件即可,agent_bridge 零改动。

---

## 5. 启动运行(串口操作全流程)

烧录后,按以下顺序在串口(115200)操作。**顺序很重要**——先确认网络,再启动,否则 WS 连接会失败。

### 5.1 硬件准备

- 麦克风接好(板载 AMIC 或外接 I2S 麦克)
- 喇叭接好(I2S1 DAO)
- 串口线连好(115200,板子启动后看到 `OHOS #` 提示符)

### 5.2 连接 WiFi 热点 + 确认网络状态

先把手机热点(或路由器)开好。然后**手动连热点、手动确认网络状态**,确认板子能上公网后
再启动 my_wake——云端 WS + deepseek 都依赖网络,先确认好避免 my_wake 启动后 WS 连不上。

**① 连接热点:**

```
OHOS # exec /bin/wifi_cli apx iPhone fx151231
```

- `iPhone` = 你的热点 SSID,`fx151231` = 密码。换成你自己的。
- 连上后 wifi_wpa 守护进程会自动重连(断线自动恢复)。

**② 确认网络状态:**

```
OHOS # exec /bin/wifi_cli status
```

输出 `status->0` 就是网络就绪了,可以启动。

> 连热点 + 确认这两步要在 `exec /bin/my_wake flow` **之前**做完。
> my_wake 内部也会 `wifi_connect_ap`(已连则幂等跳过),但先手动确认能把网络问题
> 和 my_wake 自身问题分开定位——网络没连好时 my_wake 会打
> `[flow] wifi_connect_ap FAILED` / `[flow] wifi_wait_connected FAILED`,容易误判成代码问题。

### 5.3 启动命令

```
OHOS # exec /bin/my_wake flow
```

这就是完整功能的入口:**唤醒监听 → 喊 Luna 进对话 → 对话/FC → 空闲 30s 回唤醒 → 循环**。

**可选参数**:
```
OHOS # exec /bin/my_wake flow -18 60
#                            ^^  ^^
#                            |   └─ 麦克风增益(0~126, 默认 60); 调低(30~40)减噪声防自言自语
#                            └──── energy gate 阈值(dBFS, 默认 -18); 越接近 0 越紧
```

> 不带参数用默认值(gate=-18, mic_vol=60),实测最稳。

启动成功标志:
```
[flow] === wake -> dialog flow (gate -18.0 dBFS, mic_vol 60) ===
[flow] wifi ok, ip=172.20.10.4                ← WiFi 连上了
[flow] capture + playback + wake_word init...
[wake_word] model ready: arena 20732/61440 bytes, input scale 0.10196079 zp -128
[flow] init OK, say "Luna" to start a conversation
[wake] listening for "Luna"...                 ← 在听 Luna 了
```

看到 `[flow] init OK, say "Luna" to start a conversation` 就可以喊 Luna 了。

### 5.4 正常使用流程

1. 喊"**Luna**" → 喇叭说"你好呀,有什么我可以帮你的吗?"(延迟 ~1.6s)
2. 直接说话即可对话:
   - 闲聊/问答("今天几号"/"讲个笑话") → Luna 直接语音答(延迟 ~170ms)
   - 动作意图("走两步"/"跳个舞"/"换个开心的表情") → 触发 FC,~3s 后 Luna 说 deepseek 生成的回复 + 日志显示动作执行
3. 30 秒不说话 → 自动退出对话回唤醒监听(日志 `[dialog] idle 30s, back to wake listen`)
4. 再喊 Luna 又能进对话,循环

### 5.5 停止

`exec` 运行的程序按 **Ctrl+C** 退出(或 `kill` 进程)。退出时打 `[flow] done`。
退出后采集器/播放/WS 都会清理,板子回到 `OHOS #` 提示符。

### 5.6 其他子命令(调试用,非主流程)

| 命令 | 用途 |
|------|------|
| `exec /bin/my_wake flow` | **主入口**(唤醒+对话+FC,日常用这个) |
| `exec /bin/my_wake dialog` | 跳过唤醒,直接进对话(调试对话用) |
| `exec /bin/my_wake` | 只唤醒监听,不对话(测唤醒词用) |
| `exec /bin/my_wake wstest` | 只测 WebSocket 连通性(不采集不播放) |
| `exec /bin/my_wake sendtest` | 单向上传 PCM 收转写(步骤3遗留) |
| `exec /bin/my_wake duplex` | AI+AO 双工验证(步骤1遗留) |

> 日常验证完整功能只用 `flow`。其他子命令是开发时分步骤排查用的,复现时一般用不到。

---

## 6. 验证(跑起来后看日志)

### 6.1 正常启动

```
[flow] === wake -> dialog flow (gate -18.0 dBFS, mic_vol 60) ===
[flow] wifi ok, ip=172.20.10.4
[flow] capture + playback + wake_word init...
[wake_word] model ready: arena 20732/61440 bytes, input scale 0.10196079 zp -128
[flow] init OK, say "Luna" to start a conversation
[wake] listening for "Luna"...
```

喊 Luna:
```
[wake] *** DETECTED! "Luna" ***
[flow] entering dialog...
[ws] TLS handshake OK
[ws] WebSocket upgrade OK
[dialog] ready! Luna will greet you, then talk (idle 30s -> wake)
[AI] 你好呀！有什么我可以帮你的吗？     ← 喇叭出声
[latency] wake-detected -> first-play: ~1650 ms   ← 唤醒到出声延迟
```

### 6.2 普通对话(闲聊,不走 FC)

说"今天几号":
```
[You] 今天几号
[AI] 今天是2024年5月22日。
[latency] speech-stop -> first-play: ~170 ms    ← 普通对话延迟极低
```

**关键**:日志里**没有** `[FC] call` —— 话路区分闸生效,闲聊不走 deepseek。

### 6.3 Function Calling(动作意图,走 deepseek)

说"走两步":
```
[You] 走两步。
[FC] transcript done (item_id=..., match=1, ready=1)
[FC] call control_robot, call_id=..., args={"action": "walk"}
[FC] fc_worker start (item_id=..., ready=1, waited=0ms, text=走两步。)
[agent] >> prompt 2109 bytes (user_text=12)
[llm] sending 2109 bytes to deepseek...
[llm] received 73 bytes
[llm] raw reply: [[SAY] 好的, 走两步给你看\n[EMOJI] none\n[ACT] walk\n[ACT] walk\n[END]]
[agent] reply: 好的, 走两步给你看
[agent] act -> walk (act#1, queued, stub=no-servo)
[agent] act -> walk (act#2, queued, stub=no-servo)   ← 走两步=walk×2
[FC] function_call_output: reply set, status=ok
[FC] response.create sent (round-2)
[AI] 好的, 走两步给你看      ← deepseek 的 [SAY] 被音频模型说出来 = 闭环
```

**这条日志序列每一行都对应一个环节通**(详见 SPEC_FC_AGENT.md §6.2)。
看到 `[AI]` 内容 = `[SAY]` 内容,就是端到端闭环成功。

---

## 7. 故障排查(我们踩过的坑)

### 7.1 `[my_wake:my_wake]Unsupported syscall ID: 256`

**无害**,可忽略。LiteOS-A 对 `getrandom` 之类的 syscall 未实现,但 mbedTLS 有 fallback。
不影响功能。每次启动都会打一行。

### 7.2 喊 Luna 唤醒不了 / 误触发

- **唤醒灵敏度**:`wake_capture.c` 的 `WAKE_MIC_VOL=60` 是 PGA 增益(0~126)。
  环境太吵调低(40),太安静调高(70)。`exec /bin/my_wake flow -18 60`(gate micvol)。
- **唤醒后立刻误触发**:对话结束回唤醒有 1s warmup 丢弃期 + `wake_word_reset()`。
  若仍误触发,检查喇叭余响(调大 `DIALOG_COOLDOWN_FRAMES`)。
- **score 一直是 0**:确认麦克风有声音(`[wake] mic -XX dBFS` 应在 -15~-30 之间,
  太低=-60 说明 mic 没起来,检查 `wake_capture.c` 的 ACodec 初始化)。

### 7.3 AI 自言自语(回声导致)

症状:AI 说完一句话,自己又被自己的回声打断,无限循环。

**已用的方案**:`echo_suppression`(不是 energy gate)——AI 正在生成或播放时,麦克风
**完全静音不上传**(不判能量)。代价:AI 说话时插不进话,要等说完。

- 播放结束后有 1s cooldown(`DIALOG_COOLDOWN_FRAMES=33`)滤喇叭余响。
- 若仍自言自语:确认 `g_is_playing`/`g_is_responding` 标志正确(日志 `--- speech_started ---`
  行的 `responding=` `playing=` 字段),可能是状态机漏了某个事件。

### 7.4 `[llm] llm_chat failed` / deepseek 连不上

- 检查板子能上公网:`ping` 不了就检查 WiFi。
- 检查 SiliconFlow key 有效:`[llm] llm_chat failed: -1`=连不上网关,-4=HTTP 非 200(key 错),
  -7=TLS 握手失败。
- `[llm] raw reply: [...]` 会打印 deepseek 原文——定位格式问题不靠猜,看这行。

### 7.5 `[agent] !! reply has no tags (bad format)`

deepseek 没守四行协议。看 `[llm] raw reply: [...]` 原文:
- 若是纯文本回复:多半是**发给 deepseek 的用户文字错了**(比如拿到了上一句旧值)。
  检查 `[FC] fc_worker start ... text=` 是不是当前这句话。
- 若是 `<think>...` 思考标签:模型是 reasoning 版,可能需要 prompt 里更强约束(目前没遇到)。
- 临时删 prompt 模板测试 SalvageReply 兜底,应看到 `salvaged instructions from bad format`。

### 7.6 fc_worker 拿到旧转写("走两步"却发"现在也没")

这是 item_id 关联机制要解决的。检查日志:
```
[FC] audio committed (item_id=item_xxx, ready=0)
[FC] transcript done (item_id=item_xxx, match=1, ready=1)   ← match=1 才对
[FC] fc_worker start (item_id=item_xxx, ready=1, waited=0ms, text=走两步。)  ← text 正确
```
若 `match=0` 或 `ready=0`:item_id 没对上,检查 `wake_dialog.c` 的
`input_audio_buffer.committed` 和 `transcription.completed` 处理是否都取了 `item_id`。

### 7.7 构建错误

- `cannot find -lmpi`:mpp_lib_dir 路径错,检查 `sdk_liteos_root` 变量。
- `TFLM ... undefined reference`:tflm/ 目录没拷全,或 BUILD.gn 的 kernel_srcs/signal_srcs 漏文件。
  TFLM 的 reference kernels 必须全编译(micro_op_resolver 无条件 include signal 头)。
- `LLM_MSG_MAX redefined`:llm_client.c 被编两次(可能又链了 .a),检查 BUILD.gn 别同时加源码和 .a。

---

## 8. 后续扩展(接真硬件)

### 8.1 接真舵机(替换动作 stub)

`agent_bridge.c` 的 `run_act()` 当前是 stub(只打印)。接同事的舵机控制:

```c
static void run_act(const char *name, const int *par, int npar) {
    printf("[agent] act -> %s", name);
    for (int i=0;i<npar;i++) printf(" %d", par[i]);
    printf(" (act#%d, queued, stub=no-servo)\n", ++g_act_seq);
    /* TODO(同事): 在这行日志后加真实调用, 签名不变 */
    OttoPcaRun(name, par, npar);   /* ← 加这行(或你的舵机 API) */
}
```

`run_act` 契约**非阻塞**(从 fc_worker 线程调,不能卡 deepseek 流程)。
多动作(walk×2)会按序多次调用。

### 8.2 接表情屏(ui_display 进程)

`agent_bridge.c` 的 `send_emoji_cmd()` 已经在写 `/storage/data/emoji_cmd` 文件(一行表情名)。
你的 ui_display 进程只要:

```c
/* ui_display 里 tail 这个文件, 读到表情名就切换 */
FILE *f = fopen("/storage/data/emoji_cmd", "r");
char name[32]; fscanf(f, "%s", name);   /* happy / sad / ... */
/* 切表情 */
```

agent_bridge 零改动。21 个表情名见 `g_emojiTable[]`。

### 8.3 调整对话参数

`wake_dialog.c` 顶部可调:
- `DIALOG_COOLDOWN_FRAMES 33` —— 播放后静音冷却(1s),回声大就调大
- `DIALOG_IDLE_TIMEOUT_MS 30000` —— 对话空闲超时回唤醒(30s)
- `PLAY_QUEUE_MAX_BYTES 2MB` —— 播放队列上限(TTS 比实时快防堆积)
- session.update 里 `threshold 0.5` / `silence_duration_ms 800` —— 云端 VAD 灵敏度

### 8.4 加 fctest 子命令(连通性隔离)

若 deepseek 连通性要单独测,可在 `wake_main.cc` 加:
```c
if (strcmp(argv[1], "fctest") == 0) {
    /* 直接调 agent_bridge_run("走两步", say, sizeof(say)) 不走 WS */
}
```
当前没加,FC 连通性直接在 `flow` 里测(看 `[llm]` 日志)。

---

## 9. 文档索引(深入看哪个环节)

| 想了解 | 看 |
|--------|-----|
| 唤醒词怎么从 ESP32 移植到 Hi3516 | `SPEC.md` |
| 实时对话三线程状态机 / 回声抑制 | `SPEC_DIALOG.md` |
| Function Calling 双模型架构 / 四行协议 / item_id 坑 | `SPEC_FC_AGENT.md` |
| 实现踩坑细节(音频/调试经验) | `IMPLEMENTATION.md` |
| MPP dlopen 运行时路径坑 | `wake_playback.c` 注释(预加载机制) |

---

## 10. 已知限制

- **不支持打断**:AI 说话时插不进话(echo_suppression 方案牺牲打断换稳定性)。
  要打断需加硬件 AEC(留作后续)。
- **FC 路径延迟 ~3s**:deepseek 调用 2.5s + 二轮 TTS。普通对话 170ms 不受影响。
- **舵机/表情是 stub**:链路用日志证明通,真硬件按第 7 节接。
- **WiFi 冷启动**:首次连接偶尔失败,已 3 次重试;建议先手动连好热点。
- **syscall 256 警告**:无害(第 6.1 节),每次启动一行,忽略。

---

**遇到问题别靠猜,加 printf 看日志定位。** 这是本项目调试的核心原则——
所有关键环节都有结构化日志(`[wake]`/`[dialog]`/`[FC]`/`[agent]`/`[llm]`/`[latency]`),
grep 对应前缀就能看到完整链路状态。
