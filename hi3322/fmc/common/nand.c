/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "errno.h"
#include "stdio.h"
#include "stdlib.h"

#include "dpal.h"

#include "linux/mtd/mtd.h"
#include "linux/mtd/mtd_list.h"

#include "fs/fs.h"
#include "common_def.h"
#include "watchdog.h"
#include "nand_ops.h"
#include "nand.h"

#define DPAL_MTD_FAIL_ADDR_UNKNOWN   (-1LL)

struct mtd_info *g_nand_mtd = NULL;

static int32_t g_lastbad_rets = 0;
static uint32_t g_last_block_first_page_index = 0;

int32_t nand_erase_force(void)
{
/* 预留block0 用于boot启动 */
#define BOOT_NAND_BLOCK_NUM 1

    uint32_t start_addr = g_nand_mtd->erasesize * BOOT_NAND_BLOCK_NUM;
    uint32_t len = g_nand_mtd->size - g_nand_mtd->erasesize * BOOT_NAND_BLOCK_NUM;
    g_last_block_first_page_index = 0;
    for (uint32_t addr = start_addr; addr < start_addr + len; addr += g_nand_mtd->erasesize) {
        if (nand_do_erase_ops(g_nand_mtd, addr) != 0) {
            continue;
        }
        uapi_watchdog_kick();
    }
    return 0;
}

static int32_t nand_erase(struct mtd_info *mtd, struct erase_info *ops)
{
    int32_t ret = 0;
    struct nand_info *nand = mtd->priv;
    uint32_t addr = (uint32_t)ops->addr;
    uint32_t len = (uint32_t)ops->len;
    ops->fail_addr = DPAL_MTD_FAIL_ADDR_UNKNOWN;
    g_last_block_first_page_index = 0;
    /* Start address must align on block boundary */
    if (addr & (mtd->erasesize - 1)) {
        ERR_MSG("Unaligned erase address!\n");
        return -EINVAL;
    }

    if ((len + addr) > mtd->size) {
        ERR_MSG("Out of range!\n");
        return -EINVAL;
    }

    if (len < mtd->erasesize) {
        ERR_MSG("Erase size 0x%08x smaller than one erase block 0x%08x\n!", len, mtd->erasesize);
        return -EINVAL;
    }
    for (; addr < ops->addr + len; addr += mtd->erasesize) {
        if (nand_block_isbad(nand, addr)) {
            INFO_MSG("Skipping bad block at 0x%08x\n", (uint32_t)addr);
            continue;
        }
        ret = nand_do_erase_ops(mtd, addr);
        if (ret != 0) {
            ERR_MSG("NAND erase failure: %d\n", ret);
            nand_block_markbad(nand, addr);
            ERR_MSG("Block at 0x%08x is marked bad block\n", (uint32_t)addr);
            ops->fail_addr = addr;
            return -EIO;
        }
        uapi_watchdog_kick();
    }
    return 0;
}

static uint32_t get_len_incl_bad(struct mtd_info *mtd, uint32_t addr, const uint32_t length)
{
    uint32_t len_incl_bad = 0;
    uint32_t len_excl_bad = 0;
    uint32_t block_len = 0;
    uint32_t offset = addr;
    struct nand_info *nand = mtd->priv;

    while (len_excl_bad < length) {
        block_len = nand->dev.blocksize - (offset & (nand->dev.blocksize - 1));
        if (!nand_block_isbad(nand, offset & ~((uint32_t)nand->dev.blocksize - 1))) {
            len_excl_bad += block_len;
        }

        len_incl_bad += block_len;
        offset       += block_len;
        if (offset > mtd->size) {
            break;
        }
    }

    return len_incl_bad;
}

static int32_t write_param_check(struct mtd_info *mtd, uint64_t addr, uint32_t len)
{
    if (addr & (mtd->writesize - 1)) {
        ERR_MSG("Write address not page aligned.\n");
        return -EINVAL;
    }

    if (len % (mtd->writesize) != 0) {
        ERR_MSG("Attempt to write non aligned data, length:%d pagesize:%d\n",
            (uint32_t)len, mtd->writesize);
        return -EINVAL;
    }

    return 0;
}

static int32_t write_oob_param_check(struct mtd_info *mtd, uint64_t addr, uint32_t len)
{
    if (addr & (mtd->writesize - 1)) {
        ERR_MSG("Write address not page aligned.\n");
        return -EINVAL;
    }

    if ((len % (mtd->writesize + mtd->oobsize)) != 0) {
        ERR_MSG("Attempt to write non aligned data, length:%d pagesize:%d oobsize:%d\n",
            (uint32_t)len, mtd->writesize, mtd->oobsize);
        return -EINVAL;
    }
    g_last_block_first_page_index = 0;
    return 0;
}

