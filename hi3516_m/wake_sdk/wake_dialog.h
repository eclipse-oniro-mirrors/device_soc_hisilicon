/*
 * wake_dialog.h - 端到端实时语音对话(步骤 4)
 *
 * 喊 Luna 唤醒后进入对话:
 *   - 采集 16kHz PCM → base64 → WebSocket 上传阿里云
 *   - 收云端 response.audio.delta → base64 解码 → 24kHz AO 播放
 *   - server_vad 检测用户说话 → speech_started → 清播放队列 + response.cancel(打断)
 *   - energy gate 回声抑制: AI 播放期间麦克风能量低于阈值不上传(滤回声)
 *
 * 三线程(镜像 realtime_quickstart.py 的三个 asyncio task):
 *   send 线程 / recv 线程 / play 线程
 *
 * 依赖(均已验证): wake_capture / wake_playback / wake_ws / wake_base64 / wifi_client
 */
#ifndef WAKE_DIALOG_H
#define WAKE_DIALOG_H

#ifdef __cplusplus
extern "C" {
#endif

/* energy gate 默认阈值(dBFS); 可用 dialog 命令第 2 参数覆盖, 如 "dialog -18" */
#define ENERGY_GATE_DBFS_DEFAULT  -18.0f

/*
 * 进入实时对话。假定 WiFi 已连(调用前先 exec /bin/wifi_cli connect 或 wifi_test)。
 * gate_dbfs: energy gate 阈值(dBFS), AI 播放期间麦克风能量低于此值不上传(滤回声);
 *            传 ENERGY_GATE_DBFS_DEFAULT 用默认值。
 * mic_vol:   对话期间麦克风 PGA 增益(0~0x7e), 降低可减小噪声底噪、减少云端 VAD
 *            把噪声误判为"用户说话"导致自言自语; 传 WakeCaptureDefaultMicVol() 用唤醒校准值。
 * 返回 0 正常退出, 非 0 失败。
 */
int RunDialog(float gate_dbfs, unsigned int mic_vol);

/*
 * 唤醒→对话主循环(用户用 exec /bin/my_wake flow 调用):
 *   监听 "Luna" → 检测到后连云端, Luna 主动开口 → 对话(空闲 30s 回唤醒)→ 循环。
 *   假定 WiFi 已连。gate_dbfs/mic_vol 同 RunDialog; 对话结束自动恢复唤醒校准增益。
 * 返回 0 正常退出, 非 0 失败。
 */
int RunWakeFlow(float gate_dbfs, unsigned int mic_vol);

#ifdef __cplusplus
}
#endif

#endif /* WAKE_DIALOG_H */
