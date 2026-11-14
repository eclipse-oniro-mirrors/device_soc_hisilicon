/*
 * Copyright (c) 2025 HiSilicon (Shanghai) Technologies Co., Ltd.
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
 * along with this program; if not, see
 * <https://www.gnu.org/licenses/>.
 */
#include "crypto_drv_common.h"
#include "drv_osal_init.h"

void *crypto_malloc_coherent(td_u32 size)
{
    void *ptr = NULL;
    ptr = crypto_malloc(size);
    if (ptr != NULL) {
        return ptr;
    }
    return NULL;
}

void crypto_free_coherent(td_void *ptr)
{
    crypto_free(ptr);
    return;
}

uint32_t crypto_u8_to_u32(const uint8_t data[4])
{
    uint32_t word = 0;
    word |= data[0];
    word |= (data[1] << 8);     // 1, 8: bit[8:15]
    word |= (data[2] << 16);    // 2, 16: bit[16:23]
    word |= (data[3] << 24);    // 3, 24: bit[24:31]
    return word;
}
