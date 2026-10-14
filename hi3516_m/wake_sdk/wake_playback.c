/*
 * wake_playback.c - 24kHz 音频播放实现(实时语音对话专用)
 *
 * 链路:云端 24kHz PCM(应用送入)→ HI_MPI_AO(硬件 16kHz + EnableReSmp 24k→16k)
 *   → ACodec DAC → 喇叭
 *
 * 与 wake_capture 共用 ACodec 设备 0:I2S1 采样率由 wake_capture 设为 16kHz(ACodec
 * I2S1 全局唯一,AI/AO 共享)。AO 设备 enSamplerate 也设 16kHz,再 EnableReSmp(24000)
 * 把应用送入的 24kHz PCM 重采样到 16kHz 播放。u32ClkSel=0 时 AI/AO 时钟独立,可双向工作。
 *
 * 不依赖 my_audio:参考 audio_pcm.c 的 AO API 用法,独立实现,不复用代码。
 * 不依赖 media_server:HI_MPI 用户态封装(libmpi.a)直接打内核 AO 驱动。
 *
 * 参考实现:
 *   device/soc/hisilicon/common/platform/mytest/audio/audio_pcm.c:441-580
 *   device/soc/hisilicon/hi3516dv300/sdk_linux/sample/platform/common/sample_comm_audio.c:979-1036
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <dlfcn.h>

#include "hi_type.h"
#include "hi_common.h"
#include "hi_comm_aio.h"
#include "hi_comm_vb.h"
#include "mpi_audio.h"
#include "mpi_sys.h"
#include "mpi_vb.h"
#include "acodec.h"

#include "wake_playback.h"

#define ACODEC_DEV_PATH  "/dev/acodec"

/* AO 设备/通道(与 AI 共用设备 0,通道用 0;系统通道 AO_SYSCHN_CHNID 不用) */
#define PLAY_AIO_DEV     0
#define PLAY_CHN_ID      0

/* 驱动侧缓冲帧数(与采集一致) */
#define PLAY_FRM_NUM     30

/* 连续送帧失败容忍次数 */
#define PLAY_RETRY_MAX   10

/* ================= 内部工具 ================= */

/*
 * 填充 AO AIO 属性:16kHz(硬件跑 16kHz,对齐 ACodec I2S1)/16bit/单声道/
 * 内置 ACodec/480 点每帧。应用侧送 24kHz,靠 EnableReSmp 转。
 */
static void PlayFillAttr(AIO_ATTR_S *attr)
{
    memset(attr, 0, sizeof(*attr));
    attr->enSamplerate = AUDIO_SAMPLE_RATE_16000;  /* 硬件采样率,对齐 ACodec I2S1 */
    attr->enBitwidth = AUDIO_BIT_WIDTH_16;
    attr->enWorkmode = AIO_MODE_I2S_MASTER;
    attr->enSoundmode = AUDIO_SOUND_MODE_MONO;
    attr->u32EXFlag = 0;
    attr->u32FrmNum = PLAY_FRM_NUM;
    attr->u32PtNumPerFrm = PLAY_PT_PER_FRM;
    attr->u32ChnCnt = 1;
    attr->u32ClkSel = 0;  /* AI/AO 时钟独立 */
    attr->enI2sType = AIO_I2STYPE_INNERCODEC;
}

/*
 * 配置 ACodec DAC 侧:上电 + 解静音 + 数字增益。
 * ADC 侧已由 wake_capture 配好(16kHz/mixer/增益/上电),此处只补 DAC。
 * 采样率不重复设(wake_capture 已设 16kHz,ACodec I2S1 全局唯一)。
 */
