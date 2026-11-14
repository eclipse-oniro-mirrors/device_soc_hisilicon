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

#include "cryp_rsa.h"
#include "drv_osal_lib.h"
#include "drv_pke.h"
#include "cryp_trng.h"
#include "crypto_pke_struct.h"
#include "drv_pke_priv.h"

#if defined(CHIP_RSA_SUPPORT)

/* rsa key len in bits */
#define RSA_BITS_1024                            1024
#define RSA_BITS_2048                            2048
#define RSA_BITS_3072                            3072
#define RSA_BITS_4096                            4096

int cryp_rsa_init(td_void)
{
    func_enter();

#ifdef CHIP_PKE_SUPPORT
    {
        td_s32 ret;
        ret = drv_pke_init();
        if (ret != TD_SUCCESS) {
            print_func_errno(drv_pke_init, ret);
            return ret;
        }
    }
#endif

    func_exit();
    return TD_SUCCESS;
}

void cryp_rsa_deinit(void)
{
#ifdef CHIP_PKE_SUPPORT
    drv_pke_deinit();
#endif
}

static td_s32 cryp_rsa_get_alg(td_u32 scheme, drv_pke_rsa_scheme *drv_scheme, drv_pke_hash_type* hash_type)
{
    switch (scheme) {
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_PKCS1_V1_5:
            *drv_scheme = DRV_PKE_RSA_SCHEME_PKCS1_V15;
            *hash_type = DRV_PKE_HASH_TYPE_SHA256;
            break;
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA256:
            *drv_scheme = DRV_PKE_RSA_SCHEME_PKCS1_V15;
            *hash_type = DRV_PKE_HASH_TYPE_SHA256;
            break;
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA384:
            *drv_scheme = DRV_PKE_RSA_SCHEME_PKCS1_V15;
            *hash_type = DRV_PKE_HASH_TYPE_SHA384;
            break;
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA512:
            *drv_scheme = DRV_PKE_RSA_SCHEME_PKCS1_V15;
            *hash_type = DRV_PKE_HASH_TYPE_SHA512;
            break;
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA256:
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA256:
            *drv_scheme = DRV_PKE_RSA_SCHEME_PKCS1_V21;
            *hash_type = DRV_PKE_HASH_TYPE_SHA256;
            break;
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA384:
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA384:
            *drv_scheme = DRV_PKE_RSA_SCHEME_PKCS1_V21;
            *hash_type = DRV_PKE_HASH_TYPE_SHA384;
            break;
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA512:
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA512:
            *drv_scheme = DRV_PKE_RSA_SCHEME_PKCS1_V21;
            *hash_type = DRV_PKE_HASH_TYPE_SHA512;
            break;
        default:
            ot_cipher_error("RSA padding mode error, mode = 0x%x.\n", scheme);
            return OT_ERR_CIPHER_INVALID_PARAM;
    }

    return TD_SUCCESS;
}

#define RSA_KEY_LEN_MAX 512

