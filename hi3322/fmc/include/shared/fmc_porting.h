/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Provides dma port \n
 *
 * History: \n
 * 2024-6-26， Create file. \n
 */

#ifndef FMC_PORTING_H
#define FMC_PORTING_H

#include "errcode.h"

/**
 * @brief  Config the pinmux of the fmc.
 */
void fmc_port_init(void);
void fmc_clock_div_set(uint8_t div);
void fmc_io_clock_div_set(uint8_t div);
errcode_t uapi_fmc_resume(uintptr_t arg);
errcode_t uapi_fmc_suspend(uintptr_t arg);

#endif /* FMC_PORTING_H */