/*
 * Copyright (C) 2021 HiSilicon (Shanghai) Technologies CO., LIMITED.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "drv_ifep_rsa_v100.h"
#include "drv_srsa.h"
#include "drv_trng.h"
#include "drv_klad.h"
#include "securec.h"

#ifdef CHIP_IFEP_RSA_VER_V100

/* Define the time out */
#define RSA_TIME_OUT              5000

/* rsa support rand mask or not */
#define RSA_SUB_VER_RAND_MASK     0x20160907
#define RSA_SUB_VER_NORMAL        0x00

/* crc 16 */
#define CRC16_TABLE_SIZE          256
#define U16_MSB                   0x8000
#define BYTE_MASK                 0xFF
#define CRC16_POLYNOMIAL          0x1021
#define BLOCK_BYTES               0x08

/* rsa work mode */
#define RSA_MODE_EXP              0x00
#define RSA_MODE_CLEAR_RAM        0x02

/* rsa already initialize or not */
static hi_u32 g_rsa_initialize = HI_FALSE;

/* Define the context of rsa */
typedef struct {
    hi_u32 rsa_sub_ver;
    hi_u32 done;                    /* calculation finish flag. */
    CRYPTO_QUEUE_HEAD  queue;       /* queue list */
} rsa_hard_context;

static rsa_hard_context g_rsa_info;
/* ****************************** API Declaration **************************** */
#ifdef RSA_RAND_MASK

static hi_u16 g_crc_table[CRC16_TABLE_SIZE] = {0};

static hi_void drv_rsa_crc16_init(hi_void)
{
    hi_u16 remainder;
    hi_u16 n, m;
    hi_u16 *local_crc_table = g_crc_table;

    for (n = 0; n < CRC16_TABLE_SIZE; n++) {
        remainder = (hi_u16)n << BITS_IN_BYTE;
        for (m = BITS_IN_BYTE; m > 0; m--) {
            if (remainder & U16_MSB) {
                remainder = (remainder << 1) ^ CRC16_POLYNOMIAL;
            } else {
                remainder = (remainder << 1);
            }
        }
        *(local_crc_table + n) = remainder;
    }
}

static hi_u16 drv_rsa_crc16_block(hi_u16 crc,
    const hi_u8 block[BLOCK_BYTES], const hi_u8 rand_num[BLOCK_BYTES])
{
    hi_u8 i, j;
    hi_u8 val;

    for (i = 0; i < BLOCK_BYTES / WORD_WIDTH; i++) {
        for (j = 0; j < WORD_WIDTH; j++) {
            val = block[i * WORD_WIDTH + WORD_IDX_3 - j] ^ rand_num[i * WORD_WIDTH + WORD_IDX_3 - j];
            crc = (crc << BITS_IN_BYTE) ^ g_crc_table[((crc >> BITS_IN_BYTE) ^ val) & BYTE_MASK];
        }
    }
    return crc;
}

static hi_u16 drv_rsa_key_crc(const hi_u8 *n, const hi_u8 *k, hi_u32 klen, const hi_u32 randnum[MUL_VAL_2])
{
    hi_u32 i;
    hi_u16 crc = 0;

    for (i = 0; i < klen; i += BLOCK_BYTES) {
        crc = drv_rsa_crc16_block(crc, n + i, (hi_u8 *)randnum);
    }
    for (i = 0; i < klen; i += BLOCK_BYTES) {
        crc = drv_rsa_crc16_block(crc, k + i, (hi_u8 *)randnum);
    }
    return crc;
}
#endif

#ifdef CRYPTO_OS_INT_SUPPORT

/* set interrupt */
static hi_void rsa_set_interrupt(hi_void)
{
    hi_u32 int_valid = 0;
    hi_u32 int_num = 0;
    sec_rsa_int_en int_en;

    module_get_attr(CRYPTO_MODULE_ID_IFEP_RSA, &int_valid, &int_num, HI_NULL);
    if (int_valid == HI_FALSE) {
        return;
    }
    if (g_rsa_info.rsa_sub_ver != RSA_SUB_VER_RAND_MASK) {
        return;
    }

    int_en.u32 = ifep_rsa_read(REG_SEC_RSA_INT_EN);

    /* The top interrupt switch only can be enable/disable by secure CPU. */
    int_en.bits.rsa_int_en = 1;
    int_en.bits.int_en = 1;
    ifep_rsa_write(REG_SEC_RSA_INT_EN, int_en.u32);
    hi_log_info("RSA_INT_EN: 0x%x\n", int_en.u32);

    return;
}

