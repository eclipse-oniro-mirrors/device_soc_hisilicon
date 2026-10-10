/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description:  CLOCKS CORE PRIVATE HEADER.
 *
 * Create: 202-04-13
 */
#ifndef CLOCKS_SWITCH_H
#define CLOCKS_SWITCH_H

#include "common_def.h"
#include "errcode.h"
#include "clocks_config.h"

errcode_t clocks_set_mcu_freq(system_mcu_freq_t clk_level);

errcode_t clocks_set_mcu_freq_ram(system_mcu_freq_t clk_level);

errcode_t clocks_set_dsp_freq(pm_hifi_mode_t clk_level);

errcode_t clocks_set_emmc_freq(system_emmc_freq_t clk_level);

errcode_t clocks_set_sdio_freq(system_sdio_freq_t clk_level);

uint8_t clocks_get_dsp_freq_level(void);

errcode_t clocks_set_psram_freq(clocks_src_t clk_src, uint8_t clk_div);

void set_clock_0p8_vol_vmin(uint8_t vmin);

uint8_t get_clock_0p8_vol_vmin(void);

uint8_t clocks_get_last_dsp_freq_level(void);
#endif