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

#ifndef TDE_HAL_K_BODY_INC
#define TDE_HAL_K_BODY_INC

#line 1 "tde_hal_k.c"
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

#include "tde_hal_k.h"

#ifdef CONFIG_USE_SYS_CONFIG
#include "sys_ext.h"
#endif
#include "securec.h"
#include "ot_common.h"
#include "ot_math.h"

#include "tde_hal.h"
#include "wmalloc.h"
#include "tde_adp.h"
#include "tde_osictl_k.h"

#define UPDATE_SIZE 64

#define NO_HSCALE_STEP 0x100000
#define NO_VSCALE_STEP 0x1000

#define drv_tde_node_blend_mode(node, a, b)                 \
        do {                                                \
            (node)->src1_cbmpara.bits.s1_blendmode = (a);   \
            (node)->src2_cbmpara.bits.s2_blendmode = (b);   \
        } while (0)

/* R/W register's encapsulation */
#define tde_read_reg(base, offset) (*((volatile td_u32 *)(((td_void *)(base)) + (offset))))
#define tde_write_reg(base, offset, val) (*((volatile td_u32 *)(((td_void *)(base)) + (offset))) = (val))
/* TDE register's Address range */
#define TDE_REG_SIZE 0x4100

/* Step range's type by algorithm team */
#define TDE_RESIZE_PARA_AREA_0 4096  /* 1.0 */
#define TDE_RESIZE_PARA_AREA_1 5461  /* 0.75 */
#define TDE_RESIZE_PARA_AREA_2 8192  /* 0.5 */
#define TDE_RESIZE_PARA_AREA_3 12412 /* 0.33 */
#define TDE_RESIZE_PARA_AREA_4 16384 /* 0.25 */

/* coefficient table rangle of 8*32 area */
#define TDE_RESIZE_8X32_AREA_0 1048576 /* 1.0 */
#define TDE_RESIZE_8X32_AREA_1 1398101 /* 0.75 */
#define TDE_RESIZE_8X32_AREA_2 2097152 /* 0.5 */
#define TDE_RESIZE_8X32_AREA_3 3177503 /* 0.33 */
#define TDE_RESIZE_8X32_AREA_4 4194304 /* 0.25 */

/* Colorkey mode of CMOS chip */
#define TDE_COLORKEY_IGNORE   2
#define TDE_COLORKEY_AREA_OUT 1
#define TDE_COLORKEY_AREA_IN  0

/* Aq control mode */
#define TDE_AQ_CTRL_COMP_LIST 0x0 /* start next AQ list, after complete current list's operations */
#define TDE_AQ_CTRL_COMP_LINE 0x4 /* start next AQ list, after complete current node and line */

#define TDE_MAX_READ_STATUS_TIME 10

#define TDE_TWENTYFOUR_BITS_SHIFT 24
#define TDE_EIGHT_BITS_SHIFT      8
#define TDE_SIXTEEN_BITS_SHIFT    16

#define TDE_PARA_TABLE_SIZE 32
#define TDE_PARA_TABLE_ORG_SIZE 40
#define TDE_PARA_TABLE_NUM 12

#define TDE_PARA_VTABLE_ORG_SIZE (17 * 2)

#define TDE_PARA_HTABLE_ORG_SIZE (17 * 3)
#define TDE_PARA_HTABLE_SIZE     (64 * 4)
#define TDE_PARA_HTABLE_NUM      7

#define TDE_PARA_VTABLE_SIZE (64 * 4)
#define TDE_PARA_VTABLE_NUM 7

/* vertical filter coefficient */
td_u32 g_org_vf_coef[TDE_PARA_VTABLE_ORG_SIZE * TDE_PARA_HTABLE_NUM] = {
    /* ratio is greater than 1.0 */
    0x3f0000,
    0x0,
    0x3f0000,
    0x1,
    0x3f03fe,
    0x3,
    0x3f03fd,
    0x3ff0005,
    0x3f03fc,
    0x3ff0006,
    0x3d03fc,
    0x3ff0008,
    0x3c03fb,
    0x3ff000a,
    0x3b03fb,
    0x3fe000c,
    0x3803fb,
    0x3fe000f,
    0x3603fb,
    0x3fe0011,
    0x3403fa,
    0x3fe0014,
    0x3203fb,
    0x3fd0016,
    0x2f03fb,
    0x3fd0019,
    0x2c03fb,
    0x3fd001c,
    0x2a03fb,
    0x3fc001f,
    0x2703fb,
    0x3fc0022,
    0x2403fc,
    0x3fc0024,

    /* ratio is equal to 1.0 */
    0x3f0000,
    0x0,
    0x3f0000,
    0x1,
    0x3f03fe,
    0x3,
    0x3f03fd,
    0x3ff0005,
    0x3f03fc,
    0x3ff0006,
    0x3d03fc,
    0x3ff0008,
    0x3c03fb,
    0x3ff000a,
    0x3b03fb,
    0x3fe000c,
    0x3803fb,
    0x3fe000f,
    0x3603fb,
    0x3fe0011,
    0x3403fa,
    0x3fe0014,
    0x3203fb,
    0x3fd0016,
    0x2f03fb,
    0x3fd0019,
    0x2c03fb,
    0x3fd001c,
    0x2a03fb,
    0x3fc001f,
    0x2703fb,
    0x3fc0022,
    0x2403fc,
    0x3fc0024,

    /* ratio is greater than 0.75 */
    0x3f0000,
    0x0,
    0x3f0000,
    0x1,
    0x3f03fe,
    0x3,
    0x3f03fd,
    0x3ff0005,
    0x3f03fc,
    0x3ff0006,
    0x3d03fc,
    0x3ff0008,
    0x3c03fb,
    0x3ff000a,
    0x3b03fb,
    0x3fe000c,
    0x3803fb,
    0x3fe000f,
    0x3603fb,
    0x3fe0011,
    0x3403fa,
    0x3fe0014,
    0x3203fb,
    0x3fd0016,
    0x2f03fb,
    0x3fd0019,
    0x2c03fb,
    0x3fd001c,
    0x2a03fb,
    0x3fc001f,
    0x2703fb,
    0x3fc0022,
    0x2403fc,
    0x3fc0024,

    /* ratio is not less than 0.5 */
    0x31000a,
    0x3fb000a,
    0x330008,
    0x3fa000b,
    0x320007,
    0x3fa000d,
    0x330005,
    0x3f9000f,
    0x330004,
    0x3f90010,
    0x320003,
    0x3f90012,
    0x320001,
    0x3f90014,
    0x320000,
    0x3f80016,
    0x3103ff,
    0x3f80018,
    0x3003fe,
    0x3f8001a,
    0x2f03fd,
    0x3f8001c,
    0x2e03fc,
    0x3f8001e,
    0x2c03fc,
    0x3f80020,
    0x2c03fb,
    0x3f80021,
    0x2a03fa,
    0x3f90023,
    0x2803fa,
    0x3f90025,
    0x2703f9,
    0x3f90027,

    /* ratio is greater than 0.33 */
    0x170011,
    0x70011,
    0x170011,
    0x70011,
    0x160010,
    0x80012,
    0x160010,
    0x80012,
    0x160010,
    0x80012,
    0x17000f,
    0x80012,
    0x16000f,
    0x90012,
    0x15000f,
    0x90013,
    0x16000e,
    0x90013,
    0x16000e,
    0x90013,
    0x15000e,
    0xa0013,
    0x16000d,
    0xa0013,
    0x15000d,
    0xa0014,
    0x14000d,
    0xb0014,
    0x15000c,
    0xb0014,
    0x15000c,
    0xb0014,
    0x15000c,
    0xb0014,

    /* ratio is greater than 0.25 */
    0x130011,
    0xb0011,
    0x130011,
    0xb0011,
    0x130010,
    0xc0011,
    0x130010,
    0xc0011,
    0x130010,
    0xc0011,
    0x130010,
    0xc0011,
    0x130010,
    0xc0011,
    0x14000f,
    0xc0011,
    0x13000f,
    0xd0011,
    0x12000f,
    0xd0012,
    0x12000f,
    0xd0012,
    0x12000f,
    0xd0012,
    0x12000f,
    0xd0012,
    0x13000e,
    0xd0012,
    0x12000e,
    0xe0012,
    0x12000e,
    0xe0012,
    0x12000e,
    0xe0012,

    /* ratio is less than 0.25 */
    0x110011,
    0xd0011,
    0x120010,
    0xd0011,
    0x120010,
    0xd0011,
    0x120010,
    0xd0011,
    0x120010,
    0xd0011,
    0x120010,
    0xd0011,
    0x120010,
    0xd0011,
    0x110010,
    0xe0011,
    0x110010,
    0xe0011,
    0x12000f,
    0xe0011,
    0x12000f,
    0xe0011,
    0x12000f,
    0xe0011,
    0x12000f,
    0xe0011,
    0x12000f,
    0xe0011,
    0x12000f,
    0xe0011,
    0x12000f,
    0xe0011,
    0x11000f,
    0xf0011,
};

/* anti-flash filter coefficient */
td_u32 g_deflicker_vf_coef[TDE_PARA_VTABLE_ORG_SIZE * TDE_PARA_HTABLE_NUM] = {
    /* ratio is greater than 1 */
    0x1f0011,
    0x3ff0011,
    0x1f0011,
    0x3ff0011,
    0x1e0010,
    0x12,
    0x1e0010,
    0x12,
    0x1e000f,
    0x13,
    0x1e000f,
    0x13,
    0x1e000d,
    0x10014,
    0x1e000d,
    0x10014,
    0x1d000b,
    0x20016,
    0x1d000b,
    0x20016,
    0x1d000a,
    0x20017,
    0x1d000a,
    0x20017,
    0x1d0008,
    0x30018,
    0x1d0008,
    0x30018,
    0x1c0007,
    0x40019,
    0x1c0007,
    0x40019,
    0x1a0006,
    0x6001a,
    /* ratio is equal to  1 */
    0x200010,
    0x10,
    0x1f000f,
    0x10011,
    0x1f000e,
    0x10012,
    0x1f000c,
    0x20013,
    0x1f000b,
    0x20014,
    0x1f000a,
    0x20015,
    0x1e0009,
    0x30016,
    0x1e0009,
    0x30016,
    0x1e0008,
    0x30017,
    0x1d0008,
    0x40017,
    0x1d0007,
    0x40018,
    0x1c0007,
    0x40019,
    0x1c0007,
    0x40019,
    0x1c0006,
    0x4001a,
    0x1b0006,
    0x5001a,
    0x1b0005,
    0x5001b,
    0x1b0005,
    0x5001b,
    /* ratio is less than 1, and not less than 0.75 */
    0x1f0011,
    0x3ff0011,
    0x1f0011,
    0x3ff0011,
    0x1e0010,
    0x12,
    0x1e0010,
    0x12,
    0x1e000e,
    0x10013,
    0x1e000e,
    0x10013,
    0x1e000d,
    0x10014,
    0x1e000d,
    0x10014,
    0x1e000b,
    0x20015,
    0x1e000b,
    0x20015,
    0x1c000a,
    0x30017,
    0x1c000a,
    0x30017,
    0x1b0009,
    0x40018,
    0x1b0009,
    0x40018,
    0x1b0007,
    0x50019,
    0x1b0007,
    0x50019,
    0x1a0006,
    0x6001a,
    /* ratio is less than 0.75, and not less than 0.5 */
    0x1e0011,
    0x11,
    0x1e0011,
    0x11,
    0x1d0010,
    0x10012,
    0x1d0010,
    0x10012,
    0x1d000f,
    0x10013,
    0x1d000f,
    0x10013,
    0x1d000d,
    0x20014,
    0x1d000d,
    0x20014,
    0x1c000c,
    0x30015,
    0x1c000c,
    0x30015,
    0x1c000a,
    0x40016,
    0x1c000a,
    0x40016,
    0x1b0009,
    0x50017,
    0x1b0009,
    0x50017,
    0x1a0008,
    0x60018,
    0x1a0008,
    0x60018,
    0x190007,
    0x70019,
    /* ratio is less than 0.5, and not less than 0.33 */
    0x1d0011,
    0x10011,
    0x1d0011,
    0x10011,
    0x1d0010,
    0x10012,
    0x1d0010,
    0x10012,
    0x1c000f,
    0x20013,
    0x1c000f,
    0x20013,
    0x1c000d,
    0x30014,
    0x1c000d,
    0x30014,
    0x1b000c,
    0x40015,
    0x1b000c,
    0x40015,
    0x1b000b,
    0x40016,
    0x1b000b,
    0x40016,
    0x1a000a,
    0x50017,
    0x1a000a,
    0x50017,
    0x1a0008,
    0x60018,
    0x1a0008,
    0x60018,
    0x190007,
    0x70019,

    /* ratio is less than 0.33, and not less than 0.25 */
    0x1c0011,
    0x20011,
    0x1c0011,
    0x20011,
    0x1c0010,
    0x20012,
    0x1c0010,
    0x20012,
    0x1b000f,
    0x30013,
    0x1b000f,
    0x30013,
    0x1b000e,
    0x30014,
    0x1b000e,
    0x30014,
    0x1a000d,
    0x40015,
    0x1a000d,
    0x40015,
    0x1a000c,
    0x50015,
    0x1a000c,
    0x50015,
    0x1a000a,
    0x60016,
    0x1a000a,
    0x60016,
    0x190009,
    0x70017,
    0x190009,
    0x70017,
    0x180008,
    0x80018,
    /* ratio is less than 0.25 */
    0x1b0011,
    0x30011,
    0x1b0011,
    0x30011,
    0x1b0010,
    0x30012,
    0x1b0010,
    0x30012,
    0x1a000f,
    0x40013,
    0x1a000f,
    0x40013,
    0x1a000e,
    0x50013,
    0x1a000e,
    0x50013,
    0x1a000d,
    0x50014,
    0x1a000d,
    0x50014,
    0x19000c,
    0x60015,
    0x19000c,
    0x60015,
    0x18000b,
    0x70016,
    0x18000b,
    0x70016,
    0x17000a,
    0x80017,
    0x17000a,
    0x80017,
    0x170009,
    0x90017,
};

/* horizontal DE_PARA_HTABLE_SIZE  unit:byte */
td_u32 g_tde_6x32_coef[TDE_PARA_HTABLE_ORG_SIZE * TDE_PARA_HTABLE_NUM] = {
    /* ratio is greater than 1 */
    0x5ff800,
    0x3fe0143a,
    0x0,
    0x4ffc00,
    0x3fe01839,
    0x0,
    0x2ffc00,
    0x3fe02039,
    0x0,
    0x1ffc00,
    0x3fe02838,
    0x0,
    0xffc00,
    0x3fd02c39,
    0x0,
    0x3ff00000,
    0x3fd03437,
    0x0,
    0x3fe00000,
    0x3fd03c36,
    0x0,
    0x3fd00000,
    0x3fc04436,
    0x0,
    0x3fd00000,
    0x3fc04c33,
    0x1,
    0x3fc00000,
    0x3fc05432,
    0x1,
    0x3fc00000,
    0x3fc0602f,
    0x1,
    0x3fb00000,
    0x3fb0682f,
    0x1,
    0x3fb00000,
    0x3fb0702d,
    0x1,
    0x3fb00000,
    0x3fb0782b,
    0x1,
    0x3fb00400,
    0x3fb08028,
    0x1,
    0x3fb00400,
    0x3fb08c25,
    0x1,
    0x3fb00400,
    0x3fb09423,
    0x1,

    /* ratio is equal to 1 */
    0x0,
    0x3f,
    0x0,
    0x4ffc00,
    0x3fe01839,
    0x0,
    0x2ffc00,
    0x3fe02039,
    0x0,
    0x1ffc00,
    0x3fe02838,
    0x0,
    0xffc00,
    0x3fd02c39,
    0x0,
    0x3ff00000,
    0x3fd03437,
    0x0,
    0x3fe00000,
    0x3fd03c36,
    0x0,
    0x3fd00000,
    0x3fc04436,
    0x0,
    0x3fd00000,
    0x3fc04c33,
    0x1,
    0x3fc00000,
    0x3fc05432,
    0x1,
    0x3fc00000,
    0x3fc0602f,
    0x1,
    0x3fb00000,
    0x3fb0682f,
    0x1,
    0x3fb00000,
    0x3fb0702d,
    0x1,
    0x3fb00000,
    0x3fb0782b,
    0x1,
    0x3fb00400,
    0x3fb08028,
    0x1,
    0x3fb00400,
    0x3fb08c25,
    0x1,
    0x3fb00400,
    0x3fb09423,
    0x1,

    /* ratio is less than 1, and not less than 0.75 */
    0xffe000,
    0x3f803c2f,
    0x3,
    0xdfe000,
    0x3f704031,
    0x3,
    0xcfe000,
    0x3f704831,
    0x2,
    0xafe000,
    0x3f704c32,
    0x2,
    0x9fe000,
    0x3f705431,
    0x2,
    0x7fe400,
    0x3f705831,
    0x2,
    0x6fe400,
    0x3f706031,
    0x1,
    0x5fe400,
    0x3f706830,
    0x1,
    0x4fe800,
    0x3f806c2e,
    0x1,
    0x2fe800,
    0x3f80742f,
    0x0,
    0x1fec00,
    0x3f80782e,
    0x0,
    0xfec00,
    0x3f90802d,
    0x3ff,
    0x3fffec00,
    0x3f90882c,
    0x3ff,
    0x3feff000,
    0x3fa08c2a,
    0x3ff,
    0x3fdff000,
    0x3fa0902b,
    0x3fe,
    0x3fcff400,
    0x3fb09828,
    0x3fe,
    0x3fcff400,
    0x3fc09c27,
    0x3fd,

    /* ratio is less than 0.75, and not less than 0.5 */
    0x1200400,
    0x10481e,
    0x3fc,
    0x1200400,
    0x204c1c,
    0x3fc,
    0x1100400,
    0x204c1d,
    0x3fc,
    0x1100000,
    0x20501d,
    0x3fc,
    0x1000000,
    0x30501d,
    0x3fc,
    0xf00000,
    0x30541d,
    0x3fc,
    0xfffc00,
    0x40581c,
    0x3fc,
    0xeffc00,
    0x40581d,
    0x3fc,
    0xeffc00,
    0x505c1b,
    0x3fc,
    0xdff800,
    0x505c1d,
    0x3fc,
    0xcff800,
    0x60601c,
    0x3fc,
    0xcff800,
    0x60601c,
    0x3fc,
    0xbff800,
    0x70601c,
    0x3fc,
    0xbff400,
    0x70641b,
    0x3fd,
    0xaff400,
    0x80641b,
    0x3fd,
    0xaff400,
    0x80681a,
    0x3fd,
    0x9ff400,
    0x90681a,
    0x3fd,

    /* ratio is less than 0.5, and not less than 0.33 */
    0xf01c00,
    0x703c13,
    0x1,
    0xf01c00,
    0x803c12,
    0x1,
    0xe01c00,
    0x803c12,
    0x2,
    0xe01c00,
    0x803c12,
    0x2,
    0xe01c00,
    0x803c12,
    0x2,
    0xe01800,
    0x904011,
    0x2,
    0xd01800,
    0x904012,
    0x2,
    0xd01800,
    0x904012,
    0x2,
    0xd01800,
    0x904011,
    0x3,
    0xd01400,
    0x904012,
    0x3,
    0xc01400,
    0xa04012,
    0x3,
    0xc01400,
    0xa04012,
    0x3,
    0xc01400,
    0xa04411,
    0x3,
    0xc01400,
    0xa04411,
    0x3,
    0xc01000,
    0xb04410,
    0x4,
    0xb01000,
    0xb04411,
    0x4,
    0xb01000,
    0xb04411,
    0x4,

    /* ratio is less than 0.33,and not less than 0.25 */
    0xf01c00,
    0x703c13,
    0x1,
    0xf01c00,
    0x803c12,
    0x1,
    0xe01c00,
    0x803c12,
    0x2,
    0xe01c00,
    0x803c12,
    0x2,
    0xe01c00,
    0x803c12,
    0x2,
    0xe01800,
    0x904011,
    0x2,
    0xd01800,
    0x904012,
    0x2,
    0xd01800,
    0x904012,
    0x2,
    0xd01800,
    0x904011,
    0x3,
    0xd01400,
    0x904012,
    0x3,
    0xc01400,
    0xa04012,
    0x3,
    0xc01400,
    0xa04012,
    0x3,
    0xc01400,
    0xa04411,
    0x3,
    0xc01400,
    0xa04411,
    0x3,
    0xc01000,
    0xb04410,
    0x4,
    0xb01000,
    0xb04411,
    0x4,
    0xb01000,
    0xb04411,
    0x4,

    /* ratio is less than 0.25 */
    0xe02000,
    0x803812,
    0x2,
    0xe02000,
    0x803811,
    0x3,
    0xe02000,
    0x803811,
    0x3,
    0xe01c00,
    0x903c10,
    0x3,
    0xd01c00,
    0x903c11,
    0x3,
    0xd01c00,
    0x903c11,
    0x3,
    0xd01c00,
    0x903c11,
    0x3,
    0xd01c00,
    0x903c11,
    0x3,
    0xd01800,
    0xa03c10,
    0x4,
    0xd01800,
    0xa03c10,
    0x4,
    0xc01800,
    0xa03c11,
    0x4,
    0xc01800,
    0xa04010,
    0x4,
    0xc01800,
    0xa04010,
    0x4,
    0xc01400,
    0xb04010,
    0x4,
    0xc01400,
    0xb0400f,
    0x5,
    0xb01400,
    0xb04010,
    0x5,
    0xb01400,
    0xb04010,
    0x5,
};

#ifdef TDE_TIME_COUNT
tde_timeval g_time_start;
#endif

/* Head address of argument table used as config */
typedef struct {
    td_phys_addr_t hf_coef_addr;
    td_phys_addr_t vf_coef_addr;

#if (TDE_CAPABILITY & DEFLICKER)
    td_phys_addr_t deflicker_vf_coef_addr;
#endif
} tde_para_table;

typedef struct {
    td_u32 alpha_en;
    td_u32 luma_en;
    td_u32 chrome_en;
} tde_filtermode;

/* Base addr of register after mapping */
volatile td_u32 *g_base_vir_addr = TD_NULL;
static td_bool g_use_dtsi = TD_TRUE;

/* Pointer of TDE clock register after mapping */
static volatile td_u32 *g_tde_clock_vir = TD_NULL;
/* Head address of config argument table */
static tde_para_table g_para_table = { 0 };

/* Deflicker level, default is auto */
#if (TDE_CAPABILITY & DEFLICKER)
static drv_tde_deflicker_level g_deflicker_level = DRV_TDE_DEFLICKER_LEVEL_AUTO;
#endif

/* alpha threshold switch */
static td_bool g_alpha_threshold = TD_FALSE;

/* alpha threshold value */
static td_u8 g_alpha_threshold_value = 0xff;

static td_u32 g_rgb_truncation_mode = 1; /* 1 for rgb truncation mode */

static tde_alu_mode g_cbmctrl_alu_mode[TDE_BUTT + 1] = {
    TDE_SRC2_BYPASS, TDE_ALU_NONE, TDE_ALU_ROP, TDE_SRC2_BYPASS, TDE_ALU_MASK_ROP1, TDE_ALU_MASK_ROP2,
    TDE_ALU_MASK_BLEND, TDE_SRC2_BYPASS, TDE_SRC2_BYPASS, TDE_SRC2_BYPASS, TDE_SRC2_BYPASS, TDE_BUTT
};

