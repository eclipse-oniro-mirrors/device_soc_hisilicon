/*
 * agent_bridge.c - 文本 agent(deepseek) 桥接实现
 *
 * 移植自 applications/app_test agent_service.c(参考 /code/3516/prompt/),
 * 适配到 wake: 用 llm_chat 直连 deepseek, 动作/表情执行为 stub(打印留痕, 等真实硬件)。
 * 所有日志英文(数据内容如 [SAY] 回复保持原文)。
 *
 * 能力表(23 动作 + 21 表情)与 prompt 模板与 prompt/agent_service.c 一致, 不改。
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <unistd.h>

#include "agent_bridge.h"
#include "llm_client.h"      /* llm_chat / llm_gateway_t */

/* ============ 配置 ============ */

#define BRIDGE_PROMPT_MAX   4096   /* agent prompt 组包缓冲(llm_chat 消息上限 4096) */
#define BRIDGE_REPLY_MAX    4096   /* deepseek 回复缓冲 */
#define BRIDGE_SAY_MAX      1024   /* [SAY] 文本缓冲(喂回音频模型) */
#define BRIDGE_NAME_MAX     24     /* 动作/表情名最大长度 */
#define BRIDGE_ACT_ARG_MAX  4      /* 动作整数参数上限 */
#define EMOJI_CMD_FILE      "/storage/data/emoji_cmd"   /* 跨进程表情命令文件(ui_display 消费) */

/* deepseek 网关(复用同事 SiliconFlow key, 只读不改; 照 customer_test.c) */
static const llm_gateway_t g_deepseek_gw = {
    .ip      = "47.102.215.139",
    .vhost   = "api.siliconflow.cn",
    .port    = 443,
    .api_key = "sk-jcojwurdcgxlgtrgaiuibttmopjzzxxrmnrfoieawqpkqwrj",
    .model   = "deepseek-ai/DeepSeek-V4-Flash",
};

/* ============【能力表: 与 prompt/agent_service.c 一致】============ */

static const struct AgentEmoji {
    const char *name;
    const char *desc;
} g_emojiTable[] = {
    { "neutral",      "平静" },
    { "happy",        "开心" },
    { "laughing",     "大笑" },
    { "funny",        "滑稽" },
    { "sad",          "难过" },
    { "angry",        "生气" },
    { "crying",       "哭泣" },
    { "loving",       "喜爱" },
    { "embarrassed",  "尴尬" },
    { "surprised",    "惊讶" },
    { "shocked",      "震惊" },
    { "thinking",     "思考" },
    { "winking",      "调皮" },
    { "cool",         "得意" },
    { "relaxed",      "放松" },
    { "delicious",    "美味" },
    { "kissy",        "飞吻" },
    { "confident",    "自信" },
    { "sleepy",       "困倦" },
    { "silly",        "搞怪" },
    { "confused",     "困惑" },
};
#define EMOJI_N (sizeof(g_emojiTable) / sizeof(g_emojiTable[0]))

static const struct AgentAct {
    const char *name;
    const char *desc;
} g_actTable[] = {
    { "walk",      "向前走步" },
    { "turn",      "原地转身踏步" },
    { "jump",      "原地蹲站跳" },
    { "bend",      "左右侧弯身" },
    { "shakeleg",  "抬起单腿摇" },
    { "updown",    "身体一起一落" },
    { "swing",     "左右摇摆身体" },
    { "tiptoe",    "踮脚小幅摆动" },
    { "jitter",    "原地抖动" },
    { "ascend",    "螺旋升天转圈" },
    { "crusaito",  "十字舞步(会位移)" },
    { "flapping",  "拍翅膀舞步(会位移)" },
    { "moonwalk",  "太空步滑行" },
    { "whirlwind", "旋风踢腿" },
    { "handsup",   "举起手不归位" },
    { "handsdown", "把手放下" },
    { "handwave",  "原地挥手(脚不动)" },
    { "windmill",  "手臂大风车" },
    { "takeoff",   "起飞扑扇舞步" },
    { "fitness",   "原地健身操" },
    { "greeting",  "转身挥手打招呼(会转身体)" },
    { "shy",       "侧身害羞扭捏" },
    { "sit",       "坐下(保持坐姿不归位)" },
};
#define ACT_N (sizeof(g_actTable) / sizeof(g_actTable[0]))

