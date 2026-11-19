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
#ifndef CRYPTO_DRV_COMMON_H
#define CRYPTO_DRV_COMMON_H

#include "ot_type.h"
#include <securec.h>
#include "crypto_error.h"
#include "crypto_common_macro.h"
#include "crypto_log_utils.h"
#include "crypto_pke_struct.h"
#include "cipher_cfg.h"

#define CRYPTO_BYTE_MAX             0xFF

#define CRYPTO_BITS_IN_BYTE         8
#define CRYPTO_WORD_WIDTH           4

#ifndef EOK
#define EOK 0
#endif

void *crypto_malloc_coherent(td_u32 size);

void crypto_free_coherent(td_void *ptr);

#endif
