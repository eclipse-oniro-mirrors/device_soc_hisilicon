/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __NAND_H__
#define __NAND_H__

#include "linux/mtd/mtd.h"

extern struct mtd_info *g_nand_mtd;

void nand_register(struct mtd_info *mtd);

#endif /* End of __NAND_H__ */
