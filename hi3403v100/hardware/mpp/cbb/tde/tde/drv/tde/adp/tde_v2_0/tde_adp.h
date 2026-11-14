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

#ifndef TDE_ADP_H
#define TDE_ADP_H

#include "tde_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define SIZE_256BYTE_ALIGN

#define TDE_NODE_HEAD_BYTE 16

#define TDE_NODE_TAIL_BYTE 12

#ifndef SIZE_256BYTE_ALIGN
#define CMD_SIZE    64
#define JOB_SIZE    96
#define NODE_SIZE   208
#define FILTER_SIZE 960
#else
#define CMD_SIZE    64
#define JOB_SIZE    96
#define NODE_SIZE   ((((sizeof(tde_hw_node)) + (TDE_NODE_HEAD_BYTE) + (TDE_NODE_TAIL_BYTE)) + (0x0F)) & (~0x0F))
#define FILTER_SIZE 1792
#endif

#define OT_TDE_FILTER_NUM 3

#define TDE_INTNUM       207
#define TDE_REG_BASEADDR 0x17280000
#define TDE_REG_CLOCK    0x11019d40

#define TDE_CTRL         0x0500
#define TDE_INT          0x0504
#define TDE_INTCLR       0x0508
#define TDE_AQ_NADDR_LOW 0x04fc
#define TDE_AQ_NADDR_HI  0x04f8

#define TDE_STA 0x4000

#define TDE_AQ_ADDR_LOW  0x4098
#define TDE_AQ_ADDR_HI  0x4094

#ifdef CONFIG_TDE_ZME_LINE_BUFFER2048
#define MAX_LINE_BUFFER  2048
#else
#define MAX_LINE_BUFFER  1920
#endif

#define TDE_MISCELLANEOUS 0x0514

#ifndef __LITEOS__
#define TDE_IRQ_NAME "tde_osr_isr"
#else
#define TDE_IRQ_NAME "tde"
#endif

#ifndef __RTOS__
#define DESCRIPTION "TDE Device driver"
#define AUTHOR      "Digital Media Team."
#define TDE_VERSION "V1.0.0.0"
#endif

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 src1_fmt : 6;         /* [5..0]  */
        td_u32 src1_argb_order : 5;  /* [10..6]  */
        td_u32 src1_cbcr_order : 1;  /* [11]  */
        td_u32 src1_rgb_exp : 2;     /* [13..12]  */
        td_u32 reserved_0 : 1;       /* [14]  */
        td_u32 src1_rgb_mode : 1;    /* [15]  */
        td_u32 reserved_1 : 2;       /* [17..16]  */
        td_u32 src1_alpha_range : 1; /* [18]  */
        td_u32 src1_v_scan_ord : 1;  /* [19]  */
        td_u32 src1_h_scan_ord : 1;  /* [20]  */
        td_u32 src1_422v_pro : 1;    /* [21]  */
        td_u32 reserved_2 : 7;       /* [28..22]  */
        td_u32 src1_dma : 1;         /* [29]  */
        td_u32 src1_mode : 1;        /* [30]  */
        td_u32 src1_en : 1;          /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_ctrl;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 src1_ch0_addr_high : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_ch0_addr_high;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 src1_ch0_addr_low : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_ch0_addr_low;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 src1_ch1_addr_high : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_ch1_addr_high;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 src1_ch1_addr_low : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_ch1_addr_low;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 src1_ch0_stride : 20; /* [19..0]  */
        td_u32 reserved : 12;        /* [31..20]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_ch0_stride;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 src1_ch1_stride : 20; /* [19..0]  */
        td_u32 reserved : 12;        /* [31..20]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_ch1_stride;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 src1_width : 16;  /* [15..0]  */
        td_u32 src1_height : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_imgsize;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 src1_color_fill : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_fill;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 src1_alpha0 : 8; /* [7..0]  */
        td_u32 src1_alpha1 : 8; /* [15..8]  */
        td_u32 reserved_0 : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_alpha;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 src1_hoffset_pix : 16; /* [15..0]  */
        td_u32 reserved_0 : 16;       /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_pix_offset;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 src2_fmt : 6;           /* [5..0]  */
        td_u32 src2_argb_order : 5;    /* [10..6]  */
        td_u32 src2_cbcr_order : 1;    /* [11]  */
        td_u32 src2_rgb_exp : 2;       /* [13..12]  */
        td_u32 src2_clut_mode : 1;     /* [14]  */
        td_u32 src2_rgb_mode : 1;      /* [15]  */
        td_u32 reserved_0 : 2;         /* [17..16]  */
        td_u32 src2_alpha_range : 1;   /* [18]  */
        td_u32 src2_v_scan_ord : 1;    /* [19]  */
        td_u32 src2_h_scan_ord : 1;    /* [20]  */
        td_u32 src2_422v_pro : 1;      /* [21]  */
        td_u32 src2_dcmp_en : 1;       /* [22]  */
        td_u32 src2_is_lossless : 1;   /* [23]  */
        td_u32 src2_is_lossless_a : 1; /* [24]  */
        td_u32 src2_cmp_mode : 1;      /* [25]  */
        td_u32 src2_top_pred_en : 1;   /* [26]  */
        td_u32 reserved_1 : 2;         /* [28..27]  */
        td_u32 src2_mode : 2;          /* [30..29]  */
        td_u32 src2_en : 1;            /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_ctrl;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 des_start_x : 16; /* [15..0]  */
        td_u32 des_start_y : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_des_sur_xy;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 src2_ch0_addr_high : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_ch0_addr_high;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 src2_ch0_addr_low : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_ch0_addr_low;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 src2_ch1_addr_high : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_ch1_addr_high;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 src2_ch1_addr_low : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_ch1_addr_low;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 src2_ch0_stride : 20; /* [19..0]  */
        td_u32 reserved : 12;        /* [31..20]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_ch0_stride;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 src2_ch1_stride : 20; /* [19..0]  */
        td_u32 reserved : 12;        /* [31..20]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_ch1_stride;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 src2_width : 16;  /* [15..0]  */
        td_u32 src2_height : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_imgsize;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 src2_color_fill : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_fill;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 src2_alpha0 : 8; /* [7..0]  */
        td_u32 src2_alpha1 : 8; /* [15..8]  */
        td_u32 reserved_0 : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_alpha;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 src2_hoffset_pix : 16; /* [15..0]  */
        td_u32 reserved_0 : 16;       /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_pix_offset;

typedef union {
    struct {
        td_u32 x : 12;       /* First X coordinate */
        td_u32 reserve1 : 4; /* Reserve */
        td_u32 y : 12;       /* First Y coordinate */
        td_u32 reserve2 : 4; /* Reserve */
    } bits;
    td_u32 all;
} u_sur_xy;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_fmt : 6;         /* [5..0]  */
        unsigned int des_argb_order : 5;  /* [10..6]  */
        unsigned int des_cbcr_order : 1;  /* [11]  */
        unsigned int des_bind_en : 1;     /* [12]  */
        unsigned int des_bind_mode : 1;   /* [13]  */
        unsigned int reserved_0 : 3;      /* [16..14]  */
        unsigned int des_rgb_round : 1;   /* [17]  */
        unsigned int des_alpha_range : 1; /* [18]  */
        unsigned int des_v_scan_ord : 1;  /* [19]  */
        unsigned int des_h_scan_ord : 1;  /* [20]  */
        unsigned int reserved_1 : 2;      /* [22..21]  */
        unsigned int cmp_en : 1;          /* [23]  */
        unsigned int cmp_addr_chg : 1;    /* [24]  */
        unsigned int reserved_2 : 6;      /* [30..25]  */
        unsigned int des_en : 1;          /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_des_ctrl;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 head_ar_addr_hi : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_head_ar_addr_hi;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 head_ar_addr_low : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_head_ar_addr_low;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 head_gb_addr_hi : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_head_gb_addr_hi;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 head_gb_addr_low : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_head_gb_addr_low;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 des_ch0_addr_high : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_des_ch0_addr_high;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 des_ch0_addr_low : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_des_ch0_addr_low;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 des_ch1_addr_hi : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_des_ch1_addr_high;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 des_ch1_addr_low : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_des_ch1_addr_low;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 des_ch0_stride : 20; /* [19..0]  */
        td_u32 reserved : 12;       /* [31..20]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_des_ch0_stride;
typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 des_ch1_stride : 20; /* [19..0]  */
        td_u32 reserved : 12;       /* [31..20]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_des_ch1_stride;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 des_width : 16;  /* [15..0]  */
        td_u32 des_height : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_des_imgsize;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 des_alpha_thd : 8; /* [7..0]  */
        td_u32 des_crop_mode : 1; /* [8]  */
        td_u32 des_crop_en : 1;   /* [9]  */
        td_u32 reserved_0 : 22;   /* [31..10]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_des_alpha;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 des_crop_start_x : 16; /* [15..0]  */
        td_u32 des_crop_start_y : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_des_crop_pos_st;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 des_crop_end_x : 16; /* [15..0]  */
        td_u32 des_crop_end_y : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_des_crop_pos_ed;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 src1_ch0_mmu_bypass : 1; /* [0]  */
        td_u32 src1_ch1_mmu_bypass : 1; /* [1]  */
        td_u32 src2_ch0_mmu_bypass : 1; /* [2]  */
        td_u32 src2_ch1_mmu_bypass : 1; /* [3]  */
        td_u32 gdc3_mmu_bypass : 1;     /* [4]  */
        td_u32 gdc4_mmu_bypass : 1;     /* [5]  */
        td_u32 gdc5_mmu_bypass : 1;     /* [6]  */
        td_u32 gdc6_mmu_bypass : 1;     /* [7]  */
        td_u32 gdc7_mmu_bypass : 1;     /* [8]  */
        td_u32 clut_mmu_bypass : 1;     /* [9]  */
        td_u32 reserved_0 : 6;          /* [15..10]  */
        td_u32 src1_ch0_prot : 1;       /* [16]  */
        td_u32 src1_ch1_prot : 1;       /* [17]  */
        td_u32 src2_ch0_prot : 1;       /* [18]  */
        td_u32 src2_ch1_prot : 1;       /* [19]  */
        td_u32 gdc3_prot : 1;           /* [20]  */
        td_u32 gdc4_prot : 1;           /* [21]  */
        td_u32 gdc5_prot : 1;           /* [22]  */
        td_u32 gdc6_prot : 1;           /* [23]  */
        td_u32 gdc7_prot : 1;           /* [24]  */
        td_u32 reserved_1 : 7;          /* [31..25]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_mmu_prot_ctrl;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 des_hoffset_pix : 16; /* [15..0]  */
        td_u32 reserved_0 : 16;      /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_des_pix_offset;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 dma_corner_width : 16; /* [15..0]  */
        td_u32 dma_corner_height : 16;      /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_tde_dma_corner_reso;

