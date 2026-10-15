/*
 * wake_capture.c - 16kHz 麦克风采集实现(语音唤醒专用)
 *
 * 链路:板载麦克风 -> Hi3516DV300 内置 ACodec(MICIN) -> AI 设备 0
 *   - ACodec 由 /dev/acodec ioctl 配置(16kHz/mic mixer/增益/上电解静音)
 *   - AI 由 HI_MPI_AI 采集 PCM 帧
 *
 * 与 my_audio 的区别:16kHz(非 8kHz)、每帧 480 点(非 320)、只采不放(无 AO)。
 * 不依赖 media_server:HI_MPI 用户态封装(libmpi.a)直接打内核 AI 驱动,
 * 无需任何后台服务进程。
 *
 * 参考实现:device/soc/hisilicon/common/platform/mytest/audio/audio_pcm.c
 *           device/soc/hisilicon/hi3516dv300/sdk_linux/sample/platform/common/sample_comm_audio.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include "hi_type.h"
#include "hi_common.h"
#include "hi_comm_aio.h"
#include "hi_comm_vb.h"
#include "mpi_audio.h"
#include "mpi_sys.h"
#include "mpi_vb.h"
#include "acodec.h"

#include "wake_capture.h"

#define ACODEC_DEV_PATH  "/dev/acodec"

/* 麦克风增益(实测 60 最佳; my_audio 推荐 10~56, 范围 -78~80) */
#define WAKE_MIC_VOL        60

/* 连续取帧超时容忍次数 */
#define FRAME_RETRY_MAX  10

/* ================= 内部工具 ================= */

/*
 * 填充 AIO 属性:16kHz/16bit/单声道/内置 ACodec/480 点每帧。
 * 采样率从 my_audio 的 8kHz 改为 16kHz(唤醒模型训练采样率)。
 */
static void WakeFillAttr(AIO_ATTR_S *attr)
{
    memset(attr, 0, sizeof(*attr));
    attr->enSamplerate = AUDIO_SAMPLE_RATE_16000;
    attr->enBitwidth = AUDIO_BIT_WIDTH_16;
    attr->enWorkmode = AIO_MODE_I2S_MASTER;
    attr->enSoundmode = AUDIO_SOUND_MODE_MONO;
    attr->u32EXFlag = 0;
    attr->u32FrmNum = WAKE_FRM_NUM;
    attr->u32PtNumPerFrm = WAKE_PT_PER_FRM;
    attr->u32ChnCnt = 1;
    attr->u32ClkSel = 0;
    attr->enI2sType = AIO_I2STYPE_INNERCODEC;
}

/*
 * 配置内置 ACodec:16kHz 采样率 + mic 输入 + ADC 上电解静音。
 * 不做软复位(保留用户音量)。DAC/输出侧不需配(唤醒只采不放)。
 */
static int WakeAcodecConfig(void)
{
    int fd = open(ACODEC_DEV_PATH, O_RDWR);
    if (fd < 0) {
        printf("[my_wake] open %s failed: %s\n", ACODEC_DEV_PATH, strerror(errno));
        return -1;
    }

    unsigned int fs = ACODEC_FS_16000;
    if (ioctl(fd, ACODEC_SET_I2S1_FS, &fs) != 0) {
        printf("[my_wake] set acodec 16kHz failed\n");
        close(fd);
        return -1;
    }

    unsigned int mixer = ACODEC_MIXER_IN1;
    if (ioctl(fd, ACODEC_SET_MIXER_MIC, &mixer) != 0) {
        printf("[my_wake] select acodec input mixer failed\n");
        close(fd);
        return -1;
    }

    /* mic 增益:设固定值(唤醒不需要用户调音量,固定增益保证输入电平一致) */
    unsigned int inVol = WAKE_MIC_VOL;
    (void)ioctl(fd, ACODEC_SET_INPUT_VOL, &inVol);

    /* ADC 上电 + 解静音:否则采到恒定直流,淹没真实声音 */
    unsigned int pd = 0;
    unsigned int unmute = 0;
    (void)ioctl(fd, ACODEC_SET_PD_ADCL, &pd);
    (void)ioctl(fd, ACODEC_SET_PD_ADCR, &pd);
    (void)ioctl(fd, ACODEC_SET_MICL_MUTE, &unmute);
    (void)ioctl(fd, ACODEC_SET_MICR_MUTE, &unmute);

    /* DAC 不配(唤醒只采不放)。但上电防驱动侧告警 */
    (void)ioctl(fd, ACODEC_SET_PD_DACL, &pd);
    (void)ioctl(fd, ACODEC_SET_PD_DACR, &pd);

    printf("[my_wake] acodec: 16kHz mic in, ADC powered up\n");
    close(fd);
    return 0;
}

/*
 * 清理残留的 AI 设备状态。
 * 进程退出不会自动关 MPP 设备,上次异常退出会让下次 Enable 报"已使能",
 * 所以每次开头先无条件关一遍,报错忽略。
 */
