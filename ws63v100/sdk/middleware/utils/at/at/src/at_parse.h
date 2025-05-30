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
 * limitations under the License. \n
 *
 * Description: Provides at parse service header. Only for AT module. \n
 */

#ifndef    AT_PARSE_H
#define    AT_PARSE_H

#include "at_product.h"

/**
 * @if Eng
 * @brief  Definition of AT command format.
 * @else
 * @brief  AT命令格式
 * @endif
 */
typedef enum {
    AT_FORMAT_ADD = 0x00,
    AT_FORMAT_EXPAND,
    AT_FORMAT_ERROR,
    AT_FORMAT_NUM
} at_format_t;

typedef struct at_cmd_str {
    char                 *cmd_str;
    uint16_t             str_len;
    uint16_t             channel_id;
    struct at_cmd_str    *next;
} at_cmd_info_t;

typedef enum {
    AT_NORMAL_STATE = 0x00,
    AT_ESCAPE_STATE,
    AT_END_STATE
} at_parse_state_t;

bool at_parse_has_remain_cmd(void);

void at_parse_del_one_remain_cmd(at_cmd_info_t *cmd_info);

at_cmd_info_t* at_parse_get_next_remain_cmd(void);

at_format_t at_parse_format_of_cmd(const at_cmd_info_t *cmd_info, uint16_t *offset);

at_ret_t at_parse_split_combine_cmd(const char *data, uint32_t len, uint16_t channel_id);

at_cmd_type_t at_parse_cmd_type(const char *str, uint16_t *offset);

at_ret_t at_parse_para_arguments(const char *str, void *args, const at_para_parse_syntax_t *syntax);

void at_parse_free_arguments(void *args, const at_para_parse_syntax_t *syntax);
#endif
