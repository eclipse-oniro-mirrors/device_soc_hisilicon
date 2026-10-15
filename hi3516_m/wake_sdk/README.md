# wake_sdk（语音唤醒 + 实时对话 + Function Calling）—— Hi3516DV300 HiSpark Taurus

板端程序 **my_wake**：离线喊 "Luna" 唤醒（microWakeWord + TFLM 软推理，16kHz 采集）→
连阿里云 Qwen-Audio-Realtime（WebSocket+TLS）全双工语音对话 → 动作/表情意图触发
Function Calling（deepseek 四行协议 `[SAY]/[EMOJI]/[ACT]/[END]` 决策）。
LiteOS-A 用户态独立程序，直接调 HI_MPI_AI/AO + /dev/acodec，不依赖 media_server。

## 板端命令（串口）

| 命令 | 用途 |
|------|------|
| `exec /bin/my_wake flow [-gate] [mic_vol]` | **主入口**：唤醒→对话→FC→空闲30s回唤醒，循环 |
| `exec /bin/my_wake dialog` | 跳过唤醒直接进对话（调试） |
| `exec /bin/my_wake` | 只唤醒监听不对话（测唤醒词） |
| `exec /bin/my_wake wstest` | 只测 Qwen WebSocket 连通性 |
| `exec /bin/my_wake sendtest` | 单向上传 PCM 收转写 |
| `exec /bin/my_wake duplex` | AI+AO 双工验证 |

默认参数 gate=-18 dBFS、mic_vol=60，实测最稳。

## 文件清单

| 文件 | 说明 |
|------|------|
| wake_main.cc | main：子命令分发 + 唤醒监听主循环 |
| wake_capture.c/.h | AI 采集（16kHz，HI_MPI_AI + /dev/acodec） |
| wake_playback.c/.h | AO 播放（24kHz 输入→EnableReSmp→16kHz 硬件） |
| wake_ws.c/.h | WebSocket+TLS 客户端（mbedtls，三线程发送互斥锁） |
| wake_base64.c/.h | base64 编解码（音频上行/下行） |
| wake_dialog.c/.h | 实时对话（三线程状态机+回声抑制+FC 集成） |
| agent_bridge.c/.h | deepseek 桥接（能力表+四行协议解析+动作 stub） |
| wake_word.cc/.h | microWakeWord 封装（Luna 模型推理） |
| wake_word_model_data.c/.h | Luna 唤醒词模型权重（391KB） |
| llm_client.c/.h | **llm 依赖已打包进本包**（LLM_MSG_MAX=4096 版） |
| wifi_client.h + libwificlient.a | **WiFi 依赖已打包进本包** |
| tflm/ | vendored TensorFlow Lite for Micro 全量（5.7M） |
| microfrontend/ | 音频前端（FFT+滤波器组，纯 C） |
| my_wake | 预编译板端程序（ELF 32-bit ARM musl，640KB） |
| *.o | 10 个关键中间目标（lite 工具链 bitcode：wake 9 个 + llm_client.o） |
| build_wake.sh | 独立交叉编译脚本（不依赖 GN，221 个源文件全量） |
| BUILD.gn / bundle.json | 树内 GN 集成（放 applications/wake_sdk 生效） |
| SPEC.md / SPEC_DIALOG.md / SPEC_FC_AGENT.md / IMPLEMENTATION.md / DELIVERY_GUIDE.md | 原作者设计文档与交付指南 |

## 必改配置（编译期宏，两处起，别漏）

| 宏 | 文件 | 说明 |
|----|------|------|
| `WIFI_SSID` / `WIFI_PSK` | wake_main.cc:47-48 | WiFi 热点 |
| `QWEN_API_KEY` | wake_main.cc:51 | 阿里云 DashScope key（音频模型） |
| `DIALOG_WIFI_SSID/PSK`、`DIALOG_API_KEY` | wake_dialog.c:33-35 | 同上，对话路径用的另一份 |
| `g_deepseek_gw.api_key` | agent_bridge.c:34 | SiliconFlow key（FC 用） |

包内源码带的 key 是原作者测试 key，量产前务必换成自己的。
Qwen WS 端点在 wake_ws.h（一般不用改）。

