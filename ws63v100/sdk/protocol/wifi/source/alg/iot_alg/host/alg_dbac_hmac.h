﻿/*
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
 * Description: algorithm dbac
 */

#ifndef __ALG_DBAC_HMAC_H__
#define __ALG_DBAC_HMAC_H__

#ifdef _PRE_WLAN_FEATURE_DBAC

/******************************************************************************
  1 其他头文件包含
******************************************************************************/
#include "frw_ext_if.h"
#include "hmac_alg_if.h"
#include "alg_transplant.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

osal_void alg_dbac_hmac_init(osal_void);
osal_void alg_dbac_hmac_exit(osal_void);
osal_bool alg_dbac_hmac_is_vap_stop(osal_u8 hal_vap_id);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* #ifdef _PRE_WLAN_FEATURE_TPC */
#endif /* end of alg_tpc.h */