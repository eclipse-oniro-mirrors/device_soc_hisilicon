# SPEC: Function Calling 接入文本大模型(deepseek)做动作/表情编排

> 状态：**已实现并端到端验证通过(2026-09-24)**。本文档与代码同步。
> 上游参考：`/code/3516/prompt/agent_service.c` + 《能力清单与LLM协议.md》。
> 执行层(舵机/屏幕)按"已实现"对待——通路用结构化日志证明通,真实硬件接入只差一行调用。

> **硬约束：所有 printf/日志输出用英文。** 注释可用中文。发给模型的 prompt 文本
> (session.update instructions、BuildPrompt 内容)保持中文(那是给模型的指令, 不是日志)。
> deepseek 回的 `[SAY]` 内容是中文(回复用户), 原样透传不翻译。诊断/状态/审计日志一律英文。

---

## 1. 目标

把 `handle_function_call` 里原 stub 换成：**把用户说的话填进 agent prompt，用 `llm_chat` 发给 deepseek，解析四行协议 `[SAY]/[EMOJI]/[ACT]/[END]`，执行动作+表情，并把 `[SAY]` 喂回音频模型说出来。**

两个 LLM 分工：
- **Qwen-Audio-Realtime（音频模型）**：语音 I/O + **话路区分守门员**（§3）。识别到"用户要做肢体动作/切表情"时调 `control_robot` 工具触发本流程；纯问答/闲聊不调，直接自己答。
- **deepseek（文本模型，via llm_chat）**：拿到用户转录，从 23 动作 + 21 表情里选最贴合的，生成 `[SAY]` 回复。

桥接模块 **`agent_bridge.c/.h`**（原 `servo_ctl.c`，改名——它本质是"音频模型 ↔ 文本 agent 的桥"，舵机只是回复后的执行端之一）。

---

## 2. 架构(双模型流程)

```
用户说话
  │  Qwen-Audio (wss, 现有)
  ├─ ASR 转写 ──> [You] 打印 + 存 g_last_user_transcript(item_id 关联, §5.1)
  ├─ 话路区分(§3): 判断是否动作/表情意图
  │     ├─ 否(纯问答/闲聊) ──> 音频模型自己语音答, deepseek 不参与
  │     └─ 是 ──> function_call: control_robot(action)
  │
  ▼  handle_function_call(call_id, name, arguments)  [wake_dialog.c]
  │   ├─ 不在 recv_thread 里直接调 deepseek(会阻塞收发 ~3-8s)
  │   └─ spawn fc_worker 线程(非阻塞, §5.2), 立即返回
  │
  ▼  fc_worker 线程
  │   1. 轮询等当前 item 的转写就绪(item_id 匹配, §5.1, 最多 3s)
  │   2. agent_bridge_run(text, say_out, say_cap)  [agent_bridge.c, §4]
  │        ├─ BuildPrompt(text)  [移植自 agent_service.c]
  │        ├─ llm_chat(&g_deepseek_gw, prompt, reply, sizeof)  [§5]
  │        │     失败 → 返回 <0, 降级见 §7
  │        ├─ ParseReply(reply) → [SAY]/[EMOJI]/[ACT]/[END]
  │        │     格式违规 → SalvageReply 兜底
  │        └─ 分发:
  │             [ACT]   → agent_bridge 内部 run_act(name) (可能多行, 按序)  [§6]
  │             [EMOJI] → 写 /storage/data/emoji_cmd 一行名  [§6]
  │             [SAY]   → 写入 say_out 返回给调用者
  │   3. 写回 function_call_output = {"reply":"<SAY>"}  [wake_dialog, §5.3]
  │   4. response.create → 音频模型二轮语音, 把 [SAY] 说出来  [§5.4]
  │
  ▼  Qwen-Audio 二轮: response.audio.delta → 播放队列 → 喇叭说 [SAY]
```

---

## 3. 话路区分(守门员 = 音频模型 function calling)

**区分机制**：音频模型调不调 `control_robot`，就是"走不走 FC 这条路"的开关。
- 用户说"跳个舞/挥挥手/换个开心的表情" → 音频模型判断是动作/表情意图 → 调 `control_robot` → recv_thread 收到 `response.function_call_arguments.done` → 进 fc_worker → 发 deepseek。
- 用户说"今天几号/你好/讲个笑话" → 音频模型判断不是动作 → 不调 `control_robot` → 直接自己语音答 → recv_thread 不进 FC 分支 → deepseek 完全不参与。

