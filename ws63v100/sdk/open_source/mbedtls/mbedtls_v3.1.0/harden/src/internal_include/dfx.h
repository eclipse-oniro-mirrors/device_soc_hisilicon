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

#include "mbedtls/ecp.h"
#include "mbedtls/platform.h"

#define PRINT_LINE_LEN                     16

#define mbedtls_harden_log_err(fmt, ...)   \
    mbedtls_printf("[%s:%d]" "HARDEN MBED ERR! : " fmt, __func__, __LINE__, ##__VA_ARGS__)

#define unused(x)                          (void)(x)

#if defined(MBED_HARDEN_DEBUG)
#define mbedtls_harden_log_dbg             mbedtls_printf
#else
#define mbedtls_harden_log_dbg(...)        (void)0
#endif

#define mbedtls_harden_log_func_enter()    mbedtls_harden_log_dbg("%s ===>Enter\n", __func__)
#define mbedtls_harden_log_func_exit()     mbedtls_harden_log_dbg("%s <===Exit\n", __func__)

#define MBED_HARDEN_DEBUG_BREAKPOINT       1

#if defined(MBED_HARDEN_DEBUG_BREAKPOINT)
#define mbedtls_harden_log_breakpoint()    \
    mbedtls_harden_log_dbg("your code go here %s--> LINE = %d!\n", __func__,__LINE__)
#endif

void mbedtls_dump_grp_id(mbedtls_ecp_group_id group_id);

void mbedtls_dump_hex(const char* name, const unsigned char* buf, unsigned int buf_len);

void mbedtls_dump_mpi(const char* name, const mbedtls_mpi *mpi);

void mbedtls_dump_point(const char* name, const mbedtls_ecp_point *point);