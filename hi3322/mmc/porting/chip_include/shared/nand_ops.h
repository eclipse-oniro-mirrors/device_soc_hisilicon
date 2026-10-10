/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __NAND_OPS_H__
#define __NAND_OPS_H__

#include "nand_common.h"
#include "dpal.h"
#include "linux/mtd/mtd.h"

#define page_addr(addr, page_shift)  (uint32_t)((addr) >> ((page_shift)))

int32_t nand_block_markbad(struct nand_info *nand, uint32_t addr);
int32_t nand_block_isbad(struct nand_info *nand, uint32_t addr);
int32_t nand_do_erase_ops(struct mtd_info *mtd, uint32_t addr);
int32_t nand_do_write_ops(struct mtd_info *mtd, uint32_t to, struct mtd_oob_ops *ops);
int32_t nand_do_read_ops(struct nand_info *nand, uint32_t from, struct mtd_oob_ops *ops);
int32_t nand_get_id_ops(struct mtd_info *mtd, void **ids);

#endif /* End of __NAND_OPS_H__ */

