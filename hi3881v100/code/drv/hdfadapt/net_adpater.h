/*
 * Copyright (C) 2021 HiSilicon (Shanghai) Technologies CO., LIMITED.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef NET_ADAPTER_H
#define NET_ADAPTER_H

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "hmac_ext_if.h"
#include "oam_ext_if.h"
#include "wal_main.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
/* * Ask if a driver is ready to send */
#define WAL_SIOCDEVPRIVATE 0x89F0 /* SIOCDEVPRIVATE */
#define WAL_ADDR_MAX 16

/* ****************************************************************************
  3 枚举定义
**************************************************************************** */
typedef enum {
    WAL_PHY_MODE_11N = 0,
    WAL_PHY_MODE_11G = 1,
    WAL_PHY_MODE_11B = 2,
    WAL_PHY_MODE_BUTT
} wal_phy_mode;

typedef enum {
    WAL_ADDR_IDX_STA0 = 0,
    WAL_ADDR_IDX_AP0  = 1,
    WAL_ADDR_IDX_STA1 = 2,
    WAL_ADDR_IDX_STA2 = 3,
    WAL_ADDR_IDX_BUTT
} wal_addr_idx;

/* ****************************************************************************
  7 STRUCT定义
**************************************************************************** */
typedef struct {
    hi_u8  ac_addr[WLAN_MAC_ADDR_LEN];
    hi_u16 us_status;
} wal_dev_addr_stru;

/* ****************************************************************************
  10 函数声明
**************************************************************************** */
hi_s32 wal_netdev_open(oal_net_device_stru *netdev);
hi_s32 wal_netdev_stop(oal_net_device_stru *netdev);

#ifdef _PRE_WLAN_FEATURE_MESH_LWIP_RIPPLE
hi_s32 hwal_lwip_remove_user(oal_lwip_netif *netif, oal_linklayer_addr *mac_addr);
hi_s32 hwal_lwip_set_beacon_priority(oal_lwip_netif *netif, hi_u8 prio);
hi_s32 hwal_lwip_set_mnid(oal_lwip_netif *netif, oal_uniqid_t us_mnid);
#endif
hi_u32 wal_mesh_inform_tx_data_info(frw_event_mem_stru *event_mem);
hi_u32 wal_mesh_report_mesh_user_info(frw_event_mem_stru *event_mem);
hi_u32 wal_report_sta_assoc_info(frw_event_mem_stru *event_mem);
hi_void wal_ip_conver_multi_mac(hi_u8 *puc_group_mac, const hi_u8 *puc_group_ip, hi_u8 ip_len);
hi_void wal_init_dev_addr(hi_void);
hi_u32 wal_set_dev_addr(const hi_char *pc_addr, hi_u8 mac_len);
hi_u32 wal_set_dev_addr_from_efuse(const hi_char *pc_addr, hi_u8 mac_len);
hi_u8 wal_dev_is_running(hi_void);
hi_u32 wal_get_dev_addr(hi_u8 *pc_addr, hi_u8 addr_len, nl80211_iftype_uint8 type);
hi_s32 wal_init_drv_wlan_netdev(nl80211_iftype_uint8 type, wal_phy_mode mode, oal_net_device_stru *netdev);
hi_s32 wal_deinit_drv_wlan_netdev(oal_net_device_stru *netdev);
oal_net_device_ops_stru *wal_get_net_dev_ops(hi_void);
hi_u32 wal_get_efuse_mac_addr(hi_void);
hi_s32 InitNetdev(struct NetDevice *netDevice, nl80211_iftype_uint8 type);
hi_s32 DeinitNetdev(nl80211_iftype_uint8 type);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