static int PlayAcodecDacConfig(void)
{
    int fd = open(ACODEC_DEV_PATH, O_RDWR);
    if (fd < 0) {
        printf("[my_wake] play: open %s failed: %s\n", ACODEC_DEV_PATH, strerror(errno));
        return -1;
    }

    /* DAC 上电 + 解静音(参考 audio_pcm.c:136-140) */
    unsigned int pd = 0;       /* 0 = power up */
    unsigned int unmute = 0;
    (void)ioctl(fd, ACODEC_SET_PD_DACL, &pd);
    (void)ioctl(fd, ACODEC_SET_PD_DACR, &pd);
    (void)ioctl(fd, ACODEC_SET_DACL_MUTE, &unmute);
    (void)ioctl(fd, ACODEC_SET_DACR_MUTE, &unmute);

    /* DAC 数字增益:0x7e 最大非静音(0x7f 为静音) */
    ACODEC_VOL_CTRL dv;
    memset(&dv, 0, sizeof(dv));
    dv.vol_ctrl = 0x7e;
    if (ioctl(fd, ACODEC_SET_DACL_VOL, &dv) != 0 ||
        ioctl(fd, ACODEC_SET_DACR_VOL, &dv) != 0) {
        printf("[my_wake] play: set DAC digital vol failed\n");
    }

    /* 输出音量:设默认值 0dB(用户可用 my_audio vol 调整,此处不强设) */
    unsigned int outVol = 0;
    (void)ioctl(fd, ACODEC_SET_OUTPUT_VOL, &outVol);

    printf("[my_wake] play: DAC powered up, unmuted, digital vol 0x7e\n");
    close(fd);
    return 0;
}

/*
 * 清理残留 AO 设备状态(同 wake_capture 思路:开头无条件关一遍)。
 * 注意:不能关 AI 设备(采集在跑),只关 AO。
 */
static void PlayResetDevices(void)
{
    (void)HI_MPI_AO_DisableChn(PLAY_AIO_DEV, PLAY_CHN_ID);
    (void)HI_MPI_AO_Disable(PLAY_AIO_DEV);
}

/* ================= 对外接口 ================= */

/*
 * 预加载重采样依赖的 .so(绝对路径 dlopen)。
 *
 * 问题:HI_MPI_AO_EnableReSmp 内部 dlopen("libhive_RES.so") 用裸文件名,
 * 靠 musl 搜索路径找。板上 shell execve 传 NULL envp(见 shmsg.c:378),
 * LD_LIBRARY_PATH 不传给 my_wake,而 musl 的 sys_path 是否含 /usr/lib
 * 取决于 /etc/ld-musl-arm.path 是否存在——实测搜不到。
 *
 * 解法:先按绝对路径 dlopen 加载,RTLD_GLOBAL 让符号可解析。
 * musl load_library 的"已加载"缓存按 shortname 匹配(dynlink.c:1132-1136),
 * 后续裸名 dlopen 直接命中缓存,不再走路径搜索。
 *
 * 同时是诊断:如果绝对路径 dlopen 也失败,dlerror() 暴露真正原因。
 */
static int PreloadResampleLibs(void)
{
    const char *libs[] = {
        "/usr/lib/libsecurec.so",
        "/usr/lib/libhive_RES.so",
        NULL,
    };
    for (int i = 0; libs[i]; i++) {
        void *h = dlopen(libs[i], RTLD_NOW | RTLD_GLOBAL);
        if (h == NULL) {
            const char *err = dlerror();
            printf("[my_wake] play: preload %s FAILED: %s\n",
                   libs[i], err ? err : "(no dlerror)");
            /* 试 /lib 备用路径 */
            char alt[64];
            snprintf(alt, sizeof(alt), "/lib/%s", strrchr(libs[i], '/') + 1);
            h = dlopen(alt, RTLD_NOW | RTLD_GLOBAL);
            if (h == NULL) {
                err = dlerror();
                printf("[my_wake] play: preload %s FAILED: %s\n",
                       alt, err ? err : "(no dlerror)");
                return -1;
            }
            printf("[my_wake] play: preloaded %s OK (from /lib)\n", alt);
        } else {
            printf("[my_wake] play: preloaded %s OK\n", libs[i]);
        }
        /* 不 dlclose:保持加载状态,裸名 dlopen 才能命中缓存 */
    }
    return 0;
}

