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

#ifndef __WLAN_TYPES_H__
#define __WLAN_TYPES_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "wlan_spec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
/* ****************************************************************************
  2.1 基本宏定义
**************************************************************************** */
#define MAC_BYTE_ALIGN_VALUE                4           /* 4字节对齐 */
/* MAC帧头长度放入平台 */
/* 80211MAC帧头FC字段宏定义 */
#define WLAN_PROTOCOL_VERSION               0x00        /* 协议版本 */
#define WLAN_FC0_TYPE_MGT                   0x00        /* 管理帧 */
#define WLAN_FC0_TYPE_CTL                   0x04        /* 控制帧 */
#define WLAN_FC0_TYPE_DATA                  0x08        /* 数据帧 */

/* 管理帧subtype */
#define WLAN_FC0_SUBTYPE_ASSOC_REQ          0x00
#define WLAN_FC0_SUBTYPE_ASSOC_RSP          0x10
#define WLAN_FC0_SUBTYPE_REASSOC_REQ        0x20
#define WLAN_FC0_SUBTYPE_REASSOC_RSP        0x30
#define WLAN_FC0_SUBTYPE_PROBE_REQ          0x40
#define WLAN_FC0_SUBTYPE_PROBE_RSP          0x50
#define WLAN_FC0_SUBTYPE_BEACON             0x80
#define WLAN_FC0_SUBTYPE_ATIM               0x90
#define WLAN_FC0_SUBTYPE_DISASSOC           0xa0
#define WLAN_FC0_SUBTYPE_AUTH               0xb0
#define WLAN_FC0_SUBTYPE_DEAUTH             0xc0
#define WLAN_FC0_SUBTYPE_ACTION             0xd0
#define WLAN_FC0_SUBTYPE_ACTION_NO_ACK      0xe0

/* 控制帧subtype */
#define WLAN_FC0_SUBTYPE_NDPA               0x50
#define WLAN_FC0_SUBTYPE_CONTROL_WRAPPER    0x70        /* For TxBF RC */
#define WLAN_FC0_SUBTYPE_BAR                0x80
#define WLAN_FC0_SUBTYPE_BA                 0x90

#define WLAN_FC0_SUBTYPE_PS_POLL            0xa0
#define WLAN_FC0_SUBTYPE_RTS                0xb0
#define WLAN_FC0_SUBTYPE_CTS                0xc0
#define WLAN_FC0_SUBTYPE_ACK                0xd0
#define WLAN_FC0_SUBTYPE_CF_END             0xe0
#define WLAN_FC0_SUBTYPE_CF_END_ACK         0xf0

/* 数据帧subtype */
#define WLAN_FC0_SUBTYPE_DATA               0x00
#define WLAN_FC0_SUBTYPE_CF_ACK             0x10
#define WLAN_FC0_SUBTYPE_CF_POLL            0x20
#define WLAN_FC0_SUBTYPE_CF_ACPL            0x30
#define WLAN_FC0_SUBTYPE_NODATA             0x40
#define WLAN_FC0_SUBTYPE_CFACK              0x50
#define WLAN_FC0_SUBTYPE_CFPOLL             0x60
#define WLAN_FC0_SUBTYPE_CF_ACK_CF_ACK      0x70
#define WLAN_FC0_SUBTYPE_QOS                0x80
#define WLAN_FC0_SUBTYPE_QOS_NULL           0xc0

#define WLAN_FC1_DIR_MASK                   0x03
#define WLAN_FC1_DIR_NODS                   0x00        /* STA->STA */
#define WLAN_FC1_DIR_TODS                   0x01        /* STA->AP  */
#define WLAN_FC1_DIR_FROMDS                 0x02        /* AP ->STA */
#define WLAN_FC1_DIR_DSTODS                 0x03        /* AP ->AP  */

#define WLAN_FC1_MORE_FRAG                  0x04
#define WLAN_FC1_RETRY                      0x08
#define WLAN_FC1_PWR_MGT                    0x10
#define WLAN_FC1_MORE_DATA                  0x20
#define WLAN_FC1_WEP                        0x40
#define WLAN_FC1_ORDER                      0x80

#define WLAN_HDR_DUR_OFFSET                 2           /* duartion相对于mac头的字节偏移 */
#define WLAN_HDR_ADDR1_OFFSET               4           /* addr1相对于mac头的字节偏移 */
#define WLAN_HDR_ADDR2_OFFSET               10          /* addr2相对于mac头的字节偏移 */
#define WLAN_HDR_ADDR3_OFFSET               16          /* addr3相对于mac头的字节偏移 */
#define WLAN_HDR_FRAG_OFFSET                22          /* 分片序号相对于mac的字节偏移 */

#define WLAN_REASON_CODE_LEN                2

/* 帧头DS位 */
#define WLAN_FRAME_TO_AP                   0x0100
#define WLAN_FRAME_FROME_AP                0x0200
/* FCS长度(4字节) */
#define WLAN_HDR_FCS_LENGTH                 4

#define WLAN_MAX_BAR_DATA_LEN               20  /* BAR帧的最大长度 */
#define WLAN_CHTXT_SIZE                     128 /* challenge text的长度 */

#define WLAN_SEQ_SHIFT                      4
/* AMPDU Delimeter长度(4字节) */
#define WLAN_DELIMETER_LENGTH               4

/* 配置命令最大长度: 从算法名称开始算起，不包括"alg" */
#define DMAC_ALG_CONFIG_MAX_ARG     7

