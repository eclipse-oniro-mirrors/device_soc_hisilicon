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
 * Description: security_unified kernel system init API function implementation.
 *
 * Create: 2023-05-26
*/

#include "kapi_inner.h"

#include "crypto_drv_common.h"

td_s32 kapi_env_init(td_void)
{
    td_s32 ret;
    /* KAPI Init. */
    ret = kapi_cipher_hash_env_init();
    crypto_chk_return(ret != TD_SUCCESS, ret, "kapi_cipher_hash_env_init failed, ret is 0x%x.\n", ret);

    ret = kapi_cipher_trng_env_init();
    crypto_chk_goto(ret != TD_SUCCESS, error_kapi_hash_env_deinit,
        "kapi_cipher_trng_env_init failed, ret is 0x%x.\n", ret);

    ret = kapi_cipher_symc_env_init();
    crypto_chk_goto(ret != TD_SUCCESS, error_kapi_trng_env_deinit,
        "kapi_cipher_symc_env_init failed, ret is 0x%x.\n", ret);

    ret = kapi_cipher_pke_env_init();
    crypto_chk_goto(ret != TD_SUCCESS, error_kapi_symc_env_deinit,
        "kapi_cipher_pke_env_init failed, ret is 0x%x.\n", ret);

    ret = kapi_km_env_init();
    crypto_chk_goto(ret != TD_SUCCESS, error_kapi_pke_env_deinit,
        "kapi_km_env_init failed, ret is 0x%x.\n", ret);

    return ret;
error_kapi_pke_env_deinit:
    kapi_cipher_pke_env_deinit();
error_kapi_symc_env_deinit:
    kapi_cipher_symc_env_deinit();
error_kapi_trng_env_deinit:
    kapi_cipher_trng_env_deinit();
error_kapi_hash_env_deinit:
    kapi_cipher_hash_env_deinit();
    return ret;
}

td_void kapi_env_deinit(td_void)
{
    kapi_km_env_deinit();
    kapi_cipher_pke_env_deinit();
    kapi_cipher_symc_env_deinit();
    kapi_cipher_trng_env_deinit();
    kapi_cipher_hash_env_deinit();
}