/* ---- 状态 ---- */
static char g_prompt[BRIDGE_PROMPT_MAX];
static char g_reply[BRIDGE_REPLY_MAX];
static char g_say_buf[BRIDGE_SAY_MAX];   /* ParseReply 填 [SAY]; agent_bridge_run 拷给调用者 */
static int  g_act_seq = 0;               /* 动作序号(日志用) */

/* ============【能力 prompt 组包】============ */

typedef const char *(*EntryNameFn)(const void *entry);
typedef const char *(*EntryDescFn)(const void *entry);

static void BuildNameDescList(const void *table, size_t n, size_t stride,
                              EntryNameFn nameFn, EntryDescFn descFn,
                              char *dst, size_t size)
{
    size_t i, used = 0;
    dst[0] = '\0';
    for (i = 0; i < n; i++) {
        const char *item0 = nameFn((const char *)table + i * stride);
        const char *item1 = descFn((const char *)table + i * stride);
        char item[64];
        (void)snprintf(item, sizeof(item), "%s%s(%s)", (i > 0) ? "," : "", item0, item1);
        if (used + strlen(item) + 1 >= size) break;
        (void)memcpy(dst + used, item, strlen(item) + 1);
        used += strlen(item);
    }
}

static const char *EmojiName(const void *e) { return ((const struct AgentEmoji *)e)->name; }
static const char *EmojiDesc(const void *e) { return ((const struct AgentEmoji *)e)->desc; }
static const char *ActName(const void *e)   { return ((const struct AgentAct *)e)->name; }
static const char *ActDesc(const void *e)   { return ((const struct AgentAct *)e)->desc; }

static void BuildActList(char *dst, size_t size)
{
    BuildNameDescList(g_actTable, ACT_N, sizeof(g_actTable[0]), ActName, ActDesc, dst, size);
}
static void BuildEmojiList(char *dst, size_t size)
{
    BuildNameDescList(g_emojiTable, EMOJI_N, sizeof(g_emojiTable[0]), EmojiName, EmojiDesc, dst, size);
}

/* 组一条完整消息(模板前置 + 能力表 + 核心规则 + 三示例 + 用户文字)。
 * 中文 prompt(给模型的指令, 非日志)。 */
static const char *BuildPrompt(const char *text)
{
    char actList[640];
    char emojiList[768];
    BuildActList(actList, sizeof(actList));
    BuildEmojiList(emojiList, sizeof(emojiList));
    (void)snprintf(g_prompt, sizeof(g_prompt),
        "你是六足机器人板载大脑。必须严格按[模板]四行回复, "
        "除这四行禁止输出任何其它内容(包括思考过程、解释、其它字段)。\n"
        "[模板]\n"
        "[SAY] 给用户的中文回复(一行)\n"
        "[EMOJI] 表情名或none\n"
        "[ACT] 动作名或none(要多个动作就写多行[ACT], 按顺序执行)\n"
        "[END]\n"
        "[能力-表情] %s\n"
        "[能力-动作] %s\n"
        "核心规则: 只执行用户明确要求的操作并与语义严格对上: "
        "只要表情就不写动作, 只要动作就不写表情, 都没要就都写none; "
        "禁止补动作/补表情, 禁止默认兜底(没提情绪就none); "
        "名字必须原样用表中英文小写名, 禁止自造或写中文; "
        "反向动作加-1(turn -1=右转, walk -1=后退); "
        "表情按中文含义选最贴合的一个, 不确定就不选。\n"
        "示例-用户: 换个开心的表情\n"
        "[SAY] 好的, 给你一个开心的表情\n"
        "[EMOJI] happy\n"
        "[ACT] none\n"
        "[END]\n"
        "示例-用户: 做个思考的表情, 然后向左转\n"
        "[SAY] 让我想想, 这就向左转\n"
        "[EMOJI] thinking\n"
        "[ACT] turn\n"
        "[END]\n"
        "示例-用户: 你会什么\n"
        "[SAY] 我会21种表情和23个动作, 试试让我开心地走两步!\n"
        "[EMOJI] none\n"
        "[ACT] none\n"
        "[END]\n"
        "用户说: %s",
        emojiList, actList, (text != NULL && text[0] != '\0') ? text : "(empty)");
    return g_prompt;
}

/* ============【解析辅助】============ */

static const char *LineTrim(const char *s)
{
    while (*s == ' ' || *s == '\t' || *s == '*' || *s == '-' || *s == '`') s++;
    return s;
}