#ifdef CONFIG_TDE_DMA_CORNER_V1
typedef union {
    /* Define the struct bits */
    struct {
        td_u32 dma_corner_value : 4; /* [3..0]  */
        td_u32 dma_other_value : 4;      /* [7..4]  */
        td_u32 reserved_0 : 22;      /* [29..8]  */
        td_u32 dma_des_en : 1;      /* [30]  */
        td_u32 dma_corner_en : 1;      /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_tde_dma_corner_ctrl;
#endif

#ifdef CONFIG_TDE_DMA_CORNER_V2
typedef union {
    /* Define the struct bits */
    struct {
        td_u32 reserved : 30;      /* [29..0]  */
        td_u32 dma_des_en : 1;      /* [30]  */
        td_u32 dma_corner_en : 1;      /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_tde_dma_corner_ctrl;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 dma_corner_value : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_tde_dma_corner_value;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 dma_other_value : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_tde_dma_other_value;
#endif

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 des_safe_dist : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_des_safe_dist;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 des_safe_dist_inverse : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_des_safe_dist_inverse;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 des_bind_buffer_size : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_des_bind_buffer_size;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 hratio : 24;    /* [23..0]  */
        td_u32 hfir_order : 1; /* [24]  */
        td_u32 hchfir_en : 1;  /* [25]  */
        td_u32 hlfir_en : 1;   /* [26]  */
        td_u32 hafir_en : 1;   /* [27]  */
        td_u32 hchmid_en : 1;  /* [28]  */
        td_u32 hlmid_en : 1;   /* [29]  */
        td_u32 hchmsc_en : 1;  /* [30]  */
        td_u32 hlmsc_en : 1;   /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_hsp;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 hor_loffset : 28; /* [27..0]  */
        td_u32 reserved_0 : 4;   /* [31..28]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_hloffset;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 hor_coffset : 28; /* [27..0]  */
        td_u32 reserved_0 : 4;   /* [31..28]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_hcoffset;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 reserved_0 : 19;    /* [18..0]  */
        td_u32 zme_in_fmt : 2;     /* [20..19]  */
        td_u32 zme_out_fmt : 2;    /* [22..21]  */
        td_u32 vchfir_en : 1;      /* [23]  */
        td_u32 vlfir_en : 1;       /* [24]  */
        td_u32 vafir_en : 1;       /* [25]  */
        td_u32 vsc_chroma_tap : 1; /* [26]  */
        td_u32 reserved_1 : 1;     /* [27]  */
        td_u32 vchmid_en : 1;      /* [28]  */
        td_u32 vlmid_en : 1;       /* [29]  */
        td_u32 vchmsc_en : 1;      /* [30]  */
        td_u32 vlmsc_en : 1;       /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_vsp;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 vratio : 16;     /* [15..0]  */
        td_u32 reserved_0 : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_vsr;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 vchroma_offset : 16; /* [15..0]  */
        td_u32 vluma_offset : 16;   /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_voffset;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 ow : 16; /* [15..0]  */
        td_u32 oh : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_zmeoreso;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 iw : 16; /* [15..0]  */
        td_u32 ih : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_zmeireso;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 src1_crop_x : 13; /* [12..0]  */
        td_u32 reserved_0 : 3;   /* [15..13]  */
        td_u32 src1_crop_y : 13; /* [28..16]  */
        td_u32 reserved_1 : 2;   /* [30..29]  */
        td_u32 src1_crop_en : 1; /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_crop_pos;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 src1_crop_width : 13;  /* [12..0]  */
        td_u32 reserved_0 : 3;        /* [15..13]  */
        td_u32 src1_crop_height : 13; /* [28..16]  */
        td_u32 reserved_1 : 3;        /* [31..29]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_crop_size;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 src1_hpzme_en : 1;   /* [0]  */
        td_u32 reserved_0 : 3;      /* [3..1]  */
        td_u32 src1_hpzme_mode : 4; /* [7..4]  */
        td_u32 reserved_1 : 24;     /* [31..8]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_hpzme;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 src1_hpzme_width : 16; /* [15..0]  */
        td_u32 reserved_0 : 16;       /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_hpzme_size;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 cscidc0 : 11;   /* [10..0]  */
        td_u32 reserved_0 : 5; /* [15..11]  */
        td_u32 cscidc1 : 11;   /* [26..16]  */
        td_u32 reserved_1 : 4; /* [30..27]  */
        td_u32 csc_en : 1;     /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_csc_idc0;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 cscidc2 : 11;    /* [10..0]  */
        td_u32 reserved_0 : 20; /* [31..11]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_csc_idc1;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 cscodc0 : 11;   /* [10..0]  */
        td_u32 reserved_0 : 5; /* [15..11]  */
        td_u32 cscodc1 : 11;   /* [26..16]  */
        td_u32 reserved_1 : 5; /* [31..27]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_csc_odc0;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 cscodc2 : 11;    /* [10..0]  */
        td_u32 reserved_0 : 20; /* [31..11]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_csc_odc1;

/* Define the union U_SRC1_CSC_P0 */
typedef union {
    /* Define the struct bits */
    struct {
        td_u32 cscp00 : 15;    /* [14..0]  */
        td_u32 reserved_0 : 1; /* [15]  */
        td_u32 cscp01 : 15;    /* [30..16]  */
        td_u32 reserved_1 : 1; /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_csc_p0;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 cscp02 : 15;    /* [14..0]  */
        td_u32 reserved_0 : 1; /* [15]  */
        td_u32 cscp10 : 15;    /* [30..16]  */
        td_u32 reserved_1 : 1; /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_csc_p1;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 cscp11 : 15;    /* [14..0]  */
        td_u32 reserved_0 : 1; /* [15]  */
        td_u32 cscp12 : 15;    /* [30..16]  */
        td_u32 reserved_1 : 1; /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_csc_p2;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 cscp20 : 15;    /* [14..0]  */
        td_u32 reserved_0 : 1; /* [15]  */
        td_u32 cscp21 : 15;    /* [30..16]  */
        td_u32 reserved_1 : 1; /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_csc_p3;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 cscp22 : 15;     /* [14..0]  */
        td_u32 reserved_0 : 17; /* [31..15]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_csc_p4;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 reserved_0 : 29;  /* [28..0]  */
        td_u32 dither_round : 1; /* [29]  */
        td_u32 reserved_1 : 1;   /* [30]  */
        td_u32 dither_en : 1;    /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_dither_ctrl;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 hratio : 24;    /* [23..0]  */
        td_u32 hfir_order : 1; /* [24]  */
        td_u32 hchfir_en : 1;  /* [25]  */
        td_u32 hlfir_en : 1;   /* [26]  */
        td_u32 hafir_en : 1;   /* [27]  */
        td_u32 hchmid_en : 1;  /* [28]  */
        td_u32 hlmid_en : 1;   /* [29]  */
        td_u32 hchmsc_en : 1;  /* [30]  */
        td_u32 hlmsc_en : 1;   /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_hsp;

typedef union {
    /* Define the struct bits */
    struct {
        td_s32 hor_loffset : 28; /* [27..0]  */
        td_u32 reserved_0 : 4;   /* [31..28]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_hloffset;

typedef union {
    /* Define the struct bits */
    struct {
        td_s32 hor_coffset : 28; /* [27..0]  */
        td_u32 reserved_0 : 4;   /* [31..28]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_hcoffset;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 reserved_0 : 19;    /* [18..0]  */
        td_u32 zme_in_fmt : 2;     /* [20..19]  */
        td_u32 zme_out_fmt : 2;    /* [22..21]  */
        td_u32 vchfir_en : 1;      /* [23]  */
        td_u32 vlfir_en : 1;       /* [24]  */
        td_u32 vafir_en : 1;       /* [25]  */
        td_u32 vsc_chroma_tap : 1; /* [26]  */
        td_u32 reserved_1 : 1;     /* [27]  */
        td_u32 vchmid_en : 1;      /* [28]  */
        td_u32 vlmid_en : 1;       /* [29]  */
        td_u32 vchmsc_en : 1;      /* [30]  */
        td_u32 vlmsc_en : 1;       /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_vsp;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 vratio : 16;     /* [15..0]  */
        td_u32 reserved_0 : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_vsr;

typedef union {
    /* Define the struct bits */
    struct {
        td_s32 vchroma_offset : 16; /* [15..0]  */
        td_s32 vluma_offset : 16;   /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_voffset;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 ow : 16; /* [15..0]  */
        td_u32 oh : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_zmeoreso;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 iw : 16; /* [15..0]  */
        td_u32 ih : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_zmeireso;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 src2_crop_x : 13; /* [12..0]  */
        td_u32 reserved_0 : 3;   /* [15..13]  */
        td_u32 src2_crop_y : 13; /* [28..16]  */
        td_u32 reserved_1 : 2;   /* [30..29]  */
        td_u32 src2_crop_en : 1; /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_crop_pos;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 src2_crop_width : 13;  /* [12..0]  */
        td_u32 reserved_0 : 3;        /* [15..13]  */
        td_u32 src2_crop_height : 13; /* [28..16]  */
        td_u32 reserved_1 : 3;        /* [31..29]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_crop_size;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 src2_hpzme_en : 1;   /* [0]  */
        td_u32 reserved_0 : 3;      /* [3..1]  */
        td_u32 src2_hpzme_mode : 4; /* [7..4]  */
        td_u32 reserved_1 : 24;     /* [31..8]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_hpzme;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 src2_hpzme_width : 16; /* [15..0]  */
        td_u32 reserved_0 : 16;       /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_hpzme_size;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 src2_csc_mode : 1; /* [0]  */
        td_u32 des_premulten : 1; /* [1]  */
        td_u32 src_premulten : 1; /* [2]  */
        td_u32 reserved_0 : 29;   /* [31..3]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_csc_mux;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 cscidc0 : 11;   /* [10..0]  */
        td_u32 reserved_0 : 5; /* [15..11]  */
        td_u32 cscidc1 : 11;   /* [26..16]  */
        td_u32 reserved_1 : 4; /* [30..27]  */
        td_u32 csc_en : 1;     /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_des_csc_idc0;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 cscidc2 : 11;    /* [10..0]  */
        td_u32 reserved_0 : 20; /* [31..11]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_des_csc_idc1;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 cscodc0 : 11;   /* [10..0]  */
        td_u32 reserved_0 : 5; /* [15..11]  */
        td_u32 cscodc1 : 11;   /* [26..16]  */
        td_u32 reserved_1 : 5; /* [31..27]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_des_csc_odc0;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 cscodc2 : 11;    /* [10..0]  */
        td_u32 reserved_0 : 20; /* [31..11]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_des_csc_odc1;

/* Define the union U_DES_CSC_P0 */
typedef union {
    /* Define the struct bits */
    struct {
        td_u32 cscp00 : 15;    /* [14..0]  */
        td_u32 reserved_0 : 1; /* [15]  */
        td_u32 cscp01 : 15;    /* [30..16]  */
        td_u32 reserved_1 : 1; /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_des_csc_p0;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 cscp02 : 15;    /* [14..0]  */
        td_u32 reserved_0 : 1; /* [15]  */
        td_u32 cscp10 : 15;    /* [30..16]  */
        td_u32 reserved_1 : 1; /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_des_csc_p1;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 cscp11 : 15;    /* [14..0]  */
        td_u32 reserved_0 : 1; /* [15]  */
        td_u32 cscp12 : 15;    /* [30..16]  */
        td_u32 reserved_1 : 1; /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_des_csc_p2;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 cscp20 : 15;    /* [14..0]  */
        td_u32 reserved_0 : 1; /* [15]  */
        td_u32 cscp21 : 15;    /* [30..16]  */
        td_u32 reserved_1 : 1; /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_des_csc_p3;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 cscp22 : 15;     /* [14..0]  */
        td_u32 reserved_0 : 17; /* [31..15]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_des_csc_p4;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 reserved_0 : 29;  /* [28..0]  */
        td_u32 dither_round : 1; /* [29]  */
        td_u32 reserved_1 : 1;   /* [30]  */
        td_u32 dither_en : 1;    /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_des_dither_ctrl;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 des_h_dswm_mode : 2;        /* [1..0]  */
        td_u32 reserved_0 : 2;             /* [3..2]  */
        td_u32 des_v_dswm_mode : 1;        /* [4]  */
        td_u32 des_alpha_detect_clear : 1; /* [5]  */
        td_u32 reserved_1 : 26;            /* [31..6]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_des_dswm;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 reserved_0 : 8;           /* [7..0]  */
        td_u32 src1_rcopy_pixel_num : 8; /* [15..8]  */
        td_u32 reserved_1 : 14;          /* [29..16]  */
        td_u32 src1_rcopy_en : 1;        /* [30]  */
        td_u32 src1_copy_cfg_from : 1;   /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_befor_zme_copy;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 reserved_0 : 8;           /* [7..0]  */
        td_u32 src1_rcrop_pixel_num : 8; /* [15..8]  */
        td_u32 reserved_1 : 14;          /* [29..16]  */
        td_u32 src1_rcrop_en : 1;        /* [30]  */
        td_u32 src1_crop_cfg_from : 1;   /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_after_zme_crop;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 reserved_0 : 8;           /* [7..0]  */
        td_u32 src2_rcopy_pixel_num : 8; /* [15..8]  */
        td_u32 reserved_1 : 14;          /* [29..16]  */
        td_u32 src2_rcopy_en : 1;        /* [30]  */
        td_u32 src2_copy_cfg_from : 1;   /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_befor_zme_copy;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 reserved_0 : 8;           /* [7..0]  */
        td_u32 src2_rcrop_pixel_num : 8; /* [15..8]  */
        td_u32 reserved_1 : 14;          /* [29..16]  */
        td_u32 src2_rcrop_en : 1;        /* [30]  */
        td_u32 src2_crop_cfg_from : 1;   /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_after_zme_crop;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 mix_prio0 : 3;  /* [2..0]  */
        td_u32 mix_prio1 : 3;  /* [5..3]  */
        td_u32 mix_prio2 : 3;  /* [8..6]  */
        td_u32 mix_prio3 : 3;  /* [11..9]  */
        td_u32 mix_prio4 : 3;  /* [14..12]  */
        td_u32 mix_prio5 : 3;  /* [17..15]  */
        td_u32 mix_prio6 : 3;  /* [20..18]  */
        td_u32 alu_mode : 4;   /* [24..21]  */
        td_u32 cbm_mode : 1;   /* [25]  */
        td_u32 reserved_0 : 5; /* [30..26]  */
        td_u32 cbm_en : 1;     /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_cbmctrl;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 bkgb : 8; /* [7..0]  */
        td_u32 bkgg : 8; /* [15..8]  */
        td_u32 bkgr : 8; /* [23..16]  */
        td_u32 bkga : 8; /* [31..24]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_cbmbkg;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 colorizeb : 8;  /* [7..0]  */
        td_u32 colorizeg : 8;  /* [15..8]  */
        td_u32 colorizer : 8;  /* [23..16]  */
        td_u32 reserved_0 : 7; /* [30..24]  */
        td_u32 colorizeen : 1; /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_cbmcolorize;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 rgb_rop : 4;         /* [3..0]  */
        td_u32 a_rop : 4;           /* [7..4]  */
        td_u32 reserved_0 : 8;      /* [15..8]  */
        td_u32 alpha_from : 2;      /* [17..16]  */
        td_u32 alpha_border_en : 2; /* [19..18]  */
        td_u32 reserved_1 : 11;     /* [30..20]  */
        td_u32 blendropen : 1;      /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_cbmalupara;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 keybmode : 2;    /* [1..0]  */
        td_u32 keygmode : 2;    /* [3..2]  */
        td_u32 keyrmode : 2;    /* [5..4]  */
        td_u32 keyamode : 2;    /* [7..6]  */
        td_u32 keysel : 2;      /* [9..8]  */
        td_u32 reserved_0 : 21; /* [30..10]  */
        td_u32 keyen : 1;       /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_cbmkeypara;

typedef union {
    /* Define the struct bits */
    struct {
        td_u8 keybmin : 8; /* [7..0]  */
        td_u8 keygmin : 8; /* [15..8]  */
        td_u8 keyrmin : 8; /* [23..16]  */
        td_u8 keyamin : 8; /* [31..24]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_cbmkeymin;

typedef union {
    /* Define the struct bits */
    struct {
        td_u8 keybmax : 8; /* [7..0]  */
        td_u8 keygmax : 8; /* [15..8]  */
        td_u8 keyrmax : 8; /* [23..16]  */
        td_u8 keyamax : 8; /* [31..24]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_cbmkeymax;

typedef union {
    /* Define the struct bits */
    struct {
        td_u8 keybmask : 8; /* [7..0]  */
        td_u8 keygmask : 8; /* [15..8]  */
        td_u8 keyrmask : 8; /* [23..16]  */
        td_u8 keyamask : 8; /* [31..24]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_cbmkeymask;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 s1_galphaen : 1;      /* [0]  */
        td_u32 s1_palphaen : 1;      /* [1]  */
        td_u32 s1_premulten : 1;     /* [2]  */
        td_u32 s1_multiglobalen : 1; /* [3]  */
        td_u32 s1_blendmode : 4;     /* [7..4]  */
        td_u32 s1_galpha : 8;        /* [15..8]  */
        td_u32 reserved_0 : 15;      /* [30..16]  */
        td_u32 s1_coverblenden : 1;  /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_cbmpara;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 s1_xfpos : 16; /* [15..0]  */
        td_u32 s1_yfpos : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src1_cbmstpos;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 s2_galphaen : 1;      /* [0]  */
        td_u32 s2_palphaen : 1;      /* [1]  */
        td_u32 s2_premulten : 1;     /* [2]  */
        td_u32 s2_multiglobalen : 1; /* [3]  */
        td_u32 s2_blendmode : 4;     /* [7..4]  */
        td_u32 s2_galpha : 8;        /* [15..8]  */
        td_u32 reserved_0 : 15;      /* [30..16]  */
        td_u32 s2_coverblenden : 1;  /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_cbmpara;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 s2_rtt_en : 1;   /* [0]  */
        td_u32 s2_rtt_dir : 1;  /* [1]  */
        td_u32 s2_rtt_fmt : 2;  /* [2]  */
        td_u32 reserved_0 : 28; /* [31..3]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_rtt_ctrl;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 s2_xfpos : 16; /* [15..0]  */
        td_u32 s2_yfpos : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_src2_cbmstpos;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 src1_scl_lh : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_tde_src1_zme_lhaddr;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 src1_scl_lv : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_tde_src1_zme_lvaddr;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 src1_scl_ch : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_tde_src1_zme_chaddr;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 src1_scl_cv : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_tde_src1_zme_cvaddr;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 src2_scl_lh : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_tde_src2_zme_lhaddr;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 src2_scl_lv : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_tde_src2_zme_lvaddr;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 src2_scl_ch : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_tde_src2_zme_chaddr;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 src2_scl_cv : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_tde_src2_zme_cvaddr;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 clut_addr : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_tde_clut_addr;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 reserved_0 : 20; /* [19..0]  */
        td_u32 awid_cfg0 : 4;   /* [23..20]  */
        td_u32 reserved_1 : 4;  /* [27..24]  */
        td_u32 arid_cfg0 : 4;   /* [31..28]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_tde_axiid;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 nodeid : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_tde_nodeid;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 eof_mask : 1;     /* [0]  */
        td_u32 timeout_mask : 1; /* [1]  */
        td_u32 bus_err_mask : 1; /* [2]  */
        td_u32 eof_end_mask : 1; /* [3]  */
        td_u32 reserved_0 : 28;  /* [31..4]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_tde_intmask;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 p_next_hi : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_tde_pnext_hi;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 p_next_low : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_tde_pnext_low;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 start : 1;       /* [0]  */
        td_u32 reserved_0 : 31; /* [31..1]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_tde_start;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 eof_state : 1;     /* [0]  */
        td_u32 timeout_state : 1; /* [1]  */
        td_u32 bus_err : 1;       /* [2]  */
        td_u32 eof_end_state : 1; /* [3]  */
        td_u32 reserved_0 : 28;   /* [31..4]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_tde_intstate;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 eof_clr : 1;     /* [0]  */
        td_u32 timeout_clr : 1; /* [1]  */
        td_u32 bus_err_clr : 1; /* [2]  */
        td_u32 eof_end_clr : 1; /* [3]  */
        td_u32 reserved_0 : 28; /* [31..4]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_tde_intclr;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 raw_eof : 1;     /* [0]  */
        td_u32 raw_timeout : 1; /* [1]  */
        td_u32 raw_bus_err : 1; /* [2]  */
        td_u32 raw_eof_end : 1; /* [3]  */
        td_u32 reserved_0 : 28; /* [31..4]  */
    } bits;
    /* Define an unsigned member */
    td_u32 all;
} u_tde_rawint;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 pfcnt : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 u32;
} u_tde_pfcnt;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 routstanding : 4;  /* [3..0]  */
        td_u32 woutstanding : 4;  /* [7..4]  */
        td_u32 init_timer : 16;   /* [23..8]  */
        td_u32 ck_gt_en : 1;      /* [24]  */
        td_u32 ck_gt_en_calc : 1; /* [25]  */
        td_u32 split_128b_en : 1; /* [26]  */
        td_u32 split_256b_en : 1; /* [27]  */
        td_u32 split_1k_en : 1;   /* [28]  */
        td_u32 split_2k_en : 1;   /* [29]  */
        td_u32 reserved_0 : 2;    /* [31..30]  */
    } bits;
    /* Define an unsigned member */
    td_u32 u32;
} u_tde_miscellaneous;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 mac_ch_prio : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 u32;
} u_tde_maccfg;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 timeout : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 u32;
} u_tde_timeout;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 eof_cnt : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 u32;
} u_tde_eofcnt;

typedef union {
    /* Define the struct bits */
    struct {
        td_u32 emab : 3;         /* [2..0]  */
        td_u32 emaa : 3;         /* [5..3]  */
        td_u32 emasa : 1;        /* [6]  */
        td_u32 emaw : 2;         /* [8..7]  */
        td_u32 ema : 3;          /* [11..9]  */
        td_u32 rfsuhd_wtsel : 2; /* [13..12]  */
        td_u32 rfsuhd_rtsel : 2; /* [15..14]  */
        td_u32 rfs_wtsel : 2;    /* [17..16]  */
        td_u32 rfs_rtsel : 2;    /* [19..18]  */
        td_u32 rfts_wct : 2;     /* [21..20]  */
        td_u32 rfts_rct : 2;     /* [23..22]  */
        td_u32 rfts_kp : 3;      /* [26..24]  */
        td_u32 rftf_wct : 2;     /* [28..27]  */
        td_u32 rftf_rct : 2;     /* [30..29]  */
        td_u32 reserved : 1;     /* [31]  */
    } bits;
    /* Define an unsigned member */
    td_u32 u32;
} u_tde_memctrl;
#ifdef CONFIG_TDE_GFBG_COMPRESS_V1
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int is_lossless : 1;       /* [0]  */
        unsigned int is_lossless_alpha : 1; /* [1]  */
        unsigned int cmp_mode : 1;          /* [2]  */
        unsigned int osd_mode : 2;          /* [4..3]  */
        unsigned int partition_en : 1;      /* [5]  */
        unsigned int part_num : 3;          /* [8..6]  */
        unsigned int reserved_0 : 23;       /* [31..9]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_od_pic_osd_glb_info;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int frame_width : 13;  /* [12..0]  */
        unsigned int reserved_0 : 3;    /* [15..13]  */
        unsigned int frame_height : 13; /* [28..16]  */
        unsigned int reserved_1 : 3;    /* [31..29]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_od_pic_osd_frame_size;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int budget_bits_mb : 10; /* [9..0]  */
        unsigned int reserved_0 : 6;      /* [15..10]  */
        unsigned int min_mb_bits : 10;    /* [25..16]  */
        unsigned int reserved_1 : 6;      /* [31..26]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_od_pic_osd_rc_cfg0;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int budget_bits_mb_cap : 10; /* [9..0]  */
        unsigned int reserved_0 : 22;         /* [31..10]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_od_pic_osd_rc_cfg1;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int max_qp : 4;            /* [3..0]  */
        unsigned int smth_qp : 4;           /* [7..4]  */
        unsigned int sad_bits_ngain : 4;    /* [11..8]  */
        unsigned int reserved_0 : 4;        /* [15..12]  */
        unsigned int rc_smth_ngain : 3;     /* [18..16]  */
        unsigned int reserved_1 : 5;        /* [23..19]  */
        unsigned int special_bits_gain : 4; /* [27..24]  */
        unsigned int reserved_2 : 4;        /* [31..28]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_od_pic_osd_rc_cfg2;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int max_sad_thr : 7; /* [6..0]  */
        unsigned int reserved_0 : 9;  /* [15..7]  */
        unsigned int min_sad_thr : 7; /* [22..16]  */
        unsigned int reserved_1 : 9;  /* [31..23]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_od_pic_osd_rc_cfg3;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int smth_thr : 7;      /* [6..0]  */
        unsigned int reserved_0 : 1;    /* [7]  */
        unsigned int still_thr : 7;     /* [14..8]  */
        unsigned int reserved_1 : 1;    /* [15]  */
        unsigned int big_grad_thr : 10; /* [25..16]  */
        unsigned int reserved_2 : 6;    /* [31..26]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_od_pic_osd_rc_cfg4;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int smth_pix_num_thr : 6;       /* [5..0]  */
        unsigned int reserved_0 : 2;             /* [7..6]  */
        unsigned int still_pix_num_thr : 6;      /* [13..8]  */
        unsigned int reserved_1 : 2;             /* [15..14]  */
        unsigned int noise_pix_num_thr : 6;      /* [21..16]  */
        unsigned int reserved_2 : 2;             /* [23..22]  */
        unsigned int large_smth_pix_num_thr : 6; /* [29..24]  */
        unsigned int reserved_3 : 2;             /* [31..30]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_od_pic_osd_rc_cfg5;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int noise_sad : 7;    /* [6..0]  */
        unsigned int reserved_0 : 9;   /* [15..7]  */
        unsigned int pix_diff_thr : 9; /* [24..16]  */
        unsigned int reserved_1 : 7;   /* [31..25]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_od_pic_osd_rc_cfg6;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int adj_sad_bits_thr : 7; /* [6..0]  */
        unsigned int reserved_0 : 1;       /* [7]  */
        unsigned int max_trow_bits : 8;    /* [15..8]  */
        unsigned int reserved_1 : 16;      /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_od_pic_osd_rc_cfg7;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qp_inc1_bits_thr : 8; /* [7..0]  */
        unsigned int qp_dec1_bits_thr : 8; /* [15..8]  */
        unsigned int qp_dec2_bits_thr : 8; /* [23..16]  */
        unsigned int qp_dec3_bits_thr : 8; /* [31..24]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_od_pic_osd_rc_cfg8;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int force_qp_thr : 10;     /* [9..0]  */
        unsigned int reserved_0 : 6;        /* [15..10]  */
        unsigned int force_qp_thr_cap : 10; /* [25..16]  */
        unsigned int reserved_1 : 6;        /* [31..26]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_od_pic_osd_rc_cfg9;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int max_row_len : 10; /* [9..0]  */
        unsigned int reserved_0 : 22;  /* [31..10]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_od_pic_osd_rc_cfg10;
#endif /* CONFIG_TDE_GFBG_COMPRESS_V1 */

#ifdef CONFIG_TDE_GFBG_COMPRESS_V2
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ice_en : 1; /* [0]  */
        unsigned int cmp_mode : 1; /* [1]  */
        unsigned int is_lossless : 1; /* [2]  */
        unsigned int conv_en : 1; /* [3]  */
        unsigned int osd_mode : 2; /* [5..4]  */
        unsigned int reserved : 26;       /* [31..6]  */
    } bits;
    /* Define an unsigned member */
    unsigned int all;
} u_tde_line_osd_cmp_glb_info;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int frame_width : 14;  /* [13..0]  */
        unsigned int reserved_0 : 2;    /* [15..14]  */
        unsigned int frame_height : 14; /* [29..16]  */
        unsigned int reserved_1 : 2;    /* [31..30]  */
    } bits;
    /* Define an unsigned member */
    unsigned int all;
} u_tde_line_osd_cmp_frame_size;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int budget_mb_bits : 12; /* [11..0]  */
        unsigned int budget_mb_bits_last : 12; /* [23..12]  */
        unsigned int max_mb_qp : 3; /* [26..24]  */
        unsigned int reserved : 5; /* [31..27]  */
    } bits;
    /* Define an unsigned member */
    unsigned int all;
} u_tde_line_osd_cmp_rc_cfg0;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int smth_thr : 8; /* [7..0]  */
        unsigned int still_thr : 8; /* [15..8]  */
        unsigned int big_grad_thr : 8; /* [23..16]  */
        unsigned int diff_thr : 8; /* [31..24]  */
    } bits;
    /* Define an unsigned member */
    unsigned int all;
} u_tde_line_osd_cmp_rc_cfg1;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int smth_pix_num_thr : 6; /* [5..0]  */
        unsigned int reserved_0 : 2; /* [7..6]  */
        unsigned int still_pix_num_thr : 6; /* [13..8]  */
        unsigned int reserved_1 : 2; /* [15..14]  */
        unsigned int noise_pix_num_thr : 6; /* [21..16]  */
        unsigned int reserved_2 : 2; /* [23..22]  */
        unsigned int raw_bits_penalty : 8; /* [31..24]  */
    } bits;
    /* Define an unsigned member */
    unsigned int all;
} u_tde_line_osd_cmp_rc_cfg2;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qp_inc1_bits_thr : 10; /* [9..0]  */
        unsigned int reserved_0 : 6; /* [15..10]  */
        unsigned int qp_inc2_bits_thr : 10; /* [25..16]  */
        unsigned int reserved_1 : 6; /* [31..26]  */
    } bits;
    /* Define an unsigned member */
    unsigned int all;
} u_tde_line_osd_cmp_rc_cfg3;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qp_dec1_bits_thr : 10; /* [9..0]  */
        unsigned int reserved_0 : 6; /* [15..10]  */
        unsigned int qp_dec2_bits_thr : 10; /* [25..16]  */
        unsigned int reserved_1 : 6; /* [31..26]  */
    } bits;
    /* Define an unsigned member */
    unsigned int all;
} u_tde_line_osd_cmp_rc_cfg4;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int buf_fullness_thr_reg0 : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    unsigned int all;
} u_tde_line_osd_cmp_rc_cfg5;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int buf_fullness_thr_reg1 : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    unsigned int all;
} u_tde_line_osd_cmp_rc_cfg6;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int buf_fullness_thr_reg2 : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    unsigned int all;
} u_tde_line_osd_cmp_rc_cfg7;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qp_rge_reg0 : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    unsigned int all;
} u_tde_line_osd_cmp_rc_cfg8;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qp_rge_reg1 : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    unsigned int all;
} u_tde_line_osd_cmp_rc_cfg9;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qp_rge_reg2 : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    unsigned int all;
} u_tde_line_osd_cmp_rc_cfg10;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bits_offset_reg0 : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    unsigned int all;
} u_tde_line_osd_cmp_rc_cfg11;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bits_offset_reg1 : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    unsigned int all;
} u_tde_line_osd_cmp_rc_cfg12;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bits_offset_reg2 : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    unsigned int all;
} u_tde_line_osd_cmp_rc_cfg13;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int est_err_gain_map : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    unsigned int all;
} u_tde_line_osd_cmp_rc_cfg14;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int buffer_init_bits : 16; /* [15..0]  */
        unsigned int smooth_status_thr : 4; /* [19..16]  */
        unsigned int min_mb_bits : 12; /* [31..20]  */
    } bits;
    /* Define an unsigned member */
    unsigned int all;
} u_tde_line_osd_cmp_rc_cfg15;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int first_col_adj_bits : 8; /* [7..0]  */
        unsigned int first_row_adj_bits : 8; /* [15..8]  */
        unsigned int first_mb_adj_bits : 8; /* [23..16]  */
        unsigned int reserved : 8; /* [31..24]  */
    } bits;
    /* Define an unsigned member */
    unsigned int all;
} u_v4r2_line_osd_cmp_rc_cfg16;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int still_diff_thr : 8; /* [7..0]  */
        unsigned int still_status_thr : 4; /* [11..8]  */
        unsigned int reserved : 20; /* [31..12]  */
    } bits;
    /* Define an unsigned member */
    unsigned int all;
} u_tde_line_osd_cmp_rc_cfg17;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int max_left_bits_buffer : 16; /* [15..0]  */
        unsigned int reserved : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    unsigned int all;
} u_tde_line_osd_cmp_glb_st;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug_info : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    unsigned int all;
} u_tde_line_osd_cmp_dbg_reg;
#endif /* CONFIG_TDE_GFBG_COMPRESS_V2 */

