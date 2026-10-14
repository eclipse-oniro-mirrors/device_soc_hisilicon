/*
 * agent_bridge.h - 音频模型 ↔ 文本 agent(deepseek) 桥接
 *
 * 音频模型(Qwen-Audio)的 function calling 触发后, 把用户说的话发给文本大模型
 * (deepseek, via llm_chat), 由后者按能力表+四行协议决策动作/表情/[SAY]回复。
 * 本模块负责: 组 agent prompt → llm_chat → 解析 [SAY]/[EMOJI]/[ACT]/[END]
 *           → 分发动作/表情 → 返回 [SAY] 文本(调用者喂回音频模型说出来)。
 *
 * 移植自 applications/app_test agent_service.c(参考 /code/3516/prompt/),
 * 去掉 OttoPcaRun/ShellParseInt/llm_service 依赖, 动作执行为 stub(打印, 等真实舵机接入)。
 *
 * 契约: 阻塞(llm_chat ~3-8s)。必须在 worker 线程调用, 不能在 recv_thread。
 */
#ifndef AGENT_BRIDGE_H
#define AGENT_BRIDGE_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 桥接一次 function call: user_text 填进 agent prompt 发 deepseek, 解析四行协议,
 * 分发 [ACT]/[EMOJI] 到执行层(stub), 把 [SAY] 文本经 say_out 返回给调用者
 * (调用者再写进 function_call_output 喂回音频模型说出来)。
 *
 * user_text: 用户原话(UTF-8, 可为空/NULL → 退化为固定问候)。
 * say_out  : [SAY] 文本输出缓冲(调用者分配, 建议 >= 1024)。
 * 返回     : 0 = 成功(say_out 有效, 可能为空串); <0 = 失败(say_out 置空串, 调用者走降级)。
 */
int agent_bridge_run(const char *user_text, char *say_out, int say_cap);

#ifdef __cplusplus
}
#endif

#endif /* AGENT_BRIDGE_H */
