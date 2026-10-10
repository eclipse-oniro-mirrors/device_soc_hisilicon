/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides mmc port \n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */

#ifndef MMC_PORTING_H
#define MMC_PORTING_H

#include <stdint.h>
#include <stdbool.h>
#include <platform_core.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define SDIO_HOST_REG_BASE_ADDESS 0x52060000
#define SDIO_HOST_REG_END_ADDRESS 0x52060FFF
#define SDIO_INTERRUPT_IRQ 76
#define EMMC_REG_BASE_ADDESS 0x52061000
#define EMMC_REG_END_ADDRESS 0x52061FFF
#define EMMC_INTERRUPT_IRQ 78

void sdio_clock_config(void);
void emmc_set_clock(uint32_t clock_value);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif