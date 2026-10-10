/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description:   HAL APP CLOCK DRIVER HEADER FILE
 *
 * Create: 2020-01-13
 */

#ifndef HAL_CLOCKS_APP_H
#define HAL_CLOCKS_APP_H

#include <stdint.h>
#include <chip_io.h>
#include "clocks_ccrg.h"

/** @addtogroup connectivity_drivers_hal_clock
  * @{
  */

/**
 * @brief  Application peripheral clocks.
 */
typedef enum {
    /* Clocks M_CLKEN0 region */
    HAL_CLOCKS_APP_MCLK_EN0_BASE,
    HAL_CLOCKS_APP_SDIO_APB_PCLK    = HAL_CLOCKS_APP_MCLK_EN0_BASE + 0,
    HAL_CLOCKS_APP_DIAG_CLK         = HAL_CLOCKS_APP_MCLK_EN0_BASE + 1,
    HAL_CLOCKS_APP_SEC_PKE_CLK      = HAL_CLOCKS_APP_MCLK_EN0_BASE + 2,
    HAL_CLOCKS_APP_GPIO_CLK         = HAL_CLOCKS_APP_MCLK_EN0_BASE + 3,
    HAL_CLOCKS_APP_SEC_KM_CLK       = HAL_CLOCKS_APP_MCLK_EN0_BASE + 4,
    HAL_CLOCKS_APP_GPIO_PERP_CLK    = HAL_CLOCKS_APP_MCLK_EN0_BASE + 5,
    HAL_CLOCKS_APP_SEC_BUS_CLK      = HAL_CLOCKS_APP_MCLK_EN0_BASE + 6,
    HAL_CLOCKS_APP_PWM_CLK          = HAL_CLOCKS_APP_MCLK_EN0_BASE + 7,
    HAL_CLOCKS_APP_GLB_MEM_BUS_CLK  = HAL_CLOCKS_APP_MCLK_EN0_BASE + 8,   // clk_mem_bus的时钟CG
    HAL_CLOCKS_APP_HS_SUB_CLK       = HAL_CLOCKS_APP_MCLK_EN0_BASE + 10,  // clk_mcu_to_hs_ah2h时钟使能
    HAL_CLOCKS_APP_DIAG_P2P_CLK     = HAL_CLOCKS_APP_MCLK_EN0_BASE + 11,
    HAL_CLOCKS_APP_SEC_P2P_CLK      = HAL_CLOCKS_APP_MCLK_EN0_BASE + 12,
    HAL_CLOCKS_APP_X2H_MEM_BUS_CLK  = HAL_CLOCKS_APP_MCLK_EN0_BASE + 13,
    HAL_CLOCKS_APP_M_MEM_BUS_CLK    = HAL_CLOCKS_APP_MCLK_EN0_BASE + 14,  // MEM_SUB总线时钟CG
    HAL_CLOCKS_APP_DSP2MEM_X2X_CLK  = HAL_CLOCKS_APP_MCLK_EN0_BASE + 15,  // MEM_SUB给DSP时钟的CG使能

    /* Clocks M_CLKEN1 region */
    HAL_CLOCKS_APP_MCLK_EN1_BASE,
    HAL_CLOCKS_APP_TIMER_CLK            = HAL_CLOCKS_APP_MCLK_EN1_BASE + 0,
    HAL_CLOCKS_APP_SPI1_M_CLK           = HAL_CLOCKS_APP_MCLK_EN1_BASE + 1,
    HAL_CLOCKS_APP_SPI0_MS_CLK          = HAL_CLOCKS_APP_MCLK_EN1_BASE + 2,
    HAL_CLOCKS_APP_MEM_HS_GPIO_CLK      = HAL_CLOCKS_APP_MCLK_EN1_BASE + 3,
    HAL_CLOCKS_APP_UART_H0_CLK          = HAL_CLOCKS_APP_MCLK_EN1_BASE + 4,
    HAL_CLOCKS_APP_UART_L0_CLK          = HAL_CLOCKS_APP_MCLK_EN1_BASE + 5,
    HAL_CLOCKS_APP_I2C0_CLK             = HAL_CLOCKS_APP_MCLK_EN1_BASE + 6,
    HAL_CLOCKS_APP_I2C1_CLK             = HAL_CLOCKS_APP_MCLK_EN1_BASE + 7,
    HAL_CLOCKS_APP_I2C2_CLK             = HAL_CLOCKS_APP_MCLK_EN1_BASE + 8,
    HAL_CLOCKS_APP_MEM_HS_GPIO_PERP_CLK = HAL_CLOCKS_APP_MCLK_EN1_BASE + 9,
    HAL_CLOCKS_APP_UART_H1_CLK          = HAL_CLOCKS_APP_MCLK_EN1_BASE + 11,
    HAL_CLOCKS_APP_I2C3_CLK             = HAL_CLOCKS_APP_MCLK_EN1_BASE + 12,
    HAL_CLOCKS_APP_XIP_BUS_CLK          = HAL_CLOCKS_APP_MCLK_EN1_BASE + 13,
    HAL_CLOCKS_APP_AUX_ADC_CLK          = HAL_CLOCKS_APP_MCLK_EN1_BASE + 14,

    /* Clocks M_CLKEN2 region */
    HAL_CLOCKS_APP_MCLK_EN2_BASE,
    HAL_CLOCKS_APP_M_DAP_CLK            = HAL_CLOCKS_APP_MCLK_EN2_BASE + 5,
    HAL_CLOCKS_APP_M_BUS_SLV_PRT_CLK    = HAL_CLOCKS_APP_MCLK_EN2_BASE + 6,
    HAL_CLOCKS_APP_M_CLK_CRC_CLK        = HAL_CLOCKS_APP_MCLK_EN2_BASE + 7,

    /* Clocks XIP region */
    HAL_CLOCKS_APP_XIP_BASE,
    HAL_CLOCKS_APP_QSPI_DISPLAY_CLK     = HAL_CLOCKS_APP_XIP_BASE + 1,

    HAL_CLOCKS_APP_BUTT
} hal_clocks_app_perips_clk_type_t;

/**
 * @brief  Config aux adc clock div in mcrg.
 * @param  div Config clk div.
 */
void hal_clocks_mcrg_aux_adc_div_set(uint8_t div);

/**
 * @brief  Get aux adc clock div in mcrg.
 * @return Divider number to be set.
 */
uint8_t hal_clocks_mcrg_aux_adc_div_get(void);

/**
 * @brief  Control PLL clock enable/disable.
 * @param  pll_src  Which pll module.
 * @param  clk_en  Clock open or close.
 */
void hal_clocks_pll_module_clken(clocks_pll_src_t pll_src, switch_type_t clk_en);

/**
 * @brief  Enable or diable the application peripheral clocks.
 * @param  app_clk The clock need to enable or disable.
 * @param  on TURN_ON indicates to enable clock, otherwise to disable.
 */
void hal_clocks_app_perips_config(hal_clocks_app_perips_clk_type_t app_clk, switch_type_t on);

/**
 * @brief  Get the application peripheral clocks enable status.
 * @return True: enable, false: disable.
 */
bool hal_clocks_app_perips_get_config(hal_clocks_app_perips_clk_type_t app_clk);

/**
  * @}
  */
#endif
