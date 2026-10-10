/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: force-included shim header for code ported into morpheus.
 */

#ifndef MMC_COMPAT_FIX_H
#define MMC_COMPAT_FIX_H

#include "los_interrupt.h"

/* los_errno.h: driver module ID (morpheus los_errno.h lacks the entry) */
#ifndef LOS_MOD_DRIVER
#define LOS_MOD_DRIVER 0x41
#endif

/* td_base.h / asm/platform.h (3322): cache line aligned size */
#ifndef CACHE_ALIGNED_SIZE
#define CACHE_ALIGNED_SIZE 32
#endif

/* hwsec_c securec.h */
#ifndef EOK
#define EOK 0
#endif

#ifndef OS_SCHEDULER_ACTIVE
#define OS_SCHEDULER_ACTIVE 1
#endif
#ifndef OS_EXC_ACTIVE
#define OS_EXC_ACTIVE 0
#endif

#include <stdio.h>
#include <stdarg.h>

/* declared in liteos_m kernel/base/los_printf.c (no public header) */
VOID LkDprintf(const CHAR *fmt, va_list ap);

/*
 * musl porting stdio.h declares POSIX "int dprintf(int, ...)" while
 * liteos_m provides "VOID dprintf(const CHAR *fmt, ...)" in
 * kernel/base/los_printf.c, so route dprintf call sites through a shim
 * with the kernel-side signature instead of the musl stdio layer.
 */
static inline void dprintf_shim(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    LkDprintf(fmt, ap);
    va_end(ap);
}
#define dprintf(...) dprintf_shim(__VA_ARGS__)

#ifndef LOS_TASK_PARAM_INIT_ARG
#define LOS_TASK_PARAM_INIT_ARG(initParam, arg) ((initParam).uwArg = (UINT32)(UINTPTR)(arg))
#endif

/* kernel_lite/include/los_task.h: detached task attribute */
#ifndef LOS_TASK_STATUS_DETACHED
#define LOS_TASK_STATUS_DETACHED (1 << 8)
#endif

#include <fcntl.h>
#include <unistd.h>
#undef fallocate64
#undef truncate64

/* musl poll.h: poll table type used by the fs.h poll() file op */
#ifndef MMC_POLL_TABLE_SHIM
#define MMC_POLL_TABLE_SHIM
typedef unsigned int pollevent_t;
struct tag_poll_wait_entry;
typedef struct tag_poll_wait_entry *poll_wait_head;
typedef struct tag_poll_table {
    poll_wait_head wait;
    pollevent_t key;
} poll_table;
#endif

#endif /* MMC_COMPAT_FIX_H */