> 也可以不改宏：烧录后先 `exec /bin/wifi_cli apx 热点 密码` 手动连好热点，
> my_wake 内部 `wifi_connect_ap` 已连则幂等跳过（API Key 仍必须改）。

## 编译方式（二选一）

**方式 A：树内 GN 构建（推荐量产）**
放 `applications/wake_sdk/` + 注册组件 → `hb build` → 产物 `out/.../rootfs/bin/my_wake`。
详见 `交付包_Wake_v1/量产集成说明.md`。

**方式 B：独立脚本（不进树）**
```bash
cd 交付包_Wake_v1/SDK库包
bash build_wake.sh [OH源码根目录]    # 默认 /code/3516
```
前置：树编译过一次（out 里有 sysroot、mpp/lib/*.a、libs/libmbedtls.a）。
产物 my_wake + build/ 中间 .o。旗标（clang 交叉、cortex-a7、musl sysroot、
TFLM 宏）全在脚本里。

**链接差异说明**：

| 方式 | mbedtls | 产物 NEEDED |
|------|---------|-------------|
| GN（deps mbedtls_shared） | 动态 libmbedtls.so（rootfs /usr/lib 自带） | libmbedtls.so + libc++.so + libc.so |
| build_wake.sh | 静态 out/libs/libmbedtls.a | libc++.so + libc.so |

两种都实测可跑。脚本版还静态链了 mpp 全家桶（libmpi/libdnvqe/.../libsecurec.a）
和 ./libwificlient.a（**显式路径直传，不用 -L. -lwificlient**）。

## 复用模块 API（二次开发）

本包主体是可执行程序；模块级 API 直接看头文件：

- `wake_word.h`：`WakeWordInit()/WakeWordDetect()/wake_word_reset()` —— 单独要唤醒词检测就抄这几个
- `wake_ws.h`：`ws_connect/ws_send_audio/ws_recv_event()` —— 通用 WebSocket+TLS 客户端
- `llm_client.h`：`llm_chat(gateway, msg, reply, size)`、`llm_gateway_t` —— deepseek 文本模型调用
- `agent_bridge.h`：`agent_bridge_run(user_text, say, size)` —— 四行协议决策+动作执行（非阻塞 stub，接真舵机在 run_act() 里加）

## 注意事项（板上实测）

1. **必须 `exec /bin/my_wake flow`**——LiteOS shell 超 16 字符命令名静默丢弃，裸名字报 not found
2. `Unsupported syscall ID: 256` 无害（getrandom 未实现，mbedTLS 有 fallback）
3. 回声抑制用 echo_suppression：AI 说话时麦克风完全静音，**不支持打断**（换打断要硬件 AEC）
4. FC 路径延迟 ~3s（deepseek 2.5s + 二轮 TTS）；普通对话 ~170ms 不受影响
5. `llm_client.c` 必须用包内这份（LLM_MSG_MAX=4096），**不要**链交付包_LLM_SDK_v1 里的
   预编译 libllmclient.a（旧版 1024 会截断 agent prompt，deepseek 就不守四行协议了）
6. 播放前 `dlopen("/usr/lib/libhive_RES.so")` 绝对路径预加载已在 wake_playback.c 内置，
   不用改 init.cfg
7. FC 的 `[EMOJI]` 写 `/storage/data/emoji_cmd` 文件（一行表情名），ui_display 进程 tail 它即可
8. TFLM 是 vendored 副本，必须全量编译（micro_op_resolver 无条件 include signal 头）；
   flatbuffers 用 tflm/third_party 里自带的（与树内 25.2.10 版本不兼容）

## 深入文档

| 想了解 | 看 |
|--------|-----|
| 唤醒词模型移植 | SPEC.md |
| 对话三线程状态机/回声抑制 | SPEC_DIALOG.md |
| Function Calling 双模型架构/四行协议 | SPEC_FC_AGENT.md |
| 实现踩坑细节 | IMPLEMENTATION.md |
| 烧录 | 烧录镜像/FLASH_GUIDE.txt |
| 非程序员照抄流程 | 操作手册.md |
| 树内 GN 集成 | 量产集成说明.md |
