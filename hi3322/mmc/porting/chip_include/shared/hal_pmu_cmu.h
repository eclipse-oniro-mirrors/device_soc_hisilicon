/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: Pmu and cmu hal driver header file.
 */

#ifndef HAL_PMU_CMU_H
#define HAL_PMU_CMU_H

#include <stdint.h>
#include "chip_io.h"
#include "chip_definitions.h"

/**
 * @brief  Get ANA status.
 * @return ANA status.
 */
uint16_t hal_pmu_cmu_get_ana_status(void);

/**
 * @brief  Clear ANA status.
 */
void hal_pmu_cmu_clear_ana_int_status(void);

/**
 * @brief  Enable analog status raw interrupt.
 * @param  pos Position of the intterupt.
 */
void hal_pmu_cmu_analog_status_raw_interrupt_enable(uint16_t pos);

/**
 * @brief  Disable analog status raw interrupt.
 * @param  pos Position of the intterupt.
 */
void hal_pmu_cmu_analog_status_raw_interrupt_disable(uint16_t pos);

/**
 * @brief  Config ana grim interrupt.
 */
void hal_pmu_analog_status_grm_interrupt_enable(uint16_t position, switch_type_t on);

#endif