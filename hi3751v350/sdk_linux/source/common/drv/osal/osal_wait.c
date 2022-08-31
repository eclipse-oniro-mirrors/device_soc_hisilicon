/*
 * Copyright (C) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
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

#include "hi_osal.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/slab.h>

#define OSAL_WAIT_CONDITION_TRUE 1

unsigned long osal_msecs_to_jiffies(const unsigned int m)
{
    return msecs_to_jiffies(m);
}
EXPORT_SYMBOL(osal_msecs_to_jiffies);

int osal_wait_init(osal_wait *wait)
{
    wait_queue_head_t *wq = NULL;
    if (wait == NULL) {
        osal_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    wq = (wait_queue_head_t *)kmalloc(sizeof(wait_queue_head_t), GFP_ATOMIC);
    if (wq == NULL) {
        osal_printk("%s - kmalloc error!\n", __FUNCTION__);
        return -1;
    }
    init_waitqueue_head(wq);
    wait->wait = wq;
    return 0;
}
EXPORT_SYMBOL(osal_wait_init);
int osal_wait_interruptible(osal_wait *wait, osal_wait_condition_func func, const void *param)
{
    wait_queue_head_t *wq = NULL;

    if (wait == NULL) {
        osal_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }

    wq = (wait_queue_head_t *)(wait->wait);
    if (wq == NULL) {
        osal_printk("%s - wait->wait is NULL!\n", __FUNCTION__);
        return -1;
    }

    if (func == NULL) {
        return wait_event_interruptible((*wq), OSAL_WAIT_CONDITION_TRUE);
    }

    return wait_event_interruptible((*wq), func(param));
}
EXPORT_SYMBOL(osal_wait_interruptible);

int osal_wait_uninterruptible(osal_wait *wait, osal_wait_condition_func func, const void *param)
{
    wait_queue_head_t *wq = NULL;

    if (wait == NULL) {
        osal_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }

    wq = (wait_queue_head_t *)(wait->wait);
    if (wq == NULL) {
        osal_printk("%s - wait->wait is NULL!\n", __FUNCTION__);
        return -1;
    }

    if (func == NULL) {
        wait_event((*wq), OSAL_WAIT_CONDITION_TRUE);
    } else {
        wait_event((*wq), func(param));
    }

    return 0;
}
EXPORT_SYMBOL(osal_wait_uninterruptible);

int osal_wait_timeout_interruptible(osal_wait *wait, osal_wait_condition_func func, const void *param, unsigned long ms)
{
    wait_queue_head_t *wq = NULL;

    if (wait == NULL) {
        osal_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }

    wq = (wait_queue_head_t *)(wait->wait);
    if (wq == NULL) {
        osal_printk("%s - wait->wait is NULL!\n", __FUNCTION__);
        return -1;
    }

    if (func == NULL) {
        return wait_event_interruptible_timeout((*wq), OSAL_WAIT_CONDITION_TRUE, msecs_to_jiffies(ms));
    }

    return wait_event_interruptible_timeout((*wq), func(param), msecs_to_jiffies(ms));
}

EXPORT_SYMBOL(osal_wait_timeout_interruptible);

int osal_wait_timeout_uninterruptible(osal_wait *wait, osal_wait_condition_func func,
    const void *param, unsigned long ms)
{
    int timeout = 0;
    wait_queue_head_t *wq = NULL;

    if (wait == NULL) {
        osal_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }

    wq = (wait_queue_head_t *)(wait->wait);
    if (wq == NULL) {
        osal_printk("%s - wait->wait is NULL!\n", __FUNCTION__);
        return -1;
    }

    if (func == NULL) {
        timeout = wait_event_timeout((*wq), OSAL_WAIT_CONDITION_TRUE, msecs_to_jiffies(ms));
    } else {
        timeout = wait_event_timeout((*wq), func(param), msecs_to_jiffies(ms));
    }

    return timeout;
}

EXPORT_SYMBOL(osal_wait_timeout_uninterruptible);

void osal_wait_wakeup(osal_wait *wait)
{
    wait_queue_head_t *wq = NULL;

    wq = (wait_queue_head_t *)(wait->wait);
    if (wq == NULL) {
        osal_printk("%s - wait->wait is NULL!\n", __FUNCTION__);
        return;
    }

    wake_up_all(wq);
}
EXPORT_SYMBOL(osal_wait_wakeup);
void osal_wait_destroy(osal_wait *wait)
{
    wait_queue_head_t *wq = NULL;

    wq = (wait_queue_head_t *)(wait->wait);
    if (wq == NULL) {
        osal_printk("%s - wait->wait is NULL!\n", __FUNCTION__);
        return;
    }
    kfree(wq);
    wait->wait = NULL;
}
EXPORT_SYMBOL(osal_wait_destroy);