static td_s32 tde_hal_init_para_table(td_void);
static td_void tde_hal_node_set_clutcolor_key_para(tde_hw_node *hw_node, const tde_color_key_cmd *color_key);
static td_void tde_hal_node_set_ycbcr_color_key_para(tde_hw_node *hw_node, const tde_color_key_cmd *color_key);
static td_void tde_hal_node_set_argbcolor_key_para(tde_hw_node *hw_node, const tde_color_key_cmd *color_key);
static td_u32 tde_hal_get_color_key_mode(const drv_tde_color_key_comp *color_key);
static td_u32 tde_hal_get_ycb_cr_key_mask(td_u8 cr, td_u8 cb, td_u8 cy, td_u8 alpha);
static td_u32 tde_hal_get_clut_key_mask(td_u8 clut, td_u8 alpha);
static td_u32 tde_hal_get_argb_key_mask(td_u8 blue, td_u8 green, td_u8 red, td_u8 alpha);
static td_s32 tde_hal_getbpp_by_fmt(tde_color_fmt fmt);
static td_u32 tde_hal_get_resize_para_htable(td_u32 step);
static td_u32 tde_hal_get_resize_para_vtable(td_u32 step);

static td_void tde_hal_init_queue(td_void);
td_void tde_hal_set_clock(td_bool enable);
static td_s32 tde_hal_getbpp_by_fmt1(tde_color_fmt fmt);
static td_s32 tde_hal_getbpp_by_fmt2(tde_color_fmt fmt);
static td_s32 tde_hal_getbpp_by_fmt4(tde_color_fmt fmt);
static td_s32 tde_hal_getbpp_by_fmt8(tde_color_fmt fmt);
static td_s32 tde_hal_getbpp_by_fmt16(tde_color_fmt fmt);
static td_s32 tde_hal_getbpp_by_fmt24(tde_color_fmt fmt);
static td_s32 tde_hal_getbpp_by_fmt32(tde_color_fmt fmt);

#define registder_flush() mb()

static td_bool tde_hal_is_operation_support(tde_alu_mode alu_mode, td_u32 capability, td_u32 oper_mode);
static td_u32 tde_hal_get_zme_in_fmt(tde_color_fmt in_drv_fmt);
static td_u32 tde_hal_get_hpzme_mode(td_u32 out_rect_width, td_u32 in_rect_width);
static td_void tde_hal_get_filter_mode(drv_tde_filter_mode filter_mode, tde_filtermode *flt_mode);
static td_s32 tde_hal_get_ver_scale_coeff(td_u32 out_rect_height, td_u32 in_rect_height, td_s32 *ver_scale_coeff);

static td_void tde_fill_data_by_fmt(tde_hw_node *hw_node, td_u32 data, tde_color_fmt fmt, td_u8 flag)
{
    td_u32 bpp;
    td_u32 i;
    td_u32 cell;
    bpp = (td_u32)tde_hal_getbpp_by_fmt(fmt);
    if (bpp != 0xffffffff) {
        cell = data & (0xffffffff >> (32 - bpp)); /* 32 bpp The not */
        for (i = 0; i < (32 / bpp); i++) { /* 32 bpp fmt */
            if (flag) {
                (hw_node->src2_fill.bits.src2_color_fill) |= (cell << (i * bpp));
            } else {
                (hw_node->src1_fill.bits.src1_color_fill) |= (cell << (i * bpp));
            }
        }
    } else {
        if (flag) {
            (hw_node->src2_fill.bits.src2_color_fill) = data;
        } else {
            (hw_node->src1_fill.bits.src1_color_fill) = data;
        }
    }
    return;
}

td_void tde_hal_set_base_vir_addr(td_u32 *temp_base_vir_addr)
{
    g_base_vir_addr = temp_base_vir_addr;
}

td_void tde_init_set_rgb_truncation_mode(td_u32 rgb_truncation_mode)
{
    g_rgb_truncation_mode = rgb_truncation_mode;
}

#ifdef TDE_COREDUMP_DEBUG
volatile td_u32 *tde_hal_get_base_vir_addr(td_void)
{
    return g_base_vir_addr;
}
#endif

/*
 * Function:      tde_hal_init
 * Description:   map the base address for tde
 * Input:         base_addr: the base address of tde
 * Return:        success/fail
 */
td_s32 tde_hal_init(td_u32 base_addr)
{
    /* init the pool memory of tde, CNcomment: Initialize the TDE memory pool */
    if (wmeminit() != TD_SUCCESS) {
        goto TDE_INIT_ERR;
    }
    /* config start address for the parameter, CNcomment: The first address configuration parameter table */
    if (tde_hal_init_para_table() != TD_SUCCESS) {
        wmemterm();
        goto TDE_INIT_ERR;
    }

    /* map address for the register, CNcomment: Register mapping */
    g_use_dtsi = TD_TRUE;
    if (g_base_vir_addr == TD_NULL) {
        g_base_vir_addr = (volatile td_u32 *)tde_reg_map(base_addr, TDE_REG_SIZE);
        g_use_dtsi = TD_FALSE;
    }

    if (g_base_vir_addr == TD_NULL) {
        wmemterm();
        goto TDE_INIT_ERR;
    }
    /* set limit of clock and div, CNcomment: Set the clock threshold, clock frequency division */
    g_tde_clock_vir = (volatile td_u32 *)tde_reg_map(TDE_REG_CLOCK, 4); /* 4 size */
    if (g_tde_clock_vir == TD_NULL) {
        wmemterm();
        tde_reg_unmap(g_base_vir_addr, TDE_REG_SIZE);
        goto TDE_INIT_ERR;
    }

    return TD_SUCCESS;

TDE_INIT_ERR:
    return TD_FAILURE;
}

td_void tde_hal_resume_init(td_void)
{
    tde_hal_set_clock(TD_TRUE);

    tde_hal_ctl_reset();

    tde_hal_init_queue();

    return;
}

td_s32 tde_hal_open(td_void)
{
    return TD_SUCCESS;
}

/*
 * Function:      tde_hal_release
 * Description:   release the address that had map
 * Return:        success/fail
 */
td_void tde_hal_release(td_void)
{
    td_void *buf = TD_NULL;
    if (g_para_table.hf_coef_addr != 0) {
        buf = (td_void *)wgetvrt(g_para_table.hf_coef_addr);
        if (buf != TD_NULL) {
            tde_free(buf);
            g_para_table.hf_coef_addr = 0;
        }
    }
    if (g_para_table.vf_coef_addr != 0) {
        buf = (td_void *)wgetvrt(g_para_table.vf_coef_addr);
        if (buf != TD_NULL) {
            tde_free(buf);
            g_para_table.vf_coef_addr = 0;
        }
    }

#if (TDE_CAPABILITY & DEFLICKER)
    if (g_para_table.deflicker_vf_coef_addr != 0) {
        buf = (td_void *)wgetvrt(g_para_table.deflicker_vf_coef_addr);
        if (buf != TD_NULL) {
            tde_free(buf);
            g_para_table.deflicker_vf_coef_addr = 0;
        }
    }
#endif

    /* unmap, CNcomment: Remove the mapping */
    if (g_tde_clock_vir != TD_NULL) {
        tde_reg_unmap(g_tde_clock_vir, 4); /* 4 size */
    }
    g_tde_clock_vir = TD_NULL;
    /* unmap the base address, CNcomment: Reflect the base address */
    if (g_use_dtsi == TD_FALSE) {
        tde_reg_unmap(g_base_vir_addr, TDE_REG_SIZE);
    }
    g_base_vir_addr = TD_NULL;

    /* free the pool of memory, CNcomment: TDE memory pool to initialize */
    wmemterm();

    return;
}

td_bool tde_hal_ctl_is_idle(td_void)
{
    if (g_base_vir_addr == TD_NULL) {
        tde_error("null pointer\n");
        return TD_FALSE;
    }
    return (td_bool)(!(tde_read_reg(g_base_vir_addr, TDE_STA) & 0x1));
}

/*
 * Function:      tde_hal_ctl_is_idle_safely
 * Description:   get the state of tde one more time ,make sure it's idle
 * Return:        True: Idle/False: Busy
 */
td_bool tde_hal_ctl_is_idle_safely(td_void)
{
    td_u32 i;

    /*
     * get the state of tde one more time ,make sure it's idle
     * CNcomment: Continuous read hardware status for many times, make sure complete TDE
     */
    for (i = 0; i < TDE_MAX_READ_STATUS_TIME; i++) {
        if (!tde_hal_ctl_is_idle()) {
            return TD_FALSE;
        }
    }
    return TD_TRUE;
}

td_u32 tde_hal_ctl_int_status(td_void)
{
    volatile td_u32 value;

    if (g_base_vir_addr == TD_NULL) {
        tde_error("null pointer\n");
        return 0;
    }

    value = tde_read_reg(g_base_vir_addr, TDE_INT);
    /* clear all status */
    tde_write_reg(g_base_vir_addr, TDE_INTCLR, value);
    return value;
}

td_void tde_hal_ctl_reset(td_void)
{
    if (g_base_vir_addr == TD_NULL) {
        tde_error("null pointer\n");
        return;
    }
    tde_write_reg(g_base_vir_addr, TDE_INTCLR, 0xf);
#ifdef CONFIG_TDE_MISCELLANEOUS_OUTSTANDING
    tde_write_reg(g_base_vir_addr, TDE_MISCELLANEOUS, 0x0100645a);
#else
    tde_write_reg(g_base_vir_addr, TDE_MISCELLANEOUS, 0x0100647f);
#endif
    return;
}

/*
 * Function:      tde_hal_set_clock
 * Description:   enable or disable the clock of TDE
 * Input:         td_bool bEnable:enable/disable
 */
#ifndef CONFIG_USE_SYS_CONFIG
td_void tde_hal_set_clock(td_bool enable)
{
    if (g_tde_clock_vir == TD_NULL) {
        tde_error("null pointer\n", __FUNCTION__, __LINE__);
        return;
    }
    if (enable) {
        /* enable clock */
        *g_tde_clock_vir |= 0x1010;

        /* cancel reset */
        *g_tde_clock_vir &= ~0x1;
    } else {
#ifdef TDE_LOWPOWER
        /* reset */
        *g_tde_clock_vir |= 0x1;

        /* disable clock */
        *g_tde_clock_vir &= ~0x10;
#endif
    }
}
#else
td_void tde_hal_set_clock(td_bool enable)
{
    td_bool tmp;
    ot_mpp_chn mpp_chn;
    mpp_chn.mod_id = OT_ID_TDE;
    mpp_chn.dev_id = 0;

    if (enable == TD_TRUE) {
        tmp = TD_TRUE;
        call_sys_drv_ioctrl(&mpp_chn, SYS_TDE_CLK_EN, &tmp);
        tmp = TD_FALSE;
        call_sys_drv_ioctrl(&mpp_chn, SYS_TDE_RESET_SEL, &tmp);
    } else {
#ifdef TDE_LOWPOWER
        tmp = TD_TRUE;
        call_sys_drv_ioctrl(&mpp_chn, SYS_TDE_RESET_SEL, &tmp);
        tmp = TD_FALSE;
        call_sys_drv_ioctrl(&mpp_chn, SYS_TDE_CLK_EN, &tmp);
#endif
    }
    return;
}
#endif

/*
 * Function:      tde_hal_node_init_nd
 * Description:   init the software node struct for tde
 * Input:         hw_node: the pointer of software node struct
 */
td_s32 tde_hal_node_init_nd(tde_hw_node **hw_node)
{
    td_void *buf = TD_NULL;

    if (hw_node == TD_NULL) {
        return TD_FAILURE;
    }
    buf = (td_void *)tde_malloc(sizeof(tde_hw_node) + TDE_NODE_HEAD_BYTE + TDE_NODE_TAIL_BYTE);
    if (buf == TD_NULL) {
        tde_error("malloc (%lu) failed, wgetfreenum(%u)!\n",
                  (unsigned long)(sizeof(tde_hw_node) + TDE_NODE_HEAD_BYTE + TDE_NODE_TAIL_BYTE), wgetfreenum());
        return DRV_ERR_TDE_NO_MEM;
    }

    *hw_node = (tde_hw_node *)((td_u8 *)buf + TDE_NODE_HEAD_BYTE);

    return TD_SUCCESS;
}

#ifdef CONFIG_GFX_MMU_SUPPORT
td_void tde_hal_free_tmp_buf(const tde_hw_node *hw_node)
{
    return;
}
#endif

/*
 * Function:      tde_hal_free_node_buf
 * Description:   Free TDE operate node buffer
 * Input:         hw_node:Node struct pointer.
 */
td_void tde_hal_free_node_buf(tde_hw_node *hw_node)
{
    td_void *buf = TD_NULL;

    if (hw_node == TD_NULL) {
        return;
    }
    buf = (td_void *)hw_node - TDE_NODE_HEAD_BYTE;
    if (buf == TD_NULL) {
        return;
    }

#ifdef CONFIG_GFX_MMU_SUPPORT
    tde_hal_free_tmp_buf(hw_node);
#endif

    tde_free(buf);
    return;
}

/*
 * Function:      tde_hal_node_execute
 * Description:   start list of tde
 * Input:
 *                nodephy_addr: the start address of head node address
 *                update:the head node update set
 *                aq_use_buff: whether use temp buffer
 */
td_s32 tde_hal_node_execute(td_phys_addr_t nodephy_addr, td_u64 update, td_bool aq_use_buff)
{
    ot_unused(update);
    ot_unused(aq_use_buff);
    if (g_base_vir_addr == TD_NULL) {
        tde_error("null pointer\n");
        return TD_FAILURE;
    }
    /* tde is idle */
    if (tde_hal_ctl_is_idle_safely()) {
        tde_hal_set_clock(TD_TRUE);
        tde_hal_ctl_reset();
        tde_hal_init_queue();

        /* write the first node address */
        tde_write_reg(g_base_vir_addr, TDE_AQ_NADDR_LOW, get_low_addr(nodephy_addr));
        tde_write_reg(g_base_vir_addr, TDE_AQ_NADDR_HI, get_high_addr(nodephy_addr));
        osal_dmb();
        osal_udelay(100); /* 100 time out */
        /* start Aq list, CNcomment : Start the Aq */
        tde_write_reg(g_base_vir_addr, TDE_CTRL, 0x1);
    } else {
        return TD_FAILURE;
    }
#ifdef TDE_TIME_COUNT
    (td_void) osal_gettimeofday(&g_time_start);
    /* 1000000 us */
    printk("tde_hal_node_execute: start time : %ld!\n", (td_u32)g_time_start.tv_sec * 1000000 +
        g_time_start.tv_usec);
#endif
    return TD_SUCCESS;
}

/*
 * Function:      tde_hal_node_enable_complete_int
 * Description:   enable the finish interrupt of node
 * Input:         buf: buffer of node
 */
td_void tde_hal_node_enable_complete_int(td_void *buf)
{
    tde_hw_node *hw_node = (tde_hw_node *)buf;
    if (buf == TD_NULL) {
        tde_error("hw_node Buf is null !\n");
        return;
    }
    hw_node->tde_intmask.bits.eof_mask = 0;
    hw_node->tde_intmask.bits.timeout_mask = 0x1;
    hw_node->tde_intmask.bits.bus_err_mask = 0x1;
    hw_node->tde_intmask.bits.eof_end_mask = 0x1;
    return;
}

td_void tde_hal_next_node_addr(td_void *buf, td_phys_addr_t phy_addr)
{
    tde_hw_node *hw_node = (tde_hw_node *)buf;
    if (buf == TD_NULL) {
        tde_error("hw_node Buf is null !\n");
        return;
    }
    hw_node->tde_pnext_low.bits.p_next_low = get_low_addr(phy_addr);
    hw_node->tde_pnext_hi.bits.p_next_hi = get_high_addr(phy_addr);
    return;
}

#if (TDE_CAPABILITY & SYNC)
td_void tde_hal_node_enable_sync(td_void *buf)
{
    tde_hw_node *hw_node = (tde_hw_node *)buf;
    if (buf == TD_NULL) {
        tde_error("Buf is null !\n");
        return;
    }
    hw_node->des_ctrl.bits.des_bind_en = 0x1;
    hw_node->des_ctrl.bits.des_bind_mode = 0x0;
    hw_node->des_ctrl.bits.des_h_scan_ord = 0;
    hw_node->des_ctrl.bits.des_v_scan_ord = 0;
    hw_node->des_safe_dist.all = hw_node->des_ch0_stride.all;
    hw_node->des_safe_dist_inverse.all = hw_node->des_ch0_stride.all / 2; /* 2 half */
    return;
}
#endif

/*
 * Function:      tde_hal_node_set_src1
 * Description:   set the info for source of bitmap 1
 * Input:         hw_node: pointer of node
 *                drv_surface: bitmap info
 */
td_void tde_hal_node_set_src1(tde_hw_node *hw_node, const tde_surface_msg *drv_surface)
{
    td_u32 bpp;
    td_phys_addr_t phy_addr;
    if (hw_node == TD_NULL) {
        tde_error("HWNode is null !\n");
        return;
    }
    if (drv_surface == TD_NULL) {
        tde_error("DrvSurface is null !\n");
        return;
    }
    /* set the source bitmap attribute info, CNcomment: Configure attribute information source bitmap */
    hw_node->src1_ctrl.bits.src1_fmt = (td_u32)drv_surface->color_format;
    hw_node->src1_ctrl.bits.src1_alpha_range = 1 - (td_u32)drv_surface->alpha_max_is_255;
    hw_node->src1_ctrl.bits.src1_h_scan_ord = (td_u32)drv_surface->hor_scan;
    hw_node->src1_ctrl.bits.src1_v_scan_ord = (td_u32)drv_surface->ver_scan;
    /*
     * file zero of low area and top area use low area extend
     * CNcomment: Have been using low filling is 0, high use low expansion mode
     */
    hw_node->src1_ctrl.bits.src1_rgb_exp = 0;

    hw_node->src1_imgsize.bits.src1_width = drv_surface->width - 1;
    hw_node->src1_imgsize.bits.src1_height = drv_surface->height - 1;

    bpp = (td_u32)tde_hal_getbpp_by_fmt(drv_surface->color_format);

    phy_addr = drv_surface->phys_addr + (td_u64)(drv_surface->ypos) * (td_u64)(drv_surface->pitch) +
                  (((td_u64)(drv_surface->xpos) * (td_u64)(bpp)) >> 3); /* bpp 2^3 */

    hw_node->src1_ch0_addr_low.bits.src1_ch0_addr_low = get_low_addr(phy_addr);
    hw_node->src1_ch0_addr_high.bits.src1_ch0_addr_high = get_high_addr(phy_addr);

    if (drv_surface->color_format <= TDE_DRV_COLOR_FMT_RGB565) {
        hw_node->src1_ctrl.bits.src1_argb_order = drv_surface->rgb_order;
    }
    /*
     * target bitmapis same with source bitmap 1,so not need set.
     * config the node, CNcomment: Configure the cache node
     */
    hw_node->src1_ch0_stride.bits.src1_ch0_stride = drv_surface->pitch;
    return;
}

static td_void tde_hal_set_src2_ctrl(tde_hw_node *hw_node, const tde_surface_msg *drv_surface)
{
    hw_node->src2_ctrl.bits.src2_fmt = (td_u32)drv_surface->color_format;
    hw_node->src2_ctrl.bits.src2_alpha_range = 0;
    hw_node->src2_ctrl.bits.src2_h_scan_ord = (td_u32)drv_surface->hor_scan;
    hw_node->src2_ctrl.bits.src2_v_scan_ord = (td_u32)drv_surface->ver_scan;
}

static td_void tde_hal_set_src2_img_size(tde_hw_node *hw_node, const tde_surface_msg *drv_surface)
{
    hw_node->src2_imgsize.bits.src2_width = drv_surface->width - 1;
    hw_node->src2_imgsize.bits.src2_height = drv_surface->height - 1;
}

static td_void tde_hal_set_src2_addr(tde_hw_node *hw_node, const tde_surface_msg *drv_surface, td_phys_addr_t *phy_addr,
    td_phys_addr_t *cb_crphy_addr)
{
    td_u32 bpp = (td_u32)tde_hal_getbpp_by_fmt(drv_surface->color_format);
    if (drv_surface->color_format >= TDE_DRV_COLOR_FMT_YCBCR400MBP) {
        *phy_addr = drv_surface->phys_addr + (td_u64)(drv_surface->ypos) * (td_u64)(drv_surface->pitch) +
                    (((td_u64)(drv_surface->xpos) * 8) >> 3); /* 3 8 xpos alg data */

        hw_node->src2_ch0_addr_low.bits.src2_ch0_addr_low = get_low_addr(*phy_addr);
        hw_node->src2_ch0_addr_high.bits.src2_ch0_addr_high = get_high_addr(*phy_addr);
        switch (drv_surface->color_format) {
            case TDE_DRV_COLOR_FMT_YCBCR422MBH:
                *cb_crphy_addr = drv_surface->cbcr_phys_addr +
                                 (td_u64)(drv_surface->ypos) * (td_u64)(drv_surface->cb_cr_pitch) +
                                 ((td_u64)(drv_surface->xpos) / 2 * 2); /* 2 xpos alg data cb_crphy_addr */
                break;
            case TDE_DRV_COLOR_FMT_YCBCR422MBV:
                *cb_crphy_addr = drv_surface->cbcr_phys_addr +
                                 (td_u64)(drv_surface->ypos / 2) * (td_u64)(drv_surface->cb_cr_pitch) + /* 2 alg */
                                 (((td_u64)(drv_surface->xpos) * 16) >> 3); /* 16 3 alg data cb_crphy_addr */
                break;
            case TDE_DRV_COLOR_FMT_YCBCR420MB:
                *cb_crphy_addr = drv_surface->cbcr_phys_addr +
                                 (td_u64)(drv_surface->ypos / 2) * (td_u64)(drv_surface->cb_cr_pitch) + /* 2 alg */
                                 ((td_u64)(drv_surface->xpos) / 2 * 2); /* 2 xpos alg data cb_crphy_addr */
                break;
            case TDE_DRV_COLOR_FMT_YCBCR444MB:
                *cb_crphy_addr = drv_surface->cbcr_phys_addr +
                                 (td_u64)(drv_surface->ypos) * (td_u64)(drv_surface->cb_cr_pitch) +
                                 (((td_u64)(drv_surface->xpos) * 16) >> 3); /* 16 3 alg data */
                break;
            default:;
        }

        hw_node->src2_ch1_addr_low.bits.src2_ch1_addr_low = get_low_addr(*cb_crphy_addr);
        hw_node->src2_ch1_addr_high.bits.src2_ch1_addr_high = get_high_addr(*cb_crphy_addr);

        if (drv_surface->color_format == TDE_DRV_COLOR_FMT_YCBCR422MBV) {
            hw_node->src2_ctrl.bits.src2_422v_pro = 1;
        }
    } else {
        *phy_addr = drv_surface->phys_addr + (td_u64)(drv_surface->ypos) *
            /* alg data 2^3 */
            (td_u64)(drv_surface->pitch) + (((td_u64)(drv_surface->xpos) * (td_u64)(bpp)) >> 3);
        hw_node->src2_ch0_addr_low.bits.src2_ch0_addr_low = get_low_addr(*phy_addr);
        hw_node->src2_ch0_addr_high.bits.src2_ch0_addr_high = get_high_addr(*phy_addr);

        if (drv_surface->color_format <= TDE_DRV_COLOR_FMT_RGB565) {
            hw_node->src2_ctrl.bits.src2_argb_order = drv_surface->rgb_order;
        }
    }
}

/*
 * Function:      tde_hal_node_set_src2
 * Description:   set the source bitmap 2
 * Input:         hw_node: pointer of node
 *                drv_surface:  bitmap info
 */
