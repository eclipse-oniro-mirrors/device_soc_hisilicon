/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides adc port \n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */

#ifndef PMU_PERP_H
#define PMU_PERP_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "pm_pmu.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

errcode_t uapi_pmu_perp_control(pmu_control_type_t type, uint8_t param);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
