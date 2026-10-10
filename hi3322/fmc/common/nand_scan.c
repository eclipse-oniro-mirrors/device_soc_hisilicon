/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "string.h"
#include "errno.h"
#include "dpal.h"
#include "asm/delay.h"
#include "los_exc.h"
#include "nand_common.h"
#include "nand_ids.h"

int32_t nand_get_device(struct nand_info *nand)
{
    if (OS_EXC_ACTIVE || dpal_mux_pend(nand->lock, DPAL_WAIT_FOREVER) == DPAL_OK) {
        return NAND_STATUS_SUCCESS;
    }
    return -EBUSY;
}

static void nand_put_device(struct nand_info *nand)
{
    if (OS_EXC_ACTIVE) {
        return;
    }
    (void)dpal_mux_post(nand->lock);
}

static void nand_check_func(struct nand_info *nand)
{
    if (!nand->get_device) {
        nand->get_device = nand_get_device;
    }

    if (!nand->put_device) {
        nand->put_device = nand_put_device;
    }
}

static int32_t nand_get_dev_id(struct nand_info *nand)
{
    uint8_t tmp_ids[NAND_MAX_ID_LEN];
    uint8_t *ids = nand->dev.id;

    /* Clean out ID array in nand_dev_info */
    memset_s(ids, NAND_MAX_ID_LEN, 0, NAND_MAX_ID_LEN);

    /* Send command for read nand device ID */
    nand->read_id(nand, ids);
    /* Try again to make sure this IDs isn't random data */
    nand->read_id(nand, tmp_ids);
    if (tmp_ids[0] != ids[0] || tmp_ids[1] != ids[1]) {
        ERR_MSG("Second ID:%02x %02x did not match first:%02x" \
                " %02x\n", tmp_ids[0], tmp_ids[1], ids[0], ids[1]);
        return -ENODEV;
    }

    return NAND_STATUS_SUCCESS;
}

static int32_t nand_get_dev_info(struct nand_info *nand)
{
    int32_t ret;

    ret = nand_get_dev_id(nand);
    if (ret) {
        return ret;
    }

    if (nand_get_dev_info_by_id(nand) == NULL) {
        return NAND_STATUS_FAIL;
    }

    nand->numchips++;

    return nand->oob_resize(nand);
}

int32_t nand_scan(struct mtd_info *mtd)
{
    struct nand_info *nand = mtd->priv;

    nand_check_func(nand);

    if (nand_get_dev_info(nand)) {
        return NAND_STATUS_FAIL;
    }
    return NAND_STATUS_SUCCESS;
}