**零本地过滤代码**：信任音频模型的 function calling 判断（音频模型本就是为这种 tool-use 设计）。区分的精度靠把 session.update 的 `instructions` 写精确（§5.4）。

**实测验证(2026-09-24)**：说"今天几号" → 无 `[FC] call`，音频模型直接答"今天是2024年5月22日"，闲聊不走 deepseek。✓

---

## 4. agent_bridge 模块(已实现, 移植自 prompt/agent_service.c)

### 4.1 对外契约(agent_bridge.h)

```c
/* 桥接音频模型的 function call 到文本 agent(deepseek)。
 * 由 wake_dialog 的 fc_worker 线程调用(音频模型调 control_robot 时)。
 * 把 user_text 填进 agent prompt 发 deepseek, 解析四行协议,
 * 分发 [ACT]/[EMOJI] 到执行层, 把 [SAY] 文本经 say_out 返回给调用者
 * (调用者再喂回音频模型说出来)。
 *
 * 阻塞(llm_chat ~3-8s)。必须在 worker 线程调, 不能在 recv_thread。
 * 返回: 0 = 成功(say_out 有效, 可能为空); <0 = 失败(say_out 置空, 调用者走降级)。 */
int agent_bridge_run(const char *user_text, char *say_out, int say_cap);
```

deepseek 网关是 agent_bridge.c 内部 static(`g_deepseek_gw`), 不经参数传——fc_worker 只传 user_text。

### 4.2 能力表(直接搬 prompt 目录, 不改)

- `g_emojiTable[21]`：neutral/happy/laughing/.../confused
- `g_actTable[23]`：walk/turn/jump/.../sit
- 两张表既是 prompt 数据源，又是 `[ACT]`/`[EMOJI]` 回复校验依据（`EmojiValid`/`ActFind`）。

### 4.3 BuildPrompt(text) — 组包

照搬 agent_service.c：模板前置 + 能力表 + 核心规则 + 三示例 + `用户说: <text>`。
- 完整 prompt ~2100 字节（实测），用户文字留足空间（llm_chat 单条上限 4096，见 §8）。
- `text` = 用户原话(经 item_id 等待拿到的正确转写, §5.1)。空则退化为 `arguments` 的 action，再空则 agent_bridge 内部用 `(empty)`。

### 4.4 ParseReply(reply) — 四行协议解析

照搬 agent_service.c：
- 逐行 `TagContent` 匹配 `[SAY]/[EMOJI]/[ACT]/[END]`（大小写/全角【】/中英冒号/引号/markdown 装饰容错）。
- `[ACT]` 可多行 → 按序执行；`ParseActLine` 抽动作名 + 整数参数；`NormalizeAct` 把 `turn -1`/`walk -1` 桥接成底层签名。
- 缺行审计（四行缺一不可，无操作应写 `none`）。

### 4.5 SalvageReply(reply) — 格式漂移兜底

照搬 agent_service.c：模型没按四行输出时，二次扫描 `expression:/emotion:/emoji:/表情:` / `action:/act:/动作:` / `say:/回复:` 抢救。救回任意一条即执行。

### 4.6 分发(执行层, 按"已实现"对待)

- `[ACT]` → `run_act(name, par, npar)`（agent_bridge 内部）。契约非阻塞、启动后立即返回。当前 stub：校验动作名在 23 表内即打印 `[agent] act ->`，未知打印 `unknown action ignored`。等同事真实实现替换（§6.3）。
- `[EMOJI]` → `send_emoji_cmd(name)`：`fopen("w")` 写一行名到 `/storage/data/emoji_cmd` 后 fclose。假设 ui_display 进程消费。镜像里没 ui_display 也照写（无害，通路打通后挂上即生效）。
- `[SAY]` → 写入 `say_out` 返回给调用者（wake_dialog 喂回音频模型）。

---

## 5. 集成点(wake_dialog.c)

### 5.1 转录捕获 — item_id 关联(关键, 实测修复的坑)

recv_thread 处理三类事件，用 `item_id` 把"同一句用户话"的音频提交与转写关联起来：

```c
static char g_last_user_transcript[1024];   /* 最近转写文本, g_mtx 保护 */
static char g_user_item_id[128];            /* 当前 user item 的 id */
static volatile int g_user_transcript_ready = 0;  /* 该 item 转写是否就绪 */
```

