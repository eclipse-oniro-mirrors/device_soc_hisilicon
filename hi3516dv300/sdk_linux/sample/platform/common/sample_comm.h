/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef SAMPLE_COMM_H
#define SAMPLE_COMM_H

#include <pthread.h>

#include "hi_common.h"
#include "hi_buffer.h"
#include "hi_comm_sys.h"
#include "hi_comm_vb.h"
#include "hi_comm_isp.h"
#include "hi_comm_vi.h"
#include "hi_comm_vo.h"
#include "hi_comm_venc.h"
#include "hi_comm_vdec.h"
#include "hi_comm_vpss.h"
#include "hi_comm_region.h"
#include "hi_comm_adec.h"
#include "hi_comm_aenc.h"
#include "hi_comm_ai.h"
#include "hi_comm_ao.h"
#include "hi_comm_aio.h"
#include "hi_defines.h"
#include "hi_comm_hdmi.h"
#include "hi_mipi.h"
#include "hi_comm_vgs.h"

#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_vi.h"
#include "mpi_vo.h"
#include "mpi_venc.h"
#include "mpi_vdec.h"
#include "mpi_vpss.h"
#include "mpi_region.h"
#include "mpi_audio.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "hi_math.h"
#include "hi_sns_ctrl.h"
#include "mpi_hdmi.h"
#include "mpi_vgs.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define FILE_NAME_LEN       128
#define SENSOR_5M_WIDTH     2592
#define SENSOR_5M_HEIGHT    1944
#define IMG_5M_WIDTH        2592
#define IMG_5M_HEIGHT       1944
#define IMG_4M_WIDTH        2592
#define IMG_4M_HEIGHT       1536
#define IMG_2M_WIDTH        1920
#define IMG_2M_HEIGHT       1080
#define IMG_1M_WIDTH        1280
#define IMG_1M_HEIGHT       720
#define FPS_20              20
#define FPS_25              25
#define FPS_30              30
#define FPS_50              50
#define FPS_60              60
#define FPS_90              90
#define FPS_120             120
#define FPS_240             240

#define CHECK_CHN_RET(express, Chn, name)                                                                       \
    do {                                                                                                        \
        HI_S32 Ret;                                                                                             \
        Ret = express;                                                                                          \
        if (Ret != HI_SUCCESS) {                                                                                \
            printf("\033[0;31m%s chn %d failed at %s: LINE: %d with %#x!\033[0;39m\n", name, Chn, __FUNCTION__, \
                __LINE__, Ret);                                                                                 \
            fflush(stdout);                                                                                     \
            return Ret;                                                                                         \
        }                                                                                                       \
    } while (0)

#define CHECK_RET(express, name)                                                                                    \
    do {                                                                                                            \
        HI_S32 Ret;                                                                                                 \
        Ret = express;                                                                                              \
        if (Ret != HI_SUCCESS) {                                                                                    \
            printf("\033[0;31m%s failed at %s: LINE: %d with %#x!\033[0;39m\n", name, __FUNCTION__, __LINE__, Ret); \
            return Ret;                                                                                             \
        }                                                                                                           \
    } while (0)

#define SAMPLE_PIXEL_FORMAT             PIXEL_FORMAT_YVU_SEMIPLANAR_420

#define TLV320_FILE                     "/dev/tlv320aic31"
#define COLOR_RGB_RED                   0xFF0000
#define COLOR_RGB_GREEN                 0x00FF00
#define COLOR_RGB_BLUE                  0x0000FF
#define COLOR_RGB_BLACK                 0x000000
#define COLOR_RGB_YELLOW                0xFFFF00
#define COLOR_RGB_CYN                   0x00ffff
#define COLOR_RGB_WHITE                 0xffffff

#define SAMPLE_VO_DEV_DHD0              0 /* VO's device HD0 */
#define SAMPLE_VO_DEV_DHD1              1 /* VO's device HD1 */
#define SAMPLE_VO_DEV_UHD               SAMPLE_VO_DEV_DHD0 /* VO's ultra HD device:HD0 */
#define SAMPLE_VO_DEV_HD                SAMPLE_VO_DEV_DHD1 /* VO's HD device:HD1 */
#define SAMPLE_VO_LAYER_VHD0            0
#define SAMPLE_VO_LAYER_VHD1            1
#define SAMPLE_VO_LAYER_VHD2            2
#define SAMPLE_VO_LAYER_PIP             SAMPLE_VO_LAYER_VHD2

#define SAMPLE_AUDIO_EXTERN_AI_DEV      0
#define SAMPLE_AUDIO_EXTERN_AO_DEV      0
#define SAMPLE_AUDIO_INNER_AI_DEV       0
#define SAMPLE_AUDIO_INNER_AO_DEV       0
#define SAMPLE_AUDIO_INNER_HDMI_AO_DEV  1
#define SAMPLE_AUDIO_PTNUMPERFRM        480
#define WDR_MAX_PIPE_NUM                4

#if (!defined(__HuaweiLite__)) || defined(__OHOS__)
#define VI_DATA_PATH "."
#else
#define VI_DATA_PATH "/sharefs"
#endif

#define PAUSE()                                                             \
    do {                                                                    \
        printf("---------------press Enter key to exit!---------------\n"); \
        getchar();                                                          \
    } while (0)

#define SAMPLE_PRT(fmt...)                           \
    do {                                             \
        printf("[%s]-%d: ", __FUNCTION__, __LINE__); \
        printf(fmt);                                 \
    } while (0)

#define CHECK_NULL_PTR(ptr)                                                     \
    do {                                                                        \
        if ((ptr) == NULL) {                                                    \
            printf("func:%s, line:%d, NULL pointer\n", __FUNCTION__, __LINE__); \
            return HI_FAILURE;                                                  \
        }                                                                       \
    } while (0)