/* 信道切换计数 */
#define WLAN_CHAN_SWITCH_DEFAULT_CNT        6
#define WLAN_CHAN_SWITCH_DETECT_RADAR_CNT   1

/* 5G子频段数目 */
#define WLAN_5G_20M_SUB_BAND_NUM    7
#define WLAN_5G_80M_SUB_BAND_NUM    7
#define WLAN_5G_CALI_SUB_BAND_NUM   (WLAN_5G_20M_SUB_BAND_NUM + WLAN_5G_80M_SUB_BAND_NUM)

/* 单个通道的UPC数据寄存器数目 */
#define WLAN_UPC_DATA_REG_NUM       4

#define WLAN_FIELD_TYPE_AID            0xC000
#define wlan_aid(aid)                  ((aid) & ~0xc000)

#define WLAN_MAX_VAP_NUM            2
#define WLAN_2G_SUB_BAND_NUM        13
#define WLAN_SIFS_OFDM_POWLVL_NUM   4

#ifdef _PRE_WLAN_FEATURE_MESH_ROM
#define WLAN_MESH_ACTION_SELF_PROTECTED 15
#endif

#define WLAN_ACTION_BODY_CATEGORY_OFFSET      0          /* Action帧的category字段相对于frame body的字节偏移 */
#define WLAN_ACTION_BODY_ELEMENT_OFFSET       6          /* Action帧的element字段相对于frame body的字节偏移 */
#define WLAN_ACTION_BODY_OFFSET               24         /* Action帧的frame body相对于帧起始地址的字节偏移 */

#define WLAN_ACTION_CATEGORY_VENDOR_SPECIFIC  127         /* Action帧的类别为厂商自定义 */
#define WLAN_ACTION_TYPE_NOA                  1           /* Action帧的子类别为NOA */

/* 管理帧包含的固定长度信息元素的总长度 */
#define WLAN_FC0_ASSOC_REQ_MIN_LEN          4
#define WLAN_FC0_REASSOC_REQ_MIN_LEN        8
#define WLAN_FC0_BEACON_MIN_LEN             12
#define WLAN_FC0_DISASSOC_MIN_LEN           2
#define WLAN_FC0_AUTH_MIN_LEN               6
#define WLAN_FC0_DEAUTH_MIN_LEN             2
#define WLAN_FC0_ACTION_MIN_LEN             1

/* ****************************************************************************
  2.2 WME宏定义
**************************************************************************** */
/* TID最大个数,TBD 和 WLAN_TID_MAX_NUM归一 */
#define WLAN_WME_MAX_TID_NUM 8

#define wlan_wme_ac_to_tid(_ac) \
    (((_ac) == WLAN_WME_AC_VO) ? 6 : ((_ac) == WLAN_WME_AC_VI) ? 5 : ((_ac) == WLAN_WME_AC_BK) ? 1 : 0)

#define wlan_wme_tid_to_ac(_tid) \
    ((((_tid) == 0) || ((_tid) == 3)) ? WLAN_WME_AC_BE : \
    (((_tid) == 1) || ((_tid) == 2)) ? WLAN_WME_AC_BK : \
    (((_tid) == 4) || ((_tid) == 5)) ? WLAN_WME_AC_VI : WLAN_WME_AC_VO)

/* ****************************************************************************
  2.3 HT宏定义
**************************************************************************** */
/* 11n:
   Maximum A-MSDU Length Indicates maximum A-MSDU length.See 9.11. Set to 0 for 3839 octetsSet to 1 for 7935 octets */
/* 11AC(9.11): A VHT STA that sets the Maximum MPDU Length in the VHT Capabilities element to indicate 3895 octets
   shall set the Maximum A-MSDU Length in the HT Capabilities element to indicate 3839 octets. A VHT STA
   that sets the Maximum MPDU Length in the VHT Capabilities element to indicate 7991 octets or 11 454 oc-
   tets shall set the Maximum A-MSDU Length in the HT Capabilities element to indicate 7935 octets. */
#define WLAN_AMSDU_FRAME_MAX_LEN_SHORT      3839
#define WLAN_AMSDU_FRAME_MAX_LEN_LONG       7935

/* Maximum A-MSDU Length Indicates maximum A-MSDU length.See 9.11. Set to 0 for 3839 octetsSet to 1 for 7935 octets */
#define wlan_ht_get_amsdu_max_len(_bit_amsdu_max_length) \
    ((0 == (_bit_amsdu_max_length)) ? WLAN_AMSDU_FRAME_MAX_LEN_SHORT : WLAN_AMSDU_FRAME_MAX_LEN_LONG)

/* ****************************************************************************
  2.14 安全相关宏定义
**************************************************************************** */
/* cipher suite selectors */
#define WLAN_CIPHER_SUITE_USE_GROUP 0x000FAC00
#define WLAN_CIPHER_SUITE_WEP40     0x000FAC01
#define WLAN_CIPHER_SUITE_TKIP      0x000FAC02
    /* reserved:                0x000FAC03 */
#define WLAN_CIPHER_SUITE_CCMP      0x000FAC04
#define WLAN_CIPHER_SUITE_WEP104    0x000FAC05
#define WLAN_CIPHER_SUITE_AES_CMAC  0x000FAC06
#undef  WLAN_CIPHER_SUITE_SMS4
#define WLAN_CIPHER_SUITE_SMS4      0x00147201

