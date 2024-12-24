/*
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
 * Description: common define of shell cmds
 * Author: none
 * Create: 2020
 */

#ifndef LWIP_UTILITY_H
#define LWIP_UTILITY_H

#include "arch/cc.h"
#include "lwip/opt.h"

#ifdef CUSTOM_AT_COMMAND
#include "soc_at.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define MACADDR_BUF_LEN 32
#define TYPE_VAL_ARGS 2

#ifndef PRINT_USE_CRLF
#define PRINT_USE_CRLF 1
#endif

#if PRINT_USE_CRLF
#define CRLF "\r\n"
#else
#define CRLF "\n"
#endif

#ifdef LWIP_DEBUG_OPEN
#undef PRINTK
#define PRINTK (void)uapi_at_printf
#endif

#ifndef LWIP_STATIC
#define LWIP_STATIC static
#endif

#define API_SHELL_ERRCODE_USAGE "201 invalid usage"
#define API_SHELL_ERRCODE_TCPIP_UNINTED "202 tcpip_init do not init"
#define API_SHELL_ERRCODE_MEM_ERR "203 mem malloc failed"
#define API_SHELL_ERRCODE_SEM_ERR "204 sem create failed"
#define API_SHELL_ERRCODE_DEV_NOT_FOUND "205 net device not found"
#define API_SHELL_ERRCODE_DEV_NOT_READY "206 net device not ready"
#define API_SHELL_ERRCODE_INVALID "207 invalid parameter"
#define API_SHELL_ERRCODE_SERVICE_FAILURE "208 service failure"
#define API_SHELL_ERRCODE_IP_CONFLICT "209 IP conflict"
#define API_SHELL_ERRCODE_DUPLICATE_NETWORK "210 duplicate network"
#define API_SHELL_ERRCODE_NO_ROUTE "211 not route found"

#ifndef PRINT_ERRCODE
#define PRINT_ERRCODE(x) LWIP_PLATFORM_PRINT("ERR:%s"CRLF, (x))
#endif

#define PRINT_BUF_LEN   1024
#define MAX_MACADDR_STRING_LENGTH    18 /* including NULL */

int convert_string_to_hex(const char *src, unsigned char *dest);

s32_t lwip_isdigitstr(const s8_t* str);
#ifdef __cplusplus
}
#endif

#endif /* LWIP_UTILITY_H */