typedef enum hiPIC_SIZE_E {
    PIC_CIF,
    PIC_360P,    /* 640 * 360 */
    PIC_VGA,     /* 640 * 480 */
    PIC_D1_PAL,  /* 720 * 576 */
    PIC_D1_NTSC, /* 720 * 480 */
    PIC_D1_MIPI, /* 800 * 480 */
    PIC_720P,    /* 1280 * 720 */
    PIC_1080P,   /* 1920 * 1080 */
    PIC_480P,
    PIC_576P,
    PIC_800x600,
    PIC_1024x768,
    PIC_1280x1024,
    PIC_1366x768,
    PIC_1440x900,
    PIC_1280x800,
    PIC_1600x1200,
    PIC_1680x1050,
    PIC_1920x1200,
    PIC_640x480,
    PIC_1920x2160,
    PIC_2560x1440,
    PIC_2560x1600,
    PIC_2592x1520,
    PIC_2592x1536,
    PIC_2592x1944,
    PIC_2688x1536,
    PIC_2716x1524,
    PIC_2880x1620,
    PIC_3840x2160,
    PIC_4096x2160,
    PIC_3000x3000,
    PIC_4000x3000,
    PIC_7680x4320,
    PIC_3840x8640,
    PIC_BUTT
} PIC_SIZE_E;

typedef enum hiSAMPLE_SNS_TYPE_E {
    SONY_IMX327_MIPI_2M_30FPS_12BIT,
    SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1,
    SONY_IMX327_2L_MIPI_2M_30FPS_12BIT,
    SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1,
    SONY_IMX390_MIPI_2M_30FPS_12BIT,
    SONY_IMX307_MIPI_2M_30FPS_12BIT,
    SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1,
    SONY_IMX307_2L_MIPI_2M_30FPS_12BIT,
    SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1,
    SONY_IMX307_2L_SLAVE_MIPI_2M_30FPS_12BIT,
    SONY_IMX307_2L_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1,
    SONY_IMX335_MIPI_5M_30FPS_12BIT,
    SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1,
    SONY_IMX335_MIPI_4M_30FPS_12BIT,
    SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1,
    SONY_IMX458_MIPI_8M_30FPS_10BIT,
    SONY_IMX458_MIPI_12M_20FPS_10BIT,
    SONY_IMX458_MIPI_4M_60FPS_10BIT,
    SONY_IMX458_MIPI_4M_40FPS_10BIT,
    SONY_IMX458_MIPI_2M_90FPS_10BIT,
    SONY_IMX458_MIPI_1M_129FPS_10BIT,
    SMART_SC4210_MIPI_3M_30FPS_12BIT,
    SMART_SC4210_MIPI_3M_30FPS_10BIT_WDR2TO1,
    SMART_SC500AI_MIPI_5M_30FPS_12BIT,
    SMART_SC500AI_MIPI_5M_30FPS_10BIT_WDR2TO1,
    OMNIVISION_OS04B10_MIPI_4M_30FPS_10BIT,
    OMNIVISION_OS05A_MIPI_4M_30FPS_12BIT,
    OMNIVISION_OS05A_MIPI_4M_30FPS_10BIT_WDR2TO1,
    OMNIVISION_OS05A_2L_MIPI_1M_30FPS_10BIT,
    OMNIVISION_OS05A_2L_MIPI_4M_30FPS_12BIT,
    OMNIVISION_OS05A_2L_MIPI_4M_30FPS_10BIT_WDR2TO1,
    OMNIVISION_OS08A10_MIPI_8M_30FPS_10BIT,
    GALAXYCORE_GC2053_MIPI_2M_30FPS_10BIT,
    OMNIVISION_OV12870_MIPI_1M_240FPS_10BIT,
    OMNIVISION_OV12870_MIPI_2M_120FPS_10BIT,
    OMNIVISION_OV12870_MIPI_8M_30FPS_10BIT,
    OMNIVISION_OV12870_MIPI_12M_30FPS_10BIT,
    OMNIVISION_OV9284_MIPI_1M_60FPS_10BIT,
    SONY_IMX415_MIPI_8M_30FPS_12BIT,
    SONY_IMX415_MIPI_8M_20FPS_12BIT_WDR2TO1,
    SONY_IMX415_MIPI_8M_20FPS_12BIT,
    SONY_IMX415_MIPI_2M_60FPS_12BIT,
    OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT,
    OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_WDR2TO1,
    OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_WDR2TO1_HLCG,
    OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_BUILTIN,
    OMNIVISION_OV2775_2L_MIPI_2M_30FPS_12BIT,
    OMNIVISION_OV2775_2L_MIPI_2M_30FPS_12BIT_WDR2TO1,
    PRIMESENSOR_PS5260_2L_MIPI_2M_30FPS_12BIT,
    PRIMESENSOR_PS5260_2L_MIPI_2M_30FPS_12BIT_BUILTIN,
    SAMPLE_SNS_TYPE_BUTT,
} SAMPLE_SNS_TYPE_E;

typedef enum hiSAMPLE_VO_MODE_E {
    VO_MODE_1MUX,
    VO_MODE_2MUX,
    VO_MODE_4MUX,
    VO_MODE_8MUX,
    VO_MODE_9MUX,
    VO_MODE_16MUX,
    VO_MODE_25MUX,
    VO_MODE_36MUX,
    VO_MODE_49MUX,
    VO_MODE_64MUX,
    VO_MODE_2X4,
    VO_MODE_BUTT
} SAMPLE_VO_MODE_E;

