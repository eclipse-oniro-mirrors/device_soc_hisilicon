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

/* According to RFC2313. */
#include "drv_pke_priv.h"

#define RSA_PADLEN_1                1
#define RSA_PADLEN_3                3
#define RSA_PADLEN_11               11

/* EM = 00 || 01 || PS || 00 || T */
int pkcs1_v15_sign(const drv_pke_hash_type hash_type, const rsa_pkcs1_pack *pack)
{
    int ret;
    rsa_pkcs1_hash_info hash_info = {0};
    unsigned char *p = NULL;
    unsigned int idx = 0;

    ret = pkcs1_get_hash(hash_type, NULL, &hash_info);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "pkcs1_get_hash failed, ret is 0x%x\n", ret);

    p = pack->em;
    p[idx++] = 0x00;
    p[idx++] = 0x01;

    /* PS */
    while (idx < (pack->em_len - hash_info.hash_len - hash_info.asn1_len - 1)) {
        p[idx++] = CRYPTO_BYTE_MAX;
    }

    p[idx++] = 0x00;

    /* T */
    ret = memcpy_s(&p[idx], pack->em_len - idx, hash_info.asn1_data, hash_info.asn1_len);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "memcpy_s failed, ret is 0x%x\n", ret);
    idx += hash_info.asn1_len;
    ret = memcpy_s(&p[idx], pack->em_len - idx, pack->hash, hash_info.hash_len);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "memcpy_s failed, ret is 0x%x\n", ret);

    return ret;
}

/* check EM = 00 || 01 || PS || 00 || T */
int pkcs1_v15_verify(const drv_pke_hash_type hash_type, const rsa_pkcs1_pack *pack)
{
    int ret;
    rsa_pkcs1_hash_info hash_info = {0};
    unsigned char *p = NULL;
    unsigned int idx = 0;

    ret = pkcs1_get_hash(hash_type, NULL, &hash_info);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "pkcs1_get_hash failed, ret is 0x%x\n", ret);

    p = pack->em;
    if (p[idx++] != 0x00) {
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_V15_CHECK);
    }
    if (p[idx++] != 0x01) {
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_V15_CHECK);
    }

    /* PS */
    while (idx < (pack->em_len - hash_info.hash_len - hash_info.asn1_len - 1)) {
        if (p[idx++] != CRYPTO_BYTE_MAX) {
            return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_V15_CHECK);
        }
    }
    if (p[idx++] != 0x00) {
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_V15_CHECK);
    }

    /* T */
    if (memcmp(&p[idx], hash_info.asn1_data, hash_info.asn1_len) != 0) {
        crypto_log_err("check v15 asn1 failed!\n");
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_V15_CHECK);
    }

    idx += hash_info.asn1_len;
    if (memcmp(&p[idx], pack->hash, hash_info.hash_len) != 0) {
        crypto_log_err("check v15 hash failed!\n");
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_V15_CHECK);
    }

    return CRYPTO_SUCCESS;
}

/* EM = 00 || 02 || PS || 00 || M */
int pkcs1_v15_encrypt(const rsa_pkcs1_pack *pack)
{
    int ret;
    unsigned char *p = NULL;
    unsigned int idx = 0;
    unsigned int ps_len = pack->em_len - pack->data_len - RSA_PADLEN_3;

    p = pack->em;
    p[idx++] = 0x00;
    p[idx++] = 0x02; // 0x02 see comment above func

    /* PS */
    ret = inner_get_random_with_nonzero_octets(&p[idx], ps_len);
    if (ret != CRYPTO_SUCCESS) {
        crypto_log_err("get random ps failed!\n");
        return ret;
    }
    idx += ps_len;

    p[idx++] = 0x00;
    /* M */
    if (pack->data_len != 0) {
        ret = memcpy_s(&p[idx], pack->em_len - idx, pack->data, pack->data_len);
        crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "memcpy_s failed, ret is 0x%x\n", ret);
    }

    return CRYPTO_SUCCESS;
}

/* check EM = 00 || 02 || PS || 00 || M */
int pkcs1_v15_decrypt(rsa_pkcs1_pack *pack)
{
    int ret;
    unsigned char *p = NULL;
    unsigned int idx = 0;
    unsigned int len = 0;
    /* Hint-1.1 Here pack->em_len >= 128 guaranteed by
     * drv_cipher_pke_rsa_private_decrypt->pke_rsa_decrypt->rsa_decrypt->pkcs1_v15_decrypt. */

    p = pack->em;
    if (p[idx++] != 0x00) {
        crypto_log_err("padding check failed!\n");
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_CRYPTO_V15_CHECK);
    }

    if (p[idx++] != 0x02) { // 0x02 see comment above func
        crypto_log_err("padding check failed!\n");
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_CRYPTO_V15_CHECK);
    }

    /* PS */
    while ((idx < (pack->em_len - RSA_PADLEN_1)) && p[idx] != 0x00) {
        idx++;
    }

    if (p[idx++] != 0x00) {
        crypto_log_err("padding check failed!\n");
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_CRYPTO_V15_CHECK);
    }

    len = pack->em_len - idx;
    /* Result-1.1 According to Hint-1.1, pack->em_len > RSA_PADLEN_11(11),
     * so expression 'pack->em_len - RSA_PADLEN_11' will never reverse. */
    if (len > pack->em_len - RSA_PADLEN_11) {
        crypto_log_err("padding check failed!\n");
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_CRYPTO_V15_CHECK);
    }

    /* M */
    if (len != 0) {
        ret = memcpy_s(pack->data, pack->data_len, &p[idx], len);
        crypto_chk_return(ret != CRYPTO_SUCCESS, PKE_COMPAT_ERRNO(ERROR_PKE_RSA_CRYPTO_BUFFER_TOO_SMALL),
            "memcpy_s failed, ret is 0x%x\n", ret);
    }
    pack->data_len = len; // record length of plain text for return

    return CRYPTO_SUCCESS;
}
