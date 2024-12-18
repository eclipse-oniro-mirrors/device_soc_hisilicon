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
 * Description: mini string.h
 */

#ifndef __STDARG_H__
#define __STDARG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define __NEED_va_list
#include <bits/alltypes.h>

#define va_copy(d,s)    __builtin_va_copy(d,s)
#define va_start(v,l)   __builtin_va_start(v,l)
#define va_arg(v,l)     __builtin_va_arg(v,l)
#define va_end(v)       __builtin_va_end(v)

#ifdef __cplusplus
}
#endif

#endif