typedef enum hiSAMPLE_RC_E {
    SAMPLE_RC_CBR = 0,
    SAMPLE_RC_VBR,
    SAMPLE_RC_AVBR,
    SAMPLE_RC_QVBR,
    SAMPLE_RC_CVBR,
    SAMPLE_RC_QPMAP,
    SAMPLE_RC_FIXQP
} SAMPLE_RC_E;

typedef struct hiSAMPLE_VENC_GETSTREAM_PARA_S {
    HI_BOOL bThreadStart;
    VENC_CHN VeChn[VENC_MAX_CHN_NUM];
    HI_S32 s32Cnt;
} SAMPLE_VENC_GETSTREAM_PARA_S;

typedef struct hiSAMPLE_VENC_QPMAP_SENDFRAME_PARA_S {
    HI_BOOL bThreadStart;
    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;
    VENC_CHN VeChn[VENC_MAX_CHN_NUM];
    HI_S32 s32Cnt;
    SIZE_S stSize;
} SAMPLE_VENC_QPMAP_SENDFRAME_PARA_S;

typedef struct hiSAMPLE_VI_DUMP_THREAD_INFO_S {
    VI_PIPE ViPipe;
    HI_S32 s32Cnt;
    HI_BOOL bDump;
    HI_CHAR aszName[FILE_NAME_LEN];
    pthread_t ThreadId;
} SAMPLE_VI_DUMP_THREAD_INFO_S;

typedef struct hiSAMPLE_SENSOR_INFO_S {
    SAMPLE_SNS_TYPE_E enSnsType;
    HI_S32 s32SnsId;
    HI_S32 s32BusId;
    combo_dev_t MipiDev;
} SAMPLE_SENSOR_INFO_S;

typedef struct hiSAMPLE_SNAP_INFO_S {
    HI_BOOL bSnap;
    HI_BOOL bDoublePipe;
    VI_PIPE VideoPipe;
    VI_PIPE SnapPipe;
    VI_VPSS_MODE_E enVideoPipeMode;
    VI_VPSS_MODE_E enSnapPipeMode;
} SAMPLE_SNAP_INFO_S;

typedef struct hiSAMPLE_DEV_INFO_S {
    VI_DEV ViDev;
    WDR_MODE_E enWDRMode;
} SAMPLE_DEV_INFO_S;

typedef struct hiSAMPLE_PIPE_INFO_S {
    VI_PIPE aPipe[WDR_MAX_PIPE_NUM];
    VI_VPSS_MODE_E enMastPipeMode;
    HI_BOOL bMultiPipe;
    HI_BOOL bVcNumCfged;
    HI_BOOL bIspBypass;
    PIXEL_FORMAT_E enPixFmt;
    HI_U32 u32VCNum[WDR_MAX_PIPE_NUM];
} SAMPLE_PIPE_INFO_S;

typedef struct hiSAMPLE_CHN_INFO_S {
    VI_CHN ViChn;
    PIXEL_FORMAT_E enPixFormat;
    DYNAMIC_RANGE_E enDynamicRange;
    VIDEO_FORMAT_E enVideoFormat;
    COMPRESS_MODE_E enCompressMode;
} SAMPLE_CHN_INFO_S;

typedef struct hiSAMPLE_VI_INFO_S {
    SAMPLE_SENSOR_INFO_S stSnsInfo;
    SAMPLE_DEV_INFO_S stDevInfo;
    SAMPLE_PIPE_INFO_S stPipeInfo;
    SAMPLE_CHN_INFO_S stChnInfo;
    SAMPLE_SNAP_INFO_S stSnapInfo;
} SAMPLE_VI_INFO_S;

typedef struct hiSAMPLE_VI_CONFIG_S {
    SAMPLE_VI_INFO_S astViInfo[VI_MAX_DEV_NUM];
    HI_S32 as32WorkingViId[VI_MAX_DEV_NUM];
    HI_S32 s32WorkingViNum;
} SAMPLE_VI_CONFIG_S;

typedef struct hiSAMPLE_VI_FRAME_CONFIG_S {
    HI_U32 u32Width;
    HI_U32 u32Height;
    HI_U32 u32ByteAlign;
    PIXEL_FORMAT_E enPixelFormat;
    VIDEO_FORMAT_E enVideoFormat;
    COMPRESS_MODE_E enCompressMode;
    DYNAMIC_RANGE_E enDynamicRange;
} SAMPLE_VI_FRAME_CONFIG_S;

typedef struct hiSAMPLE_VI_FRAME_INFO_S {
    VB_BLK VbBlk;
    HI_U32 u32Size;
    VIDEO_FRAME_INFO_S stVideoFrameInfo;
} SAMPLE_VI_FRAME_INFO_S;

typedef struct hiSAMPLE_VI_FPN_CALIBRATE_INFO_S {
    HI_U32 u32Threshold;
    HI_U32 u32FrameNum;
    ISP_FPN_TYPE_E enFpnType;
    PIXEL_FORMAT_E enPixelFormat;
    COMPRESS_MODE_E enCompressMode;
} SAMPLE_VI_FPN_CALIBRATE_INFO_S;

typedef struct hiSAMPLE_VI_FPN_CORRECTION_INFO_S {
    ISP_OP_TYPE_E enOpType;
    ISP_FPN_TYPE_E enFpnType;
    HI_U32 u32Strength;
    PIXEL_FORMAT_E enPixelFormat;
    COMPRESS_MODE_E enCompressMode;
    SAMPLE_VI_FRAME_INFO_S stViFrameInfo;
} SAMPLE_VI_FPN_CORRECTION_INFO_S;

