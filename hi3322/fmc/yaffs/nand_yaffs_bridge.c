/*
 * SPI-NAND (Dosilicon DS35M1GA) -> yaffs2 bridge for hi3322, CLOSED-ARCHIVE
 * variant (YAFFS2_BINARY_LIB_PORTING_PLAN.md P4).
 *
 * Difference to the morpheus source-route bridge (which this file replaces):
 * the fbb prebuilt libyaffs2.a ALREADY CONTAINS the whole mtd glue chain —
 * yaffs_osglue (pool/mutex/time), yaffs_nandcfg + yaffs_nand_drv
 * (yaffs_start/yaffs_end, nand_* page hooks) and mtd_nandcfg (nf_* page
 * glue with the fbb BBM handling). Nothing of that is re-implemented here;
 * this bridge only performs the runtime contract the closed archive
 * expects from its environment (reverse-engineered from the fbb
 * vfs_yaffs_bind / yaffs_nand_install_drv disassembly):
 *
 *   1. yaffs_mtd = &g_nand_mtd   — exported archive global; in fbb the VFS
 *      bind path assigns it from partition->mtd_info. Nothing inside the
 *      archive ever writes it, so we do, right after the NAND probe.
 *   2. LOS_YaffsSetMemPool(pool, size) — exported archive entry that runs
 *      LOS_MemInit and installs g_yaffsMemPool; yaffsfs_malloc/free go
 *      through that dedicated pool (LOSCFG_FS_YAFFS_INDEPENDENT_MEMORY_POOL
 *      is baked in).
 *   3. YaffsVfsRegister() — the OH-VFS adapter (kernel/fs/yaffs2), whose
 *      mount op calls the closed yaffs_start(mountpoint, mtd_partition*).
 *
 * Watchdog note: the closed nf_* glue cannot kick (binary), so the kicks
 * live in the ported driver (fmc/common/nand.c block_isbad patch + the
 * per-page/per-block kicks the fbb driver already has).
 */
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "los_fs.h"          /* mount() */
#include "los_printf.h"      /* PRINT */
#include "los_memory.h"      /* LOS_MemInfoGet (pool water-level print) */
#include "linux/mtd/mtd.h"   /* struct mtd_info */
#include "nand.h"            /* g_nand_mtd */
#include "nand_yaffs_bridge.h"
#include "debug_print.h"
#include "watchdog.h"        /* uapi_watchdog_kick (erase-first path) */

/*
 * fbb contract headers (third_party/yaffs2/include): yaffs_start/yaffs_end
 * declarations + mtd_partition layout. Do NOT include yaffs_adapt_os.h —
 * it pulls the fbb los_fs.h which clashes with the OH-VFS one. Do NOT
 * include yaffsfs.h either: its yaffs_list.h redefines list_head/list_add
 * and clashes with the fbb linux/list.h pulled into this fmc compile unit
 * (the same reason the morpheus bridge avoided the yaffs header tree) —
 * the few direct-API entries used here are declared manually below.
 */
#include "yaffs_nandcfg.h"
extern int yaffs_sync(const char *path);
extern int yaffs_format(const char *path, int ignore1, int ignore2, int ignore3);

#ifndef NAND_YAFFS_POOL_BYTES
#define NAND_YAFFS_POOL_BYTES (288U * 1024U)
#endif

#define NAND_YAFFS_MAX_VOL 3

/*
 * Closed-archive contract symbols (fbb diting-community, 349 T symbols):
 * see prebuilt/ORIGIN.md and the plan section 3.3 for the disassembly
 * evidence behind each declaration.
 */
extern struct mtd_info *yaffs_mtd;                       /* B, archive */
extern void LOS_YaffsSetMemPool(void *pool, unsigned int size); /* T */
extern void *g_yaffsMemPool;                             /* B, archive */
extern unsigned int yaffs_trace_mask;                    /* D, archive */
extern void YaffsVfsRegister(void);                      /* kernel/fs/yaffs2 */

/*
 * Layout sentinels — the closed yaffs_nand_install_drv() reads the mtd
 * device geometry at these hardcoded offsets (size@16 as u64, erasesize@24,
 * writesize@28, oobsize@32, read_oob@48) and the mtd_partition fields at
 * start_block@0 / end_block@4. If this compile of the fbb headers ever
 * disagrees, mounting would read garbage geometry — fail the build instead.
 */
_Static_assert(offsetof(struct mtd_info, size) == 16, "mtd_info.size drifted");
_Static_assert(offsetof(struct mtd_info, erasesize) == 24, "mtd_info.erasesize drifted");
_Static_assert(offsetof(struct mtd_info, writesize) == 28, "mtd_info.writesize drifted");
_Static_assert(offsetof(struct mtd_info, oobsize) == 32, "mtd_info.oobsize drifted");
_Static_assert(offsetof(struct mtd_info, read_oob) == 48, "mtd_info.read_oob drifted");
_Static_assert(offsetof(mtd_partition, start_block) == 0, "mtd_partition.start_block drifted");
_Static_assert(offsetof(mtd_partition, end_block) == 4, "mtd_partition.end_block drifted");
_Static_assert(offsetof(mtd_partition, mtd_info) == 24, "mtd_partition.mtd_info drifted");

