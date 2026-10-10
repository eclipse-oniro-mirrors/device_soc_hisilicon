/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "string.h"
#include "stdlib.h"
#include "errno.h"
#include "dpal.h"

#include "asm/platform.h"
#include "asm/io.h"

#include "nand.h"
#include "nand_common.h"
#include "nand_ids.h"
#include "nand_scan.h"

#include <linux/module.h>
#include "linux/mtd/mtd.h"
#include "nand_common.h"
#include "nand_ops.h"
#include "host_common.h"
#include "linux/mtd/mtd_list.h"
#include "tcxo.h"
#include "hifmc_union.h"
#include "hifmc_regs.h"
#include "hifmc_cmds.h"
#include "hifmc_common.h"
#include "hifmc100.h"

static struct nand_info *g_nand_info;
static struct nand_host *g_nand_host;
extern int mtd_init_list(void);

static void fmc_reg_write(uint32_t offset, uint32_t val)
{
    nand_write_u32((uint32_t)(uintptr_t)g_nand_host->regbase + offset, val);
}

static uint32_t fmc_reg_read(uint32_t offset)
{
    return nand_read_u32((uint32_t)(uintptr_t)g_nand_host->regbase + offset);
}

static uint8_t fmc100_read_byte(struct nand_info *nand, int32_t offset)
{
    struct nand_host *nand_host = nand->priv;
    return dpal_readb((uint8_t *)(nand_host->membase) + offset);
}

static uint16_t fmc100_read_word(struct nand_info *nand, int32_t offset)
{
    struct nand_host *nand_host = nand->priv;
    return dpal_readw(nand_host->membase + offset);
}

static void fmc100_write_buf(struct nand_info *nand, const uint8_t *buf, int32_t len, int32_t offset)
{
    struct nand_host *nand_host = nand->priv;
    struct nand_flash_info *nand_dev = (struct nand_flash_info *)(nand->dev.priv);
    memset_s((void *)(nand_host->dma_data + nand_host->pagesize), nand_host->oobsize, 0xff, nand_host->oobsize);
    memcpy_s((void *)(nand_host->dma_data + offset), len, (void *)buf, len);
    if (offset >= (int32_t)nand_host->pagesize && nand_dev->oob_format != NULL) {
        nand_dev->oob_format(1, nand_host->dma_oob, nand_host->oobsize);
    }
}

static void fmc100_read_buf(struct nand_info *nand, const uint8_t *buf, int32_t len, int32_t offset)
{
    struct nand_host *nand_host = nand->priv;
    struct nand_flash_info *nand_dev = (struct nand_flash_info *)(nand->dev.priv);
    if (offset >= (int32_t)nand_host->pagesize && nand_dev->oob_format != NULL) {
        nand_dev->oob_format(0, nand_host->dma_oob, nand_host->oobsize);
    }
    memcpy_s((void *)buf, len, (void *)(nand_host->dma_data + offset), len);
}

static int32_t fmc_status_check(int32_t flag)
{
    reg_fmc_op fmc_op;
    reg_fmc_op_ctrl fmc_op_ctrl;

    for (int32_t i = 0; i < HIFMC_TIME_OUT; i++) {
        if (flag == HIFMC_DMA) {
            fmc_op_ctrl.value = fmc_reg_read(FMC_OP_CTRL);
            if (fmc_op_ctrl.bits.dma_op_ready == 0x0) {
                return NAND_STATUS_SUCCESS;
            }
        } else {
            fmc_op.value = fmc_reg_read(FMC_OP);
            if (fmc_op.bits.reg_op_start == 0x0) {
                return NAND_STATUS_SUCCESS;
            }
        }
        uapi_tcxo_delay_us(5);  /* 5us */
    }

    return NAND_STATUS_FAIL;
}

static int32_t fmc100_reset(struct nand_info *nand)
{
    reg_fmc_cmd fmc_cmd;
    reg_fmc_op_cfg  fmc_op_cfg;
    reg_fmc_op fmc_op;

    fmc_cmd.value = 0;
    fmc_cmd.bits.cmd1 = RESET;
    fmc_reg_write(FMC_CMD, fmc_cmd.value);

    fmc_op_cfg.value = 0;
    fmc_op_cfg.bits.fm_cs = nand->cur_cs;
    fmc_reg_write(FMC_OP_CFG, fmc_op_cfg.value);

    fmc_op.value = 0;
    fmc_op.bits.reg_op_start = 0x1;
    fmc_op.bits.cmd1_en = 0x1;
    fmc_reg_write(FMC_OP, fmc_op.value);

    return fmc_status_check(HIFMC_REG);
}

