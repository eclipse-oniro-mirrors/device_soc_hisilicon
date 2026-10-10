/*
 * ---------------------------------------------------------------------------
 * SPI-NOR (GD25LQ128E) -> littlefs bridge for morpheus hi3322.
 *
 * The SPINOR branch mounts littlefs over its SFC driver; morpheus
 * liteos_m ships the same littlefs adapter (fs/littlefs/lfs_adapter.c)
 * which takes readFunc/writeFunc/eraseFunc through struct PartitionCfg
 * (los_fs.h) and resolves per-partition start offsets from a device
 * registered with LOS_DiskPartition().
 *
 *   read:  XIP window memcpy (flash 0x26000000 is memory mapped)
 *   prog:  SFC page program (256 B, GD25LQ128E page size)
 *   erase: SFC sector erase (4 KB)
 *
 * Write/erase follow the sequence exactly (sfc_porting.c):
 * lock (mutex, IRQ-context forbidden) -> write_enable -> command ->
 * poll WIP -> unlock; the SFC bus-read is disabled by the hal around
 * command mode. See NOR_PORTING_PLAN.md 4.1 for the XIP trade-off and
 * the ITCM fallback plan.
 *
 * Volume layout (3 volumes in the free area after the app partition,
 * before nv; 4 KB sector aligned).
 * /boot /system /user names:
 *   /nboot   0x26940000 + 0x100000   (1M)
 *   /nsystem 0x26A40000 + 0x200000   (2M)
 *   /nuser   0x26C40000 + 0x3B0000   (3.6M, ends at nv 0x26FFA000)
 * littlefs formats a volume on first mount automatically.
 * ---------------------------------------------------------------------------
 */
#include <errno.h>
#include <string.h>

#include "los_fs.h"
#include "los_interrupt.h"
#include "los_printf.h"

#include "errcode.h"
#include "lfs.h"
#include "sfc_v2.h"
#include "sfc_porting.h"
#include "sfc_config_info.h"
#include "common_def.h"
#include "debug_print.h"

/* XIP window (memory_config_flash.h / morpheus pack_fwpkg.sh) */
#define NOR_FLASH_BASE     0x26000000U
#define NOR_FLASH_LENGTH   0x1000000U
#define NOR_PAGE_SIZE      256U        /* GD25LQ128E page program size */
#define NOR_SECTOR_SIZE    4096U       /* GD25LQ128E sector erase size */
#define NOR_PROG_CHUNK     64U         /* SFC single-command limit (must match nor_flash_itcm.c NOR_PROG_CHUNK) */

struct nor_vol_cfg {
    const char *name;
    uint32_t addr;      /* absolute flash address */
    uint32_t size;      /* partition size */
};

static const struct nor_vol_cfg g_nor_vols[] = {
    { "/nboot",   0x26940000U, 0x100000U },
    { "/nsystem", 0x26A40000U, 0x200000U },
    { "/nuser",   0x26C40000U, 0x3B0000U },
};
#define NOR_VOL_NUM (sizeof(g_nor_vols) / sizeof(g_nor_vols[0]))

static int nor_fs_inited;

/*
 * ---- low-level callbacks (PartitionCfg signature, los_fs.h) ----
 *
 * Address semantics: the littlefs adapter adds the registered partition
 * start (addrArray) to block addresses and passes the sum in *offset.
 * We register RELATIVE offsets (flash offset from 0x26000000), because
 * the SFC command address register takes flash-relative addresses
 * (check_opt_param compares against chip_size 16MB; the boot caller
 * subtracts mapping_addr the same way).
 *
 * Hybrid write/erase architecture (NOR_PORTING_PLAN.md 4.1):
 *   - WREN and status reads go through the XIP path
 *     (hal_sfc_write_enable / sfc_port_read_flash_reg) — board-verified
 *     working; the ITCM-issued WREN variant does not latch WEL (SR1
 *     reads 0x00 on BOTH paths after an ITCM WREN, so WREN itself is
 *     issued from XIP before entering the locked ITCM section);
 *   - the data-phase commands (page program / sector erase + WIP poll)
 *     run from the ITCM-resident writer (hal/nor_flash_itcm.c) with
 *     interrupts locked, since the XIP window cannot fetch while the
 *     SFC executes a command;
 *   - a 256-byte littlefs program is split into four 64-byte chunks,
 *     each with its own WREN (WEL auto-clears after every program);
 *   - read stays on the XIP window (plain memcpy).
 * The ITCM functions are called via function pointers (auipc+jalr far
 * call — the ITCM address is out of JAL range from XIP).
 */