td_void tde_hal_node_set_src2(tde_hw_node *hw_node, const tde_surface_msg *drv_surface)
{
    td_phys_addr_t phy_addr;
    td_phys_addr_t cb_crphy_addr;

    if (hw_node == TD_NULL) {
        tde_error("HWNode is null !\n");
        return;
    }
    if (drv_surface == TD_NULL) {
        tde_error("DrvSurface is null !\n");
        return;
    }

    /* set attribute info for source bitmap, CNcomment:Configure attribute information source bitmap */
    tde_hal_set_src2_ctrl(hw_node, drv_surface);
    /*
     * file zero of low area and top area use low area extend
     * CNcomment: Have been using low filling is 0, high use low expansion mode
     */
    hw_node->src2_ctrl.bits.src2_rgb_exp = 0;

    tde_hal_set_src2_img_size(hw_node, drv_surface);

    phy_addr = drv_surface->phys_addr;
    cb_crphy_addr = drv_surface->cbcr_phys_addr;

    tde_hal_set_src2_addr(hw_node, drv_surface, &phy_addr, &cb_crphy_addr);
    if (drv_surface->color_format <= TDE_DRV_COLOR_FMT_A1) {
        hw_node->src2_alpha.bits.src2_alpha0 = 0x00;
        hw_node->src2_alpha.bits.src2_alpha1 = 0xff;
    }
    hw_node->src2_ch0_stride.bits.src2_ch0_stride = drv_surface->pitch;
    hw_node->src2_ch1_stride.bits.src2_ch1_stride = drv_surface->cb_cr_pitch;

    return;
}

#if (TDE_CAPABILITY & COMPRESS)
td_void tde_hal_node_set_src_to_decompress(tde_hw_node *hw_node, const tde_surface_msg *drv_surface)
{
    if (hw_node == TD_NULL) {
        tde_error("HWNode is null !\n");
        return;
    }
    tde_hal_node_set_src2(hw_node, drv_surface);

    hw_node->src2_ctrl.bits.src2_is_lossless = 0;
    hw_node->src2_ctrl.bits.src2_is_lossless_a = 0;
    hw_node->src2_ctrl.bits.src2_dcmp_en = 1;
    hw_node->src2_ctrl.bits.src2_top_pred_en = 1;

    return;
}
#endif

static td_void tde_hal_node_set_hw_node(tde_hw_node *hw_node, const tde_surface_msg *drv_surface,
    tde_drv_outalpha_from alpha_from)
{
    /* set bitmap attribute info, CNcomment:Configure attribute information source bitmap */
    hw_node->des_ctrl.bits.des_en = 1;
    hw_node->des_ctrl.bits.des_fmt = drv_surface->color_format;
    hw_node->des_ctrl.bits.des_alpha_range = 1 - (td_u32)drv_surface->alpha_max_is_255;
    hw_node->des_ctrl.bits.des_h_scan_ord = (td_u32)drv_surface->hor_scan;
    hw_node->des_ctrl.bits.des_v_scan_ord = (td_u32)drv_surface->ver_scan;
    hw_node->des_ctrl.bits.des_alpha_range = 1 - (td_u32)drv_surface->alpha_max_is_255;
    hw_node->des_ctrl.bits.des_rgb_round = g_rgb_truncation_mode;
    hw_node->cbmalupara.bits.alpha_from = (td_u32)alpha_from;
    hw_node->des_alpha.bits.des_alpha_thd = g_alpha_threshold_value;

    /* set bitmap size info, CNcomment: Configure the bitmap size information */
    hw_node->des_imgsize.bits.des_width = (td_u32)drv_surface->width - 1;
    hw_node->des_imgsize.bits.des_height = (td_u32)drv_surface->height - 1;
}

/*
 * Function:      tde_hal_node_set_tqt
 * Description:   set target bitmap info
 * Input:         hw_node: pointer of node
 *                drv_surface: bitmap info
 *                alpha_from: alpha from
 */
td_void tde_hal_node_set_tqt(tde_hw_node *hw_node, tde_surface_msg *drv_surface, tde_drv_outalpha_from alpha_from)
{
    td_u32 bpp;
    td_phys_addr_t phy_addr;
    td_phys_addr_t cb_crphy_addr;

    if (hw_node == TD_NULL || drv_surface == TD_NULL) {
        tde_error("null pointer !\n");
        return;
    }
    tde_hal_node_set_hw_node(hw_node, drv_surface, alpha_from);
    if (drv_surface->color_format == TDE_DRV_COLOR_FMT_AYCBCR8888) {
        drv_surface->color_format = TDE_DRV_COLOR_FMT_ARGB8888;
        hw_node->des_ctrl.bits.des_argb_order = 0x17;
    }

    if ((drv_surface->color_format <= TDE_DRV_COLOR_FMT_RGB565) ||
        (drv_surface->color_format == TDE_DRV_COLOR_FMT_AYCBCR8888)) {
        hw_node->des_ctrl.bits.des_argb_order = drv_surface->rgb_order;
        if (drv_surface->color_format == TDE_DRV_COLOR_FMT_AYCBCR8888) {
            drv_surface->color_format = TDE_DRV_COLOR_FMT_ARGB8888;
            hw_node->des_ctrl.bits.des_argb_order = 0x17;
        }
    }
    if (drv_surface->color_format == TDE_DRV_COLOR_FMT_RABG8888) {
        hw_node->des_ctrl.bits.des_argb_order = TDE_DRV_ORDER_RABG; /* RABG */
        hw_node->des_ctrl.bits.des_fmt = TDE_DRV_COLOR_FMT_ARGB8888;
    }
    bpp = (td_u32)tde_hal_getbpp_by_fmt(drv_surface->color_format);
    phy_addr = drv_surface->phys_addr + (td_u64)(drv_surface->ypos) * (td_u64)(drv_surface->pitch) +
                  (((td_u64)(drv_surface->xpos) * (td_u64)(bpp)) >> 3); /* 3 bpp narrow 8 */
    hw_node->des_ch0_addr_low.bits.des_ch0_addr_low = get_low_addr(phy_addr);
    hw_node->des_ch0_addr_high.bits.des_ch0_addr_high = get_high_addr(phy_addr);

    cb_crphy_addr = drv_surface->cbcr_phys_addr + (td_u64)(drv_surface->ypos) * (td_u64)(drv_surface->pitch) +
                    (((td_u64)(drv_surface->xpos) * (td_u64)(bpp)) >> 3); /* 3 bpp narrow 8 */
    hw_node->des_ch1_addr_low.bits.des_ch1_addr_low = get_low_addr(cb_crphy_addr);
    hw_node->des_ch1_addr_high.bits.des_ch1_addr_hi = get_high_addr(cb_crphy_addr);

    hw_node->des_ch1_stride.bits.des_ch1_stride = (td_u32)drv_surface->pitch;

    if (drv_surface->color_format == TDE_DRV_COLOR_FMT_YCBCR422) {
        hw_node->des_dswm.bits.des_h_dswm_mode = 1;
    }
    hw_node->des_ch0_stride.bits.des_ch0_stride = (td_u32)drv_surface->pitch;
    return;
}

td_void tde_hal_node_set_corner_rect(tde_hw_node *hw_node, const tde_corner_rect_info *corner_hal_info)
{
    if ((hw_node != TD_NULL) && (corner_hal_info != TD_NULL)) {
        hw_node->tde_dma_corner_reso.bits.dma_corner_width = corner_hal_info->width - 1;
        hw_node->tde_dma_corner_reso.bits.dma_corner_height = corner_hal_info->height - 1;

        hw_node->tde_dma_corner_ctrl.bits.dma_corner_en = 0x1;
        hw_node->tde_dma_corner_ctrl.bits.dma_des_en = 0x1;
#if defined CONFIG_TDE_DMA_CORNER_V2
        hw_node->tde_dma_other_value.bits.dma_other_value = corner_hal_info->outer_color;
        hw_node->tde_dma_corner_value.bits.dma_corner_value = corner_hal_info->inner_color;
#elif defined CONFIG_TDE_DMA_CORNER_V1
        hw_node->tde_dma_corner_ctrl.bits.dma_other_value = corner_hal_info->outer_color;
        hw_node->tde_dma_corner_ctrl.bits.dma_corner_value = corner_hal_info->inner_color;
#endif
    }

    return;
}

#ifdef CONFIG_TDE_DRD_LINE_SUPPORT
td_void tde_hal_node_draw_line(tde_hw_node *hw_node, const tde_line_info *line_hal_info, td_u32 length)
{
    if (length < TDE_MAX_LINE_NUM) {
        return;
    }

    if ((hw_node->des_ctrl.bits.des_fmt == TDE_DRV_COLOR_FMT_PKGVYUY) ||
        (hw_node->des_ctrl.bits.des_fmt == TDE_DRV_COLOR_FMT_YCBCR888) ||
        (hw_node->des_ctrl.bits.des_fmt == TDE_DRV_COLOR_FMT_AYCBCR8888) ||
        (hw_node->des_ctrl.bits.des_fmt == TDE_DRV_COLOR_FMT_YCBCR422)) {
        hw_node->tde_drd_mask.bits.align_mode = 0; /* 0 mode for yuv */
    } else {
        hw_node->tde_drd_mask.bits.align_mode = 0x2; /* 0x2 mode for non yuv */
    }

    hw_node->tde_drd_mask.bits.drd_enable = 0x1;
    hw_node->tde_drd_mask.bits.line0_enable = line_hal_info[0].enable;
    hw_node->tde_drd_mask.bits.line1_enable = line_hal_info[1].enable;
    hw_node->tde_drd_mask.bits.line2_enable = line_hal_info[2].enable; /* 2 line2 */
    hw_node->tde_drd_mask.bits.line3_enable = line_hal_info[3].enable; /* 3 line3 */

    hw_node->tde_drd_line_width.bits.line0_width = line_hal_info[0].thick - 1;
    hw_node->tde_drd_line_width.bits.line1_width = line_hal_info[1].thick - 1;
    hw_node->tde_drd_line_width.bits.line2_width = line_hal_info[2].thick - 1; /* 2 line2 */
    hw_node->tde_drd_line_width.bits.line3_width = line_hal_info[3].thick - 1; /* 3 line3 */

    hw_node->tde_drd_line0_color_cfg.bits.line0_color_fill = line_hal_info[0].color;
    hw_node->tde_drd_line1_color_cfg.bits.line1_color_fill = line_hal_info[1].color;
    hw_node->tde_drd_line2_color_cfg.bits.line2_color_fill = line_hal_info[2].color; /* 2 line2 */
    hw_node->tde_drd_line3_color_cfg.bits.line3_color_fill = line_hal_info[3].color; /* 3 line3 */

    hw_node->tde_drd_line0_st.bits.line0_st_x = line_hal_info[0].start_x;
    hw_node->tde_drd_line0_st.bits.line0_st_y = line_hal_info[0].start_y;
    hw_node->tde_drd_line0_ed.bits.line0_ed_x = line_hal_info[0].end_x;
    hw_node->tde_drd_line0_ed.bits.line0_ed_y = line_hal_info[0].end_y;

    hw_node->tde_drd_line1_st.bits.line1_st_x = line_hal_info[1].start_x;
    hw_node->tde_drd_line1_st.bits.line1_st_y = line_hal_info[1].start_y;
    hw_node->tde_drd_line1_ed.bits.line1_ed_x = line_hal_info[1].end_x;
    hw_node->tde_drd_line1_ed.bits.line1_ed_y = line_hal_info[1].end_y;

    hw_node->tde_drd_line2_st.bits.line2_st_x = line_hal_info[2].start_x; /* 2 line2 */
    hw_node->tde_drd_line2_st.bits.line2_st_y = line_hal_info[2].start_y; /* 2 line2 */
    hw_node->tde_drd_line2_ed.bits.line2_ed_x = line_hal_info[2].end_x; /* 2 line2 */
    hw_node->tde_drd_line2_ed.bits.line2_ed_y = line_hal_info[2].end_y; /* 2 line2 */

    hw_node->tde_drd_line3_st.bits.line3_st_x = line_hal_info[3].start_x; /* 3 line3 */
    hw_node->tde_drd_line3_st.bits.line3_st_y = line_hal_info[3].start_y; /* 3 line3 */
    hw_node->tde_drd_line3_ed.bits.line3_ed_x = line_hal_info[3].end_x; /* 3 line3 */
    hw_node->tde_drd_line3_ed.bits.line3_ed_y = line_hal_info[3].end_y; /* 3 line3 */
    return;
}
#endif
#if (TDE_CAPABILITY & COMPRESS)

static td_u32 tde_node_max(td_u32 a, td_u32 b)
{
    return (a > b) ? a : b;
}

static td_u32 tde_node_min(td_u32 a, td_u32 b)
{
    return (a > b) ? b : a;
}

#ifdef CONFIG_TDE_GFBG_COMPRESS_V2
static td_u32 tde_node_clip(td_s32 a)
{
    return ((a >= 0) ? a : 0);
}
#endif
#ifdef CONFIG_TDE_GFBG_COMPRESS_V1
static td_void tde_hal_node_set_cmp_from_osd_mode(tde_hw_node *hw_node, td_u32 width)
{
    td_u32 mb_num_x = (width + 31) / 32; /* 31 and 32 is 32 bytes align */
    td_u32 osd_mode = hw_node->tde_od_pic_osd_glb_info.bits.osd_mode;
    td_u32 budget_bits_mb = hw_node->tde_od_pic_osd_rc_cfg0.bits.budget_bits_mb;
    td_u32 adj_sad_bits_thr = (width <= 320) ? 0 : ((width <= 720) ?  8 : 30); /* 320 720 8 30 alg data */
    td_u32 qp_inc1_bits_thr = (width <= 320) ? 0 : ((width <= 720) ? 20 : /* 320 720 20 alg data */
                               tde_node_max(20, tde_node_min(40, mb_num_x - 3))); /* 20 40 3 alg data */
    /* 720 150 80 120 200 3 alg data */
    td_u32 qp_dec1_bits_thr = (width <= 720) ? 150 : tde_node_max(80, tde_node_min(120, 200 - 3 * mb_num_x));
    /* 720 200 140 180 350 4 alg data */
    td_u32 qp_dec2_bits_thr = (width <= 720) ? 200 : tde_node_max(140, tde_node_min(180, 350 - 4 * mb_num_x));
    /* 720 5 8 3 4 alg data */
    td_u32 min_mb_bits = (width <= 720) ? (budget_bits_mb * 5 / 8) : (budget_bits_mb * 3 / 4);

    if ((hw_node->tde_od_pic_osd_glb_info.bits.osd_mode == 0) ||
        (hw_node->tde_od_pic_osd_glb_info.bits.osd_mode == 1)) {
        hw_node->tde_od_pic_osd_rc_cfg7.bits.adj_sad_bits_thr = adj_sad_bits_thr;
        hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_inc1_bits_thr = qp_inc1_bits_thr;
        hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec1_bits_thr = qp_dec1_bits_thr;
        hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec2_bits_thr = qp_dec2_bits_thr;
        hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec3_bits_thr = 255; /* 255 alg data */
        hw_node->tde_od_pic_osd_rc_cfg2.bits.max_qp = (osd_mode == 0) ? 5 : 4; /* 5 4 alg data */
        hw_node->tde_od_pic_osd_rc_cfg2.bits.rc_smth_ngain = (width < 1280) ? 3 : 4; /* 1280 3 4 alg data */
        hw_node->tde_od_pic_osd_rc_cfg2.bits.special_bits_gain = (width <= 720) ? 0 : 4; /* 720 4 alg data */
        hw_node->tde_od_pic_osd_rc_cfg7.bits.max_trow_bits = (width <= 720) ? 0 : 10; /* 720 10 alg data */
        hw_node->tde_od_pic_osd_rc_cfg0.bits.min_mb_bits = min_mb_bits;
    } else {
        hw_node->tde_od_pic_osd_rc_cfg7.bits.adj_sad_bits_thr = 10; /* 10 alg data */
        hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_inc1_bits_thr = 20; /* 20 alg data */
        hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec1_bits_thr = 60; /* 60 alg data */
        hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec2_bits_thr = 100; /* 100 alg data */
        hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec3_bits_thr = 130; /* 130 alg data */
        hw_node->tde_od_pic_osd_rc_cfg2.bits.max_qp = 1;
        hw_node->tde_od_pic_osd_rc_cfg2.bits.rc_smth_ngain = 3; /* 3 alg data */
        hw_node->tde_od_pic_osd_rc_cfg2.bits.special_bits_gain = 2; /* 2 alg data */
        hw_node->tde_od_pic_osd_rc_cfg7.bits.max_trow_bits = 10; /* 10 alg data */
        hw_node->tde_od_pic_osd_rc_cfg0.bits.min_mb_bits = (budget_bits_mb * 3) / 4; /* 3 4 alg data */
    }
}

static td_void tde_hal_node_set_cmp(tde_hw_node *hw_node, td_u32 width)
{
    td_u32 bit_dep_a;
    td_u32 bit_dep_rgb;
    td_u32 mb_org_bits;
    td_u32 mb_num_x;
    td_u32 ctl_delta;
    td_u32 osd_mode;
    td_u32 budget_bits_mb;
    td_u32 comp_ratio;
    osd_mode = hw_node->tde_od_pic_osd_glb_info.bits.osd_mode;
    if (osd_mode == 1) {
        comp_ratio = 2000; /* 2000 alg data */
    } else {
        comp_ratio = 1000; /* 1000 alg data */
    }
    mb_num_x = (width + 31) / 32; /* 31 32 alg data */
    /* 8 6 2 4 alg data */
    bit_dep_a = (osd_mode == 0) ? 8 : ((osd_mode == 1) ? 6 : ((osd_mode == 2) ? 1 : 4));
    /* 8 2 5 4 alg data */
    bit_dep_rgb = (osd_mode == 0) ? 8 : ((osd_mode == 1) ? 8 : ((osd_mode == 2) ? 5 : 4));
    mb_org_bits = 32 * (bit_dep_a + bit_dep_rgb * 3); /* 32 3 alg data */
    ctl_delta = (width <= 720) ? ((20 * 16 + 500) / mb_num_x) :  /* 720 20 16 500 alg data */
                ((((40 * 16 + 1000) / mb_num_x) > 20) ? ((40 * 16 + 1000) / mb_num_x) : 20); /* 40 16 1000 20 alg */

    ctl_delta = ((width % 32) != 0) ? (ctl_delta + 8) : ctl_delta; /* 32 8 alg data */

    if ((osd_mode != 0) && (osd_mode != 1)) {
        ctl_delta = 20; /* 20 alg data */
    }
    budget_bits_mb = ((mb_org_bits * 1000 / comp_ratio - ctl_delta) > 256) ? /* 1000 256 alg data */
                      (mb_org_bits * 1000 / comp_ratio - ctl_delta) : 256; /* 1000 256 alg data */
    budget_bits_mb = (budget_bits_mb > 1023) ?  1023 :  budget_bits_mb; /* 1023 alg data */
    hw_node->tde_od_pic_osd_rc_cfg0.bits.budget_bits_mb         = budget_bits_mb;
    hw_node->tde_od_pic_osd_rc_cfg1.bits.budget_bits_mb_cap     = 1023; /* 1023 alg data */
    hw_node->tde_od_pic_osd_rc_cfg6.bits.pix_diff_thr           = 50; /* 50 alg data */
    hw_node->tde_od_pic_osd_rc_cfg4.bits.smth_thr               = 3; /* 3 alg data */
    hw_node->tde_od_pic_osd_rc_cfg4.bits.still_thr              = 1;
    hw_node->tde_od_pic_osd_rc_cfg4.bits.big_grad_thr           = 16; /* 16 alg data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.smth_pix_num_thr       = 6; /* 6 alg data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.still_pix_num_thr      = 3; /* 3 alg data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.large_smth_pix_num_thr = 10; /* 10 alg data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.noise_pix_num_thr      = 24; /* 24 alg data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.smth_qp                = 2; /* 2 alg data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.sad_bits_ngain         = 10; /* 10 alg data */
    hw_node->tde_od_pic_osd_rc_cfg3.bits.min_sad_thr            = 12; /* 12 alg data */
    hw_node->tde_od_pic_osd_rc_cfg3.bits.max_sad_thr            = 64; /* 64 alg data */
    hw_node->tde_od_pic_osd_rc_cfg6.bits.noise_sad              = 32; /* 32 alg data */
    hw_node->tde_od_pic_osd_rc_cfg9.bits.force_qp_thr           = 70; /* 70 alg data */
    hw_node->tde_od_pic_osd_rc_cfg9.bits.force_qp_thr_cap       = 20; /* 20 alg data */
    tde_hal_node_set_cmp_from_osd_mode(hw_node, width);
}

#ifndef CONFIG_COMPRESS_ECONOMIZE_MEMERY
static td_void tde_hal_node_set_argb8888_320(tde_hw_node *hw_node)
{
    hw_node->tde_od_pic_osd_rc_cfg0.bits.budget_bits_mb = 994; /* 994 budget_bits_mb data */
    hw_node->tde_od_pic_osd_rc_cfg0.bits.min_mb_bits = 750; /* 750 min_mb_bits data */

    hw_node->tde_od_pic_osd_rc_cfg1.bits.budget_bits_mb_cap = 512; /* 512 budget_bits_mb_cap data */
    hw_node->tde_od_pic_osd_rc_cfg7.bits.max_trow_bits = 0;

    hw_node->tde_od_pic_osd_rc_cfg2.bits.max_qp = 5; /* 5 max_qp data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.smth_qp = 2; /* 2 smth_qp data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.sad_bits_ngain = 10; /* 10 sad_bits_ngain data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.rc_smth_ngain = 4; /* 4 rc_smth_ngain data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.special_bits_gain = 0;

    hw_node->tde_od_pic_osd_rc_cfg3.bits.max_sad_thr = 64; /* 64 max_sad_thr data */
    hw_node->tde_od_pic_osd_rc_cfg3.bits.min_sad_thr = 12; /* 12 min_sad_thr data */

    hw_node->tde_od_pic_osd_rc_cfg4.bits.smth_thr = 3; /* 3 smth_thr data */
    hw_node->tde_od_pic_osd_rc_cfg4.bits.still_thr = 1;
    hw_node->tde_od_pic_osd_rc_cfg4.bits.big_grad_thr = 16; /* 16 big_grad_thr data */

    hw_node->tde_od_pic_osd_rc_cfg5.bits.smth_pix_num_thr = 6; /* 6 smth_pix_num_thr data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.still_pix_num_thr = 3; /* 3 still_pix_num_thr data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.noise_pix_num_thr = 24; /* 24 noise_pix_num_thr data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.large_smth_pix_num_thr = 10; /* 10 large_smth_pix_num_thr data */

    hw_node->tde_od_pic_osd_rc_cfg6.bits.noise_sad = 32; /* 32 noise_sad data */
    hw_node->tde_od_pic_osd_rc_cfg6.bits.pix_diff_thr = 50; /* 50 pix_diff_thr data */

    hw_node->tde_od_pic_osd_rc_cfg7.bits.adj_sad_bits_thr = 0;

    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_inc1_bits_thr = 0;
    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec1_bits_thr = 60; /* 60 qp_dec1_bits_thr data */
    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec2_bits_thr = 130; /* 130 qp_dec2_bits_thr data */
    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec3_bits_thr = 250; /* 250 qp_dec3_bits_thr data */

    hw_node->tde_od_pic_osd_rc_cfg9.bits.force_qp_thr = 70; /* 70 force_qp_thr data */
    hw_node->tde_od_pic_osd_rc_cfg9.bits.force_qp_thr_cap = 20; /* 20 force_qp_thr_cap data */
}