td_s32 cryp_rsa_encrypt(ot_cipher_rsa_encrypt_scheme scheme,
    const cryp_rsa_key *key, cryp_rsa_crypto_data *rsa_data)
{
    td_s32 ret;
    drv_pke_rsa_scheme drv_scheme;
    drv_pke_hash_type hash_type;
    drv_pke_rsa_pub_key pub_key;
    drv_pke_data input, output;
    td_u8 *rsa_key = NULL;
    func_enter();

    chk_ptr_err_return(key);
    chk_param_err_return(key->public == TD_FALSE);

    chk_ptr_err_return(rsa_data);
    chk_param_err_return(rsa_data->in  == TD_NULL || rsa_data->in_len == 0);
    chk_param_err_return(rsa_data->out == TD_NULL || rsa_data->out_len == 0);
    chk_param_err_return(key->klen > RSA_KEY_LEN_4096);
    chk_param_err_return(rsa_data->in_len > key->klen);
    ret = cryp_rsa_get_alg(scheme, &drv_scheme, &hash_type);
    if (ret != TD_SUCCESS) {
        print_func_errno(cryp_rsa_get_alg, ret);
        return ret;
    }

    rsa_key = crypto_malloc(RSA_KEY_LEN_MAX);
    if (rsa_key == NULL) {
        crypto_print("malloc failed\r\n");
        return -1;
    }
    (td_void)memset_s(rsa_key, RSA_KEY_LEN_MAX, 0, RSA_KEY_LEN_MAX);
    (td_void)memcpy_s(rsa_key + key->klen - sizeof(td_u32),
        RSA_KEY_LEN_MAX - (key->klen - sizeof(td_u32)), (td_u8 *)&key->e, sizeof(td_u32));

    pub_key.e = rsa_key;
    pub_key.n = key->n;
    pub_key.len = key->klen;

    input.data = rsa_data->in;
    input.length = rsa_data->in_len;

    output.data = rsa_data->out;
    output.length = rsa_data->out_len;

    ret = drv_cipher_pke_rsa_public_encrypt(drv_scheme, hash_type, &pub_key, &input, NULL, &output);
    if (ret != TD_SUCCESS) {
        print_func_errno(drv_cipher_pke_rsa_public_encrypt, ret);
        goto exit;
    }
    func_exit();
exit:
    if (rsa_key != NULL) {
        crypto_free(rsa_key);
        rsa_key = NULL;
    }
    return ret;
}

td_s32 cryp_rsa_decrypt(ot_cipher_rsa_encrypt_scheme scheme,
    const cryp_rsa_key *key, cryp_rsa_crypto_data *rsa_data)
{
    td_s32 ret;
    drv_pke_rsa_scheme drv_scheme;
    drv_pke_hash_type hash_type;
    drv_pke_rsa_priv_key priv_key;
    drv_pke_data input, output;
    func_enter();

    chk_ptr_err_return(key);
    chk_param_err_return(key->public == TD_TRUE);
    chk_ptr_err_return(rsa_data);
    chk_param_err_return(rsa_data->in  == TD_NULL || rsa_data->in_len == 0);
    chk_param_err_return(rsa_data->out == TD_NULL || rsa_data->out_len == 0);
    chk_param_err_return(key->klen > RSA_KEY_LEN_4096);
    chk_param_err_return(rsa_data->in_len != key->klen);

    ret = cryp_rsa_get_alg(scheme, &drv_scheme, &hash_type);
    if (ret != TD_SUCCESS) {
        print_func_errno(cryp_rsa_get_alg, ret);
        return ret;
    }

    priv_key.d = key->d;
    priv_key.d_len = key->klen;

    priv_key.n = key->n;
    priv_key.n_len = key->klen;

    /* CRT mode parameters */
    priv_key.dp = key->dp;
    priv_key.dq = key->dq;
    priv_key.p = key->p;
    priv_key.q = key->q;
    priv_key.qp = key->qp;

    input.data = rsa_data->in;
    input.length = rsa_data->in_len;

    output.data = rsa_data->out;
    output.length = rsa_data->out_len;

    ret = drv_cipher_pke_rsa_private_decrypt(drv_scheme, hash_type,
        &priv_key, &input, NULL, &output);
    if (ret != TD_SUCCESS) {
        print_func_errno(drv_cipher_pke_rsa_private_decrypt, ret);
        return ret;
    }
    rsa_data->out_len = output.length;
    func_exit();
    return TD_SUCCESS;
}

