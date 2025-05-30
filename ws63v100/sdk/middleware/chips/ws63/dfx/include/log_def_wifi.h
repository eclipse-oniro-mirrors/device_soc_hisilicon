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
 * Description: define wifi file id
 */

#ifndef __LOG_DEF_WIFI_H__
#define __LOG_DEF_WIFI_H__

/* WiFi 文件ID
   注：仅支持在最后新增file_id，禁止改变已有file_id的值，
       rom化代码file_id变化会导致hso log无法解析
*/
typedef enum {
    DIAG_FILE_ID_WIFI_DEV_DMAC_USER_ROM_C = 0,
    DIAG_FILE_ID_WIFI_DEV_DMAC_TXOPPS_ROM_C,
    OAM_FILE_ID_DMAC_CRYPT_MD5_C,
    OAM_FILE_ID_WAL_LINUX_SCAN_H,
    DIAG_FILE_ID_WIFI_DEV_DMAC_VAP_ROM_C,
    OAM_FILE_ID_HMAC_FEATURE_DFT_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_BLACKLIST_C,
    DIAG_FILE_ID_WIFI_DEVICE_HAL_CSI_C,
    DIAG_FILE_ID_WIFI_DEV_ALG_TXBF_ROM_C,
    OAM_FILE_ID_WIFI_HOST_HAL_PM_ROM_C,
    OAM_FILE_ID_WAL_LINUX_PSD_C,
    OAM_FILE_ID_HMAC_PSD_C,
    DIAG_FILE_ID_WIFI_DEV_ALG_DBAC_GC_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_WAL_REGDB_C,
    DIAG_FILE_ID_WIFI_DEV_DMAC_BLOCKACK_ROM_C,
    OAM_FILE_ID_DMAC_CRYPT_AES_OMAC1_C,
    OAM_FILE_ID_HMAC_KEEP_ALIVE_C,
    DIAG_FILE_ID_WIFI_DEV_ALG_PROBE_COMMON_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_ALG_TXBF_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_SME_STA_C,
    DIAG_FILE_ID_WIFI_DEV_HAL_CHIP_C,
    OAM_FILE_ID_HAL_MAC_ERR_ROM_C,
    OAM_FILE_ID_WAL_LINUX_11D_C,
    DIAG_FILE_ID_WIFI_DEV_DMAC_M2S_ROM_C,
    OAM_FILE_ID_DMAC_UAPSD_STA_ROM_C,
    DIAG_FILE_ID_COMMON_DFT_C,
    DIAG_FILE_ID_WIFI_HOST_ALG_EDCA_OPT_C,
    DIAG_FILE_ID_WIFI_DEV_HAL_TBTT_ROM_C,
    DIAG_FILE_ID_WIFI_DEV_DMAC_11W_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_BTCOEX_BTSTA_C,
    DIAG_FILE_ID_WIFI_DEV_DMAC_STA_PM_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_SAMPLE_DAQ_C,
    DIAG_FILE_ID_WIFI_DEVICE_DMAC_KEEP_ALIVE_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_ENCAP_FRAME_C,
    OAM_FILE_ID_WAL_LINUX_IOCTL_DEBUG_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_CHR_C,
    DIAG_FILE_ID_WIFI_HOST_WAL_CUSTOMIZE_C,
    DIAG_FILE_ID_WIFI_HMAC_RX_DATA_FEATURE_C,
    DIAG_FILE_ID_WIFI_DEV_HAL_VAP_ROM_C,
    OAM_FILE_ID_WIFI_DEV_HAL_PM_ROM_C,
    DIAG_FILE_ID_WIFI_DEVICE_DMAC_TX_COMPLETE_ROM_C,
    OAM_FILE_ID_MAC_FRAME_ROM_C,
    DIAG_FILE_ID_WIFI_DEV_DMAC_BEACON_ROM_C,
    OAM_FILE_ID_HMAC_BLOCKACK_H,
    OAM_FILE_ID_WIFI_DEV_HAL_MAC_ROM_C,
    DIAG_FILE_ID_WIFI_DEVICE_HAL_OMI_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_PSM_AP_C,
    DIAG_FILE_ID_WIFI_HOST_ALG_TRAFFIC_CTL_C,
    OAM_FILE_ID_HAL_EDCA_C,
    OAM_FILE_ID_DMAC_PSM_STA_ROM_H,
    DIAG_FILE_ID_WIFI_HOST_REKEY_ARP_OFFLOAD_H,
    DIAG_FILE_ID_WIFI_DEV_DMAC_FCS_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_HAL_DAQ_C,
    OAM_FILE_ID_HMAC_ROAM_ALG_H,
    DIAG_FILE_ID_WIFI_DEVICE_HAL_COEX_REG_ROM_C,
    OAM_FILE_ID_HAL_ROM_CB_H,
    DIAG_FILE_ID_WIFI_HOST_MAC_DEVICE_C,
    DIAG_FILE_ID_WIFI_DEV_HAL_POWER_C,
    DIAG_FILE_ID_WIFI_HOST_ALG_SCHEDULE_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_BLOCKACK_C,
    OAM_FILE_ID_DMAC_UAPSD_STA_H,
    DIAG_FILE_ID_WIFI_HOST_ALG_GLA_C,
    DIAG_FILE_ID_WIFI_HOST_ALG_EDCA_INTRF_MODE_C,
    OAM_FILE_ID_FRW_EXT_IF_H,
    OAM_FILE_ID_MAC_RESOURCE_H,
    OAM_FILE_ID_DMAC_PSM_AP_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_SCAN_C,
    DIAG_FILE_ID_WIFI_DEVICE_FRW_DMAC_ADAPT_ROM_C,
    OAM_FILE_ID_WAL_LINUX_11D_H,
    DIAG_FILE_ID_WIFI_DEV_HAL_CHAN_MGMT_C,
    OAM_FILE_ID_MAIN_C,
    OAM_FILE_ID_WIFI_DEV_HAL_POWER_ROM_C,
    DIAG_FILE_ID_WIFI_DEV_ALG_MAIN_C,
    DIAG_FILE_ID_WIFI_HOST_OAL_MEM_C,
    OAM_FILE_ID_DMAC_PSM_STA_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_SDP_C,
    OAM_FILE_ID_HMAC_CHBA_USER_H,
    DIAG_FILE_ID_WIFI_DEV_HAL_AMPDU_ROM_C,
    OAM_FILE_ID_CALI_CHANNEL,
    OAM_FILE_ID_WAL_LINUX_IW_C,
    OAM_FILE_ID_HMAC_WMM_H,
    OAM_FILE_ID_HMAC_UAPSD_H,
    OAM_FILE_ID_HMAC_CHBA_COEX_H,
    DIAG_FILE_ID_WIFI_DEVICE_FRW_ROM_CB_ROM_C,
    OAM_FILE_ID_OAL_MEM_HCM_H,
    OAM_FILE_ID_HAL_TPC_C,
    OAM_FILE_ID_HAL_CHIP_H,
    DIAG_FILE_ID_WIFI_HOST_HMAC_MBO_C,
    OAM_FILE_ID_HMAC_ROAM_MAIN_H,
    OAM_FILE_ID_HMAC_CHBA_COEX_C,
    OAM_FILE_ID_HAL_GP_REG_C,
    OAM_FILE_ID_HMAC_OPMODE_H,
    OAM_FILE_ID_HAL_MSG_H,
    OAM_FILE_ID_FRW_TIMER_H,
    OAM_FILE_ID_RSV1,
    OAM_FILE_ID_HAL_RF_H,
    DIAG_FILE_ID_WIFI_DEVICE_DMAC_CONFIG_C,
    OAM_FILE_ID_HMAC_CHBA_SYNC_H,
    DIAG_FILE_ID_WIFI_DEV_HAL_PM_C,
    OAM_FILE_ID_HAL_PHY_H,
    OAM_FILE_ID_DMAC_RX_DATA_H,
    DIAG_FILE_ID_WIFI_HOST_FRW_MAIN_C,
    OAM_FILE_ID_WIFI_HOST_HAL_POWER_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_ARP_OFFLOAD_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_BTCOEX_M2S_C,
    OAM_FILE_ID_HMAC_WOW_H,
    ALG_SCHEDULE_AC_WEIGHT_PRIOR_RAM_C,
    OAM_FILE_ID_HMAC_CHBA_INTERFACE_C,
    DIAG_FILE_ID_WIFI_DEVICE_DMAC_TX_BSS_COMM_ROM_C,
    DIAG_FILE_ID_WIFI_DEVICE_FRW_TASK_ROM_C,
    OAM_FILE_ID_WAL_LINUX_IW_H,
    DIAG_FILE_ID_WIFI_HOST_WAL_CCPRIV_COMMON_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_MGMT_BSS_COMM_C,
    OAM_FILE_ID_HMAC_TX_AMSDU_H,
    DIAG_FILE_ID_WIFI_DEVICE_FRW_MAIN_C,
    OAM_FILE_ID_HMAC_AMPDU_CONFIG_H,
    OAM_FILE_ID_HAL_EXT_IF_ROM_H,
    DIAG_FILE_ID_WIFI_HOST_HMAC_DFS_C,
    OAM_FILE_ID_HMAC_DATA_ACQ_H,
    DIAG_FILE_ID_WIFI_HOST_WAL_UTILS_C,
    DIAG_FILE_ID_WIFI_DEV_DMAC_PSD_C,
    OAM_FILE_ID_HMAC_PSD_H,
    OAM_FILE_ID_DMAC_APF_ROM_H,
    DIAG_FILE_ID_WIFI_HOST_HAL_ALG_TPC_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_11V_C,
    OAM_FILE_ID_WAL_LINUX_PSD_H,
    OAM_FILE_ID_CALI_INIT_POWER,
    OAM_FILE_ID_WIFI_DEV_HAL_DEVICE_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_RESET_C,
    OAM_FILE_ID_CALI_ONLINE_TX_PWR,
    OAM_FILE_ID_DMAC_STA_PM_C,
    OAM_FILE_ID_WIFI_DEV_HAL_EVENT_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_UAPSD_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_WOW_C,
    OAM_FILE_ID_WAL_LINUX_RX_RSP_H,
    OAM_FILE_ID_WAL_EXT_IF_H,
    OAM_FILE_ID_HMAC_CHBA_USER_C,
    DIAG_FILE_ID_WIFI_HOST_WAL_EVENT_C,
    OAM_FILE_ID_WAL_LINUX_IOCTL_H,
    OAM_FILE_ID_WAL_LINUX_SCAN_C,
    DIAG_FILE_ID_WIFI_DEVICE_HAL_P2P_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_11I_C,
    OAM_FILE_ID_WIFI_HOST_HAL_MAC_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_FSM_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_FCS_C,
    DIAG_FILE_ID_WIFI_DEV_HAL_COEX_REG_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_ALG_DBAC_C,
    OAM_FILE_ID_HMAC_P2P_H,
    DIAG_FILE_ID_WIFI_DEV_DMAC_DATA_SAMPLE_ROM_C,
    OAM_FILE_ID_HAL_CALI_COMP_C,
    OAM_FILE_ID_HAL_RESET_C,
    DIAG_FILE_ID_WIFI_HOST_HAL_SR_C,
    OAM_FILE_ID_HAL_IRQ_C,
    OAM_FILE_ID_HMAC_11V_H,
    DIAG_FILE_ID_WIFI_HOST_WAL_NET_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_ANT_SWITCH_C,
    OAM_FILE_ID_HMAC_CHBA_SYNC_C,
    OAM_FILE_ID_CALI_TX_COMPLETE,
    OAM_FILE_ID_HAL_ANT_SEL_C,
    DIAG_FILE_ID_WIFI_DEVICE_DMAC_ROM_PARAM_C,
    DIAG_FILE_ID_WIFI_HOST_ALG_TPC_C,
    DIAG_FILE_ID_WIFI_DEV_HAL_RF_C,
    DIAG_FILE_ID_WIFI_DEVICE_DMAC_FORWARD_MAIN_C,
    OAM_FILE_ID_HMAC_SLP_H,
    DIAG_FILE_ID_WIFI_DEV_INDEPENDENT_TX_ROM_C,
    OAM_FILE_ID_HMAC_CHBA_PS_C,
    DIAG_FILE_ID_WIFI_DEVICE_DMAC_DEVICE_ROM_C,
    OAM_FILE_ID_WAL_LINUX_UTIL_C,
    DIAG_FILE_ID_WIFI_DEV_DMAC_ALG_ROM_C,
    DIAG_FILE_ID_WIFI_DEV_HAL_EVENT_ROM_C,
    OAM_FILE_ID_WIFI_HOST_HAL_RESET_ROM_C,
    DIAG_FILE_ID_WIFI_DEV_DMAC_SR_STA_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_WAL_EVENT_MSG_C,
    OAM_FILE_ID_HMAC_CHBA_FEATURE_C,
    OAM_FILE_ID_WAL_COMMON_H,
    OAM_FILE_ID_DMAC_STA_PM_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_WAL_CFG80211_APT_C,
    OAM_FILE_ID_CALI_COMP,
    DIAG_FILE_ID_WIFI_DEV_HAL_TB_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_HAL_DFT_C,
    OAM_FILE_ID_HAL_FTM_C,
    OAM_FILE_ID_HMAC_MGMT_CLASSIFIER_H,
    OAM_FILE_ID_HMAC_DEVICE_H,
    DIAG_FILE_ID_WIFI_DEV_HAL_BOARD_RF_DEV_C,
    OAM_FILE_ID_DMAC_CRYPT_TKIP_C,
    DIAG_FILE_ID_WIFI_DEVICE_OAL_MEM_ROM_C,
    OAM_FILE_ID_CALI_RC_C,
    OAM_FILE_ID_DMAC_EXT_IF_ROM_H,
    OAM_FILE_ID_FE_TPC_RATE_POW,
    OAM_FILE_ID_HAL_CHAN_H,
    OAM_FILE_ID_HAL_CHIP_C,
    OAM_FILE_ID_WAL_MAIN_H,
    OAM_FILE_ID_HMAC_CHR_H,
    DIAG_FILE_ID_WIFI_HOST_FRW_THREAD_C,
    DIAG_FILE_ID_DMAC_SMOOTH_PHASE_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_11K_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_SMPS_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_WPS_C,
    OAM_FILE_ID_HMAC_RESET_H,
    OAM_FILE_ID_HAL_RF_DEV_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_VAP_C,
    DIAG_FILE_ID_WIFI_DEV_DMAC_POWER_ROM_C,
    DIAG_FILE_ID_WIFI_DEV_ALG_TPC_PROBE_ROM_C,
    OAM_FILE_ID_HMAC_LATENCY_STAT_C,
    OAM_FILE_ID_HMAC_11I_H,
    DIAG_FILE_ID_WIFI_DEV_PKT_DEBUG_C,
    OAM_FILE_ID_DMAC_DATA_SAMPLE_ROM_H,
    OAM_FILE_ID_WAL_DFX_H,
    DIAG_FILE_ID_WIFI_DEV_ALG_AUTORATE_BASIC_ROM_C,
    OAM_FILE_ID_HAL_RF_DEV_H,
    OAM_FILE_ID_HMAC_SINGLE_PROXYSTA_H,
    DIAG_FILE_ID_WIFI_HOST_HMAC_CONFIG_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_APF_C,
    OAM_FILE_ID_HMAC_MBO_H,
    DIAG_FILE_ID_WIFI_DEV_ALG_AGGR_PROBE_ROM_C,
    OAM_FILE_ID_DMAC_TWT_STA_ROM_C,
    OAM_FILE_ID_FE_RF_CUSTOMIZE_POWER,
    OAM_FILE_ID_HMAC_CSI_H,
    DIAG_FILE_ID_WIFI_DEV_HAL_TX_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_MGMT_CLASSIFIER_C,
    DIAG_FILE_ID_WIFI_DEV_DMAC_WUR_AP_ROM_C,
    OAM_FILE_ID_HMAC_PROTECTION_H,
    OAM_FILE_ID_OAL_CFG80211_C,
    OAM_FILE_ID_HMAC_DFR_H,
    DIAG_FILE_ID_WIFI_DEV_DMAC_PSM_STA_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_CSI_C,
    OAM_FILE_ID_WIFI_DEV_HAL_PHY_ROM_C,
    DIAG_FILE_ID_WIFI_DEVICE_HAL_CSI_ROM_C,
    OAM_FILE_ID_HMAC_SMPS_H,
    OAM_FILE_ID_DMAC_STA_PM_H,
    OAM_FILE_ID_OAM_PROMISC_H,
    OAM_FILE_ID_HMAC_CHBA_FRAME_C,
    OAM_FILE_ID_HMAC_CHBA_FEATURE_H,
    DIAG_FILE_ID_WIFI_HOST_HMAC_BTCOEX_C,
    DIAG_FILE_ID_WIFI_DEVICE_DMAC_PSM_STA_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_WAL_HIPRIV_C,
    DIAG_FILE_ID_WIFI_HOST_ALG_INTRF_MODE_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_TX_AMSDU_C,
    OAM_FILE_ID_CALI_IQ,
    DIAG_FILE_ID_WIFI_HOST_HMAC_DFX_C,
    OAM_FILE_ID_HMAC_VAP_H,
    OAM_FILE_ID_DMAC_CRYPT_AES_C,
    OAM_FILE_ID_HMAC_CHBA_MGMT_H,
    DIAG_FILE_ID_WIFI_HOST_HMAC_ENCAP_FRAME_AP_C,
    DIAG_FILE_ID_WIFI_DEVICE_DMAC_CONFIG_H,
    OAM_FILE_ID_HMAC_CHBA_MGMT_C,
    OAM_FILE_ID_HMAC_WAPI_WPI_H,
    DIAG_FILE_ID_WIFI_DEV_HAL_INTF_DET_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_WAL_IOCTL_C,
    OAM_FILE_ID_HAL_CHAN_MGMT_C,
    OAM_FILE_ID_DMAC_DHCP_OFFLOAD_C,
    DIAG_FILE_ID_WIFI_HOST_FRW_TIMER_C,
    DIAG_FILE_ID_WIFI_HOST_OAL_SKBUFF_C,
    DIAG_FILE_ID_WIFI_DEV_DMAC_DFR_ROM_C,
    DIAG_FILE_ID_WIFI_DEV_DMAC_LATENCY_STAT_ROM_C,
    DIAG_FILE_ID_WIFI_DEV_ALG_TPC_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_MGMT_AP_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_DHCP_OFFLOAD_C,
    DIAG_FILE_ID_WIFI_DEVICE_OAL_FSM_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_TX_DATA_C,
    DIAG_FILE_ID_WIFI_HOST_MAC_REGDOMAIN_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_BTCOEX_NOTIFY_C,
    OAM_FILE_ID_HMAC_TCP_OPT_STRUC_H,
    OAM_FILE_ID_HMAC_DFS_H,
    DIAG_FILE_ID_WIFI_DEV_DMAC_INTF_DET_SCAN_ROM_C,
    OAM_FILE_ID_HMAC_CHBA_PS_H,
    DIAG_FILE_ID_WIFI_HOST_OAL_SCHEDULE_C,
    DIAG_FILE_ID_WIFI_DEV_HAL_RF_DEV_C,
    DIAG_FILE_ID_HCC_DEVICE_DFX_C,
    OAM_FILE_ID_WIFI_DEV_HAL_PM_C,
    OAM_FILE_ID_DMAC_TWT_STA_H,
    DIAG_FILE_ID_WIFI_DEV_ALG_MAIN_ROM_C,
    OAM_FILE_ID_CALI_PATH,
    OAM_FILE_ID_HAL_IRQ_H,
    DIAG_FILE_ID_WIFI_HOST_ALG_CCA_OPTIMIZE_C,
    DIAG_FILE_ID_WIFI_DEV_ALG_AUTORATE_IF_ROM_C,
    OAM_FILE_ID_HMAC_MAIN_H,
    OAM_FILE_ID_WIFI_DEV_HAL_IRQ_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_HAL_DSCR_ROM_C,
    OAM_FILE_ID_HMAC_ARP_OFFLOAD_H,
    OAM_FILE_ID_WAL_LITEOS_SDP_H,
    DIAG_FILE_ID_WIFI_HOST_WAL_CONFIG_SDP_C,
    OAM_FILE_ID_HAL_RF_ROM_C,
    OAM_FILE_ID_WAL_LINUX_CFGVENDOR_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_ISOLATION_C,
    DIAG_FILE_ID_WIFI_HOST_HAL_ALG_TXBF_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_P2P_C,
    DIAG_FILE_ID_WIFI_DEV_ALG_TPC_CONFIG_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_ALG_NOTIFY_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_RX_FILTER_C,
    DIAG_FILE_ID_WIFI_HOST_FRW_COMMON_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_BGLE_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_WDS_C,
    DIAG_FILE_ID_WIFI_DEV_ALG_ANTI_INTERFERENCE_ROM_C,
    OAM_FILE_ID_HMAC_TCP_OPT_H,
    OAM_FILE_ID_HMAC_CAR_H,
    DIAG_FILE_ID_WIFI_DEV_SLP_C,
    DIAG_FILE_ID_WIFI_DEV_HAL_DPD_C,
    DIAG_FILE_ID_WIFI_HOST_ALG_AGGR_C,
    DIAG_FILE_ID_WIFI_DEVICE_FRW_UTIL_ROM_C,
    DIAG_FILE_ID_WIFI_DEV_DMAC_TX_BSS_COMM_ROM_C,
    OAM_FILE_ID_WAL_LINUX_NETDEV_H,
    DIAG_FILE_ID_WIFI_HOST_OAL_MAIN_C,
    OAM_FILE_ID_HMAC_WAPI_SMS4_H,
    DIAG_FILE_ID_WIFI_DEV_HAL_DEVICE_C,
    DIAG_FILE_ID_WIFI_HOST_WIFI_DRIVER_API_C,
    DIAG_FILE_ID_WIFI_DEV_DMAC_TX_COMPLETE_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_OAL_NET_C,
    DIAG_FILE_ID_WIFI_DEV_ALG_AUTORATE_ROM_C,
    OAM_FILE_ID_WAL_LINUX_CFGVENDOR_H,
    OAM_FILE_ID_DMAC_CHBA_ROM_C,
    DIAG_FILE_ID_WIFI_DEVICE_DMAC_RX_DATA_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_WAL_MAIN_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_RX_DATA_C,
    DIAG_FILE_ID_WIFI_DEV_DMAC_RADAR_C,
    OAM_FILE_ID_HAL_COEX_REG_H,
    DIAG_FILE_ID_WIFI_HOST_OAL_NETBUF_C,
    DIAG_FILE_ID_WIFI_DEV_HAL_ONE_PKT_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_CALI_MGMT_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_CAR_C,
    OAM_FILE_ID_HAL_RESET_H,
    DIAG_FILE_ID_WIFI_DEVICE_FRW_DMAC_ROM_C,
    OAM_FILE_ID_HAL_VAP_C,
    OAM_FILE_ID_DMAC_RADAR_H,
    OAM_FILE_ID_WAL_LINUX_ANDROID_CMD_C,
    OAM_FILE_ID_HAL_RF_C,
    OAM_FILE_ID_HMAC_SCAN_H,
    OAM_FILE_ID_HAL_CALI_C,
    OAM_FILE_ID_WIFI_DEV_HAL_CHIP_ROM_C,
    OAM_FILE_ID_RSV2,
    DIAG_FILE_ID_WIFI_HOST_ALG_ANTI_INTERFERENCE_C,
    DIAG_FILE_ID_WIFI_HOST_WAL_CFG80211_C,
    DIAG_FILE_ID_WIFI_DEVICE_DMAC_TX_COMPLETE_RETRY_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_WIFI_MAIN_C,
    DIAG_FILE_ID_WIFI_HOST_MAC_RESOURCE_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_DEVICE_C,
    DIAG_FILE_ID_WIFI_DEV_OAL_FSM_ROM_C,
    OAM_FILE_ID_HMAC_FRAG_H,
    OAM_FILE_ID_WAL_LINUX_ANDROID_CMD_H,
    OAM_FILE_ID_HMAC_CONFIG_H,
    DIAG_FILE_ID_WIFI_DEV_ALG_TX_TB_ROM_C,
    OAM_FILE_ID_WAL_MAIN_C,
    OAM_FILE_ID_CALI_DEBUG,
    DIAG_FILE_ID_WIFI_HOST_HMAC_WMM_C,
    DIAG_FILE_ID_WIFI_DEVICE_DMAC_TX_QUEUE_ROM_C,
    DIAG_FILE_ID_WIFI_DEV_HAL_IRQ_C,
    DIAG_FILE_ID_WIFI_DEVICE_HAL_CSI_ROM_H,
    DIAG_FILE_ID_WIFI_DEV_HAL_DEVICE_ROM_C,
    OAM_FILE_ID_WIFI_HOST_HAL_POWER_ROM_C,
    DIAG_FILE_ID_WIFI_DEV_HAL_COEX_REG_C,
    OAM_FILE_ID_DMAC_TX_COMPLETE_ROM_H,
    DIAG_FILE_ID_WIFI_DEV_HAL_RX_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_HAL_ALG_CCA_C,
    OAM_FILE_ID_FE_RF_DEV_C,
    OAM_FILE_ID_HMAC_RX_DATA_H,
    OAM_FILE_ID_HMAC_CHBA_FUNCTION_H,
    DIAG_FILE_ID_WIFI_HOST_SDP_TEST_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_MGMT_STA_C,
    DIAG_FILE_ID_WIFI_DEV_FRW_DMAC_ADAPT_C,
    DIAG_FILE_ID_WIFI_HOST_FRW_HCC_ADAPT_C,
    DIAG_FILE_ID_WIFI_DEVICE_DMAC_PSM_STA_ROM_H,
    OAM_FILE_ID_WAL_LINUX_UTIL_H,
    DIAG_FILE_ID_WIFI_HOST_ALG_PROBE_COMMON_C,
    DIAG_FILE_ID_WIFI_HOST_WAL_CCPRIV_DEBUG_C,
    DIAG_FILE_ID_WIFI_DEV_ALG_AUTORATE_INTRF_MODE_ROM_C,
    OAM_FILE_ID_HAL_RF_DEV_ROM_C,
    OAM_FILE_ID_HMAC_TWT_H,
    DIAG_FILE_ID_WIFI_DEVICE_DMAC_BTCOEX_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_PROTECTION_C,
    OAM_FILE_ID_DMAC_HOOK_C,
    OAM_FILE_ID_HMAC_ROAM_CONNECT_H,
    DIAG_FILE_ID_WIFI_HOST_HMAC_BTCOEX_PS_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_BTCOEX_BA_C,
    OAM_FILE_ID_MAC_RESOURCE_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_WAL_WPA_IOCTL_C,
    OAM_FILE_ID_WAL_LINUX_VAP_PROC_C,
    OAM_FILE_ID_HMAC_CHAN_MGMT_H,
    OAM_FILE_ID_DMAC_ROM_PARAM_C,
    OAM_FILE_ID_FRW_DMAC_ADAPT_H,
    DIAG_FILE_ID_WIFI_DEV_DMAC_PSM_AP_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_WAL_COMMON_C,
    OAM_FILE_ID_OAM_PROMISC_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_M2U_C,
    OAM_FILE_ID_WAL_LINUX_CCPRIV_H,
    DIAG_FILE_ID_WIFI_HOST_MAC_VAP_C,
    DIAG_FILE_ID_WIFI_DEVICE_DMAC_DFX_ROM_C,
    OAM_FILE_ID_DMAC_TWT_STA_C,
    OAM_FILE_ID_HMAC_RX_FILTER_H,
    DIAG_FILE_ID_WIFI_DEV_ALG_HAL_DBAC_ROM_C,
    OAM_FILE_ID_DMAC_REKEY_OFFLOAD_C,
    DIAG_FILE_ID_WIFI_HOST_HAL_ANTI_INTF_C,
    OAM_FILE_ID_HMAC_CHAN_MEAS_H,
    DIAG_FILE_ID_WIFI_HOST_HAL_DEVICE_FSM_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_HAL_ALG_RTS_C,
    DIAG_FILE_ID_WIFI_DEV_BOARD_RF_CORIDC_C,
    DIAG_FILE_ID_WIFI_DEV_ALG_TEMP_PROTECT_ROM_C,
    OAM_FILE_ID_HMAC_SLP_C,
    OAM_FILE_ID_HAL_DEVICE_FSM_H,
    DIAG_FILE_ID_WIFI_DEV_ALG_DBAC_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_HAL_TBTT_C,
    OAM_FILE_ID_HMAC_CHBA_INTERFACE_H,
    DIAG_FILE_ID_WIFI_DEV_HAL_RESET_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_TX_ENCAP_C,
    DIAG_FILE_ID_WIFI_HOST_OAL_WORKQUEUE_C,
    DIAG_FILE_ID_WIFI_DEVICE_FRW_HCC_ADAPT_ROM_C,
    OAM_FILE_ID_WAL_CONFIG_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_REKEY_OFFLOAD_C,
    OAM_FILE_ID_WIFI_HOST_HAL_PHY_ROM_C,
    OAM_FILE_ID_HMAC_SME_STA_H,
    DIAG_FILE_ID_WIFI_DEVICE_FRW_DMAC_HCC_ADAPT_ROM_C,
    OAM_FILE_ID_HAL_CSI_H,
    DIAG_FILE_ID_WIFI_DEVICE_MAIN_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_WAPI_C,
    DIAG_FILE_ID_WIFI_HOST_ALG_AUTORATE_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_ROAM_CONNECT_C,
    DIAG_FILE_ID_WIFI_DEV_DMAC_RX_DATA_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_CSA_STA_C,
    OAM_FILE_ID_CALI_PROC_DMAC_MSG,
    OAM_FILE_ID_DMAC_RF_CALI_C,
    OAM_FILE_ID_DMAC_ROM_PARAM_H,
    OAM_FILE_ID_WAL_LINUX_IOCTL_C,
    DIAG_FILE_ID_WIFI_HOST_WAL_NET_BRIDGE_C,
    OAM_FILE_ID_DMAC_WOW_OFFLOAD_INIT_C,
    OAM_FILE_ID_CALI_ONLINE,
    OAM_FILE_ID_CALI_RX_DC,
    OAM_FILE_ID_HMAC_CHAN_MEAS_C,
    OAM_FILE_ID_HMAC_DFX_H,
    DIAG_FILE_ID_WIFI_DEV_DMAC_APF_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_AMPDU_CONFIG_C,
    DIAG_FILE_ID_WIFI_DEVICE_FRW_TIMER_ROM_C,
    OAM_FILE_ID_HAL_TBTT_H,
    OAM_FILE_ID_DMAC_CRYPT_SHA1_C,
    OAM_FILE_ID_HMAC_CHBA_COMMON_FUNCTION_H,
    DIAG_FILE_ID_WIFI_DEV_DMAC_TID_ROM_C,
    OAM_FILE_ID_HMAC_APF_H,
    OAM_FILE_ID_WAL_CONFIG_H,
    DIAG_FILE_ID_WIFI_DEVICE_HAL_PSD_C,
    OAM_FILE_ID_FRW_EXT_IF_ROM_H,
    OAM_FILE_ID_FRW_THREAD_H,
    OAM_FILE_ID_HMAC_USER_H,
    OAM_FILE_ID_WAL_REGDB_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_WAPI_WPI_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_MAIN_C,
    DIAG_FILE_ID_WIFI_DEV_SLP_H,
    OAM_FILE_ID_RSV3,
    OAM_FILE_ID_HMAC_CHBA_FRAME_H,
    DIAG_FILE_ID_WIFI_DEV_HAL_BCN_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_MAC_USER_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_TCP_OPT_C,
    OAM_FILE_ID_WAL_DFX_C,
    DIAG_FILE_ID_WIFI_DEV_HAL_DEVICE_FSM_C,
    OAM_FILE_ID_CALI_ACCUM_DATA,
    DIAG_FILE_ID_WIFI_HOST_ALG_TX_TB_C,
    OAM_FILE_ID_DMAC_ARP_OFFLOAD_C,
    OAM_FILE_ID_WAL_LINUX_NETDEV_C,
    DIAG_FILE_ID_WIFI_HOST_ALG_MAIN_C,
    DIAG_FILE_ID_WIFI_DEV_HAL_CORIDC_ROM_C,
    OAM_FILE_ID_HAL_MAIN_C,
    DIAG_FILE_ID_WIFI_DEV_ALG_DBAC_TXRX_ROM_C,
    OAM_FILE_ID_HMAC_PSM_AP_H,
    DIAG_FILE_ID_WIFI_DEVICE_DMAC_ALRX_ROM_C,
    OAM_FILE_ID_CALI_ENTRY,
    DIAG_FILE_ID_WIFI_DEV_HAL_CALI_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_SIGMA_TRAFFIC_C,
    OAM_FILE_ID_HISI_CUSTOMIZE_WIFI_C,
    OAM_FILE_ID_HMAC_CHBA_CHAN_SWITCH_H,
    OAM_FILE_ID_WAL_LINUX_CFG80211_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_DATA_ACQ_C,
    DIAG_FILE_ID_WIFI_DEVICE_HAL_COEX_REG_C,
    OAM_FILE_ID_OAL_QUEUE_H,
    OAM_FILE_ID_CALI_TX_DC,
    DIAG_FILE_ID_WIFI_HOST_HAL_PSD_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_PROXY_ARP_C,
    DIAG_FILE_ID_WIFI_DEV_HAL_DSCR_ROM_C,
    OAM_FILE_ID_HMAC_TRAFFIC_CLASSIFY_H,
    OAM_FILE_ID_OAL_NET_C,
    OAM_FILE_ID_HMAC_CHBA_FUNCTION_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_RX_DATA_FILTER_C,
    DIAG_FILE_ID_WIFI_DEV_ALG_RTS_ROM_C,
    OAM_FILE_ID_FE_RF_CUSTOMIZE,
    OAM_FILE_ID_CALI_COMPLEX_NUM,
    DIAG_FILE_ID_WIFI_DEV_DMAC_P2P_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_TX_EVENT_C,
    DIAG_FILE_ID_WIFI_HOST_FRW_HMAC_C,
    DIAG_FILE_ID_WIFI_DEV_HAL_ALG_TXBF_ROM_C,
    OAM_FILE_ID_HAL_CSI_C,
    DIAG_FILE_ID_WIFI_HOST_HAL_CE_C,
    DIAG_FILE_ID_WIFI_HOST_WAL_SCAN_C,
    DIAG_FILE_ID_WIFI_DEV_DMAC_SDP_ROM_C,
    DIAG_FILE_ID_WIFI_DEV_ALG_GLA_ROM_C,
    DIAG_FILE_ID_WIFI_DEV_HAL_TPC_ROM_C,
    DIAG_FILE_ID_WIFI_DEVICE_HAL_COEX_REG_H,
    OAM_FILE_ID_HAL_WOW_OFFLOAD_H,
    DIAG_FILE_ID_WIFI_DEV_HAL_MAC_C,
    OAM_FILE_ID_HMAC_CHBA_FSM_C,
    OAM_FILE_ID_DMAC_UAPSD_STA_C,
    DIAG_FILE_ID_WIFI_DEV_HAL_ALG_RTS_ROM_C,
    OAM_FILE_ID_HAL_COEX_REG_ROM_C,
    OAM_FILE_ID_HMAC_11K_H,
    DIAG_FILE_ID_WIFI_HOST_HMAC_OPMODE_C,
    OAM_FILE_ID_WAL_LINUX_VAP_PROC_H,
    OAM_FILE_ID_HMAC_MGMT_STA_H,
    OAM_FILE_ID_DMAC_WOW_OFFLOAD_INIT_H,
    OAM_FILE_ID_HAL_DPD_H,
    OAM_FILE_ID_CALI_TX_DC_DEBUG,
    OAM_FILE_ID_HMAC_TX_DATA_H,
    OAM_FILE_ID_HMAC_WDS_H,
    OAM_FILE_ID_HAL_MSG_C,
    OAM_FILE_ID_CALI_IQ_TONE_ALG,
    OAM_FILE_ID_HMAC_LATENCY_STAT_H,
    DIAG_FILE_ID_HCC_DEVICE_USB_C,
    OAM_FILE_ID_HAL_PHY_C,
    OAM_FILE_ID_HAL_BCN_H,
    OAM_FILE_ID_HAL_MAC_C,
    OAM_FILE_ID_HAL_MAC_H,
    OAM_FILE_ID_HAL_WOW_OFFLOAD_C,
    OAM_FILE_ID_HAL_CALI_ROM_C,
    DIAG_FILE_ID_WIFI_DEV_ALG_DBAC_CHBA_ROM_C,
    OAM_FILE_ID_WAL_LINUX_SDP_H,
    DIAG_FILE_ID_DMAC_AP_ROM_C,
    OAM_FILE_ID_MAC_IE_H,
    OAM_FILE_ID_OAL_SDIO_DEV_C,
    OAM_FILE_ID_HMAC_MGMT_BSS_COMM_H,
    OAM_FILE_ID_RSV4,
    OAM_FILE_ID_HAL_CALI_H,
    DIAG_FILE_ID_WIFI_HOST_MAC_DATA_C,
    DIAG_FILE_ID_WIFI_DEV_HAL_ROM_CB_C,
    DIAG_FILE_ID_WIFI_HOST_WAL_CONFIG_C,
    DIAG_FILE_ID_WIFI_HOST_ALG_INTF_DET_C,
    DIAG_FILE_ID_WIFI_DEV_HAL_RF_ROM_C,
    OAM_FILE_ID_HAL_CHAN_MGMT_H,
    OAM_FILE_ID_WAL_LINUX_CFG80211_H,
    DIAG_FILE_ID_WIFI_DEV_HAL_DPD_ROM_C,
    DIAG_FILE_ID_WIFI_DEV_HAL_PHY_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_DNB_STA_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_TRAFFIC_CLASSIFY_C,
    DIAG_FILE_ID_DMAC_SMOOTH_PHASE_ROM_H,
    DIAG_FILE_ID_WIFI_HOST_HMAC_TWT_C,
    DIAG_FILE_ID_WIFI_DEV_HAL_CALI_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_RESOURCE_C,
    DIAG_FILE_ID_WIFI_HOST_WAL_LITEOS_SDP_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_WAPI_SMS4_C,
    DIAG_FILE_ID_WIFI_DEV_ALG_SCHEDULE_ROM_C,
    DIAG_FILE_ID_WIFI_DEVICE_HAL_DSCR_C,
    OAM_FILE_ID_HMAC_FTM_ROM_C,
    OAM_FILE_ID_WAL_CONFIG_ACS_H,
    DIAG_FILE_ID_WIFI_HOST_HMAC_USER_C,
    DIAG_FILE_ID_WIFI_HOST_WAL_11D_C,
    DIAG_FILE_ID_WIFI_HOST_ALG_TEMP_PROTECT_C,
    OAM_FILE_ID_DMAC_BLOCKACK_H,
    DIAG_FILE_ID_HCC_DEVICE_SDIO_C,
    OAM_FILE_ID_HMAC_FEATURE_DFT_H,
    DIAG_FILE_ID_DMAC_CSI_ROM_C,
    DIAG_FILE_ID_WIFI_DEVICE_DMAC_ROM_PARAM_H,
    OAM_FILE_ID_CALI_TX_PWR_C,
    DIAG_FILE_ID_WIFI_DEV_HAL_MAC_ROM_C,
    OAM_FILE_ID_MAC_CHBA_COMMON_H,
    OAM_FILE_ID_DMAC_HOOK_H,
    DIAG_FILE_ID_WIFI_DEVICE_DMAC_TX_DATA_ROM_C,
    OAM_FILE_ID_DMAC_CHBA_C,
    OAM_FILE_ID_HAL_RX_FILTER_C,
    DIAG_FILE_ID_WIFI_DEV_HAL_AUTOCALI_C,
    OAM_FILE_ID_HMAC_M2U_H,
    DIAG_FILE_ID_WIFI_DEV_ALG_EDCA_OPT_ROM_C,
    DIAG_FILE_ID_WIFI_DEV_ALG_SCHEDULE_TRAFFIC_CTL_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_DHCP_OFFLOAD_H,
    DIAG_FILE_ID_WIFI_DEV_ALG_DBAC_SOFTAP_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_MAC_IE_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_CHAN_MGMT_C,
    DIAG_FILE_ID_WIFI_DEV_DMAC_AUTO_ADJUST_FREQ_ROM_C,
    OAM_FILE_ID_HMAC_RESOURCE_H,
    DIAG_FILE_ID_WIFI_DEV_ALG_AGGR_ROM_C,
    OAM_FILE_ID_MAC_APF_H,
    DIAG_FILE_ID_WIFI_HOST_MAC_FRAME_C,
    OAM_FILE_ID_MAIN_H,
    OAM_FILE_ID_CALI_IQ_DEBUG,
    OAM_FILE_ID_HMAC_FSM_H,
    DIAG_FILE_ID_WIFI_HOST_HMAC_ENCAP_FRAME_STA_C,
    OAM_FILE_ID_WAL_CONFIG_DEBUG_C,
    OAM_FILE_ID_WAL_LINUX_CCPRIV_C,
    OAM_FILE_ID_HAL_COEX_REG_C,
    DIAG_FILE_ID_WIFI_DEV_HAL_PHY_C,
    DIAG_FILE_ID_WIFI_DEV_HAL_DEVICE_FSM_ROM_C,
    OAM_FILE_ID_HMAC_WAPI_H,
    OAM_FILE_ID_DMAC_WOW_C,
    DIAG_FILE_ID_WIFI_DEVICE_OAL_PKT_NETBUF_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_ROAM_MAIN_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_CSA_AP_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_ALG_CONFIG_C,
    OAM_FILE_ID_HAL_CALI_COMP_H,
    OAM_FILE_ID_HMAC_CHBA_COMMON_FUNCTION_C,
    DIAG_FILE_ID_WIFI_DEV_DMAC_WOW_OFFLOAD_ROM_C,
    OAM_FILE_ID_FE_RF_DEV_ATTACH_C,
    DIAG_FILE_ID_WIFI_DEV_HAL_MAIN_C,
    DIAG_FILE_ID_WIFI_HOST_ALG_CCA_INTRF_MODE_C,
    OAM_FILE_ID_RSV5,
    DIAG_FILE_ID_WIFI_HOST_ALG_RTS_C,
    OAM_FILE_ID_HMAC_CALI_MGMT_H,
    DIAG_FILE_ID_WIFI_DEV_HAL_DSCR_C,
    OAM_FILE_ID_HMAC_MGMT_AP_H,
    DIAG_FILE_ID_WIFI_HOST_HMAC_ROAM_ALG_C,
    OAM_FILE_ID_WAL_LINUX_WITP_CMD_C,
    OAM_FILE_ID_HMAC_KEEP_ALIVE_H,
    OAM_FILE_ID_CALI_DATA,
    OAM_FILE_ID_WAL_REGDB_H,
    OAM_FILE_ID_WAL_LINUX_RX_RSP_C,
    DIAG_FILE_ID_WIFI_DEV_DMAC_THRUPUT_TEST_ROM_C,
    OAM_FILE_ID_WAL_LINUX_SDP_C,
    OAM_FILE_ID_WIFI_HOST_HAL_PM_C,
    DIAG_FILE_ID_WIFI_DEV_DMAC_BTCOEX_C,
    OAM_FILE_ID_HMAC_EXT_IF_H,
    OAM_FILE_ID_HAL_INIT_C,
    DIAG_FILE_ID_WIFI_HMAC_TX_MPDU_ADAPT_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_FRAG_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_CCPRIV_C,
    OAM_FILE_ID_WAL_LINUX_WITP_CMD_H,
    OAM_FILE_ID_WIFI_HOST_HAL_DEVICE_ROM_C,
    OAM_FILE_ID_HMAC_CHBA_CHAN_SWITCH_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_UAPSD_STA_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_ANT_SEL_C,
    DIAG_FILE_ID_WIFI_HOST_ALG_TEMP_PROTECT_BASIC_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_11R_C,
    OAM_FILE_ID_HMAC_11R_H,
    DIAG_FILE_ID_WIFI_DEV_ALG_COMPATIBILITY_ROM_C,
    DIAG_FILE_ID_WIFI_DEVICE_HAL_RADAR_SENSOR_C,
    OAM_FILE_ID_HMAC_RADAR_SENSOR_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_STAT_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_SAMPLE_DAQ_PHY_C,
    DIAG_FILE_ID_WIFI_HOST_HAL_DAQ_PHY_C,
    DIAG_FILE_ID_WIFI_DEVICE_DMAC_FEATURE_DFT_ROM_C,
    DIAG_FILE_ID_WIFI_FE_CALI_C_COMP,
    DIAG_FILE_ID_WIFI_FE_CALI_IPA_CURRENT_COMP,
    DIAG_FILE_ID_WIFI_FE_CALI_IQ_COMP,
    DIAG_FILE_ID_WIFI_FE_CALI_LOGEN_COMP,
    DIAG_FILE_ID_WIFI_FE_CALI_R_COMP,
    DIAG_FILE_ID_WIFI_FE_CALI_RC_COMP,
    DIAG_FILE_ID_WIFI_FE_CALI_RX_DC_COMP,
    DIAG_FILE_ID_WIFI_FE_CALI_TX_DC_COMP,
    DIAG_FILE_ID_WIFI_FE_CALI_TX_PWR_COMP,
    DIAG_FILE_ID_WIFI_DEVICE_DMAC_DFT_C,
    DIAG_FILE_ID_WIFI_DEVICE_DMAC_DFX_INIT_C,
    DIAG_FILE_ID_WIFI_DEVICE_HAL_PM_PARSE_BCN_ROM_C,
    DIAG_FILE_ID_WIFI_DEVICE_HAL_PM_PARSE_BCN_C,
    DIAG_FILE_ID_WIFI_DEVICE_HAL_TBTT_OFFSET_PROBE_C,
    DIAG_FILE_ID_WIFI_DEVICE_HAL_REG_LOAD_STORE_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_AL_TX_RX_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_SINGLE_PROXYSTA_C,
    DIAG_FILE_ID_WIFI_DEV_ALG_AGGR_COM_ROM_C,
    DIAG_FILE_ID_WIFI_DEV_ALG_AGGR_PROBE_COM_ROM_C,
    DIAG_FILE_ID_WIFI_HOST_ALG_CCA_OPTIMIZE_COM_C,
    DIAG_FILE_ID_WIFI_HOST_ALG_EDCA_OPT_COM_C,
    DIAG_FILE_ID_WIFI_HOST_ALG_INTF_DET_NEG_PRF_C,
    DIAG_FILE_ID_WIFI_HOST_ALG_TEMP_PROTECT_COM_C,
    DIAG_FILE_ID_WIFI_HOST_ALG_TEMP_PROTECT_RAM_C,
    DIAG_FILE_ID_WIFI_HOST_ALG_INTF_DET_COM_C,
    DIAG_FILE_ID_WIFI_HOST_ALG_ANTI_INTF_COM_C,
    DIAG_FILE_ID_WIFI_HOST_ALG_INTF_DYN_BW_COM_C,
    DIAG_FILE_ID_WIFI_HOST_ALG_INTF_DET_FSM_COM_C,
    DIAG_FILE_ID_WIFI_HOST_ALG_CHAN_DET_COM_C,
    OAM_FILE_ID_FE_RF_CUSTOMIZE_POWER_CALI,
    OAM_FILE_ID_FE_RF_CUSTOMIZE_RSSI_COMP,
    OAM_FILE_ID_FE_EQUIPMENT,
    OAM_FILE_ID_FE_HAL_EFUSE_IF_C,
    DIAG_FILE_ID_WIFI_HMAC_OBSS_STA_C,
    DIAG_FILE_ID_WIFI_HMAC_OBSS_AP_C,
    DIAG_FILE_ID_WIFI_DEVICE_DMAC_PHY_EVENT_C,
    DIAG_FILE_ID_WIFI_DEVICE_HAL_PHY_EVENT_C,
    OAM_FILE_ID_RADAR_DRVIER_MGR_C,
    OAM_FILE_ID_RADAR_DRVIER_MGR_SPEC_C,
    OAM_FILE_ID_RADAR_FEATURE_MGR_C,
    OAM_FILE_ID_RADAR_SENSOR_ONE_RX_ANT_C,
    OAM_FILE_ID_RADAR_SENSOR_RX_GAIN_CALI_C,
    OAM_FILE_ID_RADAR_SUB_FRAME_DATA_POOL_MGR_C,
    OAM_FILE_ID_RADAR_EVENT_C,
    OAM_FILE_ID_RADAR_MIPS_C,
    OAM_FILE_ID_RADAR_FFT_C,
    OAM_FILE_ID_RADAR_AI_UTILS_C,
    OAM_FILE_ID_RADAR_PPL_UTILS_C,
    OAM_FILE_ID_RADAR_TARGET_MGR_C,
    OAM_FILE_ID_RADAR_CONFIRMER_C,
    OAM_FILE_ID_RADAR_CLEAN_CLUSTER_C,
    OAM_FILE_ID_RADAR_KALMAN_TRACKER_C,
    OAM_FILE_ID_RADAR_TARGET_FSM_C,
    OAM_FILE_ID_RADAR_PLOT_EXTRACTION_C,
    OAM_FILE_ID_RADAR_INTERPOLATION_C,
    DIAG_FILE_ID_HAL_WIFI_PATCH_C,
    DIAG_FILE_ID_DMAC_WIFI_PATCH_C,
    DIAG_FILE_ID_WIFI_HOST_HMAC_MFG_C,
    DIAG_FILE_ID_WIFI_DEV_ALG_AR_C,
    OAM_FILE_ID_RADAR_PPL_DATA_CUBE_MGC_C,

    DIAG_FILE_ID_MAX_BUFF = 1024
} log_file_list_enum_wifi_app_t;

#endif /* end of soc_diag_wdk.h */
