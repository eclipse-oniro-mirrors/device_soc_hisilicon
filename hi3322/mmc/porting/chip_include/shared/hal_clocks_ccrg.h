/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description:   HAL APP CLOCK PRIVATE HEADER.
 */
#ifndef HAL_CLOCKS_CCRG_H
#define HAL_CLOCKS_CCRG_H

#include "stdint.h"

/**
 * @addtogroup connectivity_drivers_hal_clocks
 * @{
 */

/**
 * @brief  Modules in the common crg.
 */
typedef enum {
    HAL_CLOCKS_MODULE_MCU_CORE,
    HAL_CLOCKS_MODULE_MCU_PERP_LS,
    HAL_CLOCKS_MODULE_MCU_PERP_UART,
    HAL_CLOCKS_MODULE_MCU_PERP_SPI,
    HAL_CLOCKS_MODULE_COM_BUS,
    HAL_CLOCKS_MODULE_SCR_SEC,          // sec ip
    HAL_CLOCKS_MODULE_SDIOM,            // sdio host
    HAL_CLOCKS_MODULE_EMMC,             // emmc & fmc
    HAL_CLOCKS_MODULE_CAN,              // can
    HAL_CLOCKS_MODULE_USB_BUS,          // usb
    HAL_CLOCKS_MODULE_HIFI,             // hifi3z
    HAL_CLOCKS_MODULE_CODEC,            // codec
    HAL_CLOCKS_MODULE_XIP_SFC,          // norflash
    HAL_CLOCKS_MODULE_XIP_OPI0,         // psram
    HAL_CLOCKS_MODULE_DPU,              // dpu
    HAL_CLOCKS_MODULE_XIP_QSPI2,        // qspi lcd
    HAL_CLOCKS_MODULE_NPU_CORE,         // npu
    HAL_CLOCKS_MODULE_PAD_OUT0,
    HAL_CLOCKS_MODULE_PAD_OUT1,
    HAL_CLOCKS_MODULE_MAX,
} hal_clocks_ccrg_module_t;

/**
 * @brief  MCU group clock source in the common crg.
 * @note   The orders of the elements can't be modified as it is defined by the soc in the register list.
 */
typedef enum {
    HAL_CLOCKS_CCRG_CLK_SRC_XO_RC_32M = 0,
    HAL_CLOCKS_CCRG_CLK_SRC_XO_RC_DLL2 = 1,
    HAL_CLOCKS_CCRG_CLK_SRC_FNPLL = 2,
    HAL_CLOCKS_CCRG_CLK_SRC_FNPLL_VCO_DIV = 3,
    HAL_CLOCKS_CCRG_CLK_SRC_32K = 5,
    HAL_CLOCKS_CCRG_CLK_SRC_PAD_CLKIN = 7,

    HAL_CLOCKS_CCRG_CLK_SRC_MAX,
} hal_clocks_ccrg_clk_src_t;

/**
 * @brief  Channels definition of the crg.
 */
typedef enum {
    HAL_CLOCKS_CRG_CH_INVALID = 0,
    HAL_CLOCKS_CRG_CH0 = 1,             /*!< Channel 0, the value can be read in the ch_sel_sts register is 1. */
    HAL_CLOCKS_CRG_CH1 = 2,             /*!< Channel 1, the value can be read in the ch_sel_sts register is 2. */
} hal_clocks_crg_ch_t;

/**
 * @brief  Config MCU group clock source and clock div.
 * @param  module Which MCU group module.
 * @param  clk_source MCU normal clock source.
 * @param  div Config clk div.
 */
void hal_clocks_ccrg_switch(hal_clocks_ccrg_module_t module, hal_clocks_ccrg_clk_src_t clk_source, uint8_t div);

/**
 * @brief  Get Module perp ls clk src.
 */
uint8_t hal_clocks_ccrg_module_get_src(hal_clocks_ccrg_module_t module);

/**
 * @brief  Get Module perp ls clk div.
 */
uint8_t hal_clocks_ccrg_module_get_div(hal_clocks_ccrg_module_t module);

/**
 * @brief  Config mcu bus clock div.
 * @param  div Config clk div.
 */
void hal_clocks_ccrg_mcu_bus_div_set(uint8_t div);

/**
 * @brief  Get mcpu_bus clock divider.
 * @return uint8_t The clock divider.
 */
uint8_t hal_clocks_ccrg_mcu_bus_div_get(void);

/**
 * @}
 */
#endif
