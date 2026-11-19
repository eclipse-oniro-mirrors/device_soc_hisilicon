/*
 * Copyright (c) 2025 HiSilicon (Shanghai) Technologies Co., Ltd.
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
 * along with this program; if not, see
 * <https://www.gnu.org/licenses/>.
 */
#include "drv_pke_priv.h"

static int rsa_sign(
    drv_pke_rsa_scheme scheme,
    drv_pke_hash_type hash_type,
    unsigned char *em,
    unsigned int klen,
    unsigned int em_bit,
    const drv_pke_data *msg)
{
    int ret = CRYPTO_FAILURE;
    rsa_pkcs1_pack sg_pack = {
        .klen = klen,
        .em_bit = em_bit,
        .em = em,
        .em_len = klen,
        .hash = msg->data,
        .hash_len = msg->length,
        .data = TD_NULL,
        .data_len = 0
    };

    crypto_unused(sg_pack);
    crypto_unused(hash_type);

    switch (scheme) {
#if defined(CONFIG_PKE_RSA_V15_SUPPORT)
        case DRV_PKE_RSA_SCHEME_PKCS1_V15: {
            ret = pkcs1_v15_sign(hash_type, &sg_pack);
            crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_v15_sign failed, ret is 0x%x\n", ret);
            break;
        }
#endif
#if defined(CONFIG_PKE_RSA_V21_SUPPORT)
        case DRV_PKE_RSA_SCHEME_PKCS1_V21: {
            ret = pkcs1_pss_sign(hash_type, &sg_pack);
            crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_pss_sign failed, ret is 0x%x\n", ret);
            break;
        }
#endif
        default:
            crypto_log_err("Invalid rsa sign padding type.\n");
            return PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM);
    }
    return ret;
}

static int inner_pke_rsa_sign(
    const drv_pke_rsa_priv_key *priv_key,
    drv_pke_rsa_scheme scheme,
    drv_pke_hash_type hash_type,
    const drv_pke_data *input_hash,
    const drv_pke_data *sign)
{
    int ret;
    unsigned char *em;
    unsigned int klen = priv_key->n_len;
    unsigned int em_bit = 0;
    drv_pke_data n_data = { .data = priv_key->n, .length = klen };
    drv_pke_data d_data = { .data = priv_key->d, .length = klen };
    rsa_key_width width;

    drv_pke_data em_data = { .length = klen };
    em = crypto_malloc_coherent(DRV_PKE_LEN_4096);
    crypto_chk_return(em == NULL, PKE_COMPAT_ERRNO(ERROR_MALLOC), "malloc failed\n");
    (void)memset_s(em, DRV_PKE_LEN_4096, 0x00, DRV_PKE_LEN_4096);

    em_bit = rsa_get_bit_num((const unsigned char *)priv_key->n, priv_key->n_len);
    ret = rsa_sign(scheme, hash_type, em, klen, em_bit, input_hash);
    crypto_chk_goto_with_ret(ret, ret != TD_SUCCESS, exit_free, ret, "rsa_sign failed, ret is 0x%x\n", ret);

    em_data.data = em;

    width = klen_to_width(klen);
    crypto_chk_goto(width == RSA_KEY_WIDTH_COUNT, exit_free, "klen_to_width get width = %x\n", width);

    ret = drv_ifep_rsa_exp_mod(n_data.data, d_data.data, em_data.data, sign->data, width);
    crypto_chk_goto_with_ret(ret, ret != TD_SUCCESS, exit_free, ret, "exp_mod failed, ret is 0x%x\n", ret);

    if (memcmp(em_data.data, sign->data, klen) == 0) {
        ret = PKE_COMPAT_ERRNO(ERROR_PKE_RSA_SAME_DATA);
        goto exit_free;
    }

    ret = CRYPTO_SUCCESS;
exit_free:
    if (em != TD_NULL) {
        (void)memset_s(em, DRV_PKE_LEN_4096, 0, DRV_PKE_LEN_4096);
        crypto_free_coherent(em);
    }
    return ret;
}

int drv_cipher_pke_rsa_sign(
    const drv_pke_rsa_priv_key *priv_key,
    const drv_pke_rsa_scheme scheme,
    const drv_pke_hash_type hash_type,
    const drv_pke_data *input_hash,
    const drv_pke_data *sign)
{
    int ret;
    unsigned int klen = 0;
    unsigned int hash_len = 0;
    /* check ptr. */
    pke_null_ptr_chk(priv_key);
    pke_null_ptr_chk(priv_key->n);
    pke_null_ptr_chk(priv_key->d);
    pke_null_ptr_chk(input_hash);
    pke_null_ptr_chk(input_hash->data);
    pke_null_ptr_chk(sign);
    pke_null_ptr_chk(sign->data);

    /* check length. */
    klen = priv_key->n_len;
    hash_len = drv_get_hash_len(hash_type);
    crypto_chk_return(input_hash->length != hash_len, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "input_hash->length is Invalid\n");

    crypto_chk_return(klen != DRV_PKE_LEN_1024 && klen != DRV_PKE_LEN_2048 && klen != DRV_PKE_LEN_3072 &&
        klen != DRV_PKE_LEN_4096, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "n_len is Invalid\n");
    crypto_chk_return(priv_key->d_len != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "d_len is Invalid!\n");
    crypto_chk_return(sign->length < klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "sign_len is Invalid\n");

    crypto_chk_return(crypto_pke_rsa_hash_len_check(klen, hash_type) != CRYPTO_SUCCESS,
        PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "invalid hash_type!\n");

    ret = inner_pke_rsa_sign(priv_key, scheme, hash_type, input_hash, sign);
    return ret;
}