int WakePlaybackInit(void)
{
    AIO_ATTR_S attr;
    HI_S32 ret;

    PlayResetDevices();
    if (PlayAcodecDacConfig() != 0) {
        return -1;
    }

    /* 预加载重采样 .so(必须在 EnableReSmp 之前) */
    if (PreloadResampleLibs() != 0) {
        printf("[my_wake] play: preload resample libs failed, EnableReSmp will likely fail\n");
        /* 不 return:继续试 EnableReSmp,让 MPP 的错误码也打出来对照 */
    }

    PlayFillAttr(&attr);
    ret = HI_MPI_AO_SetPubAttr(PLAY_AIO_DEV, &attr);
    if (ret != HI_SUCCESS) {
        printf("[my_wake] play: AO_SetPubAttr failed %#x\n", ret);
        return -1;
    }
    ret = HI_MPI_AO_Enable(PLAY_AIO_DEV);
    if (ret != HI_SUCCESS) {
        printf("[my_wake] play: AO_Enable failed %#x\n", ret);
        return -1;
    }
    ret = HI_MPI_AO_EnableChn(PLAY_AIO_DEV, PLAY_CHN_ID);
    if (ret != HI_SUCCESS) {
        printf("[my_wake] play: AO_EnableChn failed %#x\n", ret);
        (void)HI_MPI_AO_Disable(PLAY_AIO_DEV);
        return -1;
    }

    /* 重采样:应用送入 24kHz → 硬件 16kHz 播放(参考 sample_comm_audio.c:1021) */
    ret = HI_MPI_AO_EnableReSmp(PLAY_AIO_DEV, PLAY_CHN_ID, PLAY_SAMPLE_RATE);
    if (ret != HI_SUCCESS) {
        printf("[my_wake] play: AO_EnableReSmp(24000) failed %#x\n", ret);
        (void)HI_MPI_AO_DisableChn(PLAY_AIO_DEV, PLAY_CHN_ID);
        (void)HI_MPI_AO_Disable(PLAY_AIO_DEV);
        return -1;
    }

    printf("[my_wake] play: AO ready, 24kHz input -> 16kHz hw (resample)\n");
    return 0;
}

int WakePlaybackWrite(const void *data, int len)
{
    if (data == NULL || len <= 0) {
        return -1;
    }

    const unsigned char *p = (const unsigned char *)data;
    int miss = 0;
    int total = 0;

    while (total < len) {
        /* 每次送一帧应用侧数据(1440 字节 = 720 点@24kHz = 30ms),不足补静音 */
        unsigned char frameBuf[PLAY_IN_FRAME_BYTES];
        int chunk = len - total;
        if (chunk > PLAY_IN_FRAME_BYTES) {
            chunk = PLAY_IN_FRAME_BYTES;
        }
        memcpy(frameBuf, p + total, chunk);
        if (chunk < PLAY_IN_FRAME_BYTES) {
            memset(frameBuf + chunk, 0, PLAY_IN_FRAME_BYTES - chunk);
        }

        AUDIO_FRAME_S frame;
        memset(&frame, 0, sizeof(frame));
        frame.enBitwidth = AUDIO_BIT_WIDTH_16;
        frame.enSoundmode = AUDIO_SOUND_MODE_MONO;
        frame.u64VirAddr[0] = frameBuf;
        frame.u32Len = PLAY_IN_FRAME_BYTES;  /* 送入字节数(24kHz 侧) */

        HI_S32 ret = HI_MPI_AO_SendFrame(PLAY_AIO_DEV, PLAY_CHN_ID, &frame, 1000);
        if (ret != HI_SUCCESS) {
            if (++miss < PLAY_RETRY_MAX) {
                continue;
            }
            printf("[my_wake] play: AO_SendFrame failed %#x (%d retries), abort\n", ret, miss);
            return -1;
        }
        miss = 0;
        total += chunk;
    }
    return 0;
}

void WakePlaybackClear(void)
{
    HI_S32 ret = HI_MPI_AO_ClearChnBuf(PLAY_AIO_DEV, PLAY_CHN_ID);
    if (ret != HI_SUCCESS) {
        /* 清缓冲失败不致命,打印即可 */
        printf("[my_wake] play: ClearChnBuf %#x\n", ret);
    }
}

void WakePlaybackDeinit(void)
{
    /* 先关重采样,再关通道/设备(参考 sample_comm_audio.c:1044-1055) */
    (void)HI_MPI_AO_DisableReSmp(PLAY_AIO_DEV, PLAY_CHN_ID);
    (void)HI_MPI_AO_DisableChn(PLAY_AIO_DEV, PLAY_CHN_ID);
    (void)HI_MPI_AO_Disable(PLAY_AIO_DEV);
}
