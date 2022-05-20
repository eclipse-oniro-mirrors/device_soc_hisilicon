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

#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/version.h>
#include "hi_osal.h"

OSAL_LIST_HEAD(wq_list);
struct wq_node {
    osal_workqueue *osal_work;
    struct work_struct *work;
    struct osal_list_head node;
};

static osal_workqueue *osal_find_work(struct work_struct *work)
{
    struct osal_list_head *this = NULL;

    if (work == NULL) {
        osal_printk("%s - parameter invalid!\n", __FUNCTION__);
        return NULL;
    }

    if (osal_list_empty(&wq_list)) {
        osal_printk("find work failed! wq_list is empty!\n");
        return NULL;
    }
    osal_list_for_each(this, &wq_list) {
        struct wq_node *ws = osal_list_entry(this, struct wq_node, node);
        if (ws->work == work) {
            return ws->osal_work;
        }
    }
    osal_printk("find work failed!\n");
    return NULL;
}

static int osal_del_work(struct work_struct *work)
{
    struct osal_list_head *this = NULL;
    struct osal_list_head *next = NULL;

    if (work == NULL) {
        osal_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }

    if (osal_list_empty(&wq_list)) {
        osal_printk("find work failed! wq_list is empty!\n");
        return -1;
    }
    osal_list_for_each_safe(this, next, &wq_list) {
        struct wq_node *ws = osal_list_entry(this, struct wq_node, node);
        if (ws->work == work) {
            osal_list_del(this);
            kfree(ws);
            return 0;
        }
    }
    osal_printk("del work failed!\n");
    return -1;
}

static void osal_work_handler(struct work_struct *work)
{
    osal_workqueue *ow = osal_find_work(work);
    if ((ow != NULL) && (ow->handler != NULL)) {
        ow->handler(ow);
    }
}

int osal_workqueue_init(osal_workqueue *work, osal_workqueue_handler handler)
{
    struct work_struct *w = NULL;
    struct wq_node *w_node = NULL;

    if (work == NULL) {
        osal_printk("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }

    w = kmalloc(sizeof(struct work_struct), GFP_ATOMIC);
    if (w == NULL) {
        osal_printk("osal_init_work kmalloc failed!\n");
        return -1;
    }

    w_node = kmalloc(sizeof(struct wq_node), GFP_ATOMIC);
    if (w_node == NULL) {
        osal_printk("osal_init_work kmalloc failed!\n");
        kfree(w);
        return -1;
    }
    INIT_WORK(w, osal_work_handler);
    work->work = w;
    work->handler = handler;
    w_node->osal_work = work;
    w_node->work = w;
    osal_list_add(&(w_node->node), &wq_list);
    return 0;
}
EXPORT_SYMBOL(osal_workqueue_init);

int osal_workqueue_schedule(osal_workqueue *work)
{
    if ((work != NULL) && (work->work != NULL)) {
        return (int)schedule_work(work->work);
    } else {
        return (int)false;
    }
}
EXPORT_SYMBOL(osal_workqueue_schedule);

void osal_workqueue_destroy(osal_workqueue *work)
{
    if ((work != NULL) && (work->work != NULL)) {
        osal_del_work(work->work);
        kfree((struct work_struct *)work->work);
        work->work = NULL;
    }
}
EXPORT_SYMBOL(osal_workqueue_destroy);

int osal_workqueue_flush(osal_workqueue *work)
{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0))
    if ((work != NULL) && (work->work != NULL)) {
        return (int)flush_work(work->work);
    } else {
        return -1;
    }
#endif
    return 0;
}
EXPORT_SYMBOL(osal_workqueue_flush);