static unsigned char g_yaffs_pool_mem[NAND_YAFFS_POOL_BYTES]
    __attribute__((aligned(8)));

struct nand_yaffs_vol {
    const char *name;
    mtd_partition part;
    int mounted;
};

static struct nand_yaffs_vol g_vols[NAND_YAFFS_MAX_VOL];
static int g_vol_registered;

/* ---- backend init + volume registration + mount ---------------------------- */

static void nand_yaffs_pool_print(const char *tag)
{
    LOS_MEM_POOL_STATUS st = {0};

    if ((g_yaffsMemPool != NULL) &&
        (LOS_MemInfoGet(g_yaffsMemPool, &st) == LOS_OK)) {
        PRINT("nand_yaffs: %s pool used=%uKB total=%uKB\n", tag,
              (unsigned)(st.totalUsedSize / 1024U),
              (unsigned)((st.totalUsedSize + st.totalFreeSize) / 1024U));
    }
}

int nand_yaffs_backend_init(void)
{
    if (g_nand_mtd == NULL) {
        PRINT("nand_yaffs: no probed mtd\n");
        return -ENODEV;
    }

    /* archive runtime contract: hand over the mtd device (the closed glue
     * reads geometry and page ops through this single global) */
    yaffs_mtd = g_nand_mtd;

    /* dedicated pool: LOS_YaffsSetMemPool runs LOS_MemInit and installs
     * g_yaffsMemPool on success; yaffsfs_malloc must never see NULL */
    LOS_YaffsSetMemPool(g_yaffs_pool_mem, NAND_YAFFS_POOL_BYTES);
    if (g_yaffsMemPool == NULL) {
        PRINT("nand_yaffs: pool init fail (%u bytes)\n", NAND_YAFFS_POOL_BYTES);
        return -ENOMEM;
    }

    YaffsVfsRegister();

#if defined(NAND_YAFFS_TRACE_MASK)
#if (NAND_YAFFS_TRACE_MASK != 0)
    yaffs_trace_mask = NAND_YAFFS_TRACE_MASK;
    PRINT("nand_yaffs: trace mask 0x%08x\n", NAND_YAFFS_TRACE_MASK);
#endif
#endif

#if defined(BOARD_NAND_YAFFS_ERASE_FIRST)
    /* one-shot clean slate for bring-up: wipes every volume including the
     * TC-persist marker on every boot — MUST stay off in normal operation.
     *
     * R6' diagnostic: round 6 reset 31.6s into this loop with not even the
     * first 32-block progress line printed — a single block operation
     * (block_isbad or erase) hung >30s somewhere in blocks 1..31. Print
     * EVERY block up front so the stuck block is named exactly; widen the
     * boot WDT to 60s to survive the full ~1-2min wipe when not stuck. */
    {
        uint32_t blk;
        uint32_t total = (uint32_t)(g_nand_mtd->size / g_nand_mtd->erasesize);

        PRINT("nand_yaffs: ERASE-FIRST wiping %u blocks\n", total - 1U);
        for (blk = 1U; blk < total; blk++) {
            struct erase_info opts;
            int ret;

            PRINT("nand_yaffs: erase-first blk %u isbad...\n", blk);
            uapi_watchdog_kick();
            if (g_nand_mtd->block_isbad(g_nand_mtd,
                    (loff_t)(blk * g_nand_mtd->erasesize)) == 1) {
                PRINT("nand_yaffs: erase-first blk %u BAD, skipped\n", blk);
                continue;
            }
            PRINT("nand_yaffs: erase-first blk %u erase...\n", blk);
            (void)memset_s(&opts, sizeof(opts), 0, sizeof(opts));
            opts.addr = (uint64_t)blk * g_nand_mtd->erasesize;
            opts.len = g_nand_mtd->erasesize;
            opts.mtd = g_nand_mtd;
            ret = g_nand_mtd->erase(g_nand_mtd, &opts);
            if (ret != 0) {
                PRINT("nand_yaffs: erase-first blk %u fail ret=%d\n", blk, ret);
            }
        }
        PRINT("nand_yaffs: erase-first done\n");
    }
#endif

    PRINT("nand_yaffs: backend ready, pool %uKB (caches=20 baked in)\n",
          NAND_YAFFS_POOL_BYTES / 1024U);
    return 0;
}

