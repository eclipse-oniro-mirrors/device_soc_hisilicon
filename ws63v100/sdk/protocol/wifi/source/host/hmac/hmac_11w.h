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
* Description: hmac 11w head
* Create: 2020-7-5
*/

#ifndef __HMAC_11W_H__
#define __HMAC_11W_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "hmac_vap.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  全局变量声明
*****************************************************************************/
#ifdef _PRE_WLAN_FEATURE_PMF

typedef oal_bool_enum_uint8  (*p_hmac_11w_robust_action_cb)(oal_netbuf_stru *mgmt_buf);

typedef struct {
    p_hmac_11w_robust_action_cb   hmac_11w_robust_action;
} hmac_11w_cb;

/*****************************************************************************
  函数声明
*****************************************************************************/
osal_u32 hmac_11w_rx_filter(const hmac_vap_stru *hmac_vap, const oal_netbuf_stru *netbuf);
osal_void hmac_11w_set_ucast_mgmt_frame(hmac_vap_stru *hmac_vap, const mac_tx_ctl_stru *tx_ctl,
    hal_tx_txop_feature_stru *txop_feature, const oal_netbuf_stru *netbuf, mac_ieee80211_frame_stru *frame_hdr);
osal_void hmac_11w_set_mcast_mgmt_frame(hmac_vap_stru *hmac_vap,
    hal_tx_txop_feature_stru *txop_feature, oal_netbuf_stru *netbuf, const hal_tx_mpdu_stru *mpdu);
osal_void hmac_11w_update_users_status(hmac_vap_stru  *hmac_vap, hmac_user_stru *hmac_user,
    oal_bool_enum_uint8 user_pmf);

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hmac_11w.h */