/* AKM suite selectors */
#define WITP_WLAN_AKM_SUITE_8021X         0x000FAC01
#define WITP_WLAN_AKM_SUITE_PSK             0x000FAC02
#define WITP_WLAN_AKM_SUITE_WAPI_PSK     0x000FAC04
#define WITP_WLAN_AKM_SUITE_WAPI_CERT    0x000FAC12

#define WLAN_PMKID_LEN           16
#define WLAN_PMKID_CACHE_SIZE    4
#define WLAN_NONCE_LEN           32
#define WLAN_PTK_PREFIX_LEN      22
#define WLAN_GTK_PREFIX_LEN      19
#define WLAN_GTK_DATA_LEN        (NONCE_LEN + WLAN_MAC_ADDR_LEN)
#define WLAN_PTK_DATA_LEN        (2 * NONCE_LEN + 2 * WLAN_MAC_ADDR_LEN)

#define WLAN_KCK_LENGTH          16
#define WLAN_KEK_LENGTH          16
#define WLAN_TEMPORAL_KEY_LENGTH 16
#define WLAN_MIC_KEY_LENGTH      8

#define WLAN_PMK_SIZE        32 /* In Bytes */
#define WLAN_PTK_SIZE        64 /* In Bytes */
#define WLAN_GTK_SIZE        32 /* In Bytes */
#define WLAN_GMK_SIZE        32 /* In Bytes */

#define WLAN_WEP40_KEY_LEN              5
#define WLAN_WEP104_KEY_LEN             13
#define WLAN_TKIP_KEY_LEN               32      /* TKIP KEY length 256 BIT */
#define WLAN_CCMP_KEY_LEN               16      /* CCMP KEY length 128 BIT */
#define WLAN_BIP_KEY_LEN                16      /* BIP KEY length 128 BIT */

#define WLAN_NUM_DOT11WEPDEFAULTKEYVALUE       4
#define WLAN_MAX_WEP_STR_SIZE                  27 /* 5 for WEP 40; 13 for WEP 104 */
#define WLAN_WEP_SIZE_OFFSET                   0
#define WLAN_WEP_KEY_VALUE_OFFSET              1

#define WLAN_COUNTRY_STR_LEN 3

/* crypto status */
#define WLAN_ENCRYPT_BIT        (1 << 0)
#define WLAN_WEP_BIT            (1 << 1)
#define WLAN_WEP104_BIT         ((1 << 2) | (1 << 1))
#define WLAN_WPA_BIT            (1 << 3)
#define WLAN_WPA2_BIT           (1 << 4)
#define WLAN_CCMP_BIT           (1 << 5)
#define WLAN_TKIP_BIT           (1 << 6)

#define WLAN_WAPI_BIT           (1 << 5)

/* auth */
#define WLAN_OPEN_SYS_BIT       (1 << 0)
#define WLAN_SHARED_KEY_BIT     (1 << 1)
#define WLAN_8021X_BIT          (1 << 2)

#define WLAN_WITP_CAPABILITY_PRIVACY BIT4
#define WLAN_MESH_CCMP_COUNTER_LEN 6

/* ****************************************************************************
  3 枚举定义
**************************************************************************** */
/* ****************************************************************************
  3.1 基本枚举类型
**************************************************************************** */
/* TID编号类别放入平台 */
/* VAP的工作模式 */
typedef enum {
    WLAN_VAP_MODE_CONFIG,        /* 配置模式 */
    WLAN_VAP_MODE_BSS_STA,       /* BSS STA模式 */
    WLAN_VAP_MODE_BSS_AP,        /* BSS AP模式 */
    WLAN_VAP_MODE_WDS,           /* WDS模式 */
    WLAN_VAP_MODE_MONITOER,      /* 侦听模式 */
    WLAN_VAP_MODE_MESH,          /* MBSS MESH模式 */
    WLAN_VAP_HW_TEST,

    WLAN_VAP_MODE_BUTT
} wlan_vap_mode_enum;
typedef hi_u8 wlan_vap_mode_enum_uint8;

/* 认证的transaction number */
typedef enum {
    WLAN_AUTH_TRASACTION_NUM_ONE    = 0x0001,
    WLAN_AUTH_TRASACTION_NUM_TWO    = 0x0002,
    WLAN_AUTH_TRASACTION_NUM_THREE  = 0x0003,
    WLAN_AUTH_TRASACTION_NUM_FOUR   = 0x0004,

    WLAN_AUTH_TRASACTION_NUM_BUTT
} wlan_auth_transaction_number_enum;
typedef hi_u8 wlan_auth_transaction_number_enum_uint8;
typedef enum {
    WLAN_BAND_2G,
    WLAN_BAND_BUTT
} wlan_channel_band_enum;
typedef hi_u8 wlan_channel_band_enum_uint8;

/* Protection mode for MAC */
typedef enum {
    WLAN_PROT_NO,  /* Do not use any protection       */
    WLAN_PROT_ERP, /* Protect all ERP frame exchanges */
    WLAN_PROT_HT,  /* Protect all HT frame exchanges  */
    WLAN_PROT_GF,  /* Protect all GF frame exchanges  */

    WLAN_PROT_BUTT
} wlan_prot_mode_enum;
typedef hi_u8 wlan_prot_mode_enum_uint8;

