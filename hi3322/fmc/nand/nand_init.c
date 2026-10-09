/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * morpheus adaptation (NAND_PORTING_PLAN.md v3 A4; YAFFS_PORTING_PLAN.md A4/A6):
 * identical to the fbb drivers/chips/3322/fmc/nand/nand_init.c except
 *   - the mount backend is selected at compile time via board_nand_backend:
 *     "yaffs"    -> nand_yaffs_bridge (fbb-aligned yaffs2 stack)
 *     "littlefs" -> nand_lfs_bridge
 *     Volume names, percent layout, block-0 reservation and the two-phase
 *     nandflash_filesystem_init(bool) API are identical for both;
 *   - nand_flash_init checks the probe result and runs the backend init;
 *   - nandflash_yaffs_sync is restored on the yaffs backend (fbb
 *     exception-path flush, backed by yaffs_sync + checkpoint).
 */

#include <errno.h>
#include "dpal.h"
#include "mtd_common.h"
#include "hifmc100.h"
#include "fmc_porting.h"
#include "nand.h"
#include "dpal_driverbase.h"

#if defined(BOARD_NAND_BACKEND_YAFFS)
#include "nand_yaffs_bridge.h"
#else
#include "nand_lfs_bridge.h"
#endif

#define YF_VOLUMES_NUM 3
#define YF_NAME_STRS "/boot", "/system", "/user"
uint32_t yf_volume_percent[YF_VOLUMES_NUM] = {5, 25, 70};

/* 预留block0 用于boot启动 */
#define BOOT_NAND_BLOCK_NUM 1
int last_part_start_sector = BOOT_NAND_BLOCK_NUM;

static const char * const g_volume_names[YF_VOLUMES_NUM] = { YF_NAME_STRS };

static struct dpal_resource g_hifmc100_resources[] = {
    {
        .start  = HIFMC100_REG_BASE,
        .end    = HIFMC100_REG_BASE + 0x140,
        .flags  = IORESOURCE_MEM,
    },
    {
        .start  = HIFMC100_MEM_BASE,
        .end    = HIFMC100_MEM_BASE + _2K,
        .flags  = IORESOURCE_MEM,
    },
};

struct dpal_platform_device g_device_hifmc100_nand = {
    .name       = "fmc100_nand",
    .id         = -1,
    .resource   = g_hifmc100_resources,
    .num_resources = sizeof(g_hifmc100_resources) / sizeof((g_hifmc100_resources)[0]),
};

static int32_t machine_init(struct dpal_platform_device* device)
{
    (void)dpal_platform_device_register(device);
    return 0;
}