typedef struct tag_SAMPLE_VO_WBC_CONFIG {
    VO_WBC_SOURCE_TYPE_E enSourceType;
    DYNAMIC_RANGE_E enDynamicRange;
    COMPRESS_MODE_E enCompressMode;
    HI_S32 s32Depth;
    HI_S32 VoWbc;
    VO_WBC_ATTR_S stWbcAttr;
    VO_WBC_SOURCE_S stWbcSource;
    VO_WBC_MODE_E enWbcMode;
} SAMPLE_VO_WBC_CONFIG;

typedef struct hiSAMPLE_COMM_VO_LAYER_CONFIG_S {
    /* for layer */
    VO_LAYER VoLayer;
    VO_INTF_SYNC_E enIntfSync;
    RECT_S stDispRect;
    SIZE_S stImageSize;
    PIXEL_FORMAT_E enPixFormat;

    HI_U32 u32DisBufLen;
    DYNAMIC_RANGE_E enDstDynamicRange;

    /* for chn */
    SAMPLE_VO_MODE_E enVoMode;
} SAMPLE_COMM_VO_LAYER_CONFIG_S;

typedef struct hiSAMPLE_VO_CONFIG_S {
    /* for device */
    VO_DEV VoDev;
    VO_INTF_TYPE_E enVoIntfType;
    VO_INTF_SYNC_E enIntfSync;
    PIC_SIZE_E enPicSize;
    HI_U32 u32BgColor;

    /* for layer */
    PIXEL_FORMAT_E enPixFormat;
    RECT_S stDispRect;
    SIZE_S stImageSize;
    VO_PART_MODE_E enVoPartMode;

    HI_U32 u32DisBufLen;
    DYNAMIC_RANGE_E enDstDynamicRange;

    /* for channel */
    SAMPLE_VO_MODE_E enVoMode;
} SAMPLE_VO_CONFIG_S;

typedef enum hiTHREAD_CONTRL_E {
    THREAD_CTRL_START,
    THREAD_CTRL_PAUSE,
    THREAD_CTRL_STOP,
} THREAD_CONTRL_E;

typedef struct hiVDEC_THREAD_PARAM_S {
    HI_S32 s32ChnId;
    PAYLOAD_TYPE_E enType;
    HI_CHAR cFilePath[FILE_NAME_LEN];
    HI_CHAR cFileName[FILE_NAME_LEN];
    HI_S32 s32StreamMode;
    HI_S32 s32MilliSec;
    HI_S32 s32MinBufSize;
    HI_S32 s32IntervalTime;
    THREAD_CONTRL_E eThreadCtrl;
    HI_U64 u64PtsInit;
    HI_U64 u64PtsIncrease;
    HI_BOOL bCircleSend;
} VDEC_THREAD_PARAM_S;

typedef struct hiSAMPLE_VDEC_BUF {
    HI_U32 u32PicBufSize;
    HI_U32 u32TmvBufSize;
    HI_BOOL bPicBufAlloc;
    HI_BOOL bTmvBufAlloc;
} SAMPLE_VDEC_BUF;

typedef struct hiSAMPLE_VDEC_VIDEO_ATTR {
    VIDEO_DEC_MODE_E enDecMode;
    HI_U32 u32RefFrameNum;
    DATA_BITWIDTH_E enBitWidth;
} SAMPLE_VDEC_VIDEO_ATTR;

typedef struct hiSAMPLE_VDEC_PICTURE_ATTR {
    PIXEL_FORMAT_E enPixelFormat;
    HI_U32 u32Alpha;
} SAMPLE_VDEC_PICTURE_ATTR;

typedef struct hiSAMPLE_VDEC_ATTR {
    PAYLOAD_TYPE_E enType;
    VIDEO_MODE_E enMode;
    HI_U32 u32Width;
    HI_U32 u32Height;
    HI_U32 u32FrameBufCnt;
    HI_U32 u32DisplayFrameNum;
    union {
        SAMPLE_VDEC_VIDEO_ATTR stSapmleVdecVideo;     /* structure with video   (h265 / h264) */
        SAMPLE_VDEC_PICTURE_ATTR stSapmleVdecPicture; /* structure with picture (jpeg / mjpeg) */
    };
} SAMPLE_VDEC_ATTR;

typedef struct hiSAMPLE_VB_BASE_INFO_S {
    PIXEL_FORMAT_E enPixelFormat;
    HI_U32 u32Width;
    HI_U32 u32Height;
    HI_U32 u32Align;
    COMPRESS_MODE_E enCompressMode;
} SAMPLE_VB_BASE_INFO_S;

typedef struct hiSAMPLE_VB_CAL_CONFIG_S {
    HI_U32 u32VBSize;

    HI_U32 u32HeadStride;
    HI_U32 u32HeadSize;
    HI_U32 u32HeadYSize;

    HI_U32 u32MainStride;
    HI_U32 u32MainSize;
    HI_U32 u32MainYSize;

    HI_U32 u32ExtStride;
    HI_U32 u32ExtYSize;
} SAMPLE_VB_CAL_CONFIG_S;

HI_VOID SAMPLE_SYS_SIGNAL(void (*func)(int));
HI_VOID *SAMPLE_SYS_IOMmap(HI_U64 u64PhyAddr, HI_U32 u32Size);
HI_S32 SAMPLE_SYS_Munmap(HI_VOID *pVirAddr, HI_U32 u32Size);
HI_S32 SAMPLE_SYS_SetReg(HI_U64 u64Addr, HI_U32 u32Value);
HI_S32 SAMPLE_SYS_GetReg(HI_U64 u64Addr, HI_U32 *pu32Value);

