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
#define RSA_PADLEN_11              11

static int32_t rsa_decrypt(const drv_pke_rsa_scheme scheme, const drv_pke_hash_type hash_type, uint8_t *em,
    const uint32_t klen, const drv_pke_data *label, drv_pke_data *msg)
{
    int32_t ret;
    rsa_pkcs1_pack de_pack = {
        .klen = klen,
        .em_bit = 0,
        .em = em,
        .em_len = klen,
        .hash = TD_NULL,
        .hash_len = 0,
        .data = msg->data,
        .data_len = msg->length
    };

    (void)(de_pack);
    (void)(hash_type);
    (void)(label);

    switch (scheme) {
#if defined(CONFIG_PKE_RSA_V15_SUPPORT)
        case DRV_PKE_RSA_SCHEME_PKCS1_V15: {
            ret = pkcs1_v15_decrypt(&de_pack);
            crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_v15_decrypt failed, ret is 0x%x\n", ret);
            msg->length = de_pack.data_len; // record length of plain text for return
            break;
        }
#endif
#if defined(CONFIG_PKE_RSA_V21_SUPPORT)
        case DRV_PKE_RSA_SCHEME_PKCS1_V21: {
            ret = pkcs1_oaep_decrypt(hash_type, &de_pack, label);
            crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_oaep_decrypt failed, ret is 0x%x\n", ret);
            msg->length = de_pack.data_len; // record length of plain text for return
            break;
        }
#endif
        default:
            crypto_log_err("Invalid rsa decrypt padding type.\n");
            return PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM);
    }
    return ret;
}

static int32_t pke_rsa_decrypt(
    drv_pke_rsa_scheme scheme,
    drv_pke_hash_type hash_type,
    const drv_pke_rsa_priv_key *priv_key,
    const drv_pke_data *input,
    const drv_pke_data *label,
    drv_pke_data *output)
{
    int32_t ret = -1;
    uint8_t *em = NULL;
    uint32_t klen = priv_key->n_len;
    rsa_key_width width;

    em = crypto_malloc_coherent(klen);
    crypto_chk_return(em == NULL, ERROR_MALLOC, "malloc failed\n");
    (void)memset_s(em, klen, 0x00, klen);

    width = klen_to_width(klen);
    crypto_chk_goto(width == RSA_KEY_WIDTH_COUNT, exit_free, "klen_to_width get width = %x\n", width);

    ret = drv_ifep_rsa_exp_mod(priv_key->n, priv_key->d, input->data, em, width);
    crypto_chk_goto(ret != TD_SUCCESS, exit_free, "drv_cipher_pke_exp_mod failed, ret is 0x%x\n", ret);

    if (memcmp(input->data, em, klen) == 0) {
        ret = PKE_COMPAT_ERRNO(ERROR_PKE_RSA_SAME_DATA);
        goto exit_free;
    }

    ret = rsa_decrypt(scheme, hash_type, em, klen, label, output);
    crypto_chk_goto(ret != TD_SUCCESS, exit_free, "rsa_decrypt failed, ret is 0x%x\n", ret);

    ret = CRYPTO_SUCCESS;
exit_free:
    (void)memset_s(em, klen, 0x00, klen);
    crypto_free_coherent(em);
    return ret;
}

int32_t drv_cipher_pke_rsa_private_decrypt(
    const drv_pke_rsa_scheme scheme,
    const drv_pke_hash_type hash_type,
    const drv_pke_rsa_priv_key *priv_key,
    const drv_pke_data *input,
    const drv_pke_data *label,
    drv_pke_data *output)
{
    int32_t ret;

    uint32_t klen = 0;
    uint32_t hash_len = 0;
    /* check ptr. */
    pke_null_ptr_chk(priv_key);
    pke_null_ptr_chk(priv_key->n);
    if (priv_key->d == TD_NULL && priv_key->p == TD_NULL && priv_key->q == TD_NULL &&
        priv_key->dp == TD_NULL && priv_key->dq == TD_NULL && priv_key->qp == TD_NULL) {
        crypto_log_err("All RSA private key params are null.\n");
        return PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM);
    }
    pke_null_ptr_chk(input);
    pke_null_ptr_chk(input->data);
    pke_null_ptr_chk(output);
    pke_null_ptr_chk(output->data);
    if (label != TD_NULL) {
        pke_null_ptr_chk(label->data);
    }

    /* check length. */
    klen = priv_key->n_len;
    hash_len = drv_get_hash_len(hash_type);
    crypto_chk_return(hash_len == 0, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "hash_type is Invalid\n");

    crypto_chk_return(klen != DRV_PKE_LEN_1024 && klen != DRV_PKE_LEN_2048 && klen != DRV_PKE_LEN_3072 &&
        klen != DRV_PKE_LEN_4096, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "klen is Invalid\n");
    crypto_chk_return(scheme == DRV_PKE_RSA_SCHEME_PKCS1_V21 && klen == DRV_PKE_LEN_1024 &&
        hash_type == DRV_PKE_HASH_TYPE_SHA512, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "wrong alg combine\n");
    crypto_chk_return(priv_key->d_len != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "d_len is Invalid\n");
    crypto_chk_return(input->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "input_len is Invalid\n");

    ret = pke_rsa_decrypt(scheme, hash_type, priv_key, input, label, output);
    return ret;
}