- `input_audio_buffer.committed`：取 `item_id` → 存 `g_user_item_id`，`ready=0`，打印 `[FC] audio committed (item_id=..., ready=0)`。
- `conversation.item.input_audio_transcription.completed`：取 `transcript` + `item_id` → 存 `g_last_user_transcript`；若 `item_id` 与 `g_user_item_id` 匹配 → `ready=1`，打印 `[FC] transcript done (item_id=..., match=..., ready=...)`。
- `handle_function_call`：**只快照 `item_id`**（不快照 transcript），打印 `[FC] fc_worker spawned (call_id=..., item_id=..., ready=...)`。

**fc_worker 轮询等待**（修"旧转写"bug 的核心）：

```c
/* 轮询等当前 user item 的转写就绪(同 item_id 且 ready=1), 最多 3s */
while (g_running && waited_ms < 3000) {
    lock; if (g_user_transcript_ready && strcmp(g_user_item_id, req->item_id)==0) { ready=1; 拷 transcript; }
    unlock; if (ready) break; usleep(50ms); waited_ms += 50;
}
if (!ready) {  /* 超时降级: 用最新转写(可能旧)或 action */
    lock; 拷 g_last_user_transcript; unlock;
}
```

> **为什么不能在 function_call 时直接读 transcript**：function-call 路径下事件顺序不固定。
> 实测两种时序都出现过：
> - 时序A：`transcription.completed` 先到(ready=1) → `function_call_arguments.done` 后到 → fc_worker `waited=0ms ready=1` 立即发
> - 时序B：`function_call_arguments.done` 先到 → `transcription.completed` 后到 → fc_worker 轮询等到 ready
>
> 若 function_call 时直接读 `g_last_user_transcript`，时序B 下会拿到**上一句旧值**
> (实测 bug：用户说"走两步"，fc_worker 却拿到上句"现在也没。"，导致 deepseek 收到错句不守四行协议)。
> item_id 关联机制两种时序都覆盖。

### 5.2 fc_worker 线程(非阻塞, 关键)

`handle_function_call` 在 recv_thread 里，改为 spawn detached 线程：

```c
static void handle_function_call(call_id, name, arguments) {
    /* 拷贝 call_id/arguments/item_id 到堆上 req(NOT transcript, §5.1), spawn detached fc_worker */
    pthread_create(&tid, &detach_attr, fc_worker, req);
}
```

`fc_worker`：等转写就绪 → `agent_bridge_run` → 拿 `[SAY]` → 写 output → WS 发 function_call_output + response.create，**全在线程里**。recv_thread 立即回去收事件，播放不中断。

> 为什么必须非阻塞：llm_chat 常态 3~8s（实测 FC 路径 deepseek ~2.5s）。若在 recv_thread 同步跑，
> 期间 `response.audio.delta` 堆 WS 缓冲、play_thread 断粮、播放卡顿。

### 5.3 function_call_output 带 [SAY]

fc_worker 调 `agent_bridge_run` 后构造 output：

```c
char say[1024];
int r = agent_bridge_run(text, say, sizeof(say));
char output[1300];
if (r == 0) {
    char esc_say[1100];
    sanitize_for_json(say, esc_say, sizeof(esc_say));  /* " \ \n \r \t → 空格 */
    snprintf(output, sizeof(output),
        "{\\\"reply\\\":\\\"%s\\\",\\\"status\\\":\\\"ok\\\"}", esc_say);
    printf("[FC] function_call_output: reply set, status=ok\n");
} else {
    snprintf(output, sizeof(output),
        "{\\\"status\\\":\\\"failed\\\",\\\"error\\\":\\\"agent_bridge %d\\\"}", r);
    printf("[FC] function_call_output: status=failed (agent_bridge %d)\n", r);
}
fc_send_result(req->call_id, output);
```

`fc_send_result` 发两条 WS：
```c
/* 1. conversation.item.create (function_call_output, 嵌套 JSON 双引号已转义) */
{"type":"conversation.item.create","item":{"type":"function_call_output",
 "call_id":"<call_id>","output":"{\"reply\":\"...\",\"status\":\"ok\"}"}}
/* 2. response.create 触发二轮 */
{"type":"response.create","response":{"modalities":["audio","text"]}}
```

### 5.4 让音频模型把 [SAY] 说出来 + 精确化 instructions