static int formate_filesystem(bool part_boot)
{
    int ret;
    /* deterministic seed: the fbb last_part_start_sector global is mutated
     * by every call — seeding from it made the phase-2 recompute start at
     * the layout end (1024) and wrap to a huge unsigned /user block count
     * (4294966989 seen on board). The layout always starts after the
     * reserved block 0. */
    size_t part_start_sector = (size_t)BOOT_NAND_BLOCK_NUM;
    size_t part_count_sector = 0;
    uint32_t total_sector_num = 0;
    uint32_t blocks[YF_VOLUMES_NUM] = {0};

    total_sector_num = g_nand_mtd->size / g_nand_mtd->erasesize;

    /* fbb volume math: /boot = total*5%, /system = total*25%, /user =
     * remaining, all starting after the reserved block 0. The FULL layout
     * is computed on every call (phase-independent) because the littlefs
     * bridge registers all volumes in one LOS_DiskPartition call —
     * part_boot only selects which volumes get MOUNTED here:
     *   part_boot=true  -> /boot only (early thread stage)
     *   part_boot=false -> /system + /user                              */
    for (uint32_t index = 0; index < YF_VOLUMES_NUM; ++index) {
        part_count_sector = (total_sector_num * yf_volume_percent[index]) / 0x64;
        /* The number of blocks in the last area is calculated separately. */
        if (index == (YF_VOLUMES_NUM - 1)) {
            part_count_sector = total_sector_num - part_start_sector;
        }

        blocks[index] = (uint32_t)part_count_sector;
        part_start_sector += part_count_sector;
    }
    last_part_start_sector = (int)part_start_sector;

    /* Register the layout once (idempotent across both phases). */
#if defined(BOARD_NAND_BACKEND_YAFFS)
    ret = nand_yaffs_register_volumes((uint32_t)BOOT_NAND_BLOCK_NUM, blocks,
                                      g_volume_names, YF_VOLUMES_NUM);
#else
    ret = nand_lfs_register_volumes((uint32_t)BOOT_NAND_BLOCK_NUM, blocks,
                                    g_volume_names, YF_VOLUMES_NUM);
#endif
    if (ret != 0) {
        ERR_MSG("nand register_volumes fail ret = %d\n", ret);
        return -EIO;
    }

    for (uint32_t index = 0; index < YF_VOLUMES_NUM; ++index) {
        if (part_boot && index > 0) {
            break;
        }
        if (!part_boot && index == 0) {
            continue;
        }
        /* R4 diagnostic: mount-entry marker — round 3 reset silently inside
         * this call (no marker seen for the dying volume) */
        INFO_MSG("mounting filesystem: index = %u, folder_name = %s, blocks = %u\n",
                 index, g_volume_names[index], blocks[index]);
#if defined(BOARD_NAND_BACKEND_YAFFS)
        ret = nand_yaffs_mount(index, g_volume_names[index]);
#else
        ret = nand_lfs_mount(index, g_volume_names[index]);
#endif
        INFO_MSG("mount filesystem done: index = %u, folder_name = %s, ret = %d\n",
                 index, g_volume_names[index], ret);
        if (ret != 0) {
            ERR_MSG("mount %s fail ret = %d\n", g_volume_names[index], ret);
            return ret;
        }
    }
    return ret;
}

int32_t nand_flash_init(void)
{
    int32_t ret;

    fmc_port_init();
    machine_init(&g_device_hifmc100_nand);
    ret = nand_fmc100_init();
    if (ret != 0) {
        ERR_MSG("nand_fmc100_init fail ret = %d\n", ret);
        return ret;
    }
    if (g_nand_mtd == NULL) {
        ERR_MSG("nand probe did not register an mtd\n");
        return -ENODEV;
    }
#if defined(BOARD_NAND_BACKEND_YAFFS)
    /* dedicated yaffs pool + "yaffs" fs registration, before first mount */
    ret = nand_yaffs_backend_init();
    if (ret != 0) {
        ERR_MSG("nand_yaffs_backend_init fail ret = %d\n", ret);
        return ret;
    }
#endif
    INFO_MSG("nand probed: %s %uKB page %u block %u\n",
             g_nand_mtd->name ? g_nand_mtd->name : "?",
             (uint32_t)(g_nand_mtd->size / 1024U),
             g_nand_mtd->writesize, g_nand_mtd->erasesize);
    return 0;
}

int32_t nandflash_filesystem_init(bool part_boot)
{
    int32_t ret = 0;

    /* flash need to be initialized firstly before
     * boot partition mounted as first partition */
    if (part_boot) {
        ret = nand_flash_init();
        if (ret != 0) {
            return ret;
        }
    }
    return formate_filesystem(part_boot);
}

int32_t nandflash_format(void)
{
    struct erase_info opts;
    memset_s(&opts, sizeof(opts), 0, sizeof(opts));
    opts.addr = (uint64_t)g_nand_mtd->erasesize * BOOT_NAND_BLOCK_NUM;
    opts.len = (uint64_t)g_nand_mtd->size - g_nand_mtd->erasesize * BOOT_NAND_BLOCK_NUM;
    opts.mtd = g_nand_mtd;
    g_nand_mtd->erase(g_nand_mtd, &opts);
    return 0;
}
