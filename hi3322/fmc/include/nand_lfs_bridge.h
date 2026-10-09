/*
 * Copyright (c) 2026 HiSilicon (Shanghai) Technologies Co., Ltd.
 * littlefs-over-SPI-NAND bridge API (NAND_PORTING_PLAN.md v3 B1).
 */
#ifndef NAND_LFS_BRIDGE_H
#define NAND_LFS_BRIDGE_H

#include <stdint.h>

/*
 * Register the full volume layout with the liteos_m disk layer (one
 * LOS_DiskPartition call). start_block is the first usable NAND block
 * (fbb BOOT_NAND_BLOCK_NUM=1: block 0 reserved). blocks[i] is the size of
 * volume i in erase blocks. Idempotent: the layout is registered once.
 *
 * The (start_block, blocks[]) values come from nand_init.c, which keeps the
 * fbb volume math (percent table) authoritative.
 */
int nand_lfs_register_volumes(uint32_t start_block, const uint32_t *blocks,
                              const char *const *names, uint32_t count);

/* Mount volume part_no at mount point name (littlefs, auto-format on first
 * mount). Read/prog/erase go through g_nand_mtd; factory bad blocks are
 * refused to littlefs (erase/prog return -LFS_ERR_IO) so littlefs relocates
 * around them. */
int nand_lfs_mount(uint32_t part_no, const char *name);

#endif /* NAND_LFS_BRIDGE_H */
