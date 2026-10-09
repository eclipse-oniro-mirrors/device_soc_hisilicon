/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: SYSTEM CLOCKS CCRG DRIVER
 *
 * Create: 2020-7-28
 */

#ifndef CLOCKS_CCRG_H
#define CLOCKS_CCRG_H

#include <stdint.h>
#include "pmu_cmu.h"
#include "hal_clocks_ccrg.h"

typedef enum {
    CLOCKS_SRC_XO = 0,
    CLOCKS_SRC_RC = CLOCKS_SRC_XO,
    CLOCKS_SRC_DLL = 1,
    CLOCKS_SRC_PLL = 2,
    CLOCKS_SRC_PLL_VCO_DIV = 3,
    CLOCKS_SRC_32K = 5,
    CLOCKS_SRC_PAD_CLKIN = 7,
    CLOCKS_SRC_MAX,

    CLOCKS_SRC_TCXO = CLOCKS_SRC_XO,
    CLOCKS_SRC_TCXO_2X = CLOCKS_SRC_DLL,
} clocks_src_t;

typedef struct {
    clocks_src_t clk_src;
    uint8_t clk_div;
} clocks_clk_cfg_t;

typedef enum {
    CLOCKS_PLL_SRC_DLL,
    CLOCKS_PLL_SRC_FNPLL,
    CLOCKS_PLL_SRC_FNPLL_VCO,
    CLOCKS_PLL_SRC_MAX,
} clocks_pll_src_t;

typedef hal_clocks_ccrg_module_t clocks_ccrg_module_t;

void clocks_ccrg_set_frequency(clocks_ccrg_module_t module, clocks_src_t clk_src, uint8_t clk_div);

uint32_t clocks_ccrg_get_frequency(clocks_ccrg_module_t module);

uint32_t clocks_ccrg_get_clk_src_frequency(clocks_src_t src);

void clocks_ccrg_init(void);

void clocks_suspend(void);

void clocks_resume(void);

#endif
