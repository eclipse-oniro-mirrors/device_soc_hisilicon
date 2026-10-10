/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "mtd_common.h"
#include "nand.h"
#include "nand_ops.h"
#include "nand_fmc.h"

static int32_t nand_write_skip_bad(struct mtd_info *mtd, uint64_t offset, uint32_t length,
    const char *buffer)
{
    uint32_t rw_size = length;

    if (mtd == NULL) {
        ERR_MSG("not init g_nand_mtd!!!\n");
        return -ENODEV;
    }

    return mtd->write(mtd, offset, length, &rw_size, buffer);
}

static int32_t nand_read_skip_bad(struct mtd_info *mtd, uint64_t offset, uint32_t length,
    char *buffer)
{
    uint32_t rw_size = length;

    if (mtd == NULL) {
        ERR_MSG("not init g_nand_mtd!!!\n");
        return -ENODEV;
    }
    return mtd->read(mtd, offset, length, &rw_size, buffer); /* nand_read */
}

int32_t nand_init(void)
{
    return nand_flash_init();
}

int32_t fmc_nand_erase(uint64_t start, uint32_t size)
{
    struct erase_info opts;
    if (g_nand_mtd == NULL) {
        ERR_MSG("not init g_nand_mtd!!!\n");
        return -ENODEV;
    }

    memset_s(&opts, sizeof(opts), 0, sizeof(opts));
    opts.addr = (uint64_t)start;
    opts.len = (uint64_t)size;

    return g_nand_mtd->erase(g_nand_mtd, &opts);
}

int32_t fmc_nand_write(void *memaddr, uint64_t start, uint32_t size)
{
    if (g_nand_mtd == NULL) {
        ERR_MSG("not init g_nand_mtd!!!\n");
        return -ENODEV;
    }

    return nand_write_skip_bad(g_nand_mtd, start, size, (const char *)memaddr);
}

int32_t fmc_nand_get_id(void **ids)
{
    if (g_nand_mtd == NULL) {
        ERR_MSG("not init g_nand_mtd!!!\n");
        return -ENODEV;
    }
    nand_get_id_ops(g_nand_mtd, ids);
    return 0;
}

int32_t fmc_nand_read(void *memaddr, uint64_t start, uint32_t size)
{
    if (g_nand_mtd == NULL) {
        ERR_MSG("not init g_nand_mtd!!!\n");
        return -ENODEV;
    }
    return nand_read_skip_bad(g_nand_mtd, start, size, (char *)memaddr);
}