static int32_t fmc100_erase(struct nand_info *nand, uint32_t page)
{
    reg_fmc_cmd fmc_cmd;
    reg_fmc_addrh fmc_addrh;
    reg_fmc_addrl fmc_addrl;
    reg_fmc_op_cfg fmc_op_cfg;
    reg_fmc_op fmc_op;

    fmc_cmd.value = 0;
    fmc_cmd.bits.cmd1 = WRITE_ENABLE;
    fmc_reg_write(FMC_CMD, fmc_cmd.value);

    fmc_op.value = 0;
    fmc_op.bits.reg_op_start = 0x1;
    fmc_op.bits.cmd1_en = 0x1;
    fmc_reg_write(FMC_OP, fmc_op.value);

    int32_t ret = fmc_status_check(HIFMC_REG);
    if (ret) {
        ERR_MSG("fmc erase fail!\n");
        return ret;
    }

    fmc_cmd.value = 0;
    fmc_cmd.bits.cmd1 = BLOCK_ERASE;
    fmc_reg_write(FMC_CMD, fmc_cmd.value);

    fmc_addrh.value = 0x0;
    fmc_reg_write(FMC_ADDRH, fmc_addrh.value);

    fmc_addrl.value = 0;
    fmc_addrl.bits.addrl = page;
    fmc_reg_write(FMC_ADDRL, fmc_addrl.value);

    fmc_op_cfg.value = 0;
    fmc_op_cfg.bits.addr_num = 0x3;
    fmc_op_cfg.bits.fm_cs = nand->cur_cs;
    fmc_reg_write(FMC_OP_CFG, fmc_op_cfg.value);

    fmc_op.value = 0;
    fmc_op.bits.reg_op_start = 0x1;
    fmc_op.bits.addr_en = 0x1;
    fmc_op.bits.cmd1_en = 0x1;
    fmc_reg_write(FMC_OP, fmc_op.value);

    ret = fmc_status_check(HIFMC_REG);
    if (ret) {
        ERR_MSG("fmc erase fail!\n");
        return ret;
    }

    return ret;
}

static int32_t fmc100_dma_transfer(struct nand_info *nand, uint32_t page, uint32_t cmd)
{
    struct nand_host *nand_host = nand->priv;
    reg_fmc_addrh fmc_addrh;
    reg_fmc_addrl fmc_addrl;
    reg_fmc_dma_saddr_d0 fmc_dma_saddr_d0;
    reg_fmc_dma_saddr_oob fmc_dma_saddr_oob;
    reg_fmc_dma_len fmc_dma_len;
    reg_fmc_op_cfg fmc_op_cfg;
    reg_fmc_op_ctrl fmc_op_ctrl;
    uint32_t page_addrh = 0x0;
    uint32_t page_addrl = page;

    if (page > 0xFFFF) {
        page_addrh = (page & 0xFFFF0000);
        page_addrl = (page & 0xFFFF);
    }

    fmc_addrh.value = 0x0;
    fmc_addrh.bits.addrh = (page_addrh >> 16); // right 16
    fmc_reg_write(FMC_ADDRH, fmc_addrh.value);

    fmc_addrl.value = 0;
    fmc_addrl.bits.addrl = (page_addrl << 16); // left 16
    fmc_reg_write(FMC_ADDRL, fmc_addrl.value);

    fmc_dma_saddr_d0.value = 0;
    fmc_dma_saddr_d0.bits.dma_mem_saddr_d0 = (uint32_t)(uintptr_t)nand_host->dma_data;
    fmc_reg_write(FMC_DMA_SADDR_D0, fmc_dma_saddr_d0.value);

    fmc_dma_saddr_oob.value = 0;
    fmc_dma_saddr_oob.bits.dma_mem_saddr_oob = (uint32_t)(uintptr_t)nand_host->dma_oob;
    fmc_reg_write(FMC_DMA_SADDR_OOB, fmc_dma_saddr_oob.value);

    if (nand->dev.ecctype == NAND_ECC_0BIT) {
        fmc_dma_len.value = 0;
        fmc_dma_len.bits.dma_len = nand->dev.oobsize;
        fmc_reg_write(FMC_DMA_LEN, fmc_dma_len.value);
    }

    fmc_op_cfg.value = 0;
    if ((cmd == READ_FROM_CACHE4) || (cmd == READ_FROM_CACHE)) {
        fmc_op_cfg.bits.dummy_num = 0x1;
    }
    fmc_op_cfg.bits.addr_num = 0x2;
    fmc_op_cfg.bits.fm_cs = nand->cur_cs;
    fmc_op_cfg.bits.mem_if_type = 0x3; /* PROGRAM_LOAD4/READ_FROM_CACHE4 */
    if ((cmd == PROGRAM_LOAD) || (cmd == READ_FROM_CACHE)) {
        fmc_op_cfg.bits.mem_if_type = 0x0;
    }
    fmc_reg_write(FMC_OP_CFG, fmc_op_cfg.value);

    fmc_op_ctrl.value = 0;
    fmc_op_ctrl.bits.dma_op_ready = 0x1;
    if ((cmd == READ_FROM_CACHE4) || (cmd == READ_FROM_CACHE)) {
        fmc_op_ctrl.bits.rd_opcode = cmd;
    } else if (cmd == PROGRAM_LOAD4 || (cmd == PROGRAM_LOAD)) {
        fmc_op_ctrl.bits.rw_op = 0x1;
        fmc_op_ctrl.bits.wr_opcode = cmd;
    }
    fmc_reg_write(FMC_OP_CTRL, fmc_op_ctrl.value);

    return fmc_status_check(HIFMC_DMA);
}

static int32_t fmc100_program(struct nand_info *nand, uint32_t page)
{
    return fmc100_dma_transfer(nand, page, PROGRAM_LOAD4);
}

static int32_t fmc100_read(struct nand_info *nand, uint32_t page)
{
    return fmc100_dma_transfer(nand, page, READ_FROM_CACHE4);
}

