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

#ifndef __HI_IR_CODEDEF__
#define __HI_IR_CODEDEF__
#include "hiir.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static hiir_dev_param g_static_dev_param[] = {
    /* NEC with simple repeat code : uPD6121G */
    { 828, 972, 414, 486, 45, 67, 135, 203, 180, 270, 32, 0, HIIR_DEFAULT_FREQ },

    /* NEC with simple repeat code : D6121/BU5777/D1913 */
    { 828, 972, 414, 486, 45, 67, 135, 203, 207, 243, 32, 0, HIIR_DEFAULT_FREQ },

    /* NEC with simple repeat code : LC7461M-C13 */
    { 828, 972, 414, 486, 45, 67, 135, 203, 207, 243, 42, 0, HIIR_DEFAULT_FREQ },

    /* NEC with simple repeat code : AEHA */
    { 270, 405, 135, 203, 34, 51, 101, 152, 270, 405, 48, 0, HIIR_DEFAULT_FREQ },

    /* TC9012 : TC9012F/9243 */
    { 414, 486, 414, 486, 45, 67, 135, 203, 0, 0, 32, 1, HIIR_DEFAULT_FREQ },

    /* NEC with full repeat code : uPD6121G */
    { 828, 972, 414, 486, 45, 67, 135, 203, 0, 0, 32, 2, HIIR_DEFAULT_FREQ },

    /* NEC with full repeat code : LC7461M-C13 */
    { 828, 972, 414, 486, 45, 67, 135, 203, 0, 0, 42, 2, HIIR_DEFAULT_FREQ },

    /* NEC with full repeat code : MN6024-C5D6 */
    { 270, 405, 270, 405, 68, 101, 203, 304, 0, 0, 22, 2, HIIR_DEFAULT_FREQ },

    /* NEC with full repeat code : MN6014-C6D6 */
    { 279, 419, 279, 419, 70, 105, 140, 210, 0, 0, 24, 2, HIIR_DEFAULT_FREQ },

    /* NEC with full repeat code : MATNEW */
    { 279, 419, 300, 449, 35, 52, 105, 157, 0, 0, 48, 2, HIIR_DEFAULT_FREQ },

    /* NEC with full repeat code : MN6030 */
    { 279, 419, 279, 419, 70, 105, 210, 314, 0, 0, 22, 2, HIIR_DEFAULT_FREQ },

    /* NEC with full repeat code : PANASONIC */
    { 282, 422, 282, 422, 70, 106, 211, 317, 0, 0, 22, 2, HIIR_DEFAULT_FREQ },

    /* SONY-D7C5 */
    { 192, 288, 48, 72, 48, 72, 96, 144, 0, 0, 12, 3, HIIR_DEFAULT_FREQ },

    /* SONY-D7C6 */
    { 192, 288, 48, 72, 48, 72, 96, 144, 0, 0, 13, 3, HIIR_DEFAULT_FREQ },

    /* SONY-D7C8 */
    { 192, 288, 48, 72, 48, 72, 96, 144, 0, 0, 15, 3, HIIR_DEFAULT_FREQ },

    /* SONY-D7C13 */
    { 192, 288, 48, 72, 48, 72, 96, 144, 0, 0, 20, 3, HIIR_DEFAULT_FREQ },
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __HI_IR_CODEDEF__ */
