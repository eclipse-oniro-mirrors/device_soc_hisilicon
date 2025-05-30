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
 * Description: Header file for wal_main.c.
 */

#ifndef __WAL_MAIN_H__
#define __WAL_MAIN_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "oal_ext_if.h"
#include "oam_ext_if.h"
#include "wal_ext_if.h"
#include "wal_event_msg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 提升到与接收任务优先级一致，避免因优先级低于接收任务，DHCP结果迟迟无法发送而连接失败 */
#define WAL_TXDATA_THERAD_PRIORITY      5

/*****************************************************************************
  4 函数声明
*****************************************************************************/
td_u32 wal_main_init(td_void);
td_void wal_main_exit(td_void);
td_u32 uapi_wifi_host_init(td_void);
td_void uapi_wifi_host_exit(td_void);
td_u32 uapi_wifi_plat_init(td_void);
td_void uapi_wifi_plat_exit(td_void);
osal_u32 device_init_ready(osal_u8 *cb_data);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of wal_main */
