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
 * Description: only use for init befor drv layer test.
 *
 * Create: 2023-02-08
*/

#ifndef CRYPTO_DRV_INIT_H
#define CRYPTO_DRV_INIT_H

#include "crypto_type.h"

td_s32 crypto_drv_mod_init(td_void);
td_void crypto_drv_mod_deinit(td_void);

#endif