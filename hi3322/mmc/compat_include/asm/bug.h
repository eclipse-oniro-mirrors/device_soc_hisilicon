/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: LiteOS compact for linux
 * Create: 2026-03-25
 */

#ifndef _ASM_BUG_H
#define _ASM_BUG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef BUG
#define BUG() do { \
        diag_printf("BUG() at %s %d\n", __FUNCTION__, __LINE__); \
} while (0)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASM_BUG_H */
