/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description:  os clic header.
 * Author:
 * Create: 2023-03-09
 */

#ifndef _HISOC_CLIC_H
#define _HISOC_CLIC_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */
#define REG_CLIC_INFO_CLICINTCTLBITS 3
#define CLIC_CFG                0
#define CLIC_INFO               4
#define CLIC_INTIP(x)           (0x1000 + 4 * (x))
#define CLIC_INTIE(x)           (0x1001 + 4 * (x))
#define CLIC_INTATTR(x)         (0x1002 + 4 * (x))
#define CLIC_INTCTL(x)          (0x1003 + 4 * (x))

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