extern int nor_itcm_sector_erase(uint32_t rel_off);
extern int nor_itcm_page_prog64(uint32_t rel_off, const uint8_t *buf);

static int (*const nor_erase_itcm)(uint32_t) = nor_itcm_sector_erase;
static int (*const nor_prog_itcm)(uint32_t, const uint8_t *) = nor_itcm_page_prog64;

static uint32_t g_nor_erase_count;
static uint32_t g_nor_prog_count;

/*
 * WREN through the XIP path + WEL check. Returns 0 when WEL is
 * latched (SR1 bit1), -2 otherwise.
 */
static int nor_wren_xip(void)
{
    uint8_t status = 0;
    errcode_t ret = hal_sfc_write_enable(0, SPI_CMD_WREN);
    if (ret != ERRCODE_SUCC) {
        PRINT("nor_lfs: WREN failed 0x%x\n", ret);
        return -2;
    }
    ret = sfc_port_read_flash_reg(0, &status, SPI_CMD_RDSR);
    if (ret != ERRCODE_SUCC) {
        PRINT("nor_lfs: RDSR failed 0x%x\n", ret);
        return -2;
    }
    if ((status & 0x02u) == 0u) {
        PRINT("nor_lfs: WEL not latched SR1=0x%02x\n", status);
        return -2;
    }
    return 0;
}

static int nor_lfs_read(int partition, UINT32 *offset, void *buf, UINT32 size)
{
    uint32_t addr = NOR_FLASH_BASE + (uint32_t)*offset;

    (void)partition;
    (void)memcpy_s(buf, size, (const void *)(uintptr_t)addr, size);
    return 0;
}

static int nor_lfs_write(int partition, UINT32 *offset, const void *buf, UINT32 size)
{
    uint32_t addr = (uint32_t)*offset;
    uint32_t done;

    (void)partition;
    if ((addr % NOR_PAGE_SIZE) != 0) {
        PRINT("nor_lfs: write addr 0x%x not page aligned\n", addr);
        return -LFS_ERR_IO;
    }
    if (((addr % NOR_PROG_CHUNK) != 0) || ((size % NOR_PROG_CHUNK) != 0)) {
        PRINT("nor_lfs: write addr 0x%x size %u not 64B aligned\n", addr, size);
        return -LFS_ERR_IO;
    }

    /*
     * littlefs may pass several prog_size pages in one call (e.g. a 768B
     * commit): loop the 64-byte SFC chunks, each preceded by its own WREN
     * through the XIP path (WEL auto-clears after every program).
     */
    for (done = 0; done < size; done += NOR_PROG_CHUNK) {
        int r = nor_wren_xip();
        if (r != 0) {
            return -LFS_ERR_IO;
        }

        uint32_t lock = LOS_IntLock();
        r = nor_prog_itcm(addr + done, (const uint8_t *)buf + done);
        LOS_IntRestore(lock);
        if (r != 0) {
            PRINT("nor_lfs: prog 0x%x+%u failed %d\n", addr, done, r);
            return -LFS_ERR_IO;
        }
        g_nor_prog_count++;
    }

    /*
     * TEMP bring-up diagnosis: verify the programmed bytes through the XIP
     * window — separates "command accepted but data/addr wrong" from a
     * clean program.
     */
    {
        const uint8_t *xip = (const uint8_t *)(uintptr_t)(NOR_FLASH_BASE + addr);
        uint32_t k;
        for (k = 0; k < size; ++k) {
            if (xip[k] != ((const uint8_t *)buf)[k]) {
                PRINT("nor_lfs: VERIFY prog 0x%x[%u] wrote 0x%02x read 0x%02x\n",
                      addr, k, ((const uint8_t *)buf)[k], xip[k]);
                return -LFS_ERR_IO;
            }
        }
    }
    return 0;
}

static int nor_lfs_erase(int partition, UINT32 offset, UINT32 size)
{
    uint32_t lock;
    int r;

    (void)partition;
    if ((offset % NOR_SECTOR_SIZE) != 0) {
        PRINT("nor_lfs: erase addr 0x%x not sector aligned\n", offset);
        return -LFS_ERR_IO;
    }
    if (size != NOR_SECTOR_SIZE) {
        PRINT("nor_lfs: erase size %u != sector\n", size);
        return -LFS_ERR_IO;
    }

    /*
     * WREN through the XIP path (with WEL check) — outside the locked
     * section; WEL auto-clears when the erase completes.
     */
    r = nor_wren_xip();
    if (r != 0) {
        return -LFS_ERR_IO;
    }

    lock = LOS_IntLock();
    r = nor_erase_itcm(offset);
    LOS_IntRestore(lock);

    if (r != 0) {
        PRINT("nor_lfs: erase 0x%x failed %d\n", offset, r);
        return -LFS_ERR_IO;
    }
    g_nor_erase_count++;
    if ((g_nor_erase_count % 32u) == 1u) {
        PRINT("nor_lfs: erase #%u @0x%x ok\n", g_nor_erase_count, offset);
    }
    return 0;
}