static uint32_t nand_size_get(uint32_t len, uint32_t remain)
{
    uint32_t size;

    if (len < remain) {
        size = len;
    } else {
        size = remain;
    }

    return size;
}

static int32_t nand_write(struct mtd_info *mtd, int64_t addr, uint32_t len,
    uint32_t *retlen, const char *buffer)
{
    unused(retlen);
    struct mtd_oob_ops ops = {0};
    const char *buf = buffer;
    uint32_t left_to_write, len_incl_bad, len_data;
    uint32_t to = (uint32_t)addr;

    if (write_param_check(mtd, to, len)) {
        return -EINVAL;
    }
    len_data = len;
    g_last_block_first_page_index = 0;
    len_incl_bad = get_len_incl_bad(mtd, to, len_data);
    if ((to + len_incl_bad) > mtd->size) {
        return -EINVAL;
    }
    if (len_incl_bad == len_data) {
        for (uint32_t i = 0; i < len_data / mtd->writesize; i++) {
            ops.datbuf = (const char *)buf + i * (mtd->writesize);
            ops.len = mtd->writesize;
            ops.ooblen = 0;
            if (nand_do_write_ops(mtd, to + i * mtd->writesize, &ops)) {
                return -EIO;
            }
            uapi_watchdog_kick();
        }
        return 0;
    }
    left_to_write = len_data;
    while (left_to_write > 0) {
        uint32_t block_offset = to & (mtd->erasesize - 1);
        if (nand_block_isbad(mtd->priv, to & ~((uint32_t)mtd->erasesize - 1))) {
            to += mtd->erasesize - block_offset;
            continue;
        }
        uint32_t write_size = nand_size_get(left_to_write, mtd->erasesize - block_offset);
        for (uint32_t i = 0; i < write_size / mtd->writesize; i++) {
            ops.datbuf = (const char *)buf;
            ops.len = mtd->writesize;
            ops.ooblen = 0;
            if (nand_do_write_ops(mtd, to, &ops)) {
                return -EIO;
            }
            buf += mtd->writesize;
            left_to_write -= mtd->writesize;
            to += mtd->writesize;
            uapi_watchdog_kick();
        }
    }
    return 0;
}

static int32_t nand_write_oob(struct mtd_info *mtd, int64_t addr, uint32_t len,
    uint32_t *retlen, const char *buffer)
{
    unused(retlen);
    const char *buf = buffer;
    struct mtd_oob_ops ops = {0};
    uint32_t to = (uint32_t)addr;
    if (write_oob_param_check(mtd, to, len)) {
        return -EINVAL;
    }
    uint32_t len_data = (len / (mtd->writesize + mtd->oobsize)) * mtd->writesize;
    uint32_t len_incl_bad = get_len_incl_bad(mtd, to, len_data);
    if ((to + len_incl_bad) > mtd->size) {
        return -EINVAL;
    }
    if (len_incl_bad == len_data) {
        for (uint32_t i = 0; i < len_data / mtd->writesize; i++) {
            ops.datbuf = (const char *)buf + i * (mtd->writesize + mtd->oobsize);
            ops.oobbuf = (const char *)buf + i * (mtd->writesize + mtd->oobsize) + mtd->writesize;
            ops.len = mtd->writesize;
            ops.ooblen = mtd->oobsize;
            if (nand_do_write_ops(mtd, to + i * mtd->writesize, &ops)) {
                return -EIO;
            }
            uapi_watchdog_kick();
        }
        return 0;
    }
    uint32_t left_to_write = len_data;
    while (left_to_write > 0) {
        uint32_t block_offset = to & (mtd->erasesize - 1);
        if (nand_block_isbad(mtd->priv, to & ~((uint32_t)mtd->erasesize - 1))) {
            to += mtd->erasesize - block_offset;
            continue;
        }
        uint32_t write_size = nand_size_get(left_to_write, mtd->erasesize - block_offset);
        for (uint32_t i = 0; i < write_size / mtd->writesize; i++) {
            ops.datbuf = (const char *)buf;
            ops.oobbuf = (const char *)buf + mtd->writesize;
            ops.len = mtd->writesize;
            ops.ooblen = mtd->oobsize;
            if (nand_do_write_ops(mtd, to, &ops)) {
                return -EIO;
            }
            buf += mtd->writesize + mtd->oobsize;
            left_to_write -= mtd->writesize;
            to += mtd->writesize;
            uapi_watchdog_kick();
        }
    }
    return 0;
}

static int32_t read_param_check(struct mtd_info *mtd, uint64_t addr, uint32_t len)
{
    if (addr & (mtd->writesize - 1)) {
        ERR_MSG("read address not page aligned.\n");
        return -EINVAL;
    }

    if (len <= 0) {
        ERR_MSG("read len < 0.\n");
        return -EINVAL;
    }

    return 0;
}

