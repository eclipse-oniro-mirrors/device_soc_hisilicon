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
 * 文 件 名   : hmac_data_acq.h
 * 生成日期   : 2014年4月25日
 * 功能描述   : hmac_data_acq.c 的头文件
 */

#ifndef __HMAC_DATA_ACQ_H__
#define __HMAC_DATA_ACQ_H__

#ifdef _PRE_WLAN_FEATURE_DAQ

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "oal_types.h"
#include "mac_vap_ext.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_DATA_ACQ_H
/*****************************************************************************
  2 宏定义
*****************************************************************************/


/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern osal_void  hmac_data_acq_init(osal_void);
extern osal_void  hmac_data_acq_exit(osal_void);
osal_u8 hmac_get_data_acq_used(osal_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  /* end of _PRE_WLAN_FEATURE_DAQ */
#endif /* end of hmac_data_acq.h */
