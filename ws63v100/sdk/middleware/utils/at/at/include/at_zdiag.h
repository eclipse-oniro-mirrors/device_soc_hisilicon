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
 * Description: software at by zdiag
 */
#ifndef ZDIAG_AT_H
#define ZDIAG_AT_H

#ifdef CONFIG_AT_SUPPORT_ZDIAG
#include "diag.h"

void zdiag_at_init(void);
errcode_t zdiag_at_proc(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);

#endif
#endif /* ZDIAG_AT_H */