static int32_t nand_read_priv(struct mtd_info *mtd, uint64_t addr, uint32_t len, char *buf)
{
    struct nand_info *nand = mtd->priv;
    struct  mtd_oob_ops ops = {0};
    uint32_t left_to_read = len;
    uint32_t len_incl_bad;
    uint32_t from = (uint32_t)addr;
    uint32_t col;

    ops.ooblen = 0;
    ops.datbuf = (const char *)buf;
    g_last_block_first_page_index = 0;
    len_incl_bad = get_len_incl_bad(mtd, from, len);
    if ((uint32_t)(from + len_incl_bad) > mtd->size) {
        ERR_MSG("Attempt to read outside the flash area.\n");
        return -EINVAL;
    }

    col = from & nand->dev.pagemask;
    if ((uint32_t)len_incl_bad == len) {
        left_to_read = len_incl_bad;
        if (col != 0) {
            ops.len = nand_size_get(left_to_read, mtd->writesize - col);
            if (nand_do_read_ops(nand, from, &ops)) {
                ERR_MSG("NAND read to from %08x\n", (uint32_t)from);
                return -EIO;
            }

            from += ops.len;
            left_to_read -= ops.len;
            ops.datbuf += ops.len;
        }

        while (left_to_read > 0) {
            if ((uint32_t)left_to_read >= mtd->writesize) {
                ops.len = mtd->writesize;
            } else {
                ops.len = left_to_read;
            }
            if (nand_do_read_ops(nand, from, &ops)) {
                ERR_MSG("NAND read to from %08x\n", (uint32_t)from);
                return -EIO;
            }
            ops.datbuf += ops.len;
            from += ops.len;
            left_to_read -= ops.len;
        }
        return 0;
    }

    return 1;
}

static int32_t nand_read(struct mtd_info *mtd, int64_t addr, uint32_t len,
    uint32_t *retlen, char *buf)
{
    unused(retlen);
    struct nand_info *nand = mtd->priv;
    struct  mtd_oob_ops ops = {0};
    uint32_t left_to_read = len;
    uint32_t from = (uint32_t)addr;
    uint32_t col;
    ops.datbuf = (const char *)buf;
    col = from & nand->dev.pagemask;
    g_last_block_first_page_index = 0;
    if ((read_param_check(mtd, addr, len)) || (nand_read_priv(mtd, addr, len, buf) != 1)) {
        return 0;
    }
    while (left_to_read > 0) {
        if (nand_block_isbad(nand, from & ~((uint32_t)mtd->erasesize - 1))) {
            from = from + mtd->erasesize;
            continue;
        }
        int32_t block_from = from & (mtd->erasesize - 1);
        uint32_t read_size = nand_size_get(left_to_read, mtd->erasesize - block_from);
        if (col != 0) {
            ops.len = nand_size_get(left_to_read, mtd->writesize - col);
            if (nand_do_read_ops(nand, from, &ops)) {
                return -EIO;
            }
            uapi_watchdog_kick();
            from += ops.len;
            left_to_read -= ops.len;
            read_size -= ops.len;
            ops.datbuf += ops.len;
            col = 0;
        }

        while (read_size > 0) {
            if ((uint32_t)read_size >= mtd->writesize) {
                ops.len = mtd->writesize;
            } else {
                ops.len = read_size;
            }
            if (nand_do_read_ops(nand, from, &ops)) {
                return -EIO;
            }
            ops.datbuf += ops.len;
            from += ops.len;
            left_to_read -= ops.len;
            read_size -= ops.len;
            uapi_watchdog_kick();
        }
    }
    return 0;
}

static int32_t read_oob_param_check(struct mtd_info *mtd, uint64_t addr, uint32_t len)
{
    if (addr & (mtd->writesize - 1)) {
        ERR_MSG("Attempt to read non page aligned data, from %08x.\n", (uint32_t)addr);
        return -EINVAL;
    }
    if ((len % (mtd->writesize + mtd->oobsize)) != 0) {
        ERR_MSG("Attempt to read non aligned data, length:%d pagesize:%d oobsize:%d\n",
                (uint32_t)len, mtd->writesize, mtd->oobsize);
        return -EINVAL;
    }

    return 0;
}