static int32_t fmc100_feature_op(struct nand_info *nand, uint32_t cmd,
    uint8_t addr, uint8_t w_val, uint8_t *r_val)
{
    struct nand_host *nand_host = nand->priv;
    reg_fmc_cmd fmc_cmd;
    reg_fmc_addrh fmc_addrh;
    reg_fmc_addrl fmc_addrl;
    reg_fmc_op_cfg fmc_op_cfg;
    reg_fmc_data_num fmc_data_num;
    reg_fmc_op fmc_op;
    int32_t ret;

    fmc_cmd.value = 0;
    fmc_cmd.bits.cmd1 = cmd;
    fmc_reg_write(FMC_CMD, fmc_cmd.value);
    fmc_addrh.value = 0x0;
    fmc_reg_write(FMC_ADDRH, fmc_addrh.value);

    fmc_addrl.value = 0;
    fmc_addrl.bits.addrl = addr;
    fmc_reg_write(FMC_ADDRL, fmc_addrl.value);

    fmc_op_cfg.value = 0;
    fmc_op_cfg.bits.addr_num = 0x1;
    fmc_op_cfg.bits.fm_cs = nand->cur_cs;
    fmc_reg_write(FMC_OP_CFG, fmc_op_cfg.value);

    fmc_data_num.value = 0;
    fmc_data_num.bits.op_data_num = 0x1;
    fmc_reg_write(FMC_DATA_NUM, fmc_data_num.value);

    fmc_op.value = 0;
    fmc_op.bits.reg_op_start = 0x1;
    fmc_op.bits.addr_en = 0x1;
    fmc_op.bits.cmd1_en = 0x1;
    if (cmd == SET_FEATURE) {
        fmc_op.bits.write_data_en = 0x1;
        nand_write_u8((uint32_t)(uintptr_t)nand_host->membase, w_val);
    } else {
        fmc_op.bits.read_data_en = 0x1;
    }
    fmc_reg_write(FMC_OP, fmc_op.value);
    ret = fmc_status_check(HIFMC_REG);
    if (ret) {
        ERR_MSG("fmc op fail!\n");
        return ret;
    }

    if (cmd == GET_FEATURE) {
        *r_val = nand_read_u8((uint32_t)(uintptr_t)nand_host->membase);
    }

    return ret;
}

static int32_t fmc100_read_status(struct nand_info *nand)
{
    uint8_t status;
    int32_t ret;

    for (int32_t i = 0; i < HIFMC_TIME_OUT; i++) {
        nand->feature_op(nand, GET_FEATURE, DEVICE_STATUS, 0, &status);
        status = status & 0xff;
        switch (nand->op_state) {
            case NAND_PROGING:
                ret = ((status & STATUS_OIP_MASK) || (status & STATUS_P_FAIL_MASK)) ?
                    NAND_STATUS_FAIL : NAND_STATUS_SUCCESS;
                break;
            case NAND_ERASING:
                ret = ((status & STATUS_OIP_MASK) || (status & STATUS_E_FAIL_MASK)) ?
                    NAND_STATUS_FAIL : NAND_STATUS_SUCCESS;
                break;
            case NAND_READING:
                ret = (status & STATUS_OIP_MASK) ? NAND_STATUS_FAIL : NAND_STATUS_SUCCESS;
                break;
            default:
                ret = NAND_STATUS_SUCCESS;
        }
        uapi_tcxo_delay_us(5); /* 5us */
        if (ret == NAND_STATUS_SUCCESS) {
            return ret;
        }
    }

    ERR_MSG("fmc read status err: 0x%x\n", status);
    return NAND_STATUS_FAIL;
}

static int32_t fmc100_read_id(struct nand_info *nand, uint8_t *id)
{
    struct nand_host *nand_host = nand->priv;
    reg_fmc_cmd fmc_cmd;
    reg_fmc_data_num fmc_data_num;
    reg_fmc_addrl fmc_addrl;
    reg_fmc_op_cfg  fmc_op_cfg;
    reg_fmc_op fmc_op;
    int32_t ret;

    memset_s((uint8_t *)(nand_host->membase), MAX_SPI_NAND_ID_LEN, 0, MAX_SPI_NAND_ID_LEN);

    fmc_cmd.value = 0;
    fmc_cmd.bits.cmd1 = READ_ID;
    fmc_reg_write(FMC_CMD, fmc_cmd.value);

    fmc_data_num.value = 0;
    fmc_data_num.bits.op_data_num = MAX_SPI_NAND_ID_LEN;
    fmc_reg_write(FMC_DATA_NUM, fmc_data_num.value);

    fmc_addrl.value = 0;
    fmc_addrl.bits.addrl = 0x0;
    fmc_reg_write(FMC_ADDRL, fmc_addrl.value);

    fmc_op_cfg.value = 0;
    fmc_op_cfg.bits.dummy_num = 0x1;
    fmc_op_cfg.bits.fm_cs = nand->cur_cs;
    fmc_reg_write(FMC_OP_CFG, fmc_op_cfg.value);

    fmc_op.value = 0;
    fmc_op.bits.reg_op_start = 0x1;
    fmc_op.bits.read_data_en = 0x1;
    fmc_op.bits.cmd1_en = 0x1;
    fmc_op.bits.dummy_en = 0x1;
    fmc_reg_write(FMC_OP, fmc_op.value);

    ret = fmc_status_check(HIFMC_REG);
    if (ret) {
        ERR_MSG("fmc read id fail!\n");
        return ret;
    }

    memcpy_s((void *)id, MAX_SPI_NAND_ID_LEN, (void *)nand_host->membase, MAX_SPI_NAND_ID_LEN);

    return ret;
}

