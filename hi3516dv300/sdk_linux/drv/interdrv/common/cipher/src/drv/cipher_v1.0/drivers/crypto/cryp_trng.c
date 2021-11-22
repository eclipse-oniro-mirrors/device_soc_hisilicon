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

#include "drv_osal_lib.h"
#include "drv_trng.h"
#include "cryp_trng.h"

/* the max continuous bits of randnum is allowed */
#define CONTINUOUS_BITS_ALLOWD              0x08

/* times try to read rang  */
#define RANG_READ_TRY_TIME                  0x40

/* ****************************** API Code **************************** */
#ifdef CHIP_TRNG_SUPPORT
static hi_s32 cryp_trng_check(hi_u32 randnum)
{
#ifdef CIPHER_CHECK_RNG_BY_BYTE
    static hi_u32 lastrand = 0;
    hi_u8 *byte = HI_NULL;
    hi_u32 i;

    /* compare with last rand number */
    if (randnum == lastrand) {
        return HI_ERR_CIPHER_NO_AVAILABLE_RNG;
    }

    /* update last randnum */
    lastrand = randnum;
    byte = (hi_u8 *)&randnum;

    /* continuous 8 bits0 or bit1 is prohibited */
    for (i = 0; i < WORD_WIDTH; i++) {
        /* compare with 0x00 and 0xff */
        if ((byte[i] == 0x00) || (byte[i] == 0xff)) {
            return HI_ERR_CIPHER_NO_AVAILABLE_RNG;
        }
    }
#else
    /* continuous 32 bits0 or bit1 is prohibited */
    if ((randnum == 0x00000000) || (randnum == 0xffffffff)) {
        return HI_ERR_CIPHER_NO_AVAILABLE_RNG;
    }
#endif
    return HI_SUCCESS;
}

hi_s32 cryp_trng_get_random(hi_u32 *randnum, hi_u32 timeout)
{
    hi_u32 i;
    hi_s32 ret;
    trng_capacity capacity;

    hi_log_func_enter();

    drv_trng_get_capacity(&capacity);
    if (!capacity.trng) {
        hi_log_error("error, trng nonsupport\n");
        hi_log_print_err_code(HI_ERR_CIPHER_UNSUPPORTED);
        return HI_ERR_CIPHER_UNSUPPORTED;
    }

    for (i = 0; i < RANG_READ_TRY_TIME; i++) {
        ret = drv_trng_randnum(randnum, timeout);
        if (ret != HI_SUCCESS) {
            return ret;
        }

        ret = cryp_trng_check(*randnum);
        if (ret == HI_SUCCESS) {
            break;
        }
    }

    if (i >= RANG_READ_TRY_TIME) {
        hi_log_error("error, trng randnum check failed\n");
        return HI_ERR_CIPHER_NO_AVAILABLE_RNG;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

#else
#include <linux/random.h>

hi_s32 cryp_trng_get_random(hi_u32 *randnum, hi_u32 timeout)
{
    hi_log_func_enter();

    get_random_bytes((hi_u8 *)randnum, WORD_WIDTH);

    hi_log_func_exit();
    return HI_SUCCESS;
}
#endif