HI_S32 SAMPLE_COMM_SYS_GetPicSize(PIC_SIZE_E enPicSize, SIZE_S *pstSize);
HI_S32 SAMPLE_COMM_SYS_MemConfig(HI_VOID);
HI_VOID SAMPLE_COMM_SYS_Exit(void);
HI_S32 SAMPLE_COMM_SYS_Init(VB_CONFIG_S *pstVbConfig);
HI_S32 SAMPLE_COMM_SYS_InitWithVbSupplement(VB_CONFIG_S *pstVbConf, HI_U32 u32SupplementConfig);

HI_S32 SAMPLE_COMM_VI_Bind_VO(VI_PIPE ViPipe, VI_CHN ViChn, VO_LAYER VoLayer, VO_CHN VoChn);
HI_S32 SAMPLE_COMM_VI_UnBind_VO(VI_PIPE ViPipe, VI_CHN ViChn, VO_LAYER VoLayer, VO_CHN VoChn);
HI_S32 SAMPLE_COMM_VI_Bind_VPSS(VI_PIPE ViPipe, VI_CHN ViChn, VPSS_GRP VpssGrp);
HI_S32 SAMPLE_COMM_VI_UnBind_VPSS(VI_PIPE ViPipe, VI_CHN ViChn, VPSS_GRP VpssGrp);
HI_S32 SAMPLE_COMM_VI_Bind_VENC(VI_PIPE ViPipe, VI_CHN ViChn, VENC_CHN VencChn);
HI_S32 SAMPLE_COMM_VI_UnBind_VENC(VI_PIPE ViPipe, VI_CHN ViChn, VENC_CHN VencChn);
HI_S32 SAMPLE_COMM_VPSS_Bind_AVS(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AVS_GRP AvsGrp, AVS_PIPE AvsPipe);
HI_S32 SAMPLE_COMM_VPSS_UnBind_AVS(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AVS_GRP AvsGrp, AVS_PIPE AvsPipe);
HI_S32 SAMPLE_COMM_VPSS_Bind_VO(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VO_LAYER VoLayer, VO_CHN VoChn);
HI_S32 SAMPLE_COMM_VPSS_UnBind_VO(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VO_LAYER VoLayer, VO_CHN VoChn);
HI_S32 SAMPLE_COMM_VPSS_Bind_VENC(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn);
HI_S32 SAMPLE_COMM_VPSS_UnBind_VENC(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn);

HI_S32 SAMPLE_COMM_AVS_Bind_AVS(AVS_GRP AvsSrcGrp, AVS_CHN AvsSrcChn, AVS_GRP AvsDestGrp, AVS_CHN AvsDestChn);
HI_S32 SAMPLE_COMM_AVS_UnBind_AVS(AVS_GRP AvsSrcGrp, AVS_CHN AvsSrcChn, AVS_GRP AvsDestGrp, AVS_CHN AvsDestChn);
HI_S32 SAMPLE_COMM_AVS_Bind_VPSS(AVS_GRP AvsGrp, AVS_CHN AvsChn, VPSS_GRP VpssGrp);
HI_S32 SAMPLE_COMM_AVS_UnBind_VPSS(AVS_GRP AvsGrp, AVS_CHN AvsChn, VPSS_GRP VpssGrp);
HI_S32 SAMPLE_COMM_AVS_Bind_VENC(AVS_GRP AvsGrp, AVS_CHN AvsChn, VENC_CHN VencChn);
HI_S32 SAMPLE_COMM_AVS_UnBind_VENC(AVS_GRP AvsGrp, AVS_CHN AvsChn, VENC_CHN VencChn);
HI_S32 SAMPLE_COMM_AVS_Bind_VO(AVS_GRP AvsGrp, AVS_CHN AvsChn, VO_LAYER VoLayer, VO_CHN VoChn);
HI_S32 SAMPLE_COMM_AVS_UnBind_VO(AVS_GRP AvsGrp, AVS_CHN AvsChn, VO_LAYER VoLayer, VO_CHN VoChn);
HI_S32 SAMPLE_COMM_VDEC_Bind_VPSS(VDEC_CHN VdecChn, VPSS_GRP VpssGrp);
HI_S32 SAMPLE_COMM_VDEC_UnBind_VPSS(VDEC_CHN VdecChn, VPSS_GRP VpssGrp);

HI_S32 SAMPLE_COMM_VO_Bind_VO(VO_LAYER SrcVoLayer, VO_CHN SrcVoChn, VO_LAYER DstVoLayer, VO_CHN DstVoChn);
HI_S32 SAMPLE_COMM_VO_UnBind_VO(VO_LAYER DstVoLayer, VO_CHN DstVoChn);

HI_VOID SAMPLE_COMM_ISP_Stop(ISP_DEV IspDev);
HI_VOID SAMPLE_COMM_All_ISP_Stop(HI_VOID);
HI_S32 SAMPLE_COMM_ISP_Run(ISP_DEV IspDev);
HI_S32 SAMPLE_COMM_ISP_BindSns(ISP_DEV IspDev, HI_U32 u32SnsId, SAMPLE_SNS_TYPE_E enSnsType, HI_S8 s8SnsDev);
HI_S32 SAMPLE_COMM_ISP_Sensor_Regiter_callback(ISP_DEV IspDev, HI_U32 u32SnsId);
HI_S32 SAMPLE_COMM_ISP_Sensor_UnRegiter_callback(ISP_DEV IspDev);
HI_S32 SAMPLE_COMM_ISP_Aelib_Callback(ISP_DEV IspDev);
HI_S32 SAMPLE_COMM_ISP_Aelib_UnCallback(ISP_DEV IspDev);
HI_S32 SAMPLE_COMM_ISP_Awblib_Callback(ISP_DEV IspDev);
HI_S32 SAMPLE_COMM_ISP_Awblib_UnCallback(ISP_DEV IspDev);
HI_S32 SAMPLE_COMM_ISP_GetIspAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, ISP_PUB_ATTR_S *pstPubAttr);
HI_VOID SAMPLE_COMM_VI_GetComboAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, combo_dev_t MipiDev,
    combo_dev_attr_t *pstComboAttr);
