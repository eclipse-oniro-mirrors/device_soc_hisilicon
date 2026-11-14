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

static int32_t rsa_encrypt(
    const drv_pke_rsa_scheme scheme,
    const drv_pke_hash_type hash_type,
    uint8_t *em,
    const uint32_t klen,
    const drv_pke_data *msg,
    const drv_pke_data *label)
{
    int32_t ret;
    rsa_pkcs1_pack en_pack = {
        .klen = klen,
        .em_bit = 0,
        .em = em,
        .em_len = klen,
        .hash = TD_NULL,
        .hash_len = 0,
        .data = msg->data,
        .data_len = msg->length
    };

    (void)(en_pack);
    (void)(hash_type);
    (void)(label);

    switch (scheme) {
#if defined(CONFIG_PKE_RSA_V15_SUPPORT)
        case DRV_PKE_RSA_SCHEME_PKCS1_V15: {
            ret = pkcs1_v15_encrypt(&en_pack);
            crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_v15_encrypt failed, ret is 0x%x\n", ret);
            break;
        }
#endif
#if defined(CONFIG_PKE_RSA_V21_SUPPORT)
        case DRV_PKE_RSA_SCHEME_PKCS1_V21: {
            ret = pkcs1_oaep_encrypt(hash_type, &en_pack, label);
            crypto_chk_return(ret != TD_SUCCESS, ret, "pkcs1_oaep_encrypt failed, ret is 0x%x\n", ret);
            break;
        }
#endif
        default:
            crypto_log_err("Invalid rsa encrypt padding type.\n");
            return PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM);
    }
    return ret;
}

static int32_t pke_rsa_encrypt(
    drv_pke_rsa_scheme scheme,
    drv_pke_hash_type hash_type,
    const drv_pke_rsa_pub_key *pub_key,
    const drv_pke_data *input,
    const drv_pke_data *label,
    const drv_pke_data *output)
{
    int32_t ret;
    uint8_t *em = NULL;
    uint32_t klen = pub_key->len;
    rsa_key_width width;

    em = crypto_malloc_coherent(klen);
    crypto_chk_return(em == NULL, ERROR_MALLOC, "malloc failed\n");
    (void)memset_s(em, klen, 0x00, klen);

    ret = rsa_encrypt(scheme, hash_type, em, klen, input, label);
    crypto_chk_goto(ret != TD_SUCCESS, exit_free, "rsa_encrypt failed, ret is 0x%x\n", ret);

    width = klen_to_width(klen);
    crypto_chk_goto(width == RSA_KEY_WIDTH_COUNT, exit_free, "klen_to_width get width = %x\n", width);

    ret = drv_ifep_rsa_exp_mod(pub_key->n, pub_key->e, em, output->data, width);
    crypto_chk_goto(ret != TD_SUCCESS, exit_free, "drv_ifep_rsa_exp_mod failed, ret is 0x%x\n", ret);
    if (memcmp(em, output->data, klen) == 0) {
        ret = PKE_COMPAT_ERRNO(ERROR_PKE_RSA_SAME_DATA);
        goto exit_free;
    }

    ret = CRYPTO_SUCCESS;
exit_free:
    (void)memset_s(em, klen, 0x00, klen);
    crypto_free_coherent(em);
    return ret;
}

int32_t drv_cipher_pke_rsa_public_encrypt(
    const drv_pke_rsa_scheme scheme,
    const drv_pke_hash_type hash_type,
    const drv_pke_rsa_pub_key *pub_key,
    const drv_pke_data *input,
    const drv_pke_data *label,
    drv_pke_data *output)
{
    int32_t ret;
    uint32_t klen = 0;
    uint32_t hash_len = 0;
    uint32_t input_len = 0;
    /* check ptr. */
    pke_null_ptr_chk(pub_key);
    pke_null_ptr_chk(pub_key->n);
    pke_null_ptr_chk(pub_key->e);
    pke_null_ptr_chk(input);
    pke_null_ptr_chk(input->data);
    pke_null_ptr_chk(output);
    pke_null_ptr_chk(output->data);
    if (label != TD_NULL) {
        pke_null_ptr_chk(label->data);
    }

    /* check length. */
    klen = pub_key->len;
    input_len = input->length;
    hash_len = drv_get_hash_len(hash_type);
    crypto_chk_return(hash_len == 0, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "hash_type is Invalid\n");

    crypto_chk_return(klen != DRV_PKE_LEN_1024 && klen != DRV_PKE_LEN_2048 && klen != DRV_PKE_LEN_3072 &&
        klen != DRV_PKE_LEN_4096, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "klen is Invalid\n");
    /*
     * For V15, the plain_text's max length is klen - 11.
     * For OAEP, the plain_text's max length is klen - 2 * hash_len - 2.
     */
    if (scheme == DRV_PKE_RSA_SCHEME_PKCS1_V15) {
        crypto_chk_return(input_len > klen - RSA_PADLEN_11, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
            "input_len is too long for V15\n");
    } else {
        crypto_chk_return(input_len > klen - 2 * hash_len - 2,  // 2: refer to comment.
            PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "input_len is too long for V21\n");
    }
    crypto_chk_return(input_len == 0, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "input_len is zero\n");
    crypto_chk_return(output->length < klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "output_len is Invalid\n");

    ret = pke_rsa_encrypt(scheme, hash_type, pub_key, input, label, output);
    if (ret == TD_SUCCESS) {
        output->length = klen;
    }

    return ret;
}
