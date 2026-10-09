/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __HIFMC_CMDS_H__
#define __HIFMC_CMDS_H__

#define GET_FEATURE       0x0F
#define SET_FEATURE       0x1F
#define WRITE_ENABLE      0x06
#define WRITE_DISABLE     0x04
#define PAGE_READ         0x13
#define READ_FROM_CACHE   0x03
#define READ_FROM_CACHE4  0x6B
#define PROGRAM_LOAD      0x02
#define PROGRAM_LOAD4     0x32
#define PROGRAM_EXECUTE   0x10
#define BLOCK_ERASE       0xD8
#define READ_ID           0x9F
#define RESET             0xFF

#endif /* __HIFMC_CMDS_H__ */