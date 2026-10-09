/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2022. All rights reserved.
 * Description:  PMU DRIVER HEADER FILE.
 */

#ifndef PMU_MEM_H
#define PMU_MEM_H

#include "errcode.h"
#include "pm_pmu_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

errcode_t uapi_pmu_mem_control(pmu_mem_type_t id, uint8_t state);

errcode_t uapi_pmu_mem_fb_on(void);

errcode_t uapi_pmu_mem_fb_off(void);

void uapi_pmu_mem_0p8_set(void);

void uapi_pmu_mem_0p9_set(void);

void uapi_pmu_mem_codec_0p8_set(void);

void uapi_pmu_mem_codec_0p9_set(void);

void uapi_pmu_mem_video_0p8_set(void);

void uapi_pmu_mem_video_0p9_set(void);

void uapi_pmu_mem_hs_0p8_set(void);

void uapi_pmu_mem_hs_0p9_set(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