HI_VOID SAMPLE_COMM_VI_GetPipeAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, VI_PIPE_ATTR_S *pstPipeAttr);
HI_VOID SAMPLE_COMM_VI_GetChnAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, VI_CHN_ATTR_S *pstChnAttr);

HI_S32 SAMPLE_COMM_VI_GetSizeBySensor(SAMPLE_SNS_TYPE_E enMode, PIC_SIZE_E *penSize);
HI_S32 SAMPLE_COMM_VI_GetFrameRateBySensor(SAMPLE_SNS_TYPE_E enMode, HI_U32 *pu32FrameRate);
HI_S32 SAMPLE_COMM_VI_StartDev(SAMPLE_VI_INFO_S *pstViInfo);
HI_S32 SAMPLE_COMM_VI_StopDev(SAMPLE_VI_INFO_S *pstViInfo);
HI_S32 SAMPLE_COMM_VI_StartChn(VI_CHN ViChn, RECT_S *pstCapRect, SIZE_S *pstTarSize, SAMPLE_VI_CONFIG_S *pstViConfig);
HI_S32 SAMPLE_COMM_VI_StartMIPI(SAMPLE_VI_CONFIG_S *pstViConfig);
HI_S32 SAMPLE_COMM_VI_StopMIPI(SAMPLE_VI_CONFIG_S *pstViConfig);
HI_S32 SAMPLE_COMM_VI_StartVi(SAMPLE_VI_CONFIG_S *pstViConfig);
HI_S32 SAMPLE_COMM_VI_StopVi(SAMPLE_VI_CONFIG_S *pstViConfig);
HI_S32 SAMPLE_COMM_VI_SetMipiAttr(SAMPLE_VI_CONFIG_S *pstViConfig);
HI_VOID SAMPLE_COMM_VI_GetDevAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, VI_DEV_ATTR_S *pstViDevAttr);
HI_VOID SAMPLE_COMM_VI_GetSensorInfo(SAMPLE_VI_CONFIG_S *pstViConfig);
HI_S32 SAMPLE_COMM_VI_BindPipeDev(SAMPLE_VI_INFO_S *pstViInfo);
HI_S32 SAMPLE_COMM_VI_ModeSwitch_EnableViPipe(SAMPLE_VI_INFO_S *pstViInfo);
HI_VOID SAMPLE_COMM_VI_StopViPipe(SAMPLE_VI_INFO_S *pstViInfo);
HI_S32 SAMPLE_COMM_VI_CreateVi(SAMPLE_VI_CONFIG_S *pstViConfig);
HI_S32 SAMPLE_COMM_VI_DestroyVi(SAMPLE_VI_CONFIG_S *pstViConfig);

HI_S32 SAMPLE_COMM_VI_CreateSingleVi(SAMPLE_VI_INFO_S *pstViInfo);
HI_VOID SAMPLE_COMM_VI_DestroySingleVi(SAMPLE_VI_INFO_S *pstViInfo);
HI_S32 SAMPLE_COMM_VI_StopIsp(SAMPLE_VI_INFO_S *pstViInfo);
HI_S32 SAMPLE_COMM_VI_StartIsp(SAMPLE_VI_INFO_S *pstViInfo);

combo_dev_t SAMPLE_COMM_VI_GetComboDevBySensor(SAMPLE_SNS_TYPE_E enMode, HI_S32 s32SnsIdx);
HI_S32 SAMPLE_COMM_VI_SetParam(SAMPLE_VI_CONFIG_S *pstViConfig);
HI_S32 SAMPLE_COMM_VI_SwitchMode_StopVI(SAMPLE_VI_CONFIG_S *pstViConfigSrc);
HI_S32 SAMPLE_COMM_VI_SwitchMode(SAMPLE_VI_CONFIG_S *pstViConfigDes);
HI_S32 SAMPLE_COMM_ModeSwitch_VI_CreateSingleVi(SAMPLE_VI_INFO_S *pstViInfo);
HI_S32 SAMPLE_COMM_VI_SwitchISPMode(SAMPLE_VI_CONFIG_S *pstViConfig);

HI_S32 SAMPLE_COMM_VI_FpnCalibrateConfig(VI_PIPE ViPipe, SAMPLE_VI_FPN_CALIBRATE_INFO_S *pstViFpnCalibrateInfo);
HI_S32 SAMPLE_COMM_VI_FpnCorrectionConfig(VI_PIPE ViPipe, SAMPLE_VI_FPN_CORRECTION_INFO_S *pstViFpnCorrectionInfo);
HI_S32 SAMPLE_COMM_VI_DisableFpnCorrection(VI_PIPE ViPipe, SAMPLE_VI_FPN_CORRECTION_INFO_S *pstViFpnCorrectionInfo);

HI_S32 SAMPLE_COMM_VI_ResetMipi(SAMPLE_VI_CONFIG_S *pstViConfig);
HI_S32 SAMPLE_COMM_VI_UnresetMipi(SAMPLE_VI_CONFIG_S *pstViConfig);
HI_S32 SAMPLE_COMM_VI_EnableMipiClock(SAMPLE_VI_CONFIG_S *pstViConfig);
HI_S32 SAMPLE_COMM_VI_SetMipiHsMode(lane_divide_mode_t enHsMode);
HI_U32 SAMPLE_COMM_VI_GetMipiLaneDivideMode(SAMPLE_VI_CONFIG_S *pstViConfig);

