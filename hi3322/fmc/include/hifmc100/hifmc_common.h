/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __HIFMC_COMMON_H__
#define __HIFMC_COMMON_H__

#include "common_def.h"

#define NAND_PAGE_2K               2048
#define NAND_PAGE_4K               4096
#define NAND_PAGE_8K               8192
#define NAND_PAGE_16K              16384
#define NAND_BLOCK_SIZE_64         64
#define NAND_BLOCK_SIZE_128        128
#define NAND_BLOCK_SIZE_256        256
#define NAND_BLOCK_SIZE_512        512

#define HIFMC_PAGE_SIZE_2K          0x0
#define HIFMC_PAGE_SIZE_4K          0x1
#define HIFMC_PAGE_SIZE_8K          0x2
#define HIFMC_PAGE_SIZE_16K         0x3
#define HIFMC_BLOCK_SIZE_64         0x0
#define HIFMC_BLOCK_SIZE_128        0x1
#define HIFMC_BLOCK_SIZE_256        0x2
#define HIFMC_BLOCK_SIZE_512        0x3

typedef struct match_type_size {
    int32_t type;
    int32_t size;
} match_type_size;

int32_t nandpage_type2size(int32_t type);
int32_t nandpage_size2type(int32_t size);

int32_t nandblock_type2size(int32_t type);
int32_t nandblock_size2type(int32_t size);

#endif