static const char *ContentTrim(const char *s)
{
    for (;;) {
        if (*s == ' ' || *s == '\t' || *s == ':' || *s == '"') { s++; continue; }
        if ((unsigned char)s[0] == 0xEF && (unsigned char)s[1] == 0xBC &&
            (unsigned char)s[2] == 0x9A) { s += 3; continue; }  /* 全角冒号 */
        break;
    }
    return s;
}

/* 行首是否为指定标记(大小写不敏感, 兼容半角[]与全角【】)。是则返回标记后内容, 否则 NULL。 */
static const char *TagContent(const char *s, const char *tag)
{
    size_t n = strlen(tag);
    if (s[0] == '[') {
        if (strncasecmp(s + 1, tag, n) != 0) return NULL;
        s += 1 + n;
    } else if ((unsigned char)s[0] == 0xE3 && (unsigned char)s[1] == 0x80 &&
               (unsigned char)s[2] == 0x90) {  /* 全角【 */
        if (strncasecmp(s + 3, tag, n) != 0) return NULL;
        s += 3 + n;
    } else {
        return NULL;
    }
    if (*s == ']' || *s == '\0' || *s == ' ' || *s == '\t' || *s == ':') {
        if (*s == ']') s++;
    } else if ((unsigned char)s[0] == 0xE3 && (unsigned char)s[1] == 0x80 &&
               (unsigned char)s[2] == 0x91) {  /* 全角】 */
        s += 3;
    } else {
        return NULL;
    }
    return ContentTrim(s);
}

static void FirstToken(const char *s, char *out, size_t size)
{
    size_t used = 0;
    while (*s == ' ' || *s == '\t' || *s == '"') s++;
    while (*s != '\0' && *s != ' ' && *s != '\t' && *s != '"' && used + 1 < size) {
        out[used++] = *s++;
    }
    out[used] = '\0';
}

/* turn/walk 的 ±1 方向桥接(见 agent_service.c NormalizeAct) */
static void NormalizeAct(const char *name, int *par, int *npar)
{
    int dir;
    if (strcmp(name, "turn") != 0 && strcmp(name, "walk") != 0) return;
    if (*npar < 1 || (par[0] != 1 && par[0] != -1)) return;
    dir = par[0];
    if (*npar >= 2) {
        par[0] = (par[1] > 0) ? par[1] : 4;
        par[1] = 1000;
        par[2] = dir;
        *npar = 3;
    } else {
        par[0] = 4;
        par[1] = 1000;
        par[2] = dir;
        *npar = 3;
    }
}

static int EmojiValid(const char *name)
{
    size_t i;
    for (i = 0; i < EMOJI_N; i++) {
        if (strcmp(g_emojiTable[i].name, name) == 0) return 1;
    }
    return 0;
}

static const struct AgentAct *ActFind(const char *name)
{
    size_t i;
    for (i = 0; i < ACT_N; i++) {
        if (strcmp(g_actTable[i].name, name) == 0) return &g_actTable[i];
    }
    return NULL;
}

/* ============【执行层 stub: 打印留痕, 等真实硬件】============ */

/* [ACT] 执行 stub: 校验动作名后打印。非阻塞(立即返回)。
 * TODO(同事接真舵机): 在日志后加 OttoPcaRun(name, par, npar); 签名不变。 */
static void run_act(const char *name, const int *par, int npar)
{
    printf("[agent] act -> %s", name);
    for (int i = 0; i < npar; i++) printf(" %d", par[i]);
    printf(" (act#%d, queued, stub=no-servo)\n", ++g_act_seq);
}

/* [EMOJI] 执行: 真写文件(通道是真写的, ui_display 接上即生效), 并打印。 */
static void send_emoji_cmd(const char *name)
{
    FILE *f = fopen(EMOJI_CMD_FILE, "w");
    if (f != NULL) {
        (void)fprintf(f, "%s\n", name);
        (void)fclose(f);
    }
    printf("[agent] emoji -> %s (emoji_cmd write %s, stub=no-ui)\n",
           name, (f != NULL) ? "OK" : "FAIL");
}