/*
 * ---- mount sequence ----
 */

int nor_lfs_mount_all(void)
{
    static int lengths[NOR_VOL_NUM];
    static int addrs[NOR_VOL_NUM];
    uint32_t i;
    int ret;

    if (nor_fs_inited) {
        return 0;
    }

    sfc_flash_config_t sfc_cfg = {
        .read_type = FAST_READ_QUAD_IO,
        .write_type = QUAD_INPUT_PAGE_PROGRAM,
        .mapping_addr = NOR_FLASH_BASE,
        .mapping_size = NOR_FLASH_LENGTH,
    };
    ret = uapi_sfc_init(SFC_ID_0, &sfc_cfg);
    if ((ret != ERRCODE_SUCC) && (ret != ERRCODE_SFC_ALREADY_INIT)) {
        PRINT("nor_lfs: sfc init failed 0x%x\n", ret);
        return ret;
    }

    {
        uint8_t status = 0;
        (void)sfc_port_read_flash_reg(0, &status, SPI_CMD_RDSR);
        PRINT("nor_lfs: SR1=0x%02x (WIP=%u WEL=%u BP=%u)\n", status,
              status & 0x1u, (status >> 1) & 0x1u, (status >> 2) & 0x7u);
    }

    for (i = 0; i < NOR_VOL_NUM; ++i) {
        lengths[i] = (int)g_nor_vols[i].size;
        /*
         * relative flash offset: read() adds NOR_FLASH_BASE for the XIP
         * window, write()/erase() pass it to uapi_sfc as-is
         */
        addrs[i] = (int)(g_nor_vols[i].addr - NOR_FLASH_BASE);
    }

    ret = LOS_DiskPartition("spinor0", "littlefs", lengths, addrs, (int)NOR_VOL_NUM);
    PRINT("nor_lfs: LOS_DiskPartition ret=%d\n", ret);
    if (ret != 0) {
        return ret;
    }

    for (i = 0; i < NOR_VOL_NUM; ++i) {
        struct PartitionCfg cfg;

        (void)memset_s(&cfg, sizeof(cfg), 0, sizeof(cfg));
        cfg.readFunc = nor_lfs_read;
        cfg.writeFunc = nor_lfs_write;
        cfg.eraseFunc = nor_lfs_erase;
        cfg.readSize = NOR_PAGE_SIZE;
        cfg.writeSize = NOR_PAGE_SIZE;
        cfg.blockSize = NOR_SECTOR_SIZE;
        cfg.blockCount = (int)(g_nor_vols[i].size / NOR_SECTOR_SIZE);
        cfg.cacheSize = NOR_SECTOR_SIZE;
        cfg.partNo = (int)i;
        cfg.lookaheadSize = 128;
        cfg.blockCycles = 32;

        ret = mount("spinor0", g_nor_vols[i].name, "littlefs", 0, &cfg);
        PRINT("nor_lfs: mount %s ret=%d errno=%d (erase#%u prog#%u)\n",
              g_nor_vols[i].name, ret, errno,
              g_nor_erase_count, g_nor_prog_count);
        /*
         * TEMP bring-up: raw dump of the first 16 bytes at the volume base
         * (XIP window) — shows whether littlefs actually wrote a
         * superblock structure there.
         */
        {
            const uint8_t *xip = (const uint8_t *)(uintptr_t)g_nor_vols[i].addr;
            PRINT("nor_lfs: raw @0x%08x: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
                  g_nor_vols[i].addr, xip[0], xip[1], xip[2], xip[3], xip[4],
                  xip[5], xip[6], xip[7], xip[8], xip[9], xip[10], xip[11],
                  xip[12], xip[13], xip[14], xip[15]);
        }
        if (ret != 0) {
            return ret;
        }
        PRINT("nor_lfs: mounted %s (0x%08x, %uKB)\n",
              g_nor_vols[i].name, g_nor_vols[i].addr, g_nor_vols[i].size / 1024U);
    }

    nor_fs_inited = 1;
    return 0;
}
