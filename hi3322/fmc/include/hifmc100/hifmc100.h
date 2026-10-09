/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __HIFMC100_H__
#define __HIFMC100_H__

#include "host_common.h"
#include "spi_common.h"

#define HIFMC100_REG_BASE  0x52062000
#define HIFMC100_MEM_BASE  0x5c000000


#define HIFMC100_NAND_CS_NUM        0x1

#define HIFMC_TIME_OUT   10000
#define HIFMC_DMA        0
#define HIFMC_REG        1
#define HIFMC_ID_LEN     2


#define STATUS_P_FAIL_MASK                  (1 << 3)
#define STATUS_E_FAIL_MASK                  (1 << 2)
#define STATUS_WEL_MASK                     (1 << 1)
#define STATUS_OIP_MASK                     (1 << 0)

struct hifmc_host {
    struct nand_host *host;
    struct spi spi[1];
    void (*set_system_clock)(unsigned clk, int clk_en);
};

int32_t nand_fmc100_init(void);
int32_t fmc_nand_write_dma_single(void *memaddr, uint64_t start, uint32_t size);
int32_t fmc_nand_read_dma_single(void *memaddr, uint64_t start, uint32_t size);
int32_t fmc_nand_write_reg_single(void *memaddr, uint64_t start, uint32_t size);
int32_t fmc_nand_write_reg(void *memaddr, uint64_t start, uint32_t size);
int32_t fmc_nand_read_reg_single(void *memaddr, uint64_t start, uint32_t size);
int32_t fmc_nand_read_reg(void *memaddr, uint64_t start, uint32_t size);
int spinand_general_qe_enable(struct spi *spi);
int spinand_qe_not_enable(struct spi *spi);
int spinand_buf_enable(struct spi *spi);

#endif /* End of __HIFMC100_H__ */