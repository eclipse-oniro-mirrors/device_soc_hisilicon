/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PMU INTERRUPT FUNCTIONS
 */

#ifndef SRC_DRIVERS_HAL_APPLICATION_HAL_PMU_INTERRUPT_H
#define SRC_DRIVERS_HAL_APPLICATION_HAL_PMU_INTERRUPT_H

#include <stdint.h>
#include "chip_io.h"

typedef enum {
    PMU_CMU_GRM_INT,
    PMU_CMU_RAW_INT,
} pmu_cmu_err_int_type_t;

/**
 * @brief  Clear PMU error interrupt.
 * @param  int_type         PMU_CMU_GRM_INT/PMU_CMU_RAW_INT.
 */
void hal_pmu_err_irq_clear(pmu_cmu_err_int_type_t int_type);

/**
 * @brief  Get the exception type that triggered the PMU error interrupt.
 * @param  int_type         PMU_CMU_GRM_INT/PMU_CMU_RAW_INT.
 * @return The value of the 1stick register.
 */
uint32_t hal_pmu_err_irq_get_status(pmu_cmu_err_int_type_t int_type);

/**
 * @brief  Get the enable type of PMU error interrupt
 * @param  int_type         PMU_CMU_GRM_INT/PMU_CMU_RAW_INT.
 * @return The value of the int_en register.
 */
uint32_t hal_pmu_err_irq_get_enable_status(pmu_cmu_err_int_type_t int_type);

/**
  * @}
  */
#endif
