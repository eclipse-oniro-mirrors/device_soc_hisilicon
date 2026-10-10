/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __NAND_COMMON_H__
#define __NAND_COMMON_H__

#include "dpal_typedef.h"
#include "linux/mtd/mtd.h"
#include "mtd_common.h"

#define BBP_LAST_PAGE           0x01
#define BBP_FIRST_PAGE          0x02

#define GOOD_BLOCK              0
#define BAD_BLOCK               1

/* NAND features macros */
#define NAND_RANDOMIZER             0x01 /* nand chip need randomizer */
#define NAND_SYNCHRONOUS            0x02 /* nand chip support syncrono */
#define NAND_ASYNCHRONOUS           0x04 /* nand chip support asynchro */
#define NAND_SYNCHRONOUS_BOOT       0x08 /* nand boot from synchronous */
#define NAND_HW_AUTO                0x10 /* controller support hardware */
#define NAND_CONFIG_DONE            0x20 /* current controller config */

/* Max configuration */
#define NAND_BB_SIZE            2
#define NAND_MAX_ID_LEN         8

/* Status bits */
#define NAND_STATUS_SUCCESS     0x00
#define NAND_STATUS_FAIL        0x01
#define NAND_STATUS_FAIL_N1     0x02
#define NAND_STATUS_TRUE_READY  0x20
#define NAND_STATUS_READY       0x40
#define NAND_STATUS_WP          0x80

/* Standard NAND flash commands */
#define NAND_CMD_READ0          0x00
#define NAND_CMD_READ1          0x30
#define NAND_CMD_PROG0          0x80
#define NAND_CMD_PROG1          0x10
#define NAND_CMD_ERASE1         0x60
#define NAND_CMD_ERASE2         0xd0
#define NAND_CMD_STATUS         0x70
#define NAND_CMD_READID         0x90
#define NAND_CMD_RESET          0xff

/* Addr Cyecles configuration */
#define NAND_PROG_READ_ADDR_CYCLE   (5)
#define NAND_ERASE_ADDR_CYCLE       (3)

#define READ_ID_ADDR_NUM            1

/* nand_info->op_state */
#define NAND_READING    0x01
#define NAND_PROGING    0x02
#define NAND_ERASING    0x04

/*---------------------------------------------------------------------------*/
/* struct nand_dev_info - Nand device information structure */
/*---------------------------------------------------------------------------*/
struct nand_dev_info {
    char *name; /* Human-readable label */
    union {
        uint8_t id[NAND_MAX_ID_LEN]; /* The full ID array */
        struct {
            uint8_t mfr_id; /* id[0]: Manufacturer ID */
            uint8_t dev_id; /* id[1]: Device ID */
        };
    };
    unsigned short id_len; /* The valid length of ID */
    unsigned short oobsize; /* Spare area(OOB) size */

    uint32_t ecctype;

    uint32_t pagesize; /* Size of a page area */
    uint32_t page_shift;
    uint32_t pagemask;

    uint32_t blocksize; /* Size of an erase block */
    uint32_t block_shift;
    uint32_t blockmask;

    uint64_t chipsize; /* Total size of the device */
    uint32_t chip_shift;

    void *priv;
};

/*---------------------------------------------------------------------------*/
/* struct nand_info - Nand various interface and information structure */
/*---------------------------------------------------------------------------*/
struct nand_info {
    struct nand_dev_info dev;
    int32_t numchips;

    void *priv;

    int32_t (*erase)(struct nand_info *nand, uint32_t page);
    int32_t (*program)(struct nand_info *nand, uint32_t page);
    int32_t (*read)(struct nand_info *nand, uint32_t page);

    int32_t (*read_id)(struct nand_info *nand, uint8_t *id);
    int32_t (*read_status)(struct nand_info *nand);
    int32_t (*reset)(struct nand_info *nand);
    int32_t (*feature_op)(struct nand_info *nand, uint32_t cmd, uint8_t addr, uint8_t w_val, uint8_t *r_val);
    void (*ids_probe)(struct nand_info *nand);

    uint8_t (*read_byte)(struct nand_info *nand, int32_t offset);
    uint16_t (*read_word)(struct nand_info *nand, int32_t offset);
    void (*read_buf)(struct nand_info *nand, const uint8_t *buf, int32_t len, int32_t offset);
    void (*write_buf)(struct nand_info *nand, const uint8_t *buf, int32_t len, int32_t offset);

    struct nand_dev_info *(*get_dev_info_by_id)(struct nand_info *nand);
    int32_t (*oob_resize)(struct nand_info *nand);

    int32_t (*block_isbad)(struct nand_info *nand, uint64_t ofs);
    int32_t (*block_markbad)(struct nand_info *nand, uint64_t ofs);

    uint8_t cur_cs;
    int32_t (*get_device)(struct nand_info *nand);
    void (*put_device)(struct nand_info *nand);

    int32_t op_state;

    uint32_t lock;
    int32_t (*resume)(struct nand_info *nand);
};

#endif /* End of __NAND_COMMON_H__ */

