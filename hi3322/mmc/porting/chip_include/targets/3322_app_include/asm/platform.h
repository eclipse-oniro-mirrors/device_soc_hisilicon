/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description:  os platform header.
 * Author:
 * Create: 2023-03-09
 */

#ifndef _ASM_PLATFORM_H
#define _ASM_PLATFORM_H

#include "los_typedef.h"
#include "register_config.h"
#include "soc/timer.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define LOS_EMG_LEVEL   0

#define LOS_COMMOM_LEVEL   (LOS_EMG_LEVEL + 1)

#define LOS_ERR_LEVEL   (LOS_COMMOM_LEVEL + 1)

#define LOS_WARN_LEVEL  (LOS_ERR_LEVEL + 1)

#define LOS_INFO_LEVEL  (LOS_WARN_LEVEL + 1)

#define LOS_DEBUG_LEVEL (LOS_INFO_LEVEL + 1)

#define PRINT_LEVEL LOS_ERR_LEVEL

#define OS_SYS_CLOCK    (32000000ULL)

/* timezone 默认向西为正方向，这里调整向东为正方向 */
#define OS_TIMEZONE_GET   (timezone * (-1))

#ifndef CACHE_ALIGNED_SIZE
#define CACHE_ALIGNED_SIZE        32
#endif

#ifdef LOSCFG_PLATFORM_OSAPPINIT
extern VOID app_init(VOID);
#endif

#ifdef BUFSIZ
#undef BUFSIZ
#define BUFSIZ 1024
#endif

extern UINTPTR                        __irq_stack_top__;
extern UINTPTR                        __irq_stack_end__;
#define INTERRUPT_STACK_BOTTOM       ((UINTPTR)&__irq_stack_top__)
#define INTERRUPT_STACK_SIZE         ((UINTPTR)&__irq_stack_end__ - (UINTPTR)&__irq_stack_top__)

extern uint32_t oal_get_sleep_ticks(void);
extern void oal_ticks_restore(uint32_t ticks);
void OsCacheInit(void);
extern VOID *OsGetMainTask(VOID);
extern VOID OsSetMainTask(VOID);
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _ASM_PLATFORM_H */