typedef enum {
    WLAN_RTS_RATE_SELECT_MODE_REG,  /* 0: RTS速率 = PROT_DATARATE的配置                           */
    WLAN_RTS_RATE_SELECT_MODE_DESC, /* 1: RTS速率 = 硬件根据TX描述符计算出的值                    */
    WLAN_RTS_RATE_SELECT_MODE_MIN,  /* 2: RTS速率 = min(PROT_DATARATE,硬件根据TX描述符计算出的值) */
    WLAN_RTS_RATE_SELECT_MODE_MAX,  /* 3: RTS速率 = max(PROT_DATARATE,硬件根据TX描述符计算出的值) */

    WLAN_RTS_RATE_SELECT_MODE_BUTT
} wlan_rts_rate_select_mode_enum;

typedef enum {
    WLAN_WITP_AUTH_OPEN_SYSTEM = 0,
    WLAN_WITP_AUTH_SHARED_KEY,
    WLAN_WITP_AUTH_FT,
    WLAN_WITP_AUTH_SAE = 3,
    WLAN_WITP_AUTH_NETWORK_EAP, /* 802.11-2016协议查得认证帧中Auth Alg mode 字段3为SAE认证 */
    WLAN_WITP_AUTH_NUM,
    WLAN_WITP_AUTH_MAX = WLAN_WITP_AUTH_NUM - 1,
    WLAN_WITP_AUTH_AUTOMATIC,

    WLAN_WITP_ALG_AUTH_BUTT
} wlan_auth_alg_mode_enum;
typedef hi_u8 wlan_auth_alg_enum_uint8;

typedef enum {
    /*
    *  注意: wlan_cipher_key_type_enum和hal_key_type_enum 值一致,
    *        如果硬件有改变，则应该在HAL 层封装
    */
    WLAN_KEY_TYPE_TX_GTK              = 0,  /* TX GTK */
    WLAN_KEY_TYPE_PTK                 = 1,  /* PTK */
    WLAN_KEY_TYPE_RX_GTK              = 2,  /* RX GTK */
    WLAN_KEY_TYPE_RX_GTK2             = 3,  /* RX GTK2 51不用 */
    WLAN_KEY_TYPE_BUTT
} wlan_cipher_key_type_enum;
typedef hi_u8 wlan_cipher_key_type_enum_uint8;

typedef enum {
    /*
    *  注意: wlan_key_origin_enum_uint8和hal_key_origin_enum_uint8 值一致,
    *        如果硬件有改变，则应该在HAL 层封装
    */
    WLAN_AUTH_KEY = 0,      /* Indicates that for this key, this STA is the authenticator */
    WLAN_SUPP_KEY = 1,      /* Indicates that for this key, this STA is the supplicant */

    WLAN_KEY_ORIGIN_BUTT,
} wlan_key_origin_enum;
typedef hi_u8 wlan_key_origin_enum_uint8;

typedef enum {
    /* 按照80211-2012 协议 Table 8-99 Cipher suite selectors 定义 */
    WLAN_80211_CIPHER_SUITE_GROUP_CIPHER = 0,
    WLAN_80211_CIPHER_SUITE_WEP_40       = 1,
    WLAN_80211_CIPHER_SUITE_TKIP         = 2,
    WLAN_80211_CIPHER_SUITE_RSV          = 3,
    WLAN_80211_CIPHER_SUITE_NO_ENCRYP    = WLAN_80211_CIPHER_SUITE_RSV, /* 采用协议定义的保留值做不加密类型 */
    WLAN_80211_CIPHER_SUITE_CCMP         = 4,
    WLAN_80211_CIPHER_SUITE_WEP_104      = 5,
    WLAN_80211_CIPHER_SUITE_BIP          = 6,
    WLAN_80211_CIPHER_SUITE_GROUP_DENYD  = 7,

    WLAN_80211_CIPHER_SUITE_WAPI         = 10,      /* 随意定，不影响11i即可 */
} wlan_ciper_protocol_type_enum;
typedef hi_u8 wlan_ciper_protocol_type_enum_uint8;

/* 按照80211-2012 协议 Table 8-101 AKM suite selectors 定义 */
#define WLAN_AUTH_SUITE_RSV              0
#define WLAN_AUTH_SUITE_1X               1
#define WLAN_AUTH_SUITE_PSK              2
#define WLAN_AUTH_SUITE_FT_1X            3
#define WLAN_AUTH_SUITE_FT_PSK           4
#define WLAN_AUTH_SUITE_1X_SHA256        5
#define WLAN_AUTH_SUITE_PSK_SHA256       6
#define WLAN_AUTH_SUITE_TDLS             7
#define WLAN_AUTH_SUITE_SAE_SHA256       8
#define WLAN_AUTH_SUITE_FT_SHA256        9

typedef enum {
    WITP_WPA_VERSION_1 = 1,
    WITP_WPA_VERSION_2 = 2,
#ifdef _PRE_WLAN_FEATURE_WAPI
    WITP_WAPI_VERSION = 1 << 2,
#endif
} witp_wpa_versions_enum;
typedef hi_u8 witp_wpa_versions_enum_uint8;

typedef struct {
    wlan_cipher_key_type_enum_uint8          cipher_key_type;      /* 密钥ID/类型 */
    wlan_ciper_protocol_type_enum_uint8      cipher_protocol_type;
    hi_u8                                    cipher_key_id;
    hi_u8                                    auc_resv;
} wlan_security_txop_params_stru;

/* 协议能力枚举 */
typedef enum {
    WLAN_PROTOCOL_CAP_LEGACY,
    WLAN_PROTOCOL_CAP_HT,
    WLAN_PROTOCOL_CAP_VHT,

    WLAN_PROTOCOL_CAP_BUTT,
} wlan_protocol_cap_enum;
typedef hi_u8 wlan_protocol_cap_enum_uint8;

