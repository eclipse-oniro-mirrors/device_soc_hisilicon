/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "hifmc_common.h"
#define BIT_16  16
#define BIT_8   8
#define BIT_4   4
#define BIT_2   2

static struct match_type_size type_2_page_size[] = {
    { HIFMC_PAGE_SIZE_2K,  NAND_PAGE_2K },
    { HIFMC_PAGE_SIZE_4K,  NAND_PAGE_4K },
    { HIFMC_PAGE_SIZE_8K,  NAND_PAGE_8K },
    { HIFMC_PAGE_SIZE_16K, NAND_PAGE_16K },
};

static struct match_type_size type_2_block_size[] = {
    { HIFMC_BLOCK_SIZE_64,  NAND_BLOCK_SIZE_64 },
    { HIFMC_BLOCK_SIZE_128, NAND_BLOCK_SIZE_128 },
    { HIFMC_BLOCK_SIZE_256, NAND_BLOCK_SIZE_256 },
    { HIFMC_BLOCK_SIZE_512, NAND_BLOCK_SIZE_512 },
};

int32_t nandpage_type2size(int32_t type)
{
    for (uint32_t i = 0; i < array_size(type_2_page_size); i++) {
        if (type_2_page_size[i].type == type) {
            return type_2_page_size[i].size;
        }
    }
    return NAND_PAGE_2K;
}

int32_t nandpage_size2type(int32_t size)
{
    for (uint32_t i = 0; i < array_size(type_2_page_size); i++) {
        if (type_2_page_size[i].size == size) {
            return type_2_page_size[i].type;
        }
    }
    return HIFMC_PAGE_SIZE_2K;
}

int32_t nandblock_type2size(int32_t type)
{
    for (uint32_t i = 0; i < array_size(type_2_block_size); i++) {
        if (type_2_block_size[i].type == type) {
            return type_2_block_size[i].size;
        }
    }
    return NAND_BLOCK_SIZE_64;
}

int32_t nandblock_size2type(int32_t size)
{
    for (uint32_t i = 0; i < array_size(type_2_block_size); i++) {
        if (type_2_block_size[i].size == size) {
            return type_2_block_size[i].type;
        }
    }
    return HIFMC_BLOCK_SIZE_64;
}