static int32_t fmc100_oob_resize(struct nand_info *nand)
{
    uint32_t reg;
    reg_fmc_cfg fmc_cfg;
    struct nand_host *nand_host = nand->priv;

    reg = fmc_reg_read(FMC_CFG);
    INFO_MSG("fmc_cfg = 0x%x.\n", reg);

    fmc_cfg.value = reg;
    fmc_cfg.bits.page_size = (uint32_t)nandpage_size2type(nand->dev.pagesize);
    fmc_cfg.bits.block_size = (uint32_t)nandblock_size2type(nand->dev.blocksize / nand->dev.pagesize);
    fmc_reg_write(FMC_CFG, fmc_cfg.value);
    INFO_MSG("fmc_cfg = 0x%x.\n", fmc_cfg.value);

    nand_host->cfg = fmc_cfg.value;
    nand_host->oobsize = nand->dev.oobsize;
    nand_host->pagesize = nand->dev.pagesize;

    nand_host->buforg = osal_kmalloc_align(dpal_align(nand_host->pagesize + nand_host->oobsize, CACHE_ALIGNED_SIZE),
                                           0, CACHE_ALIGNED_SIZE);
    if (!nand_host->buforg) {
        ERR_MSG("Can't malloc memory for NAND driver.\n");
        return -1;
    }
    nand_host->buffer = nand_host->buforg;
    nand_host->dma_data = nand_host->buffer;
    nand_host->dma_oob = nand_host->dma_data + nand_host->pagesize;
    nand_host->bbm = (nand_host->buffer + nand_host->pagesize + NAND_BAD_BLOCK_POS);

    return 0;
}

int spinand_general_qe_enable(struct spi *spi)
{
    uint8_t value = 0;
    struct hifmc_host *fmc_host = (struct hifmc_host *)(spi->host);
    struct nand_host *nand_host = (struct nand_host *)(fmc_host->host);
    struct nand_info *nand = nand_host->nand;

    int32_t ret = nand->feature_op(nand, GET_FEATURE, DEVICE_OTP, 0x0, &value);
    INFO_MSG("get [0x%x]feature: 0x%x ret = %d\n", DEVICE_OTP, value, ret);

    value |= FEATURE_QE_ENABLE;
    ret = nand->feature_op(nand, SET_FEATURE, DEVICE_OTP, value, NULL);
    INFO_MSG("set [0x%x]feature: 0x%x ret = %d\n", DEVICE_OTP, value, ret);

    ret = nand->feature_op(nand, GET_FEATURE, DEVICE_OTP, 0x0, &value);
    INFO_MSG("get [0x%x]feature: 0x%x ret = %d\n", DEVICE_OTP, value, ret);
    return 0;
}

int spinand_qe_not_enable(struct spi *spi)
{
    return 0;
}

int spinand_buf_enable(struct spi *spi)
{
    uint8_t value = 0;
    struct hifmc_host *fmc_host = (struct hifmc_host *)(spi->host);
    struct nand_host *nand_host = (struct nand_host *)(fmc_host->host);
    struct nand_info *nand = nand_host->nand;

    int32_t ret = nand->feature_op(nand, GET_FEATURE, DEVICE_OTP, 0x0, &value);
    INFO_MSG("get [0x%x]feature: 0x%x ret = %d\n", DEVICE_OTP, value, ret);

    value = 0x8;  // 0x8 enable buf
    ret = nand->feature_op(nand, SET_FEATURE, DEVICE_OTP, value, NULL);
    INFO_MSG("set [0x%x]feature: 0x%x ret = %d\n", DEVICE_OTP, value, ret);

    ret = nand->feature_op(nand, GET_FEATURE, DEVICE_OTP, 0x0, &value);
    INFO_MSG("get [0x%x]feature: 0x%x ret = %d\n", DEVICE_OTP, value, ret);
    return 0;
}

static void fmc100_ids_probe(struct nand_info *nand)
{
    struct nand_host *nand_host = nand->priv;
    struct hifmc_host *fmc_host = nand_host->priv;
    struct spi *spi = fmc_host->spi;
    struct nand_flash_info *spi_dev = (struct nand_flash_info *)(nand->dev.priv);

    spi->host = fmc_host;
    spi->name = spi_dev->name;
    spi->driver = spi_dev->driver;
    spi->driver->qe_enable(spi);

    /* Disable write protection */
    uint8_t value = 0;
    int32_t ret = nand->feature_op(nand, GET_FEATURE, DEVICE_BLOCK_LOCK, 0, &value);
    INFO_MSG("get [0x%x]feature: 0x%x ret = %d\n", DEVICE_BLOCK_LOCK, value, ret);

    value = 0;
    ret = nand->feature_op(nand, SET_FEATURE, DEVICE_BLOCK_LOCK, value, NULL);
    INFO_MSG("set [0x%x]feature: 0x%x ret = %d\n", DEVICE_BLOCK_LOCK, value, ret);

    ret = nand->feature_op(nand, GET_FEATURE, DEVICE_BLOCK_LOCK, 0x0, &value);
    INFO_MSG("get [0x%x]feature: 0x%x ret = %d\n", DEVICE_BLOCK_LOCK, value, ret);
}

static void fmc100_nand_init(struct nand_info *nand)
{
    nand->erase = fmc100_erase;
    nand->program = fmc100_program;
    nand->read = fmc100_read;

    nand->read_id = fmc100_read_id;
    nand->read_status = fmc100_read_status;
    nand->reset = fmc100_reset;
    nand->feature_op = fmc100_feature_op;
    nand->ids_probe = fmc100_ids_probe;

    nand->read_byte = fmc100_read_byte;
    nand->read_word = fmc100_read_word;
    nand->read_buf = fmc100_read_buf;
    nand->write_buf = fmc100_write_buf;

    nand->oob_resize = fmc100_oob_resize;

    nand->cur_cs = HIFMC100_NAND_CS_NUM;
}

