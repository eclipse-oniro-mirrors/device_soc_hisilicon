/* ---------------------------------------------------------------------------
 * SPI-NAND (DS35M1GA) -> yaffs2 bridge for morpheus hi3322
 * (YAFFS_PORTING_PLAN.md A4). fbb-aligned two-phase mount API, mirrors
 * nand_lfs_bridge.h. Enabled when board_nand_backend == "yaffs".
 * --------------------------------------------------------------------------- */
#ifndef NAND_YAFFS_BRIDGE_H
#define NAND_YAFFS_BRIDGE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Install the dedicated yaffs memory pool and register the "yaffs" fs type.
 * Call once from nand_flash_init() after a successful NAND probe. */
int nand_yaffs_backend_init(void);

/* Register the volume layout once (fbb 5%/25%/70% computed by nand_init.c);
 * idempotent across the two mount phases. */
int nand_yaffs_register_volumes(uint32_t start_block, const uint32_t *blocks,
                                const char *const *names, uint32_t count);

/* Mount one registered volume by index (part_boot phase selects which). */
int nand_yaffs_mount(uint32_t part_no, const char *name);

/* Full sync (cache flush + checkpoint) of every mounted volume — the fbb
 * exception-path flush (nandflash_yaffs_sync in fbb nand_init.c). */
void nandflash_yaffs_sync(void);

/* C4 on-board image-compat test support
 * (YAFFS2_BINARY_LIB_PORTING_PLAN.md C4-②): raw accessors for the fixture
 * test in fmc/yaffs/nand_yaffs_c4.c. */
/* forward tags matching the fbb contract headers (mtd_partition.h defines
 * "typedef struct mtd_node {...} mtd_partition", linux/mtd/mtd.h the info) */
struct mtd_node;
typedef struct mtd_node mtd_partition;
struct mtd_info;
const mtd_partition *nand_yaffs_get_part(uint32_t part_no);
struct mtd_info *nand_yaffs_get_mtd(void);
/* VFS-level umount of one mounted volume (clears the bridge mounted flag);
 * the C4 test writes raw image pages into the volume between this and the
 * next nand_yaffs_mount(). */
int nand_yaffs_umount(uint32_t part_no);
/* umount at VFS level, then re-mount the volume (same partition data). With
 * reformat != 0 the volume is wiped in between (yaffs_start + yaffs_format
 * + yaffs_end), leaving a clean empty volume mounted. */
int nand_yaffs_remount(uint32_t part_no, int reformat);
/* C4 fixture test: writes the embedded fbb mkyaffs2image sample volume into
 * /boot via the mtd free stream, mounts it with the CLOSED archive, reads
 * back and compares all sample files. Returns 0 on pass. */
int nand_yaffs_c4_test(void);

#ifdef __cplusplus
}
#endif

#endif /* NAND_YAFFS_BRIDGE_H */
