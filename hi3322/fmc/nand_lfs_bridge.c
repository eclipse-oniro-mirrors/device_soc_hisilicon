/* ---------------------------------------------------------------------------
 * SPI-NAND (Dosilicon DS35M1GA) -> littlefs bridge for morpheus hi3322.
 * (NAND_PORTING_PLAN.md v3 B1; modeled on sfc/nor_lfs_bridge.c)
 *
 * The liteos_m littlefs adapter (fs/littlefs/lfs_adapter.c) takes
 * readFunc/writeFunc/eraseFunc through struct PartitionCfg (los_fs.h) and
 * resolves per-partition start offsets from a device registered with
 * LOS_DiskPartition(): callbacks receive the ABSOLUTE offset (partition
 * start + in-partition offset) in *offset. NAND offsets map 1:1 onto
 * g_nand_mtd addresses — no window base addition like the NOR XIP path.
 *
 *   read:  mtd->read  (page read + on-die ECC, DS35M1GA internal 4-bit)
 *   prog:  mtd->write (page program; fbb mtd layer handles page loop)
 *   erase: mtd->erase (block erase)
 *
 * Bad blocks: factory-bad blocks are NEVER handed to littlefs — the bridge
 * checks mtd->block_isbad before prog/erase and returns -LFS_ERR_IO, which
 * drives littlefs's failure-driven relocation. littlefs only ever programs
 * within one erase block per call, so the fbb mtd layer's bad-block
 * skipping inside a multi-block range never shifts addresses under it.
 * --------------------------------------------------------------------------- */
#include <errno.h>
#include <string.h>

#include "los_fs.h"
#include "los_printf.h"
#include "lfs.h"

#include "linux/mtd/mtd.h"
#include "nand.h"
#include "nand_ops.h"
#include "nand_lfs_bridge.h"
#include "debug_print.h"
#include "watchdog.h"

/* ---------------------------------------------------------------------------
 * TEMP performance probe (2026-09-10): per-op timing + FMC register dump to
 * locate the ~20ms/page read and ~280ms/page program costs. Remove the
 * NAND_LFS_PERF_PROBE define once the timing story is understood.
 *
 * Disabled here (2026-09-22): the timing story was settled in the morpheus
 * v3 rounds, and the probe's extern g_tickIrqCount does not exist in THIS
 * tree's liteos_m (the littlefs backend was never rebuilt on morpheus
 * after the probe landed, so the link break was latent — first exposed by
 * the C5 regression here). Re-enable only for FMC timing bring-up work.
 * ------------------------------------------------------------------------- */
#define NAND_LFS_PERF_PROBE 0
#if NAND_LFS_PERF_PROBE
#include "los_tick.h"
#include "hifmc_regs.h"
#include "hifmc100.h"
#include "soc.h" /* TIMER_CLOCK */

static uint32_t perf_cycles_to_us(uint32_t delta)
{
    return delta / (uint32_t)(TIMER_CLOCK / 1000000U);
}

static uint32_t perf_fmc_reg(uint32_t off)
{
    return *(volatile uint32_t *)(uintptr_t)(HIFMC100_REG_BASE + off);
}
#endif

#define NAND_LFS_MAX_VOL 3

#define NAND_LFS_LOOKAHEAD 128U  /* bytes: 128*8 = 1024 blocks tracked */
#define NAND_LFS_BLOCK_CYCLES 32U

struct nand_lfs_state {
    int registered;
    int mounted[NAND_LFS_MAX_VOL];
    uint32_t vol_start_block;
    uint32_t vol_blocks[NAND_LFS_MAX_VOL];
};

static struct nand_lfs_state g_nand_lfs;

/* Bad-block bitmap: filled once by the census (factory scan), updated on
 * runtime erase/program failures. With the map valid the littlefs
 * callbacks answer bad-block queries in O(1) instead of a ~20ms OOB page
 * read per query, and bypass the fbb mtd wrappers whose get_len_incl_bad
 * adds extra full page reads per call (every 2KB op costs ~3 fixed
 * ~20ms FMC command units through mtd->read; 1 unit direct). */
