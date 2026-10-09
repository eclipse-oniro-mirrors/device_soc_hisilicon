/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description:  3322 product config
 * Author: @CompanyNameTag
 * Create:  2020-10-23
 */
#ifndef PRODUCT_H
#define PRODUCT_H

#ifndef YES
#define YES (1)
#endif

#ifndef NO
#define NO (0)
#endif

#ifdef CONFIG_PRODUCT_EVB_DITING
#define APPLICATION_VERSION_STRING "HiDiTing V100R001C10SPC052T"
#else
#define APPLICATION_VERSION_STRING "Hi3322 V100R001C10SPC100"
#endif
#define TEE_VERSION_STRING "S200"
#include "product_evb_standard.h"

#define DSP_EXIST                           YES
#ifdef CONFIG_PRODUCT_EVB_DITING
#define ALIPAY_SEC_I2C_INDEX                I2C_BUS_3
#define ALIPAY_SEC_GPIO                     S_AGPIO13
#else
#define ALIPAY_SEC_I2C_INDEX                I2C_BUS_3
#define ALIPAY_SEC_GPIO                     S_AGPIO13
#endif
#endif