static hi_void drv_rsa_done_notify(hi_void)
{
    sec_rsa_int_status int_st;
    sec_rsa_int_raw int_raw;

    int_st.u32 = ifep_rsa_read(REG_SEC_RSA_INT_STATUS);
    int_raw.u32 = 0x00;

    hi_log_debug("REG_SEC_RSA_INT_STATUS: 0x%x\n", int_st.u32);
    crypto_unused(int_st.u32);

    /* Clean raw interrupt. */
    int_raw.bits.rsa_int_raw = RSA_INT_RAW_CLR;
    ifep_rsa_write(REG_SEC_RSA_INT_RAW, int_raw.u32);

    /* Clean error interrupt. */
    ifep_rsa_write(REG_SEC_RSA_INT_ERR_CLR, RSA_INT_ERR_CLR);
}

/* rsa interrupt process function */
static CRYPTO_IRQRETURN_T drv_rsa_interrupt_isr(hi_s32 irq, hi_void *dev_id)
{
    crypto_unused(irq);
    crypto_unused(dev_id);

    drv_rsa_done_notify();

    g_rsa_info.done = HI_TRUE;
    hi_log_debug("rsa wake up\n");
    crypto_queue_wait_up(&g_rsa_info.queue);

    return IRQ_HANDLED;
}

/* rsa register interrupt process function */
static hi_s32 drv_rsa_register_interrupt(hi_void)
{
    hi_s32 ret;
    hi_u32 int_valid = 0;
    hi_u32 int_num = 0;
    const char *name = HI_NULL;

    hi_log_func_enter();

    module_get_attr(CRYPTO_MODULE_ID_IFEP_RSA, &int_valid, &int_num, &name);
    if (int_valid == HI_FALSE) {
        return HI_SUCCESS;
    }
    if (g_rsa_info.rsa_sub_ver != RSA_SUB_VER_RAND_MASK) {
        return HI_SUCCESS;
    }

    /* request irq */
    ret = crypto_request_irq(int_num, drv_rsa_interrupt_isr, name);
    if (ret != HI_SUCCESS) {
        hi_log_error("Irq request failure, irq = %u", int_num);
        hi_log_print_err_code(HI_ERR_CIPHER_REGISTER_IRQ);
        return ret;
    }

    /* initialize queue list. */
    crypto_queue_init(&g_rsa_info.queue);

    hi_log_func_exit();
    return HI_SUCCESS;
}

/* rsa unregister interrupt process function */
static hi_void drv_rsa_unregister_interrupt(hi_void)
{
    hi_u32 int_valid = 0;
    hi_u32 int_num = 0;
    const char *name = HI_NULL;

    hi_log_func_enter();

    module_get_attr(CRYPTO_MODULE_ID_IFEP_RSA, &int_valid, &int_num, &name);

    if (int_valid == HI_FALSE) {
        return;
    }

    if (g_rsa_info.rsa_sub_ver != RSA_SUB_VER_RAND_MASK) {
        return;
    }

    /* free irq */
    hi_log_info("rsa free irq, num %u, name %s\n", int_num, name);
    crypto_free_irq(int_num, name);

    hi_log_func_exit();
}
#endif