#define NAND_LFS_BITMAP_BLOCKS 1024U  /* DS35M1GA: 1024 blocks */
#define NAND_LFS_BITMAP_BYTES  ((NAND_LFS_BITMAP_BLOCKS + 7U) / 8U)
static uint8_t g_isbad_bitmap[NAND_LFS_BITMAP_BYTES];
static int g_isbad_map_valid;
static uint32_t g_blk_shift;

#define NAND_LFS_ISBAD_TEST(blk) \
    (g_isbad_bitmap[(blk) >> 3] & (uint8_t)(1U << ((blk) & 7U)))
#define NAND_LFS_ISBAD_SET(blk) \
    (g_isbad_bitmap[(blk) >> 3] = \
         (uint8_t)(g_isbad_bitmap[(blk) >> 3] | (1U << ((blk) & 7U))))

static uint32_t nand_lfs_blk_of(uint32_t addr)
{
    return addr >> g_blk_shift;
}

static int nand_lfs_blk_isbad(uint32_t blk)
{
    if (!g_isbad_map_valid) {
        /* map not built (chip larger than the bitmap) — fall back to the
         * driver-level isbad page read */
        return (g_nand_mtd->block_isbad(g_nand_mtd,
                (loff_t)(blk << g_blk_shift)) == 1) ? 1 : 0;
    }
    return (NAND_LFS_ISBAD_TEST(blk) != 0) ? 1 : 0;
}

/* ---- low-level callbacks (PartitionCfg signature, los_fs.h) ---- */

static int nand_lfs_read(int partition, UINT32 *offset, void *buf, UINT32 size)
{
    struct nand_info *nand;
    struct mtd_oob_ops ops;
    static uint8_t oob_sink[64]; /* littlefs never uses OOB; sink the read */
    uint32_t addr = (uint32_t)*offset;
    int ret;

    (void)partition;
    if ((g_nand_mtd == NULL) || (g_nand_mtd->priv == NULL)) {
        return -LFS_ERR_IO;
    }
    if ((size == 0) || ((addr % size) != 0)) {
        PRINT("nand_lfs: read 0x%x+%u not page aligned\n", addr, size);
        return -LFS_ERR_IO;
    }
    if (nand_lfs_blk_isbad(nand_lfs_blk_of(addr)) != 0) {
        PRINT("nand_lfs: read into bad block @0x%x refused\n", addr);
        return -LFS_ERR_IO;
    }

    /* Direct nand_do_read_ops: one FMC page-read unit per call. The fbb
     * mtd->read wrapper would additionally run get_len_incl_bad (another
     * full isbad page read) for the same result — the bitmap above already
     * guarantees this block is good. */
    nand = (struct nand_info *)g_nand_mtd->priv;
    (void)memset_s(&ops, sizeof(ops), 0, sizeof(ops));
    ops.datbuf = (const char *)buf;
    ops.len = size;
    ops.oobbuf = (const char *)oob_sink;
    ops.ooblen = 0;

    ret = nand_do_read_ops(nand, addr, &ops);
    if (ret != 0) {
        PRINT("nand_lfs: read 0x%x+%u failed ret=%d\n", addr, size, ret);
        return -LFS_ERR_IO;
    }
    return 0;
}

