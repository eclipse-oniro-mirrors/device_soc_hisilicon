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
 *
 * Description: Provides tcxo trim port \n
 *
 * History: \n
 * 2024-01-31， Create file. \n
 */

#ifndef XO_TRIM_PORTING_H
#define XO_TRIM_PORTING_H

#include <stdint.h>
#include "std_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

void cmu_xo_trim_init(void);
#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV)
void cmu_xo_trim_temp_comp_init(void);
void cmu_xo_trim_temp_comp_print(void);
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