hi_s32 drv_rsa_init(hi_void)
{
    hi_log_func_enter();

    hi_log_info("enable rsa\n");

    if (g_rsa_initialize == HI_TRUE) {
        return HI_SUCCESS;
    }

#ifdef RSA_RAND_MASK
    drv_rsa_crc16_init();
#endif

    module_enable(CRYPTO_MODULE_ID_IFEP_RSA);

    /* RSA request the TRNG must valid */
    module_enable(CRYPTO_MODULE_ID_TRNG);

    g_rsa_info.rsa_sub_ver = ifep_rsa_read(REG_SEC_RSA_VERSION_ID);
    hi_log_info("rsa version 0x%x\n", g_rsa_info.rsa_sub_ver);
    module_disable(CRYPTO_MODULE_ID_IFEP_RSA);

#ifdef CRYPTO_OS_INT_SUPPORT
    {
        hi_s32 ret;

        hi_log_info("rsa register interrupt function\n");
        ret = drv_rsa_register_interrupt();
        if (ret != HI_SUCCESS) {
            hi_log_error("error, register interrupt failed\n");
            hi_log_print_func_err(drv_rsa_register_interrupt, ret);
            return ret;
        }
    }
#endif

    g_rsa_initialize = HI_TRUE;

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 drv_rsa_deinit(hi_void)
{
    hi_log_func_enter();

    if (g_rsa_initialize == HI_FALSE) {
        return HI_SUCCESS;
    }

#ifdef CRYPTO_OS_INT_SUPPORT
    drv_rsa_unregister_interrupt();
#endif

    g_rsa_initialize = HI_FALSE;

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_void drv_rsa_resume(hi_void)
{
    hi_log_func_enter();

    module_enable(CRYPTO_MODULE_ID_IFEP_RSA);

#ifdef CRYPTO_OS_INT_SUPPORT
    hi_log_info("RSA interrupt configure\n");
    rsa_set_interrupt();
#endif

    hi_log_func_exit();
    return;
}

static hi_void drv_rsa_suspend(hi_void)
{
    hi_log_func_enter();

    module_disable(CRYPTO_MODULE_ID_IFEP_RSA);

    hi_log_func_exit();
    return;
}

static hi_void drv_rsa_set_width(rsa_key_width width)
{
    sec_rsa_mod_reg ctrl;

    ctrl.u32 = 0x00;
    ctrl.bits.sec_rsa_mod_sel = RSA_MODE_EXP;
    ctrl.bits.sec_rsa_key_width = width;
    ifep_rsa_write(REG_SEC_RSA_MOD_REG, ctrl.u32);
    hi_log_info("REG_SEC_RSA_MOD_REG 0x%x\n", ctrl.u32);
    return;
}

static hi_s32 drv_rsa_set_key(hi_u32 ca_type,
    const hi_u8 *n, const hi_u8 *d, hi_u32 klen, const hi_u32 random[MUL_VAL_2])
{
    hi_u32 i;
    hi_u32 id = 0;
    hi_u32 val = 0x00;
    hi_s32 ret;

    /*
     * The even word shell XOR with even random[0]
     * The odd word shell XOR with odd random[1]
     * The random may be zero.
     * Must set N before set E.
     * The E must padding with zero.
     */
    for (i = 0; i < klen; i += WORD_WIDTH) {
        /* Set N */
        if (memcpy_s(&val, sizeof(hi_u32), n + i, WORD_WIDTH) != EOK) {
            hi_log_print_func_err(memcpy_s, HI_ERR_CIPHER_MEMCPY_S_FAILED);
            return HI_ERR_CIPHER_MEMCPY_S_FAILED;
        }
        val ^= random[id];
        ifep_rsa_write(REG_SEC_RSA_WSEC_REG, val);

        /* switch between even and odd */
        id ^= 0x01;
    }

    /* Set D */
    if (ca_type != HI_CIPHER_KEY_SRC_USER) {
        ret = drv_cipher_klad_load_key(0, ca_type, HI_CIPHER_KLAD_TARGET_RSA, d, klen);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(drv_cipher_klad_load_key, ret);
            return ret;
        }
    } else {
        for (i = 0; i < klen; i += WORD_WIDTH) {
            if (memcpy_s(&val, sizeof(hi_u32), d + i, WORD_WIDTH) != EOK) {
                hi_log_print_func_err(memcpy_s, HI_ERR_CIPHER_MEMCPY_S_FAILED);
                return HI_ERR_CIPHER_MEMCPY_S_FAILED;
            }
            val ^= random[id];
            ifep_rsa_write(REG_SEC_RSA_WSEC_REG, val);

            /* switch between even and odd */
            id ^= 0x01;
        }
    }
    return HI_SUCCESS;
}

static hi_void drv_rsa_set_input(const hi_u8 *in, hi_u32 klen)
{
    hi_u32 i;
    hi_u32 val = 0x00;

    for (i = 0; i < klen; i += WORD_WIDTH) {
        if (memcpy_s(&val, sizeof(hi_u32), in + i, WORD_WIDTH) != EOK) {
            hi_log_print_func_err(memcpy_s, HI_ERR_CIPHER_MEMCPY_S_FAILED);
            return;
        }
        ifep_rsa_write(REG_SEC_RSA_WDAT_REG, val);
    }
    return;
}

static hi_void drv_rsa_get_output(hi_u8 *out, hi_u32 klen)
{
    hi_u32 i;
    hi_u32 val;

    for (i = 0; i < klen; i += WORD_WIDTH) {
        val = ifep_rsa_read(REG_SEC_RSA_RRSLT_REG);
        if (memcpy_s(out + i, sizeof(hi_u32), &val, sizeof(hi_u32)) != EOK) {
            hi_log_print_func_err(memcpy_s, HI_ERR_CIPHER_MEMCPY_S_FAILED);
            return;
        }
    }
}

static hi_u32 drv_rsa_get_klen(rsa_key_width width)
{
    hi_u32 klen;

    /* nonsupport rsa 3072, can compute it as 4096 */
    switch (width) {
        case RSA_KEY_WIDTH_1024: {
            klen = RSA_KEY_LEN_1024;
            break;
        }
        case RSA_KEY_WIDTH_2048: {
            klen = RSA_KEY_LEN_2048;
            break;
        }
        case RSA_KEY_WIDTH_3072: {
            klen = RSA_KEY_LEN_3072;
            break;
        }
        case RSA_KEY_WIDTH_4096: {
            klen = RSA_KEY_LEN_4096;
            break;
        }
        default: {
            hi_log_error("error, nonsupport RSA width %d\n", width);
            klen = 0;
            break;
        }
    }

    return klen;
}

static hi_void drv_rsa_start(hi_void)
{
    sec_rsa_start_reg start;

    hi_log_func_enter();

    g_rsa_info.done = HI_FALSE;

    start.u32 = 0x00;

    if (g_rsa_info.rsa_sub_ver == RSA_SUB_VER_RAND_MASK) {
        start.bits.sec_rsa_start_reg = 0x05;
    } else {
        start.bits.sec_rsa_start_reg = 0x01;
    }

    ifep_rsa_write(REG_SEC_RSA_START_REG, start.u32);
    hi_log_info("REG_SEC_RSA_START_REG 0x%x\n", start.u32);

    hi_log_func_exit();

    return;
}

static hi_s32 drv_rsa_wait_done(hi_void)
{
    hi_u32 int_valid = 0;
    hi_u32 int_num = 0;
    hi_u32 i;
    sec_rsa_busy_reg ready;
    const char *name = HI_NULL;

    hi_log_func_enter();

    module_get_attr(CRYPTO_MODULE_ID_IFEP_RSA, &int_valid, &int_num, &name);

#ifdef CRYPTO_OS_INT_SUPPORT
    /* interrupt support, wait irq. */
    if ((g_rsa_info.rsa_sub_ver == RSA_SUB_VER_RAND_MASK) && (int_valid == HI_TRUE)) {
        hi_s32 ret;

        /* wait interrupt */
        ret = crypto_queue_wait_timeout(g_rsa_info.queue, &g_rsa_info.done, RSA_TIME_OUT);
        if ((ret <= 0x00) && (ret != -ERESTARTSYS)) {
            hi_log_error("wait done timeout\n");
            hi_log_print_func_err(crypto_queue_wait_timeout, ret);
            return HI_ERR_CIPHER_TIMEOUT;
        }
    } else {
        /* wait ready */
        for (i = 0; i < RSA_TIME_OUT; i++) {
            ready.u32 = ifep_rsa_read(REG_SEC_RSA_BUSY_REG);
            if (!ready.bits.sec_rsa_busy_reg) {
                break;
            }
            crypto_msleep(1);
        }

        if (i >= RSA_TIME_OUT) {
            hi_log_error("error, rsa wait free timeout\n");
            hi_log_print_err_code(HI_ERR_CIPHER_TIMEOUT);
            return HI_ERR_CIPHER_TIMEOUT;
        }
    }
#else
        /* wait ready */
        for (i = 0; i < RSA_TIME_OUT; i++) {
            ready.u32 = ifep_rsa_read(REG_SEC_RSA_BUSY_REG);
            if (!ready.bits.sec_rsa_busy_reg) {
                break;
            }
            crypto_msleep(1);
        }

        if (i >= RSA_TIME_OUT) {
            hi_log_error("error, rsa wait free timeout\n");
            hi_log_print_err_code(HI_ERR_CIPHER_TIMEOUT);
            return HI_ERR_CIPHER_TIMEOUT;
        }
#endif
    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_void drv_rsa_randnum(const hi_u8 *n, const hi_u8 *k, hi_u32 klen, hi_u32 random[MUL_VAL_2])
{
    hi_log_func_enter();

#ifdef RSA_RAND_MASK
    if (g_rsa_info.rsa_sub_ver == RSA_SUB_VER_RAND_MASK) {
        hi_u16 crc16;

        random[0] = get_rand();
        random[1] = get_rand();

        crc16 = drv_rsa_key_crc(n, k, klen, random);
        hi_log_info("random 0x%x 0x%x, CRC16: 0x%x\n", random[WORD_IDX_0], random[WORD_IDX_1], crc16);
        ifep_rsa_write(REG_SEC_RSA_KEY_RANDOM_1, random[WORD_IDX_0]);
        ifep_rsa_write(REG_SEC_RSA_KEY_RANDOM_2, random[WORD_IDX_1]);
        ifep_rsa_write(REG_SEC_RSA_CRC16_REG, crc16);
    } else {
        random[WORD_IDX_0] = 0x00;
        random[WORD_IDX_1] = 0x00;
    }
#else
    crypto_unused(n);
    crypto_unused(k);
    crypto_unused(klen);
    random[WORD_IDX_0] = 0x00;
    random[WORD_IDX_1] = 0x00;
#endif
    hi_log_func_exit();
}

static hi_s32 drv_rsa_clean_ram(hi_void)
{
    hi_s32 ret;
    sec_rsa_mod_reg ctrl;

    ctrl.u32 = ifep_rsa_read(REG_SEC_RSA_MOD_REG);
    ctrl.bits.sec_rsa_mod_sel = RSA_MODE_CLEAR_RAM;
    ctrl.bits.sec_rsa_data0_clr = RSA_CLR_RAM_STORED_KEY;
    ctrl.bits.sec_rsa_data1_clr = RSA_CLR_RAM_STORED_MSG;
    ctrl.bits.sec_rsa_data2_clr = RSA_CLR_RAM_STORED_RESULT;
    ifep_rsa_write(REG_SEC_RSA_MOD_REG, ctrl.u32);

    /* start */
    drv_rsa_start();

    /* wait done */
    ret = drv_rsa_wait_done();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_rsa_wait_done, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_rsa_error_code(hi_void)
{
    hi_log_func_enter();

    if (g_rsa_info.rsa_sub_ver == RSA_SUB_VER_RAND_MASK) {
        hi_u32 code;

        code = ifep_rsa_read(REG_SEC_RSA_ERROR_REG);
        if (code != 0) {
            hi_log_error("rsa error code: 0x%x.\n", code);
            hi_log_print_err_code(HI_ERR_CIPHER_HARD_STATUS);
            return HI_ERR_CIPHER_HARD_STATUS;
        }
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 drv_ifep_rsa_exp_mod(hi_u32 ca_type, const hi_u8 *n, const hi_u8 *k,
    const hi_u8 *in, hi_u8 *out, rsa_key_width width)
{
    hi_s32 ret, ret_exit;
    hi_u32 klen;
    hi_u32 random[MUL_VAL_2] = {0, 0};

    hi_log_func_enter();

    hi_log_chk_init_err_return(g_rsa_initialize);

    klen = drv_rsa_get_klen(width);
    if (klen == 0) {
        return HI_ERR_CIPHER_INVALID_PARAM;
    }

    drv_rsa_resume();

    /* set rsa width */
    drv_rsa_set_width(width);

    /* config randnum */
    drv_rsa_randnum(n, k, klen, random);

    /* set rsa key */
    ret = drv_rsa_set_key(ca_type, n, k, klen, random);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_rsa_set_key, ret);
        ret = HI_ERR_CIPHER_ILLEGAL_KEY;
        goto exit;
    }

    /* set input data */
    drv_rsa_set_input(in, klen);

    /* start */
    drv_rsa_start();

    /* wait done */
    ret = drv_rsa_wait_done();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_rsa_wait_done, ret);
        ret = HI_ERR_CIPHER_TIMEOUT;
        goto exit;
    }

    /* get input data */
    drv_rsa_get_output(out, klen);

    ret = drv_rsa_error_code();
exit:
    /* clean key and data */
    ret_exit = drv_rsa_clean_ram();
    if (ret_exit != HI_SUCCESS) {
        hi_log_print_func_err(drv_rsa_clean_ram, ret_exit);
    }

    drv_rsa_suspend();

    hi_log_func_exit();

    return ret;
}

hi_void drv_ifep_rsa_get_capacity(rsa_capacity *capacity)
{
    (hi_void)memset_s(capacity, sizeof(rsa_capacity), 0,  sizeof(rsa_capacity));

    capacity->rsa = CRYPTO_CAPACITY_SUPPORT;

    return;
}
#endif /* End of CHIP_RSA_VER_V100 */