static int32_t fmc100_host_init(struct nand_host *nand_host)
{
    reg_fmc_cfg fmc_cfg;

    struct hifmc_host *fmc_host = osal_kzalloc(sizeof(struct hifmc_host), 0);
    if (!fmc_host) {
        ERR_MSG("no mem for hifmc_host!\n");
        return -1;
    }
    fmc_host->host = nand_host;
    nand_host->priv = fmc_host;

    fmc_cfg.value = 0;
    fmc_cfg.bits.op_mode = 0x1;
    fmc_cfg.bits.flash_sel = 0x1;
    fmc_cfg.bits.spi_nand_sel = 0x3;        /* enable plane select */
    fmc_reg_write(FMC_CFG, fmc_cfg.value);
    return 0;
}

static int32_t fmc100_resource_build(struct dpal_platform_device *dev)
{
    struct dpal_resource *res = NULL;

    g_nand_mtd = (struct mtd_info *)osal_kzalloc(sizeof(struct mtd_info), 0);
    if (!g_nand_mtd) {
        ERR_MSG("no mem for mtd_info!\n");
        return -1;
    }

    /* nand chip init */
    g_nand_info = (struct nand_info *)osal_kzalloc(sizeof(struct nand_info), 0);
    if (!g_nand_info) {
        ERR_MSG("no mem for nand_info!\n");
        goto err;
    }

    g_nand_mtd->priv = g_nand_info;
    g_nand_host = (struct nand_host *)osal_kzalloc(sizeof(struct nand_host), 0);
    if (!g_nand_host) {
        ERR_MSG("no mem for nand_host!\n");
        goto err1;
    }

    res = dpal_platform_get_resource(dev, IORESOURCE_MEM, 0);
    if (!res) {
        goto err2;
    }

    g_nand_host->regbase = (char *)(uintptr_t)dpal_platform_ioremap_resource(res);

    res = dpal_platform_get_resource(dev, IORESOURCE_MEM, 1);
    if (!res) {
        goto err2;
    }

    g_nand_host->membase = (char *)(uintptr_t)dpal_platform_ioremap_resource(res);

    g_nand_host->nand = g_nand_info;
    g_nand_info->priv = g_nand_host;

    return 0;

err2:
    osal_kfree(g_nand_host);
    g_nand_host = NULL;
err1:
    osal_kfree(g_nand_info);
    g_nand_info = NULL;
err:
    osal_kfree(g_nand_mtd);
    g_nand_mtd = NULL;
    return -1;
}

static int32_t fmc100_probe(struct dpal_platform_device *dev)
{
    if (fmc100_resource_build(dev)) {
        return -1;
    }

    fmc100_nand_init(g_nand_info);
    if (fmc100_host_init(g_nand_host)) {
        ERR_MSG("fmc100_host_init failed\n");
        goto err;
    }
    if (dpal_mux_create(&g_nand_info->lock) != DPAL_OK) {
        ERR_MSG("dpal_mux_create failed\n");
        goto err;
    }
    /* scan nand device entry */
    if (nand_scan(g_nand_mtd)) {
        ERR_MSG("nand scan fail!\n");
        goto err;
    }

    /* nand register */
    nand_register(g_nand_mtd);
    mtd_init_list();
    add_mtd_list("nand", g_nand_mtd);

    return 0;

err:
    osal_kfree(g_nand_host);
    osal_kfree(g_nand_info);
    osal_kfree(g_nand_mtd);
    g_nand_host = NULL;
    g_nand_info = NULL;
    g_nand_mtd = NULL;

    return -1;
}

static int32_t fmc100_suspend(struct device_dpal *dev)
{
    (void)dev;

    return 0;
}

static int32_t fmc100_resume(struct device_dpal *dev)
{
    (void)dev;

    return 0;
}

static const struct dpal_dev_pm_op fmc100_dev_pm_ops = {
    .suspend = fmc100_suspend,
    .resume = fmc100_resume,
};

static int32_t fmc100_remove(struct dpal_platform_device *dev)
{
    (void)dev;

    return 0;
}

static struct dpal_platform_driver fmc100_driver = {
    .probe      = fmc100_probe,
    .remove     = fmc100_remove,
    .drv     = {
        .name   = "fmc100_nand",
        .pm = &fmc100_dev_pm_ops,
    },
};

int32_t nand_fmc100_init(void)
{
    return dpal_platform_driver_register(&fmc100_driver);
}

void nand_fmc100_exit(void)
{
    dpal_platform_driver_unregister(&fmc100_driver);
}

module_init(nand_fmc100_init);
module_exit(nand_fmc100_exit);


/************************************ex*********************************************/

static int32_t fmc100_write_enable(void)
{
    reg_fmc_cmd fmc_cmd;
    reg_fmc_op fmc_op;
    int32_t ret;

    fmc_cmd.value = 0;
    fmc_cmd.bits.cmd1 = WRITE_ENABLE;
    fmc_reg_write(FMC_CMD, fmc_cmd.value);

    fmc_op.value = 0;
    fmc_op.bits.reg_op_start = 0x1;
    fmc_op.bits.cmd1_en = 0x1;
    fmc_reg_write(FMC_OP, fmc_op.value);

    ret = fmc_status_check(HIFMC_REG);
    if (ret) {
        ERR_MSG("fmc100_reg_write failed\n");
    }

    return ret;
}

