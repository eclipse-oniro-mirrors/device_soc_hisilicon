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
#include "cryp_rsa.h"

/* ****************************** API Code **************************** */
/*
 * brief   Kapi Init.
 * retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 kapi_rsa_init(void)
{
    hi_s32 ret;

    hi_log_func_enter();

    ret = cryp_rsa_init();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cryp_rsa_init, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

/*
 * brief   Kapi Deinitialize.
 * retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 kapi_rsa_deinit(void)
{
    hi_log_func_enter();

    cryp_rsa_deinit();

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 kapi_rsa_encrypt(cryp_rsa_key *key, cryp_rsa_crypt_data *rsa)
{
    hi_s32 ret;

    hi_log_func_enter();

    ret = cryp_rsa_encrypt(key, rsa);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cryp_rsa_encrypt, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 kapi_rsa_decrypt(cryp_rsa_key *key, cryp_rsa_crypt_data *rsa)
{
    hi_s32 ret;

    hi_log_func_enter();

    ret = cryp_rsa_decrypt(key, rsa);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cryp_rsa_decrypt, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 kapi_rsa_sign_hash(cryp_rsa_key *key, cryp_rsa_sign_data *rsa)
{
    hi_s32 ret;

    hi_log_func_enter();

    ret = cryp_rsa_sign_hash(key, rsa);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cryp_rsa_sign_hash, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 kapi_rsa_verify_hash(cryp_rsa_key *key, cryp_rsa_sign_data *rsa)
{
    hi_s32 ret;

    hi_log_func_enter();

    ret = cryp_rsa_verify_hash(key, rsa);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cryp_rsa_verify_hash, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 kapi_rsa_bn_exp_mod(cryp_rsa_exp_mod *exp_mod)
{
    hi_s32 ret;

    hi_log_func_enter();

    ret = cryp_rsa_bn_exp_mod(exp_mod);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cryp_rsa_bn_exp_mod, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}
