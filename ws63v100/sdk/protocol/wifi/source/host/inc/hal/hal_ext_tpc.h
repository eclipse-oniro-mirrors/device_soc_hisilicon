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
 * Description: Header file for hal_ext_tpc.c
 */

#ifndef __HAL_EXT_TPC_H__
#define __HAL_EXT_TPC_H__
/*****************************************************************************
    1 头文件包含
*****************************************************************************/
#include "osal_types.h"
#include "hal_ops_common_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/*****************************************************************************
    2 函数声明
*****************************************************************************/
#ifdef _PRE_WLAN_FEATURE_TPC
osal_void hh503_set_tpc_bf_pow(hal_to_dmac_device_stru *hal_device);
osal_void hh503_set_tpc_rts_pow(osal_void);
/*****************************************************************************
 功能描述  : 适配sounding report帧的功率
*****************************************************************************/
static INLINE__ osal_void hal_set_tpc_bf_pow(hal_to_dmac_device_stru *hal_device)
{
    hal_public_hook_func(_set_tpc_bf_pow)(hal_device);
}
/*****************************************************************************
 功能描述  : 适配RTS的功率
*****************************************************************************/
static INLINE__ osal_void hal_set_tpc_rts_pow(osal_void)
{
    hal_public_hook_func(_set_tpc_rts_pow)();
}
osal_void hal_tpc_query_band_rate_power_debug(hal_to_dmac_device_stru *hal_device);
osal_void hal_tpc_query_ack_power_reg(osal_void);
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hal_ext_tpc.h */