static td_void tde_hal_node_set_argb8888_720(tde_hw_node *hw_node)
{
    hw_node->tde_od_pic_osd_rc_cfg0.bits.budget_bits_mb = 492; /* 492 budget_bits_mb data */
    hw_node->tde_od_pic_osd_rc_cfg0.bits.min_mb_bits = 375; /* 375 min_mb_bits data */

    hw_node->tde_od_pic_osd_rc_cfg1.bits.budget_bits_mb_cap = 512; /* 512 budget_bits_mb_cap data */
    hw_node->tde_od_pic_osd_rc_cfg7.bits.max_trow_bits = 0;

    hw_node->tde_od_pic_osd_rc_cfg2.bits.max_qp = 5; /* 5 max_qp data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.smth_qp = 2; /* 2 smth_qp data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.sad_bits_ngain = 10; /* 10 sad_bits_ngain data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.rc_smth_ngain = 3; /* 3 rc_smth_ngain data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.special_bits_gain = 0;

    hw_node->tde_od_pic_osd_rc_cfg3.bits.max_sad_thr = 64; /* 64 max_sad_thr data */
    hw_node->tde_od_pic_osd_rc_cfg3.bits.min_sad_thr = 12; /* 12 min_sad_thr data */

    hw_node->tde_od_pic_osd_rc_cfg4.bits.smth_thr = 3; /* 3 smth_thr data */
    hw_node->tde_od_pic_osd_rc_cfg4.bits.still_thr = 1;
    hw_node->tde_od_pic_osd_rc_cfg4.bits.big_grad_thr = 16; /* 16 big_grad_thr data */

    hw_node->tde_od_pic_osd_rc_cfg5.bits.smth_pix_num_thr = 6; /* 6 smth_pix_num_thr data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.still_pix_num_thr = 3; /* 3 still_pix_num_thr data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.noise_pix_num_thr = 24; /* 24 noise_pix_num_thr data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.large_smth_pix_num_thr = 10; /* 10 large_smth_pix_num_thr data */

    hw_node->tde_od_pic_osd_rc_cfg6.bits.noise_sad = 32; /* 32 noise_sad data */
    hw_node->tde_od_pic_osd_rc_cfg6.bits.pix_diff_thr = 50; /* 50 pix_diff_thr data */

    hw_node->tde_od_pic_osd_rc_cfg7.bits.adj_sad_bits_thr = 8; /* 8 adj_sad_bits_thr data */

    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_inc1_bits_thr = 20; /* 20 qp_inc1_bits_thr data */
    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec1_bits_thr = 60; /* 60 qp_dec1_bits_thr data */
    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec2_bits_thr = 130; /* 130 qp_dec2_bits_thr data */
    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec3_bits_thr = 250; /* 250 qp_dec3_bits_thr data */

    hw_node->tde_od_pic_osd_rc_cfg9.bits.force_qp_thr = 70; /* 70 force_qp_thr data */
    hw_node->tde_od_pic_osd_rc_cfg9.bits.force_qp_thr_cap = 20; /* 20 force_qp_thr_cap data */
}

static td_void tde_hal_node_set_argb8888_3840(tde_hw_node *hw_node)
{
    hw_node->tde_od_pic_osd_rc_cfg0.bits.budget_bits_mb = 492; /* 492 budget_bits_mb data */
    hw_node->tde_od_pic_osd_rc_cfg0.bits.min_mb_bits = 375; /* 375 min_mb_bits data */

    hw_node->tde_od_pic_osd_rc_cfg1.bits.budget_bits_mb_cap = 512; /* 512 budget_bits_mb_cap data */
    hw_node->tde_od_pic_osd_rc_cfg7.bits.max_trow_bits = 10; /* 10 max_trow_bits data */

    hw_node->tde_od_pic_osd_rc_cfg2.bits.max_qp = 5; /* 5 max_qp data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.smth_qp = 2; /* 2 smth_qp data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.sad_bits_ngain = 10; /* 10 sad_bits_ngain data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.rc_smth_ngain = 4; /* 4 rc_smth_ngain data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.special_bits_gain = 4; /* 4 special_bits_gain data */

    hw_node->tde_od_pic_osd_rc_cfg3.bits.max_sad_thr = 64; /* 64 max_sad_thr data */
    hw_node->tde_od_pic_osd_rc_cfg3.bits.min_sad_thr = 12; /* 12 min_sad_thr data */

    hw_node->tde_od_pic_osd_rc_cfg4.bits.smth_thr = 3; /* 3 smth_thr data */
    hw_node->tde_od_pic_osd_rc_cfg4.bits.still_thr = 1;
    hw_node->tde_od_pic_osd_rc_cfg4.bits.big_grad_thr = 16; /* 16 big_grad_thr data */

    hw_node->tde_od_pic_osd_rc_cfg5.bits.smth_pix_num_thr = 6; /* 6 smth_pix_num_thr data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.still_pix_num_thr = 3; /* 3 still_pix_num_thr data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.noise_pix_num_thr = 24; /* 24 noise_pix_num_thr data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.large_smth_pix_num_thr = 10; /* 10 large_smth_pix_num_thr data */

    hw_node->tde_od_pic_osd_rc_cfg6.bits.noise_sad = 32; /* 32 noise_sad data */
    hw_node->tde_od_pic_osd_rc_cfg6.bits.pix_diff_thr = 50; /* 50 pix_diff_thr data */

    hw_node->tde_od_pic_osd_rc_cfg7.bits.adj_sad_bits_thr = 30; /* 30 adj_sad_bits_thr data */

    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_inc1_bits_thr = 40; /* 40 qp_inc1_bits_thr data */
    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec1_bits_thr = 60; /* 60 qp_dec1_bits_thr data */
    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec2_bits_thr = 100; /* 100 qp_dec2_bits_thr data */
    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec3_bits_thr = 250; /* 250 qp_dec3_bits_thr data */

    hw_node->tde_od_pic_osd_rc_cfg9.bits.force_qp_thr = 70; /* 70 force_qp_thr data */
    hw_node->tde_od_pic_osd_rc_cfg9.bits.force_qp_thr_cap = 20; /* 20 force_qp_thr_cap data */
}
#endif

static td_void tde_hal_node_set_rgb888_320(tde_hw_node *hw_node)
{
    hw_node->tde_od_pic_osd_rc_cfg0.bits.budget_bits_mb = 930; /* 930 budget_bits_mb data */
    hw_node->tde_od_pic_osd_rc_cfg0.bits.min_mb_bits = 702; /* 702 min_mb_bits data */

    hw_node->tde_od_pic_osd_rc_cfg1.bits.budget_bits_mb_cap = 512; /* 512 budget_bits_mb_cap data */
    hw_node->tde_od_pic_osd_rc_cfg7.bits.max_trow_bits = 0;

    hw_node->tde_od_pic_osd_rc_cfg2.bits.max_qp = 4; /* 4 max_qp data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.smth_qp = 2; /* 2 smth_qp data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.sad_bits_ngain = 10; /* 10 sad_bits_ngain data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.rc_smth_ngain = 4; /* 4 rc_smth_ngain data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.special_bits_gain = 0;

    hw_node->tde_od_pic_osd_rc_cfg3.bits.max_sad_thr = 64; /* 64 max_sad_thr data */
    hw_node->tde_od_pic_osd_rc_cfg3.bits.min_sad_thr = 12; /* 12 min_sad_thr data */

    hw_node->tde_od_pic_osd_rc_cfg4.bits.smth_thr = 3; /* 3 smth_thr data */
    hw_node->tde_od_pic_osd_rc_cfg4.bits.still_thr = 1;
    hw_node->tde_od_pic_osd_rc_cfg4.bits.big_grad_thr = 16; /* 16 big_grad_thr data */

    hw_node->tde_od_pic_osd_rc_cfg5.bits.smth_pix_num_thr = 6; /* 6 smth_pix_num_thr data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.still_pix_num_thr = 3; /* 3 still_pix_num_thr data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.noise_pix_num_thr = 24; /* 24 noise_pix_num_thr data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.large_smth_pix_num_thr = 10; /* 10 large_smth_pix_num_thr data */

    hw_node->tde_od_pic_osd_rc_cfg6.bits.noise_sad = 32; /* 32 noise_sad data */
    hw_node->tde_od_pic_osd_rc_cfg6.bits.pix_diff_thr = 50; /* 50 pix_diff_thr data */

    hw_node->tde_od_pic_osd_rc_cfg7.bits.adj_sad_bits_thr = 0;

    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_inc1_bits_thr = 0;
    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec1_bits_thr = 60; /* 60 qp_dec1_bits_thr data */
    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec2_bits_thr = 130; /* 130 qp_dec2_bits_thr data */
    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec3_bits_thr = 250; /* 250 qp_dec3_bits_thr data */

    hw_node->tde_od_pic_osd_rc_cfg9.bits.force_qp_thr = 70; /* 70 force_qp_thr data */
    hw_node->tde_od_pic_osd_rc_cfg9.bits.force_qp_thr_cap = 20; /* 20 force_qp_thr_cap data */
}

static td_void tde_hal_node_set_rgb888_720(tde_hw_node *hw_node)
{
    hw_node->tde_od_pic_osd_rc_cfg0.bits.budget_bits_mb = 500; /* 500 budget_bits_mb data */
    hw_node->tde_od_pic_osd_rc_cfg0.bits.min_mb_bits = 375; /* 375 min_mb_bits data */

    hw_node->tde_od_pic_osd_rc_cfg1.bits.budget_bits_mb_cap = 512; /* 512 budget_bits_mb_cap data */
    hw_node->tde_od_pic_osd_rc_cfg7.bits.max_trow_bits = 0;

    hw_node->tde_od_pic_osd_rc_cfg2.bits.max_qp = 4; /* 4 max_qp data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.smth_qp = 2; /* 2 smth_qp data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.sad_bits_ngain = 10; /* 10 sad_bits_ngain data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.rc_smth_ngain = 3; /* 3 rc_smth_ngain data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.special_bits_gain = 0;

    hw_node->tde_od_pic_osd_rc_cfg3.bits.max_sad_thr = 64; /* 64 max_sad_thr data */
    hw_node->tde_od_pic_osd_rc_cfg3.bits.min_sad_thr = 12; /* 12 min_sad_thr data */

    hw_node->tde_od_pic_osd_rc_cfg4.bits.smth_thr = 3; /* 3 smth_thr data */
    hw_node->tde_od_pic_osd_rc_cfg4.bits.still_thr = 1;
    hw_node->tde_od_pic_osd_rc_cfg4.bits.big_grad_thr = 16; /* 16 big_grad_thr data */

    hw_node->tde_od_pic_osd_rc_cfg5.bits.smth_pix_num_thr = 6; /* 6 smth_pix_num_thr data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.still_pix_num_thr = 3; /* 3 still_pix_num_thr data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.noise_pix_num_thr = 24; /* 24 noise_pix_num_thr data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.large_smth_pix_num_thr = 10; /* 10 large_smth_pix_num_thr data */

    hw_node->tde_od_pic_osd_rc_cfg6.bits.noise_sad = 32; /* 32 noise_sad data */
    hw_node->tde_od_pic_osd_rc_cfg6.bits.pix_diff_thr = 50; /* 50 pix_diff_thr data */

    hw_node->tde_od_pic_osd_rc_cfg7.bits.adj_sad_bits_thr = 8; /* 8 adj_sad_bits_thr data */

    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_inc1_bits_thr = 20; /* 20 qp_inc1_bits_thr data */
    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec1_bits_thr = 60; /* 60 qp_dec1_bits_thr data */
    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec2_bits_thr = 130; /* 130 qp_dec2_bits_thr data */
    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec3_bits_thr = 250; /* 250 qp_dec3_bits_thr data */

    hw_node->tde_od_pic_osd_rc_cfg9.bits.force_qp_thr = 70; /* 70 force_qp_thr data */
    hw_node->tde_od_pic_osd_rc_cfg9.bits.force_qp_thr_cap = 20; /* 20 force_qp_thr_cap data */
}

static td_void tde_hal_node_set_rgb888_3840(tde_hw_node *hw_node)
{
    hw_node->tde_od_pic_osd_rc_cfg0.bits.budget_bits_mb = 500; /* 500 budget_bits_mb data */
    hw_node->tde_od_pic_osd_rc_cfg0.bits.min_mb_bits = 375; /* 375 min_mb_bits data */

    hw_node->tde_od_pic_osd_rc_cfg1.bits.budget_bits_mb_cap = 512; /* 512 budget_bits_mb_cap data */
    hw_node->tde_od_pic_osd_rc_cfg7.bits.max_trow_bits = 10; /* 10 max_trow_bits data */

    hw_node->tde_od_pic_osd_rc_cfg2.bits.max_qp = 4; /* 4 max_qp data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.smth_qp = 2; /* 2 smth_qp data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.sad_bits_ngain = 10; /* 10 sad_bits_ngain data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.rc_smth_ngain = 4; /* 4 rc_smth_ngain data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.special_bits_gain = 4; /* 4 special_bits_gain data */

    hw_node->tde_od_pic_osd_rc_cfg3.bits.max_sad_thr = 64; /* 64 max_sad_thr data */
    hw_node->tde_od_pic_osd_rc_cfg3.bits.min_sad_thr = 12; /* 12 min_sad_thr data */

    hw_node->tde_od_pic_osd_rc_cfg4.bits.smth_thr = 3; /* 3 smth_thr data */
    hw_node->tde_od_pic_osd_rc_cfg4.bits.still_thr = 1;
    hw_node->tde_od_pic_osd_rc_cfg4.bits.big_grad_thr = 16; /* 16 big_grad_thr data */

    hw_node->tde_od_pic_osd_rc_cfg5.bits.smth_pix_num_thr = 6; /* 6 smth_pix_num_thr data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.still_pix_num_thr = 3; /* 3 still_pix_num_thr data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.noise_pix_num_thr = 24; /* 24 noise_pix_num_thr data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.large_smth_pix_num_thr = 10; /* 10 large_smth_pix_num_thr data */

    hw_node->tde_od_pic_osd_rc_cfg6.bits.noise_sad = 32; /* 32 noise_sad data */
    hw_node->tde_od_pic_osd_rc_cfg6.bits.pix_diff_thr = 50; /* 50 pix_diff_thr data */

    hw_node->tde_od_pic_osd_rc_cfg7.bits.adj_sad_bits_thr = 30; /* 30 adj_sad_bits_thr data */

    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_inc1_bits_thr = 40; /* 40 qp_inc1_bits_thr data */
    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec1_bits_thr = 60; /* 60 qp_dec1_bits_thr data */
    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec2_bits_thr = 100; /* 100 qp_dec2_bits_thr data */
    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec3_bits_thr = 250; /* 250 qp_dec3_bits_thr data */

    hw_node->tde_od_pic_osd_rc_cfg9.bits.force_qp_thr = 70; /* 70 force_qp_thr data */
    hw_node->tde_od_pic_osd_rc_cfg9.bits.force_qp_thr_cap = 20; /* 20 force_qp_thr_cap data */
}

static td_void tde_hal_node_set_argb15555_or_argb4444_720(tde_hw_node *hw_node)
{
    hw_node->tde_od_pic_osd_rc_cfg0.bits.budget_bits_mb = 492; /* 492 budget_bits_mb data */
    hw_node->tde_od_pic_osd_rc_cfg0.bits.min_mb_bits = 375; /* 375 min_mb_bits data */

    hw_node->tde_od_pic_osd_rc_cfg1.bits.budget_bits_mb_cap = 512; /* 512 budget_bits_mb_cap data */
    hw_node->tde_od_pic_osd_rc_cfg7.bits.max_trow_bits = 10; /* 10 max_trow_bits data */

    hw_node->tde_od_pic_osd_rc_cfg2.bits.max_qp = 1;
    hw_node->tde_od_pic_osd_rc_cfg2.bits.smth_qp = 2; /* 2 smth_qp data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.sad_bits_ngain = 10; /* 10 sad_bits_ngain data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.rc_smth_ngain = 3; /* 3 rc_smth_ngain data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.special_bits_gain = 2; /* 2 special_bits_gain data */

    hw_node->tde_od_pic_osd_rc_cfg3.bits.max_sad_thr = 64; /* 64 max_sad_thr data */
    hw_node->tde_od_pic_osd_rc_cfg3.bits.min_sad_thr = 12; /* 12 min_sad_thr data */

    hw_node->tde_od_pic_osd_rc_cfg4.bits.smth_thr = 3; /* 3 smth_thr data */
    hw_node->tde_od_pic_osd_rc_cfg4.bits.still_thr = 1;
    hw_node->tde_od_pic_osd_rc_cfg4.bits.big_grad_thr = 16; /* 16 big_grad_thr data */

    hw_node->tde_od_pic_osd_rc_cfg5.bits.smth_pix_num_thr = 6; /* 6 smth_pix_num_thr data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.still_pix_num_thr = 3; /* 3 still_pix_num_thr data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.noise_pix_num_thr = 24; /* 24 noise_pix_num_thr data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.large_smth_pix_num_thr = 10; /* 10 large_smth_pix_num_thr data */

    hw_node->tde_od_pic_osd_rc_cfg6.bits.noise_sad = 32; /* 32 noise_sad data */
    hw_node->tde_od_pic_osd_rc_cfg6.bits.pix_diff_thr = 50; /* 50 pix_diff_thr data */

    hw_node->tde_od_pic_osd_rc_cfg7.bits.adj_sad_bits_thr = 10; /* 10 adj_sad_bits_thr data */

    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_inc1_bits_thr = 20; /* 20 qp_inc1_bits_thr data */
    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec1_bits_thr = 60; /* 60 qp_dec1_bits_thr data */
    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec2_bits_thr = 100; /* 100 qp_dec2_bits_thr data */
    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec3_bits_thr = 130; /* 130 qp_dec3_bits_thr data */

    hw_node->tde_od_pic_osd_rc_cfg9.bits.force_qp_thr = 70; /* 70 force_qp_thr data */
    hw_node->tde_od_pic_osd_rc_cfg9.bits.force_qp_thr_cap = 20; /* 20 force_qp_thr_cap data */
}

static td_void tde_hal_node_set_argb15555_or_argb4444_3840(tde_hw_node *hw_node)
{
    hw_node->tde_od_pic_osd_rc_cfg0.bits.budget_bits_mb = 492; /* 492 budget_bits_mb data */
    hw_node->tde_od_pic_osd_rc_cfg0.bits.min_mb_bits = 375; /* 375 min_mb_bits data */

    hw_node->tde_od_pic_osd_rc_cfg1.bits.budget_bits_mb_cap = 512; /* 512 budget_bits_mb_cap data */
    hw_node->tde_od_pic_osd_rc_cfg7.bits.max_trow_bits = 10; /* 10 max_trow_bits data */

    hw_node->tde_od_pic_osd_rc_cfg2.bits.max_qp = 1;
    hw_node->tde_od_pic_osd_rc_cfg2.bits.smth_qp = 2; /* 2 smth_qp data */

    hw_node->tde_od_pic_osd_rc_cfg2.bits.sad_bits_ngain = 10; /* 10 sad_bits_ngain data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.rc_smth_ngain = 3; /* 3 rc_smth_ngain data */
    hw_node->tde_od_pic_osd_rc_cfg2.bits.special_bits_gain = 2; /* 2 special_bits_gain data */

    hw_node->tde_od_pic_osd_rc_cfg3.bits.max_sad_thr = 64; /* 64 max_sad_thr data */
    hw_node->tde_od_pic_osd_rc_cfg3.bits.min_sad_thr = 12; /* 12 min_sad_thr data */

    hw_node->tde_od_pic_osd_rc_cfg4.bits.smth_thr = 3; /* 3 smth_thr data */
    hw_node->tde_od_pic_osd_rc_cfg4.bits.still_thr = 1;
    hw_node->tde_od_pic_osd_rc_cfg4.bits.big_grad_thr = 16; /* 16 big_grad_thr data */

    hw_node->tde_od_pic_osd_rc_cfg5.bits.smth_pix_num_thr = 6; /* 6 smth_pix_num_thr data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.still_pix_num_thr = 3; /* 3 still_pix_num_thr data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.noise_pix_num_thr = 24; /* 24 noise_pix_num_thr data */
    hw_node->tde_od_pic_osd_rc_cfg5.bits.large_smth_pix_num_thr = 10; /* 10 large_smth_pix_num_thr data */

    hw_node->tde_od_pic_osd_rc_cfg6.bits.noise_sad = 32; /* 32 noise_sad data */
    hw_node->tde_od_pic_osd_rc_cfg6.bits.pix_diff_thr = 50; /* 50 pix_diff_thr data */

    hw_node->tde_od_pic_osd_rc_cfg7.bits.adj_sad_bits_thr = 10; /* 10 adj_sad_bits_thr data */

    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_inc1_bits_thr = 20; /* 20 qp_inc1_bits_thr data */
    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec1_bits_thr = 60; /* 60 qp_dec1_bits_thr data */
    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec2_bits_thr = 100; /* 100 qp_dec2_bits_thr data */
    hw_node->tde_od_pic_osd_rc_cfg8.bits.qp_dec3_bits_thr = 130; /* 130 qp_dec3_bits_thr data */

    hw_node->tde_od_pic_osd_rc_cfg9.bits.force_qp_thr = 70; /* 70 force_qp_thr data */
    hw_node->tde_od_pic_osd_rc_cfg9.bits.force_qp_thr_cap = 20; /* 20 force_qp_thr_cap data */
}

td_void tde_hal_node_set_compress(tde_hw_node *hw_node)
{
    if (hw_node == TD_NULL) {
        tde_error("HWNode is null !\n");
        return;
    }

    hw_node->tde_od_pic_osd_glb_info.bits.is_lossless = 0;
    hw_node->tde_od_pic_osd_glb_info.bits.is_lossless_alpha = hw_node->tde_od_pic_osd_glb_info.bits.is_lossless;
    hw_node->tde_od_pic_osd_glb_info.bits.cmp_mode = 0;
    hw_node->tde_od_pic_osd_glb_info.bits.osd_mode =
        (hw_node->des_ctrl.bits.des_fmt == TDE_DRV_COLOR_FMT_ARGB8888) ? 0 :
        ((hw_node->des_ctrl.bits.des_fmt == TDE_DRV_COLOR_FMT_RGB888) ? 1 :
        ((hw_node->des_ctrl.bits.des_fmt == TDE_DRV_COLOR_FMT_ARGB1555) ? 2 : 3)); /* 2 3 des_fmt value */
    hw_node->tde_od_pic_osd_glb_info.bits.partition_en = 0;
    hw_node->tde_od_pic_osd_glb_info.bits.part_num = 1;

    hw_node->tde_od_pic_osd_frame_size.bits.frame_width = hw_node->des_imgsize.bits.des_width;
    hw_node->tde_od_pic_osd_frame_size.bits.frame_height = hw_node->des_imgsize.bits.des_height;

    if (hw_node->des_ctrl.bits.des_fmt == TDE_DRV_COLOR_FMT_ARGB8888) {
#ifndef CONFIG_COMPRESS_ECONOMIZE_MEMERY
        if (hw_node->des_imgsize.bits.des_width <= 320) { /* 320 set argb8888 */
            tde_hal_node_set_argb8888_320(hw_node);
        } else if (hw_node->des_imgsize.bits.des_width <= 720) { /* 720 set argb8888 */
            tde_hal_node_set_argb8888_720(hw_node);
        } else if (hw_node->des_imgsize.bits.des_width <= 3840) { /* 3840 set argb8888 */
            tde_hal_node_set_argb8888_3840(hw_node);
        }
#else
        tde_hal_node_set_cmp(hw_node, hw_node->des_imgsize.bits.des_width);
#endif
    } else if (hw_node->des_ctrl.bits.des_fmt == TDE_DRV_COLOR_FMT_RGB888) {
        if (hw_node->des_imgsize.bits.des_width <= 320) { /* 320 set rgb888 */
            tde_hal_node_set_rgb888_320(hw_node);
        } else if (hw_node->des_imgsize.bits.des_width <= 720) { /* 720 set rgb888 */
            tde_hal_node_set_rgb888_720(hw_node);
        } else if (hw_node->des_imgsize.bits.des_width <= 3840) { /* 3840 set rgb888 */
            tde_hal_node_set_rgb888_3840(hw_node);
        }
    } else if ((hw_node->des_ctrl.bits.des_fmt == TDE_DRV_COLOR_FMT_ARGB1555) ||
               (hw_node->des_ctrl.bits.des_fmt == TDE_DRV_COLOR_FMT_ARGB4444)) {
        if (hw_node->des_imgsize.bits.des_width <= 720) { /* 720 set argb15555 or argb4444 */
            tde_hal_node_set_argb15555_or_argb4444_720(hw_node);
        } else if (hw_node->des_imgsize.bits.des_width <= 3840) { /* 3840 set argb15555 or argb4444 */
            tde_hal_node_set_argb15555_or_argb4444_3840(hw_node);
        }
    }
}
#endif /* CONFIG_TDE_GFBG_COMPRESS_V1 */
#ifdef CONFIG_TDE_GFBG_COMPRESS_V2
static td_void tde_hal_node_set_cmp_rc_base(tde_hw_node *hw_node)
{
    hw_node->tde_line_osd_cmp_rc_cfg1.bits.smth_thr = 8; /* 8 alg data */
    hw_node->tde_line_osd_cmp_rc_cfg1.bits.still_thr = 1;
    hw_node->tde_line_osd_cmp_rc_cfg1.bits.big_grad_thr = 45; /* 45 alg data */
    hw_node->tde_line_osd_cmp_rc_cfg1.bits.diff_thr = 30; /* 30 alg data */
    hw_node->tde_line_osd_cmp_rc_cfg2.bits.smth_pix_num_thr = 6; /* 6 alg data */
    hw_node->tde_line_osd_cmp_rc_cfg2.bits.still_pix_num_thr = 8; /* 8 alg data */
    hw_node->tde_line_osd_cmp_rc_cfg2.bits.noise_pix_num_thr = 30; /* 30 alg data */
    hw_node->tde_line_osd_cmp_rc_cfg2.bits.raw_bits_penalty = 10; /* 10 alg data */
}