/* Define the union U_TDE_DEBUG0 */
typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 debug0 : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 u32;
} u_tde_debug0;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 debug1 : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 u32;
} u_tde_debug1;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 debug2 : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 u32;
} u_tde_debug2;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 debug3 : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 u32;
} u_tde_debug3;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 debug4 : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 u32;
} u_tde_debug4;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 debug5 : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 u32;
} u_tde_debug5;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 debug6 : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 u32;
} u_tde_debug6;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 debug7 : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 u32;
} u_tde_debug7;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 debug8 : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 u32;
} u_tde_debug8;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 debug9 : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 u32;
} u_tde_debug9;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 debug10 : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 u32;
} u_tde_debug10;

typedef union {
    /* Define the struct bits  */
    struct {
        td_u32 debug11 : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    td_u32 u32;
} u_tde_debug11;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cmp1_frame_size_reg : 22; /* [21..0]  */
        unsigned int reserved_0 : 10;          /* [31..22]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_od_pic_osd_bs_size_cmp1;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cmp1_max_frm_row_len : 16; /* [15..0]  */
        unsigned int reserved_0 : 16;           /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_od_pic_osd_worst_row_cmp1;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cmp1_min_frm_row_len : 16; /* [15..0]  */
        unsigned int reserved_0 : 16;           /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_od_pic_osd_best_row_cmp1;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cmp1_max_gap_bw_row_len_cnt : 16; /* [15..0]  */
        unsigned int reserved_0 : 16;                  /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_od_pic_osd_stat_info_cmp1;

typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int cmp1_glb_st : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_od_pic_osd_debug0_cmp1;

typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int cmp1_bitsmux_st : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_od_pic_osd_debug1_cmp1;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int special_max_row_len : 16;      /* [15..0]  */
        unsigned int cmp1_special_max_row_len : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_od_pic_osd_special_worst_row_cmp1;

#ifdef CONFIG_TDE_DRD_LINE_SUPPORT
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int line0_enable : 1;      /* [0]  */
        unsigned int line1_enable : 1;      /* [1]  */
        unsigned int line2_enable : 1;      /* [2]  */
        unsigned int line3_enable : 1;      /* [3]  */
        unsigned int align_mode : 2;      /* [5..4]  */
        unsigned int drd_req_merge_en : 1;      /* [6]  */
        unsigned int reserved : 24; /* [30..7]  */
        unsigned int drd_enable : 1; /* [31]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_drd_mask;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int line0_width : 8;      /* [7..0]  */
        unsigned int line1_width : 8;      /* [15..8]  */
        unsigned int line2_width : 8;      /* [23..16]  */
        unsigned int line3_width : 8;      /* [31..24]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_drd_line_width;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int line0_color_fill : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_drd_line0_color_cfg;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int line0_st_x : 16; /* [15..0]  */
        unsigned int line0_st_y : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_drd_line0_st;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int line0_ed_x : 16; /* [15..0]  */
        unsigned int line0_ed_y : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_drd_line0_ed;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int line1_color_fill : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_drd_line1_color_cfg;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int line1_st_x : 16; /* [15..0]  */
        unsigned int line1_st_y : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_drd_line1_st;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int line1_ed_x : 16; /* [15..0]  */
        unsigned int line1_ed_y : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_drd_line1_ed;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int line2_color_fill : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_drd_line2_color_cfg;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int line2_st_x : 16; /* [15..0]  */
        unsigned int line2_st_y : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_drd_line2_st;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int line2_ed_x : 16; /* [15..0]  */
        unsigned int line2_ed_y : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_drd_line2_ed;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int line3_color_fill : 32; /* [31..0]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_drd_line3_color_cfg;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int line3_st_x : 16; /* [15..0]  */
        unsigned int line3_st_y : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_drd_line3_st;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int line3_ed_x : 16; /* [15..0]  */
        unsigned int line3_ed_y : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    unsigned int u32;
} u_tde_drd_line3_ed;
#endif

