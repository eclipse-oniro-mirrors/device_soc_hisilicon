/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description:  os dma header.
 * Author:
 * Create: 2023-03-09
 */

#ifndef _SW39_DMA_H
#define _SW39_DMA_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

extern void dma_cache_clean(uintptr_t start, uintptr_t end);
extern void dma_cache_inv(uintptr_t start, uintptr_t end);


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _SW39_DMA_H */