static td_void tde_hal_node_set_cmp_cfg(tde_hw_node *hw_node, td_u32 max_mb_qp, td_u32 bit_depth_rgb)
{
    td_u32 qp_rge_reg0;
    td_u32 qp_rge_reg1;
    td_u32 qp_rge_reg2;

    tde_hal_node_set_cmp_rc_base(hw_node);

    if (max_mb_qp > (bit_depth_rgb / 2)) { /* 2 alg data */
        qp_rge_reg0 = (tde_node_clip(max_mb_qp - 4) << 28) | (0 << 24) | /* 4 28 24 alg data */
                      (tde_node_clip(max_mb_qp - 5) << 20) | (0 << 16) | /* 5 20 16 alg data */
                      (tde_node_clip(max_mb_qp - 6) << 12) | (0 << 8) | /* 6 12 8 alg data */
                      (tde_node_clip(max_mb_qp - 7) << 4) | 0; /* 7 4 alg data */
        qp_rge_reg1 = (tde_node_clip(max_mb_qp - 3) << 28) | (0 << 24) | /* 3 28 24 alg data */
                      (tde_node_clip(max_mb_qp - 4) << 20) | (0 << 16) | /* 4 20 16 alg data */
                      (tde_node_clip(max_mb_qp - 4) << 12) | (0 << 8) | /* 4 12 8 alg data */
                      (tde_node_clip(max_mb_qp - 4) << 4) | 0; /* 4 alg data */
        /* 28 4 24 20 16 alg data */
        qp_rge_reg2 = (max_mb_qp << 28) | (4 << 24) | (tde_node_clip(max_mb_qp - 1) << 20) | (4 << 16) |
            /* 2 12 8 4 alg data */
            (tde_node_clip(max_mb_qp - 2) << 12) | (2 << 8) | (tde_node_clip(max_mb_qp - 2) << 4) | 0;
    } else {
        qp_rge_reg0 = (tde_node_clip(max_mb_qp - 3) << 28) | (0 << 24) | /* 3 28 24 alg data */
                      (tde_node_clip(max_mb_qp - 3) << 20) | (0 << 16) | /* 3 20 16 alg data */
                      (tde_node_clip(max_mb_qp - 3) << 12) | (0 << 8) | /* 3 12 8 alg data */
                      (tde_node_clip(max_mb_qp - 3) << 4) | 0; /* 3 4 alg data */
        qp_rge_reg1 = (tde_node_clip(max_mb_qp - 2) << 28) | (1 << 24) | /* 2 28 24 alg data */
                      (tde_node_clip(max_mb_qp - 2) << 20) | (1 << 16) | /* 2 20 16 alg data */
                      (tde_node_clip(max_mb_qp - 2) << 12) | (1 << 8) | /* 2 12 8 alg data */
                      (tde_node_clip(max_mb_qp - 3) << 4) | 1; /* 3 4 alg data */
        qp_rge_reg2 = (max_mb_qp << 28) | (2 << 24) | (tde_node_clip(max_mb_qp - 1) << 20) | /* 28 2 24 20 alg data */
                      (2 << 16) | (tde_node_clip(max_mb_qp - 1) << 12) | (2 << 8) | /* 2 16 12 2 8 alg data */
                      (tde_node_clip(max_mb_qp - 1) << 4) | 2; /* 4 2 alg data */
    }

    hw_node->tde_line_osd_cmp_rc_cfg8.bits.qp_rge_reg0 = qp_rge_reg0;
    hw_node->tde_line_osd_cmp_rc_cfg9.bits.qp_rge_reg1 = qp_rge_reg1;
    hw_node->tde_line_osd_cmp_rc_cfg10.bits.qp_rge_reg2 = qp_rge_reg2;

    /* 71 24 56 16 35 8 18 alg data */
    hw_node->tde_line_osd_cmp_rc_cfg5.bits.buf_fullness_thr_reg0 = (71 << 24) | (56 << 16) | (35 << 8) | 18;
    /* 116 24 108 16 98 8 85 alg data */
    hw_node->tde_line_osd_cmp_rc_cfg6.bits.buf_fullness_thr_reg1 = (116 << 24) | (108 << 16) | (98 <<  8) | 85;
    /* 24 123 16 121 8 119 alg data */
    hw_node->tde_line_osd_cmp_rc_cfg7.bits.buf_fullness_thr_reg2 = (0 << 24) | (123 << 16) | (121 << 8) | 119;
    /* 0xff 24 16 alg data */
    hw_node->tde_line_osd_cmp_rc_cfg11.bits.bits_offset_reg0 = ((1 & 0xff) << 24) | ((1 & 0xff) << 16) |
                                                                ((1 & 0xff) << 8) | (1 & 0xff); /* 0xff 8 alg data */
    /* 0xff 24 16 alg data */
    hw_node->tde_line_osd_cmp_rc_cfg12.bits.bits_offset_reg1 = ((0 & 0xff) << 24) | ((0 & 0xff) << 16) |
                                                                ((0 & 0xff) << 8) | (0 & 0xff); /* 0xff 8 alg data */
    /* -4 0xff 24 -3 16 alg data */
    hw_node->tde_line_osd_cmp_rc_cfg13.bits.bits_offset_reg2 = ((-4 & 0xff) << 24) | ((-3 & 0xff) << 16) |
        ((-2 & 0xff) << 8) | (-1 & 0xff); /* -2 0xff 8 -1 alg data */
    /* 7 28 24 6 20 5 16 alg data */
    hw_node->tde_line_osd_cmp_rc_cfg14.bits.est_err_gain_map = (7 << 28) | (7 << 24) | (6 << 20) | (5 << 16) |
        (4 << 12) | (4 << 8) | (3 << 4) | 3; /* 4 12 8 3 alg data */
}

static td_void tde_hal_node_set_cmp_qp_thr(tde_hw_node *hw_node, td_u32 osd_mode)
{
    /* 96 32 16 alg data */
    hw_node->tde_line_osd_cmp_rc_cfg3.bits.qp_inc1_bits_thr = (unsigned int)(96 * (32 * ((osd_mode <= 1) ? 32 : 16)) /
        (32 * 8 * 3)); /* 32 8 3 alg data */
    /* 96 32 16 alg data */
    hw_node->tde_line_osd_cmp_rc_cfg3.bits.qp_inc2_bits_thr = (unsigned int)(96 * (32 * ((osd_mode <= 1) ? 32 : 16)) /
        (32 * 8 * 3)); /* 32 8 3 alg data */
    /* 30 32 16 alg data */
    hw_node->tde_line_osd_cmp_rc_cfg4.bits.qp_dec1_bits_thr = (unsigned int)(30 * (32 * ((osd_mode <= 1) ? 32 : 16)) /
        (32 * 8 * 3)); /* 32 8 3 alg data */
    /* 62 32 16 alg data */
    hw_node->tde_line_osd_cmp_rc_cfg4.bits.qp_dec2_bits_thr = (unsigned int)(62 * (32 * ((osd_mode <= 1) ? 32 : 16)) /
        (32 * 8 * 3)); /* 32 8 3 alg data */
}

static td_void tde_hal_node_set_cmp_status(tde_hw_node *hw_node, td_u32 budget_mb_bits)
{
    hw_node->tde_line_osd_cmp_rc_cfg15.bits.smooth_status_thr = 9; /* 9 alg data */
    hw_node->tde_line_osd_cmp_rc_cfg15.bits.min_mb_bits = budget_mb_bits * 3 / 4; /* 3 4 alg data */
    /* 255 2 alg data */
    hw_node->v4r2_line_osd_cmp_rc_cfg16.bits.first_mb_adj_bits = tde_node_min(255, budget_mb_bits >> 2);
    /* 255 4 alg data */
    hw_node->v4r2_line_osd_cmp_rc_cfg16.bits.first_row_adj_bits = tde_node_min(255, budget_mb_bits >> 4);
    /* 255 4 alg data */
    hw_node->v4r2_line_osd_cmp_rc_cfg16.bits.first_col_adj_bits = tde_node_min(255, budget_mb_bits >> 4);
    /* 6 alg data */
    hw_node->tde_line_osd_cmp_rc_cfg17.bits.still_status_thr = 6;
    /* 64 alg data */
    hw_node->tde_line_osd_cmp_rc_cfg17.bits.still_diff_thr = 64;
}

static td_void tde_hal_node_set_cmp(tde_hw_node *hw_node, td_u32 width)
{
    td_u32 osd_mode;
    td_u32 mb_ori_bits;
    td_u32 budget_mb_bits;
    td_u32 bit_depth_rgb;
    td_u32 bit_depth_a;
    td_u32 delta;
    td_u32 buffer_init_bits;
    td_u32 mb_num_x;
    td_u32 last_mb_width;
    td_u32 max_mb_qp;
    const td_u32 max_head_bits_raw = 1;
    td_u32 comp_ratio;
    osd_mode = hw_node->tde_line_osd_cmp_glb_info.bits.osd_mode;
    if (osd_mode == 1) {
        comp_ratio = 2000; /* 2000 alg data */
    } else {
        comp_ratio = 1000; /* 1000 alg data */
    }

    /* for calculate budget_mb_bits */
    bit_depth_a = (osd_mode <= 1) ? 8 : ((osd_mode == 2) ? 1 : 4); /* 8 2 4 alg data */
    bit_depth_rgb = (osd_mode <= 1) ? 8 : ((osd_mode == 2) ? 5 : 4); /* 8 2 5 4 alg data */
    mb_num_x = (width + 32 - 1) / 32; /* 32 alg data */
    /* 2 9 7946 7000 18 8082 alg data */
    buffer_init_bits = (osd_mode < 2) ? ((mb_num_x < 9) ? 7946 : 7000) : ((mb_num_x < 18) ? 8082 : 7000);
    delta = tde_node_max(1, (9216 - buffer_init_bits + mb_num_x - 1) / (mb_num_x)); /* 9216 alg data */
    mb_ori_bits = 32 * (bit_depth_rgb * 3 + bit_depth_a); /* 32 3 alg data */
    budget_mb_bits = mb_ori_bits * 1000 / comp_ratio - delta; /* 1000 alg data */
    budget_mb_bits = tde_node_min(tde_node_max(budget_mb_bits, 64), mb_ori_bits); /* 64 alg data */

    /* for calculate max_mb_qp */
    if (osd_mode == 2) { /* 2 alg data */
        /* 32 3 alg data */
        max_mb_qp = (bit_depth_rgb) - ((budget_mb_bits - bit_depth_a * 32 - max_head_bits_raw - 1) / (32 * 3)) + 1;
    } else {
        /* 5 2 alg data */
        max_mb_qp = (bit_depth_rgb) - ((budget_mb_bits - max_head_bits_raw - 1) >> (5 + 2)) + 1;
    }

    /* for calculate budget_mb_bits_last */
    last_mb_width = (width % 32) ? (width % 32) : 32; /* 32 alg data */

    /* config reg */
    hw_node->tde_line_osd_cmp_rc_cfg0.bits.budget_mb_bits = budget_mb_bits;
    hw_node->tde_line_osd_cmp_rc_cfg0.bits.max_mb_qp = max_mb_qp;
    /* 32 alg data */
    hw_node->tde_line_osd_cmp_rc_cfg0.bits.budget_mb_bits_last = (budget_mb_bits * last_mb_width) / 32;
    tde_hal_node_set_cmp_qp_thr(hw_node, osd_mode);
    tde_hal_node_set_cmp_cfg(hw_node, max_mb_qp, bit_depth_rgb);
    tde_hal_node_set_cmp_status(hw_node, budget_mb_bits);
}

td_void tde_hal_node_set_compress(tde_hw_node *hw_node)
{
    if (hw_node == TD_NULL) {
        tde_error("HWNode is null !\n");
        return;
    }
    hw_node->tde_line_osd_cmp_glb_info.bits.ice_en = 1;
    hw_node->tde_line_osd_cmp_glb_info.bits.cmp_mode = 0;
    hw_node->tde_line_osd_cmp_glb_info.bits.is_lossless = 0;

    hw_node->tde_line_osd_cmp_glb_info.bits.osd_mode = (hw_node->des_ctrl.bits.des_fmt == TDE_DRV_COLOR_FMT_RGB888) ?
        0 : ((hw_node->des_ctrl.bits.des_fmt == TDE_DRV_COLOR_FMT_ARGB8888) ?
        1 : ((hw_node->des_ctrl.bits.des_fmt == TDE_DRV_COLOR_FMT_ARGB1555) ?
        2 : 3)); /* 2 3 des_fmt value */

    hw_node->tde_line_osd_cmp_glb_info.bits.conv_en = (hw_node->tde_line_osd_cmp_glb_info.bits.osd_mode > 1) ? 0 : 1;

    hw_node->tde_line_osd_cmp_frame_size.bits.frame_width = hw_node->des_imgsize.bits.des_width;
    hw_node->tde_line_osd_cmp_frame_size.bits.frame_height = hw_node->des_imgsize.bits.des_height;

    tde_hal_node_set_cmp(hw_node, hw_node->des_imgsize.bits.des_width);
    return;
}
#endif /* CONFIG_TDE_GFBG_COMPRESS_V2 */

td_void tde_hal_node_set_compress_tqt(tde_hw_node *hw_node, tde_surface_msg *drv_surface,
                                      tde_drv_outalpha_from alpha_from)
{
    if (hw_node == TD_NULL) {
        tde_error("HWNode is null !\n");
        return;
    }
    tde_hal_node_set_tqt(hw_node, drv_surface, alpha_from);
    hw_node->des_ctrl.bits.cmp_en = 1;
    return;
}

#endif
#if (TDE_CAPABILITY & ROTATE)
td_void tde_hal_node_set_rotate(tde_hw_node *hw_node, drv_tde_rotate_angle rotate_angle)
{
    if (hw_node == TD_NULL) {
        tde_error("HWNode is null !\n");
        return;
    }
    hw_node->src2_rtt_ctrl.bits.s2_rtt_en = 1;
    /* set bitmap attribute info */
    if (TDE_DRV_COLOR_FMT_YCBCR422 == hw_node->src2_ctrl.bits.src2_fmt) {
        hw_node->src2_rtt_ctrl.bits.s2_rtt_fmt = 0;
    }
    if ((hw_node->src2_ctrl.bits.src2_fmt == TDE_DRV_COLOR_FMT_RGB888) ||
        (TDE_DRV_COLOR_FMT_YCBCR888 == hw_node->src2_ctrl.bits.src2_fmt)) {
        hw_node->src2_rtt_ctrl.bits.s2_rtt_fmt = 1;
    }
    if ((hw_node->src2_ctrl.bits.src2_fmt == TDE_DRV_COLOR_FMT_ARGB4444) ||
        (hw_node->src2_ctrl.bits.src2_fmt == TDE_DRV_COLOR_FMT_ARGB1555)) {
        hw_node->src2_rtt_ctrl.bits.s2_rtt_fmt = 2; /* 2 ARGB4444 ARGB1555 */
    }
    if (hw_node->src2_ctrl.bits.src2_fmt == TDE_DRV_COLOR_FMT_ARGB8888) {
        hw_node->src2_rtt_ctrl.bits.s2_rtt_fmt = 3; /* 3 ARGB8888 */
    }
    if (rotate_angle == DRV_TDE_ROTATE_CLOCKWISE_90) {
        hw_node->src2_rtt_ctrl.bits.s2_rtt_dir = 0;
    } else {
        hw_node->src2_rtt_ctrl.bits.s2_rtt_dir = 1;
    }

    return;
}
#endif