/* 解析 [ACT] 内容: 动作名 + 最多 ACT_ARG_MAX 个整数参数(用 strtol 替代 ShellParseInt) */
static void ParseActLine(const char *rest, char *name, size_t size,
                         int *par, int *npar)
{
    char word[32];
    size_t used = 0;
    const char *s = rest;
    *npar = 0;
    s = LineTrim(s);
    while (*s != '\0' && *s != ' ' && *s != '\t' && *s != '"' && used + 1 < size) {
        name[used++] = *s++;
    }
    name[used] = '\0';
    for (;;) {
        while (*s == ' ' || *s == '\t' || *s == '"') s++;
        if (*s == '\0' || *npar >= BRIDGE_ACT_ARG_MAX) break;
        used = 0;
        while (*s != '\0' && *s != ' ' && *s != '\t' && *s != '"' && used + 1 < sizeof(word)) {
            word[used++] = *s++;
        }
        word[used] = '\0';
        if (word[0] != '\0') {
            char *end = NULL;
            long val = strtol(word, &end, 10);
            if (end != word) {  /* 成功解析了数字 */
                par[(*npar)++] = (int)val;
            }
        }
    }
}

/* ============【解析与分发】============ */

/*
 * 扫回复逐行解析分发。say_buf 填 [SAY] 文本。
 * 返回 0=完整格式(见到[END]) 1=有标记但缺[END] -1=无任何标记。
 */
static int ParseReply(const char *reply)
{
    const char *p = reply;
    const char *c;
    char line[256];
    char name[BRIDGE_NAME_MAX];
    int sawEnd = 0, sawTag = 0, sawSay = 0, sawEmoji = 0, sawAct = 0;

    g_say_buf[0] = '\0';
    while (*p != '\0' && !sawEnd) {
        size_t used = 0;
        while (*p != '\0' && *p != '\n' && used + 1 < sizeof(line)) {
            line[used++] = *p++;
        }
        line[used] = '\0';
        if (*p == '\n') p++;
        c = LineTrim(line);
        if (c[0] == '\0') continue;
        if (TagContent(c, "END") != NULL) {
            sawEnd = 1; sawTag = 1; continue;
        }
        c = TagContent(c, "SAY");
        if (c != NULL) {
            if (c[0] != '\0') {
                printf("[agent] reply: %s\n", c);
                (void)snprintf(g_say_buf, sizeof(g_say_buf), "%s", c);
            }
            sawSay = 1; sawTag = 1; continue;
        }
        c = TagContent(LineTrim(line), "EMOJI");
        if (c != NULL) {
            FirstToken(c, name, sizeof(name));
            sawEmoji = 1; sawTag = 1;
            if (name[0] == '\0' || strcmp(name, "none") == 0) continue;
            if (!EmojiValid(name)) {
                printf("[agent] unknown emoji ignored: %s\n", name);
                continue;
            }
            send_emoji_cmd(name);
            continue;
        }
        c = TagContent(LineTrim(line), "ACT");
        if (c != NULL) {
            int par[BRIDGE_ACT_ARG_MAX];
            int npar = 0;
            ParseActLine(c, name, sizeof(name), par, &npar);
            sawAct = 1; sawTag = 1;
            if (name[0] == '\0' || strcmp(name, "none") == 0) continue;
            if (ActFind(name) == NULL) {
                printf("[agent] unknown action ignored: %s\n", name);
                continue;
            }
            NormalizeAct(name, par, &npar);
            run_act(name, par, npar);
            continue;
        }
        /* 其余行忽略 */
    }
    if (!sawTag) {
        if (reply[0] != '\0') {
            printf("[agent] !! reply has no tags (bad format)\n");
        }
        return -1;
    }
    if (!sawSay)   printf("[agent] !! missing [SAY] line\n");
    if (!sawEmoji) printf("[agent] !! missing [EMOJI] line (should be none)\n");
    if (!sawAct)   printf("[agent] !! missing [ACT] line (should be none)\n");
    if (!sawEnd)   printf("[agent] !! missing [END] line, parsed anyway\n");
    return sawEnd ? 0 : 1;
}

/* ---- 兜底扫描(格式漂移) ---- */
static const char *WordValue(const char *s, const char *word)
{
    size_t n = strlen(word);
    if (strncasecmp(s, word, n) != 0) return NULL;
    s = ContentTrim(s + n);
    return (*s != '\0') ? s : NULL;
}

static const char *SalvTrim(const char *s)
{
    s = LineTrim(s);
    while ((*s >= '0' && *s <= '9') || *s == '.' || *s == ')') s++;
    if ((unsigned char)s[0] == 0xEF && (unsigned char)s[1] == 0xBC &&
        ((unsigned char)s[2] == 0x88 || (unsigned char)s[2] == 0x89)) {
        s += 3;
    } else if ((unsigned char)s[0] == 0xE3 && (unsigned char)s[1] == 0x80 &&
               (unsigned char)s[2] == 0x82) {
        s += 3;
    }
    return LineTrim(s);
}