HI_S32 SAMPLE_COMM_VPSS_Start(VPSS_GRP VpssGrp, HI_BOOL *pabChnEnable, VPSS_GRP_ATTR_S *pstVpssGrpAttr,
    VPSS_CHN_ATTR_S *pastVpssChnAttr);
HI_S32 SAMPLE_COMM_VPSS_Stop(VPSS_GRP VpssGrp, HI_BOOL *pabChnEnable);

HI_S32 SAMPLE_COMM_VO_GetWH(VO_INTF_SYNC_E enIntfSync, HI_U32 *pu32W, HI_U32 *pu32H, HI_U32 *pu32Frm);
HI_S32 SAMPLE_COMM_VO_MemConfig(VO_DEV VoDev, HI_CHAR *pcMmzName);
HI_S32 SAMPLE_COMM_VO_StartDev(VO_DEV VoDev, VO_PUB_ATTR_S *pstPubAttr);
HI_S32 SAMPLE_COMM_VO_StopDev(VO_DEV VoDev);
HI_S32 SAMPLE_COMM_VO_StartLayer(VO_LAYER VoLayer, const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);
HI_S32 SAMPLE_COMM_VO_StopLayer(VO_LAYER VoLayer);
HI_S32 SAMPLE_COMM_VO_StartChn(VO_LAYER VoLayer, SAMPLE_VO_MODE_E enMode);
HI_S32 SAMPLE_COMM_VO_StopChn(VO_LAYER VoLayer, SAMPLE_VO_MODE_E enMode);
HI_S32 SAMPLE_COMM_VO_BindVpss(VO_LAYER VoLayer, VO_CHN VoChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn);
HI_S32 SAMPLE_COMM_VO_UnBindVpss(VO_LAYER VoLayer, VO_CHN VoChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn);
HI_S32 SAMPLE_COMM_VO_BindVi(VO_LAYER VoLayer, VO_CHN VoChn, VI_CHN ViChn);
HI_S32 SAMPLE_COMM_VO_UnBindVi(VO_LAYER VoLayer, VO_CHN VoChn);
HI_S32 SAMPLE_COMM_VO_HdmiStart(VO_INTF_SYNC_E enIntfSync);
HI_S32 SAMPLE_COMM_VO_HdmiStartByDyRg(VO_INTF_SYNC_E enIntfSync, DYNAMIC_RANGE_E enDyRg);
HI_S32 SAMPLE_COMM_VO_HdmiStop(HI_VOID);
HI_S32 SAMPLE_COMM_VO_GetDefConfig(SAMPLE_VO_CONFIG_S *pstVoConfig);
HI_S32 SAMPLE_COMM_VO_StopVO(SAMPLE_VO_CONFIG_S *pstVoConfig);
HI_S32 SAMPLE_COMM_VO_StartVO(SAMPLE_VO_CONFIG_S *pstVoConfig);
HI_S32 SAMPLE_COMM_VO_StartLayerChn(SAMPLE_COMM_VO_LAYER_CONFIG_S *pstVoLayerConfig);
HI_S32 SAMPLE_COMM_VO_StopLayerChn(SAMPLE_COMM_VO_LAYER_CONFIG_S *pstVoLayerConfig);
HI_VOID SAMPLE_COMM_VO_Exit(HI_VOID);

HI_S32 SAMPLE_COMM_VENC_MemConfig(HI_VOID);
HI_S32 SAMPLE_COMM_VENC_Create(VENC_CHN VencChn, PAYLOAD_TYPE_E enType, PIC_SIZE_E enSize, SAMPLE_RC_E enRcMode,
    HI_U32 u32Profile, HI_BOOL bRcnRefShareBuf, VENC_GOP_ATTR_S *pstGopAttr);
HI_S32 SAMPLE_COMM_VENC_Start(VENC_CHN VencChn, PAYLOAD_TYPE_E enType, PIC_SIZE_E enSize, SAMPLE_RC_E enRcMode,
    HI_U32 u32Profile, HI_BOOL bRcnRefShareBuf, VENC_GOP_ATTR_S *pstGopAttr);
HI_S32 SAMPLE_COMM_VENC_Stop(VENC_CHN VencChn);
HI_S32 SAMPLE_COMM_VENC_SnapStart(VENC_CHN VencChn, SIZE_S *pstSize, HI_BOOL bSupportDCF);
HI_S32 SAMPLE_COMM_VENC_SnapProcess(VENC_CHN VencChn, HI_U32 SnapCnt, HI_BOOL bSaveJpg, HI_BOOL bSaveThm);
HI_S32 SAMPLE_COMM_VENC_SaveJpeg(VENC_CHN VencChn, HI_U32 SnapCnt);
HI_S32 SAMPLE_COMM_VENC_SnapStop(VENC_CHN VencChn);
HI_S32 SAMPLE_COMM_VENC_StartGetStream(VENC_CHN VeChn[], HI_S32 s32Cnt);
HI_S32 SAMPLE_COMM_VENC_StopGetStream(void);
HI_S32 SAMPLE_COMM_VENC_StartGetStream_Svc_t(HI_S32 s32Cnt);
HI_S32 SAMPLE_COMM_VENC_GetGopAttr(VENC_GOP_MODE_E enGopMode, VENC_GOP_ATTR_S *pstGopAttr);
HI_S32 SAMPLE_COMM_VENC_QpmapSendFrame(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VeChn[], HI_S32 s32Cnt,
    SIZE_S stSize);
HI_S32 SAMPLE_COMM_VENC_StopSendQpmapFrame(void);
HI_S32 SAMPLE_COMM_VENC_SaveStream(FILE *pFd, VENC_STREAM_S *pstStream);