session.update 的 `instructions` 是两段（已实现, wake_dialog.c `SESSION_UPDATE_JSON`）：

**① 话路区分(精确化守门规则)**：
> 你是装在机器人上的语音助手Luna。用户说话时先判断意图:若用户明确要求做肢体动作或切换表情(如跳舞/挥手/点头/走/转身/坐/换个表情/开心点等),调用control_robot工具(action填一个粗略动作名即可,精确动作由机器人大脑决定),调用后等结果再自然回应;若是问答/闲聊/知识/天气/时间/讲故事等,不要调用control_robot,直接自己语音回答;拿不准是不是动作意图时,不要调control_robot,先正常对话澄清。

**② [SAY] 喂回规则**：
> 当control_robot工具返回结果时,把结果里reply字段的文字原样说给用户,不要改写或补充,这是机器人大脑生成的回复。语气口语化,不输出emoji和Markdown。

二轮 `response.create` 后，音频模型读 function_call_output 里的 `reply` → `response.audio.delta` → 播放队列 → 喇叭说 `[SAY]`。

### 5.5 WS send 互斥(已实现, 在 wake_ws.c)

fc_worker、send_thread、recv_thread 三者都会 `wake_ws_send_text`。互斥锁加在 **wake_ws.c 内部**（不在 wake_dialog.c）：

```c
/* wake_ws.c */
static pthread_mutex_t g_send_mtx = PTHREAD_MUTEX_INITIALIZER;
/* ws_send_frame: tls_write_all(hdr) 和 tls_write_all(masked) 都用 g_send_mtx 包 */
```

调用方（wake_dialog.c）无需加锁，`wake_ws_send_text` 已线程安全。

### 5.6 deepseek 网关配置(复用, 不改 key)

定义在 **agent_bridge.c 顶部**（不在 wake_dialog.c）：

```c
static const llm_gateway_t g_deepseek_gw = {
    .ip      = "47.102.215.139",
    .vhost   = "api.siliconflow.cn",
    .port    = 443,
    .api_key = "sk-jcojwurdcgxlgtrgaiuibttmopjzzxxrmnrfoieawqpkqwrj",  /* 同事的, 不改 */
    .model   = "deepseek-ai/DeepSeek-V4-Flash",
};
```

**只读不改**（遵守"不改同事 SiliconFlow key"约束）。agent_bridge_run 内部直接引用。

### 5.7 fc 并发互斥(已实现)

fc_worker 期间又来一次 function_call：用 `g_fc_busy` 标志（g_mtx 保护）拒绝并发，第二个直接 output=busy 跳过 deepseek，避免并发请求打爆 deepseek。

---

## 6. 执行层假设(按"已实现"对待) + 可测试性

舵机/屏幕实际未接，但整条 agent 通路要在"无硬件"下可证明通。核心思路：
**执行层全部打结构化日志(带统一前缀 `[agent]`)，用日志串联代替硬件动作验证。**
未来接真硬件时，stub 的日志行保留(无害)，真实执行代码加在日志旁即可。

### 6.1 执行层动作 + 对应日志(可观测点, 全英文)

| 步骤 | 执行 | 日志(无硬件也打) | 证明什么 |
|---|---|---|---|
| `[ACT]` | `run_act(name,par)` stub | `[agent] act -> walk 2 800 (act#1, queued, stub=no-servo)` | ParseReply 抽对了动作名+参数 |
| `[ACT]` 多行 | 按序入队 | `[agent] act -> handwave (act#2, queued, stub=no-servo)` | 复合指令拆解+排队对 |
| `[EMOJI]` | `send_emoji_cmd` 写文件 | `[agent] emoji -> happy (emoji_cmd write OK, stub=no-ui)` | 名校验过+文件写成功(通道通) |
| `[SAY]` | 返回给调用者 | `[agent] reply: <SAY 文本>` | 文本抽取对,喂回链路起点 |
| 未知名 | 忽略 | `[agent] unknown action ignored: xxx` / `unknown emoji ignored: xxx` | 校验逻辑生效 |
| 格式违规 | 兜底 | `[agent] !! reply has no tags (bad format)` + `salvaged instructions from bad format` | Salvage 兜底生效 |

> 每行带 `[agent]` 前缀和序号, 串口日志 grep `\[agent\]` 即得完整决策链。

### 6.2 链路完整性自检: 一次成功的 FC 应打出的日志序列(实测验证 2026-09-24)

