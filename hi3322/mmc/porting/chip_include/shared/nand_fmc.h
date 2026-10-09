/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __NAND_FMC__H__
#define __NAND_FMC__H__

/**
 *
 * @ingroup fmc
 * @brief fmc init.
 *
 * @par description:
 * fmc init.
 *
 * @param void
 *
 * @return NAND_STATUS_SUCCESS/NAND_STATUS_FAIL
 *
 */
int nand_init(void);

/**
 *
 * @ingroup fmc
 * @brief fmc erase.
 *
 * @par description:
 * fmc erase.
 *
 * @param start [in]: uint64_t
 * @param size [in]: size_t
 *
 * @return NAND_STATUS_SUCCESS/NAND_STATUS_FAIL
 *
 */
int fmc_nand_erase(uint64_t start, size_t size);

/**
 *
 * @ingroup fmc
 * @brief fmc write.
 *
 * @par description:
 * fmc write.
 *
 * @param memaddr [in]: void
 * @param start [in]: uint64_t
 * @param size [in]: size_t
 *
 * @return NAND_STATUS_SUCCESS/NAND_STATUS_FAIL
 *
 */
int fmc_nand_write(void *memaddr, uint64_t start, size_t size);

/**
 *
 * @ingroup fmc
 * @brief fmc read.
 *
 * @par description:
 * fmc rea.
 *
 * @param memaddr [out]: void
 * @param start [in]: uint64_t
 * @param size [in]: size_t
 *
 * @return NAND_STATUS_SUCCESS/NAND_STATUS_FAIL
 *
 */
int fmc_nand_read(void *memaddr, uint64_t start, size_t size);

int32_t nand_flash_init(void);
int32_t nandflash_filesystem_init(bool part_boot);
int32_t nandflash_format(void);
void nandflash_yaffs_sync(void);

#endif /* End of __NAND_FMC__H__ */
