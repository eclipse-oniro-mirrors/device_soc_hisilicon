/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description:  OTP map
 */
#ifndef OTP_MAP_H
#define OTP_MAP_H

#include "chip_definitions.h"
#include "chip_io.h"

/**
 * @addtogroup connectivity_config_otp OTP
 * @{
 */
/*
 * OTP address map provides start addresses and lengths
 */
// EFUSE0
#define OTP_ATE_START                           0
#define OTP_ATE_DIEID_START                     44
#define OTP_CHIP_ID_START                       63
#define OTP_ATE_DIEID_LENGTH                    20

#define OTP_APP_USE_START                       100     // 448 bit for app use
#define OTP_APP_USE_LENGTH                      56

#define OTP_OEM_SEC_BOOT_ROOTKEY_HASH_START     216     // 256 bit for sec boot rootkey hash
#define OTP_OEM_SEC_BOOT_ROOTKEY_HASH_LENGTH    32

#define OTP_OEM_RSV_HASH_START                  248     // 256 bit for app use
#define OTP_OEM_RSV_HASH_LENGTH                 32

// 可用于密钥派生  for oem
#define OTP_OEM_MRK_GLOBAL_ROOTKEY_START        156
#define OTP_OEM_MRK_GLOBAL_ROOTKEY_LENGTH       16

// 可用于密钥派生 for hisi
#define OTP_HISI_MRK_GLOBAL_ROOTKEY_START       172
#define OTP_HISI_MRK_GLOBAL_ROOTKEY_LENGTH      16

// 可用于密钥派生 for oem  一机一密
#define OTP_HUK_ROOTKEY_START                   188
#define OTP_HUK_ROOTKEY_LENGTH                  16

// rollback version
#define OTP_SSB_ROLLBACK_VERSION        206
#define OTP_SSB_VERSION_LENGTH          1
#define OTP_SSB_VERSION_LENGTH_BIT      8
#define OTP_SELITOS_ROLLBACK_VERSION    280
#define OTP_SELITOS_VERSION_LENGTH      2
#define OTP_SELITOS_VERSION_LENGTH_BIT  16
#define OTP_NOT_SUPPORT_ANTI_ROLLBACK   0


/**
 * @}
 */
#endif