static td_s32 tde_hal_node_file(tde_hw_node *hw_node, tde_base_opt_mode mode, tde_alu_mode alu,
                                const tde_color_fill *color_fill, td_u32 capability)
{
    if (alu > TDE_BUTT) {
        tde_error("alu invalid\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    if (mode == TDE_QUIKE_COPY) {
        if (!tde_hal_is_operation_support(alu, capability, QUICKCOPY)) {
            tde_error("It does not support QuickCopy\n");
            return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
        }
        hw_node->src1_ctrl.bits.src1_en = 1;
        hw_node->src1_ctrl.bits.src1_dma = 1;
    }
    if (mode == TDE_NORM_FILL_1OPT) {
        if (color_fill == TD_NULL) {
            tde_error("stColorFill is null !\n");
            return DRV_ERR_TDE_NULL_PTR;
        }
        tde_fill_data_by_fmt(hw_node, color_fill->fill_data, color_fill->drv_color_fmt, 1);
        hw_node->src2_ctrl.bits.src2_en = 1;
        hw_node->src2_ctrl.bits.src2_mode = 1;
        hw_node->cbmctrl.bits.cbm_en = 1;
        hw_node->cbmctrl.bits.cbm_mode = 1;
        hw_node->cbmctrl.bits.alu_mode = 1;
    }
    if (mode == TDE_NORM_FILL_2OPT) {
        if (color_fill == TD_NULL) {
            tde_error("stColorFill is null !\n");
            return DRV_ERR_TDE_NULL_PTR;
        }
        tde_fill_data_by_fmt(hw_node, color_fill->fill_data, color_fill->drv_color_fmt, 1);
        hw_node->src1_ctrl.bits.src1_en = 1;
        hw_node->src1_ctrl.bits.src1_mode = 0;
        hw_node->src2_ctrl.bits.src2_en = 1;
        hw_node->src2_ctrl.bits.src2_mode = 1;
        hw_node->cbmctrl.bits.cbm_en = 1;
        hw_node->cbmctrl.bits.cbm_mode = 1;

        hw_node->cbmctrl.bits.alu_mode = g_cbmctrl_alu_mode[alu];
    }
    return TD_SUCCESS;
}

static td_s32 tde_hal_node_opt(tde_hw_node *hw_node, tde_base_opt_mode mode, tde_alu_mode alu, td_u32 capability)
{
    if (alu < 0 || alu >= TDE_BUTT) {
        tde_error("alu invalid\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    if (mode == TDE_SINGLE_SRC_PATTERN_FILL_OPT) {
        if (!tde_hal_is_operation_support(alu, capability, PATTERFILL)) {
            tde_error("It deos not support PatternFill\n");
            return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
        }

        hw_node->src2_ctrl.bits.src2_en = 1;
        hw_node->src2_ctrl.bits.src2_mode = 2; /* 2 mode */
        hw_node->cbmctrl.bits.cbm_en = 1;
        hw_node->cbmctrl.bits.cbm_mode = 1;

        hw_node->cbmctrl.bits.alu_mode = g_cbmctrl_alu_mode[alu];
    }
    if (mode == TDE_DOUBLE_SRC_PATTERN_FILL_OPT) {
        if (!tde_hal_is_operation_support(alu, capability, PATTERFILL)) {
            tde_error("It deos not support PatternFill\n");
            return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
        }

        hw_node->src1_ctrl.bits.src1_en = 1;
        hw_node->src1_ctrl.bits.src1_mode = 0;

        hw_node->src2_ctrl.bits.src2_en = 1;
        hw_node->src2_ctrl.bits.src2_mode = 2; /* 2 mode */
        hw_node->cbmctrl.bits.cbm_en = 1;
        hw_node->cbmctrl.bits.cbm_mode = 1;

        hw_node->cbmctrl.bits.alu_mode = g_cbmctrl_alu_mode[alu];
    }
    return TD_SUCCESS;
}

static td_s32 tde_hal_node_set_base_hw(tde_hw_node *hw_node, tde_base_opt_mode mode, tde_alu_mode alu)
{
    if (alu < 0 || alu >= TDE_BUTT) {
        tde_error("alu invalid\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    if (mode == TDE_QUIKE_FILL) {
        hw_node->src1_ctrl.bits.src1_en = 1;
        hw_node->src1_ctrl.bits.src1_mode = 1;
        hw_node->src1_ctrl.bits.src1_dma = 1;
    }
    if (mode == TDE_NORM_BLIT_1OPT) {
        hw_node->src2_ctrl.bits.src2_en = 1;
        hw_node->src2_ctrl.bits.src2_mode = 0;
        hw_node->cbmctrl.bits.cbm_en = 1;
        hw_node->cbmctrl.bits.cbm_mode = 1;
        hw_node->cbmctrl.bits.alu_mode = 1;
    }

    if (mode == TDE_NORM_BLIT_2OPT) {
        hw_node->src1_ctrl.bits.src1_en = 1;
        hw_node->src1_ctrl.bits.src1_mode = 0;

        hw_node->src2_ctrl.bits.src2_en = 1;
        hw_node->src2_ctrl.bits.src2_mode = 0;

        hw_node->cbmctrl.bits.cbm_en = 1;
        hw_node->cbmctrl.bits.cbm_mode = 1;

        hw_node->cbmctrl.bits.alu_mode = g_cbmctrl_alu_mode[alu];
    }
    if (mode == TDE_MB_2OPT) {
        hw_node->src2_ctrl.bits.src2_en = 1;
        hw_node->src2_ctrl.bits.src2_mode = 0;

        hw_node->cbmctrl.bits.cbm_en = 1;
        hw_node->cbmctrl.bits.cbm_mode = 1;
        hw_node->cbmctrl.bits.alu_mode = 0x1;
    }
    return TD_SUCCESS;
}

static td_s32 tde_hal_quick_fill(tde_hw_node *hw_node, tde_base_opt_mode mode, tde_alu_mode alu,
    const tde_color_fill *color_fill, td_u32 capability)
{
    if (color_fill == TD_NULL) {
        tde_error("stColorFill is null !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }
    if (!tde_hal_is_operation_support(alu, capability, QUICKFILL)) {
        tde_error("It deos not support QuickFill\n");
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    tde_fill_data_by_fmt(hw_node, color_fill->fill_data, color_fill->drv_color_fmt, 0);
    if (tde_hal_node_set_base_hw(hw_node, mode, alu) != TD_SUCCESS) {
        return DRV_ERR_TDE_INVALID_PARA;
    }
    return TD_SUCCESS;
}

static td_s32 tde_hal_node_operation_support(tde_alu_mode alu, td_u32 capability)
{
#if (TDE_CAPABILITY & ROP)
    if (!tde_hal_is_operation_support(alu, capability, MASKROP)) {
        tde_error("It deos not support MaskRop\n");
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
#endif
#if (TDE_CAPABILITY & MASKBLEND)
    if (!tde_hal_is_operation_support(alu, capability, MASKBLEND)) {
        tde_error("It deos not support MaskBlend\n");
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
#endif
    return TD_SUCCESS;
}

td_s32 tde_hal_node_set_base_operate(tde_hw_node *hw_node, tde_base_opt_mode mode, tde_alu_mode alu,
                                     const tde_color_fill *color_fill)
{
    td_u32 capability = 0;
    td_s32 ret;

    if (hw_node == TD_NULL) {
        tde_error("hw_node is null !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }
    tde_hal_get_capability(&capability);

    switch (mode) {
        case TDE_QUIKE_FILL: /* quick file */
            ret = tde_hal_quick_fill(hw_node, mode, alu, color_fill, capability);
            if (ret != TD_SUCCESS) {
                return ret;
            }
            break;
        case TDE_QUIKE_COPY:
        case TDE_NORM_FILL_1OPT:
        case TDE_NORM_FILL_2OPT:
            ret = tde_hal_node_file(hw_node, mode, alu, color_fill, capability);
            if (ret != TD_SUCCESS) {
                return ret;
            }
            break;
        case TDE_SINGLE_SRC_PATTERN_FILL_OPT:
        case TDE_DOUBLE_SRC_PATTERN_FILL_OPT:
            ret = tde_hal_node_opt(hw_node, mode, alu, capability);
            if (ret != TD_SUCCESS) {
                return ret;
            }
            break;
        case TDE_NORM_BLIT_2OPT:
            ret = tde_hal_node_operation_support(alu, capability);
            if (ret != TD_SUCCESS) {
                return ret;
            }
            fallthrough;
        case TDE_NORM_BLIT_1OPT:
            fallthrough;
        case TDE_MB_2OPT:
            ret = tde_hal_node_set_base_hw(hw_node, mode, alu);
            if (ret != TD_SUCCESS) {
                return ret;
            }
            break;
        default:
            break;
    }
    return TD_SUCCESS;
}

static td_bool tde_hal_is_operation_support(tde_alu_mode alu_mode, td_u32 capability, td_u32 oper_mode)
{
    if (!((td_u32)MASKBLEND & oper_mode) && !((td_u32)MASKROP & oper_mode)) {
        return (td_bool)((capability & oper_mode) ? TD_TRUE : TD_FALSE);
    }

#if (TDE_CAPABILITY & MASKBLEND)
    if ((td_u32)MASKBLEND & oper_mode) {
        return (td_bool)(!((alu_mode == TDE_ALU_MASK_BLEND) && (!(capability & oper_mode))));
    }
#endif

#if (TDE_CAPABILITY & ROP)
    if ((td_u32)MASKROP & oper_mode) {
        return (td_bool)(!(((alu_mode == TDE_ALU_MASK_ROP1) || (alu_mode == TDE_ALU_MASK_ROP2)) &&
                         (!(capability & oper_mode))));
    }
#endif

    return TD_TRUE;
}

td_void tde_hal_node_set_global_alpha(tde_hw_node *hw_node, td_u8 alpha, td_bool enable)
{
    ot_unused(enable);

    if (hw_node == TD_NULL) {
        tde_error("HWNode is null !\n");
        return;
    }

    /* set node */ /* CNcomment:配置缓存节点 */
    hw_node->src2_cbmpara.bits.s2_galpha = alpha;

    return;
}

td_void tde_hal_node_set_src1_alpha(tde_hw_node *hw_node)
{
    if (hw_node == TD_NULL) {
        tde_error("HWNode is null !\n");
        return;
    }

    /* set alpha0 alpha1, CNcomment:alpha0, alpha1 */
    hw_node->src1_alpha.bits.src1_alpha0 = 0;
    hw_node->src1_alpha.bits.src1_alpha1 = 0xff;

    return;
}

td_void tde_hal_node_set_src2_alpha(tde_hw_node *hw_node)
{
    if (hw_node == TD_NULL) {
        tde_error("HWNode is null !\n");
        return;
    }

    /* set alpha0 alpha1, CNcomment:alpha0, alpha1 */
    hw_node->src2_alpha.bits.src2_alpha0 = 0;
    hw_node->src2_alpha.bits.src2_alpha1 = 0xff;

    return;
}

/*
 * Function:      tde_hal_node_set_exp_alpha
 * Description:   extend to alpha0 and alpha1 operation when extend alpha for RGB5551
 * Input:         hw_node:pointer of node
 *                alpha0: Alpha0 value
 *                alpha1: Alpha1 value
 */
td_void tde_hal_node_set_exp_alpha(tde_hw_node *hw_node, tde_src_mode src, td_u8 alpha0, td_u8 alpha1)
{
    if (hw_node == TD_NULL) {
        tde_error("HWNode is null !\n");
        return;
    }

    /* set alpha0 alpha1 */ /* CNcomment:配置alpha0, alpha1 */
    if (TDE_DRV_SRC_S1 & (td_u32)src) {
        hw_node->src1_alpha.bits.src1_alpha0 = alpha0;
        hw_node->src1_alpha.bits.src1_alpha1 = alpha1;
        hw_node->src1_ctrl.bits.src1_rgb_exp = 3; /* 3 set alpha0 alpha1 */
    }

    if (TDE_DRV_SRC_S2 & (td_u32)src) {
        hw_node->src2_alpha.bits.src2_alpha0 = alpha0;
        hw_node->src2_alpha.bits.src2_alpha1 = alpha1;
        hw_node->src2_ctrl.bits.src2_rgb_exp = 3; /* 3 set alpha0 alpha1 */
    }

    return;
}

#if (TDE_CAPABILITY & ROP)
td_s32 tde_hal_node_set_rop(tde_hw_node *hw_node, drv_tde_rop_mode rgb_rop, drv_tde_rop_mode alpha_rop)
{
    td_u32 capability;
    if (hw_node == TD_NULL) {
        tde_error("HWNode is null !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }
    tde_hal_get_capability(&capability);
    if (!(capability & ROP)) {
        tde_error("It deos not support Rop\n");
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    /* set node, CNcomment: Configure the cache node */
    hw_node->cbmctrl.bits.cbm_en = 1;
    hw_node->cbmctrl.bits.cbm_mode = 1;

    if (rgb_rop < 0 || rgb_rop >= DRV_TDE_ROP_MAX || alpha_rop < 0 || alpha_rop >= DRV_TDE_ROP_MAX) {
        tde_error("rgb_rop(%d) and alpha_rop(%d) invalid !should be in [%d, %d]",
            rgb_rop, alpha_rop, 0, DRV_TDE_ROP_WHITE);
        return DRV_ERR_TDE_INVALID_PARA;
    }
    hw_node->cbmalupara.bits.rgb_rop = (td_u32)rgb_rop;
    hw_node->cbmalupara.bits.a_rop = (td_u32)alpha_rop;

    return TD_SUCCESS;
}
#endif

static td_void tde_hal_node_blend_mode(tde_hw_node *hw_node, const drv_tde_blend_opt *blend_opt)
{
    if (blend_opt->blend_cmd == DRV_TDE_BLEND_CMD_NONE) {
        /* fs: sa      fd: 1.0-sa */
        drv_tde_node_blend_mode(hw_node, DRV_TDE_BLEND_INVSRC2ALPHA, DRV_TDE_BLEND_SRC2ALPHA);
    } else if (blend_opt->blend_cmd == DRV_TDE_BLEND_CMD_CLEAR) {
        drv_tde_node_blend_mode(hw_node, DRV_TDE_BLEND_ZERO, DRV_TDE_BLEND_ZERO);
    } else if (blend_opt->blend_cmd == DRV_TDE_BLEND_CMD_SRC) {
        drv_tde_node_blend_mode(hw_node, DRV_TDE_BLEND_ZERO, DRV_TDE_BLEND_ONE);
    } else if (blend_opt->blend_cmd == DRV_TDE_BLEND_CMD_SRCOVER) {
        drv_tde_node_blend_mode(hw_node, DRV_TDE_BLEND_INVSRC2ALPHA, DRV_TDE_BLEND_ONE);
    } else if (blend_opt->blend_cmd == DRV_TDE_BLEND_CMD_DSTOVER) {
        drv_tde_node_blend_mode(hw_node, DRV_TDE_BLEND_ONE, DRV_TDE_BLEND_INVSRC1ALPHA);
    } else if (blend_opt->blend_cmd == DRV_TDE_BLEND_CMD_SRCIN) {
        drv_tde_node_blend_mode(hw_node, DRV_TDE_BLEND_ZERO, DRV_TDE_BLEND_SRC1ALPHA);
    } else if (blend_opt->blend_cmd == DRV_TDE_BLEND_CMD_DSTIN) {
        drv_tde_node_blend_mode(hw_node, DRV_TDE_BLEND_SRC2ALPHA, DRV_TDE_BLEND_ZERO);
    } else if (blend_opt->blend_cmd == DRV_TDE_BLEND_CMD_SRCOUT) {
        drv_tde_node_blend_mode(hw_node, DRV_TDE_BLEND_ZERO, DRV_TDE_BLEND_INVSRC1ALPHA);
    } else if (blend_opt->blend_cmd == DRV_TDE_BLEND_CMD_DSTOUT) {
        drv_tde_node_blend_mode(hw_node, DRV_TDE_BLEND_INVSRC2ALPHA, DRV_TDE_BLEND_ZERO);
    } else if (blend_opt->blend_cmd == DRV_TDE_BLEND_CMD_SRCATOP) {
        drv_tde_node_blend_mode(hw_node, DRV_TDE_BLEND_INVSRC2ALPHA, DRV_TDE_BLEND_SRC1ALPHA);
    } else if (blend_opt->blend_cmd == DRV_TDE_BLEND_CMD_DSTATOP) {
        drv_tde_node_blend_mode(hw_node, DRV_TDE_BLEND_SRC2ALPHA, DRV_TDE_BLEND_INVSRC1ALPHA);
    } else if (blend_opt->blend_cmd == DRV_TDE_BLEND_CMD_ADD) {
        drv_tde_node_blend_mode(hw_node, DRV_TDE_BLEND_ONE, DRV_TDE_BLEND_ONE);
    } else if (blend_opt->blend_cmd == DRV_TDE_BLEND_CMD_XOR) {
        drv_tde_node_blend_mode(hw_node, DRV_TDE_BLEND_INVSRC2ALPHA, DRV_TDE_BLEND_INVSRC1ALPHA);
    } else if (blend_opt->blend_cmd == DRV_TDE_BLEND_CMD_DST) {
        drv_tde_node_blend_mode(hw_node, DRV_TDE_BLEND_ONE, DRV_TDE_BLEND_ZERO);
    } else {
        /* user parameter, CNcomment: The user's own configuration parameters */
        drv_tde_node_blend_mode(hw_node, blend_opt->src1_blend_mode, blend_opt->src2_blend_mode);
    }
}

td_s32 tde_hal_node_set_blend(tde_hw_node *hw_node, const drv_tde_blend_opt *blend_opt)
{
    td_u32 capability;

    if (hw_node == TD_NULL) {
        tde_error("HWNode is null !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }
    if (blend_opt == TD_NULL) {
        tde_error("stBlendOpt is null !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }
    tde_hal_get_capability(&capability);
    if (!(capability & ALPHABLEND)) {
        tde_error("It deos not support Blend\n");
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    hw_node->src1_cbmpara.bits.s1_palphaen = TD_TRUE;
    hw_node->src1_cbmpara.bits.s1_galphaen = TD_FALSE;
    hw_node->src1_cbmpara.bits.s1_multiglobalen = TD_FALSE;
    hw_node->src2_cbmpara.bits.s2_multiglobalen = TD_FALSE;

    hw_node->src1_cbmpara.bits.s1_premulten = blend_opt->src1_alpha_premulti;
    hw_node->src2_cbmpara.bits.s2_premulten = blend_opt->src2_alpha_premulti;
    hw_node->src2_cbmpara.bits.s2_palphaen = blend_opt->pixel_alpha_en;
    hw_node->src2_cbmpara.bits.s2_galphaen = blend_opt->global_alpha_en;

    /* set mode for src1 and src2 */ /* CNcomment:  配置Src1、Src2模式 */
    tde_hal_node_blend_mode(hw_node, blend_opt);
    hw_node->cbmctrl.bits.cbm_en = 1;
    hw_node->cbmctrl.bits.cbm_mode = 1;
    return TD_SUCCESS;
}

#if (TDE_CAPABILITY & COLORIZE)
td_s32 tde_hal_node_set_colorize(tde_hw_node *hw_node, td_u32 colorize)
{
    td_u32 capability;
    if (hw_node == TD_NULL) {
        tde_error("HWNode is null !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }
    tde_hal_get_capability(&capability);
    if (!(capability & COLORIZE)) {
        tde_error("It deos not support Colorize\n");
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    hw_node->cbmcolorize.bits.colorizeen = 1;
    hw_node->cbmcolorize.bits.colorizeb = colorize & 0xff;
    hw_node->cbmcolorize.bits.colorizeg = (colorize >> 8) & 0xff; /* 8 move to a minimum */
    hw_node->cbmcolorize.bits.colorizer = (colorize >> 16) & 0xff; /* 16 move to a minimum */
    return TD_SUCCESS;
}
#endif

td_void tde_hal_node_enable_alpha_rop(tde_hw_node *hw_node)
{
    if (hw_node == TD_NULL) {
        tde_error("null pointer\n");
        return;
    }
    hw_node->cbmalupara.bits.blendropen = 1;
    return;
}

/*
 * Function:      tde_hal_node_set_clut_opt
 * Description:   set color extend or color revise parameter
 * Input:         hw_node: pointer of node
 *                clut_cmd: Clut operation parameter
 */
td_s32 tde_hal_node_set_clut_opt(tde_hw_node *hw_node, const tde_clut_cmd *clut_cmd, td_bool reload)
{
    td_u32 capability = 0;

    if (hw_node == TD_NULL) {
        tde_error("HWNode is null !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }
    if (clut_cmd == TD_NULL) {
        tde_error("ClutCmd is null !");
        return DRV_ERR_TDE_NULL_PTR;
    }
    tde_hal_get_capability(&capability);
    if (!(capability & CLUT)) {
        tde_error("It deos not support Clut\n");
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    hw_node->src2_ctrl.bits.src2_clut_mode = (td_u32)clut_cmd->clut_mode;
    /* set node */ /* CNcomment:配置缓存节点 */
    if (clut_cmd->phy_clut_addr) {
        hw_node->tde_clut_addr_low = get_low_addr(clut_cmd->phy_clut_addr);
        hw_node->tde_clut_addr_high = get_high_addr(clut_cmd->phy_clut_addr);
    }

    ot_unused(reload);
    return TD_SUCCESS;
}

/*
 * Function:      tde_hal_node_set_colorkey
 * Description:   set parameter for color key operation  according color format
 * Input:         hw_node:pointer of node
 *                fmt_cat: color format
 *                color_key: pointer of color key value
 */
td_s32 tde_hal_node_set_colorkey(tde_hw_node *hw_node, tde_colorfmt_category fmt_cat,
                                 const tde_color_key_cmd *color_key)
{
    td_u32 capability = 0;

    if (hw_node == TD_NULL) {
        tde_error("hw_node is null !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }
    if (color_key == TD_NULL) {
        tde_error("color_key is null !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }

    tde_hal_get_capability(&capability);
    if (!(capability & COLORKEY)) {
        tde_error("It deos not support ColorKey\n");
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    hw_node->cbmkeypara.bits.keysel = (td_u32)color_key->colorkey_mode;
    hw_node->cbmkeypara.bits.keyen = 1;
    if (fmt_cat == TDE_COLORFMT_CATEGORY_ARGB) {
        tde_hal_node_set_argbcolor_key_para(hw_node, color_key);
    } else if (fmt_cat == TDE_COLORFMT_CATEGORY_CLUT) {
        tde_hal_node_set_clutcolor_key_para(hw_node, color_key);
    } else if (fmt_cat == TDE_COLORFMT_CATEGORY_YCBCR) {
        tde_hal_node_set_ycbcr_color_key_para(hw_node, color_key);
    } else {
        tde_error("The clorfmt deos not support ColorKey\n");
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    hw_node->cbmctrl.bits.cbm_en = 1;
    hw_node->cbmctrl.bits.cbm_mode = 1;

    return TD_SUCCESS;
}

static td_void tde_hal_node_set_ycbcr_color_key_para(tde_hw_node *hw_node, const tde_color_key_cmd *color_key)
{
    drv_tde_color_key_comp cr = color_key->colorkey_value.ycbcr_colorkey.cr;
    drv_tde_color_key_comp cb = color_key->colorkey_value.ycbcr_colorkey.cb;
    drv_tde_color_key_comp y = color_key->colorkey_value.ycbcr_colorkey.y;
    drv_tde_color_key_comp alpha = color_key->colorkey_value.ycbcr_colorkey.alpha;

    hw_node->cbmkeymin.all =
        tde_hal_get_ycb_cr_key_mask(cr.component_min, cb.component_min, y.component_min, alpha.component_min);
    hw_node->cbmkeymax.all =
        tde_hal_get_ycb_cr_key_mask(cr.component_max, cb.component_max, y.component_max, alpha.component_max);
    hw_node->cbmkeymask.all =
        tde_hal_get_ycb_cr_key_mask(cr.component_mask, cb.component_mask, y.component_mask, alpha.component_mask);

    hw_node->cbmkeypara.bits.keybmode = tde_hal_get_color_key_mode(&cr);
    hw_node->cbmkeypara.bits.keygmode = tde_hal_get_color_key_mode(&cb);
    hw_node->cbmkeypara.bits.keyrmode = tde_hal_get_color_key_mode(&y);
    hw_node->cbmkeypara.bits.keyamode = tde_hal_get_color_key_mode(&alpha);

    return;
}

static td_void tde_hal_node_set_clutcolor_key_para(tde_hw_node *hw_node, const tde_color_key_cmd *color_key)
{
    drv_tde_color_key_comp clut = color_key->colorkey_value.clut_colorkey.clut;
    drv_tde_color_key_comp alpha = color_key->colorkey_value.clut_colorkey.alpha;

    hw_node->cbmkeymin.all = tde_hal_get_clut_key_mask(clut.component_min, alpha.component_min);
    hw_node->cbmkeymax.all = tde_hal_get_clut_key_mask(clut.component_max, alpha.component_max);
    hw_node->cbmkeymask.all = tde_hal_get_clut_key_mask(clut.component_mask, alpha.component_mask);

    hw_node->cbmkeypara.bits.keybmode = tde_hal_get_color_key_mode(&clut);
    hw_node->cbmkeypara.bits.keyamode = tde_hal_get_color_key_mode(&alpha);

    return;
}

static td_void tde_hal_node_set_argbcolor_key_para(tde_hw_node *hw_node, const tde_color_key_cmd *color_key)
{
    drv_tde_color_key_comp blue = color_key->colorkey_value.argb_colorkey.blue;
    drv_tde_color_key_comp green = color_key->colorkey_value.argb_colorkey.green;
    drv_tde_color_key_comp red = color_key->colorkey_value.argb_colorkey.red;
    drv_tde_color_key_comp alpha = color_key->colorkey_value.argb_colorkey.alpha;

    hw_node->cbmkeymin.all =
        tde_hal_get_argb_key_mask(blue.component_min, green.component_min, red.component_min, alpha.component_min);
    hw_node->cbmkeymax.all =
        tde_hal_get_argb_key_mask(blue.component_max, green.component_max, red.component_max, alpha.component_max);
    hw_node->cbmkeymask.all =
        tde_hal_get_argb_key_mask(blue.component_mask, green.component_mask, red.component_mask, alpha.component_mask);

    hw_node->cbmkeypara.bits.keybmode = tde_hal_get_color_key_mode(&blue);
    hw_node->cbmkeypara.bits.keygmode = tde_hal_get_color_key_mode(&green);
    hw_node->cbmkeypara.bits.keyrmode = tde_hal_get_color_key_mode(&red);
    hw_node->cbmkeypara.bits.keyamode = tde_hal_get_color_key_mode(&alpha);
    return;
}

static td_u32 tde_hal_get_color_key_mode(const drv_tde_color_key_comp *color_key)
{
    return (td_u32)((color_key->is_component_ignore) ? TDE_COLORKEY_IGNORE :
                    (color_key->is_component_out) ? TDE_COLORKEY_AREA_OUT : TDE_COLORKEY_AREA_IN);
}

static td_u32 tde_hal_get_ycb_cr_key_mask(td_u8 cr, td_u8 cb, td_u8 cy, td_u8 alpha)
{
    return (td_u32)(cr | (cb << TDE_EIGHT_BITS_SHIFT) | (cy << TDE_SIXTEEN_BITS_SHIFT) |
            (alpha << TDE_TWENTYFOUR_BITS_SHIFT));
}

static td_u32 tde_hal_get_clut_key_mask(td_u8 clut, td_u8 alpha)
{
    return (td_u32)(clut | (alpha << TDE_TWENTYFOUR_BITS_SHIFT));
}

static td_u32 tde_hal_get_argb_key_mask(td_u8 blue, td_u8 green, td_u8 red, td_u8 alpha)
{
    return (td_u32)(blue | (green << TDE_EIGHT_BITS_SHIFT) | (red << TDE_SIXTEEN_BITS_SHIFT) |
            (alpha << TDE_TWENTYFOUR_BITS_SHIFT));
}

td_s32 tde_hal_node_set_clipping(tde_hw_node *hw_node, const tde_clip_cmd *clip)
{
    td_u32 capability = 0;

    if (hw_node == TD_NULL) {
        tde_error("HWNode is null !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }
    if (clip == TD_NULL) {
        tde_error("Clip is null !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }

    tde_hal_get_capability(&capability);
    if (!(capability & CLIP)) {
        tde_error("It deos not support Clip\n");
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    hw_node->des_alpha.bits.des_crop_mode = 0;
    if (!clip->inside_clip) {
        /* over clip */ /* CNcomment: 区域外clip指示 */
        hw_node->des_alpha.bits.des_crop_mode = 1;
    }
    hw_node->des_alpha.bits.des_crop_en = 1;
    hw_node->des_crop_pos_st.bits.des_crop_start_x = clip->clip_start_x;
    hw_node->des_crop_pos_st.bits.des_crop_start_y = clip->clip_start_y;
    hw_node->des_crop_pos_ed.bits.des_crop_end_x = clip->clip_end_x;
    hw_node->des_crop_pos_ed.bits.des_crop_end_y = clip->clip_end_y;
    return TD_SUCCESS;
}

static td_s32 tde_hal_calc_src2_filter_opt_check(const tde_hw_node *node, const tde_rect_opt *rect_opt)
{
    if (node == TD_NULL || rect_opt == TD_NULL) {
        tde_error("null pointer !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }
    if (rect_opt->in_rect == TD_NULL) {
        tde_error("rect_opt->pInRect is null !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }
    if (rect_opt->out_rect == TD_NULL) {
        tde_error("rect_opt->pOutRect is null !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }
    if (rect_opt->in_fmt >= (DRV_TDE_COLOR_FMT_MAX + 1)) {
        return DRV_ERR_TDE_INVALID_PARA;
    }
    return TD_SUCCESS;
}

static td_s32 tde_hal_set_zme_in_limit(tde_hw_node *node, const tde_rect_opt *rect_opt)
{
    td_s32 ver_scale_coeff = 1;
    td_s32 ret;
    if ((rect_opt->out_rect->width * 16) <= rect_opt->in_rect->width) { /* 16 2^4 alg data */
        node->src2_hpzme.bits.src2_hpzme_en = 1;
        node->src2_hpzme.bits.src2_hpzme_mode = tde_hal_get_hpzme_mode(rect_opt->out_rect->width,
            rect_opt->in_rect->width);
        if ((node->src2_hpzme.bits.src2_hpzme_mode + 1) == 0) {
            return DRV_ERR_TDE_INVALID_PARA;
        }
        node->src2_hpzme_size.bits.src2_hpzme_width =
            rect_opt->in_rect->width / (node->src2_hpzme.bits.src2_hpzme_mode + 1) +
            ((rect_opt->in_rect->width % (node->src2_hpzme.bits.src2_hpzme_mode + 1)) ? 1 : 0);
        node->src2_hsp.bits.hratio = (rect_opt->out_rect->width <= 1) ? 0 :
            (osal_div_u64(((td_u64)(node->src2_hpzme_size.bits.src2_hpzme_width) << TDE_HAL_HSTEP_FLOATLEN),
                (rect_opt->out_rect->width)));
    }

    if ((rect_opt->out_rect->height * 16) <= rect_opt->in_rect->height) { /* 16 2^4 alg data */
        ret = tde_hal_get_ver_scale_coeff(rect_opt->out_rect->height, rect_opt->in_rect->height, &ver_scale_coeff);
        if (ret != TD_SUCCESS) {
            return ret;
        }
        if (ver_scale_coeff == 0) {
            return DRV_ERR_TDE_INVALID_PARA;
        }
        node->src2_zmeireso.bits.ih = rect_opt->in_rect->height / ver_scale_coeff - 1;
        node->src2_imgsize.bits.src2_height = rect_opt->in_rect->height / ver_scale_coeff - 1;
        node->src2_ch0_stride.bits.src2_ch0_stride = node->src2_ch0_stride.bits.src2_ch0_stride * ver_scale_coeff;
        if (rect_opt->out_rect->height == 0) {
            return DRV_ERR_TDE_INVALID_PARA;
        }
        node->src2_vsr.bits.vratio = osal_div_u64(((td_u64)(node->src2_imgsize.bits.src2_height) <<
            TDE_FLOAT_BITLEN), (rect_opt->out_rect->height));
    } else {
        if (rect_opt->out_rect->height == 0) {
            return DRV_ERR_TDE_INVALID_PARA;
        }
        node->src2_vsr.bits.vratio = (rect_opt->out_rect->height <= 1) ? 0 :
            (osal_div_u64(((td_u64)(rect_opt->in_rect->height) << TDE_FLOAT_BITLEN), (rect_opt->out_rect->height)));
    }
    return TD_SUCCESS;
}

static td_void tde_hal_set_zme(tde_hw_node *node, const tde_rect_opt *rect_opt, drv_tde_deflicker_mode filter_mode,
    td_bool defilicker)
{
    tde_filtermode flt_mode = {0};
    td_phys_addr_t ch_coef_addr;
    td_phys_addr_t cv_coef_addr;

    node->src2_hsp.bits.hchmsc_en = 1;
    node->src2_hsp.bits.hlmsc_en = 1;

    tde_hal_get_filter_mode((drv_tde_filter_mode)filter_mode, &flt_mode);

    if (node->src2_hsp.bits.hratio != NO_HSCALE_STEP) {
        if (rect_opt->out_rect->width > MAX_LINE_BUFFER) {
            node->src2_hsp.bits.hfir_order = 1;
        }
        if (node->src2_vsp.bits.zme_in_fmt == TDE_ZME_FMT_ARGB) {
            node->src2_hsp.bits.hlfir_en = flt_mode.alpha_en;
        } else {
            node->src2_hsp.bits.hlfir_en = flt_mode.luma_en;
        }
        node->src2_hsp.bits.hafir_en = flt_mode.alpha_en;
        node->src2_hsp.bits.hchfir_en = flt_mode.chrome_en;
        ch_coef_addr = g_para_table.hf_coef_addr + tde_hal_get_resize_para_htable(node->src2_hsp.bits.hratio) *
                       TDE_PARA_HTABLE_SIZE;

        node->tde_src2_zme_chaddr_low = get_low_addr(ch_coef_addr);
        node->tde_src2_zme_chaddr_high = get_high_addr(ch_coef_addr);

        node->tde_src2_zme_lhaddr_low = node->tde_src2_zme_chaddr_low;
        node->tde_src2_zme_lhaddr_high = node->tde_src2_zme_chaddr_high;
    }

    node->src2_vsp.bits.vchmsc_en = 1;
    node->src2_vsp.bits.vlmsc_en = 1;

    if ((node->src2_vsr.bits.vratio != NO_VSCALE_STEP) && (!defilicker)) {
        node->src2_vsp.bits.vafir_en = flt_mode.alpha_en;
        if (node->src2_vsp.bits.zme_in_fmt == TDE_ZME_FMT_ARGB) {
            node->src2_vsp.bits.vlfir_en = flt_mode.alpha_en;
        } else {
            node->src2_vsp.bits.vlfir_en = flt_mode.luma_en;
        }
        node->src2_vsp.bits.vchfir_en = flt_mode.chrome_en;

        cv_coef_addr = g_para_table.vf_coef_addr + tde_hal_get_resize_para_vtable(node->src2_vsr.bits.vratio) *
                       TDE_PARA_VTABLE_SIZE;
        node->tde_src2_zme_cvaddr_low = get_low_addr(cv_coef_addr);
        node->tde_src2_zme_cvaddr_high = get_high_addr(cv_coef_addr);

        node->tde_src2_zme_lvaddr_low = node->tde_src2_zme_cvaddr_low;
        node->tde_src2_zme_lvaddr_high = node->tde_src2_zme_cvaddr_high;
    }
}

#if (TDE_CAPABILITY & DEFLICKER)
static td_void tde_hal_set_defilicker(tde_hw_node *node)
{
    td_phys_addr_t cv_deflicker_coef_addr;

    node->src2_vsp.bits.vchmsc_en = 1;
    node->src2_vsp.bits.vlmsc_en = 1;
    node->src2_vsp.bits.vafir_en = 1;
    node->src2_vsp.bits.vlfir_en = 1;
    node->src2_vsp.bits.vchfir_en = 1;

    cv_deflicker_coef_addr = g_para_table.deflicker_vf_coef_addr +
                             tde_hal_get_resize_para_vtable(node->src2_vsr.bits.vratio) * TDE_PARA_VTABLE_SIZE;

    node->tde_src2_zme_cvaddr_low = get_low_addr(cv_deflicker_coef_addr);
    node->tde_src2_zme_cvaddr_high = get_high_addr(cv_deflicker_coef_addr);

    node->tde_src2_zme_lvaddr_low = node->tde_src2_zme_cvaddr_low;
    node->tde_src2_zme_lvaddr_high = node->tde_src2_zme_cvaddr_high;
}
#endif

td_s32 tde_hal_calc_src2_filter_opt(tde_hw_node *node, const tde_rect_opt *rect_opt, td_bool defilicker,
                                    drv_tde_deflicker_mode filter_mode)
{
    tde_color_fmt in_drv_fmt;
    td_s32 ret;

    ret = tde_hal_calc_src2_filter_opt_check(node, rect_opt);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    in_drv_fmt = tde_get_common_drv_color_fmt((td_u32)rect_opt->in_fmt);

    node->src2_vsp.bits.zme_in_fmt = tde_hal_get_zme_in_fmt(in_drv_fmt);

    node->src2_vsp.bits.zme_out_fmt = (node->src2_vsp.bits.zme_in_fmt == TDE_ZME_FMT_ARGB) ? TDE_ZME_FMT_ARGB :
                                       TDE_ZME_FMT_YUV444;

    node->src2_imgsize.bits.src2_width = rect_opt->in_rect->width - 1;
    node->src2_zmeireso.bits.iw = rect_opt->in_rect->width - 1;
    node->src2_zmeoreso.bits.ow = rect_opt->out_rect->width - 1;
    node->src2_zmeoreso.bits.oh = rect_opt->out_rect->height - 1;
    node->src2_imgsize.bits.src2_height = rect_opt->in_rect->height - 1;
    node->src2_zmeireso.bits.ih = rect_opt->in_rect->height - 1;

    node->des_imgsize.bits.des_width = rect_opt->out_rect->width - 1;
    node->des_imgsize.bits.des_height = rect_opt->out_rect->height - 1;

    node->src2_hsp.bits.hratio = (rect_opt->out_rect->width <= 1) ? 0 :
        (osal_div_u64(((td_u64)(rect_opt->in_rect->width) << TDE_HAL_HSTEP_FLOATLEN), (rect_opt->out_rect->width)));

    node->src2_hpzme_size.bits.src2_hpzme_width = rect_opt->in_rect->width;

    ret = tde_hal_set_zme_in_limit(node, rect_opt);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    tde_hal_set_zme(node, rect_opt, filter_mode, defilicker);

#if (TDE_CAPABILITY & DEFLICKER)
    if (defilicker) {
        tde_hal_set_defilicker(node);
    }
#endif

    node->src2_zmeireso.bits.iw = node->src2_hpzme_size.bits.src2_hpzme_width - 1;

    node->src2_hpzme_size.bits.src2_hpzme_width = node->src2_hpzme_size.bits.src2_hpzme_width - 1;

    return TD_SUCCESS;
}

static td_u32 tde_hal_get_zme_in_fmt(tde_color_fmt in_drv_fmt)
{
    td_u32 zme_in_fmt = 0;

    td_bool fmt_argb = (in_drv_fmt <= TDE_DRV_COLOR_FMT_ACLUT88) || ((in_drv_fmt == TDE_DRV_COLOR_FMT_YCBCR444MB)) ||
                       (in_drv_fmt == TDE_DRV_COLOR_FMT_YCBCR400MBP);
    td_bool fmt_yuv422 = (in_drv_fmt == TDE_DRV_COLOR_FMT_YCBCR422MBH) || (in_drv_fmt == TDE_DRV_COLOR_FMT_YCBCR422);
    td_bool fmt_yuv420 = (in_drv_fmt == TDE_DRV_COLOR_FMT_YCBCR420MB) || (in_drv_fmt == TDE_DRV_COLOR_FMT_YCBCR422MBV);

    if (fmt_argb) {
        zme_in_fmt = TDE_ZME_FMT_ARGB;
    }

    if (fmt_yuv422) {
        zme_in_fmt = TDE_ZME_FMT_YUV422;
    }

    if (fmt_yuv420) {
        zme_in_fmt = TDE_ZME_FMT_YUV420;
    }

    return zme_in_fmt;
}

static td_u32 tde_hal_get_hpzme_mode(td_u32 out_rect_width, td_u32 in_rect_width)
{
    td_u32 hpzme_mode;

    if ((out_rect_width * 32) > in_rect_width) { /* 32 alg data */
        hpzme_mode = 1;
    } else if ((out_rect_width * 64) > in_rect_width) { /* 64 alg data */
        hpzme_mode = 3; /* 3 alg data */
    } else if ((out_rect_width * 128) > in_rect_width) { /* 128 alg data */
        hpzme_mode = 7; /* 7 hpzme mode */
    } else {
        hpzme_mode = 15; /* 15 hpzme mode */
    }

    return hpzme_mode;
}

static td_void tde_hal_get_filter_mode(drv_tde_filter_mode filter_mode, tde_filtermode *flt_mode)
{
    td_bool filter_mode_color = (filter_mode == DRV_TDE_FILTER_MODE_COLOR);
    td_bool filter_mode_alpha = (filter_mode == DRV_TDE_FILTER_MODE_ALPHA);
    td_bool filter_mode_both = (filter_mode == DRV_TDE_FILTER_MODE_BOTH);
    td_u32 alpha_en;
    td_u32 luma_en;
    td_u32 chrome_en;

    if (filter_mode_color) {
        chrome_en = 1;
        luma_en = 1;
        alpha_en = 0;
    } else if (filter_mode_alpha) {
        chrome_en = 0;
        luma_en = 0;
        alpha_en = 1;
    } else if (filter_mode_both) {
        chrome_en = 1;
        luma_en = 1;
        alpha_en = 1;
    } else {
        chrome_en = 0;
        luma_en = 0;
        alpha_en = 0;
    }
    flt_mode->alpha_en = alpha_en;
    flt_mode->luma_en = luma_en;
    flt_mode->chrome_en = chrome_en;

    return;
}

static td_s32 tde_hal_get_ver_scale_coeff(td_u32 out_rect_height, td_u32 in_rect_height, td_s32 *ver_scale_coeff)
{
    if ((out_rect_height * 32) >= in_rect_height) { /* 32 alg data */
        *ver_scale_coeff = 2; /* 2 ver scale coeff data */
    } else if ((out_rect_height * 64) >= in_rect_height) { /* 64 alg data */
        *ver_scale_coeff = 4; /* 4 ver scale coeff data */
    } else if ((out_rect_height * 128) >= in_rect_height) { /* 128 alg data */
        *ver_scale_coeff = 8; /* 8 ver scale coeff data */
    } else if (out_rect_height * 256 >= in_rect_height) { /* 256 alg data */
        *ver_scale_coeff = 16; /* 16 ver scale coeff data */
    } else {
        tde_error("Invalid para input!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    return TD_SUCCESS;
}

static td_void tde_hal_node_set_color_cvt_in_rgb2yuv(tde_hw_node *hw_node)
{
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wbitfield-constant-conversion"
#endif
    hw_node->src1_csc_idc0.bits.cscidc0 = 0;
    hw_node->src1_csc_idc0.bits.cscidc1 = 0;
    hw_node->src1_csc_idc1.bits.cscidc2 = 0;
    hw_node->src1_csc_odc0.bits.cscodc0 = 128 * 4; /* 4 * 128 cscodc0 */
    hw_node->src1_csc_odc0.bits.cscodc1 = 128 * 4; /* 4 * 128 cscodc1 */
    hw_node->src1_csc_odc1.bits.cscodc2 = 16 * 4;  /* 4 * 16 cscodc2 */

    hw_node->src1_csc_p0.bits.cscp00 = 66 * 4;  /* 4 * 66 cscp00 */
    hw_node->src1_csc_p0.bits.cscp01 = 129 * 4; /* 4 * 129 cscp01 data */

    hw_node->src1_csc_p1.bits.cscp02 = 25 * 4;  /* 4 * 25 cscp02 */
    hw_node->src1_csc_p1.bits.cscp10 = -38 * 4; /* 4 * -38 cscp10 data */

    hw_node->src1_csc_p2.bits.cscp11 = -74 * 4; /* 4 * -74 cscp11 data */
    hw_node->src1_csc_p2.bits.cscp12 = 112 * 4; /* 112 * 4 cscp12 data */

    hw_node->src1_csc_p3.bits.cscp20 = 112 * 4; /* 4 * 112 cscp20 data */
    hw_node->src1_csc_p3.bits.cscp21 = -94 * 4; /* 4 * -94 cscp21 data */

    hw_node->src1_csc_p4.bits.cscp22 = -18 * 4; /* 4 * -18 cscp22 data */
#ifdef __clang__
#pragma clang diagnostic pop
#endif
}

static td_void tde_hal_node_set_color_cvt_in_yuv2rgb(tde_hw_node *hw_node)
{
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wbitfield-constant-conversion"
#endif
    hw_node->src1_csc_idc0.bits.cscidc0 = -128 * 4; /* 4 * -128 cscidc0 data */
    hw_node->src1_csc_idc0.bits.cscidc1 = -128 * 4; /* 4 * -128 cscidc1 data */
    hw_node->src1_csc_idc1.bits.cscidc2 = -16 * 4;  /* 4 * -16 cscidc2 data */
    hw_node->src1_csc_odc0.bits.cscodc0 = 0;
    hw_node->src1_csc_odc0.bits.cscodc1 = 0;
    hw_node->src1_csc_odc1.bits.cscodc2 = 0;

    hw_node->src1_csc_p0.bits.cscp00 = 297 * 4; /* 4 * 297 cscp00 data */
    hw_node->src1_csc_p0.bits.cscp01 = 0;

    hw_node->src1_csc_p1.bits.cscp02 = 408 * 4; /* 4 * 408 cscp02 data */
    hw_node->src1_csc_p1.bits.cscp10 = 297 * 4; /* 4 * 297 cscp10 data */

    hw_node->src1_csc_p2.bits.cscp11 = -100 * 4; /* 4 * -100 cscp11 data */
    hw_node->src1_csc_p2.bits.cscp12 = -208 * 4; /* 4 * -208 cscp12 data */

    hw_node->src1_csc_p3.bits.cscp20 = 297 * 4; /* 4 * 297 cscp20 data */
    hw_node->src1_csc_p3.bits.cscp21 = 516 * 4; /* 4 * 516 cscp21 data */

    hw_node->src1_csc_p4.bits.cscp22 = 0;
#ifdef __clang__
#pragma clang diagnostic pop
#endif
}

static td_void tde_hal_node_set_color_cvt_out_rgb2yuv(tde_hw_node *hw_node)
{
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wbitfield-constant-conversion"
#endif
    hw_node->des_csc_idc0.bits.cscidc0 = -128 * 4; /* 4 * -128 cscidc0 data */
    hw_node->des_csc_idc0.bits.cscidc1 = -128 * 4; /* 4 * -128 cscidc1 data */
    hw_node->des_csc_idc1.bits.cscidc2 = -16 * 4;  /* 4 * -16 cscidc2 data */
    hw_node->des_csc_odc0.bits.cscodc0 = 0;
    hw_node->des_csc_odc0.bits.cscodc1 = 0;
    hw_node->des_csc_odc1.bits.cscodc2 = 0;

    hw_node->des_csc_p0.bits.cscp00 = 297 * 4; /* 4 * 297 cscp00 data */
    hw_node->des_csc_p0.bits.cscp01 = 0;

    hw_node->des_csc_p1.bits.cscp02 = 408 * 4; /* 4 * 408 cscp02 data */
    hw_node->des_csc_p1.bits.cscp10 = 297 * 4; /* 4 * 297 cscp10 data */

    hw_node->des_csc_p2.bits.cscp11 = -100 * 4; /* 4 * -100 cscp11 data */
    hw_node->des_csc_p2.bits.cscp12 = -208 * 4; /* 4 * -208 cscp12 data */

    hw_node->des_csc_p3.bits.cscp20 = 297 * 4; /* 4 * 297 cscp20 data */
    hw_node->des_csc_p3.bits.cscp21 = 516 * 4; /* 4 * 516 cscp21 data */
    hw_node->des_csc_p4.bits.cscp22 = 0;
#ifdef __clang__
#pragma clang diagnostic pop
#endif
}

static td_void tde_hal_node_set_color_cvt_out_yuv2rgb(tde_hw_node *hw_node)
{
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wbitfield-constant-conversion"
#endif
    hw_node->des_csc_idc0.bits.cscidc0 = 0;
    hw_node->des_csc_idc0.bits.cscidc1 = 0;
    hw_node->des_csc_idc1.bits.cscidc2 = 0;
    hw_node->des_csc_odc0.bits.cscodc0 = 128 * 4; /* 4 * 128 cscodc0 data */
    hw_node->des_csc_odc0.bits.cscodc1 = 128 * 4; /* 4 * 128 cscodc1 data */
    hw_node->des_csc_odc1.bits.cscodc2 = 16 * 4;  /* 4 * 16 cscodc2 data */

    hw_node->des_csc_p0.bits.cscp00 = 66 * 4;  /* 4 * 66 cscp00 data */
    hw_node->des_csc_p0.bits.cscp01 = 129 * 4; /* 4 * 129 cscp01 data */

    hw_node->des_csc_p1.bits.cscp02 = 25 * 4;  /* 4 * 25 cscp02 data */
    hw_node->des_csc_p1.bits.cscp10 = -38 * 4; /* 4 * -38 cscp10 data */

    hw_node->des_csc_p2.bits.cscp11 = -74 * 4; /* 4 * -74 cscp11 data */

    hw_node->des_csc_p2.bits.cscp12 = 112 * 4; /* 4 * 112 cscp12 data */

    hw_node->des_csc_p3.bits.cscp20 = 112 * 4; /* 4 * 112 cscp20 data */

    hw_node->des_csc_p3.bits.cscp21 = -94 * 4; /* 4 * -94 cscp21 data */
    hw_node->des_csc_p4.bits.cscp22 = -18 * 4; /* 4 * -18 cscp22 data */
#ifdef __clang__
#pragma clang diagnostic pop
#endif
}

/*
 * Function:      tde_hal_node_set_color_convert
 * Description:   set parameter for color space change
 * Input:         hw_node:pointer of node
 *                conv: parameter of color space change
 */
td_s32 tde_hal_node_set_color_convert(tde_hw_node *hw_node, const tde_conv_mode_cmd *conv)
{
    td_u32 capability = 0;
    if (hw_node == TD_NULL) {
        tde_error("HWNode is null !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }
    if (conv == TD_NULL) {
        tde_error("Conv is null !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }
    tde_hal_get_capability(&capability);
    if (!(capability & CSCCOVERT)) {
        tde_error("It deos not support CSCCovert\n");
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    if (conv->in_conv) {
        hw_node->src2_csc_mux.bits.src2_csc_mode = 1 - (td_u32)conv->in_src1_conv;

        hw_node->src1_csc_idc0.bits.csc_en = 1;
        hw_node->src1_dither_ctrl.bits.dither_en = 1;
        hw_node->src1_dither_ctrl.bits.dither_round = 0;

        if (conv->in_rgb2_yc) {
            tde_hal_node_set_color_cvt_in_rgb2yuv(hw_node);
        } else {
            tde_hal_node_set_color_cvt_in_yuv2rgb(hw_node);
        }
    }

    if (conv->out_conv) {
        hw_node->src2_csc_mux.bits.src2_csc_mode = 1 - (td_u32)conv->in_src1_conv;

        hw_node->des_csc_idc0.bits.csc_en = 1;
        hw_node->dst_dither_ctrl.bits.dither_en = 1;
        hw_node->dst_dither_ctrl.bits.dither_round = 0;

        if (conv->in_rgb2_yc) {
            tde_hal_node_set_color_cvt_out_rgb2yuv(hw_node);
        } else {
            tde_hal_node_set_color_cvt_out_yuv2rgb(hw_node);
        }
    }
    return TD_SUCCESS;
}

td_s32 tde_set_node_csc(const tde_hw_node *hw_node, drv_tde_csc_opt csc_opt)
{
    ot_unused(hw_node);
    ot_unused(csc_opt);
    return TD_SUCCESS;
}

static td_u32 *tde_hal_make_hf_coef(td_void)
{
    td_u32 i;
    td_u32 *hf_coef = TD_NULL;

    hf_coef = (td_u32 *)tde_malloc(TDE_PARA_HTABLE_SIZE * TDE_PARA_HTABLE_NUM);
    if (hf_coef == TD_NULL) {
        tde_error("Alloc horizontal coef failed!HCoef table size:%d\n", TDE_PARA_HTABLE_SIZE * TDE_PARA_HTABLE_NUM);
        return TD_NULL;
    }

    for (i = 0; i < TDE_PARA_HTABLE_NUM; i++) {
         /* 4 size value */
        if (memcpy_s(hf_coef + i * (TDE_PARA_HTABLE_SIZE / 4), TDE_PARA_HTABLE_SIZE, g_tde_6x32_coef +
            i * (TDE_PARA_HTABLE_ORG_SIZE), (TDE_PARA_HTABLE_ORG_SIZE) * 4) != EOK) { /* 4 size */
            tde_error("memcpy_s failure\n");
            tde_free(hf_coef);
            return TD_NULL;
        }
    }

#ifndef __RTOS__
    g_para_table.hf_coef_addr = wgetphy((td_void *)hf_coef);
#else
    g_para_table.hf_coef_addr = (td_u32)hf_coef;
#endif
    return hf_coef;
}

static td_u32 *tde_hal_make_vf_coef(td_void)
{
    td_u32 i;
    td_u32 *vf_coef = TD_NULL;

    vf_coef = (td_u32 *)tde_malloc(TDE_PARA_VTABLE_SIZE * TDE_PARA_VTABLE_NUM);
    if (vf_coef == TD_NULL) {
        tde_error("Alloc vertical coef failed!VfCoef table size:%d\n", TDE_PARA_VTABLE_SIZE * TDE_PARA_VTABLE_NUM);
        return TD_NULL;
    }
    /*
     * copy parameter according other offer way , CNcomment :According to the parameter table algorithm
     * group provides the structure of the copies
     */
    for (i = 0; i < TDE_PARA_VTABLE_NUM; i++) {
        if (memcpy_s(vf_coef + i * (TDE_PARA_VTABLE_SIZE / 4), TDE_PARA_VTABLE_SIZE, g_org_vf_coef + /* 4 size value */
            i * (TDE_PARA_VTABLE_ORG_SIZE), (TDE_PARA_VTABLE_ORG_SIZE) * 4) != EOK) { /* 4 size value */
            tde_error("secure function failure\n");
            tde_free(vf_coef);
            return TD_NULL;
        }
    }

#ifndef __RTOS__
    g_para_table.vf_coef_addr = wgetphy((td_void *)vf_coef);
#else
    g_para_table.vf_coef_addr = (td_u32)vf_coef;
#endif
    return vf_coef;
}

#if (TDE_CAPABILITY & DEFLICKER)
static td_u32 *tde_hal_make_deflicker_vf_coef(td_void)
{
    td_u32 i;
    td_u32 *deflicker_vf_coef = TD_NULL;

    deflicker_vf_coef = (td_u32 *)tde_malloc(TDE_PARA_VTABLE_SIZE * TDE_PARA_VTABLE_NUM);
    if (deflicker_vf_coef == TD_NULL) {
        tde_error("Alloc Deflicker vertical coef failed!g_deflicker_vf_coef table size:%d\n",
                  TDE_PARA_VTABLE_SIZE * TDE_PARA_VTABLE_NUM);
        return TD_NULL;
    }

    for (i = 0; i < TDE_PARA_VTABLE_NUM; i++) {
        /* 4 alg data */
        if (memcpy_s(deflicker_vf_coef + i * (TDE_PARA_VTABLE_SIZE / 4), TDE_PARA_VTABLE_SIZE, g_deflicker_vf_coef +
            i * (TDE_PARA_VTABLE_ORG_SIZE), (TDE_PARA_VTABLE_ORG_SIZE) * 4) != EOK) { /* 4 alg data */
            tde_error("secure function failure\n");
            tde_free(deflicker_vf_coef);
            return TD_NULL;
        }
    }

#ifndef __RTOS__
    g_para_table.deflicker_vf_coef_addr = wgetphy((td_void *)deflicker_vf_coef);
#else
    g_para_table.deflicker_vf_coef_addr = (td_u32)deflicker_vf_coef;
#endif
    return deflicker_vf_coef;
}
#endif

static td_s32 tde_hal_init_para_table(td_void)
{
    td_u32 *hf_coef = TD_NULL;
    td_u32 *vf_coef = TD_NULL;

#if (TDE_CAPABILITY & DEFLICKER)
    td_u32 *deflicker_vf_coef = TD_NULL;
#endif

    (td_void)memset_s(&g_para_table, sizeof(g_para_table), 0, sizeof(g_para_table));
    hf_coef = tde_hal_make_hf_coef();
    if (hf_coef == TD_NULL) {
        return TD_FAILURE;
    }
    vf_coef = tde_hal_make_vf_coef();
    if (vf_coef == TD_NULL) {
        tde_free(hf_coef);
        return TD_FAILURE;
    }

#if (TDE_CAPABILITY & DEFLICKER)
    deflicker_vf_coef = tde_hal_make_deflicker_vf_coef();
    if (deflicker_vf_coef == TD_NULL) {
        tde_free(hf_coef);
        tde_free(vf_coef);
        return TD_FAILURE;
    }
#endif

    return TD_SUCCESS;
}

/*
 * Function:      tde_hal_cur_node
 * Description:   get the node physics address that is suspended
 * Return:        the address of current running node
 */
td_phys_addr_t tde_hal_cur_node(void)
{
    volatile td_u32 l_addr;
    volatile td_u32 h_addr;
    td_phys_addr_t addr;

    if (g_base_vir_addr == TD_NULL) {
        tde_error("null pointer\n");
        return 0;
    }

    l_addr = tde_read_reg(g_base_vir_addr, TDE_AQ_ADDR_LOW);
    h_addr = tde_read_reg(g_base_vir_addr, TDE_AQ_ADDR_HI);
    addr = ((td_u64)h_addr << 32) | l_addr; /* 32 alg data */
    return addr;
}

/*
 * Function:      tde_hal_getbpp_by_fmt
 * Description:   get bpp according color of driver
 * Input:         fmt: color type
 * Output:        pitch width
 * Return:        -1: wrong format
 */
static td_s32 tde_hal_getbpp_by_fmt(tde_color_fmt fmt)
{
    if (tde_hal_getbpp_by_fmt16(fmt) == 16) {        /* 16 fmt data */
        return 16;                                   /* 16 fmt data */
    } else if (tde_hal_getbpp_by_fmt24(fmt) == 24) { /* 24 fmt data */
        return 24;                                   /* 24 fmt data */
    } else if (tde_hal_getbpp_by_fmt32(fmt) == 32) { /* 32 fmt data */
        return 32;                                   /* 32 fmt data */
    } else if (tde_hal_getbpp_by_fmt1(fmt) == 1) {
        return 1;
    } else if (tde_hal_getbpp_by_fmt2(fmt) == 2) {   /* 2 fmt data */
        return 2;                                    /* 2 fmt data */
    } else if (tde_hal_getbpp_by_fmt4(fmt) == 4) {   /* 4 fmt data */
        return 4;                                    /* 4 fmt data */
    } else if (tde_hal_getbpp_by_fmt8(fmt) == 8) {   /* 8 fmt data */
        return 8;                                    /* 8 fmt data */
    } else {
        return -1;
    }
}

static td_s32 tde_hal_getbpp_by_fmt16(tde_color_fmt fmt)
{
    switch (fmt) {
        case TDE_DRV_COLOR_FMT_RGB444:
        case TDE_DRV_COLOR_FMT_RGB555:
        case TDE_DRV_COLOR_FMT_RGB565:
        case TDE_DRV_COLOR_FMT_ARGB4444:
        case TDE_DRV_COLOR_FMT_ARGB1555:
        case TDE_DRV_COLOR_FMT_ACLUT88:
        case TDE_DRV_COLOR_FMT_YCBCR422:
        case TDE_DRV_COLOR_FMT_HALFWORD:
        case TDE_DRV_COLOR_FMT_PKGVYUY:
            return 16; /* 16 fmt data */

        default:
            return -1;
    }
}

static td_s32 tde_hal_getbpp_by_fmt24(tde_color_fmt fmt)
{
    switch (fmt) {
        case TDE_DRV_COLOR_FMT_RGB888:
        case TDE_DRV_COLOR_FMT_ARGB8565:
        case TDE_DRV_COLOR_FMT_YCBCR888:
            return 24; /* 24 fmt data */
        default:
            return -1;
    }
}

static td_s32 tde_hal_getbpp_by_fmt32(tde_color_fmt fmt)
{
    switch (fmt) {
        case TDE_DRV_COLOR_FMT_ARGB8888:
        case TDE_DRV_COLOR_FMT_AYCBCR8888:
        case TDE_DRV_COLOR_FMT_RABG8888:
            return 32; /* 32 fmt data */
        default:
            return -1;
    }
}

static td_s32 tde_hal_getbpp_by_fmt1(tde_color_fmt fmt)
{
    switch (fmt) {
        case TDE_DRV_COLOR_FMT_CLUT1:
        case TDE_DRV_COLOR_FMT_A1:
        case TDE_DRV_COLOR_FMT_CLUT1B:
        case TDE_DRV_COLOR_FMT_A1B:
            return 1;
        default:
            return -1;
    }
}

static td_s32 tde_hal_getbpp_by_fmt2(tde_color_fmt fmt)
{
    switch (fmt) {
        case TDE_DRV_COLOR_FMT_CLUT2:
        case TDE_DRV_COLOR_FMT_CLUT2B:
            return 2; /* 2 function return bpp */
        default:
            return -1; /* -1  return failure */
    }
}
static td_s32 tde_hal_getbpp_by_fmt4(tde_color_fmt fmt)
{
    switch (fmt) {
        case TDE_DRV_COLOR_FMT_CLUT4:
        case TDE_DRV_COLOR_FMT_CLUT4B:
            return 4; /* 4 alg data */
        default:
            return -1;
    }
}

static td_s32 tde_hal_getbpp_by_fmt8(tde_color_fmt fmt)
{
    switch (fmt) {
        case TDE_DRV_COLOR_FMT_CLUT8:
        case TDE_DRV_COLOR_FMT_ACLUT44:
        case TDE_DRV_COLOR_FMT_A8:
        case TDE_DRV_COLOR_FMT_BYTE:
            return 8; /* 8 fmt data */
        default:
            return -1;
    }
}

/*
 * Function:      tde_hal_get_resize_para_htable
 * Description:   get index table according the step
 * Input:         step: input step
 * Return:        index table address
 */
static td_u32 tde_hal_get_resize_para_htable(td_u32 step)
{
    td_u32 index;

    if (step < TDE_RESIZE_8X32_AREA_0) {
        index = 0;
    } else if (step == TDE_RESIZE_8X32_AREA_0) {
        index = 1;
    } else if ((step > TDE_RESIZE_8X32_AREA_0) && (step <= TDE_RESIZE_8X32_AREA_1)) {
        index = 2; /* 2 ResizeParaHTable */
    } else if ((step > TDE_RESIZE_8X32_AREA_1) && (step <= TDE_RESIZE_8X32_AREA_2)) {
        index = 3; /* 3 ResizeParaHTable */
    } else if ((step > TDE_RESIZE_8X32_AREA_2) && (step <= TDE_RESIZE_8X32_AREA_3)) {
        index = 4; /* 4 ResizeParaHTable */
    } else if ((step > TDE_RESIZE_8X32_AREA_3) && (step <= TDE_RESIZE_8X32_AREA_4)) {
        index = 5; /* 5 ResizeParaHTable */
    } else {
        index = 6; /* 6 ResizeParaHTable */
    }
    return index;
}

/*
 * Function:      tde_hal_get_resize_para_vtable
 * Description:   get table of parameter for resize
 * Input:         step:input step
 * Return:        address of table
 */
static td_u32 tde_hal_get_resize_para_vtable(td_u32 step)
{
    td_u32 index;

    /* get index table according step, CNcomment: According to the step length to find index table */
    if (step < TDE_RESIZE_PARA_AREA_0) {
        index = 0;
    } else if (step == TDE_RESIZE_PARA_AREA_0) {
        index = 1;
    } else if ((step > TDE_RESIZE_PARA_AREA_0) && (step <= TDE_RESIZE_PARA_AREA_1)) {
        index = 2; /* 2 alg data */
    } else if ((step > TDE_RESIZE_PARA_AREA_1) && (step <= TDE_RESIZE_PARA_AREA_2)) {
        index = 3; /* 3 alg data */
    } else if ((step > TDE_RESIZE_PARA_AREA_2) && (step <= TDE_RESIZE_PARA_AREA_3)) {
        index = 4; /* 4 alg data */
    } else if ((step > TDE_RESIZE_PARA_AREA_3) && (step <= TDE_RESIZE_PARA_AREA_4)) {
        index = 5; /* 5 alg data */
    } else {
        index = 6; /* 6 alg data */
    }
    return index;
}

/*
 * Function:      tde_hal_init_queue
 * Description:   Initialize Aq list,config the operation which is needed
 */
static td_void tde_hal_init_queue(td_void)
{
    if (g_base_vir_addr == TD_NULL) {
        tde_error("null pointer\n");
        return;
    }
  /*
   * write 0 to Aq list start address register
   * CNcomment: 0 will be written Aq list first address register
   */
    tde_write_reg(g_base_vir_addr, TDE_AQ_NADDR_HI, 0);
    tde_write_reg(g_base_vir_addr, TDE_AQ_NADDR_LOW, 0);
}

/*
 * Function:      tde_hal_set_deflicer_level
 * Description:   SetDeflicerLevel
 * Input:         deflicker_level:anti-flicker levels including:auto,low,middle,high
 */
#if (TDE_CAPABILITY & DEFLICKER)
td_s32 tde_hal_set_deflicer_level(drv_tde_deflicker_level deflicker_level)
{
    g_deflicker_level = deflicker_level;
    return TD_SUCCESS;
}

td_s32 tde_hal_get_deflicer_level(drv_tde_deflicker_level *deflicker_level)
{
    if (deflicker_level == TD_NULL) {
        tde_error("null pointer\n");
        return TD_FAILURE;
    }
    *deflicker_level = g_deflicker_level;
    return TD_SUCCESS;
}
#endif

td_s32 tde_hal_set_alpha_threshold(td_u8 alpha_threshold_value)
{
    g_alpha_threshold_value = alpha_threshold_value;

    return TD_SUCCESS;
}

td_s32 tde_hal_get_alpha_threshold(td_u8 *alpha_threshold_value)
{
    if (alpha_threshold_value == TD_NULL) {
        tde_error("null pointer\n");
        return TD_FAILURE;
    }
    *alpha_threshold_value = g_alpha_threshold_value;

    return TD_SUCCESS;
}

td_s32 tde_hal_set_alpha_threshold_state(td_bool alpha_threshold_en)
{
    g_alpha_threshold = alpha_threshold_en;

    return TD_SUCCESS;
}

td_s32 tde_hal_get_alpha_threshold_state(td_bool *alpha_threshold_en)
{
    if (alpha_threshold_en == TD_NULL) {
        tde_error("null pointer\n");
        return TD_FAILURE;
    }
    *alpha_threshold_en = g_alpha_threshold;

    return TD_SUCCESS;
}

#ifdef CONFIG_OT_PROC_SHOW_SUPPORT
/* see define of tde_hw_node */
td_u8 *g_update[] = {
    "src1_ctrl            ", /* 0x0000 */
    "src1_ch0_addr_high   ", /* 0x0004 */
    "src1_ch0_addr_low    ", /* 0x0008 */
    "src1_ch1_addr_high   ", /* 0x000c */
    "src1_ch1_addr_low    ", /* 0x0010 */
    "src1_ch0_stride      ", /* 0x0014 */
    "src1_ch1_stride      ", /* 0x0018 */
    "src1_imgsize         ", /* 0x001c */
    "src1_fill            ", /* 0x0020 */
    "src1_alpha           ", /* 0x0024 */
    "src1_pix_offset      ", /* 0x0028 */
    "src2_ctrl            ", /* 0x002c */
    "src2_ch0_addr_high   ", /* 0x0030 */
    "src2_ch0_addr_low    ", /* 0x0034 */
    "src2_ch1_addr_high   ", /* 0x0038 */
    "src2_ch1_addr_low    ", /* 0x003c */
    "src2_ch0_stride      ", /* 0x0040 */
    "src2_ch1_stride      ", /* 0x0044 */
    "src2_imgsize         ", /* 0x0048 */
    "src2_fill            ", /* 0x004c */
    "src2_alpha           ", /* 0x0050 */
    "src2_pix_offset      ", /* 0x0054 */
    "des_ctrl             ", /* 0x00f8 */
    "des_ch0_addr_high    ", /* 0x00fc */
    "des_ch0_addr_low     ", /* 0x0100 */
    "des_ch1_addr_high    ", /* 0x0104 */
    "des_ch1_addr_low     ", /* 0x0108 */
    "des_ch0_stride       ", /* 0x010c */
    "des_ch1_stride       ", /* 0x0110 */
    "des_imgsize          ", /* 0x0114 */
    "des_alpha            ", /* 0x0118 */
    "des_crop_pos_st      ", /* 0x011c */
    "des_crop_pos_ed      ", /* 0x0120 */
    "des_pix_offset       ", /* 0x0124 */
    "src1_hsp             ", /* 0x200 */
    "src1_hloffset        ", /* 0x204 */
    "src1_hcoffset        ", /* 0x208 */
    "src1_vsp             ", /* 0x20c */
    "src1_vsr             ", /* 0x210 */
    "src1_voffset         ", /* 0x214 */
    "src1_zmeoreso        ", /* 0x218 */
    "src1_zmeireso        ", /* 0x21c */
    "src1_hpzme           ", /* 0x228 */
    "src1_hpzme_size      ", /* 0x22c */
    "src1_csc_idc0        ", /* 0x230 */
    "src1_csc_idc1        ", /* 0x234 */
    "src1_csc_odc0        ", /* 0x238 */
    "src1_csc_odc1        ", /* 0x23c */
    "src1_csc_p0          ", /* 0x240 */
    "src1_csc_p1          ", /* 0x244 */
    "src1_csc_p2          ", /* 0x248 */
    "src1_csc_p3          ", /* 0x24c */
    "src1_csc_p4          ", /* 0x250 */
    "src1_dither_ctrl     ", /* 0x254 */
    "src2_hsp             ", /* 0x280 */
    "src2_hloffset        ", /* 0x284 */
    "src2_hcoffset        ", /* 0x288 */
    "src2_vsp             ", /* 0x28c */
    "src2_vsr             ", /* 0x290 */
    "src2_voffset         ", /* 0x294 */
    "src2_zmeoreso        ", /* 0x298 */
    "src2_zmeireso        ", /* 0x29c */
    "src2_hpzme           ", /* 0x2a8 */
    "src2_hpzme_size      ", /* 0x2ac */
    "src2_csc_mux         ", /* 0x2b0 */
    "des_csc_idc0         ", /* 0x2b4 */
    "des_csc_idc1         ", /* 0x2b8 */
    "des_csc_odc0         ", /* 0x2bc */
    "des_csc_odc1         ", /* 0x2c0 */
    "des_csc_p0           ", /* 0x2c4 */
    "des_csc_p1           ", /* 0x2c8 */
    "des_csc_p2           ", /* 0x2cc */
    "des_csc_p3           ", /* 0x2d0 */
    "des_csc_p4           ", /* 0x2d4 */
    "des_dither_ctrl      ", /* offset 0x2d0 + 0x08 */
    "des_dswn             ", /* 0x2dc */
    "src2_rtt_ctrl        ", /* 0x3f0 */
    "cbmctrl              ", /* 0x300 */
    "cbmbkg               ", /* 0x304 */
    "cbmcolorize          ", /* 0x308 */
    "cbmalupara           ", /* 0x30c */
    "cbmkeypara           ", /* 0x310 */
    "cbmkeymin            ", /* 0x314 */
    "cbmkeymax            ", /* 0x318 */
    "cbmkeymask           ", /* 0x31c */
    "src1_cbmpara         ", /* 0x320 */
    "src1_cbmstpos        ", /* 0x324 */
    "src2_cbmpara         ", /* 0x328 */
    "src2_cbmstpos        ", /* 0x32c */
    "src1_zme_lhaddr_high ", /* 0x480 */
    "src1_zme_lhaddr_low  ", /* 0x484 */
    "src1_zme_lvaddr_high ", /* 0x488 */
    "src1_zme_lvaddr_low  ", /* 0x48c */
    "src1_zme_chaddr_high ", /* 0x490 */
    "src1_zme_chaddr_low  ", /* 0x494 */
    "src1_zme_cvaddr_high ", /* 0x498 */
    "src1_zme_cvaddr_low  ", /* 0x49c */
    "src2_zme_lhaddr_high ", /* 0x4a0 */
    "src2_zme_lhaddr_low  ", /* 0x4a4 */
    "src2_zme_lvaddr_high ", /* 0x4a8 */
    "src2_zme_lvaddr_low  ", /* 0x4ac */
    "src2_zme_chaddr_high ", /* 0x4b0 */
    "src2_zme_chaddr_low  ", /* 0x4b4 */
    "src2_zme_cvaddr_high ", /* 0x4b8 */
    "src2_zme_cvaddr_low  ", /* 0x4bc */
    "clut_addr_high       ", /* 0x4c0 */
    "clut_addr_low        ", /* 0x4c4 */
};

static td_void tde_hal_node_print_31d(osal_proc_entry_t *p, const td_u32 *cur_node)
{
    td_s32 i;
    /* 22 SRC1_CTRL 0x0 */
    for (i = 0; i < 22; i++) {
        osal_seq_printf(p, "(%s):\t0x%08x\n", g_update[i], *(cur_node + i));
    }
    /* 12 DES_CTRL  0x0f8 */
    for (i = 0; i < 12; i++) {
        osal_seq_printf(p, "(%s):\t0x%08x\n", g_update[i + 22], *(cur_node + i + 62)); /* 22 62 alg data */
    }
    /* 8 SRC1_HSP 0x200 */
    for (i = 0; i < 8; i++) {
        osal_seq_printf(p, "(%s):\t0x%08x\n", g_update[i + 34], *(cur_node + i + 128)); /* 34 128 alg data */
    }
    /* 12 SRC1_HPZME 0x228 */
    for (i = 0; i < 12; i++) {
        osal_seq_printf(p, "(%s):\t0x%08x\n", g_update[i + 42], *(cur_node + i + 138)); /* 42 138 alg data */
    }
    /* 8 SRC2_HSP 0x280 */
    for (i = 0; i < 8; i++) {
        osal_seq_printf(p, "(%s):\t0x%08x\n", g_update[i + 54], *(cur_node + i + 160)); /* 54 160 alg data */
    }
    /* 14 SRC2_HPZME 0x2a8 */
    for (i = 0; i < 14; i++) {
        osal_seq_printf(p, "(%s):\t0x%08x\n", g_update[i + 62], *(cur_node + i + 170)); /* 62 170 alg data */
    }
    /* SRC2_RTT_CTRL 0x3f0 */
    for (i = 0; i < 1; i++) {
        osal_seq_printf(p, "(%s):\t0x%08x\n", g_update[i + 76], *(cur_node + i + 252)); /* 76 252 alg data */
    }
    /* 12 CBMCTRL 0x300 */
    for (i = 0; i < 12; i++) {
        osal_seq_printf(p, "(%s):\t0x%08x\n", g_update[i + 77], *(cur_node + i + 192)); /* 77 192 alg data */
    }
    /* 18 TDE_SRC1_ZME_LHADDR 0x480 */
    for (i = 0; i < 18; i++) {
        osal_seq_printf(p, "(%s):\t0x%08x\n", g_update[i + 89], *(cur_node + i + 288)); /* 89 288 alg data */
    }
}

osal_proc_entry_t *tde_hal_node_print_info(osal_proc_entry_t *p, const td_u32 *cur_node)
{
    if (p == TD_NULL) {
        return 0;
    }
    /* print node information */
    osal_seq_printf(p, "\n--------- tde node params info ---------\n");

    tde_hal_node_print_31d(p, cur_node);

    return p;
}
#endif

#endif /* TDE_HAL_K_BODY_INC */