static int32_t fmc100_load(struct nand_info *nand, uint32_t cmd)
{
    reg_fmc_cmd fmc_cmd;
    reg_fmc_addrh fmc_addrh;
    reg_fmc_addrl fmc_addrl;
    reg_fmc_op_cfg fmc_op_cfg;
    reg_fmc_data_num fmc_data_num;
    reg_fmc_op fmc_op;
    int32_t ret;

    /* load to cache */
    fmc_cmd.value = 0;
    fmc_cmd.bits.cmd1 = cmd;
    fmc_reg_write(FMC_CMD, fmc_cmd.value);

    fmc_addrh.value = 0;
    fmc_reg_write(FMC_ADDRH, fmc_addrh.value);

    fmc_addrl.value = 0;
    fmc_reg_write(FMC_ADDRL, fmc_addrl.value);

    fmc_op_cfg.value = 0;
    fmc_op_cfg.bits.addr_num = 0x2;
    if (cmd == PROGRAM_LOAD) {
        fmc_op_cfg.bits.mem_if_type = 0x0;
    } else if (cmd == PROGRAM_LOAD4) {
        fmc_op_cfg.bits.mem_if_type = 0x3;
    }
    fmc_op_cfg.bits.fm_cs = nand->cur_cs;
    fmc_reg_write(FMC_OP_CFG, fmc_op_cfg.value);

    fmc_data_num.value = 0;
    fmc_data_num.bits.op_data_num = nand->dev.pagesize;
    fmc_reg_write(FMC_DATA_NUM, fmc_data_num.value);

    fmc_op.value = 0;
    fmc_op.bits.reg_op_start = 0x1;
    fmc_op.bits.write_data_en = 0x1;
    fmc_op.bits.addr_en = 0x1;
    fmc_op.bits.cmd1_en = 0x1;
    fmc_reg_write(FMC_OP, fmc_op.value);

    ret = fmc_status_check(HIFMC_REG);
    if (ret) {
        ERR_MSG("fmc100_reg_write failed\n");
    }
    return ret;
}

static int32_t fmc100_excute(struct nand_info *nand, uint32_t page)
{
    reg_fmc_cmd fmc_cmd;
    reg_fmc_addrh fmc_addrh;
    reg_fmc_addrl fmc_addrl;
    reg_fmc_op_cfg fmc_op_cfg;
    reg_fmc_data_num fmc_data_num;
    reg_fmc_op fmc_op;
    int32_t ret;

    /* excute  */
    fmc_cmd.value = 0;
    fmc_cmd.bits.cmd1 = PROGRAM_EXECUTE;
    fmc_reg_write(FMC_CMD, fmc_cmd.value);

    fmc_addrh.value = 0x0;
    fmc_reg_write(FMC_ADDRH, fmc_addrh.value);

    fmc_addrl.value = 0;
    fmc_addrl.bits.addrl = page;
    fmc_reg_write(FMC_ADDRL, fmc_addrl.value);

    fmc_op_cfg.value = 0;
    fmc_op_cfg.bits.addr_num = 0x3;
    fmc_op_cfg.bits.fm_cs = nand->cur_cs;
    fmc_reg_write(FMC_OP_CFG, fmc_op_cfg.value);

    fmc_data_num.value = 0;
    fmc_reg_write(FMC_DATA_NUM, fmc_data_num.value);

    fmc_op.value = 0;
    fmc_op.bits.reg_op_start = 0x1;
    fmc_op.bits.addr_en = 0x1;
    fmc_op.bits.cmd1_en = 0x1;
    fmc_reg_write(FMC_OP, fmc_op.value);

    ret = fmc_status_check(HIFMC_REG);
    if (ret) {
        ERR_MSG("fmc100_reg_write failed\n");
    }
    return ret;
}

static int32_t fmc100_reg_write(struct nand_info *nand, uint32_t page, uint32_t cmd)
{
    int32_t ret;

    ret = fmc100_write_enable();
    if (ret) {
        return ret;
    }

    ret = fmc100_load(nand, cmd);
    if (ret) {
        return ret;
    }

    ret = fmc100_excute(nand, page);
    if (ret) {
        return ret;
    }

    return NAND_STATUS_SUCCESS;
}

static int32_t fmc100_page_read(struct nand_info *nand, uint32_t page)
{
    reg_fmc_cmd fmc_cmd;
    reg_fmc_addrh fmc_addrh;
    reg_fmc_addrl fmc_addrl;
    reg_fmc_op_cfg fmc_op_cfg;
    reg_fmc_data_num fmc_data_num;
    reg_fmc_op fmc_op;
    int32_t ret;

    /* page read */
    fmc_cmd.value = 0;
    fmc_cmd.bits.cmd1 = PAGE_READ;
    fmc_reg_write(FMC_CMD, fmc_cmd.value);
    fmc_addrh.value = 0x0;
    fmc_reg_write(FMC_ADDRH, fmc_addrh.value);

    fmc_addrl.value = 0;
    fmc_addrl.bits.addrl = page;
    fmc_reg_write(FMC_ADDRL, fmc_addrl.value);

    fmc_op_cfg.value = 0;
    fmc_op_cfg.bits.addr_num = 0x3;
    fmc_op_cfg.bits.fm_cs = nand->cur_cs;
    fmc_reg_write(FMC_OP_CFG, fmc_op_cfg.value);

    fmc_data_num.value = 0;
    fmc_reg_write(FMC_DATA_NUM, fmc_data_num.value);

    fmc_op.value = 0;
    fmc_op.bits.reg_op_start = 0x1;
    fmc_op.bits.addr_en = 0x1;
    fmc_op.bits.cmd1_en = 0x1;
    fmc_reg_write(FMC_OP, fmc_op.value);

    ret = fmc_status_check(HIFMC_REG);
    if (ret) {
        ERR_MSG("fmc100_reg_read failed\n");
    }

    fmc_op.value = 0;
    fmc_op.bits.reg_op_start = 0x1;
    fmc_op.bits.read_data_en = 0x1;
    fmc_reg_write(FMC_OP, fmc_op.value);

    ret = fmc_status_check(HIFMC_REG);
    if (ret) {
        ERR_MSG("fmc100_reg_read failed\n");
    }

    nand->op_state = NAND_READING;
    ret = nand->read_status(nand);
    if (ret) {
        ERR_MSG("fmc read fail!\n");
    }
    return ret;
}