无硬件也能跑、能 grep 出这条链即证明通路通。下面是实测日志(英文, 已验证):

```
[You] 走两步。                                              ← 转录(§5.1)
[FC] transcript done (item_id=item_L7oIU..., match=1, ready=1)  ← 转写就绪标记
[FC] call control_robot, call_id=call_a3df..., args={"action": "walk"}  ← 音频模型调工具(话路区分闸生效)
[FC] fc_worker spawned (call_id=call_a3df..., item_id=item_L7oIU..., ready=1)  ← 非阻塞线程起(§5.2)
[FC] fc_worker start (item_id=item_L7oIU..., ready=1, waited=0ms, text=走两步。)  ← 拿到正确转写
[agent] >> prompt 2109 bytes (user_text=12)                ← BuildPrompt 组包(§4.3)
[llm] sending 2109 bytes to deepseek...                     ← llm_chat 发出(无截断告警=未超 4KB)
[llm] received 73 bytes                                     ← deepseek 回复到
[llm] raw reply: [[SAY] 好的, 走两步给你看\n[EMOJI] none\n[ACT] walk\n[ACT] walk\n[END]]  ← 原文诊断
[agent] reply: 好的, 走两步给你看                            ← ParseReply 解出 [SAY]
[agent] act -> walk (act#1, queued, stub=no-servo)          ← ParseReply 解出 [ACT](走两步=walk×2)
[agent] act -> walk (act#2, queued, stub=no-servo)          ← 第二个 [ACT]
[FC] function_call_output: reply set, status=ok             ← 写回音频模型(§5.3)
[FC] response.create sent (round-2)                         ← 触发二轮(§5.4)
[recv] response.audio.delta ...                             ← 音频模型把 [SAY] 说出来(链路闭环)
[AI] 好的, 走两步给你看                                      ← 转录确认=喇叭说的内容=闭环
```

**证明清单**(这条日志序列每一行对应一个环节通):
1. `[You]`+`[FC] call` = 音频模型话路区分闸生效(§3), 不是每句话都走 FC
2. `[FC] transcript done match=1 ready=1` + `fc_worker start ready=1` = item_id 关联拿到**正确转写**(§5.1, 非旧值)
3. `[agent] >>`+`[llm] sending` = BuildPrompt 组包 + llm_chat 发出
4. `[llm] received` + `[llm] raw reply` = deepseek 网关+key+TLS 通 + 四行协议格式正确
5. `[agent] act/reply` = ParseReply 四行解析对(无真舵机/屏幕, 但名+参数+文本都解对了)
6. `[FC] function_call_output` + `response.create` = 喂回链路通
7. `[AI]` = 喇叭说的内容 = `[SAY]` 内容 = 闭环

> **诊断约定**：`[llm] raw reply: [...]` 用方括号包住 deepseek 原文(看清首尾边界, 截断 512 字节)。
> 若模型不守四行协议(返纯文本), 此行直接暴露原文——定位不靠猜。

### 6.3 执行层 stub 设计(可平滑换真硬件)

`agent_bridge.c` 内执行函数留**双重身份**: 既打日志(永远打, 留痕), 又调真实实现(同事接好后):

```c
/* [ACT] 执行: 当前 stub 只打日志; 同事接好后, 在日志后加真实调用。
 * 契约: 非阻塞。签名不变, 换实现不改调用方。 */
static void run_act(const char *name, const int *par, int npar) {
    printf("[agent] act -> %s", name);
    for (int i=0;i<npar;i++) printf(" %d", par[i]);
    printf(" (act#%d, queued, stub=no-servo)\n", ++g_act_seq);
    /* TODO(同事): OttoPcaRun(name, par, npar); */   /* 真硬件接这行 */
}
static void send_emoji_cmd(const char *name) {
    FILE *f = fopen("/storage/data/emoji_cmd", "w");
    if (f) { fprintf(f, "%s\n", name); fclose(f); }   /* 通道是真写的, 路径真在 */
    printf("[agent] emoji -> %s (emoji_cmd write %s, stub=no-ui)\n",
           name, f ? "OK" : "FAIL");
    /* TODO(同事): 无需改, ui_display 消费此文件 */   /* 接 ui_display 即生效 */
}
```

> emoji_cmd 是真写文件(不只是打印)——这样 ui_display 接上后零改动即生效,
> 且 stub 阶段也能验证"文件通道可写"这一事实(非纯打印)。

