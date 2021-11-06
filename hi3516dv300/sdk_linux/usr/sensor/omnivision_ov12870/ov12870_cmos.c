/*
 * Copyright (c) 2021 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ov12870_cmos.h"
#include "ov12870_cmos_ex.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"

#define OV12870_ID 12870

#define HIGH_8BITS(x) (((x) & 0xff00) >> 8)
#define LOW_8BITS(x)  ((x) & 0x00ff)

ISP_SNS_STATE_S *g_pastOv12870[ISP_MAX_PIPE_NUM] = {HI_NULL};

#define OV12870_SENSOR_SET_CTX(dev, pstCtx)   ((g_pastOv12870[dev]) = (pstCtx))
#define OV12870_SENSOR_RESET_CTX(dev)         (g_pastOv12870[dev] = HI_NULL)

static HI_U32 g_au32InitExposure[ISP_MAX_PIPE_NUM]  = {0};
static HI_U32 g_au32LinesPer500ms[ISP_MAX_PIPE_NUM] = {0};
static HI_U16 g_au16InitWBGain[ISP_MAX_PIPE_NUM][ISP_RGB_CHN_NUM] = {{0}};
static HI_U16 g_au16SampleRgain[ISP_MAX_PIPE_NUM] = {0};
static HI_U16 g_au16SampleBgain[ISP_MAX_PIPE_NUM] = {0};
static HI_BOOL g_quick_start_en[ISP_MAX_PIPE_NUM] = {HI_FALSE};

static HI_U16 g_init_ccm[ISP_MAX_PIPE_NUM][CCM_MATRIX_SIZE] = {{0}};
static HI_U32 g_au32Again[ISP_MAX_PIPE_NUM] = {1024};

static HI_BOOL g_abAERouteExValid[ISP_MAX_PIPE_NUM] = {0};
static ISP_AE_ROUTE_S g_astInitAERoute[ISP_MAX_PIPE_NUM] = {{0}};
static ISP_AE_ROUTE_EX_S g_astInitAERouteEx[ISP_MAX_PIPE_NUM] = {{0}};

ISP_SNS_COMMBUS_U g_aunOv12870BusInfo[ISP_MAX_PIPE_NUM] = {
    [0] = { .s8I2cDev = 0 },
    [1 ... ISP_MAX_PIPE_NUM - 1] = { .s8I2cDev = -1 }
};

ISP_SNS_COMMBUS_U *ov12870_get_bus_Info(VI_PIPE vi_pipe)
{
    return &g_aunOv12870BusInfo[vi_pipe];
}

ISP_SNS_STATE_S *ov12870_get_ctx(VI_PIPE vi_pipe)
{
    return g_pastOv12870[vi_pipe];
}

const OV12870_VIDEO_MODE_TBL_S g_astOv12870ModeTbl[OV12870_MODE_BUTT] = {
    { 0x0340, 0, 8, 36, 240, 0, "720P_10BIT_240FPS" },
    { 0x0682, 0, 8, 36, 120, 0, "2K1K_10BIT_120FPS" },
    { 0x0d06, 0, 8, 36, 30, 0,  "4K2K_10BIT_30FPS" },
    { 0x0d06, 0, 8, 36, 30,  0, "4K3K_10BIT_30FPS" },
};

const OV12870_VIDEO_MODE_TBL_S *ov12870_get_mode_tb1(HI_U8 u8ImgMode)
{
    return &g_astOv12870ModeTbl[u8ImgMode];
}

#define OV12870_FULL_LINES_MAX  0xFFFF

#define OV12870_ERR_MODE_PRINT(pstSensorImageMode) \
    do { \
        SNS_ERR_TRACE("Not support! Width:%d, Height:%d, Fps:%f, SnsMode:%d\n", \
                      (pstSensorImageMode)->u16Width,  \
                      (pstSensorImageMode)->u16Height, \
                      (pstSensorImageMode)->f32Fps,    \
                      (pstSensorImageMode)->u8SnsMode); \
    } while (0)

static HI_VOID cmos_get_ae_comm_default(VI_PIPE vi_pipe, AE_SENSOR_DEFAULT_S *pstAeSnsDft,
    ISP_SNS_STATE_S *pstSnsState)
{
    HI_U32 u32Fll;
    HI_FLOAT f32MaxFps;
    u32Fll = g_astOv12870ModeTbl[pstSnsState->u8ImgMode].u32VMax;
    f32MaxFps = g_astOv12870ModeTbl[pstSnsState->u8ImgMode].f32MaxFps;
    pstAeSnsDft->stIntTimeAccu.f32Offset = g_astOv12870ModeTbl[pstSnsState->u8ImgMode].u32Offset;
    pstSnsState->u32FLStd = u32Fll;
    pstAeSnsDft->u32MaxIntTime = pstSnsState->u32FLStd - g_astOv12870ModeTbl[pstSnsState->u8ImgMode].u32ExpLineLimit;

    pstAeSnsDft->u32FullLinesStd = pstSnsState->u32FLStd;
    pstAeSnsDft->u32FlickerFreq = 0;
    pstAeSnsDft->u32HmaxTimes = (1000000000) / SNS_DIV_0_TO_1_FLOAT(u32Fll * f32MaxFps); /* 1000000000ns */

    pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;

    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 0.3; /* Accuracy 0.3 */

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.3; /* Accuracy 0.3 */

    pstAeSnsDft->u32ISPDgainShift = 8; /* accuracy: 8 */
    pstAeSnsDft->u32MinISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;
    pstAeSnsDft->u32MaxISPDgainTarget = 4 << pstAeSnsDft->u32ISPDgainShift; /* max 4 */

    (hi_void)memcpy_s(&pstAeSnsDft->stPirisAttr, sizeof(ISP_PIRIS_ATTR_S),
                      &g_gstPirisAttr, sizeof(ISP_PIRIS_ATTR_S));
    pstAeSnsDft->enMaxIrisFNO = ISP_IRIS_F_NO_1_4;
    pstAeSnsDft->enMinIrisFNO = ISP_IRIS_F_NO_5_6;

    pstAeSnsDft->bAERouteExValid = HI_FALSE;
    pstAeSnsDft->stAERouteAttr.u32TotalNum = 0;
    pstAeSnsDft->stAERouteAttrEx.u32TotalNum = 0;
    if (g_au32InitExposure[vi_pipe] == 0) {
        pstAeSnsDft->u32InitExposure = 1000000; /* init 1000000 */
    } else {
        pstAeSnsDft->u32InitExposure = g_au32InitExposure[vi_pipe];
    }

    if (g_au32LinesPer500ms[vi_pipe] == 0) {
        pstAeSnsDft->u32LinesPer500ms = ((HI_U64)(u32Fll * f32MaxFps)) >> 1;
    } else {
        pstAeSnsDft->u32LinesPer500ms = g_au32LinesPer500ms[vi_pipe];
    }
    pstAeSnsDft->stQuickStart.bQuickStartEn = g_quick_start_en[vi_pipe];
    pstAeSnsDft->stQuickStart.u8BlackFrameNum = 0;
    return;
}

