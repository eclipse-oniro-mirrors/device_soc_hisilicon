/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: platform section header.
 *
 * Create: 2024-09-10
 */
#ifndef __PLATFORM_SECTION_H__
#define __PLATFORM_SECTION_H__

#ifndef PLT_SEC_ITCM_T
#define PLT_SEC_ITCM_T __attribute__((section(".plt.itcm.text")))
#endif

#ifndef PLT_SEC_ITCM_R
#define PLT_SEC_ITCM_R __attribute__((section(".plt.itcm.rodata")))
#endif

#ifndef PLT_SEC_DTCM_D
#define PLT_SEC_DTCM_D __attribute__((section(".plt.dtcm.data")))
#endif

#ifndef PLT_SEC_DTCM_B
#define PLT_SEC_DTCM_B __attribute__((section(".plt.dtcm.bss")))
#endif

#endif