### 6.4 可测试性验收用例(无硬件)

| 输入 | 期望日志(关键行, 无硬件也能看) | 证明 |
|---|---|---|
| "走两步" | `[agent] act -> walk`×2 + `reply: ...` + `[AI] ...` | 完整闭环(走两步=walk×2) |
| "走两步再挥挥手" | `[agent] act -> walk (act#1)` + `act -> handwave (act#2)` | 复合指令拆解排队 |
| "换个开心的表情" | `[agent] emoji -> happy`, **无** `act ->` 行 | 只要表情不补动作(语义严格) |
| "向右转" | `[agent] act -> turn 4 1000 -1` | NormalizeAct 桥接(turn -1→右转4步) |
| "今天几号" | **无** `[FC] call`/`[agent]` 行, 只有 `[AI] <音频模型自答>` | 话路区分闸: 闲聊不走 FC |
| "你会什么" | `[agent] reply: 我会...`, 动作/表情行 `none`→不打 | 双 none 只说话 |
| 断 WiFi 说动作 | `[llm] llm_chat failed` + `[FC] function_call_output: status=failed` + `[AI] <音频模型兜底自言>` | 降级通 |
| deepseek 返纯文本 | `[agent] !! reply has no tags` + `salvaged ...` 或 `no usable instructions` | Salvage 兜底通 |

---

## 7. 边界与降级

| 情况 | 处理 |
|---|---|
| llm_chat 返回负数(连不上/超时) | agent_bridge_run 返回 <0 → output=`{"status":"failed","error":"agent_bridge <code>"}` → response.create，音频模型自言兜底 |
| 回复无任何标记(格式违规)且 SalvageReply 救不回 | output=`{"status":"failed",...}` 或 reply 为空 → 音频模型按 function_call_output 自行生成 |
| `[SAY]` 为空/缺 | output 不带 reply → 音频模型自行生成 |
| item_id 等 3s 超时(转写始终不来) | fc_worker 降级用最新 `g_last_user_transcript`(可能旧)或 `arguments` 的 action |
| fc_worker 期间又来一次 function_call | `g_fc_busy` 拒绝并发，第二个 output=busy 跳过 |
| llm_chat 阻塞期间对话空闲超时 | 超时只断 WS，fc_worker 的 llm_chat 仍会返回(结果丢弃)；不阻塞退出 |
| 音频模型误触发 control_robot(纯问答也调) | 信任模型判断，不本地拦截；instructions 已精确化降低误触发 |
| deepseek 收到错误转写(闲聊句却要决策动作) | 可能不守四行协议返纯文本 → `[llm] raw reply` 暴露 + Salvage 兜底(实测: 修好 item_id 后此情况消失) |

---

## 8. 构建集成 + LLM 缓冲改动

### 8.1 BUILD.gn

wake BUILD.gn（`executable("wake")`）：
- **sources**：删 `servo_ctl.c`，加 `agent_bridge.c` + `"//applications/llm_sdk/llm_client.c"`（**编译源码进 wake，不链接 .a**，见 §8.2）
- **include_dirs**：加 `"//third_party/mbedtls/include"`
- **libs**：已有 `_wifi_a`（libwificlient.a）；**不加** libllmclient.a

### 8.2 llm_client.c 常量放大(关键坑)

预编译 `libllmclient.a` 的 `LLM_MSG_MAX=1024`，会把 agent prompt(~1.3KB 固定部分)截断——模型看不到完整格式约束。

**解法**：把 `llm_client.c` 源码直接编译进 wake（不链接 .a），常量放大：

```c
/* llm_client.c (编译进 wake 时) */
#define LLM_MSG_MAX   4096   /* 原 1024, agent prompt 不截断 */
#define LLM_REQ_SIZE  6144   /* 原 3584, 同步放大 */
#define LLM_BODY_SIZE 5120   /* 原 3072, 同步放大 */
#define LLM_ESCJ_SIZE 8192   /* 原 2048, 同步放大 */
```

预编译 `libllmclient.a` 不受影响（`llm_test`/`customer_test` 仍用 1024）。

---

## 9. 文件改动清单(已实现)