static void WakeResetDevices(void)
{
    (void)HI_MPI_AI_DisableChn(WAKE_AIO_DEV, WAKE_CHN_ID);
    (void)HI_MPI_AI_Disable(WAKE_AIO_DEV);
}

/*
 * 系统初始化(同 my_audio/sample):VB_Exit→VB_SetConfig→VB_Init→SYS_Init。
 * AIO 帧缓冲由驱动从 VB 分配,不做 VB_Init 时 AI_Enable 报 NOMEM。
 * 注意:HI_MPI_SYS_Exit 会影响共享 MPP 驱动的进程(media_server 若在跑),
 * 但 my_wake 独立运行不依赖 media_server,此处正常初始化即可。
 */
static int WakeSysInit(void)
{
    VB_CONFIG_S conf;
    HI_S32 ret;

    printf("[my_wake] mpp: sys init\n");
    (void)HI_MPI_SYS_Exit();
    (void)HI_MPI_VB_Exit();
    memset(&conf, 0, sizeof(conf));
    (void)HI_MPI_VB_SetConfig(&conf);
    (void)HI_MPI_VB_Init();
    ret = HI_MPI_SYS_Init();
    printf("[my_wake] mpp: SYS_Init=0x%x\n", ret);
    if (ret != HI_SUCCESS) {
        printf("[my_wake] SYS_Init failed\n");
        return -1;
    }
    return 0;
}

/* ================= 对外接口 ================= */

int WakeCaptureInit(void)
{
    AIO_ATTR_S attr;
    HI_S32 ret;

    if (WakeSysInit() != 0) {
        return -1;
    }
    WakeResetDevices();
    if (WakeAcodecConfig() != 0) {
        return -1;
    }

    WakeFillAttr(&attr);
    ret = HI_MPI_AI_SetPubAttr(WAKE_AIO_DEV, &attr);
    if (ret != HI_SUCCESS) {
        printf("[my_wake] AI_SetPubAttr failed %#x\n", ret);
        return -1;
    }
    ret = HI_MPI_AI_Enable(WAKE_AIO_DEV);
    if (ret != HI_SUCCESS) {
        printf("[my_wake] AI_Enable failed %#x\n", ret);
        return -1;
    }
    ret = HI_MPI_AI_EnableChn(WAKE_AIO_DEV, WAKE_CHN_ID);
    if (ret != HI_SUCCESS) {
        printf("[my_wake] AI_EnableChn failed %#x\n", ret);
        (void)HI_MPI_AI_Disable(WAKE_AIO_DEV);
        return -1;
    }

    printf("[my_wake] capture ready: 16kHz mono, %d pts/frame\n", WAKE_PT_PER_FRM);
    return 0;
}

int WakeCaptureRead(short *dest, int samples)
{
    AUDIO_FRAME_S frame;
    HI_S32 ret;
    int miss = 0;

    while (miss < FRAME_RETRY_MAX) {
        ret = HI_MPI_AI_GetFrame(WAKE_AIO_DEV, WAKE_CHN_ID, &frame, NULL, 1000);
        if (ret != HI_SUCCESS) {
            if (++miss < FRAME_RETRY_MAX) {
                continue;
            }
            printf("[my_wake] AI_GetFrame failed %#x (%d retries), abort\n", ret, miss);
            return -1;
        }
        break;
    }

    /* 拷贝 min(请求样本数, 帧内样本数)。u64VirAddr[0] 是声道 0 的虚拟地址 */
    int frmSamples = frame.u32Len / sizeof(short);
    int copySamples = (samples < frmSamples) ? samples : frmSamples;
    memcpy(dest, frame.u64VirAddr[0], copySamples * sizeof(short));

    (void)HI_MPI_AI_ReleaseFrame(WAKE_AIO_DEV, WAKE_CHN_ID, &frame, NULL);
    return 0;
}

void WakeCaptureDeinit(void)
{
    (void)HI_MPI_AI_DisableChn(WAKE_AIO_DEV, WAKE_CHN_ID);
    (void)HI_MPI_AI_Disable(WAKE_AIO_DEV);
}

/*
 * 运行时改麦克风输入 PGA 增益。复用 WakeAcodecConfig 里的 ACODEC_SET_INPUT_VOL,
 * 单独开 /dev/acodec 改一次即可(I2S1 采样率等不动), 不影响正在跑的 AI 采集。
 */
int WakeCaptureSetMicVol(unsigned int vol)
{
    if (vol > 0x7e) {
        printf("[my_wake] set mic vol: %u out of range (0~0x7e)\n", vol);
        return -1;
    }
    int fd = open(ACODEC_DEV_PATH, O_RDWR);
    if (fd < 0) {
        printf("[my_wake] set mic vol: open %s failed: %s\n", ACODEC_DEV_PATH, strerror(errno));
        return -1;
    }
    unsigned int v = vol;
    int ret = ioctl(fd, ACODEC_SET_INPUT_VOL, &v);
    close(fd);
    if (ret != 0) {
        printf("[my_wake] set mic vol %u failed\n", vol);
        return -1;
    }
    return 0;
}

unsigned int WakeCaptureDefaultMicVol(void)
{
    return WAKE_MIC_VOL;
}
