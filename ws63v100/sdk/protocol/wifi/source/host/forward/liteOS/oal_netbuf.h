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
 * Description: netbuf function.
 * Author:
 * Create: 2022-04-07
 */

#ifndef __OAL_NETBUF_H__
#define __OAL_NETBUF_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "oal_skbuff.h"
#include "lwip/netif.h"

/*****************************************************************************
  2 宏定义
*****************************************************************************/
typedef struct pbuf oal_lwip_buf;
typedef gfp_t        oal_gfp_enum_uint8;

/*****************************************************************************
  3 结构体定义
*****************************************************************************/
typedef struct {
    oal_netbuf_stru netbuf;
    td_u8 flag;     // 1: 已申请      0:未申请
    td_u8 direct;   // 2：TX   1：RX   0:else
    td_u16 resv;
} netbuf_stru;

#define ZERO_COPY_LWIP_DRIECT_RX 1
#define ZERO_COPY_LWIP_DRIECT_TX 2

td_void oal_init_netbuf_stru(td_void);
oal_netbuf_stru *oal_malloc_netbuf_stru(td_u8 direct);
td_void oal_free_netbuf_stru(oal_netbuf_stru *netbuf);
td_void oal_free_netbuf_list(oal_netbuf_head_stru *list_head);
#ifdef _PRE_LWIP_ZERO_COPY
oal_netbuf_stru *oal_pbuf_netbuf_alloc(td_u32 len);
td_u32 oal_malloc_netbuf_used(td_u8 direct);
#endif

/* 参考hmac_rx_copy_netbuff_etc */
oal_netbuf_stru* _oal_netbuf_copy(oal_netbuf_stru *src_netbuf, oal_gfp_enum_uint8 priority);

#endif