static int32_t nand_read_page_oob(struct mtd_info *mtd, int64_t addr, uint32_t len,
    uint32_t *retlen, char *buf)
{
    unused(retlen);
    if (read_oob_param_check(mtd, addr, len)) {
        return -EINVAL;
    }
    struct nand_info *nand = mtd->priv;
    struct mtd_oob_ops ops = {0};
    uint32_t from = (uint32_t)addr;
    // 某block的第一页的索引
    uint32_t curr_block_first_page_index = from &  ~((uint32_t)mtd->erasesize - 1);
    /*
     *  如果跟上次读的是同一个block，复用上次结果, 减少一次读Nand操作。
     *  如果有其他操作nand g_last_block_first_page_index = 0 失效。
    */
    if (g_last_block_first_page_index == 0 || (curr_block_first_page_index != g_last_block_first_page_index)) {
        g_last_block_first_page_index = curr_block_first_page_index;
        g_lastbad_rets = nand_block_isbad(nand, curr_block_first_page_index);
    }
    if (g_lastbad_rets) { // 1是坏块
        return -EIO;
    }
    ops.datbuf = (const char *)buf;
    ops.oobbuf = (const char *)buf + mtd->writesize;
    ops.len = mtd->writesize;
    ops.ooblen = mtd->oobsize;
    if (nand_do_read_ops(nand, from, &ops)) {
        return -EIO;
    }
    return 0;
}

int32_t nand_read_oob(struct mtd_info *mtd, int64_t addr, uint32_t len,
    uint32_t *retlen, char *buf)
{
    unused(retlen);
    struct nand_info *nand = mtd->priv;
    struct mtd_oob_ops ops = {0};
    uint32_t from = (uint32_t)addr;
    if (read_oob_param_check(mtd, addr, len)) {
        return -EINVAL;
    }
    uint32_t len_data = len / (mtd->writesize + mtd->oobsize) * mtd->writesize;
    uint32_t len_incl_bad = get_len_incl_bad(mtd, from, len_data);
    if ((from + len_incl_bad) > mtd->size) {
        return -EINVAL;
    }
    if (len_incl_bad == len_data) {
        for (uint32_t i = 0; i < len_data / mtd->writesize; i++) {
            ops.datbuf = (const char *)buf + i * (mtd->writesize + mtd->oobsize);
            ops.oobbuf = (const char *)buf + i * (mtd->writesize + mtd->oobsize) + mtd->writesize;
            ops.len = mtd->writesize;
            ops.ooblen = mtd->oobsize;
            if (nand_do_read_ops(nand, from + i * mtd->writesize, &ops)) {
                return -EIO;
            }
            uapi_watchdog_kick();
        }
        return 0;
    }
    uint32_t left_to_read = len_data;
    while (left_to_read > 0) {
        uint32_t block_from = from & (mtd->erasesize - 1);
        if (nand_block_isbad(nand, from &  ~((uint32_t)mtd->erasesize - 1))) {
            from += mtd->erasesize - block_from;
            continue;
        }
        uint32_t read_size = nand_size_get(left_to_read, mtd->erasesize - block_from);
        for (uint32_t i = 0; i < read_size / mtd->writesize; i++) {
            ops.datbuf = (const char *)buf;
            ops.oobbuf = (const char *)buf + mtd->writesize;
            ops.len = mtd->writesize;
            ops.ooblen = mtd->oobsize;
            if (nand_do_read_ops(nand, from, &ops)) {
                return -EIO;
            }
            buf += mtd->writesize + mtd->oobsize;
            left_to_read -= mtd->writesize;
            from += mtd->writesize;
            uapi_watchdog_kick();
        }
    }
    return 0;
}

static int32_t block_isbad(struct mtd_info *mtd, int64_t ofs)
{
    struct nand_info *nand = mtd->priv;
    /* fbb-verbatim deviation: yaffs scan/mount loops call block_isbad
     * hundreds of times back to back. The CLOSED fbb yaffs archive cannot
     * kick from its nf_* glue (binary), so with a 10s WDT the first mount
     * reset the board (morpheus board round 1). Kick here — behavior-neutral
     * for every other caller. */
    uapi_watchdog_kick();
    g_last_block_first_page_index = 0;
    return nand_block_isbad(nand, (uint32_t)ofs);
}

static int32_t block_markbad(struct mtd_info *mtd, int64_t ofs)
{
    struct nand_info *nand = mtd->priv;
    uapi_watchdog_kick();
    g_last_block_first_page_index = 0;
    return nand_block_markbad(nand, (uint32_t)ofs);
}

void nand_register(struct mtd_info *mtd)
{
    struct nand_info *nand;
    nand = mtd->priv;
    mtd->size = nand->dev.chipsize;
    mtd->erasesize = nand->dev.blocksize;
    mtd->writesize = nand->dev.pagesize;
    mtd->oobsize = nand->dev.oobsize;
    mtd->name = "nand";
    mtd->type = MTD_NANDFLASH;
    mtd->flags = MTD_CAP_NANDFLASH;
    mtd->erase = nand_erase;
    mtd->read_oob = nand_read_page_oob; // 只读一个page, yaffs已做好分包
    mtd->write_oob = nand_write_oob;
    mtd->read = nand_read;
    mtd->write = nand_write;
    mtd->block_isbad = block_isbad;
    mtd->block_markbad = block_markbad;
}