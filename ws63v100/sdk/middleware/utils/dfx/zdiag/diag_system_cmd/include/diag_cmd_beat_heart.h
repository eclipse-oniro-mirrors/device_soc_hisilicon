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
 * Description: dfx beat heart
 * This file should be changed only infrequently and with great care.
 */
#ifndef __DIAG_CMD_BEAT_HEART_H__
#define __DIAG_CMD_BEAT_HEART_H__

#include "errcode.h"
#include "diag.h"

errcode_t diag_cmd_beat_heart(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
errcode_t diag_beat_heart_init(void);
void diag_beat_heart_process(void);
#endif