td_s32 cryp_rsa_sign_hash(ot_cipher_rsa_sign_scheme scheme,
    const cryp_rsa_key *key, cryp_rsa_sign_data *rsa_sign)
{
    td_s32 ret;
    drv_pke_rsa_scheme drv_scheme;
    drv_pke_hash_type hash_type;
    drv_pke_rsa_priv_key priv_key;
    drv_pke_data input, output;
    func_enter();

    chk_ptr_err_return(key);
    chk_ptr_err_return(rsa_sign);
    chk_param_err_return(rsa_sign->in  == TD_NULL || rsa_sign->in_len == 0);
    chk_param_err_return(rsa_sign->out == TD_NULL || rsa_sign->out_len == 0);
    chk_param_err_return(key->klen > RSA_KEY_LEN_4096);
    chk_param_err_return(rsa_sign->in_len > key->klen);

    ret = cryp_rsa_get_alg(scheme, &drv_scheme, &hash_type);
    if (ret != TD_SUCCESS) {
        print_func_errno(cryp_rsa_get_alg, ret);
        return ret;
    }

    priv_key.d = key->d;
    priv_key.d_len = key->klen;

    priv_key.n = key->n;
    priv_key.n_len = key->klen;

    input.data = rsa_sign->in;
    input.length = rsa_sign->in_len;

    output.data = rsa_sign->out;
    output.length = rsa_sign->out_len;

    ret = drv_cipher_pke_rsa_sign(&priv_key, drv_scheme, hash_type,
        &input, &output);
    if (ret != TD_SUCCESS) {
        print_func_errno(drv_cipher_pke_rsa_sign, ret);
        return ret;
    }
    func_exit();
    return TD_SUCCESS;
}

td_s32 cryp_rsa_verify_hash(ot_cipher_rsa_sign_scheme scheme,
    const cryp_rsa_key *key, const cryp_rsa_sign_data *rsa_verify)
{
    td_s32 ret;
    drv_pke_rsa_scheme drv_scheme;
    drv_pke_hash_type hash_type;
    drv_pke_rsa_pub_key pub_key;
    drv_pke_data input, output;
    td_u8 *rsa_key = NULL;
    func_enter();
    chk_ptr_err_return(key);
    chk_ptr_err_return(rsa_verify);
    chk_param_err_return(rsa_verify->in  == TD_NULL || rsa_verify->in_len == 0);
    chk_param_err_return(rsa_verify->out == TD_NULL || rsa_verify->out_len == 0);
    chk_param_err_return(key->klen > RSA_KEY_LEN_4096);
    chk_param_err_return(rsa_verify->in_len != key->klen);
    chk_param_err_return(rsa_verify->out_len > key->klen);

    ret = cryp_rsa_get_alg(scheme, &drv_scheme, &hash_type);
    if (ret != TD_SUCCESS) {
        print_func_errno(cryp_rsa_get_alg, ret);
        return ret;
    }

    rsa_key = crypto_malloc(RSA_KEY_LEN_MAX);
    if (rsa_key == NULL) {
        crypto_print("malloc failed\r\n");
        return -1;
    }
    (td_void)memset_s(rsa_key, RSA_KEY_LEN_MAX, 0, RSA_KEY_LEN_MAX);
    (td_void)memcpy_s(rsa_key + key->klen - sizeof(td_u32),
        RSA_KEY_LEN_MAX - (key->klen - sizeof(td_u32)), (td_u8 *)&key->e, sizeof(td_u32));

    pub_key.e = rsa_key;
    pub_key.n = key->n;
    pub_key.len = key->klen;

    input.data = rsa_verify->out;  // hash.
    input.length = rsa_verify->out_len;

    output.data = rsa_verify->in;  // sign
    output.length = rsa_verify->in_len;

    ret = drv_cipher_pke_rsa_verify(&pub_key, drv_scheme, hash_type,
        &input, &output);
    if (ret != TD_SUCCESS) {
        print_func_errno(drv_cipher_pke_rsa_verify, ret);
        goto exit;
    }
    func_exit();
exit:
    if (rsa_key != NULL) {
        crypto_free(rsa_key);
        rsa_key = NULL;
    }
    return ret;
}
#endif

/** @} */ /** <!-- ==== API Code end ==== */
