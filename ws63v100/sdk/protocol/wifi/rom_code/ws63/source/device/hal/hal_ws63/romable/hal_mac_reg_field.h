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
 * Description: Define all registers/tables.
 */

#ifndef __HAL_MAC_REG_FIELD_H__
#define __HAL_MAC_REG_FIELD_H__
#define HH503_MAC_CFG_RTS_RATE_CALC_AT_BASIC_RATE_LEN    1
#define HH503_MAC_CFG_RTS_RATE_CALC_AT_BASIC_RATE_OFFSET    5
#define HH503_MAC_CFG_RTS_RATE_CALC_AT_BASIC_RATE_MASK    0x20
#define HH503_MAC_CFG_BIP_REPLAY_FAIL_FLT_EN_LEN    1
#define HH503_MAC_CFG_BIP_REPLAY_FAIL_FLT_EN_OFFSET    31
#define HH503_MAC_CFG_BIP_REPLAY_FAIL_FLT_EN_MASK    0x80000000
#define HH503_MAC_CFG_CCMP_REPLAY_FAIL_FLT_EN_LEN    1
#define HH503_MAC_CFG_CCMP_REPLAY_FAIL_FLT_EN_OFFSET    26
#define HH503_MAC_CFG_CCMP_REPLAY_FAIL_FLT_EN_MASK    0x4000000
#define HH503_MAC_CFG_OTHER_CTRL_FRAME_FLT_EN_LEN    1
#define HH503_MAC_CFG_OTHER_CTRL_FRAME_FLT_EN_OFFSET    10
#define HH503_MAC_CFG_OTHER_CTRL_FRAME_FLT_EN_MASK    0x400
#define HH503_MAC_CFG_BCMC_MGMT_OTHER_BSS_FLT_EN_LEN    1
#define HH503_MAC_CFG_BCMC_MGMT_OTHER_BSS_FLT_EN_OFFSET    4
#define HH503_MAC_CFG_BCMC_MGMT_OTHER_BSS_FLT_EN_MASK    0x10
#define HH503_MAC_CFG_BCMC_DATA_OTHER_BSS_FLT_EN_LEN    1
#define HH503_MAC_CFG_BCMC_DATA_OTHER_BSS_FLT_EN_OFFSET    3
#define HH503_MAC_CFG_BCMC_DATA_OTHER_BSS_FLT_EN_MASK    0x8
#define HH503_MAC_CFG_VHT_OFDM_RXPLCP_DELAY_LEN    4
#define HH503_MAC_CFG_VHT_OFDM_RXPLCP_DELAY_OFFSET    12
#define HH503_MAC_CFG_VHT_OFDM_RXPLCP_DELAY_MASK    0xf000
#define HH503_MAC_CFG_HT_OFDM_RXPLCP_DELAY_LEN    4
#define HH503_MAC_CFG_HT_OFDM_RXPLCP_DELAY_OFFSET    8
#define HH503_MAC_CFG_HT_OFDM_RXPLCP_DELAY_MASK    0xf00
#define HH503_MAC_CFG_NON_HT_OFDM_RXPLCP_DELAY_LEN    4
#define HH503_MAC_CFG_NON_HT_OFDM_RXPLCP_DELAY_OFFSET    4
#define HH503_MAC_CFG_NON_HT_OFDM_RXPLCP_DELAY_MASK    0xf0
#define HH503_MAC_CFG_DSSS_RXPLCP_DELAY_LEN    4
#define HH503_MAC_CFG_DSSS_RXPLCP_DELAY_OFFSET    0
#define HH503_MAC_CFG_DSSS_RXPLCP_DELAY_MASK    0xf
#define HH503_MAC_CFG_DSSS_RX2TX_TURNAROUND_TIME_LEN    8
#define HH503_MAC_CFG_DSSS_RX2TX_TURNAROUND_TIME_OFFSET    24
#define HH503_MAC_CFG_DSSS_RX2TX_TURNAROUND_TIME_MASK    0xff000000
#define HH503_MAC_CFG_OFDM_RX2TX_TURNAROUND_TIME_LEN    8
#define HH503_MAC_CFG_OFDM_RX2TX_TURNAROUND_TIME_OFFSET    16
#define HH503_MAC_CFG_OFDM_RX2TX_TURNAROUND_TIME_MASK    0xff0000
#define HH503_MAC_CFG_MU_VO_AIFS_SLOT_NUM_LEN    4
#define HH503_MAC_CFG_MU_VO_AIFS_SLOT_NUM_OFFSET    28
#define HH503_MAC_CFG_MU_VO_AIFS_SLOT_NUM_MASK    0xf0000000
#define HH503_MAC_CFG_MU_VI_AIFS_SLOT_NUM_LEN    4
#define HH503_MAC_CFG_MU_VI_AIFS_SLOT_NUM_OFFSET    24
#define HH503_MAC_CFG_MU_VI_AIFS_SLOT_NUM_MASK    0xf000000
#define HH503_MAC_CFG_MU_BK_AIFS_SLOT_NUM_LEN    4
#define HH503_MAC_CFG_MU_BK_AIFS_SLOT_NUM_OFFSET    20
#define HH503_MAC_CFG_MU_BK_AIFS_SLOT_NUM_MASK    0xf00000
#define HH503_MAC_CFG_MU_BE_AIFS_SLOT_NUM_LEN    4
#define HH503_MAC_CFG_MU_BE_AIFS_SLOT_NUM_OFFSET    16
#define HH503_MAC_CFG_MU_BE_AIFS_SLOT_NUM_MASK    0xf0000
#define HH503_MAC_CFG_MU_VO_CWMAX_LEN    4
#define HH503_MAC_CFG_MU_VO_CWMAX_OFFSET    28
#define HH503_MAC_CFG_MU_VO_CWMAX_MASK    0xf0000000
#define HH503_MAC_CFG_MU_VO_CWMIN_LEN    4
#define HH503_MAC_CFG_MU_VO_CWMIN_OFFSET    24
#define HH503_MAC_CFG_MU_VO_CWMIN_MASK    0xf000000
#define HH503_MAC_CFG_MU_VI_CWMAX_LEN    4
#define HH503_MAC_CFG_MU_VI_CWMAX_OFFSET    20
#define HH503_MAC_CFG_MU_VI_CWMAX_MASK    0xf00000
#define HH503_MAC_CFG_MU_VI_CWMIN_LEN    4
#define HH503_MAC_CFG_MU_VI_CWMIN_OFFSET    16
#define HH503_MAC_CFG_MU_VI_CWMIN_MASK    0xf0000
#define HH503_MAC_CFG_MU_BK_CWMAX_LEN    4
#define HH503_MAC_CFG_MU_BK_CWMAX_OFFSET    12
#define HH503_MAC_CFG_MU_BK_CWMAX_MASK    0xf000
#define HH503_MAC_CFG_MU_BK_CWMIN_LEN    4
#define HH503_MAC_CFG_MU_BK_CWMIN_OFFSET    8
#define HH503_MAC_CFG_MU_BK_CWMIN_MASK    0xf00
#define HH503_MAC_CFG_MU_BE_CWMAX_LEN    4
#define HH503_MAC_CFG_MU_BE_CWMAX_OFFSET    4
#define HH503_MAC_CFG_MU_BE_CWMAX_MASK    0xf0
#define HH503_MAC_CFG_MU_BE_CWMIN_LEN    4
#define HH503_MAC_CFG_MU_BE_CWMIN_OFFSET    0
#define HH503_MAC_CFG_MU_BE_CWMIN_MASK    0xf
#define HH503_MAC_CFG_AC_BK_MU_EDCA_TIMER_LEN    8
#define HH503_MAC_CFG_AC_BK_MU_EDCA_TIMER_OFFSET    24
#define HH503_MAC_CFG_AC_BK_MU_EDCA_TIMER_MASK    0xff000000
#define HH503_MAC_CFG_AC_BE_MU_EDCA_TIMER_LEN    8
#define HH503_MAC_CFG_AC_BE_MU_EDCA_TIMER_OFFSET    16
#define HH503_MAC_CFG_AC_BE_MU_EDCA_TIMER_MASK    0xff0000
#define HH503_MAC_CFG_AC_VI_MU_EDCA_TIMER_LEN    8
#define HH503_MAC_CFG_AC_VI_MU_EDCA_TIMER_OFFSET    8
#define HH503_MAC_CFG_AC_VI_MU_EDCA_TIMER_MASK    0xff00
#define HH503_MAC_CFG_AC_VO_MU_EDCA_TIMER_LEN    8
#define HH503_MAC_CFG_AC_VO_MU_EDCA_TIMER_OFFSET    0
#define HH503_MAC_CFG_AC_VO_MU_EDCA_TIMER_MASK    0xff
#define HH503_MAC_CFG_NAV_BYPASS_EN_LEN    1
#define HH503_MAC_CFG_NAV_BYPASS_EN_OFFSET    10
#define HH503_MAC_CFG_NAV_BYPASS_EN_MASK    0x400
#define HH503_MAC_CFG_P2P_VAP_SEL_LEN    2
#define HH503_MAC_CFG_P2P_VAP_SEL_OFFSET    2
#define HH503_MAC_CFG_P2P_VAP_SEL_MASK    0xc
#define HH503_MAC_CFG_P2P_APP_PS_EN_LEN    1
#define HH503_MAC_CFG_P2P_APP_PS_EN_OFFSET    0
#define HH503_MAC_CFG_P2P_APP_PS_EN_MASK    0x1
#define HH503_MAC_CFG_RX_DYN_BW_SEL_LEN    1
#define HH503_MAC_CFG_RX_DYN_BW_SEL_OFFSET    3
#define HH503_MAC_CFG_RX_DYN_BW_SEL_MASK    0x8
#define HH503_MAC_CFG_BSS_COLOR_DISALLOW_LEN    1
#define HH503_MAC_CFG_BSS_COLOR_DISALLOW_OFFSET    11
#define HH503_MAC_CFG_BSS_COLOR_DISALLOW_MASK    0x800
#define HH503_MAC_CFG_OBSS_PD_SR_TRIG_DIS_LEN    1
#define HH503_MAC_CFG_OBSS_PD_SR_TRIG_DIS_OFFSET    10
#define HH503_MAC_CFG_OBSS_PD_SR_TRIG_DIS_MASK    0x400
#define HH503_MAC_CFG_OBSS_PD_EN_SEL_LEN    1
#define HH503_MAC_CFG_OBSS_PD_EN_SEL_OFFSET    9
#define HH503_MAC_CFG_OBSS_PD_EN_SEL_MASK    0x200
#define HH503_MAC_CFG_SRG_PARTIAL_BSSID_EN_LEN    1
#define HH503_MAC_CFG_SRG_PARTIAL_BSSID_EN_OFFSET    5
#define HH503_MAC_CFG_SRG_PARTIAL_BSSID_EN_MASK    0x20
#define HH503_MAC_CFG_SRG_BSS_COLOR_EN_LEN    1
#define HH503_MAC_CFG_SRG_BSS_COLOR_EN_OFFSET    4
#define HH503_MAC_CFG_SRG_BSS_COLOR_EN_MASK    0x10
#define HH503_MAC_CFG_SRG_ELEMENT_VLD_LEN    1
#define HH503_MAC_CFG_SRG_ELEMENT_VLD_OFFSET    3
#define HH503_MAC_CFG_SRG_ELEMENT_VLD_MASK    0x8
#define HH503_MAC_CFG_SRG_PPDU_CHK_EN_LEN    1
#define HH503_MAC_CFG_SRG_PPDU_CHK_EN_OFFSET    2
#define HH503_MAC_CFG_SRG_PPDU_CHK_EN_MASK    0x4
#define HH503_MAC_CFG_OBSS_PD_BASE_CHK_EN_LEN    1
#define HH503_MAC_CFG_OBSS_PD_BASE_CHK_EN_OFFSET    1
#define HH503_MAC_CFG_OBSS_PD_BASE_CHK_EN_MASK    0x2
#define HH503_MAC_CFG_OBSS_PD_SPR_EN_LEN    1
#define HH503_MAC_CFG_OBSS_PD_SPR_EN_OFFSET    0
#define HH503_MAC_CFG_OBSS_PD_SPR_EN_MASK    0x1
#define HH503_MAC_CFG_PSR_SR_PWR_OFFSET_LEN    8
#define HH503_MAC_CFG_PSR_SR_PWR_OFFSET_OFFSET    8
#define HH503_MAC_CFG_PSR_SR_PWR_OFFSET_MASK    0xff00
#define HH503_MAC_CFG_PSR_PWR_SW_CALC_LEN    1
#define HH503_MAC_CFG_PSR_PWR_SW_CALC_OFFSET    7
#define HH503_MAC_CFG_PSR_PWR_SW_CALC_MASK    0x80
#define HH503_MAC_CFG_PSR_SR_RPT_START_LEN    1
#define HH503_MAC_CFG_PSR_SR_RPT_START_OFFSET    15
#define HH503_MAC_CFG_PSR_SR_RPT_START_MASK    0x8000
#define HH503_MAC_CFG_PSR_SR_RPT_CLR_LEN    1
#define HH503_MAC_CFG_PSR_SR_RPT_CLR_OFFSET    13
#define HH503_MAC_CFG_PSR_SR_RPT_CLR_MASK    0x2000
#define HH503_MAC_CFG_OBSS_PD_RPT_START_LEN    1
#define HH503_MAC_CFG_OBSS_PD_RPT_START_OFFSET    9
#define HH503_MAC_CFG_OBSS_PD_RPT_START_MASK    0x200
#define HH503_MAC_CFG_OBSS_PD_RPT_END_LEN    1
#define HH503_MAC_CFG_OBSS_PD_RPT_END_OFFSET    8
#define HH503_MAC_CFG_OBSS_PD_RPT_END_MASK    0x100
#define HH503_MAC_CFG_SR_DISALLOW_NAV_EN_LEN    1
#define HH503_MAC_CFG_SR_DISALLOW_NAV_EN_OFFSET    6
#define HH503_MAC_CFG_SR_DISALLOW_NAV_EN_MASK    0x40
#define HH503_MAC_CFG_OBSS_PD_BASED_TX_RATE_RANK_SEL_LEN    2
#define HH503_MAC_CFG_OBSS_PD_BASED_TX_RATE_RANK_SEL_OFFSET    4
#define HH503_MAC_CFG_OBSS_PD_BASED_TX_RATE_RANK_SEL_MASK    0x30
#define HH503_MAC_CFG_OBSS_PD_BASED_TX_RATE_RANK_EN_LEN    1
#define HH503_MAC_CFG_OBSS_PD_BASED_TX_RATE_RANK_EN_OFFSET    3
#define HH503_MAC_CFG_OBSS_PD_BASED_TX_RATE_RANK_EN_MASK    0x8
#define HH503_MAC_CFG_BSS_COLOR_RPT_CLR_LEN    1
#define HH503_MAC_CFG_BSS_COLOR_RPT_CLR_OFFSET    10
#define HH503_MAC_CFG_BSS_COLOR_RPT_CLR_MASK    0x400
#define HH503_MAC_CFG_BSS_COLOR_RPT_EN_LEN    1
#define HH503_MAC_CFG_BSS_COLOR_RPT_EN_OFFSET    9
#define HH503_MAC_CFG_BSS_COLOR_RPT_EN_MASK    0x200
#define HH503_MAC_CFG_OBSS_PD_EN_OFFSET_LEN    8
#define HH503_MAC_CFG_OBSS_PD_EN_OFFSET_OFFSET    24
#define HH503_MAC_CFG_OBSS_PD_EN_OFFSET_MASK    0xff000000
#define HH503_MAC_CFG_OBSS_PD_SRG_TX_POWER_LEN    8
#define HH503_MAC_CFG_OBSS_PD_SRG_TX_POWER_OFFSET    24
#define HH503_MAC_CFG_OBSS_PD_SRG_TX_POWER_MASK    0xff000000
#define HH503_MAC_CFG_OBSS_PD_SR_TX_POWER_LEN    8
#define HH503_MAC_CFG_OBSS_PD_SR_TX_POWER_OFFSET    16
#define HH503_MAC_CFG_OBSS_PD_SR_TX_POWER_MASK    0xff0000
#define HH503_MAC_CFG_SRG_OBSS_PD_LV_LEN    8
#define HH503_MAC_CFG_SRG_OBSS_PD_LV_OFFSET    8
#define HH503_MAC_CFG_SRG_OBSS_PD_LV_MASK    0xff00
#define HH503_MAC_CFG_OBSS_PD_LV_LEN    8
#define HH503_MAC_CFG_OBSS_PD_LV_OFFSET    0
#define HH503_MAC_CFG_OBSS_PD_LV_MASK    0xff
#define HH503_MAC_CFG_VAP0_MACADDR_H_LEN    16
#define HH503_MAC_CFG_VAP0_MACADDR_H_OFFSET    16
#define HH503_MAC_CFG_VAP0_MACADDR_H_MASK    0xffff0000
#define HH503_MAC_CFG_VAP0_BSSID_H_LEN    16
#define HH503_MAC_CFG_VAP0_BSSID_H_OFFSET    0
#define HH503_MAC_CFG_VAP0_BSSID_H_MASK    0xffff
#define HH503_MAC_CFG_VAP0_BSSID_L_LEN    32
#define HH503_MAC_CFG_VAP0_BSSID_L_OFFSET    0
#define HH503_MAC_CFG_VAP0_BSSID_L_MASK    0xffffffff
#define HH503_MAC_CFG_VAP0_BCN_FAIL_TX_BC_Q_EN_LEN    1
#define HH503_MAC_CFG_VAP0_BCN_FAIL_TX_BC_Q_EN_OFFSET    8
#define HH503_MAC_CFG_VAP0_BCN_FAIL_TX_BC_Q_EN_MASK    0x100
#define HH503_MAC_CFG_VAP0_TX_BCN_SUSPEND_LEN    1
#define HH503_MAC_CFG_VAP0_TX_BCN_SUSPEND_OFFSET    7
#define HH503_MAC_CFG_VAP0_TX_BCN_SUSPEND_MASK    0x80
#define HH503_MAC_CFG_VAP0_OFFSET_TBTT_VAL_LEN    16
#define HH503_MAC_CFG_VAP0_OFFSET_TBTT_VAL_OFFSET    0
#define HH503_MAC_CFG_VAP0_OFFSET_TBTT_VAL_MASK    0xffff
#define HH503_MAC_CFG_VAP0_PARTIAL_BSS_COLOR_LEN    4
#define HH503_MAC_CFG_VAP0_PARTIAL_BSS_COLOR_OFFSET    28
#define HH503_MAC_CFG_VAP0_PARTIAL_BSS_COLOR_MASK    0xf0000000
#define HH503_MAC_CFG_VAP0_AID_VAL_LEN    12
#define HH503_MAC_CFG_VAP0_AID_VAL_OFFSET    16
#define HH503_MAC_CFG_VAP0_AID_VAL_MASK    0xfff0000
#define HH503_MAC_CFG_VAP0_BSS_COLOR_LEN    6
#define HH503_MAC_CFG_VAP0_BSS_COLOR_OFFSET    9
#define HH503_MAC_CFG_VAP0_BSS_COLOR_MASK    0x7e00
#define HH503_MAC_CFG_VAP0_PARTIAL_AID_LEN    9
#define HH503_MAC_CFG_VAP0_PARTIAL_AID_OFFSET    0
#define HH503_MAC_CFG_VAP0_PARTIAL_AID_MASK    0x1ff
#define HH503_MAC_CFG_VAP1_BCN_FAIL_TX_BC_Q_EN_LEN    1
#define HH503_MAC_CFG_VAP1_BCN_FAIL_TX_BC_Q_EN_OFFSET    8
#define HH503_MAC_CFG_VAP1_BCN_FAIL_TX_BC_Q_EN_MASK    0x100
#define HH503_MAC_CFG_VAP2_BCN_FAIL_TX_BC_Q_EN_LEN    1
#define HH503_MAC_CFG_VAP2_BCN_FAIL_TX_BC_Q_EN_OFFSET    8
#define HH503_MAC_CFG_VAP2_BCN_FAIL_TX_BC_Q_EN_MASK    0x100
#define HH503_MAC_CFG_VAP0_TRANS_BSSID_H_LEN    16
#define HH503_MAC_CFG_VAP0_TRANS_BSSID_H_OFFSET    0
#define HH503_MAC_CFG_VAP0_TRANS_BSSID_H_MASK    0xffff
#define HH503_MAC_CFG_HE_BASICTRIG_RESP_INSERT_HTC_EN_LEN    1
#define HH503_MAC_CFG_HE_BASICTRIG_RESP_INSERT_HTC_EN_OFFSET    23
#define HH503_MAC_CFG_HE_BASICTRIG_RESP_INSERT_HTC_EN_MASK    0x800000
#define HH503_MAC_CFG_BASIC_RESP_DESC_INSERT_HTC_EN_LEN    1
#define HH503_MAC_CFG_BASIC_RESP_DESC_INSERT_HTC_EN_OFFSET    9
#define HH503_MAC_CFG_BASIC_RESP_DESC_INSERT_HTC_EN_MASK    0x200
#define HH503_MAC_CFG_HE_TRS_RESP_INSERT_HTC_EN_LEN    1
#define HH503_MAC_CFG_HE_TRS_RESP_INSERT_HTC_EN_OFFSET    25
#define HH503_MAC_CFG_HE_TRS_RESP_INSERT_HTC_EN_MASK    0x2000000
#define HH503_MAC_CFG_HE_BFRP_RESP_INSERT_HTC_EN_LEN    1
#define HH503_MAC_CFG_HE_BFRP_RESP_INSERT_HTC_EN_OFFSET    15
#define HH503_MAC_CFG_HE_BFRP_RESP_INSERT_HTC_EN_MASK    0x8000
#define HH503_MAC_CFG_BQRP_RESP_HTC_FLD_VAL_EN_LEN    1
#define HH503_MAC_CFG_BQRP_RESP_HTC_FLD_VAL_EN_OFFSET    16
#define HH503_MAC_CFG_BQRP_RESP_HTC_FLD_VAL_EN_MASK    0x10000
#define HH503_MAC_CFG_HE_BQRP_RESP_INSERT_HTC_EN_LEN    1
#define HH503_MAC_CFG_HE_BQRP_RESP_INSERT_HTC_EN_OFFSET    15
#define HH503_MAC_CFG_HE_BQRP_RESP_INSERT_HTC_EN_MASK    0x8000
#define HH503_MAC_RPT_INTR_HE_ROM_UPDATE_LEN    1
#define HH503_MAC_RPT_INTR_HE_ROM_UPDATE_OFFSET    31
#define HH503_MAC_RPT_INTR_HE_ROM_UPDATE_MASK    0x80000000
#define HH503_MAC_RPT_INTR_RX_TRIG_FRM_LEN    1
#define HH503_MAC_RPT_INTR_RX_TRIG_FRM_OFFSET    30
#define HH503_MAC_RPT_INTR_RX_TRIG_FRM_MASK    0x40000000
#define HH503_MAC_RPT_INTR_BCN_NO_FRM_LEN    1
#define HH503_MAC_RPT_INTR_BCN_NO_FRM_OFFSET    29
#define HH503_MAC_RPT_INTR_BCN_NO_FRM_MASK    0x20000000
#define HH503_MAC_RPT_INTR_FTM_LEN    1
#define HH503_MAC_RPT_INTR_FTM_OFFSET    28
#define HH503_MAC_RPT_INTR_FTM_MASK    0x10000000
#define HH503_MAC_RPT_INTR_VAP2_WAIT_BCN_TIMEOUT_LEN    1
#define HH503_MAC_RPT_INTR_VAP2_WAIT_BCN_TIMEOUT_OFFSET    23
#define HH503_MAC_RPT_INTR_VAP2_WAIT_BCN_TIMEOUT_MASK    0x800000
#define HH503_MAC_RPT_INTR_VAP1_WAIT_BCN_TIMEOUT_LEN    1
#define HH503_MAC_RPT_INTR_VAP1_WAIT_BCN_TIMEOUT_OFFSET    22
#define HH503_MAC_RPT_INTR_VAP1_WAIT_BCN_TIMEOUT_MASK    0x400000
#define HH503_MAC_RPT_INTR_VAP0_WAIT_BCN_TIMEOUT_LEN    1
#define HH503_MAC_RPT_INTR_VAP0_WAIT_BCN_TIMEOUT_OFFSET    21
#define HH503_MAC_RPT_INTR_VAP0_WAIT_BCN_TIMEOUT_MASK    0x200000
#define HH503_MAC_RPT_INTR_P2P_NOA_ABSENT_END_LEN    1
#define HH503_MAC_RPT_INTR_P2P_NOA_ABSENT_END_OFFSET    20
#define HH503_MAC_RPT_INTR_P2P_NOA_ABSENT_END_MASK    0x100000
#define HH503_MAC_RPT_INTR_P2P_NOA_ABSENT_START_LEN    1
#define HH503_MAC_RPT_INTR_P2P_NOA_ABSENT_START_OFFSET    19
#define HH503_MAC_RPT_INTR_P2P_NOA_ABSENT_START_MASK    0x80000
#define HH503_MAC_RPT_INTR_P2P_CT_WINDOW_END_LEN    1
#define HH503_MAC_RPT_INTR_P2P_CT_WINDOW_END_OFFSET    18
#define HH503_MAC_RPT_INTR_P2P_CT_WINDOW_END_MASK    0x40000
#define HH503_MAC_RPT_INTR_CH_STATISTIC_DONE_LEN    1
#define HH503_MAC_RPT_INTR_CH_STATISTIC_DONE_OFFSET    17
#define HH503_MAC_RPT_INTR_CH_STATISTIC_DONE_MASK    0x20000
#define HH503_MAC_RPT_INTR_MODE_SELECT_END_LEN    1
#define HH503_MAC_RPT_INTR_MODE_SELECT_END_OFFSET    16
#define HH503_MAC_RPT_INTR_MODE_SELECT_END_MASK    0x10000
#define HH503_MAC_RPT_INTR_ABORT_DONE_LEN    1
#define HH503_MAC_RPT_INTR_ABORT_DONE_OFFSET    15
#define HH503_MAC_RPT_INTR_ABORT_DONE_MASK    0x8000
#define HH503_MAC_RPT_INTR_ONE_PACKET_DONE_LEN    1
#define HH503_MAC_RPT_INTR_ONE_PACKET_DONE_OFFSET    14
#define HH503_MAC_RPT_INTR_ONE_PACKET_DONE_MASK    0x4000
#define HH503_MAC_RPT_INTR_CSI_DONE_LEN    1
#define HH503_MAC_RPT_INTR_CSI_DONE_OFFSET    13
#define HH503_MAC_RPT_INTR_CSI_DONE_MASK    0x2000
#define HH503_MAC_RPT_INTR_ABORT_END_LEN    1
#define HH503_MAC_RPT_INTR_ABORT_END_OFFSET    10
#define HH503_MAC_RPT_INTR_ABORT_END_MASK    0x400
#define HH503_MAC_RPT_INTR_ABORT_START_LEN    1
#define HH503_MAC_RPT_INTR_ABORT_START_OFFSET    9
#define HH503_MAC_RPT_INTR_ABORT_START_MASK    0x200
#define HH503_MAC_RPT_INTR_VAP0_TBTT_LEN    1
#define HH503_MAC_RPT_INTR_VAP0_TBTT_OFFSET    8
#define HH503_MAC_RPT_INTR_VAP0_TBTT_MASK    0x100
#define HH503_MAC_RPT_INTR_VAP1_TBTT_LEN    1
#define HH503_MAC_RPT_INTR_VAP1_TBTT_OFFSET    7
#define HH503_MAC_RPT_INTR_VAP1_TBTT_MASK    0x80
#define HH503_MAC_RPT_INTR_VAP2_TBTT_LEN    1
#define HH503_MAC_RPT_INTR_VAP2_TBTT_OFFSET    6
#define HH503_MAC_RPT_INTR_VAP2_TBTT_MASK    0x40
#define HH503_MAC_RPT_INTR_ERROR_LEN    1
#define HH503_MAC_RPT_INTR_ERROR_OFFSET    3
#define HH503_MAC_RPT_INTR_ERROR_MASK    0x8
#define HH503_MAC_RPT_INTR_RX_HIPRI_COMPLETE_LEN    1
#define HH503_MAC_RPT_INTR_RX_HIPRI_COMPLETE_OFFSET    2
#define HH503_MAC_RPT_INTR_RX_HIPRI_COMPLETE_MASK    0x4
#define HH503_MAC_RPT_INTR_TX_COMPLETE_LEN    1
#define HH503_MAC_RPT_INTR_TX_COMPLETE_OFFSET    1
#define HH503_MAC_RPT_INTR_TX_COMPLETE_MASK    0x2
#define HH503_MAC_RPT_INTR_RX_NORM_COMPLETE_LEN    1
#define HH503_MAC_RPT_INTR_RX_NORM_COMPLETE_OFFSET    0
#define HH503_MAC_RPT_INTR_RX_NORM_COMPLETE_MASK    0x1
#define HH503_MAC_RPT_ERR_OBSS_NAV_PROT_LEN    1
#define HH503_MAC_RPT_ERR_OBSS_NAV_PROT_OFFSET    12
#define HH503_MAC_RPT_ERR_OBSS_NAV_PROT_MASK    0x1000
#define HH503_MAC_RPT_ERR_BSS_NAV_PROT_LEN    1
#define HH503_MAC_RPT_ERR_BSS_NAV_PROT_OFFSET    11
#define HH503_MAC_RPT_ERR_BSS_NAV_PROT_MASK    0x800
#define HH503_MAC_HE_HTC_INTR_TYPE_LEN    4
#define HH503_MAC_HE_HTC_INTR_TYPE_OFFSET    16
#define HH503_MAC_HE_HTC_INTR_TYPE_MASK    0xf0000
#define HH503_MAC_RPT_HE_ROM_VAP_INDEX_LEN    3
#define HH503_MAC_RPT_HE_ROM_VAP_INDEX_OFFSET    8
#define HH503_MAC_RPT_HE_ROM_VAP_INDEX_MASK    0x700
#define HH503_MAC_TX_HI_PRI_MPDU_CNT_LEN    16
#define HH503_MAC_TX_HI_PRI_MPDU_CNT_OFFSET    16
#define HH503_MAC_TX_HI_PRI_MPDU_CNT_MASK    0xffff0000
#define HH503_MAC_TX_NORMAL_PRI_MPDU_CNT_LEN    16
#define HH503_MAC_TX_NORMAL_PRI_MPDU_CNT_OFFSET    0
#define HH503_MAC_TX_NORMAL_PRI_MPDU_CNT_MASK    0xffff
#define HH503_MAC_TX_MPDU_INAMPDU_COUNT_LEN    16
#define HH503_MAC_TX_MPDU_INAMPDU_COUNT_OFFSET    0
#define HH503_MAC_TX_MPDU_INAMPDU_COUNT_MASK    0xffff
#define HH503_MAC_TKIP_REPLAY_FAIL_CNT_LEN    16
#define HH503_MAC_TKIP_REPLAY_FAIL_CNT_OFFSET    16
#define HH503_MAC_TKIP_REPLAY_FAIL_CNT_MASK    0xffff0000
#define HH503_MAC_CCMP_REPLAY_FAIL_CNT_LEN    16
#define HH503_MAC_CCMP_REPLAY_FAIL_CNT_OFFSET    0
#define HH503_MAC_CCMP_REPLAY_FAIL_CNT_MASK    0xffff
#define HH503_MAC_RPT_RX_TKIP_MIC_FAIL_CNT_LEN    16
#define HH503_MAC_RPT_RX_TKIP_MIC_FAIL_CNT_OFFSET    16
#define HH503_MAC_RPT_RX_TKIP_MIC_FAIL_CNT_MASK    0xffff0000
#define HH503_MAC_RPT_RX_CCMP_MIC_FAIL_CNT_LEN    16
#define HH503_MAC_RPT_RX_CCMP_MIC_FAIL_CNT_OFFSET    0
#define HH503_MAC_RPT_RX_CCMP_MIC_FAIL_CNT_MASK    0xffff
#define HH503_MAC_RPT_RX_KEY_SEARCH_FAIL_CNT_LEN    16
#define HH503_MAC_RPT_RX_KEY_SEARCH_FAIL_CNT_OFFSET    0
#define HH503_MAC_RPT_RX_KEY_SEARCH_FAIL_CNT_MASK    0xffff
#define HH503_MAC_RPT_SRG_62_68_CNT_LEN    16
#define HH503_MAC_RPT_SRG_62_68_CNT_OFFSET    16
#define HH503_MAC_RPT_SRG_62_68_CNT_MASK    0xffff0000
#define HH503_MAC_RPT_SRG_68_74_CNT_LEN    16
#define HH503_MAC_RPT_SRG_68_74_CNT_OFFSET    0
#define HH503_MAC_RPT_SRG_68_74_CNT_MASK    0xffff
#define HH503_MAC_RPT_SRG_74_78_CNT_LEN    16
#define HH503_MAC_RPT_SRG_74_78_CNT_OFFSET    16
#define HH503_MAC_RPT_SRG_74_78_CNT_MASK    0xffff0000
#define HH503_MAC_RPT_SRG_78_82_CNT_LEN    16
#define HH503_MAC_RPT_SRG_78_82_CNT_OFFSET    0
#define HH503_MAC_RPT_SRG_78_82_CNT_MASK    0xffff
#define HH503_MAC_RPT_NON_SRG_62_68_CNT_LEN    16
#define HH503_MAC_RPT_NON_SRG_62_68_CNT_OFFSET    16
#define HH503_MAC_RPT_NON_SRG_62_68_CNT_MASK    0xffff0000
#define HH503_MAC_RPT_NON_SRG_68_74_CNT_LEN    16
#define HH503_MAC_RPT_NON_SRG_68_74_CNT_OFFSET    0
#define HH503_MAC_RPT_NON_SRG_68_74_CNT_MASK    0xffff
#define HH503_MAC_RPT_NON_SRG_74_78_CNT_LEN    16
#define HH503_MAC_RPT_NON_SRG_74_78_CNT_OFFSET    16
#define HH503_MAC_RPT_NON_SRG_74_78_CNT_MASK    0xffff0000
#define HH503_MAC_RPT_NON_SRG_78_82_CNT_LEN    16
#define HH503_MAC_RPT_NON_SRG_78_82_CNT_OFFSET    0
#define HH503_MAC_RPT_NON_SRG_78_82_CNT_MASK    0xffff
#define HH503_MAC_RPT_OBSS_PD_TX_NUM_LEN    16
#define HH503_MAC_RPT_OBSS_PD_TX_NUM_OFFSET    16
#define HH503_MAC_RPT_OBSS_PD_TX_NUM_MASK    0xffff0000
#define HH503_MAC_RPT_OBSS_PD_TX_SUCCESS_NUM_LEN    16
#define HH503_MAC_RPT_OBSS_PD_TX_SUCCESS_NUM_OFFSET    0
#define HH503_MAC_RPT_OBSS_PD_TX_SUCCESS_NUM_MASK    0xffff
#define HH503_MAC_CFG_PEER_ADDR_LUT_OPER_EN_LEN    1
#define HH503_MAC_CFG_PEER_ADDR_LUT_OPER_EN_OFFSET    0
#define HH503_MAC_CFG_PEER_ADDR_LUT_OPER_EN_MASK    0x1
#define HH503_MAC_CFG_BA_LUT_OPER_EN_LEN    1
#define HH503_MAC_CFG_BA_LUT_OPER_EN_OFFSET    0
#define HH503_MAC_CFG_BA_LUT_OPER_EN_MASK    0x1
#define HH503_MAC_SOFT_RST_WL0_MAC_SLAVE_N_LEN    1
#define HH503_MAC_SOFT_RST_WL0_MAC_SLAVE_N_OFFSET    3
#define HH503_MAC_SOFT_RST_WL0_MAC_SLAVE_N_MASK    0x8
#define HH503_MAC_SOFT_RST_WL0_MAC_AON_N_LEN    1
#define HH503_MAC_SOFT_RST_WL0_MAC_AON_N_OFFSET    0
#define HH503_MAC_SOFT_RST_WL0_MAC_AON_N_MASK    0x1
#define HH503_MAC_TRIG_RESP_TPC_POWERMODE_MASK 0x7F
#define HH503_MAC_IS_TRS_OFFSET  8
#define HH503_MAC_IS_TRS_MASK 0x100
#define HH503_MAC_TRIG_TYPE_OFFSET  0
#define HH503_MAC_TRIG_TYPE_MASK 0xF
#define HH503_MAC_TRIG_UL_BW_OFFSET  18
#define HH503_MAC_TRIG_UL_BW_MASK 0xC0000
#define HH503_MAC_TRIG_AP_TX_POWER_LSB_OFFSET  28
#define HH503_MAC_TRIG_AP_TX_POWER_LSB_MASK 0xF0000000
#define HH503_MAC_TRIG_AP_TX_POWER_MSB_OFFSET  0
#define HH503_MAC_TRIG_AP_TX_POWER_MSB_MASK 0x3
#define HH503_MAC_TRIG_UL_HE_MCS_OFFSET  21
#define HH503_MAC_TRIG_UL_HE_MCS_MASK 0x1E00000
#define HH503_MAC_TRIG_UL_TARGET_RSSI_OFFSET  0
#define HH503_MAC_TRIG_UL_TARGET_RSSI_MASK 0x7F
#define HH503_MAC_ROM_INFO_BW_MASK 0x00000003
#define HH503_MAC_ROM_INFO_NSS_MASK 0x0000001C
#define HH503_MAC_ROM_INFO_NSS_OFFSET 2
#define HH503_MAC_NEW_ROM_INFO_CHANGE 13
#define HH503_MAC_ROM_INTR_ONE_MASK 0x2000
#define HH503_MAC_ORIGINAL_ROM_INFO_CHANGE 14
#define HH503_MAC_ROM_INTR_TWO_MASK 0x4000
#define MAC_CANCEL_TWT  0x2
#define MAC_NEW_TWT 0x1
#define MAC_CALI_TWT 0x3
#define HH503_MAC_CFG_RX_HE_ROM_HTC_TYPE_EN_MASK 0x6000
#define HH503_MAC_TX_DESC_BASE_ADDR_MASK 0x0fffff
#define HH503_MAC_TX_DSCR_HDR_MSDU_BASE_ADDR_LEN 20
#define HH503_MAC_RPT_TX_HI_MPDU_CNT_MASK 0xFFFF0000
#define HH503_MAC_RPT_TX_HI_MPDU_CNT_OFFSET 16
#define HH503_MAC_RPT_TX_NORMAL_MPDU_CNT_MASK 0x0000FFFF
#define HH503_MAC_RPT_TX_AMPDU_COUNT_MASK 0xFFFF0000
#define HH503_MAC_RPT_TX_AMPDU_COUNT_OFFSET 16
#define HH503_MAC_RPT_TX_MPDU_INAMPDU_COUNT_MASK 0x0000FFFF
#define HH503_MAC_RPT_RX_FILTERED_CNT_MASK 0x0000FFFF
#define HH503_MAC_TX_INTR_CNT_MASK 0x0000FFFF
#endif