/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description:  pmp config
 */

#ifndef PMP_CONFIG_H
#define PMP_CONFIG_H

#include "stdint.h"

/**
 * @addtogroup connectivity_config_otp OTP
 * @{
 */
/**
 * @brief chip pmp config init.
 */
void pmp_init(void);

/**
 * @brief 配置norflash的可读访问
 *
 * @param  [in]  readable 指定是否可读，0表示不可读，非0表示可读
 */
void pmp_norflash_config(uint8_t readable);

/**
 * @brief 配置mcu访问bt ram的权限
 *
 * @param  [in]  rwable 0表示不可读写，非0表示可读可写
 */
void pmp_mcu_to_btram_config(uint8_t rwable);

/**
 * @}
 */
#endif
