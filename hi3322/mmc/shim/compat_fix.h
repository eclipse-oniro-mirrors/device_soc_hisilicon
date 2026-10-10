/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: force-included shim header for fbb code ported into morpheus.
 *
 * morpheus los_hwi.h is an empty shell; the HWI types and LOS_HwiCreate API
 * live in its los_interrupt.h. Force-include it so fbb drivers/base code
 * (compat/driver.c, compat/interrupt.c) sees the same declarations it had
 * in fbb. Also provides the few kernel macros/constants that the fbb
 * toolchain headers provided but morpheus does not.
 */

#ifndef MMC_COMPAT_FIX_H
#define MMC_COMPAT_FIX_H

#include "los_interrupt.h"

/* fbb los_errno.h: driver module ID (morpheus los_errno.h lacks the entry) */
#ifndef LOS_MOD_DRIVER
#define LOS_MOD_DRIVER 0x41
#endif

/* fbb td_base.h / asm/platform.h (3322): cache line aligned size */
#ifndef CACHE_ALIGNED_SIZE
#define CACHE_ALIGNED_SIZE 32
#endif

/* fbb hwsec_c securec.h */
#ifndef EOK
#define EOK 0
#endif

/*
 * fbb los_exc.h / los_sched_pri.h. In fbb these gate the "eMMC access from
 * boot/exception context" fast path. morpheus drives eMMC only after the
 * scheduler is up and has no exception-mode eMMC use, so: scheduler active,
 * exception never active.
 */
#ifndef OS_SCHEDULER_ACTIVE
#define OS_SCHEDULER_ACTIVE 1
#endif
#ifndef OS_EXC_ACTIVE
#define OS_EXC_ACTIVE 0
#endif


/*
 * fbb LiteOS provides a 1-arg printf-style dprintf (varargs starting at fmt).
 * morpheus's kernel musl porting declares the POSIX dprintf(int, fmt, ...)
 * unconditionally, which breaks every fbb dprintf("...") call site. Pull the
 * toolchain stdio.h in first (locking that declaration), then shadow the
 * symbol with a shim so all fbb declarations and call sites redirect here.
 */
#include <stdio.h>
#include <stdarg.h>

/* declared in liteos_m kernel/base/los_printf.c (no public header) */
VOID LkDprintf(const CHAR *fmt, va_list ap);

#ifndef LOSCFG_LIB_LIBC
/*
 * liteos_m provides LkDprintf(fmt, va_list) (kernel/base/los_printf.c) but
 * no vprintf, so forward through it instead of the musl stdio layer.
 */
static inline int dprintf_shim(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    LkDprintf(fmt, ap);
    va_end(ap);
    return 0;
}
#define dprintf(...) dprintf_shim(__VA_ARGS__)
#endif

/*
 * fbb los_task.h:1358 — morpheus TSK_INIT_PARAM_S (without
 * LOSCFG_KERNEL_TASK_ENTRY_VOID_PTR) carries the argument in the uwArg
 * member; riscv32 pointers fit a UINT32.
 */
#ifndef LOS_TASK_PARAM_INIT_ARG
#define LOS_TASK_PARAM_INIT_ARG(initParam, arg) ((initParam).uwArg = (UINT32)(UINTPTR)(arg))
#endif

/* fbb kernel_lite/include/los_task.h: detached task attribute */
#ifndef LOS_TASK_STATUS_DETACHED
#define LOS_TASK_STATUS_DETACHED (1 << 8)
#endif

/*
 * nuttx fs/fs.h (fbb version) has a fallocate64 file-op member. The musl
 * porting fcntl.h unconditionally "#define fallocate64 fallocate", which
 * collides with the fallocate member right above it when fcntl.h is pulled
 * in before fs.h. Include fcntl.h here to lock its guard, then restore the
 * real fallocate64 symbol for the rest of the translation unit.
 */
#include <fcntl.h>
#include <unistd.h>
#undef fallocate64
#undef truncate64

/* fbb musl poll.h: poll table type used by the fs.h poll() file op */
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