static HI_VOID cmos_get_ae_linear_default(VI_PIPE vi_pipe, AE_SENSOR_DEFAULT_S *pstAeSnsDft,
    ISP_SNS_STATE_S *pstSnsState)
{
    pstAeSnsDft->au8HistThresh[0] = 0xd;
    pstAeSnsDft->au8HistThresh[1] = 0x28;
    pstAeSnsDft->au8HistThresh[2] = 0x60; /* index 2 */
    pstAeSnsDft->au8HistThresh[3] = 0x80; /* index 3 */

    pstAeSnsDft->u8AeCompensation = 0x2D;
    pstAeSnsDft->enAeExpMode = AE_EXP_HIGHLIGHT_PRIOR;

    pstAeSnsDft->u32MinIntTime = g_astOv12870ModeTbl[pstSnsState->u8ImgMode].u32ExpLineMin;
    pstAeSnsDft->u32MaxIntTimeTarget = 65535; /* max 65535 */
    pstAeSnsDft->u32MinIntTimeTarget = pstAeSnsDft->u32MinIntTime;

    pstAeSnsDft->u32MaxAgain = 15872 * 2; /* max Again 15872(15.5x) * Dgain2x */
    pstAeSnsDft->u32MinAgain = 1024; /* min 1024 */
    pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
    pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;

    pstAeSnsDft->u32MaxDgain = 1024; /* max 1024 */
    pstAeSnsDft->u32MinDgain = 1024; /* min 1024 */
    pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
    pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;

    pstAeSnsDft->bAERouteExValid = g_abAERouteExValid[vi_pipe];
    (hi_void)memcpy_s(&pstAeSnsDft->stAERouteAttr, sizeof(ISP_AE_ROUTE_S),
                      &g_astInitAERoute[vi_pipe], sizeof(ISP_AE_ROUTE_S));
    (hi_void)memcpy_s(&pstAeSnsDft->stAERouteAttrEx, sizeof(ISP_AE_ROUTE_EX_S),
                      &g_astInitAERouteEx[vi_pipe], sizeof(ISP_AE_ROUTE_EX_S));
    return;
}