static const char *SalvageEmojiVal(const char *c)
{
    const char *v = WordValue(c, "expression");
    if (v == NULL) v = WordValue(c, "emotion");
    if (v == NULL) v = WordValue(c, "emoji");
    if (v == NULL) v = WordValue(c, "\xE8\xA1\xA8\xE6\x83\x85");        /* 表情 */
    if (v == NULL) v = WordValue(c, "\xE8\xA1\xA8\xE6\x83\x85");        /* 表情 */
    return v;
}

static const char *SalvageActVal(const char *c)
{
    const char *v = WordValue(c, "action");
    if (v == NULL) v = WordValue(c, "act");
    if (v == NULL) v = WordValue(c, "\xE5\x8A\xA8\xE4\xBD\x9C");        /* 动作 */
    if (v == NULL) v = WordValue(c, "\xE5\x8B\x95\xE4\xBD\x9C");        /* 动作 */
    return v;
}

static const char *SalvageSayVal(const char *c)
{
    const char *v = WordValue(c, "say");
    if (v == NULL) v = WordValue(c, "\xE5\x9B\x9E\xE5\xA4\x8D");        /* 回复 */
    if (v == NULL) v = WordValue(c, "\xE6\x83\xB3\xE6\xB3\x95");        /* 想法 */
    return v;
}

static int SalvageReply(const char *reply)
{
    const char *p = reply;
    char line[256];
    char name[BRIDGE_NAME_MAX];
    int hit = 0;
    while (*p != '\0') {
        const char *c;
        const char *v;
        size_t used = 0;
        while (*p != '\0' && *p != '\n' && used + 1 < sizeof(line)) {
            line[used++] = *p++;
        }
        line[used] = '\0';
        if (*p == '\n') p++;
        c = SalvTrim(line);
        if (c[0] == '\0') continue;
        v = SalvageEmojiVal(c);
        if (v != NULL) {
            FirstToken(v, name, sizeof(name));
            if (name[0] != '\0' && EmojiValid(name)) {
                send_emoji_cmd(name);
                hit = 1;
            }
            continue;
        }
        v = SalvageActVal(c);
        if (v != NULL) {
            int par[BRIDGE_ACT_ARG_MAX];
            int npar = 0;
            ParseActLine(v, name, sizeof(name), par, &npar);
            if (name[0] != '\0' && ActFind(name) != NULL) {
                NormalizeAct(name, par, &npar);
                run_act(name, par, npar);
                hit = 1;
            }
            continue;
        }
        v = SalvageSayVal(c);
        if (v != NULL && v[0] != '\0') {
            printf("[agent] reply(salvage): %s\n", v);
            (void)snprintf(g_say_buf, sizeof(g_say_buf), "%s", v);
            hit = 1;
            continue;
        }
    }
    return hit;
}

/* ============【对外接口】============ */

int agent_bridge_run(const char *user_text, char *say_out, int say_cap)
{
    if (say_out == NULL || say_cap <= 0) return -6;
    say_out[0] = '\0';
    g_say_buf[0] = '\0';
    g_act_seq = 0;

    const char *prompt = BuildPrompt(user_text);
    int plen = (int)strlen(prompt);
    printf("[agent] >> prompt %d bytes (user_text=%d)\n", plen,
           (user_text != NULL) ? (int)strlen(user_text) : 0);
    if (plen > 4000) {
        printf("[agent] !! prompt near/over limit, may truncate\n");
    }

    printf("[llm] sending %d bytes to deepseek...\n", plen);
    int n = llm_chat(&g_deepseek_gw, prompt, g_reply, sizeof(g_reply));
    if (n < 0) {
        printf("[llm] llm_chat failed: %d\n", n);
        return n;
    }
    printf("[llm] received %d bytes\n", n);
    /* 打印原始返回(诊断: 确认模型是否守四行协议 / 是否含思考标签 / 是否被截断)。
     * 不猜, 看原文。用方括号包住以看清首尾边界(含尾换行), 截断到 512 字节防刷屏。 */
    printf("[llm] raw reply: [%.*s]\n", (n < 512 ? n : 512), g_reply);

    int r = ParseReply(g_reply);
    if (r < 0) {
        if (SalvageReply(g_reply) > 0) {
            printf("[agent] salvaged instructions from bad format\n");
        } else {
            printf("[agent] no usable instructions\n");
        }
    }
    (void)snprintf(say_out, say_cap, "%s", g_say_buf);
    return 0;
}
