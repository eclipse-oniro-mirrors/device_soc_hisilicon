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
#ifndef DRV_PKE_PRIV_H
#define DRV_PKE_PRIV_H

#include "drv_pke.h"
#include "crypto_pke_struct.h"

typedef struct {
    unsigned int klen;
    unsigned int em_bit;
    unsigned char *em;
    unsigned int em_len;
    unsigned char *hash;
    unsigned int hash_len;
    unsigned char *data;
    unsigned int data_len;
} rsa_pkcs1_pack;

typedef struct {
    drv_pke_hash_type hash_type;
    unsigned int hash_len;
    unsigned char *lhash_data;
    unsigned char *asn1_data;
    unsigned int asn1_len;
} rsa_pkcs1_hash_info;

/* result size */
#define HASH_SIZE_SHA_1                            20
#define HASH_SIZE_SHA_224                          28
#define HASH_SIZE_SHA_256                          32
#define HASH_SIZE_SHA_384                          48
#define HASH_SIZE_SHA_512                          64
#define HASH_SIZE_SHA_MAX                          64

#define PKE_COMPAT_ERRNO(err_code)      DRV_COMPAT_ERRNO(ERROR_MODULE_PKE, err_code)
#define pke_null_ptr_chk(ptr)   \
    crypto_chk_return((ptr) == TD_NULL, PKE_COMPAT_ERRNO(ERROR_PARAM_IS_NULL), #ptr" is NULL\n")

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* Common. */
int inner_get_random(unsigned char *random, const unsigned int size);

int inner_get_random_with_nonzero_octets(unsigned char *random, unsigned int size);

int inner_get_limit_random(unsigned char *rand, const unsigned char *limit, const unsigned int size);

/* RSA Common */
uint32_t drv_get_hash_len(const drv_pke_hash_type hash_type);

unsigned int rsa_get_bit_num(const unsigned char *big_num, unsigned int num_len);

int pkcs1_get_hash(const drv_pke_hash_type hash_type, const drv_pke_data *label,
    rsa_pkcs1_hash_info *hash_info);

/* RSA V15. */
int pkcs1_v15_sign(const drv_pke_hash_type hash_type, const rsa_pkcs1_pack *pack);

int pkcs1_v15_verify(const drv_pke_hash_type hash_type, const rsa_pkcs1_pack *pack);

int pkcs1_v15_encrypt(const rsa_pkcs1_pack *pack);

int pkcs1_v15_decrypt(rsa_pkcs1_pack *pack);

/* RSA V21. */
int pkcs1_pss_sign(const drv_pke_hash_type hash_type, const rsa_pkcs1_pack *pack);

int pkcs1_pss_verify(const drv_pke_hash_type hash_type, const rsa_pkcs1_pack *pack);

int pkcs1_oaep_encrypt(const drv_pke_hash_type hash_type, const rsa_pkcs1_pack *pack,
    const drv_pke_data *label);

int pkcs1_oaep_decrypt(const drv_pke_hash_type hash_type, rsa_pkcs1_pack *pack,
    const drv_pke_data *label);

bool inner_drv_is_in_range(const uint8_t *value, const uint8_t *range, uint32_t len);

int inner_sm2_kdf(const drv_pke_ecc_point *param, unsigned char *out, const unsigned int klen);

int inner_ecc_hash_len_check(unsigned int klen, unsigned int hash_len);

int32_t inner_modular_chk(const drv_pke_data *p, bool is_odd_required);

int32_t inner_ecdsa_type_check(drv_pke_ecc_curve_type ecc_type);

int32_t crypto_pke_rsa_hash_len_check(uint32_t klen, drv_pke_hash_type hash_type);

rsa_key_width klen_to_width(drv_pke_len klen);

int drv_cipher_pke_rsa_sign(
    const drv_pke_rsa_priv_key *priv_key,
    const drv_pke_rsa_scheme scheme,
    const drv_pke_hash_type hash_type,
    const drv_pke_data *input_hash,
    const drv_pke_data *sign);

int drv_cipher_pke_rsa_verify(
    const drv_pke_rsa_pub_key *pub_key,
    const drv_pke_rsa_scheme scheme,
    const drv_pke_hash_type hash_type,
    const drv_pke_data *input_hash,
    const drv_pke_data *sign);

int32_t drv_cipher_pke_rsa_private_decrypt(
    const drv_pke_rsa_scheme scheme,
    const drv_pke_hash_type hash_type,
    const drv_pke_rsa_priv_key *priv_key,
    const drv_pke_data *input,
    const drv_pke_data *label,
    drv_pke_data *output);

int32_t drv_cipher_pke_rsa_public_encrypt(
    const drv_pke_rsa_scheme scheme,
    const drv_pke_hash_type hash_type,
    const drv_pke_rsa_pub_key *pub_key,
    const drv_pke_data *input,
    const drv_pke_data *label,
    drv_pke_data *output);

int32_t crypto_pke_common_calc_hash(const drv_pke_data *arr, uint32_t arr_len,
    const drv_pke_hash_type hash_type, drv_pke_data *hash);
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