typedef struct {
    u_src1_ctrl src1_ctrl;                   /* 0x0 */
    u_src1_ch0_addr_high src1_ch0_addr_high; /* 0x4 */
    u_src1_ch0_addr_low src1_ch0_addr_low;   /* 0x8 */
    u_src1_ch1_addr_high src1_ch1_addr_high; /* 0xc */
    u_src1_ch1_addr_low src1_ch1_addr_low;   /* 0x10 */
    u_src1_ch0_stride src1_ch0_stride;       /* 0x14 */
    u_src1_ch1_stride src1_ch1_stride;       /* 0x18 */
    u_src1_imgsize src1_imgsize;             /* 0x1c */
    u_src1_fill src1_fill;                   /* 0x20 */
    u_src1_alpha src1_alpha;                 /* 0x24 */
    u_src1_pix_offset src1_pix_offset;       /* 0x28 */
    u_src2_ctrl src2_ctrl;                   /* 0x2c */
    u_src2_ch0_addr_high src2_ch0_addr_high; /* 0x30 */
    u_src2_ch0_addr_low src2_ch0_addr_low;   /* 0x34 */
    u_src2_ch1_addr_high src2_ch1_addr_high; /* 0x38 */
    u_src2_ch1_addr_low src2_ch1_addr_low;   /* 0x3c */
    u_src2_ch0_stride src2_ch0_stride;       /* 0x40 */
    u_src2_ch1_stride src2_ch1_stride;       /* 0x44 */
    u_src2_imgsize src2_imgsize;             /* 0x48 */
    u_src2_fill src2_fill;                   /* 0x4c */
    u_src2_alpha src2_alpha;                 /* 0x50 */
    u_src2_pix_offset src2_pix_offset;       /* 0x54 */
    td_u32 gdc3_ctrl;                        /* 0x58 */
    td_u32 gdc3_addr_high;                   /* 0x5c */
    td_u32 gdc3_addr_low;                    /* 0x60 */
    td_u32 gdc3_stride;                      /* 0x64 */
    td_u32 gdc3_imgsize;                     /* 0x68 */
    td_u32 gdc3_fill;                        /* 0x6c */
    td_u32 gdc3_alpha;                       /* 0x70 */
    td_u32 gdc3_pix_offset;                  /* 0x74 */
    td_u32 gdc4_ctrl;                        /* 0x78 */
    td_u32 gdc4_addr_high;                   /* 0x7c */
    td_u32 gdc4_addr_low;                    /* 0x80 */
    td_u32 gdc4_stride;                      /* 0x84 */
    td_u32 gdc4_imgsize;                     /* 0x88 */
    td_u32 gdc4_fill;                        /* 0x8c */
    td_u32 gdc4_alpha;                       /* 0x90 */
    td_u32 gdc4_pix_offset;                  /* 0x94 */
    td_u32 gdc5_ctrl;                        /* 0x98 */
    td_u32 gdc5_addr_high;                   /* 0x9C */
    td_u32 gdc5_addr_low;                    /* 0xA0 */
    td_u32 gdc5_stride;                      /* 0xA4 */
    td_u32 gdc5_imgsize;                     /* 0xA8 */
    td_u32 gdc5_fill;                        /* 0xAC */
    td_u32 gdc5_alpha;                       /* 0xB0 */
    td_u32 gdc5_pix_offset;                  /* 0xB4 */
    td_u32 gdc6_ctrl;                        /* 0xB8 */
    td_u32 gdc6_addr_high;                   /* 0xBC */
    td_u32 gdc6_addr_low;                    /* 0xC0 */
    td_u32 gdc6_stride;                      /* 0xC4 */
    td_u32 gdc6_imgsize;                     /* 0xC8 */
    td_u32 gdc6_fill;                        /* 0xCc */
    td_u32 gdc6_alpha;                       /* 0xD0 */
    td_u32 gdc6_pix_offset;                  /* 0xD4 */
    td_u32 gdc7_ctrl;                        /* 0xD8 */
    td_u32 gdc7_addr_high;                   /* 0xDC */
    td_u32 gdc7_addr_low;                    /* 0xE0 */
    td_u32 gdc7_stride;                      /* 0xE4 */
    td_u32 gdc7_imgsize;                     /* 0xE8 */
    td_u32 gdc7_fill;                        /* 0xEC */
    td_u32 gdc7_alpha;                       /* 0xF0 */
    td_u32 gdc7_pix_offset;                  /* 0xF4 */
    u_des_ctrl des_ctrl;                     /* 0xF8 */
    u_des_ch0_addr_high des_ch0_addr_high;   /* 0xFC */
    u_des_ch0_addr_low des_ch0_addr_low;     /* 0x100 */
    u_des_ch1_addr_high des_ch1_addr_high;   /* 0x104 */
    u_des_ch1_addr_low des_ch1_addr_low;     /* 0x108 */
    u_des_ch0_stride des_ch0_stride;         /* 0x10C */
    u_des_ch1_stride des_ch1_stride;         /* 0x110 */
    u_des_imgsize des_imgsize;               /* 0x114 */
    u_des_alpha des_alpha;                   /* 0x118 */
    u_des_crop_pos_st des_crop_pos_st;       /* 0x11c */
    u_des_crop_pos_ed des_crop_pos_ed;       /* 0x120 */
    u_des_pix_offset des_pix_offset;         /* 0x124 */
    td_u32 reserved_1_01[4];                 /* 4:0x128~0x134 */
#if defined CONFIG_TDE_DMA_CORNER_V2
    u_tde_dma_corner_reso tde_dma_corner_reso;  /* 0x138 */
    u_tde_dma_corner_ctrl tde_dma_corner_ctrl;  /* 0x13c */
    u_tde_dma_corner_value tde_dma_corner_value; /* 0x140 */
    u_tde_dma_other_value tde_dma_other_value; /* 0x144 */
    u_des_safe_dist des_safe_dist;              /* 0x148 */
    u_des_safe_dist_inverse des_safe_dist_inverse;  /* 0x14c */
    u_des_bind_buffer_size des_bind_buffer_size; /* 0x150 */
    td_u32 reserved_1_02[43];                   /* 43:0x154~0x1fc */
#elif defined CONFIG_TDE_DMA_CORNER_V1
    u_tde_dma_corner_reso tde_dma_corner_reso;  /* 0x138 */
    u_tde_dma_corner_ctrl tde_dma_corner_ctrl;  /* 0x13c */
    u_des_safe_dist des_safe_dist;              /* 0x140 */
    u_des_safe_dist_inverse des_safe_dist_inverse;  /* 0x144 */
    u_des_bind_buffer_size des_bind_buffer_size; /* 0x148 */
    td_u32 reserved_1_02[45];                   /* 45:0x14c~0x1fc */
#endif
    u_src1_hsp src1_hsp;                     /* 0x200 */
    td_s32 src1_hloffset;                    /* 0x204 */
    td_s32 src1_hcoffset;                    /* 0x208 */
    u_src1_vsp src1_vsp;                     /* 0x20c */
    u_src1_vsr src1_vsr;                     /* 0x210 */
    u_src1_voffset src1_voffset;             /* 0x214 */
    u_src1_zmeoreso src1_zmeoreso;           /* 0x218 */
    u_src1_zmeireso src1_zmeireso;           /* 0x21c */
    td_u32 reserved_2[2];                    /* 2:0x220~0x224 */
    u_src1_hpzme src1_hpzme;                 /* 0x228 */
    u_src1_hpzme_size src1_hpzme_size;       /* 0x22c */
    u_src1_csc_idc0 src1_csc_idc0;           /* 0x230 */
    u_src1_csc_idc1 src1_csc_idc1;           /* 0x234 */
    u_src1_csc_odc0 src1_csc_odc0;           /* 0x238 */
    u_src1_csc_odc1 src1_csc_odc1;           /* 0x23c */
    u_src1_csc_p0 src1_csc_p0;               /* 0x240 */
    u_src1_csc_p1 src1_csc_p1;               /* 0x244 */
    u_src1_csc_p2 src1_csc_p2;               /* 0x248 */
    u_src1_csc_p3 src1_csc_p3;               /* 0x24c */
    u_src1_csc_p4 src1_csc_p4;               /* 0x250 */
    u_src1_dither_ctrl src1_dither_ctrl;     /* 0x254 */
    td_u32 reserved_3[10];                   /* 10:0x258~0x27c */
    u_src2_hsp src2_hsp;                     /* 0x280 */
    td_s32 src2_hloffset;                    /* 0x284 */
    td_s32 src2_hcoffset;                    /* 0x288 */
    u_src2_vsp src2_vsp;                     /* 0x28c */
    u_src2_vsr src2_vsr;                     /* 0x290 */
    u_src2_voffset src2_voffset;             /* 0x294 */
    u_src2_zmeoreso src2_zmeoreso;           /* 0x298 */
    u_src2_zmeireso src2_zmeireso;           /* 0x29c */
    td_u32 reserved_4[2];                    /* 2:0x2a0~0x2a4 */
    u_src2_hpzme src2_hpzme;                 /* 0x2a8 */
    u_src2_hpzme_size src2_hpzme_size;       /* 0x2ac */
    u_src2_csc_mux src2_csc_mux;             /* 0x2b0 */
    u_des_csc_idc0 des_csc_idc0;             /* 0x2b4 */
    u_des_csc_idc1 des_csc_idc1;             /* 0x2b8 */
    u_des_csc_odc0 des_csc_odc0;             /* 0x2bc */
    u_des_csc_odc1 des_csc_odc1;             /* 0x2c0 */
    u_des_csc_p0 des_csc_p0;                 /* 0x2c4 */
    u_des_csc_p1 des_csc_p1;                 /* 0x2c8 */
    u_des_csc_p2 des_csc_p2;                 /* 0x2cc */
    u_des_csc_p3 des_csc_p3;                 /* 0x2d0 */
    u_des_csc_p4 des_csc_p4;                 /* 0x2d4 */
    u_des_dither_ctrl dst_dither_ctrl;       /* offset 0x2d0 + 0x08 */
    u_des_dswm des_dswm;                     /* 0x2dc */
    td_u32 reserved_6[8];                    /* 8:0x2e0~0x2fc */
    u_cbmctrl cbmctrl;                       /* 0x300 */
    u_cbmbkg cbmbkg;                         /* 0x304 */
    u_cbmcolorize cbmcolorize;               /* 0x308 */
    u_cbmalupara cbmalupara;                 /* 0x30c */
    u_cbmkeypara cbmkeypara;                 /* 0x310 */
    u_cbmkeymin cbmkeymin;                   /* 0x314 */
    u_cbmkeymax cbmkeymax;                   /* 0x318 */
    u_cbmkeymask cbmkeymask;                 /* 0x31c */
    u_src1_cbmpara src1_cbmpara;             /* 0x320 */
    td_u32 src1_cbmstpos;                    /* 0x324 */
    u_src2_cbmpara src2_cbmpara;             /* 0x328 */
    td_u32 src2_cbmstpos;                    /* 0x32c */
    td_u32 gdc3_cbmpara;                     /* 0x330 */
    td_u32 gdc3_cbmstpos;                    /* 0x334 */
    td_u32 gdc4_cbmpara;                     /* 0x338 */
    td_u32 gdc4_cbmstpos;                    /* 0x33c */
    td_u32 gdc5_cbmpara;                     /* 0x340 */
    td_u32 gdc5_cbmstpos;                    /* 0x344 */
    td_u32 gdc6_cbmpara;                     /* 0x348 */
    td_u32 gdc6_cbmstpos;                    /* 0x34c */
    td_u32 gdc7_cbmpara;                     /* 0x350 */
    td_u32 gdc7_cbmstpos;                    /* 0x354 */
    td_u32 reserved_7[38];                   /* 38:0x358~0x3ec */
    u_src2_rtt_ctrl src2_rtt_ctrl;           /* 0x3f0 */
    td_u32 reserved_8[35];                   /* 35:0x3f4~0x47c */
    td_u32 tde_src1_zme_lhaddr_high;         /* 0x480 */
    td_u32 tde_src1_zme_lhaddr_low;          /* 0x484 */
    td_u32 tde_src1_zme_lvaddr_high;         /* 0x488 */
    td_u32 tde_src1_zme_lvaddr_low;          /* 0x48C */
    td_u32 tde_src1_zme_chaddr_high;         /* 0x490 */
    td_u32 tde_src1_zme_chaddr_low;          /* 0x494 */
    td_u32 tde_src1_zme_cvaddr_high;         /* 0x498 */
    td_u32 tde_src1_zme_cvaddr_low;          /* 0x49c */
    td_u32 tde_src2_zme_lhaddr_high;         /* 0x4A0 */
    td_u32 tde_src2_zme_lhaddr_low;          /* 0x4A4 */
    td_u32 tde_src2_zme_lvaddr_high;         /* 0x4A8 */
    td_u32 tde_src2_zme_lvaddr_low;          /* 0x4AC */
    td_u32 tde_src2_zme_chaddr_high;         /* 0x4B0 */
    td_u32 tde_src2_zme_chaddr_low;          /* 0x4B4 */
    td_u32 tde_src2_zme_cvaddr_high;         /* 0x4B8 */
    td_u32 tde_src2_zme_cvaddr_low;          /* 0x4Bc */
    td_u32 tde_clut_addr_high;               /* 0x4C0 */
    td_u32 tde_clut_addr_low;                /* 0x4C4 */
    td_u32 tde_axiid;                        /* 0x4C8 */
    td_u32 tde_nodeid;                       /* 0x4CC */
    u_tde_intmask tde_intmask;               /* 0x4D0 */
    td_u32 reserved_9[9];                    /* 9:0x3D4~0x4F4 */
    u_tde_pnext_hi tde_pnext_hi;             /* 0x4f8 */
    u_tde_pnext_low tde_pnext_low;           /* 0x4fc */
    u_tde_start tde_start;                   /* 0x500 */
    u_tde_intstate tde_intstate;             /* 0x504 */
    u_tde_intclr tde_intclr_;               /* 0x508 */
    u_tde_rawint tde_rawint;                 /* 0x50c */
    u_tde_pfcnt tde_pfcnt;                   /* 0x510 */
    u_tde_miscellaneous tde_miscellaneous_; /* 0x514 */
    u_tde_maccfg tde_maccfg;                 /* 0x518 */
    u_tde_timeout tde_timeout;               /* 0x51c */
    u_tde_eofcnt tde_eofcnt;                 /* 0x520 */
    u_tde_memctrl tde_memctrl;               /* 0x524 */
    td_u32 tde_memctrl1;                     /* 0x528 */
    td_u32 reserved_10[53]; /* 53:0x52c~0x5fc */
#ifdef CONFIG_TDE_GFBG_COMPRESS_V1
    u_tde_od_pic_osd_glb_info tde_od_pic_osd_glb_info;     /* 0x600 */
    u_tde_od_pic_osd_frame_size tde_od_pic_osd_frame_size; /* 0x604 */
    u_tde_od_pic_osd_rc_cfg0 tde_od_pic_osd_rc_cfg0;       /* 0x608 */
    u_tde_od_pic_osd_rc_cfg1 tde_od_pic_osd_rc_cfg1;       /* 0x60c */
    u_tde_od_pic_osd_rc_cfg2 tde_od_pic_osd_rc_cfg2;       /* 0x610 */
    u_tde_od_pic_osd_rc_cfg3 tde_od_pic_osd_rc_cfg3;       /* 0x614 */
    u_tde_od_pic_osd_rc_cfg4 tde_od_pic_osd_rc_cfg4;       /* 0x618 */
    u_tde_od_pic_osd_rc_cfg5 tde_od_pic_osd_rc_cfg5;       /* 0x61c */
    u_tde_od_pic_osd_rc_cfg6 tde_od_pic_osd_rc_cfg6;       /* 0x620 */
    u_tde_od_pic_osd_rc_cfg7 tde_od_pic_osd_rc_cfg7;       /* 0x624 */
    u_tde_od_pic_osd_rc_cfg8 tde_od_pic_osd_rc_cfg8;       /* 0x628 */
    u_tde_od_pic_osd_rc_cfg9 tde_od_pic_osd_rc_cfg9;       /* 0x62c */
    u_tde_od_pic_osd_rc_cfg10 tde_od_pic_osd_rc_cfg10;     /* 0x630 */
    td_u32 reserved_11[19]; /* 19:0x634~0x67c */
#endif
#ifdef CONFIG_TDE_GFBG_COMPRESS_V2
    u_tde_line_osd_cmp_glb_info tde_line_osd_cmp_glb_info; /* 0x600 */
    u_tde_line_osd_cmp_frame_size tde_line_osd_cmp_frame_size; /* 0x604 */
    u_tde_line_osd_cmp_rc_cfg0 tde_line_osd_cmp_rc_cfg0; /* 0x608 */
    u_tde_line_osd_cmp_rc_cfg1 tde_line_osd_cmp_rc_cfg1; /* 0x60c */
    u_tde_line_osd_cmp_rc_cfg2 tde_line_osd_cmp_rc_cfg2; /* 0x610 */
    u_tde_line_osd_cmp_rc_cfg3 tde_line_osd_cmp_rc_cfg3; /* 0x614 */
    u_tde_line_osd_cmp_rc_cfg4 tde_line_osd_cmp_rc_cfg4; /* 0x618 */
    u_tde_line_osd_cmp_rc_cfg5 tde_line_osd_cmp_rc_cfg5; /* 0x61c */
    u_tde_line_osd_cmp_rc_cfg6 tde_line_osd_cmp_rc_cfg6; /* 0x620 */
    u_tde_line_osd_cmp_rc_cfg7 tde_line_osd_cmp_rc_cfg7; /* 0x624 */
    u_tde_line_osd_cmp_rc_cfg8 tde_line_osd_cmp_rc_cfg8; /* 0x628 */
    u_tde_line_osd_cmp_rc_cfg9 tde_line_osd_cmp_rc_cfg9; /* 0x62c */
    u_tde_line_osd_cmp_rc_cfg10 tde_line_osd_cmp_rc_cfg10; /* 0x630 */
    u_tde_line_osd_cmp_rc_cfg11 tde_line_osd_cmp_rc_cfg11; /* 0x634 */
    u_tde_line_osd_cmp_rc_cfg12 tde_line_osd_cmp_rc_cfg12; /* 0x638 */
    u_tde_line_osd_cmp_rc_cfg13 tde_line_osd_cmp_rc_cfg13; /* 0x63c */
    u_tde_line_osd_cmp_rc_cfg14 tde_line_osd_cmp_rc_cfg14; /* 0x640 */
    u_tde_line_osd_cmp_rc_cfg15 tde_line_osd_cmp_rc_cfg15; /* 0x644 */
    u_v4r2_line_osd_cmp_rc_cfg16 v4r2_line_osd_cmp_rc_cfg16; /* 0x648 */
    u_tde_line_osd_cmp_rc_cfg17 tde_line_osd_cmp_rc_cfg17; /* 0x64c */
    u_tde_line_osd_cmp_glb_st tde_line_osd_cmp_glb_st; /* 0x650 */
    u_tde_line_osd_cmp_dbg_reg tde_line_osd_cmp_dbg_reg; /* 0x654 */
    td_u32 reserved_11[10]; /* 10:0x658~0x67c */
#endif
#ifdef CONFIG_TDE_DRD_LINE_SUPPORT
    u_tde_drd_mask tde_drd_mask;                   /* 0x680 */
    u_tde_drd_line_width tde_drd_line_width;       /* 0x684 */
    u_tde_drd_line0_color_cfg tde_drd_line0_color_cfg; /* 0x688 */
    u_tde_drd_line0_st tde_drd_line0_st; /* 0x68c */
    u_tde_drd_line0_ed tde_drd_line0_ed; /* 0x690 */
    u_tde_drd_line1_color_cfg tde_drd_line1_color_cfg; /* 0x694 */
    u_tde_drd_line1_st tde_drd_line1_st; /* 0x698 */
    u_tde_drd_line1_ed tde_drd_line1_ed; /* 0x69c */
    u_tde_drd_line2_color_cfg tde_drd_line2_color_cfg; /* 0x6a0 */
    u_tde_drd_line2_st tde_drd_line2_st; /* 0x6a4 */
    u_tde_drd_line2_ed tde_drd_line2_ed; /* 0x6a8 */
    u_tde_drd_line3_color_cfg tde_drd_line3_color_cfg; /* 0x6ac */
    u_tde_drd_line3_st tde_drd_line3_st; /* 0x6b0 */
    u_tde_drd_line3_ed tde_drd_line3_ed; /* 0x6b4 */
#endif
} tde_hw_node;

