/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2022. All rights reserved.
 * Description:  PMU DRIVER HEADER FILE
 */

#ifndef PMU_CMU_H
#define PMU_CMU_H

#include "chip_io.h"
#include "chip_definitions.h"
#include "platform_core.h"
#include "hal_pmu_cmu.h"

/** @defgroup connectivity_drivers_non_os_pmu_cmu PMU CMU
  * @ingroup  connectivity_drivers_non_os_pmu
  * @{
  */

typedef enum {
    PMU_CMU_FNPLL,
    PMU_CMU_MAX,
} pmu_cmu_core_t;

/**
 * @brief  Cmu pll reinit.
 */
uint32_t  pmu_cmu_pll_reinit(pmu_cmu_core_t pll);

/**
 * @brief  Cmu pll reinit. with retry
 */
void pmu_cmu_pll_reinit_with_retry(pmu_cmu_core_t pll);

/**
 * @brief  Cmu pll power off.
 */
void pmu_cmu_pll_deinit(pmu_cmu_core_t pll);

/**
 * @brief  Get pll config value.
 * @return FNPLL clock value.
 */
uint32_t pmu_cmu_get_pll_clock_value(void);

/**
  * @}
  */
#endif