static int nand_lfs_write(int partition, UINT32 *offset, const void *buf, UINT32 size)
{
    struct mtd_oob_ops ops;
    uint32_t addr = (uint32_t)*offset;
    uint32_t writesize;
    uint32_t blk;
    int ret;

    (void)partition;
    if (g_nand_mtd == NULL) {
        return -LFS_ERR_IO;
    }
    writesize = g_nand_mtd->writesize;
    if (((addr % writesize) != 0) || ((size % writesize) != 0)) {
        PRINT("nand_lfs: write 0x%x+%u not page(%u) aligned\n", addr, size, writesize);
        return -LFS_ERR_IO;
    }
    blk = nand_lfs_blk_of(addr);
    /* factory-bad block: refuse, littlefs relocates (see file header) */
    if (nand_lfs_blk_isbad(blk) != 0) {
        PRINT("nand_lfs: write into bad block @0x%x refused\n", addr);
        return -LFS_ERR_IO;
    }

    /* Direct nand_do_write_ops: program load + execute + status in one
     * unit; ooblen==0 makes the ops layer program the BBM area as 0xFF. */
    (void)memset_s(&ops, sizeof(ops), 0, sizeof(ops));
    ops.datbuf = (const char *)buf;
    ops.len = size;
    ops.oobbuf = NULL;
    ops.ooblen = 0;

    ret = nand_do_write_ops(g_nand_mtd, addr, &ops);
    if (ret != 0) {
        /* program failure: retire the block (bitmap + physical BBM) so
         * littlefs relocates and never comes back */
        NAND_LFS_ISBAD_SET(blk);
        (void)g_nand_mtd->block_markbad(g_nand_mtd, (loff_t)addr);
        PRINT("nand_lfs: write 0x%x+%u failed ret=%d, block %u retired\n",
              addr, size, ret, blk);
        return -LFS_ERR_IO;
    }
    return 0;
}

static int nand_lfs_erase(int partition, UINT32 offset, UINT32 size)
{
    struct erase_info opts;
    uint32_t erasesize;
    uint32_t blk;
    int ret;

    (void)partition;
    if (g_nand_mtd == NULL) {
        return -LFS_ERR_IO;
    }
    erasesize = g_nand_mtd->erasesize;
    if (((offset % erasesize) != 0) || (size != erasesize)) {
        PRINT("nand_lfs: erase 0x%x+%u not one erase block (%u)\n",
              offset, size, erasesize);
        return -LFS_ERR_IO;
    }
    blk = nand_lfs_blk_of(offset);
    if (nand_lfs_blk_isbad(blk) != 0) {
        PRINT("nand_lfs: erase bad block @0x%x refused\n", offset);
        return -LFS_ERR_IO;
    }

    (void)memset_s(&opts, sizeof(opts), 0, sizeof(opts));
    opts.addr = (uint64_t)offset;
    opts.len = (uint64_t)size;
    opts.mtd = g_nand_mtd;

    /* Direct nand_do_erase_ops: one erase unit; the mtd->erase wrapper
     * would pre-scan the (single) block with a full isbad page read. */
    ret = nand_do_erase_ops(g_nand_mtd, offset);
    if (ret != 0) {
        /* erase failure: retire the block (bitmap + physical BBM) */
        NAND_LFS_ISBAD_SET(blk);
        (void)g_nand_mtd->block_markbad(g_nand_mtd, (loff_t)offset);
        PRINT("nand_lfs: erase 0x%x failed ret=%d, block %u retired\n",
              offset, ret, blk);
        return -LFS_ERR_IO;
    }
    return 0;
}

/* ---- registration + mount ---- */