HI_S32 SAMPLE_COMM_REGION_Create(HI_S32 HandleNum, RGN_TYPE_E enType);
HI_S32 SAMPLE_COMM_REGION_Destroy(HI_S32 HandleNum, RGN_TYPE_E enType);
HI_S32 SAMPLE_COMM_REGION_AttachToChn(HI_S32 HandleNum, RGN_TYPE_E enType, MPP_CHN_S *pstMppChn);
HI_S32 SAMPLE_COMM_REGION_DetachFrmChn(HI_S32 HandleNum, RGN_TYPE_E enType, MPP_CHN_S *pstMppChn);
HI_S32 SAMPLE_COMM_REGION_SetBitMap(RGN_HANDLE Handle, PIXEL_FORMAT_E enPixelFmt);
HI_S32 SAMPLE_COMM_REGION_GetUpCanvas(RGN_HANDLE Handle);
HI_S32 SAMPLE_COMM_REGION_GetMinHandle(RGN_TYPE_E enType);

HI_S32 SAMPLE_COMM_AUDIO_CreateTrdAiAo(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
HI_S32 SAMPLE_COMM_AUDIO_CreateTrdAiAenc(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn);
HI_S32 SAMPLE_COMM_AUDIO_CreateTrdAencAdec(AENC_CHN AeChn, ADEC_CHN AdChn, FILE *pAecFd);
HI_S32 SAMPLE_COMM_AUDIO_CreateTrdFileAdec(ADEC_CHN AdChn, FILE *pAdcFd);
HI_S32 SAMPLE_COMM_AUDIO_CreateTrdAoVolCtrl(AUDIO_DEV AoDev);
HI_S32 SAMPLE_COMM_AUDIO_DestroyTrdAi(AUDIO_DEV AiDev, AI_CHN AiChn);
HI_S32 SAMPLE_COMM_AUDIO_DestroyTrdAencAdec(AENC_CHN AeChn);
HI_S32 SAMPLE_COMM_AUDIO_DestroyTrdFileAdec(ADEC_CHN AdChn);
HI_S32 SAMPLE_COMM_AUDIO_DestroyTrdAoVolCtrl(AUDIO_DEV AoDev);
HI_S32 SAMPLE_COMM_AUDIO_DestroyAllTrd(void);
HI_S32 SAMPLE_COMM_AUDIO_AoBindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn);
HI_S32 SAMPLE_COMM_AUDIO_AoUnbindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn);
HI_S32 SAMPLE_COMM_AUDIO_AoBindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
HI_S32 SAMPLE_COMM_AUDIO_AoUnbindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
HI_S32 SAMPLE_COMM_AUDIO_AencBindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn);
HI_S32 SAMPLE_COMM_AUDIO_AencUnbindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn);
HI_S32 SAMPLE_COMM_AUDIO_CfgAcodec(const AIO_ATTR_S *pstAioAttr);
HI_S32 SAMPLE_COMM_AUDIO_StartAi(AUDIO_DEV AiDevId, HI_S32 s32AiChnCnt, AIO_ATTR_S *pstAioAttr,
    AUDIO_SAMPLE_RATE_E enOutSampleRate, HI_BOOL bResampleEn, const HI_VOID *pstAiVqeAttr, HI_U32 u32AiVqeType);
HI_S32 SAMPLE_COMM_AUDIO_StopAi(AUDIO_DEV AiDevId, HI_S32 s32AiChnCnt, HI_BOOL bResampleEn, HI_BOOL bVqeEn);
HI_S32 SAMPLE_COMM_AUDIO_StartAo(AUDIO_DEV AoDevId, HI_S32 s32AoChnCnt, AIO_ATTR_S *pstAioAttr,
    AUDIO_SAMPLE_RATE_E enInSampleRate, HI_BOOL bResampleEn);
HI_S32 SAMPLE_COMM_AUDIO_StopAo(AUDIO_DEV AoDevId, HI_S32 s32AoChnCnt, HI_BOOL bResampleEn);
HI_S32 SAMPLE_COMM_AUDIO_StartAenc(HI_S32 s32AencChnCnt, const AIO_ATTR_S *pstAioAttr, PAYLOAD_TYPE_E enType);
HI_S32 SAMPLE_COMM_AUDIO_StopAenc(HI_S32 s32AencChnCnt);
HI_S32 SAMPLE_COMM_AUDIO_StartAdec(ADEC_CHN AdChn, PAYLOAD_TYPE_E enType);
HI_S32 SAMPLE_COMM_AUDIO_StopAdec(ADEC_CHN AdChn);

HI_S32 SAMPLE_COMM_VDEC_InitVBPool(HI_U32 ChnNum, SAMPLE_VDEC_ATTR *pastSampleVdec, HI_U32 arr_num);
HI_VOID SAMPLE_COMM_VDEC_ExitVBPool(HI_VOID);
HI_VOID SAMPLE_COMM_VDEC_StartSendStream(HI_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread,
    HI_U32 arr_len);
HI_VOID SAMPLE_COMM_VDEC_StopSendStream(HI_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread,
    HI_U32 arr_len);
HI_VOID *SAMPLE_COMM_VDEC_SendStream(HI_VOID *pArgs);
HI_VOID SAMPLE_COMM_VDEC_StartGetPic(HI_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecGet, pthread_t *pVdecThread);
HI_VOID SAMPLE_COMM_VDEC_StopGetPic(HI_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecGet, pthread_t *pVdecThread);
HI_S32 SAMPLE_COMM_VDEC_Start(HI_S32 s32ChnNum, SAMPLE_VDEC_ATTR *pastSampleVdec, HI_U32 arr_num);
HI_S32 SAMPLE_COMM_VDEC_Stop(HI_S32 s32ChnNum);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
#endif /* End of #ifndef SAMPLE_COMMON_H */
