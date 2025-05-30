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
 * limitations under the License. *
 * Description: Provides the entries for initializing and starting services and features. \n
 */
#ifndef APP_INIT_H
#define APP_INIT_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup middleware_utils_app_init Application Initialize
 * @ingroup  middleware_utils
 * @{
 */

typedef void (*init_call_t)(void);

#define USED_ATTR __attribute__((used))

#define layer_initcall(func, layer, clayer, priority)            \
    static const init_call_t USED_ATTR __zinitcall_##layer##_##func \
        __attribute__((section(".zinitcall." clayer #priority ".init"))) = (func)

#define layer_initcall_def(func, layer, clayer) \
    layer_initcall(func, layer, clayer, 0)

/**
 * @brief Identifies the entry for initializing and starting a system running phase.
 *
 * @param func Indicates the entry function for initializing and starting a system running phase.
 * The type is void (*)(void).
 */

/**
 * @if Eng
 * @brief  Identifies the entry for initializing and starting a system running phase.
 * @param  [in]  func Indicates the entry function for initializing and starting a system running phase.
 *                    The type is void (*)(void).
 * @else
 * @brief  定义系统初始化和启动的入口。
 * @param  [in]  func 系统初始化和启动的入口。类型是：void (*)(void)。
 * @endif
 */
#define app_run(func) layer_initcall_def(func, run, "app_run")

/**
 * @if Eng
 * @brief  Initialize the APP Manager.
 * @else
 * @brief  初始化APP Manager。
 * @endif
 */
void app_tasks_init(void);

/**
 * @if Eng
 * @brief  Use the chip configure to generate the initialize code.
 * @else
 * @brief  使用芯片配置生成的代码进行系统初始化。
 * @endif
 */
void system_init(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