int nand_lfs_register_volumes(uint32_t start_block, const uint32_t *blocks,
                              const char *const *names, uint32_t count)
{
    static int lengths[NAND_LFS_MAX_VOL];
    static int addrs[NAND_LFS_MAX_VOL];
    uint32_t erasesize;
    uint32_t i;
    uint32_t bad_blocks = 0;
    uint32_t total_blocks;
    int ret;
#if NAND_LFS_PERF_PROBE
    UINT32 perf_hw_ticks0;
#endif

    if ((g_nand_mtd == NULL) || (blocks == NULL) || (names == NULL) ||
        (count == 0) || (count > NAND_LFS_MAX_VOL)) {
        return -EINVAL;
    }
    if (g_nand_lfs.registered) {
        return 0;
    }
    erasesize = g_nand_mtd->erasesize;
    total_blocks = (uint32_t)(g_nand_mtd->size / erasesize);

    g_blk_shift = 0;
    while (((uint32_t)1U << g_blk_shift) < erasesize) {
        g_blk_shift++;
    }

    /* diagnostics: factory bad-block census over the whole chip (~20s at
     * the current per-page cost — the kick keeps the 10s watchdog happy). */
    PRINT("nand_lfs: bad-block census start (%u blocks)\n", total_blocks);
#if NAND_LFS_PERF_PROBE
    {
        static uint8_t perf_page[2048];
        size_t perf_retlen = 0;
        uint32_t t0, t1, t2, t3, t4;
        extern volatile UINT32 g_tickIrqCount;

        /* absolute calibration: a nominal 1s LOS_UDelay busy wait bracketed
         * by UART timestamps, plus the hardware tick counter (independent
         * of SysCycleGet) over the same window — the two together give the
         * true delay-scale factor and the real tick period. */
        perf_hw_ticks0 = g_tickIrqCount;
        PRINT("nand_lfs: perf calib start (nominal 1s LOS_UDelay)\n");
        LOS_UDelay(1000000U);
        PRINT("nand_lfs: perf calib end, hw_ticks_delta=%u\n",
              g_tickIrqCount - perf_hw_ticks0);

        PRINT("nand_lfs: FMC_CFG=0x%08x GLOBAL=0x%08x TIMING_SPI=0x%08x TIMEOUT_WR=0x%08x\n",
              perf_fmc_reg(FMC_CFG), perf_fmc_reg(GLOBAL_CFG),
              perf_fmc_reg(TIMING_SPI_CFG), perf_fmc_reg(FMC_TIMEOUT_WR));

        t0 = LOS_SysCycleGet();
        (void)g_nand_mtd->block_isbad(g_nand_mtd, 0);
        t1 = LOS_SysCycleGet();
        (void)g_nand_mtd->block_isbad(g_nand_mtd, erasesize);
        t2 = LOS_SysCycleGet();
        (void)g_nand_mtd->read(g_nand_mtd, 0, 2048, &perf_retlen, perf_page);
        t3 = LOS_SysCycleGet();
        (void)g_nand_mtd->read(g_nand_mtd, 0, 2048, &perf_retlen, perf_page);
        t4 = LOS_SysCycleGet();
        PRINT("nand_lfs: perf isbad#0=%uus isbad#1=%uus page_read#0=%uus page_read#1=%uus\n",
              perf_cycles_to_us(t1 - t0), perf_cycles_to_us(t2 - t1),
              perf_cycles_to_us(t3 - t2), perf_cycles_to_us(t4 - t3));

        /* TIMING_SPI_CFG A/B scan (reset value 0x6f gives ~20ms per 2KB
         * cache read ≈ 1MHz SPI — a divider field is suspected). For each
         * candidate: write it, re-read the /boot superblock page
         * (offset 0x20000, real littlefs data) and time the read; the
         * candidate is only reported as usable when the content is
         * bit-identical to the baseline read at the reset value. */
        {
            static uint8_t base_page[2048];
            static uint8_t cand_page[2048];
            static const uint32_t cand[] = {
                0x60, 0x61, 0x62, 0x63, 0x64, 0x66, 0x68, 0x6c,
                0x5f, 0x4f, 0x3f, 0x2f, 0x1f, 0x0f, 0x07, 0x03,
            };
            uint32_t c;
            size_t rl = 0;

            (void)g_nand_mtd->read(g_nand_mtd, erasesize, 2048, &rl, base_page);
            (void)memcpy_s(cand_page, sizeof(cand_page), base_page, sizeof(cand_page));

            for (c = 0; c < sizeof(cand) / sizeof(cand[0]); c++) {
                uint32_t tA, tB, ok;
                uint32_t k;

                *(volatile uint32_t *)(uintptr_t)(HIFMC100_REG_BASE + TIMING_SPI_CFG) = cand[c];
                tA = LOS_SysCycleGet();
                (void)g_nand_mtd->read(g_nand_mtd, erasesize, 2048, &rl, cand_page);
                tB = LOS_SysCycleGet();

                ok = 1;
                for (k = 0; k < sizeof(cand_page); k++) {
                    if (cand_page[k] != base_page[k]) {
                        ok = 0;
                        break;
                    }
                }
                PRINT("nand_lfs: TIMING 0x%02x -> %uus, data %s\n",
                      cand[c], perf_cycles_to_us(tB - tA),
                      (ok != 0) ? "MATCH" : "MISMATCH");
            }
            /* restore the reset value for the rest of the boot */
            *(volatile uint32_t *)(uintptr_t)(HIFMC100_REG_BASE + TIMING_SPI_CFG) = 0x6f;
        }
    }
#endif
    for (i = 0; i < total_blocks; i++) {
        if (g_nand_mtd->block_isbad(g_nand_mtd, (loff_t)(i * erasesize)) == 1) {
            bad_blocks++;
            if (i < NAND_LFS_BITMAP_BLOCKS) {
                NAND_LFS_ISBAD_SET(i);
            }
        }
        if ((i % 64U) == 0U) {
            uapi_watchdog_kick();
        }
    }
    uapi_watchdog_kick();
    if (total_blocks <= NAND_LFS_BITMAP_BLOCKS) {
        g_isbad_map_valid = 1;
    }
    PRINT("nand_lfs: chip %uKB, %u blocks @%uKB, bad=%u (bitmap %s)\n",
          (uint32_t)(g_nand_mtd->size / 1024U), total_blocks,
          erasesize / 1024U, bad_blocks,
          (g_isbad_map_valid != 0) ? "on" : "OFF");
#if NAND_LFS_PERF_PROBE
    {
        extern volatile UINT32 g_tickIrqCount;
        PRINT("nand_lfs: perf census wall check: hw_ticks_delta=%u over %u isbads\n",
              g_tickIrqCount - perf_hw_ticks0, total_blocks);
    }
#endif

    for (i = 0; i < count; i++) {
        uint32_t blk = start_block;
        uint32_t k;

        for (k = 0; k < i; k++) {
            blk += blocks[k];
        }
        if ((blk + blocks[i]) > total_blocks) {
            PRINT("nand_lfs: volume %s out of range (block %u+%u)\n",
                  names[i], blk, blocks[i]);
            return -EINVAL;
        }
        g_nand_lfs.vol_blocks[i] = blocks[i];
        lengths[i] = (int)(blocks[i] * erasesize);
        addrs[i] = (int)(blk * erasesize);
        PRINT("nand_lfs: vol%u %s @block%u (%uKB)\n",
              i, names[i], blk, (uint32_t)(blocks[i] * erasesize / 1024U));
    }
    g_nand_lfs.vol_start_block = start_block;

    ret = LOS_DiskPartition("nand0", "littlefs", lengths, addrs, (int)count);
    PRINT("nand_lfs: LOS_DiskPartition ret=%d\n", ret);
    if (ret != 0) {
        return ret;
    }
    g_nand_lfs.registered = 1;
    return 0;
}