typedef enum {
    TDE_DRV_INT_NODE = 0x1,
    TDE_DRV_INT_TIMEOUT = 0x2,
    TDE_DRV_INT_ERROR = 0x4,
    TDE_DRV_INT_NODE_COMP_AQ = 0x8,
} tde_drv_int;

typedef enum {
    OT_GFX_TDE_ID = 0, /* TDE ID */
    OT_GFX_JPGDEC_ID,  /* JPEG DECODE ID */
    OT_GFX_JPGENC_ID,  /* JPEG_ENCODE ID */
    OT_GFX_FB_ID,      /*  FRAMEBUFFER ID */
    OT_GFX_PNG_ID,     /* PNG ID */
    OT_GFX_HIGO_ID,
    OT_GFX_GFX2D_ID,
    OT_GFX_BUTT_ID,
} gfx_mode_id;

#define conver_id(module_id) ((module_id) + OT_ID_TDE - OT_GFX_TDE_ID)

#define CONFIG_TDE_TDE_EXPORT_FUNC

#define TDE_NO_SCALE_VSTEP     0x1000
#define TDE_NO_SCALE_HSTEP     0x100000
#define TDE_FLOAT_BITLEN       12
#define TDE_HAL_HSTEP_FLOATLEN 20
#define TDE_HAL_VSTEP_FLOATLEN 12
#define TDE_MAX_SLICE_WIDTH    256
#define TDE_MAX_SLICE_NUM      20
#define TDE_MAX_SURFACE_PITCH  0xffff
#define TDE_MAX_ZOOM_OUT_STEP  8
#define TDE_MAX_RECT_WIDTH_EX  0x2000
#define TDE_MAX_RECT_HEIGHT_EX 0x2000