/* 频带能力枚举 */
typedef enum {
    WLAN_BAND_CAP_2G, /* 只支持2G */
    WLAN_BAND_CAP_BUTT
} wlan_band_cap_enum;
typedef hi_u8 wlan_band_cap_enum_uint8;

/* 带宽能力枚举 */
typedef enum {
    WLAN_BW_CAP_20M,
    WLAN_BW_CAP_40M,
    WLAN_BW_CAP_80M,
    WLAN_BW_CAP_160M,

    WLAN_BW_CAP_BUTT
} wlan_bw_cap_enum;
typedef hi_u8 wlan_bw_cap_enum_uint8;

/* monitor类型枚举 */
typedef enum {
    WLAN_MONITOR_OFF,              /* 关闭报文上报 */
    WLAN_MONITOR_MCAST_DATA,       /* 上报组播(广播)数据包 */
    WLAN_MONITOR_UCAST_DATA,       /* 上报单播数据包 */
    WLAN_MONITOR_MCAST_MANAGEMENT, /* 上报组播(广播)管理包 */
    WLAN_MONITOR_UCAST_MANAGEMENT, /* 上报单播管理包 */
    WLAN_MONITOR_BUTT
} wlan_monitor_enum;
typedef hi_u8 wlan_monitor_enum_uint8;

/* ****************************************************************************
  3.4 VHT枚举类型
**************************************************************************** */
typedef enum {
    WLAN_VHT_MCS0,
    WLAN_VHT_MCS1,
    WLAN_VHT_MCS2,
    WLAN_VHT_MCS3,
    WLAN_VHT_MCS4,
    WLAN_VHT_MCS5,
    WLAN_VHT_MCS6,
    WLAN_VHT_MCS7,
    WLAN_VHT_MCS8,
    WLAN_VHT_MCS9,

    WLAN_VHT_MCS_BUTT,
} wlan_vht_mcs_enum;
typedef hi_u8 wlan_vht_mcs_enum_uint8;

/*
    复用1101定义的顺序
    TBD，和周赟讨论后，修正速率的先后顺序
*/
typedef enum {
    WLAN_LEGACY_11B_RESERVED1       = 0,
    WLAN_SHORT_11B_2M_BPS           = 1,
    WLAN_SHORT_11B_5_HALF_M_BPS     = 2,
    WLAN_SHORT_11B_11_M_BPS         = 3,

    WLAN_LONG_11B_1_M_BPS           = 4,
    WLAN_LONG_11B_2_M_BPS           = 5,
    WLAN_LONG_11B_5_HALF_M_BPS      = 6,
    WLAN_LONG_11B_11_M_BPS          = 7,

    WLAN_LEGACY_OFDM_48M_BPS        = 8,
    WLAN_LEGACY_OFDM_24M_BPS        = 9,
    WLAN_LEGACY_OFDM_12M_BPS        = 10,
    WLAN_LEGACY_OFDM_6M_BPS         = 11,
    WLAN_LEGACY_OFDM_54M_BPS        = 12,
    WLAN_LEGACY_OFDM_36M_BPS        = 13,
    WLAN_LEGACY_OFDM_18M_BPS        = 14,
    WLAN_LEGACY_OFDM_9M_BPS         = 15,

    WLAN_LEGACY_RATE_VALUE_BUTT
} wlan_legacy_rate_value_enum;
typedef hi_u8 wlan_legacy_rate_value_enum_uint8;

/* WIFI协议类型定义 */
typedef enum {
    WLAN_LEGACY_11A_MODE            = 0,    /* 11a, 5G, OFDM */
    WLAN_LEGACY_11B_MODE            = 1,    /* 11b, 2.4G */
    WLAN_LEGACY_11G_MODE            = 2,    /* 旧的11g only已废弃, 2.4G, OFDM */
    WLAN_MIXED_ONE_11G_MODE         = 3,    /* 11bg, 2.4G */
    WLAN_MIXED_TWO_11G_MODE         = 4,    /* 11g only, 2.4G */
    WLAN_HT_MODE                    = 5,    /* 11n(11bgn或者11an，根据频段判断) */
    WLAN_VHT_MODE                   = 6,    /* 11ac */
    WLAN_HT_ONLY_MODE               = 7,    /* 11n only mode,只有带HT的设备才可以接入 */
    WLAN_VHT_ONLY_MODE              = 8,    /* 11ac only mode 只有带VHT的设备才可以接入 */
    WLAN_HT_11G_MODE                = 9,    /* 11ng,不包括11b */

    WLAN_PROTOCOL_BUTT
} wlan_protocol_enum;
typedef hi_u8 wlan_protocol_enum_uint8;

/* 重要:代表VAP的preamble协议能力的使用该枚举，0表示long preamble; 1表示short preamble */
typedef enum {
    WLAN_LEGACY_11B_MIB_LONG_PREAMBLE    = 0,
    WLAN_LEGACY_11B_MIB_SHORT_PREAMBLE   = 1,
} wlan_11b_mib_preamble_enum;
typedef hi_u8 wlan_11b_mib_preamble_enum_uint8;

/* 重要:仅限描述符接口使用(表示发送该帧使用的pramble类型)，0表示short preamble; 1表示long preamble */
typedef enum {
    WLAN_LEGACY_11B_DSCR_SHORT_PREAMBLE  = 0,
    WLAN_LEGACY_11B_DSCR_LONG_PREAMBLE   = 1,

    WLAN_LEGACY_11B_PREAMBLE_BUTT
} wlan_11b_dscr_preamble_enum;
typedef hi_u8 wlan_11b_dscr_preamble_enum_uint8;