static int32_t fmc100_cache_read(struct nand_info *nand, uint32_t cmd)
{
    reg_fmc_cmd fmc_cmd;
    reg_fmc_addrh fmc_addrh;
    reg_fmc_addrl fmc_addrl;
    reg_fmc_op_cfg fmc_op_cfg;
    reg_fmc_data_num fmc_data_num;
    reg_fmc_op fmc_op;
    int32_t ret;

    /* read cache */
    fmc_cmd.value = 0;
    fmc_cmd.bits.cmd1 = cmd;
    fmc_reg_write(FMC_CMD, fmc_cmd.value);

    fmc_addrh.value = 0;
    fmc_reg_write(FMC_ADDRH, fmc_addrh.value);

    fmc_addrl.value = 0;
    fmc_reg_write(FMC_ADDRL, fmc_addrl.value);

    fmc_op_cfg.value = 0;
    fmc_op_cfg.bits.dummy_num = 0x1;
    fmc_op_cfg.bits.addr_num = 0x2;
    fmc_op_cfg.bits.fm_cs = nand->cur_cs;
    if (cmd == READ_FROM_CACHE) {
        fmc_op_cfg.bits.mem_if_type = 0x0;
    } else if (cmd == READ_FROM_CACHE4) {
        fmc_op_cfg.bits.mem_if_type = 0x3;
    }
    fmc_reg_write(FMC_OP_CFG, fmc_op_cfg.value);

    fmc_data_num.value = 0;
    fmc_data_num.bits.op_data_num = nand->dev.pagesize;
    fmc_reg_write(FMC_DATA_NUM, fmc_data_num.value);

    fmc_op.value = 0;
    fmc_op.bits.reg_op_start = 0x1;
    fmc_op.bits.read_data_en = 0x1;
    fmc_op.bits.addr_en = 0x1;
    fmc_op.bits.cmd1_en = 0x1;
    fmc_op.bits.dummy_en = 0x1;
    fmc_reg_write(FMC_OP, fmc_op.value);

    ret = fmc_status_check(HIFMC_REG);
    if (ret) {
        ERR_MSG("fmc100_reg_read failed\n");
    }

    return ret;
}

static int32_t fmc100_reg_read(struct nand_info *nand, uint32_t page, uint32_t cmd)
{
    int32_t ret;

    ret = fmc100_page_read(nand, page);
    if (ret) {
        return ret;
    }

    ret = fmc100_cache_read(nand, cmd);
    if (ret) {
        return ret;
    }

    return NAND_STATUS_SUCCESS;
}

static int32_t fmc_nand_param_check(uint32_t addr, uint32_t len)
{
    if (g_nand_mtd == NULL) {
        ERR_MSG("not init g_nand_mtd!!!\n");
        return -ENODEV;
    }

    if (addr & (g_nand_mtd->writesize - 1)) {
        ERR_MSG("Write address not page aligned.\n");
        return -EINVAL;
    }

    if (len % (g_nand_mtd->writesize) != 0) {
        ERR_MSG("Attempt to write non aligned data, length:%d pagesize:%d\n",
            (uint32_t)len, g_nand_mtd->writesize);
        return -EINVAL;
    }

    if ((addr + len) > g_nand_mtd->size) {
        ERR_MSG("Attempt to write outside the flash area.\n");
        return -EINVAL;
    }

    return NAND_STATUS_SUCCESS;
}

static void fmc100_write_buf_reg(struct nand_info *nand, const uint8_t *buf, int32_t len, int32_t offset)
{
    struct nand_host *nand_host = nand->priv;
    memset_s((void *)(nand_host->membase + nand_host->pagesize), nand_host->oobsize, 0xff, nand_host->oobsize);
    memcpy_s((void *)(nand_host->membase + offset), len, (void *)buf, len);
}

static void fmc100_read_buf_reg(struct nand_info *nand, const uint8_t *buf, int32_t len, int32_t offset)
{
    struct nand_host *nand_host = nand->priv;
    memcpy_s((void *)buf, len, (void *)(nand_host->membase + offset), len);
}

static int32_t fmc_nand_write_reg_base(void *memaddr, uint64_t start, uint32_t size, uint32_t cmd)
{
    uint32_t addr = (uint32_t)start;
    uint32_t len = (uint32_t)size;
    uint32_t write_szie = g_nand_mtd->writesize;
    uint8_t *buf = (uint8_t *)memaddr;
    uint8_t bad_flag[NAND_BB_SIZE];
    int32_t ret;

    ret = fmc_nand_param_check(addr, len);
    if (ret) {
        return ret;
    }

    if (g_nand_info->get_device(g_nand_info)) {
        return NAND_STATUS_FAIL;
    }

    for (uint32_t i = 0; i < len / write_szie; i++) {
        fmc100_write_buf_reg(g_nand_info, (uint8_t *)buf, write_szie, 0);
        memset_s(bad_flag, NAND_BB_SIZE, 0xff, NAND_BB_SIZE);
        fmc100_write_buf_reg(g_nand_info, bad_flag, NAND_BB_SIZE, write_szie);

        uint32_t page = (uint32_t)(addr >> g_nand_info->dev.page_shift);
        g_nand_info->op_state = NAND_PROGING;
        fmc100_reg_write(g_nand_info, page, cmd);
        int32_t status = g_nand_info->read_status(g_nand_info);
        if ((uint32_t)status & NAND_STATUS_FAIL) {
            ERR_MSG("page[0x%0x]:program fail!\n", page);
            g_nand_info->put_device(g_nand_info);
            return -EIO;
        }

        buf += write_szie;
        addr += write_szie;
        len -= write_szie;
    }

    g_nand_info->put_device(g_nand_info);

    return NAND_STATUS_SUCCESS;
}

