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
 * Description: Common Boot for Standard Application Core
 *
 * Create: 2023-01-09
 */

#ifndef BOOT_CMD_LOOP_H
#define BOOT_CMD_LOOP_H

#include "boot_uart_auth.h"

#define ACK_SUCCESS        0x5A
#define ACK_FAILURE        0xA5

enum {
    CMD_ACK = 0xE1,
    CMD_DL_IMAGE = 0xD2,
    CMD_BURN_EFUSE = 0xC3,
    CMD_UL_DATA = 0xB4,
    CMD_READ_EFUSE = 0xA5,
    CMD_FLASH_PROTECT = 0x96,
    CMD_RESET = 0x87,
    CMD_FACTORY_IMAGE = 0x78,
    CMD_VERSION = 0x69,
    CMD_SET_BUADRATE = 0x5A,
};

#define LOADER_CMD_MAX      8

uint32_t loader_download_image(const uart_ctx *cmd_ctx);
uint32_t loader_upload_data(const uart_ctx *cmd_ctx);
uint32_t loader_reset(const uart_ctx *cmd_ctx);
uint32_t loader_read_ver(const uart_ctx *cmd_ctx);
uint32_t loader_set_baudrate(const uart_ctx *cmd_ctx);
void cmd_loop(uart_ctx *ctx);
void loader_ack(uint8_t err_code);
bool loader_download_is_flash_all_erased(void);
void loader_download_set_flash_all_erase(bool all_erased);
#endif