int nand_yaffs_register_volumes(uint32_t start_block, const uint32_t *blocks,
                                const char *const *names, uint32_t count)
{
    uint32_t i;
    uint32_t total_blocks;

    if ((g_nand_mtd == NULL) || (blocks == NULL) || (names == NULL) ||
        (count == 0U) || (count > NAND_YAFFS_MAX_VOL)) {
        return -EINVAL;
    }
    if (g_vol_registered != 0) {
        return 0; /* idempotent across the two mount phases */
    }

    total_blocks = (uint32_t)(g_nand_mtd->size / g_nand_mtd->erasesize);
    for (i = 0; i < count; i++) {
        uint32_t first = start_block;
        uint32_t k;

        for (k = 0; k < i; k++) {
            first += blocks[k];
        }
        if ((first + blocks[i]) > total_blocks) {
            PRINT("nand_yaffs: volume %s out of range (%u+%u)\n",
                  names[i], first, blocks[i]);
            return -EINVAL;
        }

        /*
         * Assemble the fbb mtd_partition the closed yaffs_start() expects.
         * install_drv reads start_block/end_block (end_block==0 would mean
         * "whole device") and takes the mtd device from the global yaffs_mtd;
         * mtd_info is filled for contract fidelity with the fbb bind path.
         */
        (void)memset_s(&g_vols[i].part, sizeof(mtd_partition), 0,
                       sizeof(mtd_partition));
        g_vols[i].name = names[i];
        g_vols[i].part.start_block = first;
        g_vols[i].part.end_block = first + blocks[i] - 1U;
        g_vols[i].part.patitionnum = i;
        g_vols[i].part.mountpoint_name = (CHAR *)names[i];
        g_vols[i].part.mtd_info = g_nand_mtd;
        g_vols[i].mounted = 0;
        PRINT("nand_yaffs: vol%u %s @block%u..%u (%uKB)\n", i, names[i],
              first, first + blocks[i] - 1U,
              blocks[i] * g_nand_mtd->erasesize / 1024U);
    }
    g_vol_registered = 1;
    return 0;
}

int nand_yaffs_mount(uint32_t part_no, const char *name)
{
    int ret;

    if ((part_no >= NAND_YAFFS_MAX_VOL) || (!g_vol_registered) ||
        (g_vols[part_no].name == NULL)) {
        return -EINVAL;
    }
    if ((name != NULL) && (strcmp(g_vols[part_no].name, name) != 0)) {
        return -EINVAL;
    }
    if (g_vols[part_no].mounted != 0) {
        return 0;
    }

    /* source "nand0" is nominal for yaffs: the OH-VFS adapter takes the
     * layout from the mtd_partition mount data, not a device registry */
    ret = mount("nand0", g_vols[part_no].name, "yaffs", 0, &g_vols[part_no].part);
    if (ret != 0) {
        PRINT("nand_yaffs: mount %s fail ret=%d errno=%d\n",
              g_vols[part_no].name, ret, errno);
    } else {
        g_vols[part_no].mounted = 1;
        /* pool water-level after each mount — board measurement decides the
         * final NAND_YAFFS_POOL_BYTES (plan section 6.1) */
        nand_yaffs_pool_print(g_vols[part_no].name);
    }
    return ret;
}

void nandflash_yaffs_sync(void)
{
    uint32_t i;

    for (i = 0; i < NAND_YAFFS_MAX_VOL; i++) {
        if ((g_vols[i].mounted != 0) && (g_vols[i].name != NULL)) {
            int ret = yaffs_sync(g_vols[i].name);
            if (ret != 0) {
                PRINT("nand_yaffs: sync %s fail ret=%d\n", g_vols[i].name, ret);
            }
        }
    }
}

/* ---- C4 test support (see nand_yaffs_bridge.h) ---------------------------- */

const mtd_partition *nand_yaffs_get_part(uint32_t part_no)
{
    if (part_no >= NAND_YAFFS_MAX_VOL) {
        return NULL;
    }
    return &g_vols[part_no].part;
}

struct mtd_info *nand_yaffs_get_mtd(void)
{
    return g_nand_mtd;
}

int nand_yaffs_umount(uint32_t part_no)
{
    if ((part_no >= NAND_YAFFS_MAX_VOL) || (g_vols[part_no].name == NULL)) {
        return -EINVAL;
    }
    if (g_vols[part_no].mounted == 0) {
        return 0;
    }
    /* VFS umount runs the adapter's yaffs_unmount + yaffs_end pair */
    if (umount(g_vols[part_no].name) != 0) {
        PRINT("nand_yaffs: umount %s fail errno=%d\n",
              g_vols[part_no].name, errno);
        return -1;
    }
    g_vols[part_no].mounted = 0;
    return 0;
}

int nand_yaffs_remount(uint32_t part_no, int reformat)
{
    if ((part_no >= NAND_YAFFS_MAX_VOL) || (g_vols[part_no].name == NULL)) {
        return -EINVAL;
    }

    if (nand_yaffs_umount(part_no) != 0) {
        return -1;
    }

    if (reformat != 0) {
        /*
         * Wipe cycle: rebuild the device (closed yaffs_start — success is
         * NONZERO, see the adapter contract note), format the volume while
         * it is started-but-not-mounted, tear it down again and let the
         * plain mount below bring up a clean empty volume.
         */
        if (yaffs_start(g_vols[part_no].name, &g_vols[part_no].part) == 0) {
            PRINT("nand_yaffs: remount start fail\n");
            return -1;
        }
        if (yaffs_format(g_vols[part_no].name, 0, 0, 0) != 0) {
            PRINT("nand_yaffs: remount format fail\n");
            yaffs_end(g_vols[part_no].name);
            return -1;
        }
        yaffs_end(g_vols[part_no].name);
    }

    return nand_yaffs_mount(part_no, g_vols[part_no].name);
}