int32_t fmc_nand_read_reg_base(void *memaddr, uint64_t start, uint32_t size, uint32_t cmd)
{
    uint32_t addr = (uint32_t)start;
    uint32_t len = (uint32_t)size;
    uint32_t read_szie = g_nand_mtd->writesize;
    uint8_t *buf = (uint8_t *)memaddr;

    if (g_nand_mtd == NULL) {
        ERR_MSG("not init g_nand_mtd!!!\n");
        return -ENODEV;
    }

    if ((addr + len) > g_nand_mtd->size) {
        ERR_MSG("Attempt to write outside the flash area.\n");
        return -EINVAL;
    }

    if (g_nand_info->get_device(g_nand_info)) {
        return NAND_STATUS_FAIL;
    }

    memset_s((uint8_t *)(g_nand_host->membase), len, 0, len);

    while (len > 0) {
        uint32_t page = (uint32_t)(addr >> g_nand_info->dev.page_shift);
        fmc100_reg_read(g_nand_info, page, cmd);

        uint32_t col = addr & g_nand_info->dev.pagemask;
        uint32_t left_to_read = ((uint32_t)len >= read_szie) ? read_szie : len;
        fmc100_read_buf_reg(g_nand_info, buf, left_to_read, col);

        buf += read_szie;
        addr += read_szie;
        len -= read_szie;
    }

    g_nand_info->put_device(g_nand_info);

    return NAND_STATUS_SUCCESS;
}

int32_t fmc_nand_write_dma_single(void *memaddr, uint64_t start, uint32_t size)
{
    uint32_t addr = (uint32_t)start;
    uint32_t len = (uint32_t)size;
    uint32_t write_szie = g_nand_mtd->writesize;
    uint8_t *buf = (uint8_t *)memaddr;
    uint8_t bad_flag[NAND_BB_SIZE];
    int32_t ret;

    ret = fmc_nand_param_check(addr, len);
    if (ret) {
        return ret;
    }

    if (g_nand_info->get_device(g_nand_info)) {
        return NAND_STATUS_FAIL;
    }

    for (uint32_t i = 0; i < len / write_szie; i++) {
        g_nand_info->write_buf(g_nand_info, (uint8_t *)buf, write_szie, 0);
        memset_s(bad_flag, NAND_BB_SIZE, 0xff, NAND_BB_SIZE);
        g_nand_info->write_buf(g_nand_info, bad_flag, NAND_BB_SIZE, write_szie);

        uint32_t page = (uint32_t)(addr >> g_nand_info->dev.page_shift);
        g_nand_info->op_state = NAND_PROGING;
        fmc100_dma_transfer(g_nand_info, page, PROGRAM_LOAD);
        int32_t status = g_nand_info->read_status(g_nand_info);
        if ((uint32_t)status & NAND_STATUS_FAIL) {
            ERR_MSG("page[0x%0x]:program fail!\n", page);
            g_nand_info->put_device(g_nand_info);
            return -EIO;
        }

        buf += write_szie;
        addr += write_szie;
        len -= write_szie;
    }

    g_nand_info->put_device(g_nand_info);

    return NAND_STATUS_SUCCESS;
}

int32_t fmc_nand_read_dma_single(void *memaddr, uint64_t start, uint32_t size)
{
    uint32_t addr = (uint32_t)start;
    uint32_t len = (uint32_t)size;
    uint32_t read_szie = g_nand_mtd->writesize;
    uint8_t *buf = (uint8_t *)memaddr;

    if (g_nand_mtd == NULL) {
        ERR_MSG("not init g_nand_mtd!!!\n");
        return -ENODEV;
    }

    if ((addr + len) > g_nand_mtd->size) {
        ERR_MSG("Attempt to write outside the flash area.\n");
        return -EINVAL;
    }

    if (g_nand_info->get_device(g_nand_info)) {
        return NAND_STATUS_FAIL;
    }

    memset_s((uint8_t *)(g_nand_host->dma_data), len, 0, len);

    while (len > 0) {
        uint32_t page = (uint32_t)(addr >> g_nand_info->dev.page_shift);
        fmc100_dma_transfer(g_nand_info, page, READ_FROM_CACHE);

        uint32_t col = addr & g_nand_info->dev.pagemask;
        uint32_t left_to_read = (len >= read_szie) ? read_szie : len;
        g_nand_info->read_buf(g_nand_info, buf, left_to_read, col);

        buf += read_szie;
        addr += read_szie;
        len -= read_szie;
    }

    g_nand_info->put_device(g_nand_info);

    return NAND_STATUS_SUCCESS;
}

int32_t fmc_nand_write_reg_single(void *memaddr, uint64_t start, uint32_t size)
{
    return fmc_nand_write_reg_base(memaddr, start, size, PROGRAM_LOAD);
}

int32_t fmc_nand_write_reg(void *memaddr, uint64_t start, uint32_t size)
{
    return fmc_nand_write_reg_base(memaddr, start, size, PROGRAM_LOAD4);
}

int32_t fmc_nand_read_reg_single(void *memaddr, uint64_t start, uint32_t size)
{
    return fmc_nand_read_reg_base(memaddr, start, size, READ_FROM_CACHE);
}

int32_t fmc_nand_read_reg(void *memaddr, uint64_t start, uint32_t size)
{
    return fmc_nand_read_reg_base(memaddr, start, size, READ_FROM_CACHE4);
}
