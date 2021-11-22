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
#include <linux/random.h>
#include <linux/version.h>
#include "hi_osal.h"

/* the result of u64/u32. */
unsigned long long osal_div_u64(unsigned long long dividend, unsigned int divisor)
{
    return div_u64(dividend, divisor);
}
EXPORT_SYMBOL(osal_div_u64);

/* the result of s64/s32. */
long long osal_div_s64(long long dividend, int divisor)
{
    return div_s64(dividend, divisor);
}
EXPORT_SYMBOL(osal_div_s64);

/* the result of u64/u64. */
unsigned long long osal_div64_u64(unsigned long long dividend, unsigned long long divisor)
{
    return div64_u64(dividend, divisor);
}
EXPORT_SYMBOL(osal_div64_u64);

/* the result of s64/s64. */
long long osal_div64_s64(long long dividend, long long divisor)
{
    return div64_s64(dividend, divisor);
}
EXPORT_SYMBOL(osal_div64_s64);

/* the remainder of u64/u32. */
unsigned long long osal_div_u64_rem(unsigned long long dividend, unsigned int divisor)
{
    unsigned int remainder;

    div_u64_rem(dividend, divisor, &remainder);

    return remainder;
}
EXPORT_SYMBOL(osal_div_u64_rem);

/* the remainder of s64/s32. */
long long osal_div_s64_rem(long long dividend, int divisor)
{
    int remainder;

    div_s64_rem(dividend, divisor, &remainder);

    return remainder;
}
EXPORT_SYMBOL(osal_div_s64_rem);

/* the remainder of u64/u64. */
unsigned long long osal_div64_u64_rem(unsigned long long dividend, unsigned long long divisor)
{
    unsigned long long remainder;

    div64_u64_rem(dividend, divisor, &remainder);

    return remainder;
}
EXPORT_SYMBOL(osal_div64_u64_rem);

unsigned int osal_random(void)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 18, 0)
    return random32();
#else
    return get_random_int();
#endif
}
EXPORT_SYMBOL(osal_random);
