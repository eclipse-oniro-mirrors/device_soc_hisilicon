/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __SPI_NAND_IDS_H__
#define __SPI_NAND_IDS_H__

#include "nand_common.h"
#include "spi_common.h"

#define MAX_SPI_NAND_ID_LEN     3
typedef void (*oob_format_fn)(int32_t flag, char *oob_data, int32_t oob_len);
struct nand_flash_info {
    char *name;
    uint8_t id[MAX_SPI_NAND_ID_LEN];
    uint32_t id_len;
    uint64_t chipsize;
    uint32_t pagesize;
    uint32_t blocksize;
    uint32_t oobsize;
    uint32_t ecctype;
    uint32_t badblock_pos;
    struct spi_drv *driver;
    oob_format_fn oob_format;
};

struct nand_dev_info *nand_get_dev_info_by_id(struct nand_info *nand);

#endif /* End of __SPI_NAND_IDS_H__ */