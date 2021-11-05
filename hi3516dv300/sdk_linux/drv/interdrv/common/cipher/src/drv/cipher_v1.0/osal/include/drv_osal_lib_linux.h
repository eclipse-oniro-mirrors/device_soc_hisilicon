/*
 * Copyright (C) 2021 HiSilicon (Shanghai) Technologies CO., LIMITED.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __DRV_OSAL_LIB_LINUX_H__
#define __DRV_OSAL_LIB_LINUX_H__
#include <linux/proc_fs.h>
#include <linux/module.h>
#include <linux/signal.h>
#include <linux/spinlock.h>
#include <linux/personality.h>
#include <linux/ptrace.h>
#include <linux/kallsyms.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/seq_file.h>
#include <linux/version.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <asm/atomic.h>
#include <asm/cacheflush.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <asm/traps.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <asm/page.h>
#include "hi_types.h"
#include "hi_debug.h"
#include "drv_osal_chip.h"
#include "drv_cipher_kapi.h"

#include "linux/uaccess.h"

#if LINUX_VERSION_CODE > KERNEL_VERSION(5,6,0)
#define crypto_ioremap_nocache(addr, size)  ioremap(addr, size)
#else
#define crypto_ioremap_nocache(addr, size)  ioremap_nocache(addr, size)
#endif
#define crypto_iounmap(addr, size)          iounmap(addr)

#define crypto_read(addr)       readl(addr)
#define crypto_write(addr, val) writel(val, addr)

#define crypto_msleep(msec)     msleep(msec)
#define crypto_udelay(msec)     udelay(msec)

#define MAX_MALLOC_BUF_SIZE     0x10000
hi_void *crypto_calloc(size_t n, size_t size);
#define crypto_malloc(x)        ((x) > 0 ? kzalloc((x), GFP_KERNEL) : HI_NULL)
#define crypto_free(x)        \
    do {                      \
        if ((x) != HI_NULL) { \
            kfree((x));       \
            x = HI_NULL;      \
        }                     \
    } while (0)

/* 512M */
#define MAX_COPY_FROM_USER_SIZE    0x20000000

hi_s32 crypto_copy_from_user(hi_void *to, unsigned long to_len,
    const hi_void *from, unsigned long from_len);

hi_s32 crypto_copy_to_user(hi_void *to, unsigned long to_len,
    const hi_void *from, unsigned long from_len);

#define CRYPTO_QUEUE_HEAD       wait_queue_head_t
#define crypto_queue_init(x)    init_waitqueue_head(x)
#define crypto_queue_wait_up(x) wake_up_interruptible(x)

/*
* Returns of crypto_queue_wait_timeout:
* 0 if the @condition evaluated to %false after the @timeout elapsed,
* 1 if the @condition evaluated to %true after the @timeout elapsed,
* the remaining jiffies (at least 1) if the @condition evaluated
* to %true before the @timeout elapsed, or -%ERESTARTSYS if it was
* interrupted by a signal.
*/
#define crypto_queue_wait_timeout(head, con, time) \
    wait_event_interruptible_timeout(head, *(con), time)

#define crypto_request_irq(irq, func, name) \
    request_irq(irq, func, IRQF_SHARED, name, (hi_void *)(name))

#define crypto_free_irq(irq, name)          \
    free_irq(irq, (hi_void *)(name))

#define CRYPTO_MUTEX                         struct semaphore
#define crypto_mutex_init(x)                 sema_init(x, 1)
#define crypto_mutex_lock(x)                 down_interruptible(x)
#define crypto_mutex_unlock(x)               up(x)
#define crypto_mutex_destroy(x)

#define CRYPTO_OWNER                         pid_t
#define crypto_get_owner(x)                  *(x) = task_tgid_nr(current)

#define CRYPTO_IRQRETURN_T                   irqreturn_t

#define hi_log_fatal(fmt...) \
    do { \
        printk("[FATAL-HI_CIPHER]:%s[%d]:", __FUNCTION__, __LINE__); \
        printk(fmt); \
    } while (0)
#define hi_log_error(fmt...) \
    do { \
        printk("[ERROR-HI_CIPHER]:%s[%d]:", __FUNCTION__, __LINE__); \
        printk(fmt); \
    } while (0)

#define hi_log_warn(fmt...)
#define hi_log_info(fmt...)
#define hi_log_debug(fmt...)

#define CRYPTO_PROC_PRINT               seq_printf
#endif  /* End of #ifndef __DRV_OSAL_LIB_LINUX_H__ */
