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
 * Description:   PANIC module to rom implementation
 * Author:
 * Create: 2020-4-16
 */

#include "panic.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define BACK_TO_REAL_CALLER 5

static panic_deal_handler g_panic_deal = panic_deal;

void panic_register_deal_callback(panic_deal_handler callback)
{
    if (callback != NULL) {
        g_panic_deal = callback;
    }
}

void panic_unregister_deal_callback(void)
{
    g_panic_deal = NULL;
}

static void panic_trigger_deal_callback(panic_id_t origin, uint32_t code, uint32_t caller)
{
    if (g_panic_deal != NULL) {
        g_panic_deal(origin, code, caller);
    } else {
        // unlike the hardfault handler 'i' does not need to be static, since we trust our stack
        volatile uint8_t i = 1;
        // Loop forever - or until jlink changes i
        while (i != 0) {
            // Spin
        }
    }
}

void panic(panic_id_t source, uint32_t code)
{
#if defined(__GNUC__)
    uint32_t caller = (uint32_t)((uintptr_t)__builtin_return_address(0)) - BACK_TO_REAL_CALLER;
#elif defined(__ICCARM__)
    uint32_t caller = (uint32_t)__get_LR();
#endif

    panic_trigger_deal_callback(source, code, caller);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif