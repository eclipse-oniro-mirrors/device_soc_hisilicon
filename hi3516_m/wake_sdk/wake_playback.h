/*
 * wake_playback.h - 24kHz 音频播放(实时语音对话专用)
 *
 * 独立于 my_audio,直接调 HI_MPI_AO + /dev/acodec,不依赖 media_server。
 *
 * 与 wake_capture(16kHz 采集)共用 ACodec 设备 0:I2S1 采样率由 wake_capture
 * 设为 16kHz(内置 ACodec 的 I2S1 采样率全局唯一,AI/AO 共享)。本模块 AO 设备
 * enSamplerate 也设 16kHz(硬件跑 16kHz),通过 HI_MPI_AO_EnableReSmp(...,24000)
 * 把应用送入的 24kHz PCM 重采样到 16kHz 再播放。这样:
 *   - 采集链路:16kHz 原生(不重采样),给云端
 *   - 播放链路:应用送 24kHz(云端输出格式)→ 重采样到 16kHz → 喇叭
 *   - AI/AO 时钟一致(同 16kHz),u32ClkSel=0 时双向独立工作
 *
 * 参考实现(不依赖,仅看 API 用法):
 *   device/soc/hisilicon/common/platform/mytest/audio/audio_pcm.c:441-580
 *   device/soc/hisilicon/hi3516dv300/sdk_linux/sample/platform/common/sample_comm_audio.c
 */
#ifndef WAKE_PLAYBACK_H
#define WAKE_PLAYBACK_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 云端输出音频规格:24kHz / 16bit / 单声道 PCM。
 * 应用调用 WakePlaybackWrite 送入的应为 24kHz PCM 数据。
 */
#define PLAY_SAMPLE_RATE     24000   /* 云端输出采样率(应用侧送入) */
#define PLAY_HW_SAMPLE_RATE  16000   /* 硬件实际播放采样率(ACodec I2S1) */

/*
 * 播放帧参数。
 * 硬件跑 16kHz,u32PtNumPerFrm=480(30ms@16kHz=960B)。
 * 应用送入 24kHz 数据:等时长 30ms@24kHz = 720 点 = 1440 字节/帧。
 * 驱动按 EnableReSmp(24000) 把 1440B(720点@24kHz)重采样为 960B(480点@16kHz)播放。
 */
#define PLAY_PT_PER_FRM      480                          /* 硬件帧点数@16kHz */
#define PLAY_FRAME_BYTES     (PLAY_PT_PER_FRM * 2)        /* 硬件帧字节@16kHz = 960B */
#define PLAY_IN_PT_PER_FRM   720                          /* 应用送入帧点数@24kHz(等时长) */
#define PLAY_IN_FRAME_BYTES  (PLAY_IN_PT_PER_FRM * 2)     /* 应用送入帧字节@24kHz = 1440B */

/*
 * 初始化播放链路:AO 设备使能(16kHz) + 重采样(24kHz→16kHz) + ACodec DAC 上电解静音。
 * 前提:WakeCaptureInit 已先调用(它做了 SYS_Init/VB_Init 并配置 ACodec I2S1=16kHz + ADC)。
 * 成功返回 0,失败返回 -1。
 */
int WakePlaybackInit(void);

/*
 * 向播放链路送入 24kHz PCM 数据。data 为 24kHz/16bit/单声道 PCM。
 * 内部按硬件帧大小(960 字节 = 480 点@16kHz)分块 SendFrame,不足一帧补静音。
 * 重采样(24kHz→16kHz)只改变采样点数,不改变播放时长/音调(24kHz 的一段话
 * 重采样到 16kHz 后播放,时长相同,只是采样点少了 1/3,高频上限略降)。
 * 成功返回 0,失败返回 -1。
 */
int WakePlaybackWrite(const void *data, int len);

/*
 * 清空 AO 通道缓冲(打断时立即闭嘴用)。
 */
void WakePlaybackClear(void);

/*
 * 清理播放设备(对话结束时调用)。
 */
void WakePlaybackDeinit(void);

#ifdef __cplusplus
}
#endif

#endif /* WAKE_PLAYBACK_H */