int nand_lfs_mount(uint32_t part_no, const char *name)
{
    struct PartitionCfg cfg;
    uint32_t erasesize;
    int ret;

    if ((g_nand_mtd == NULL) || (name == NULL) || (!g_nand_lfs.registered) ||
        (part_no >= NAND_LFS_MAX_VOL)) {
        return -EINVAL;
    }
    if (g_nand_lfs.mounted[part_no]) {
        return 0;
    }
    erasesize = g_nand_mtd->erasesize;

    (void)memset_s(&cfg, sizeof(cfg), 0, sizeof(cfg));
    cfg.readFunc = nand_lfs_read;
    cfg.writeFunc = nand_lfs_write;
    cfg.eraseFunc = nand_lfs_erase;
    cfg.readSize = g_nand_mtd->writesize;      /* 2048B page */
    cfg.writeSize = g_nand_mtd->writesize;     /* 2048B page */
    cfg.blockSize = erasesize;                 /* 128KB erase block */
    cfg.blockCount = g_nand_lfs.vol_blocks[part_no];
    cfg.cacheSize = g_nand_mtd->writesize;
    cfg.partNo = (int)part_no;
    cfg.lookaheadSize = NAND_LFS_LOOKAHEAD;
    cfg.blockCycles = NAND_LFS_BLOCK_CYCLES;

    ret = mount("nand0", name, "littlefs", 0, &cfg);
    PRINT("nand_lfs: mount %s ret=%d errno=%d\n", name, ret, errno);
    if (ret != 0) {
        return ret;
    }
    g_nand_lfs.mounted[part_no] = 1;
    return 0;
}
