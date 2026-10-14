/*
 * wake_capture.h - 16kHz 麦克风采集(语音唤醒专用)
 *
 * 独立于 my_audio,直接调 HI_MPI_AI + /dev/acodec,不依赖 media_server。
 * 采样率 16kHz(唤醒模型训练采样率),单声道 16bit,每帧 480 点 = 30ms,
 * 对齐 TFLM 一次 invoke 消耗的 3 个 10ms feature step。
 */
#ifndef WAKE_CAPTURE_H
#define WAKE_CAPTURE_H

#ifdef __cplusplus
extern "C" {
#endif

/* 采样率 16kHz —— 唤醒模型训练采样率,奈奎斯特极限 8kHz,覆盖人声全频段 */
#define WAKE_SAMPLE_RATE    16000
/* 每帧 480 点 = 30ms@16kHz = 3 个 10ms 前端 step,对齐一次 TFLM invoke */
#define WAKE_PT_PER_FRM     480
/* 每帧字节数:480 × 2(16bit 单声道)= 960 */
#define WAKE_FRAME_BYTES    (WAKE_PT_PER_FRM * 2)
/* 驱动侧缓冲帧数,与 my_audio 一致 */
#define WAKE_FRM_NUM        30

/* AIO 设备/通道号(内置 ACodec 固定挂在 AI 设备 0) */
#define WAKE_AIO_DEV        0
#define WAKE_CHN_ID         0

/*
 * 初始化采集链路:HI_MPI 系统初始化 + ACodec 16kHz 配置 + AI 设备使能。
 * 成功返回 0,失败返回 -1。不依赖 media_server,直接静态链 libmpi.a。
 */
int WakeCaptureInit(void);

/*
 * 阻塞取一帧 PCM 数据。dest 至少能装 WAKE_PT_PER_FRM 个 int16_t。
 * 成功返回 0,失败返回 -1。
 */
int WakeCaptureRead(short *dest, int samples);

/*
 * 清理采集设备(进程退出前调用,避免残留设备状态影响下次运行)。
 */
void WakeCaptureDeinit(void);

/*
 * 运行时设置麦克风输入 PGA 增益(ACODEC_SET_INPUT_VOL)。
 * vol 范围 0~0x7e(0~126), 值越大增益越高; 0x7f=静音。
 * 无需重新初始化 ACodec, 可在对话/唤醒期间随时调。成功返回 0, 失败 -1。
 * 用途: 对话时降低增益减小噪声底噪, 减少云端 VAD 把噪声误判为"用户说话"导致自言自语。
 */
int WakeCaptureSetMicVol(unsigned int vol);

/* 唤醒词校准用的默认麦克风增益(WAKE_MIC_VOL=60)。对话结束后恢复此值保证唤醒灵敏度。 */
unsigned int WakeCaptureDefaultMicVol(void);

#ifdef __cplusplus
}
#endif

#endif /* WAKE_CAPTURE_H */
