/**
 * Copyright (c) 2020 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * Description: mbedtls harden adapt internal header file.
 *
 * Create: 2023-05-10
*/

#ifndef MBEDTLS_PLATFORM_HARDWARE_CONFIG_H
#define MBEDTLS_PLATFORM_HARDWARE_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define MBEDTLS_HARDEN_SUCCESS           TD_SUCCESS
#define MBEDTLS_HARDEN_TRUE              TD_TRUE
#define MBEDTLS_HARDEN_FALSE             TD_FALSE

#define MAX_AES_HARD_CTX_NUM  4

#define MAX_AES_XTS_TWEAK_SIZE   256

#define MAX_GCM_HARD_CTX_NUM   2

#define MBEDTLS_CIPHER_MAX_ADD_LEN   128

#define MAX_CCM_HARD_CTX_NUM   2

#define MAX_CMAC_HARD_CTX_NUM   4

#define MBEDTLS_AES_192_HARD_KEY_SUPPORT

#define DRV_PKE_LEN_1024_SUPPORT

#define HARDWARE_521_ALIGN_576_LEN

#define MAX_HASH_HARD_CTX_NUM   4

#ifndef CONFIG_SUPPORT_HILINK
#undef MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED

#undef MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED

#undef MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
#endif
/*******************************************alternative macro*********************************************/

#define MBEDTLS_ENTROPY_HARDWARE_ALT

#define MBEDTLS_AES_ALT

// #define MBEDTLS_CMAC_ALT

// #define MBEDTLS_ECDH_COMPUTE_SHARED_ALT

// #define MBEDTLS_ECDSA_VERIFY_ALT

// #define MBEDTLS_ECDSA_SIGN_ALT

#define MBEDTLS_ECC_GEN_KEY_ALT

#define MBEDTLS_SHA1_ALT

#define MBEDTLS_SHA256_ALT

#define MBEDTLS_SHA512_ALT

#define MBEDTLS_HKDF_ALT

#define MBEDTLS_PBKDF2_HMAC_ALT

// #define MBEDTLS_ECDSA_GENKEY_ALT

#define MBEDTLS_ECP_RESTARTABLE

// #define MBEDTLS_ECP_MUL_ALT

// #define MBEDTLS_INNER_ECDSA_VERIFY_RESTARTABLE_ALT

#define MBEDTLS_SHA1_C
#define MBEDTLS_SHA224_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_SHA384_C
#define MBEDTLS_SHA512_C

#ifndef CONFIG_SUPPORT_HILINK
#ifndef CONFIG_NOT_SUPPORT_RSA_ALT
#define MBEDTLS_RSA_ALT
#define MBEDTLS_RSA_NO_CRT
#define MBEDTLS_RSA_ALT_MAX_SIZE    512
#endif
#endif

/*******************************************alternative macro*********************************************/

/*******************************************algorithm cfg macro*******************************************/

#define MBEDTLS_SECP192R1_USE_HARDWARE
#define MBEDTLS_SECP224R1_USE_HARDWARE
#define MBEDTLS_SECP256R1_USE_HARDWARE
#define MBEDTLS_SECP384R1_USE_HARDWARE
#define MBEDTLS_SECP521R1_USE_HARDWARE
#define MBEDTLS_BP256R1_USE_HARDWARE
#define MBEDTLS_BP384R1_USE_HARDWARE
#define MBEDTLS_BP512R1_USE_HARDWARE
#define MBEDTLS_CURVE25519_USE_HARDWARE
#define MBEDTLS_CURVE448_USE_HARDWARE

#define MBEDTLS_SHA1_USE_HARDWARE
#define MBEDTLS_SHA224_USE_HARDWARE
#define MBEDTLS_SHA256_USE_HARDWARE
#define MBEDTLS_SHA384_USE_HARDWARE
#define MBEDTLS_SHA512_USE_HARDWARE

#define MBEDTLS_BIGNUM_EXP_MOD_USE_HARDWARE
#define MBEDTLS_BIGNUM_MOD_USE_HARDWARE

#define MBEDTLS_NO_PLATFORM_ENTROPY
/*******************************************algorithm cfg macro*******************************************/

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_PLATFORM_USER_CONFIG_H */