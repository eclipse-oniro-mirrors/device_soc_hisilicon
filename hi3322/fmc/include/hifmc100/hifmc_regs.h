/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __HIFMC_REGS_H__
#define __HIFMC_REGS_H__

#define FMC_DMA_ADDR 0x20000000
#define FMC_OOB_ADDR 0x20220000

/* offset */
#define FMC_CFG           0x0
#define GLOBAL_CFG        0x4
#define TIMING_SPI_CFG    0x8
#define PND_PWIDTH_CFG    0xC
#define PND_OPIDLE_CFG    0x10
#define FMC_INT           0x18
#define FMC_INT_EN        0x1C
#define FMC_INT_CLR       0x20
#define FMC_CMD           0x24
#define FMC_ADDRH         0x28
#define FMC_ADDRL         0x2C
#define FMC_OP_CFG        0x30
#define SPI_OP_ADDR       0x34
#define FMC_DATA_NUM      0x38
#define FMC_OP            0x3C
#define FMC_DMA_LEN       0x40
#define FMC_DMA_AHB_CTRL  0x48
#define FMC_DMA_SADDR_D0  0x4C
#define FMC_DMA_SADDR_D1  0x50
#define FMC_DMA_SADDR_D2  0x54
#define FMC_DMA_SADDR_D3  0x58
#define FMC_DMA_SADDR_OOB 0x5C
#define FMC_OP_CTRL       0x68
#define FMC_TIMEOUT_WR    0x6C

/* device reg */
#define DEVICE_BLOCK_LOCK 0xA0
#define DEVICE_OTP        0xB0
#define DEVICE_STATUS     0xC0

#endif /* __HIFMC_REGS_H__ */