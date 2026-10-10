/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description:  CLOCKS CONFIG PRIVATE HEADER.
 */
#ifndef CLOCKS_CONFIG_DRV_H
#define CLOCKS_CONFIG_DRV_H

#include "hal_clocks_app.h"
#include "clocks_ccrg.h"
#ifndef BUILD_APPLICATION_SSB
#include "pm_clocks.h"
#endif

typedef enum {
    CLOCKS_ALL_TCXO_64M,
    CLOCKS_ALL_PLL,
    CLOCKS_CONFIG_MAX,
} system_clocks_config_t;

typedef enum {
    CLOCKS_EMMC_FREQ_32M,      // sdio:32M
    CLOCKS_EMMC_FREQ_64M,      // sdio:64M
    CLOCKS_EMMC_FREQ_H,        // 100M
    CLOCKS_EMMC_FREQ_MAX,
} system_emmc_freq_t;

typedef enum {
    CLOCKS_SDIO_FREQ_32M,      // sdio:32M
    CLOCKS_SDIO_FREQ_50M,      // sdio:50M
    CLOCKS_SDIO_FREQ_MAX,
} system_sdio_freq_t;

typedef struct {
    clocks_clk_cfg_t hifi_clk;
    clocks_clk_cfg_t codec_clk;
} system_hifi_clk_cfg_t;

typedef enum {
    CLOCKS_M32,     //  mcu:32M
    CLOCKS_M64,     //  mcu:64M
    CLOCKS_M_H0,    //  mcu:133M
    CLOCKS_M_H1,    //  mcu:266M
    CLOCKS_MCU_FREQ_MAX,
} system_mcu_freq_t;

typedef struct {
    clocks_clk_cfg_t mcu_clk;
    clocks_clk_cfg_t psram_clk;
    clocks_clk_cfg_t flash_clk;
    clocks_clk_cfg_t emmc_clk;
    clocks_clk_cfg_t sec_clk;
    clocks_clk_cfg_t vau_clk;
    clocks_clk_cfg_t qspi_display_clk;
} system_mcu_core_clk_cfg_t;

#ifndef BUILD_APPLICATION_SSB
clocks_clk_cfg_t const *clocks_system_all_clocks_get(system_clocks_config_t clk_level);

system_mcu_core_clk_cfg_t clocks_system_mcu_freq_get(system_mcu_freq_t clk_level);

uint8_t clocks_system_mcu_need_vol_get(system_mcu_freq_t clk_level);

clocks_clk_cfg_t clocks_system_emmc_freq_get(system_emmc_freq_t clk_level);

clocks_clk_cfg_t clocks_system_sdio_freq_get(system_sdio_freq_t clk_level);

system_hifi_clk_cfg_t clocks_system_hifi_freq_get(pm_hifi_mode_t clk_level);

uint8_t clocks_system_hifi_need_vol_get(pm_hifi_mode_t clk_level);

#endif
#endif