/* ****************************************************************************
  3.3 HT枚举类型
**************************************************************************** */
typedef enum {
    WLAN_BAND_WIDTH_20M                 = 0,
    WLAN_BAND_WIDTH_40PLUS              = 1,    /* 从20信道+1 */
    WLAN_BAND_WIDTH_40MINUS             = 2,    /* 从20信道-1 */
    WLAN_BAND_WIDTH_80PLUSPLUS          = 3,    /* 从20信道+1, 从40信道+1 */
    WLAN_BAND_WIDTH_80PLUSMINUS         = 4,    /* 从20信道+1, 从40信道-1 */
    WLAN_BAND_WIDTH_80MINUSPLUS         = 5,    /* 从20信道-1, 从40信道+1 */
    WLAN_BAND_WIDTH_80MINUSMINUS        = 6,    /* 从20信道-1, 从40信道-1 */
    WLAN_BAND_WIDTH_5M                  = 7,
    WLAN_BAND_WIDTH_10M                 = 8,
    WLAN_BAND_WIDTH_BUTT
} wlan_channel_bandwidth_enum;
typedef hi_u8 wlan_channel_bandwidth_enum_uint8;

typedef enum {
    WLAN_CH_SWITCH_DONE = 0,     /* 信道切换已经完成，AP在新信道运行 */
    WLAN_CH_SWITCH_STATUS_1 = 1, /* AP还在当前信道，准备进行信道切换(发送CSA帧/IE) */
    WLAN_CH_SWITCH_STATUS_2 = 2,

    WLAN_CH_SWITCH_BUTT
} wlan_ch_switch_status_enum;
typedef hi_u8 wlan_ch_switch_status_enum_uint8;

typedef enum {
    WLAN_BW_SWITCH_DONE     = 0,    /* 频宽切换已完成 */
    WLAN_BW_SWITCH_40_TO_20 = 1,    /* 从40MHz带宽切换至20MHz带宽 */
    WLAN_BW_SWITCH_20_TO_40 = 2,    /* 从20MHz带宽切换至40MHz带宽 */

    /* 后续添加 */
    WLAN_BW_SWITCH_BUTT
} wlan_bw_switch_status_enum;
typedef hi_u8 wlan_bw_switch_status_enum_uint8;

typedef enum {
    WLAN_BAND_ASSEMBLE_20M                   = 0,

    WLAN_BAND_ASSEMBLE_40M                   = 4,
    WLAN_BAND_ASSEMBLE_40M_DUP               = 5,

    WLAN_BAND_ASSEMBLE_80M                   = 8,
    WLAN_BAND_ASSEMBLE_80M_DUP               = 9,

    WLAN_BAND_ASSEMBLE_160M                  = 12,
    WLAN_BAND_ASSEMBLE_160M_DUP              = 13,

    WLAN_BAND_ASSEMBLE_80M_80M               = 15,

    WLAN_BAND_ASSEMBLE_5M                    = 16,
    WLAN_BAND_ASSEMBLE_10M                   = 17,

    WLAN_BAND_ASSEMBLE_AUTO,

    WLAN_BAND_ASSEMBLE_BUTT
} hal_channel_assemble_enum;
typedef hi_u8 hal_channel_assemble_enum_uint8;

typedef enum {
    WLAN_HT_MIXED_PREAMBLE          = 0,
    WLAN_HT_GF_PREAMBLE             = 1,

    WLAN_HT_PREAMBLE_BUTT
} wlan_ht_preamble_enum;
typedef hi_u8 wlan_ht_preamble_enum_uint8;

typedef enum {
    WLAN_HT_MCS0,
    WLAN_HT_MCS1,
    WLAN_HT_MCS2,
    WLAN_HT_MCS3,
    WLAN_HT_MCS4,
    WLAN_HT_MCS5,
    WLAN_HT_MCS6,
    WLAN_HT_MCS7,
    WLAN_HT_MCS8,
    WLAN_HT_MCS9,
    WLAN_HT_MCS10,
    WLAN_HT_MCS11,
    WLAN_HT_MCS12,
    WLAN_HT_MCS13,
    WLAN_HT_MCS14,
    WLAN_HT_MCS15,

    WLAN_HT_MCS_BUTT
} wlan_ht_mcs_enum;
typedef hi_u8 wlan_ht_mcs_enum_uint8;
#define WLAN_HT_MCS32 32
#define WLAN_MIN_MPDU_LEN_FOR_MCS32 12
#define WLAN_MIN_MPDU_LEN_FOR_MCS32_SHORTGI 13

typedef enum {
    WLAN_SINGLE_NSS                 = 0,
    WLAN_DOUBLE_NSS                 = 1,
    WLAN_TRIPLE_NSS                 = 2,
    WLAN_FOUR_NSS                   = 3,

    WLAN_NSS_BUTT
} wlan_nss_enum;
typedef hi_u8 wlan_nss_enum_uint8;

typedef enum {
    WLAN_HT_NON_STBC                   = 0,
    WLAN_HT_ADD_ONE_NTS                = 1,
    WLAN_HT_ADD_TWO_NTS                = 2,

    WLAN_HT_STBC_BUTT
} wlan_ht_stbc_enum;
typedef hi_u8 wlan_ht_stbc_enum_uint8;

