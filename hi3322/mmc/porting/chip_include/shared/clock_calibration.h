/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description:  CLOCK CALIBRATION DRIVER
 *
 * Create: 2020-03-17
 */

#ifndef CLOCK_CALIBRATION_H
#define CLOCK_CALIBRATION_H

#include <stdint.h>
#include <stdbool.h>

#define BT_RC_32K_SUPPORT 0

#define CALIBRATION_XO_CORE_TRIM_DEFAULT        0x49
#define CALIBRATION_XO_CORE_CTRIM_MAX           0xFF
#define CALIBRATION_XO_CORE_CTRIM_MIN           1
#define NV_ID_BTSRV_XOTRIM0_CONFIG              0x1008

#define CALI_CLOCK_MUL               100

#ifdef CONFIG_ENABLE_RC_CALIBRATION
#define CLOCK_32K_CALI_FREQ (clocks_calibration_get_freq_result() / CALI_CLOCK_MUL)
#else
#define CLOCK_32K_CALI_FREQ (32768)
#endif

/** @addtogroup connectivity_drivers_non_os_clocks_core
  * @{
  */

uint32_t clocks_calibration_get_freq(uint32_t cali_count);

uint32_t clocks_calibration_get_freq_result(void);

void clocks_calibration_config_cali_cycle(uint8_t cali_count);

void clocks_calibration_enable(uint8_t enable);

bool clocks_calibration_get_done_status(void);

uint32_t clocks_calibration_get_cali_result(void);

void calibration_set_xo_core_ctrim(uint8_t xo_ctrim_value);

void calibration_xo_core_ctrim_algorithm(bool increase, uint8_t step_num);

void calibration_get_xo_core_ctrim_reg(uint8_t *xo_ctrim_value);

void calibration_set_xo_core_trim_reg(uint8_t xo_trim);

void hal_set_cali_mode(uint32_t cail_mode);

void cali_32k_init(void);

void set_cali_timer_internal(uint32_t seconds);

void cali_32k(uint32_t cali_count, uint32_t update);

void rc32k_cali_process(void);


/**
  * @}
  */
#endif