static HI_S32 cmos_get_ae_default(VI_PIPE vi_pipe, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    ISP_SNS_STATE_S *pstSnsState = HI_NULL;

    CMOS_CHECK_POINTER(pstAeSnsDft);
    OV12870_SENSOR_GET_CTX(vi_pipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    cmos_get_ae_comm_default(vi_pipe, pstAeSnsDft, pstSnsState);

    switch (pstSnsState->enWDRMode) {
        default:
        case WDR_MODE_NONE: {
            cmos_get_ae_linear_default(vi_pipe, pstAeSnsDft, pstSnsState);
            break;
        }
    }
    return HI_SUCCESS;
}

/* the function of sensor set fps */
static HI_VOID cmos_fps_set(VI_PIPE vi_pipe, HI_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    HI_FLOAT f32MaxFps;
    HI_U32 u32Lines;
    ISP_SNS_STATE_S *pstSnsState = HI_NULL;

    CMOS_CHECK_POINTER_VOID(pstAeSnsDft);
    OV12870_SENSOR_GET_CTX(vi_pipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    f32MaxFps = g_astOv12870ModeTbl[pstSnsState->u8ImgMode].f32MaxFps;
    u32Lines = g_astOv12870ModeTbl[pstSnsState->u8ImgMode].u32VMax * (f32MaxFps / SNS_DIV_0_TO_1_FLOAT(f32Fps));
    pstSnsState->u32FLStd = u32Lines;
    pstAeSnsDft->u32MaxIntTime = pstSnsState->u32FLStd - g_astOv12870ModeTbl[pstSnsState->u8ImgMode].u32ExpLineLimit;

    /* SHR 16bit, So limit full_lines as 0xFFFF */
    if (f32Fps > f32MaxFps || u32Lines > OV12870_FULL_LINES_MAX) {
        SNS_ERR_TRACE("Not support Fps: %f\n", f32Fps);
        return;
    }

    pstAeSnsDft->f32Fps = f32Fps;
    pstAeSnsDft->u32LinesPer500ms = g_astOv12870ModeTbl[pstSnsState->u8ImgMode].u32VMax * f32MaxFps / 2; /* div 2 */
    pstAeSnsDft->u32FullLinesStd = pstSnsState->u32FLStd;

    pstSnsState->au32FL[0] = pstSnsState->u32FLStd;
    pstAeSnsDft->u32FullLines = pstSnsState->au32FL[0];
    pstAeSnsDft->u32HmaxTimes = (1000000000) / SNS_DIV_0_TO_1_FLOAT(pstSnsState->u32FLStd * f32Fps); /* 1000000000ns */

    pstSnsState->astRegsInfo[0].astI2cData[7].u32Data = LOW_8BITS(pstSnsState->au32FL[0]); /* index 7 */
    pstSnsState->astRegsInfo[0].astI2cData[8].u32Data = HIGH_8BITS(pstSnsState->au32FL[0]); /* index 8 */

    return;
}

#define FSCLK 0xb4   /* get from 'ov12870_timing_v4.xlsx' */
#define EXPTIMEMAX 600  /* calculated maxtime is 6080s, 600s is enough */
static HI_VOID cmos_slow_framerate_set(VI_PIPE vi_pipe, HI_U32 u32FullLines,
    AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    ISP_SNS_STATE_S *pstSnsState = HI_NULL;
    HI_FLOAT fExpoTime;
    HI_U32 u32RegExpo;
    HI_U8 u8R3412, u8R3413, u8R3414, u8R3415;

    CMOS_CHECK_POINTER_VOID(pstAeSnsDft);
    OV12870_SENSOR_GET_CTX(vi_pipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);
    if (u32FullLines > OV12870_FULL_LINES_MAX) {
        fExpoTime = ((HI_FLOAT)pstAeSnsDft->u32HmaxTimes / (1000000000)) * u32FullLines; /* 1000000000ns */
        if (fExpoTime > EXPTIMEMAX) {
            SNS_ERR_TRACE("Not support long exposure time %f !\n", fExpoTime);
            return;
        }
    }
    pstSnsState->au32FL[0] = u32FullLines;
    pstAeSnsDft->u32FullLines = pstSnsState->au32FL[0];
    pstAeSnsDft->u32MaxIntTime = pstSnsState->au32FL[0] - g_astOv12870ModeTbl[pstSnsState->u8ImgMode].u32ExpLineLimit;

    if (u32FullLines > OV12870_FULL_LINES_MAX) {
        u32RegExpo = (HI_U32)(fExpoTime * FSCLK * 0xf424 / 0x10);
        u8R3412 = u32RegExpo / 0x1000000;
        u8R3413 = (u32RegExpo - u8R3412 * 0x1000000) / 0x10000;
        u8R3414 = (u32RegExpo - u8R3412 * 0x1000000 - u8R3413 * 0x10000) / 0x100;
        u8R3415 = u32RegExpo - u8R3412 * 0x1000000 - u8R3413 * 0x10000 - u8R3414 * 0x100;
        pstSnsState->astRegsInfo[0].astI2cData[9].u32Data = 0x0; /* index 9 */
        pstSnsState->astRegsInfo[0].astI2cData[10].u32Data = 0x1; /* index 10 */
        pstSnsState->astRegsInfo[0].astI2cData[11].u32Data = u8R3412; /* index 11 */
        pstSnsState->astRegsInfo[0].astI2cData[12].u32Data = u8R3413; /* index 12 */
        pstSnsState->astRegsInfo[0].astI2cData[13].u32Data = u8R3414; /* index 13 */
        pstSnsState->astRegsInfo[0].astI2cData[14].u32Data = u8R3415; /* index 14 */

        return;
    } else {
        pstSnsState->astRegsInfo[0].astI2cData[9].u32Data = 0x0; /* index 9 */
        pstSnsState->astRegsInfo[0].astI2cData[10].u32Data = 0x0; /* index 10 */
        pstSnsState->astRegsInfo[0].astI2cData[11].u32Data = 0; /* index 11 */
        pstSnsState->astRegsInfo[0].astI2cData[12].u32Data = 0; /* index 12 */
        pstSnsState->astRegsInfo[0].astI2cData[13].u32Data = 0; /* index 13 */
        pstSnsState->astRegsInfo[0].astI2cData[14].u32Data = 0; /* index 14 */
    }
    pstSnsState->astRegsInfo[0].astI2cData[7].u32Data = LOW_8BITS(u32FullLines); /* index 7 */
    pstSnsState->astRegsInfo[0].astI2cData[8].u32Data = HIGH_8BITS(u32FullLines); /* index 8 */

    return;
}

/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_VOID cmos_inttime_update(VI_PIPE vi_pipe, HI_U32 u32IntTime)
{
    ISP_SNS_STATE_S *pstSnsState = HI_NULL;

    OV12870_SENSOR_GET_CTX(vi_pipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    pstSnsState->astRegsInfo[0].astI2cData[0].u32Data = LOW_8BITS(u32IntTime);
    pstSnsState->astRegsInfo[0].astI2cData[1].u32Data = HIGH_8BITS(u32IntTime);
    pstSnsState->astRegsInfo[0].astI2cData[2].u32Data = HIGHER_8BITS(u32IntTime); /* index 2 */

    return;
}

#define OV12870_AGIN_TAB_RANGE   64
static HI_U32 g_gain_table[OV12870_AGIN_TAB_RANGE] = {
    1024, 1088, 1152, 1216, 1280,  1344,  1408,  1472,  1536,  1600,  1664,  1728,  1792,  1856,  1920,  1984,
    2048, 2176, 2304, 2432, 2560,  2688,  2816,  2944,  3072,  3200,  3328,  3456,  3584,  3712,  3840,  3968,
    4096, 4352, 4608, 4864, 5120,  5376,  5632,  5888,  6144,  6400,  6656,  6912,  7168,  7424,  7680,  7936,
    8192, 8704, 9216, 9728, 10240, 10752, 11264, 11776, 12288, 12800, 13312, 13824, 14336, 14848, 15360, 15872
};
static HI_VOID cmos_again_calc_table(VI_PIPE vi_pipe, HI_U32 *pu32AgainLin, HI_U32 *pu32AgainDb)
{
    int i;

    CMOS_CHECK_POINTER_VOID(pu32AgainLin);
    CMOS_CHECK_POINTER_VOID(pu32AgainDb);

    if (*pu32AgainLin >= g_gain_table[OV12870_AGIN_TAB_RANGE - 1]) {
        *pu32AgainDb = (*pu32AgainLin >> 3); /* shift 3 */
        *pu32AgainLin = ((*pu32AgainLin >> 3) << 3); /* shift 3 */
        g_au32Again[vi_pipe] = *pu32AgainLin;
        return;
    }

    for (i = 1; i < OV12870_AGIN_TAB_RANGE; i++) {
        if (*pu32AgainLin < g_gain_table[i]) {
            *pu32AgainLin = g_gain_table[i - 1];
            g_au32Again[vi_pipe] = *pu32AgainLin;
            *pu32AgainDb = (g_gain_table[i - 1] >> 3); /* shift 3 */
            break;
        }
    }
    return;
}

static HI_VOID cmos_dgain_calc_table(VI_PIPE vi_pipe, HI_U32 *pu32DgainLin, HI_U32 *pu32DgainDb)
{
    hi_unused(vi_pipe);
    hi_unused(pu32DgainLin);
    hi_unused(pu32DgainDb);

    return;
}

static HI_VOID cmos_gains_update(VI_PIPE vi_pipe, HI_U32 u32Again, HI_U32 u32Dgain)
{
    ISP_SNS_STATE_S *pstSnsState = HI_NULL;

    OV12870_SENSOR_GET_CTX(vi_pipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);
    hi_unused(u32Dgain);

    pstSnsState->astRegsInfo[0].astI2cData[3].u32Data = (u32Again & 0xFF); /* index 3 */
    pstSnsState->astRegsInfo[0].astI2cData[4].u32Data = ((u32Again >> 8) & 0xf); /* index 4, shift 8 */
    pstSnsState->astRegsInfo[0].astI2cData[5].u32Data = (u32Again & 0xFF); /* index 5 */
    pstSnsState->astRegsInfo[0].astI2cData[6].u32Data = ((u32Again >> 8) & 0xf); /* index 6, shift 8 */
    return;
}

static HI_S32 cmos_init_ae_exp_function(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    CMOS_CHECK_POINTER(pstExpFuncs);

    (hi_void)memset_s(pstExpFuncs, sizeof(AE_SENSOR_EXP_FUNC_S), 0, sizeof(AE_SENSOR_EXP_FUNC_S));

    pstExpFuncs->pfn_cmos_get_ae_default    = cmos_get_ae_default;
    pstExpFuncs->pfn_cmos_fps_set           = cmos_fps_set;
    pstExpFuncs->pfn_cmos_slow_framerate_set = cmos_slow_framerate_set;
    pstExpFuncs->pfn_cmos_inttime_update    = cmos_inttime_update;
    pstExpFuncs->pfn_cmos_gains_update      = cmos_gains_update;
    pstExpFuncs->pfn_cmos_again_calc_table  = cmos_again_calc_table;
    pstExpFuncs->pfn_cmos_dgain_calc_table  = cmos_dgain_calc_table;

    return HI_SUCCESS;
}

/* AWB default parameter and function */
#define CALIBRATE_STATIC_TEMP      4983
#define CALIBRATE_STATIC_WB_R_GAIN 518
#define CALIBRATE_STATIC_WB_GR_GAIN 256
#define CALIBRATE_STATIC_WB_GB_GAIN 256
#define CALIBRATE_STATIC_WB_B_GAIN 409

/* Calibration results for Auto WB Planck */
#define CALIBRATE_AWB_P1 30
#define CALIBRATE_AWB_P2 188
#define CALIBRATE_AWB_Q1 (-38)
#define CALIBRATE_AWB_A1 153679
#define CALIBRATE_AWB_B1 128
#define CALIBRATE_AWB_C1 (-101709)

/* Rgain and Bgain of the golden sample */
#define GOLDEN_RGAIN 0
#define GOLDEN_BGAIN 0

static HI_S32 cmos_get_awb_default(VI_PIPE vi_pipe, AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
    ISP_SNS_STATE_S *pstSnsState = HI_NULL;
    HI_S32 i;

    CMOS_CHECK_POINTER(pstAwbSnsDft);
    OV12870_SENSOR_GET_CTX(vi_pipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    (hi_void)memset_s(pstAwbSnsDft, sizeof(AWB_SENSOR_DEFAULT_S), 0, sizeof(AWB_SENSOR_DEFAULT_S));

    pstAwbSnsDft->u16WbRefTemp = CALIBRATE_STATIC_TEMP;

    pstAwbSnsDft->au16GainOffset[0] = CALIBRATE_STATIC_WB_R_GAIN;   /* 0: R channel */
    pstAwbSnsDft->au16GainOffset[1] = CALIBRATE_STATIC_WB_GR_GAIN;  /* 1: Gr channel */
    pstAwbSnsDft->au16GainOffset[2] = CALIBRATE_STATIC_WB_GB_GAIN;  /* 2: Gb channel */
    pstAwbSnsDft->au16GainOffset[3] = CALIBRATE_STATIC_WB_B_GAIN;   /* 3: B channel */

    pstAwbSnsDft->as32WbPara[0] = CALIBRATE_AWB_P1;       /* index0: p1 */
    pstAwbSnsDft->as32WbPara[1] = CALIBRATE_AWB_P2;       /* index1: p2 */
    pstAwbSnsDft->as32WbPara[2] = CALIBRATE_AWB_Q1;       /* index2: q1 */
    pstAwbSnsDft->as32WbPara[3] = CALIBRATE_AWB_A1;       /* index3: a1 */
    pstAwbSnsDft->as32WbPara[4] = CALIBRATE_AWB_B1;       /* index4: b1 */
    pstAwbSnsDft->as32WbPara[5] = CALIBRATE_AWB_C1;       /* index5: c1 */
    pstAwbSnsDft->u16GoldenRgain = GOLDEN_RGAIN;
    pstAwbSnsDft->u16GoldenBgain = GOLDEN_BGAIN;

    switch (pstSnsState->enWDRMode) {
        default:
        case WDR_MODE_NONE:
            (hi_void)memcpy_s(&pstAwbSnsDft->stCcm, sizeof(AWB_CCM_S), &g_stAwbCcm, sizeof(AWB_CCM_S));
            (hi_void)memcpy_s(&pstAwbSnsDft->stAgcTbl, sizeof(AWB_AGC_TABLE_S),
                              &g_stAwbAgcTable, sizeof(AWB_AGC_TABLE_S));
            break;
    }
    pstAwbSnsDft->u16InitRgain = g_au16InitWBGain[vi_pipe][0]; /* 0: Rgain */
    pstAwbSnsDft->u16InitGgain = g_au16InitWBGain[vi_pipe][1]; /* 1: Ggain */
    pstAwbSnsDft->u16InitBgain = g_au16InitWBGain[vi_pipe][2]; /* 2: Bgain */
    pstAwbSnsDft->u16SampleRgain = g_au16SampleRgain[vi_pipe];
    pstAwbSnsDft->u16SampleBgain = g_au16SampleBgain[vi_pipe];
    for (i = 0; i < CCM_MATRIX_SIZE; i++) {
        pstAwbSnsDft->au16InitCCM[i] = g_init_ccm[vi_pipe][i];
    }

    return HI_SUCCESS;
}

static HI_S32 cmos_get_awb_spec_default(VI_PIPE vi_pipe, AWB_SPEC_SENSOR_DEFAULT_S *pstAwbSpecSnsDft)
{
    CMOS_CHECK_POINTER(pstAwbSpecSnsDft);
    hi_unused(vi_pipe);

    (hi_void)memset_s(pstAwbSpecSnsDft, sizeof(AWB_SPEC_SENSOR_DEFAULT_S), 0, sizeof(AWB_SPEC_SENSOR_DEFAULT_S));

    (hi_void)memcpy_s(&pstAwbSpecSnsDft->stSpecAwbAttrs, sizeof(ISP_SPECAWB_ATTR_S), &g_SpecAWBFactTbl,
                      sizeof(ISP_SPECAWB_ATTR_S));
    (hi_void)memcpy_s(&pstAwbSpecSnsDft->stCaaControl, sizeof(ISP_SPECAWB_CAA_CONTROl_S), &g_SpecKCAWBCaaTblControl,
                      sizeof(ISP_SPECAWB_CAA_CONTROl_S));

    return HI_SUCCESS;
}

static HI_S32 cmos_init_awb_exp_function(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    CMOS_CHECK_POINTER(pstExpFuncs);

    (hi_void)memset_s(pstExpFuncs, sizeof(AWB_SENSOR_EXP_FUNC_S), 0, sizeof(AWB_SENSOR_EXP_FUNC_S));

    pstExpFuncs->pfn_cmos_get_awb_default = cmos_get_awb_default;
    pstExpFuncs->pfn_cmos_get_awb_spec_default = cmos_get_awb_spec_default;

    return HI_SUCCESS;
}

static ISP_CMOS_DNG_COLORPARAM_S g_stDngColorParam = {{ 378, 256, 430 }, { 439, 256, 439 }};
static HI_VOID cmos_get_isp_dng_default(ISP_SNS_STATE_S *pstSnsState, ISP_CMOS_DEFAULT_S *pstDef)
{
    (hi_void)memcpy_s(&pstDef->stDngColorParam, sizeof(ISP_CMOS_DNG_COLORPARAM_S), &g_stDngColorParam,
                      sizeof(ISP_CMOS_DNG_COLORPARAM_S));

    switch (pstSnsState->u8ImgMode) {
        default:
            pstDef->stSensorMode.stDngRawFormat.u8BitsPerSample = 10; /* 10bit */
            pstDef->stSensorMode.stDngRawFormat.u32WhiteLevel = 1023; /* max 1023 */
            break;
    }

    pstDef->stSensorMode.stDngRawFormat.stDefaultScale.stDefaultScaleH.u32Denominator = 1;
    pstDef->stSensorMode.stDngRawFormat.stDefaultScale.stDefaultScaleH.u32Numerator = 1;
    pstDef->stSensorMode.stDngRawFormat.stDefaultScale.stDefaultScaleV.u32Denominator = 1;
    pstDef->stSensorMode.stDngRawFormat.stDefaultScale.stDefaultScaleV.u32Numerator = 1;
    pstDef->stSensorMode.stDngRawFormat.stCfaRepeatPatternDim.u16RepeatPatternDimRows = 2; /* pattern 2 */
    pstDef->stSensorMode.stDngRawFormat.stCfaRepeatPatternDim.u16RepeatPatternDimCols = 2; /* pattern 2 */
    pstDef->stSensorMode.stDngRawFormat.stBlcRepeatDim.u16BlcRepeatRows = 2; /* pattern 2 */
    pstDef->stSensorMode.stDngRawFormat.stBlcRepeatDim.u16BlcRepeatCols = 2; /* pattern 2 */
    pstDef->stSensorMode.stDngRawFormat.enCfaLayout = CFALAYOUT_TYPE_RECTANGULAR;
    pstDef->stSensorMode.stDngRawFormat.au8CfaPlaneColor[0] = 0;
    pstDef->stSensorMode.stDngRawFormat.au8CfaPlaneColor[1] = 1;
    pstDef->stSensorMode.stDngRawFormat.au8CfaPlaneColor[2] = 2; /* index 2, CfaPlaneColor 2 */
    pstDef->stSensorMode.stDngRawFormat.au8CfaPattern[0] = 0;
    pstDef->stSensorMode.stDngRawFormat.au8CfaPattern[1] = 1;
    pstDef->stSensorMode.stDngRawFormat.au8CfaPattern[2] = 1; /* index 2, CfaPattern 1 */
    pstDef->stSensorMode.stDngRawFormat.au8CfaPattern[3] = 2; /* index 3, CfaPattern 2 */
    pstDef->stSensorMode.bValidDngRawFormat = HI_TRUE;
    return;
}

static void cmos_get_isp_linear_default(ISP_CMOS_DEFAULT_S *pstDef)
{
    pstDef->unKey.bit1Demosaic       = 1;
    pstDef->pstDemosaic              = &g_stIspDemosaic;
    pstDef->unKey.bit1Sharpen        = 1;
    pstDef->pstSharpen               = &g_stIspYuvSharpen;
    pstDef->unKey.bit1Drc            = 1;
    pstDef->pstDrc                   = &g_stIspDRC;
    pstDef->unKey.bit1Gamma          = 1;
    pstDef->pstGamma                 = &g_stIspGamma;
    pstDef->unKey.bit1BayerNr        = 1;
    pstDef->pstBayerNr               = &g_stIspBayerNr;
    pstDef->unKey.bit1AntiFalseColor = 1;
    pstDef->pstAntiFalseColor = &g_stIspAntiFalseColor;
    pstDef->unKey.bit1Ldci = 1;
    pstDef->pstLdci = &g_stIspLdci;
    pstDef->unKey.bit1Dehaze = 1;
    pstDef->pstDehaze = &g_stIspDehaze;
    pstDef->unKey.bit1Lcac = 1;
    pstDef->pstLcac = &g_stIspLCac;
    (hi_void)memcpy_s(&pstDef->stNoiseCalibration, sizeof(ISP_CMOS_NOISE_CALIBRATION_S),
                      &g_stIspNoiseCalibration, sizeof(ISP_CMOS_NOISE_CALIBRATION_S));
    return;
}

static HI_S32 cmos_get_isp_default(VI_PIPE vi_pipe, ISP_CMOS_DEFAULT_S *pstDef)
{
    ISP_SNS_STATE_S *pstSnsState = HI_NULL;

    CMOS_CHECK_POINTER(pstDef);
    OV12870_SENSOR_GET_CTX(vi_pipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    (hi_void)memset_s(pstDef, sizeof(ISP_CMOS_DEFAULT_S), 0, sizeof(ISP_CMOS_DEFAULT_S));
#ifdef CONFIG_HI_ISP_CA_SUPPORT
    pstDef->unKey.bit1Ca       = 1;
    pstDef->pstCa              = &g_stIspCA;
#endif
    pstDef->unKey.bit1Clut     = 1;
    pstDef->pstClut            = &g_stIspCLUT;

    pstDef->unKey.bit1Dpc      = 1;
    pstDef->pstDpc             = &g_stCmosDpc;

    pstDef->unKey.bit1Wdr      = 1;
    pstDef->pstWdr             = &g_stIspWDR;
#ifdef CONFIG_HI_ISP_HLC_SUPPORT
    pstDef->unKey.bit1Hlc      = 0;
    pstDef->pstHlc             = &g_stIspHlc;
#endif
    if (pstSnsState->u8ImgMode == OV12870_8M_30FPS_10BIT_LINEAR_MODE) {
        pstDef->unKey.bit1Lsc      = 1;
        pstDef->pstLsc             = &g_stCmosLsc_8M;
    } else if (pstSnsState->u8ImgMode == OV12870_12M_30FPS_10BIT_LINEAR_MODE) {
        pstDef->unKey.bit1Lsc      = 1;
        pstDef->pstLsc             = &g_stCmosLsc_12M;
    } else if (pstSnsState->u8ImgMode == OV12870_2M_120FPS_10BIT_LINEAR_MODE) {
        pstDef->unKey.bit1Lsc      = 1;
        pstDef->pstLsc             = &g_stCmosLsc_2M;
    }
#ifdef CONFIG_HI_ISP_EDGEMARK_SUPPORT
    pstDef->unKey.bit1EdgeMark = 0;
    pstDef->pstEdgeMark        = &g_stIspEdgeMark;
#endif
#ifdef CONFIG_HI_ISP_CR_SUPPORT
    pstDef->unKey.bit1Ge       = 1;
    pstDef->pstGe              = &g_stIspGe;
#endif
    pstDef->unKey.bit1Detail   = 1;
    pstDef->pstDetail          = &g_stIspDetail;

    cmos_get_isp_linear_default(pstDef);

    pstDef->stSensorMode.u32SensorID = OV12870_ID;
    pstDef->stSensorMode.u8SensorMode = pstSnsState->u8ImgMode;

    cmos_get_isp_dng_default(pstSnsState, pstDef);

    return HI_SUCCESS;
}

static HI_S32 cmos_get_isp_black_level(VI_PIPE vi_pipe, ISP_CMOS_BLACK_LEVEL_S *pstBlackLevel)
{
    CMOS_CHECK_POINTER(pstBlackLevel);

    /* It need to update black level when iso change */
    pstBlackLevel->bUpdate = HI_TRUE;

    if (g_au32Again[vi_pipe] < 4096) { /* adgain thresh 4096 */
        pstBlackLevel->au16BlackLevel[0] = 255; /* 0: R channel  255 */
        pstBlackLevel->au16BlackLevel[1] = 255; /* 1: Gr channel 255 */
        pstBlackLevel->au16BlackLevel[2] = 255; /* 2: Gb channel 255 */
        pstBlackLevel->au16BlackLevel[3] = 255; /* 3: B channel  255 */
    } else if (g_au32Again[vi_pipe] < 8192) { /* adgain thresh 8192 */
        pstBlackLevel->au16BlackLevel[0] = 252; /* 0: R channel  252 */
        pstBlackLevel->au16BlackLevel[1] = 252; /* 1: Gr channel 252 */
        pstBlackLevel->au16BlackLevel[2] = 252; /* 2: Gb channel 252 */
        pstBlackLevel->au16BlackLevel[3] = 252; /* 3: B channel  252 */
    } else if (g_au32Again[vi_pipe] < 15872) { /* adgain thresh 15872 */
        pstBlackLevel->au16BlackLevel[0] = 250; /* 0: R channel  250 */
        pstBlackLevel->au16BlackLevel[1] = 250; /* 1: Gr channel 250 */
        pstBlackLevel->au16BlackLevel[2] = 250; /* 2: Gb channel 250 */
        pstBlackLevel->au16BlackLevel[3] = 250; /* 3: B channel  250 */
    } else {
        pstBlackLevel->au16BlackLevel[0] = 255; /* 0: R channel  255 */
        pstBlackLevel->au16BlackLevel[1] = 255; /* 1: Gr channel 255 */
        pstBlackLevel->au16BlackLevel[2] = 255; /* 2: Gb channel 255 */
        pstBlackLevel->au16BlackLevel[3] = 255; /* 3: B channel  255 */
    }

    return HI_SUCCESS;
}

static HI_VOID cmos_set_pixel_detect(VI_PIPE vi_pipe, HI_BOOL bEnable)
{
    ISP_SNS_STATE_S *pstSnsState = HI_NULL;
    HI_U32 u32Again;
    HI_U32 u32CoarseTime;
    HI_U32 u32Vmax;

    OV12870_SENSOR_GET_CTX(vi_pipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    if (bEnable) { /* setup for ISP pixel calibration mode */
        u32Again = (1024 >> 3); /* 1024 shift 3 */
        u32Vmax = g_astOv12870ModeTbl[pstSnsState->u8ImgMode].u32VMax *
                  g_astOv12870ModeTbl[pstSnsState->u8ImgMode].f32MaxFps / 5; /* 5fps */
        u32Vmax = MIN(u32Vmax, OV12870_FULL_LINES_MAX);
        u32CoarseTime = u32Vmax - g_astOv12870ModeTbl[pstSnsState->u8ImgMode].u32ExpLineLimit;
        ov12870_write_register(vi_pipe, OV12870_COARSE_INTEG_TIME_L, LOW_8BITS(u32CoarseTime));
        ov12870_write_register(vi_pipe, OV12870_COARSE_INTEG_TIME_M, HIGH_8BITS(u32CoarseTime));
        ov12870_write_register(vi_pipe, OV12870_COARSE_INTEG_TIME_H, HIGHER_8BITS(u32CoarseTime));
        ov12870_write_register(vi_pipe, OV12870_ANA_GAIN_GLOBAL_L_0, (u32Again & 0xFF));
        ov12870_write_register(vi_pipe, OV12870_ANA_GAIN_GLOBAL_H_0, ((u32Again >> 8) & 0xf)); /* shift 8 */
        ov12870_write_register(vi_pipe, OV12870_ANA_GAIN_GLOBAL_L_1, (u32Again & 0xFF));
        ov12870_write_register(vi_pipe, OV12870_ANA_GAIN_GLOBAL_H_1, ((u32Again >> 8) & 0xf)); /* shift 8 */
        ov12870_write_register(vi_pipe, OV12870_VMAX_L, LOW_8BITS(u32Vmax));
        ov12870_write_register(vi_pipe, OV12870_VMAX_H, HIGH_8BITS(u32Vmax));
    } else { /* setup for ISP 'normal mode' */
        u32Vmax = pstSnsState->u32FLStd;
        ov12870_write_register(vi_pipe, OV12870_VMAX_L, LOW_8BITS(u32Vmax));
        ov12870_write_register(vi_pipe, OV12870_VMAX_L, HIGH_8BITS(u32Vmax));
        pstSnsState->bSyncInit  = HI_FALSE;
    }
    return;
}

static HI_S32 cmos_set_wdr_mode(VI_PIPE vi_pipe, HI_U8 u8Mode)
{
    ISP_SNS_STATE_S *pstSnsState = HI_NULL;

    OV12870_SENSOR_GET_CTX(vi_pipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    pstSnsState->bSyncInit = HI_FALSE;

    switch (u8Mode & 0x3F) {
        case WDR_MODE_NONE:
            pstSnsState->enWDRMode = WDR_MODE_NONE;
            printf("linear mode\n");
            break;
        default:
            SNS_ERR_TRACE("NOT support this mode!\n");
            return HI_FAILURE;
    }

    (hi_void)memset_s(pstSnsState->au32WDRIntTime,
                      sizeof(pstSnsState->au32WDRIntTime), 0, sizeof(pstSnsState->au32WDRIntTime));
    return HI_SUCCESS;
}

static HI_VOID cmos_comm_sns_reg_info_init(VI_PIPE vi_pipe, ISP_SNS_STATE_S *pstSnsState)
{
    hi_u32 i;
    pstSnsState->astRegsInfo[0].enSnsType = ISP_SNS_I2C_TYPE;
    pstSnsState->astRegsInfo[0].unComBus.s8I2cDev = g_aunOv12870BusInfo[vi_pipe].s8I2cDev;
    pstSnsState->astRegsInfo[0].u8Cfg2ValidDelayMax = 2; /* DelayMax 2 */
    pstSnsState->astRegsInfo[0].u32RegNum = 15; /* RegNum 15 */

    for (i = 0; i < pstSnsState->astRegsInfo[0].u32RegNum; i++) {
        pstSnsState->astRegsInfo[0].astI2cData[i].bUpdate = HI_TRUE;
        pstSnsState->astRegsInfo[0].astI2cData[i].u8DevAddr = OV12870_I2C_ADDR;
        pstSnsState->astRegsInfo[0].astI2cData[i].u32AddrByteNum = OV12870_ADDR_BYTE;
        pstSnsState->astRegsInfo[0].astI2cData[i].u32DataByteNum = OV12870_DATA_BYTE;
    }

    /* shutter related */
    pstSnsState->astRegsInfo[0].astI2cData[0].u8DelayFrmNum = 0;
    pstSnsState->astRegsInfo[0].astI2cData[0].u32RegAddr = OV12870_COARSE_INTEG_TIME_L;
    pstSnsState->astRegsInfo[0].astI2cData[1].u8DelayFrmNum = 0;
    pstSnsState->astRegsInfo[0].astI2cData[1].u32RegAddr = OV12870_COARSE_INTEG_TIME_M;
    pstSnsState->astRegsInfo[0].astI2cData[2].u8DelayFrmNum = 0; /* index 2 */
    pstSnsState->astRegsInfo[0].astI2cData[2].u32RegAddr = OV12870_COARSE_INTEG_TIME_H; /* index 2 */

    /* gain related */
    pstSnsState->astRegsInfo[0].astI2cData[3].u32RegAddr = OV12870_ANA_GAIN_GLOBAL_L_0; /* index 3 */
    pstSnsState->astRegsInfo[0].astI2cData[3].u8DelayFrmNum = 0; /* index 3 */
    pstSnsState->astRegsInfo[0].astI2cData[4].u32RegAddr = OV12870_ANA_GAIN_GLOBAL_H_0; /* index 4 */
    pstSnsState->astRegsInfo[0].astI2cData[4].u8DelayFrmNum = 0; /* index 4 */

    /* Dgain_gr */
    pstSnsState->astRegsInfo[0].astI2cData[5].u8DelayFrmNum = 0; /* index 5 */
    pstSnsState->astRegsInfo[0].astI2cData[5].u32RegAddr = OV12870_ANA_GAIN_GLOBAL_L_1; /* index 5 */
    pstSnsState->astRegsInfo[0].astI2cData[6].u8DelayFrmNum = 0; /* index 6 */
    pstSnsState->astRegsInfo[0].astI2cData[6].u32RegAddr = OV12870_ANA_GAIN_GLOBAL_H_1; /* index 6 */

    /* Vmax */
    pstSnsState->astRegsInfo[0].astI2cData[7].u8DelayFrmNum = 0; /* index 7 */
    pstSnsState->astRegsInfo[0].astI2cData[7].u32RegAddr = OV12870_VMAX_L; /* index 7 */
    pstSnsState->astRegsInfo[0].astI2cData[8].u8DelayFrmNum = 0; /* index 8 */
    pstSnsState->astRegsInfo[0].astI2cData[8].u32RegAddr = OV12870_VMAX_H; /* index 8 */

    /* long exposure */
    pstSnsState->astRegsInfo[0].astI2cData[9].u8DelayFrmNum = 1; /* index 9 */
    pstSnsState->astRegsInfo[0].astI2cData[9].u32RegAddr = OV12870_LONG_EXPOSURE_0; /* index 9 */
    pstSnsState->astRegsInfo[0].astI2cData[10].u8DelayFrmNum = 1; /* index 10 */
    pstSnsState->astRegsInfo[0].astI2cData[10].u32RegAddr = OV12870_LONG_EXPOSURE_1; /* index 10 */
    pstSnsState->astRegsInfo[0].astI2cData[11].u8DelayFrmNum = 1; /* index 11 */
    pstSnsState->astRegsInfo[0].astI2cData[11].u32RegAddr = OV12870_LONG_EXPOSURE_2; /* index 11 */
    pstSnsState->astRegsInfo[0].astI2cData[12].u8DelayFrmNum = 1; /* index 12 */
    pstSnsState->astRegsInfo[0].astI2cData[12].u32RegAddr = OV12870_LONG_EXPOSURE_3; /* index 12 */
    pstSnsState->astRegsInfo[0].astI2cData[13].u8DelayFrmNum = 1; /* index 13 */
    pstSnsState->astRegsInfo[0].astI2cData[13].u32RegAddr = OV12870_LONG_EXPOSURE_4; /* index 13 */
    pstSnsState->astRegsInfo[0].astI2cData[14].u8DelayFrmNum = 1; /* index 14 */
    pstSnsState->astRegsInfo[0].astI2cData[14].u32RegAddr = OV12870_LONG_EXPOSURE_5; /* index 14 */

    pstSnsState->bSyncInit = HI_TRUE;
    return;
}

static HI_VOID cmos_sns_reg_info_update(VI_PIPE vi_pipe, ISP_SNS_STATE_S *pstSnsState)
{
    hi_u32 i;
    hi_unused(vi_pipe);

    for (i = 0; i < pstSnsState->astRegsInfo[0].u32RegNum; i++) {
        if (pstSnsState->astRegsInfo[0].astI2cData[i].u32Data ==
            pstSnsState->astRegsInfo[1].astI2cData[i].u32Data) {
            pstSnsState->astRegsInfo[0].astI2cData[i].bUpdate = HI_FALSE;
        } else {
            pstSnsState->astRegsInfo[0].astI2cData[i].bUpdate = HI_TRUE;
        }
    }
    return;
}

static HI_S32 cmos_get_sns_regs_info(VI_PIPE vi_pipe, ISP_SNS_REGS_INFO_S *pstSnsRegsInfo)
{
    ISP_SNS_STATE_S *pstSnsState = HI_NULL;

    CMOS_CHECK_POINTER(pstSnsRegsInfo);
    OV12870_SENSOR_GET_CTX(vi_pipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    if ((pstSnsState->bSyncInit == HI_FALSE) || (pstSnsRegsInfo->bConfig == HI_FALSE)) {
        cmos_comm_sns_reg_info_init(vi_pipe, pstSnsState);
    } else {
        cmos_sns_reg_info_update(vi_pipe, pstSnsState);
    }
    (hi_void)memcpy_s(pstSnsRegsInfo, sizeof(ISP_SNS_REGS_INFO_S),
                      &pstSnsState->astRegsInfo[0], sizeof(ISP_SNS_REGS_INFO_S));
    (hi_void)memcpy_s(&pstSnsState->astRegsInfo[1], sizeof(ISP_SNS_REGS_INFO_S),
                      &pstSnsState->astRegsInfo[0], sizeof(ISP_SNS_REGS_INFO_S));

    pstSnsState->au32FL[1] = pstSnsState->au32FL[0];
    return HI_SUCCESS;
}

static HI_S32 cmos_comm_set_image_mode(HI_U32 u32W, HI_U32 u32H, HI_U8 *u8SensorImageMode,
    HI_U8 u8SnsMode, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
    if (OV12870_RES_IS_1M(u32W, u32H)) {
        if (u8SnsMode == 0) {
            *u8SensorImageMode = OV12870_1M_240FPS_10BIT_LINEAR_MODE;
            return HI_SUCCESS;
        } else {
            OV12870_ERR_MODE_PRINT(pstSensorImageMode);
            return HI_FAILURE;
        }
    }
    if (OV12870_RES_IS_2M(u32W, u32H)) {
        if (u8SnsMode == 0) {
            *u8SensorImageMode = OV12870_2M_120FPS_10BIT_LINEAR_MODE;
            return HI_SUCCESS;
        } else {
            OV12870_ERR_MODE_PRINT(pstSensorImageMode);
            return HI_FAILURE;
        }
    }
    if (OV12870_RES_IS_8M(u32W, u32H)) {
        if (u8SnsMode == 0) {
            *u8SensorImageMode = OV12870_8M_30FPS_10BIT_LINEAR_MODE;
            return HI_SUCCESS;
        } else {
            OV12870_ERR_MODE_PRINT(pstSensorImageMode);
            return HI_FAILURE;
        }
    }
    if (OV12870_RES_IS_12M(u32W, u32H)) {
        if (u8SnsMode == 0) {
            *u8SensorImageMode = OV12870_12M_30FPS_10BIT_LINEAR_MODE;
        } else {
            OV12870_ERR_MODE_PRINT(pstSensorImageMode);
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}

static HI_S32 cmos_set_image_mode(VI_PIPE vi_pipe, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
    HI_U8 u8SensorImageMode;
    ISP_SNS_STATE_S *pstSnsState = HI_NULL;
    HI_U8 u8SnsMode;
    HI_U32 u32W, u32H;
    HI_S32 ret;
    CMOS_CHECK_POINTER(pstSensorImageMode);
    OV12870_SENSOR_GET_CTX(vi_pipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    u8SensorImageMode = pstSnsState->u8ImgMode;
    pstSnsState->bSyncInit = HI_FALSE;
    u32H = pstSensorImageMode->u16Height;
    u32W = pstSensorImageMode->u16Width;
    u8SnsMode = pstSensorImageMode->u8SnsMode;

    ret = cmos_comm_set_image_mode(u32W, u32H, &u8SensorImageMode, u8SnsMode, pstSensorImageMode);
    if (ret == HI_FAILURE) {
        return HI_FAILURE;
    }

    if ((pstSnsState->bInit == HI_TRUE) && (u8SensorImageMode == pstSnsState->u8ImgMode)) {
        return ISP_DO_NOT_NEED_SWITCH_IMAGEMODE;
    }

    pstSnsState->u8ImgMode = u8SensorImageMode;
    pstSnsState->u32FLStd  = g_astOv12870ModeTbl[pstSnsState->u8ImgMode].u32VMax;
    pstSnsState->au32FL[0] = pstSnsState->u32FLStd;
    pstSnsState->au32FL[1] = pstSnsState->au32FL[0];
    return HI_SUCCESS;
}

static HI_VOID sensor_global_init(VI_PIPE vi_pipe)
{
    ISP_SNS_STATE_S *pstSnsState = HI_NULL;

    OV12870_SENSOR_GET_CTX(vi_pipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    pstSnsState->bInit = HI_FALSE;
    pstSnsState->bSyncInit = HI_FALSE;
    pstSnsState->u8ImgMode = OV12870_2M_120FPS_10BIT_LINEAR_MODE;
    pstSnsState->enWDRMode = WDR_MODE_NONE;
    pstSnsState->u32FLStd = g_astOv12870ModeTbl[pstSnsState->u8ImgMode].u32VMax;
    pstSnsState->au32FL[0] = g_astOv12870ModeTbl[pstSnsState->u8ImgMode].u32VMax;
    pstSnsState->au32FL[1] = g_astOv12870ModeTbl[pstSnsState->u8ImgMode].u32VMax;

    (hi_void)memset_s(&pstSnsState->astRegsInfo[0], sizeof(ISP_SNS_REGS_INFO_S), 0, sizeof(ISP_SNS_REGS_INFO_S));
    (hi_void)memset_s(&pstSnsState->astRegsInfo[1], sizeof(ISP_SNS_REGS_INFO_S), 0, sizeof(ISP_SNS_REGS_INFO_S));
    return;
}

static HI_S32 cmos_init_sensor_exp_function(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
    CMOS_CHECK_POINTER(pstSensorExpFunc);

    (hi_void)memset_s(pstSensorExpFunc, sizeof(ISP_SENSOR_EXP_FUNC_S), 0, sizeof(ISP_SENSOR_EXP_FUNC_S));

    pstSensorExpFunc->pfn_cmos_sensor_init = ov12870_init;
    pstSensorExpFunc->pfn_cmos_sensor_exit = ov12870_exit;
    pstSensorExpFunc->pfn_cmos_sensor_global_init = sensor_global_init;
    pstSensorExpFunc->pfn_cmos_set_image_mode = cmos_set_image_mode;
    pstSensorExpFunc->pfn_cmos_set_wdr_mode = cmos_set_wdr_mode;
    pstSensorExpFunc->pfn_cmos_get_isp_default = cmos_get_isp_default;
    pstSensorExpFunc->pfn_cmos_get_isp_black_level = cmos_get_isp_black_level;
    pstSensorExpFunc->pfn_cmos_set_pixel_detect = cmos_set_pixel_detect;
    pstSensorExpFunc->pfn_cmos_get_sns_reg_info = cmos_get_sns_regs_info;

    return HI_SUCCESS;
}

static HI_S32 ov12870_set_bus_info(VI_PIPE vi_pipe, ISP_SNS_COMMBUS_U unSNSBusInfo)
{
    g_aunOv12870BusInfo[vi_pipe].s8I2cDev = unSNSBusInfo.s8I2cDev;
    return HI_SUCCESS;
}

static HI_S32 sensor_ctx_init(VI_PIPE vi_pipe)
{
    ISP_SNS_STATE_S *pastSnsStateCtx = HI_NULL;

    OV12870_SENSOR_GET_CTX(vi_pipe, pastSnsStateCtx);

    if (pastSnsStateCtx == HI_NULL) {
        pastSnsStateCtx = (ISP_SNS_STATE_S *)malloc(sizeof(ISP_SNS_STATE_S));
        if (pastSnsStateCtx == HI_NULL) {
            SNS_ERR_TRACE("Isp[%d] SnsCtx malloc memory failed!\n", vi_pipe);
            return HI_ERR_ISP_NOMEM;
        }
    }

    (hi_void)memset_s(pastSnsStateCtx, sizeof(ISP_SNS_STATE_S), 0, sizeof(ISP_SNS_STATE_S));

    OV12870_SENSOR_SET_CTX(vi_pipe, pastSnsStateCtx);
    return HI_SUCCESS;
}

static HI_VOID sensor_ctx_exit(VI_PIPE vi_pipe)
{
    ISP_SNS_STATE_S *pastSnsStateCtx = HI_NULL;

    OV12870_SENSOR_GET_CTX(vi_pipe, pastSnsStateCtx);
    SENSOR_FREE(pastSnsStateCtx);
    OV12870_SENSOR_RESET_CTX(vi_pipe);
    return;
}

static HI_S32 sensor_register_callback(VI_PIPE vi_pipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    HI_S32 s32Ret;
    ISP_SENSOR_REGISTER_S stIspRegister;
    AE_SENSOR_REGISTER_S  stAeRegister;
    AWB_SENSOR_REGISTER_S stAwbRegister;
    ISP_SNS_ATTR_INFO_S   stSnsAttrInfo;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = sensor_ctx_init(vi_pipe);
    if (s32Ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    stSnsAttrInfo.eSensorId = OV12870_ID;
    s32Ret = cmos_init_sensor_exp_function(&stIspRegister.stSnsExp);
    if (s32Ret != HI_SUCCESS) {
        SNS_ERR_TRACE("cmos init exp function failed!\n");
        return HI_FAILURE;
    }
    s32Ret = HI_MPI_ISP_SensorRegCallBack(vi_pipe, &stSnsAttrInfo, &stIspRegister);
    if (s32Ret != HI_SUCCESS) {
        SNS_ERR_TRACE("sensor register callback function failed!\n");
        return s32Ret;
    }

    s32Ret = cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    if (s32Ret != HI_SUCCESS) {
        SNS_ERR_TRACE("cmos init ae exp function failed!\n");
        return HI_FAILURE;
    }
    s32Ret = HI_MPI_AE_SensorRegCallBack(vi_pipe, pstAeLib, &stSnsAttrInfo, &stAeRegister);
    if (s32Ret != HI_SUCCESS) {
        SNS_ERR_TRACE("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    s32Ret = cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    if (s32Ret != HI_SUCCESS) {
        SNS_ERR_TRACE("cmos init awb exp function failed!\n");
        return HI_FAILURE;
    }
    s32Ret = HI_MPI_AWB_SensorRegCallBack(vi_pipe, pstAwbLib, &stSnsAttrInfo, &stAwbRegister);
    if (s32Ret != HI_SUCCESS) {
        SNS_ERR_TRACE("sensor register callback function to awb lib failed!\n");
        return s32Ret;
    }

    return HI_SUCCESS;
}

static HI_S32 sensor_unregister_callback(VI_PIPE vi_pipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    HI_S32 s32Ret;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(vi_pipe, OV12870_ID);
    if (s32Ret != HI_SUCCESS) {
        SNS_ERR_TRACE("sensor unregister callback function failed!\n");
        return s32Ret;
    }

    s32Ret = HI_MPI_AE_SensorUnRegCallBack(vi_pipe, pstAeLib, OV12870_ID);
    if (s32Ret != HI_SUCCESS) {
        SNS_ERR_TRACE("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(vi_pipe, pstAwbLib, OV12870_ID);
    if (s32Ret != HI_SUCCESS) {
        SNS_ERR_TRACE("sensor unregister callback function to awb lib failed!\n");
        return s32Ret;
    }

    sensor_ctx_exit(vi_pipe);
    return HI_SUCCESS;
}

static HI_S32 sensor_set_init(VI_PIPE vi_pipe, ISP_INIT_ATTR_S *pstInitAttr)
{
    HI_S32 i;
    CMOS_CHECK_POINTER(pstInitAttr);

    g_au32InitExposure[vi_pipe] = pstInitAttr->u32Exposure;
    g_au32LinesPer500ms[vi_pipe] = pstInitAttr->u32LinesPer500ms;
    g_au16InitWBGain[vi_pipe][0] = pstInitAttr->u16WBRgain;
    g_au16InitWBGain[vi_pipe][1] = pstInitAttr->u16WBGgain;
    g_au16InitWBGain[vi_pipe][2] = pstInitAttr->u16WBBgain; /* index 2 */
    g_au16SampleRgain[vi_pipe] = pstInitAttr->u16SampleRgain;
    g_au16SampleBgain[vi_pipe] = pstInitAttr->u16SampleBgain;
    g_quick_start_en[vi_pipe] = pstInitAttr->stQuickStart.bQuickStartEn;

    g_abAERouteExValid[vi_pipe] = pstInitAttr->bAERouteExValid;
    (hi_void)memcpy_s(&g_astInitAERoute[vi_pipe], sizeof(ISP_AE_ROUTE_S),
                      &pstInitAttr->stAERoute, sizeof(ISP_AE_ROUTE_S));
    (hi_void)memcpy_s(&g_astInitAERouteEx[vi_pipe], sizeof(ISP_AE_ROUTE_EX_S),
                      &pstInitAttr->stAERouteEx, sizeof(ISP_AE_ROUTE_EX_S));
    for (i = 0; i < CCM_MATRIX_SIZE; i++) {
        g_init_ccm[vi_pipe][i] = pstInitAttr->au16CCM[i];
    }
    return HI_SUCCESS;
}

ISP_SNS_OBJ_S stSnsOv12870Obj = {
    .pfnRegisterCallback    = sensor_register_callback,
    .pfnUnRegisterCallback  = sensor_unregister_callback,
    .pfnStandby             = ov12870_standby,
    .pfnRestart             = ov12870_restart,
    .pfnMirrorFlip          = ov12870_mirror_flip,
    .pfnWriteReg            = ov12870_write_register,
    .pfnReadReg             = ov12870_read_register,
    .pfnSetBusInfo          = ov12870_set_bus_info,
    .pfnSetInit             = sensor_set_init
};