#define TDE_MAX_RECT_WIDTH  0x1000
#define TDE_MAX_RECT_HEIGHT 0x1000

#define TDE_MAX_SLICE_RECT_WIDTH  0xfff
#define TDE_MAX_SLICE_RECT_HEIGHT 0xfff

#define TDE_MAX_MINIFICATION_H 255
#define TDE_MAX_MINIFICATION_V 255

#define ROP 0x1                /* Rop */
#define ALPHABLEND (0x1 << 1)  /* AlphaBlend */
#define COLORIZE   (0x1 << 2)  /* Colorize */
#define CLUT       (0x1 << 3)  /* Clut */
#define COLORKEY   (0x1 << 4)  /* ColorKey */
#define CLIP       (0x1 << 5)  /* Clip */
#define DEFLICKER  (0x1 << 6)  /* Deflicker */
#define RESIZE     (0x1 << 7)  /* Resize */
#define MIRROR     (0x1 << 8)  /* Mirror */
#define CSCCOVERT  (0x1 << 9)  /* CSC */
#define QUICKCOPY  (0x1 << 10) /* copy */
#define QUICKFILL  (0x1 << 11) /* fill */
#define PATTERFILL (0x1 << 12) /* patterfill */
#define MASKROP    (0x1 << 13) /* MaskRop */
#define MASKBLEND  (0x1 << 14) /* MaskBlend */
#define ROTATE     (0x1 << 15) /* Rotate */
#define COMPRESS   (0x1 << 16) /* Compress */
#define SYNC       (0x1 << 17) /* Sync */
#define SLICE      (0x1 << 18) /* Slice */