typedef enum {
    WLAN_NO_SOUNDING                = 0,
    WLAN_NDP_SOUNDING               = 1,
    WLAN_STAGGERED_SOUNDING         = 2,
    WLAN_LEGACY_SOUNDING            = 3,

    WLAN_SOUNDING_BUTT
} wlan_sounding_enum;
typedef hi_u8 wlan_sounding_enum_uint8;

typedef struct {
    hi_u8                               group_id;      /* group_id   */
    hi_u8                               txop_ps_not_allowed;
    hi_u16                              us_partial_aid;   /* partial_aid */
} wlan_groupid_partial_aid_stru;

/* ****************************************************************************
  3.4 TXBF枚举类型
**************************************************************************** */
typedef enum {
    WLAN_NON_TXBF                   = 0,
    WLAN_EXPLICIT_TXBF              = 1,
    WLAN_LEGACY_TXBF                = 2,

    WLAN_TXBF_BUTT
} wlan_txbf_enum;
typedef hi_u8 wlan_txbf_enum_uint8;

/* ****************************************************************************
  3.5 WME枚举类型
**************************************************************************** */
/* WMM枚举类型放入平台 */
/* 帧类型 (2-bit) */
typedef enum {
    WLAN_MANAGEMENT            = 0,
    WLAN_CONTROL               = 1,
    WLAN_DATA_BASICTYPE        = 2,
    WLAN_RESERVED              = 3,

    WLAN_FRAME_TYPE_BUTT
} wlan_frame_type_enum;
typedef hi_u8 wlan_frame_type_enum_uint8;

/* 帧子类型 (4-bit) */
/* 管理帧子类型放入平台SPEC */
/* 控制帧帧子类型 */
typedef enum {
    /* 0~6 reserved */
    WLAN_VHT_NDPA               = 5,     /* 0101 */
    WLAN_CONTROL_WRAPPER        = 7,
    WLAN_BLOCKACK_REQ           = 8,
    WLAN_BLOCKACK               = 9,
    WLAN_PS_POLL                = 10,
    WLAN_RTS                    = 11,
    WLAN_CTS                    = 12,
    WLAN_ACK                    = 13,
    WLAN_CF_END                 = 14,
    WLAN_CF_END_CF_ACK          = 15,

    WLAN_CONTROL_SUBTYPE_BUTT   = 16,
} wlan_frame_control_subtype_enum;

/* 数据帧子类型 */
typedef enum {
    WLAN_DATA                   = 0,
    WLAN_DATA_CF_ACK            = 1,
    WLAN_DATA_CF_POLL           = 2,
    WLAN_DATA_CF_ACK_POLL       = 3,
    WLAN_NULL_FRAME             = 4,
    WLAN_CF_ACK                 = 5,
    WLAN_CF_POLL                = 6,
    WLAN_CF_ACK_POLL            = 7,
    WLAN_QOS_DATA               = 8,
    WLAN_QOS_DATA_CF_ACK        = 9,
    WLAN_QOS_DATA_CF_POLL       = 10,
    WLAN_QOS_DATA_CF_ACK_POLL   = 11,
    WLAN_QOS_NULL_FRAME         = 12,
    WLAN_DATA_SUBTYPE_RESV1     = 13,
    WLAN_QOS_CF_POLL            = 14,
    WLAN_QOS_CF_ACK_POLL        = 15,

    WLAN_DATA_SUBTYPE_MGMT      = 16,
} wlan_frame_data_subtype_enum;

/* ACK类型定义 */
typedef enum {
    WLAN_TX_NORMAL_ACK = 0,
    WLAN_TX_NO_ACK,
    WLAN_TX_NO_EXPLICIT_ACK,
    WLAN_TX_BLOCK_ACK,

    WLAN_TX_NUM_ACK_BUTT
} wlan_tx_ack_policy_enum;
typedef hi_u8 wlan_tx_ack_policy_enum_uint8;

/* ****************************************************************************
  3.6 信道枚举
**************************************************************************** */
/* 信道编码方式 */
typedef enum {
    WLAN_BCC_CODE = 0,
    WLAN_LDPC_CODE = 1,
    WLAN_CHANNEL_CODE_BUTT
} wlan_channel_code_enum;
typedef hi_u8 wlan_channel_code_enum_uint8;

/* 扫描类型 */
typedef enum {
    WLAN_SCAN_TYPE_PASSIVE = 0,
    WLAN_SCAN_TYPE_ACTIVE = 1,

    WLAN_SCAN_TYPE_BUTT
} wlan_scan_type_enum;
typedef hi_u8 wlan_scan_type_enum_uint8;

/* 扫描模式 */
typedef enum {
    WLAN_SCAN_MODE_FOREGROUND,      /* 前景扫描不分AP/STA(即初始扫描，连续式) */
    WLAN_SCAN_MODE_BACKGROUND_STA,  /* STA背景扫描 */
    WLAN_SCAN_MODE_BACKGROUND_AP,   /* AP背景扫描(间隔式) */
    WLAN_SCAN_MODE_BACKGROUND_OBSS, /* 20/40MHz共存的obss扫描 */
    WLAN_SCAN_MODE_BACKGROUND_CCA,
    WLAN_SCAN_MODE_BACKGROUND_PNO, /* PNO调度扫描 */

    WLAN_SCAN_MODE_BUTT
} wlan_scan_mode_enum;
typedef hi_u8 wlan_scan_mode_enum_uint8;

