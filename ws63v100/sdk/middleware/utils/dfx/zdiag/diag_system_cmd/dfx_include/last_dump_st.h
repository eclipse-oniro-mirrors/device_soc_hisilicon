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
 * Description: last dump
 * This file should be changed only infrequently and with great care.
 */

#ifndef LAST_DUMP_ST_H
#define LAST_DUMP_ST_H
#include <stdint.h>

#define LAST_DUMP_NAME_LEN 64

typedef struct {
    uint32_t file_num;
} last_dump_start_ind_t;

typedef struct {
    char name[LAST_DUMP_NAME_LEN];
    uint32_t total_size;
    uint32_t offset;
    uint32_t size;
    uint32_t crc;
    uint8_t data[0];
} last_dump_data_ind_t;

typedef struct {
    char name[LAST_DUMP_NAME_LEN];
    uint32_t total_size;
    uint32_t start_address;
} last_dump_data_ind_finish_t;

#endif