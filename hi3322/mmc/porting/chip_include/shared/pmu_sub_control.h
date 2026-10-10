/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2022. All rights reserved.
 * Description:  PMU DRIVER HEADER FILE.
 */

#ifndef PMU_SUBSYSTEM_H
#define PMU_SUBSYSTEM_H

#include "chip_io.h"
#include "hal_pmu_ldo.h"
#include "errcode.h"
#include "pm_pmu_porting.h"

#define PMU_SUB_COUNT                   6

typedef enum {
    HS_SUB_FMC_ID,
    HS_SUB_EMMC_ID,
    HS_SUB_SDIO_HOST_ID,
    HS_SUB_USB_ID,
    HS_SUB_CAN_ID,
    HS_SUB_VICAP_ID,
    HS_SUB_MAX_ID,
} pm_sid_t;

typedef struct pmu_sub_info {
    uint32_t time_stamp;
    uint32_t runtime;
    uint32_t poweron_cnt;
    uint32_t poweroff_cnt;
}pmu_sub_info_t;

void startup_other_core(void);

void pm_video_sub_power_on(void);

void pm_jpgd_h264_power_on(void);

errcode_t uapi_pmu_control(pmu_control_type_t type, uint8_t param);

errcode_t uapi_pmu_hs_sub_control(uint8_t sid, uint8_t param);

pmu_sub_info_t *get_pmu_sub_info(void);

uint8_t get_pmu_audio_poweron_flag(void);
#endif