/* 扫描结果枚举 */
typedef enum {
    WLAN_SCAN_EVENT_COMPLETE = 0,
    WLAN_SCAN_EVENT_FAILED,
    WLAN_SCAN_EVENT_ABORT, /* 强制终止，比如卸载 */
    WLAN_SCAN_EVENT_TIMEOUT,
    WLAN_SCAN_EVENT_BUTT
} dmac_scan_event_enum;
typedef hi_u8 wlan_scan_event_enum_uint8;

typedef enum {
    WLAN_LEGACY_VAP_MODE     = 0,    /* 非P2P设备 */
    WLAN_P2P_GO_MODE         = 1,    /* P2P_GO */
    WLAN_P2P_DEV_MODE        = 2,    /* P2P_Device */
    WLAN_P2P_CL_MODE         = 3,    /* P2P_CL */

    WLAN_P2P_BUTT
} wlan_p2p_mode_enum;
typedef hi_u8 wlan_p2p_mode_enum_uint8;

/* ****************************************************************************
  3.7 加密枚举
**************************************************************************** */
/* pmf的能力 */
typedef enum {
    MAC_PMF_DISABLED = 0, /* 不支持pmf能力 */
    MAC_PMF_ENABLED,      /* 支持pmf能力，且不强制 */
    MAC_PME_REQUIRED,     /* 严格执行pmf能力 */

    MAC_PMF_BUTT
} wlan_pmf_cap_status;
typedef hi_u8 wlan_pmf_cap_status_uint8;

/* ****************************************************************************
  3.8 报文类型枚举
**************************************************************************** */
typedef enum {
    WLAN_PKT_TYPE_BEACON,
    WLAN_PKT_TYPE_PROBE_REQ,
    WLAN_PKT_TYPE_PROBE_RESP,
    WLAN_PKT_TYPE_ASSOC_REQ,
    WLAN_PKT_TYPE_ASSOC_RESP,
    WLAN_PKT_TYPE_BUTT
} wlan_pkt_type;
typedef hi_u8 wlan_pkt_type_uint8;

typedef enum {
    IEEE80211_ROC_TYPE_NORMAL = 0,
    IEEE80211_ROC_TYPE_MGMT_TX,
    IEEE80211_ROC_TYPE_BUTT,
}wlan_ieee80211_roc_type;
typedef hi_u8 wlan_ieee80211_roc_type_uint8;

/* ****************************************************************************
  4 全局变量声明
**************************************************************************** */
/* ****************************************************************************
  5 消息头定义
**************************************************************************** */
/* ****************************************************************************
  6 消息定义
**************************************************************************** */
/* ****************************************************************************
  7 STRUCT定义
**************************************************************************** */
typedef struct {
    hi_u32                      cipher;                      /* 加密类型 */
    hi_u32                      key_len;                     /* 密钥长 */
    hi_u32                      seq_len;                     /* sequnece长 */
    hi_u8                       auc_key[WLAN_WPA_KEY_LEN];   /* 密钥 */
    hi_u8                       auc_seq[WLAN_WPA_SEQ_LEN];   /* sequence */
} wlan_priv_key_param_stru;

typedef struct {
    hi_s8 rssi;       /* 信号强度 */
    hi_u8 auc_rsv[3]; /* 3: bytes保留字段 */
} wlan_rssi_stru;

typedef struct {
    hi_u8 enable;
    wlan_pkt_type_uint8 type;
    hi_u8 idx;
    hi_u8 rsv;
    const hi_u8 *puc_vnd_ie;
} wlan_vendor_ie_stru;

typedef struct {
    hi_u8 *puc_data;
    hi_u32 us_len;
} wlan_custom_pkt_stru;

#ifdef _PRE_WLAN_FEATURE_MESH_ROM
/* 数据帧发送状态Mesh定义 */
typedef enum {
    WLAN_MESH_TX_STATUS_TX_ACK_OK            = 0,         /* Mesh TX成功   */
    WLAN_MESH_TX_STATUS_MAX_RETRIES          = 1,         /* 达到最高重传次数         */
    WLAN_MESH_TX_STATUS_ERR_FATAL            = 2,         /* 发送失败 */
    WLAN_MESH_TX_STATUS_PEER_UNREACHABLE     = 3,         /* 远端不可达 */
    WLAN_MESH_TX_STATUS_DROP_POLICY          = 4,         /* 策略丢帧（描述符不够） */

    WLAN_MESH_TX_STATUS_BUTT
} wlan_mesh_tx_status_enum;
typedef hi_u8 wlan_mesh_tx_status_enum_uint8;
#endif

#ifdef _PRE_WLAN_FEATURE_BW_HIEX
typedef struct {
    hi_u8                             selfcts;        /* 是否使能窄带切宽带发送selfcts */
    hi_u8                             duration;       /* selfcts的占用信道时间，单位ms */
    hi_u16                            us_per;            /* 发送selfcts的PER阈值 */
} wlan_selfcts_param_stru;
#endif

/* ****************************************************************************
  8 UNION定义
**************************************************************************** */
/* ****************************************************************************
  9 OTHERS定义
**************************************************************************** */
/* ****************************************************************************
 函 数 名  : wlan_hdr_get_frame_type
 功能描述  : 获取帧类型
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年4月10日
    作    者   : HiSilicon
    修改内容   : 新生成函数

**************************************************************************** */
static inline hi_u8 wlan_hdr_get_frame_type(const hi_u8 *puc_header)
{
    return ((puc_header[0] & (0x0c)) >> 2); /* 2: 右移2位 */
}

/* ****************************************************************************
  10 函数声明
**************************************************************************** */
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of wlan_types.h */