#define ROP_MASK        0xffffffff
#define ALPHABLEND_MASK 0xffffffff
#define COLORIZE_MASK   0xffffffff
#define CLUT_MASK       0xffffffff
#define COLORKEY_MASK   0xffffffff
#define CLIP_MASK       0xffffffff
#define DEFLICKER_MASK  0xffffffff
#define RESIZE_MASK     0xffffffff
#define MIRROR_MASK     0xffffffff
#define CSCCOVERT_MASK  0xffffffff
#define QUICKCOPY_MASK  0xffffffff
#define QUICKFILL_MASK  0xffffffff
#define PATTERFILL_MASK 0xffffffff
#define MASKROP_MASK    0xffffffff
#define MASKBLEND_MASK  0xffffffff
#ifdef CONFIG_TDE_ROTATE_SUPPORT
#define ROTATE_MASK     0xffffffff
#else
#define ROTATE_MASK     0x0
#endif
#define COMPRESS_MASK   0xffffffff
#ifdef CONFIG_GFBG_LOW_DELAY_SUPPORT
#define SYNC_MASK       0xffffffff
#else
#define SYNC_MASK       0x0
#endif
#define SLICE_MASK      0x0

td_void tde_hal_get_capability(td_u32 *capability);

#define TDE_CAPABILITY (((ROP) & (ROP_MASK)) | \
                        ((ALPHABLEND) & (ALPHABLEND_MASK)) | \
                        ((COLORIZE) & (COLORIZE_MASK)) | \
                        ((CLUT) & (CLUT_MASK)) | \
                        ((COLORKEY) & (COLORKEY_MASK)) | \
                        ((CLIP) & (CLIP_MASK)) | \
                        ((DEFLICKER) & (DEFLICKER_MASK)) | \
                        ((RESIZE) & (RESIZE_MASK)) | \
                        ((MIRROR) & (MIRROR_MASK)) | \
                        ((CSCCOVERT) & (CSCCOVERT_MASK)) | \
                        ((QUICKCOPY) & (QUICKCOPY_MASK)) | \
                        ((QUICKFILL) & (QUICKFILL_MASK)) | \
                        ((PATTERFILL) & (PATTERFILL_MASK)) | \
                        ((MASKROP) & (MASKROP_MASK)) | \
                        ((MASKBLEND) & (MASKBLEND_MASK)) | \
                        ((ROTATE) & (ROTATE_MASK)) | \
                        ((COMPRESS) & (COMPRESS_MASK)) | \
                        ((SYNC) & (SYNC_MASK)) | \
                        ((SLICE) & (SLICE_MASK)))

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* TDE_ADP_H */
