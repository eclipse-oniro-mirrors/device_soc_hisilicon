/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "errno.h"
#include "string.h"
#include "stdlib.h"
#include "linux/mtd/mtd.h"
#include "dpal.h"
#include "pm_veto.h"
#include "nand_common.h"
#include "nand_ops.h"

int32_t nand_block_markbad(struct nand_info *nand, uint32_t addr)
{
    uint32_t pagesize;
    uint32_t page;
    int32_t status;

    /* get device */
    if (nand->get_device(nand)) {
        return -1;
    }
    uapi_pm_add_sleep_veto(PM_ID_NAND);
    uint8_t badflag[NAND_BB_SIZE];
    /* set first page's BB to all 0x00 to mark bad block */
    memset_s(badflag, NAND_BB_SIZE, 0x00, NAND_BB_SIZE);

    pagesize = nand->dev.pagesize;
    nand->write_buf(nand, badflag, NAND_BB_SIZE, pagesize);

    page = page_addr(addr, nand->dev.page_shift);
    nand->program(nand, page);
    nand->op_state = NAND_PROGING;
    status = nand->read_status(nand);
    if ((uint32_t)status & NAND_STATUS_FAIL) {
        ERR_MSG("page[0x%0x]:program fail!\n", page);
        uapi_pm_remove_sleep_veto(PM_ID_NAND);
        nand->put_device(nand);
        return -EIO;
    }

    /* release device */
    uapi_pm_remove_sleep_veto(PM_ID_NAND);
    nand->put_device(nand);
    return 0;
}

int32_t nand_block_isbad(struct nand_info *nand, uint32_t addr)
{
    uint8_t bad_block[NAND_BB_SIZE];
    uint32_t page;
    int32_t status;

    /* get device */
    if (nand->get_device(nand)) {
        return -1;
    }
    uapi_pm_add_sleep_veto(PM_ID_NAND);
    page = page_addr(addr, nand->dev.page_shift);
    nand->read(nand, page);
    nand->op_state = NAND_READING;
    status = nand->read_status(nand);
    if ((uint32_t)status & NAND_STATUS_FAIL) {
        ERR_MSG("page[0x%0x]:read fail!\n", page);
        uapi_pm_remove_sleep_veto(PM_ID_NAND);
        nand->put_device(nand);
        return -EIO;
    }
    nand->read_buf(nand, bad_block, NAND_BB_SIZE, nand->dev.pagesize);
    /* release device */
    uapi_pm_remove_sleep_veto(PM_ID_NAND);
    nand->put_device(nand);
    if (bad_block[0] != 0xff || bad_block[1] != 0xff) { /* bad block */
        return 1;
    }
    return 0;
}

int32_t nand_do_erase_ops(struct mtd_info *mtd, uint32_t addr)
{
    struct nand_info *nand = mtd->priv;
    uint32_t page;
    int32_t status;

    /* get device */
    if (nand->get_device(nand)) {
        return -1;
    }
    uapi_pm_add_sleep_veto(PM_ID_NAND);
    page = page_addr(addr, nand->dev.page_shift);
    nand->op_state = NAND_ERASING;
    nand->erase(nand, page);
    status = nand->read_status(nand);
    if ((uint32_t)status & NAND_STATUS_FAIL) {
        ERR_MSG("page[0x%0x]:erase fail!\n", page);
        uapi_pm_remove_sleep_veto(PM_ID_NAND);
        nand->put_device(nand);
        return -EIO;
    }
    /* release device */
    uapi_pm_remove_sleep_veto(PM_ID_NAND);
    nand->put_device(nand);
    return NAND_STATUS_SUCCESS;
}

int32_t nand_get_id_ops(struct mtd_info *mtd, void **ids)
{
    struct nand_info *nand = mtd->priv;
    *ids = nand->dev.id;
    return 0;
}

int32_t nand_do_write_ops(struct mtd_info *mtd, uint32_t to, struct mtd_oob_ops *ops)
{
    struct nand_info *nand = mtd->priv;
    const char *datbuf = ops->datbuf;
    const char *oobbuf = ops->oobbuf;
    uint8_t badflag[NAND_BB_SIZE];
    uint32_t page;
    int32_t status;

    uint32_t writesize = ops->len;
    uint32_t oobsize = ops->ooblen;

    /* get device */
    if (nand->get_device(nand)) {
        return -1;
    }
    uapi_pm_add_sleep_veto(PM_ID_NAND);
    nand->write_buf(nand, (uint8_t *)datbuf, writesize, 0);
    if (oobsize == 0) {
        memset_s(badflag, NAND_BB_SIZE, 0xff, NAND_BB_SIZE);
        nand->write_buf(nand, badflag, NAND_BB_SIZE, writesize);
    } else {
        nand->write_buf(nand, (uint8_t *)oobbuf, oobsize, writesize);
    }

    page = page_addr(to, nand->dev.page_shift);
    nand->op_state = NAND_PROGING;
    nand->program(nand, page);
    status = nand->read_status(nand);
    /* See if page program succeeded */
    if ((uint32_t)status & NAND_STATUS_FAIL) {
        ERR_MSG("page[0x%0x]:program fail!\n", page);
        uapi_pm_remove_sleep_veto(PM_ID_NAND);
        nand->put_device(nand);
        return -EIO;
    }
    uapi_pm_remove_sleep_veto(PM_ID_NAND);
    nand->put_device(nand);
    return 0;
}

int32_t nand_do_read_ops(struct nand_info *nand, uint32_t from, struct mtd_oob_ops *ops)
{
    const char *datbuf = ops->datbuf;
    const char *oobbuf = ops->oobbuf;
    uint32_t col = from & nand->dev.pagemask;
    uint32_t pagesize = ops->len;
    uint32_t oobsize = ops->ooblen;
    uint32_t page;
    int32_t status;

    /* get device */
    if (nand->get_device(nand)) {
        return -1;
    }
    uapi_pm_add_sleep_veto(PM_ID_NAND);
    page = page_addr(from, nand->dev.page_shift);
    /* Now read the page into the buffer */
    nand->op_state = NAND_READING;
    nand->read(nand, page);
    status = nand->read_status(nand);
    /* See if page read succeeded */
    if ((uint32_t)status & NAND_STATUS_FAIL) {
        ERR_MSG("page[0x%0x]:read fail!\n", page);
        uapi_pm_remove_sleep_veto(PM_ID_NAND);
        nand->put_device(nand);
        return -EIO;
    }
    nand->read_buf(nand, (uint8_t *)datbuf, pagesize, col);
    nand->read_buf(nand, (uint8_t *)oobbuf, oobsize, pagesize);
    uapi_pm_remove_sleep_veto(PM_ID_NAND);
    /* release device */
    nand->put_device(nand);

    return 0;
}