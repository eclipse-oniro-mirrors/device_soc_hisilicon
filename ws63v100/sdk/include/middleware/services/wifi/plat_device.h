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
 * Description: plat_device.h 的头文件
 */


#ifndef SERVICE_PLAT_DEVICE_C_H
#define SERVICE_PLAT_DEVICE_C_H

#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup middleware_service_plat_device PLAT Device
 * @ingroup  middleware_service_plat
 * @{
 */

/**
 * @if Eng
 * @brief  Set pm enable.
 * @param  [in]  pm_switch        Set linkloss parameters. 1: open, 0 close.
 * @retval EXT_ERR_SUCCESS        Execute successfully.
 * @retval Other                  Error code.
 * @else
 * @brief  设置平台低功耗开关。
 * @param  [in]  pm_switch        开关参数。 1: 开启, 0 关闭。
 * @retval EXT_ERR_SUCCESS        成功。
 * @retval Other                  失败。
 * @endif
 */
errcode_t plat_set_pm_mode(int32_t pm_switch);

/**
 * @if Eng
 * @brief  Reset board.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  复位单板。
 * @retval ERRCODE_SUCC        成功。
 * @retval Other               失败。
 * @endif
 */
errcode_t plat_reset_board(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif // SERVICE_PLAT_DEVICE_C_H
