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

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/rtc.h>
#include <linux/sched/clock.h>
#include <linux/version.h>
#include "hi_osal.h"

struct timer_list_info {
    struct timer_list time_list;
    unsigned long data;
};
typedef void (*timer_callback_fun)(struct timer_list *data);

unsigned long osal_timer_get_private_data(void *data)
{
    struct timer_list_info *list_info = osal_container_of(data, struct timer_list_info, time_list);

    return list_info->data;
}
EXPORT_SYMBOL(osal_timer_get_private_data);

int osal_timer_init(osal_timer_t *timer)
{
    struct timer_list_info *t = NULL;

    if (timer == NULL) {
        osal_trace("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }

    t = (struct timer_list_info *)kmalloc(sizeof(struct timer_list_info), GFP_KERNEL);
    if (t == NULL) {
        osal_trace("%s - kmalloc error!\n", __FUNCTION__);
        return -1;
    }

    t->data = timer->data;
    timer_setup(&t->time_list, (timer_callback_fun)timer->function, 0);
    timer->timer = t;
    return 0;
}
EXPORT_SYMBOL(osal_timer_init);
int osal_set_timer(osal_timer_t *timer, unsigned long interval)
{
    struct timer_list_info *list_info = NULL;
    if ((timer == NULL) || (timer->timer == NULL) || (timer->function == NULL) || (interval == 0)) {
        osal_trace("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    list_info = (struct timer_list_info *)timer->timer;
    list_info->data = timer->data;
    list_info->time_list.function = (timer_callback_fun)timer->function;
    return mod_timer(&list_info->time_list, jiffies + msecs_to_jiffies(interval) - 1);
}
EXPORT_SYMBOL(osal_set_timer);

int osal_del_timer(osal_timer_t *timer)
{
    struct timer_list *t = NULL;
    if ((timer == NULL) || (timer->timer == NULL) || (timer->function == NULL)) {
        osal_trace("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    t = timer->timer;
    return del_timer(t);
}
EXPORT_SYMBOL(osal_del_timer);

int osal_timer_destroy(osal_timer_t *timer)
{
    struct timer_list *t = timer->timer;
    del_timer(t);
    kfree(t);
    timer->timer = NULL;
    return 0;
}
EXPORT_SYMBOL(osal_timer_destroy);

unsigned long osal_msleep(unsigned int msecs)
{
    return msleep_interruptible(msecs);
}
EXPORT_SYMBOL(osal_msleep);

void osal_udelay(unsigned int usecs)
{
    udelay(usecs);
}
EXPORT_SYMBOL(osal_udelay);

void osal_mdelay(unsigned int msecs)
{
    mdelay(msecs);
}
EXPORT_SYMBOL(osal_mdelay);

unsigned int osal_get_tickcount()
{
    return jiffies_to_msecs(jiffies);
}
EXPORT_SYMBOL(osal_get_tickcount);

unsigned long long osal_sched_clock()
{
    return sched_clock();
}
EXPORT_SYMBOL(osal_sched_clock);

void osal_gettimeofday(osal_timeval_t *tv)
{
#if LINUX_VERSION_CODE > KERNEL_VERSION(5,10,0)
    struct timespec64 t;
    if (tv == NULL) {
        osal_trace("%s - parameter invalid!\n", __FUNCTION__);
        return;
    }
    ktime_get_real_ts64(&t);

    tv->tv_sec = t.tv_sec;
    tv->tv_usec = t.tv_nsec;
#else
    struct timeval t;
    if (tv == NULL) {
        osal_trace("%s - parameter invalid!\n", __FUNCTION__);
        return;
    }
    do_gettimeofday(&t);

    tv->tv_sec = t.tv_sec;
    tv->tv_usec = t.tv_usec;
#endif
}
EXPORT_SYMBOL(osal_gettimeofday);

void osal_rtc_time_to_tm(unsigned long time, osal_rtc_time_t *tm)
{
    struct rtc_time _tm = {0};

#if LINUX_VERSION_CODE > KERNEL_VERSION(5,10,0)
    rtc_time64_to_tm(time, &_tm);
#else
    rtc_time_to_tm(time, &_tm);
#endif
    tm->tm_sec = _tm.tm_sec;
    tm->tm_min = _tm.tm_min;
    tm->tm_hour = _tm.tm_hour;
    tm->tm_mday = _tm.tm_mday;
    tm->tm_mon = _tm.tm_mon;
    tm->tm_year = _tm.tm_year;
    tm->tm_wday = _tm.tm_wday;
    tm->tm_yday = _tm.tm_yday;
    tm->tm_isdst = _tm.tm_isdst;
}
EXPORT_SYMBOL(osal_rtc_time_to_tm);

void osal_rtc_tm_to_time(osal_rtc_time_t *tm, unsigned long *time)
{
    struct rtc_time _tm;
    _tm.tm_sec = tm->tm_sec;
    _tm.tm_min = tm->tm_min;
    _tm.tm_hour = tm->tm_hour;
    _tm.tm_mday = tm->tm_mday;
    _tm.tm_mon = tm->tm_mon;
    _tm.tm_year = tm->tm_year;
    _tm.tm_wday = tm->tm_wday;
    _tm.tm_yday = tm->tm_yday;
    _tm.tm_isdst = tm->tm_isdst;

#if LINUX_VERSION_CODE > KERNEL_VERSION(5,10,0)
    *time = rtc_tm_to_time64(&_tm);
#else
    rtc_tm_to_time(&_tm, time);
#endif
}
EXPORT_SYMBOL(osal_rtc_tm_to_time);

void osal_getjiffies(unsigned long long *pjiffies)
{
    *pjiffies = jiffies;
}
EXPORT_SYMBOL(osal_getjiffies);

int osal_rtc_valid_tm(struct osal_rtc_time *tm)
{
    struct rtc_time _tm;
    _tm.tm_sec = tm->tm_sec;
    _tm.tm_min = tm->tm_min;
    _tm.tm_hour = tm->tm_hour;
    _tm.tm_mday = tm->tm_mday;
    _tm.tm_mon = tm->tm_mon;
    _tm.tm_year = tm->tm_year;
    _tm.tm_wday = tm->tm_wday;
    _tm.tm_yday = tm->tm_yday;
    _tm.tm_isdst = tm->tm_isdst;

    return rtc_valid_tm(&_tm);
}
EXPORT_SYMBOL(osal_rtc_valid_tm);