| 文件 | 改动 |
|---|---|
| `agent_bridge.c/.h`（新增，原 servo_ctl.c 改名+扩展） | 能力表(23+21) + BuildPrompt + ParseReply + SalvageReply + send_emoji_cmd + run_act(stub) + `agent_bridge_run`（调 llm_chat + 解析 + 分发 + 返回 [SAY]）。deepseek 网关 `g_deepseek_gw` 在此文件。移植自 prompt/agent_service.c，去掉 OttoPcaRun/ShellParseInt/llm_service 依赖。 |
| `wake_dialog.c` | 加 `g_last_user_transcript`/`g_user_item_id`/`g_user_transcript_ready`(item_id 关联)；`handle_function_call` 改 spawn fc_worker(只快照 item_id)；fc_worker 轮询等转写就绪 + 调 `agent_bridge_run` + 写 output + response.create；recv 加 `committed`/`speech_stopped` 处理 + `transcription.completed` 加 item_id 匹配；session.update instructions 精确化；`g_fc_busy` 并发互斥。 |
| `wake_ws.c` | 加 `g_send_mtx` 发送互斥锁(§5.5)，ws_send_frame 内部加锁，三线程并发写帧安全。 |
| `llm_client.c`（applications/llm_sdk/） | `LLM_MSG_MAX` 1024→4096 + REQ/BODY/ESCJ 同步放大(§8.2)。 |
| `BUILD.gn` | sources 删 `servo_ctl.c`、加 `agent_bridge.c` + `llm_client.c`(编译进, 非 .a)；include_dirs 加 mbedtls。 |
| `servo_ctl.c/.h` | **删除**（并入 agent_bridge）。 |

---

## 10. 端到端时延打点(已实现, gettimeofday)

用户需求：测"喊 Luna 到出声"和"说完话到出声"的时延。用 `gettimeofday` 取毫秒，在关键事件点打点，每轮回复首帧送喇叭时算差值打印：

| 打点 | 时刻 | 日志 |
|---|---|---|
| `g_t_wake_ms` | wake_listen_loop 检测到 Luna | (检测时设) |
| `g_t_speech_stop_ms` | `input_audio_buffer.speech_stopped` | `[latency] speech stopped at %ld ms` |
| `g_t_committed_ms` | `input_audio_buffer.committed` | (设值) |
| `g_t_resp_created_ms` | `response.created` | (设值) |
| 首帧播放(play_thread) | 本轮首个 audio.delta 送喇叭 | 算差值打印(见下) |

首帧播放时打印四段(问候轮只测 wake→play 一次)：
```
[latency] wake-detected -> first-play: %ld ms        ← 喊 Luna 到出声(总延迟)
[latency] speech-stop -> first-play: %ld ms          ← 说完话(VAD静音)到出声(用户感知总延迟, 含 800ms 静音确认)
[latency] committed -> first-play: %ld ms            ← 云端拿到完整音频到出声(排除 800ms 静音确认)
[latency] response-created -> first-play: %ld ms     ← 云端开始生成到出声(TTS+网络, 排除 ASR)
```

### 10.1 实测时延(2026-09-24)

| 场景 | 时延 | 含义 |
|---|---|---|
| 唤醒→首次播放(greeting) | **1654 ms** | 含 WS 建连+session+inject+TTS |
| └ 其中 response-created→play | 373 ms | 纯 TTS+网络首帧 |
| 普通对话(今天几号)说完→播放 | **172 ms** | 纯云端处理, 极快 |
| FC 对话(走两步)说完→音频模型首轮应答 | 321 ms | 模型边说话边等函数结果 |
| FC 对话说完→deepseek 最终回复播放 | **2989 ms** | 含 deepseek ~2.5s |
| └ 其中 response-created→play | 418 ms | 二轮 TTS 首帧 |

**结论**：普通对话 172ms 体验极佳；FC 路径多出 ~2.5s 是双模型架构固有代价(deepseek 调用)。

---

## 11. 测试计划(实测结果)

1. **端到端**：`exec /bin/my_wake flow` → 喊 Luna → 说"走两步" → ✅ 已验证完整链路(§6.2 日志序列)
2. **话路区分**：说"今天几号" → ✅ 无 `[FC] call`，音频模型直接答
3. **降级**：断 WiFi 后说动作 → 期望 `[llm] llm_chat failed` + 音频模型兜底自言（待测）
4. **格式漂移**：临时改 prompt 去掉模板，看 SalvageReply 兜底日志（待测）
5. **回归**：正常对话（不涉动作）不受影响——音频模型不调 control_robot，不走 deepseek → ✅ 已验证
