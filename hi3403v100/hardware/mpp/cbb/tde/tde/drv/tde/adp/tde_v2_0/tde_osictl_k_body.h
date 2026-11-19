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

#ifndef TDE_OSICTL_K_BODY_INC
#define TDE_OSICTL_K_BODY_INC

#line 1 "tde_osictl_k.c"
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

#include "tde_osictl_k.h"

#ifdef CONFIG_OT_PROC_SHOW_SUPPORT
#include "tde_proc.h"
#endif

#include "ot_math.h"
#include "securec.h"
#include "wmalloc.h"
#include "tde_handle.h"
#include "tde_osictl.h"
#include "tde_osilist.h"
#include "tde_hal.h"
#include "tde_adp.h"

/* TDE osi ctl macro definition */
#define TDE_CLUT_SIZE (256 * 4)

#define tde_min(a, b) (((a) > (b)) ? (b) : (a))

#define drv_tde_mb_src_val(mb_src, theight, twidth)        \
    do {                                                   \
        theight = (mb_src)->src_rect->height;              \
        twidth = (mb_src)->src_rect->width;                \
    } while (0)

#define drv_tde_mb_src_addr(mb_src, tu64Yphy_addr, tu64CbCrphy_addr, tu64phy)    \
        do {                                                                     \
            tu64Yphy_addr = (mb_src)->mb_surface->y_addr;                        \
            tu64CbCrphy_addr = (mb_src)->mb_surface->cbcr_phys_addr;              \
            tu64phy = (mb_src)->dst_surface->phys_addr;                           \
        } while (0)

#define tde_get_yc422r_fillvalue(value) (((value)&(0xffffff)) | ((((value) >> (8)) & (0xff)) << (24)))

#define DRD_RIGHT_LIMIT 16383
#define DRD_LEFT_LIMIT (-16383)
#define DRD_MAX_LINE_WIDTH 256

static td_bool tde_osi_check_format_support_draw(drv_tde_color_fmt format)
{
#if defined CONFIG_TDE_DMA_CORNER_V1
    if ((format != DRV_TDE_COLOR_FMT_CLUT2) && (format != DRV_TDE_COLOR_FMT_CLUT4)) {
        tde_error("This operation only support clut2 or clut4!\n");
        return TD_FALSE;
    }
#elif defined CONFIG_TDE_DMA_CORNER_V2
    if (format >= DRV_TDE_COLOR_FMT_YCBCR422) {
        tde_error("This color format %d not support!should be in [%d %d]\n", format, 0, DRV_TDE_COLOR_FMT_AYCBCR8888);
        return TD_FALSE;
    }
#endif
    return TD_TRUE;
}

static td_s32 tde_check_subbyte_startx(td_s32 startx, td_u32 w, drv_tde_color_fmt format)
{
    td_s32 bpp;
    bpp = tde_osi_get_bpp_by_fmt(format);
    if (bpp < 0) {
        tde_error("Unknown color format!\n");
        return TD_FAILURE;
    }
    if (bpp < 8) { /* 8 bits */
        /* when writing, subbyte format align ask start point byte align */
        if (((startx) * bpp % 8) || ((w) * bpp % 8)) { /* 8 bits */
            tde_error("The input start or write width does not meet the single byte alignment!\n");
            return TD_FAILURE;
        }
    }
    return TD_SUCCESS;
}

#define ycc2rgb(y, cb, cr, r, g, b)                                                            \
    do {                                                                                       \
        r = (((298 * ((y) - 16) + 409 * ((cr) - 128)) >> 7) + 1) >> 1;                         \
        g = (((298 * ((y) - 16) - 100 * ((cb) - 128) - 208 * ((cr) - 128)) >> 7) + 1) >> 1;    \
        b = (((298 * ((y) - 16) + 517 * ((cb) - 128)) >> 7) + 1) >> 1;                         \
    } while (0)

#define rgb2ycc(r, g, b, y, cb, cr)                                          \
    do {                                                                     \
        y  = ((((263 * (r) + 516 * (g) + 100 * (b)) >> 9) + 1) >> 1) + 16;   \
        cb = ((((-152 * (r) - 298 * (g) + 450 * (b)) >> 9) + 1) >> 1) + 128; \
        cr = ((((450 * (r) - 377 * (g) - 73 * (b)) >> 9) + 1) >> 1) + 128;   \
    } while (0)

#define TDE_FOUR_BITS_SHIFT  4
#define TDE_THREE_BITS_SHIFT 3
#define TDE_TWO_BITS_SHIFT   2
#define TDE_ONE_BIT_SHIFT    1
#define TDE_ZERO_BIT_SHIFT   0
#define TDE_MAX_PATTERNWIDTH 256

/* TDE osi ctl struct definition */
#if (TDE_CAPABILITY & SLICE)
typedef struct {
    td_u32 des_width;
    td_u32 des_hoffset_pix;
    td_u32 des_h_scan_ord;
    td_u32 des_crop_en;
    td_u32 des_crop_start_x;
    td_u32 des_crop_end_x;

    td_u32 xdpos;
    td_u32 h_scan_ord;
    td_u32 width;

    td_u32 hlmsc_en;
    td_u32 hchmsc_en;
    td_u32 hratio;
    td_s32 hor_loffset;
    td_s32 hor_coffset;
    td_u32 zme_ow;
    td_u32 zme_iw;
    td_u32 hpzme_en;
    td_u32 hpzme_mode;
    td_u32 hpzme_width;

    td_u32 des_xst_pos_blk;
    td_u32 des_xed_pos_blk;
    td_u32 xed_pos_blk;
    td_u32 xst_pos_cord;
    td_u32 xed_pos_cord;
    td_u32 xst_pos_cord_in;
    td_u32 xed_pos_cord_in;
    td_u32 hor_loffset_cfg_int_comp;
    td_s32 hor_loffset_cfg_int;
    td_s32 xst_pos_cord_in_offset;
    td_s32 xed_pos_cord_in_offset;
    td_u32 xst_pos_cord_in_tap_rgb;
    td_u32 xed_pos_cord_in_tap_rgb;
    td_u32 node_cfg_zme_iw_rgb;
    td_u32 hor_loffset_cfg_fraction;
    td_u32 hor_loffset_pix_fraction;
    td_u32 hor_loffset_fraction;
    td_s32 hor_loffset_int;
    td_s32 node_cfg_hor_loffset_rgb;
    td_u32 xst_pos_cord_in_tap_luma;
    td_u32 xed_pos_cord_in_tap_luma;
    td_u32 xst_pos_cord_in_chroma;
    td_u32 xed_pos_cord_in_chroma;
    td_u32 hor_coffset_cfg_int_comp;
    td_s32 hor_coffset_cfg_int;
    td_s32 xst_pos_cord_in_offset_chroma;
    td_s32 xed_pos_cord_in_offset_chroma;
    td_u32 xst_pos_cord_in_tap_chroma;
    td_u32 xed_pos_cord_in_tap_chroma;
    td_u32 xst_pos_cord_in_tap_chroma_x2;
    td_u32 xed_pos_cord_in_tap_chroma_x2;
    td_u32 xst_pos_cord_in_tap_sp;
    td_u32 xed_pos_cord_in_tap_sp;
    td_u32 node_cfg_zme_iw_sp;
    td_u32 hor_coffset_cfg_fraction;
    td_u32 hor_coffset_pix_fraction;
    td_u32 hor_coffset_fraction;
    td_s32 hor_loffset_int_sp;
    td_s32 hor_coffset_int_sp;
    td_u32 hor_loffset_int_sp_complent;
    td_u32 hor_coffset_int_sp_complent;
    td_s32 node_cfg_hor_loffset_sp;
    td_s32 node_cfg_hor_coffset_sp;
    td_u32 xst_pos_cord_in_tap;
    td_u32 xed_pos_cord_in_tap;

    td_u32 hor_loffset_int_complement;
    td_u32 node_num;

    td_u32 xst_pos_cord_in_tap_hpzme;
    td_u32 xed_pos_cord_in_tap_hpzme;
    td_u32 xst_pos_cord_in_tap_hpzme_hso;
    td_u32 xed_pos_cord_in_tap_hpzme_hso;
    td_u32 u32422v_pro;
    td_u32 hor_loffset_int_beyond;
    td_u32 hor_loffset_int_beyond_complent;

    td_u32 slice_width;
    td_u32 slice_wi;
    td_s32 slice_c_ofst;
    td_s32 slice_l_ofst;
    td_u32 slice_hoffset;
    td_u32 slice_wo;
    td_u32 slice_w_hpzme;
    td_u32 slice_dst_width;
    td_u32 slice_dst_hoffset;
    td_u32 fmt;
} tde_slice_data;
#endif

/* pixel format transform type */
typedef enum {
    TDE_COLORFMT_TRANSFORM_ARGB_ARGB = 0,
    TDE_COLORFMT_TRANSFORM_ARGB_YCBCR,
    TDE_COLORFMT_TRANSFORM_CLUT_ARGB,
    TDE_COLORFMT_TRANSFORM_CLUT_YCBCR,
    TDE_COLORFMT_TRANSFORM_CLUT_CLUT,
    TDE_COLORFMT_TRANSFORM_YCBCR_ARGB,
    TDE_COLORFMT_TRANSFORM_YCBCR_YCBCR,
    TDE_COLORFMT_TRANSFORM_AN_AN,
    TDE_COLORFMT_TRANSFORM_ARGB_AN,
    TDE_COLORFMT_TRANSFORM_YCBCR_AN,
    TDE_COLORFMT_TRANSFORM_BUTT,
} tde_colorfmt_transform;

/* CLUT table use */
typedef enum {
    TDE_CLUT_COLOREXPENDING = 0, /* color expend */
    TDE_CLUT_COLORCORRECT,       /* color correct */
    TDE_CLUT_CLUT_BYPASS,
    TDE_CLUT_USAGE_BUTT
} tde_clut_usage;

typedef enum {
    TDE_OPERATION_SINGLE_SRC1 = 0,
    TDE_OPERATION_SINGLE_SRC2,
    TDE_OPERATION_DOUBLE_SRC,
    TDE_OPERATION_BUTT
} tde_operation_category;

typedef enum {
    TDE_PATTERN_OPERATION_SINGLE_SRC = 0,
    TDE_PATTERN_OPERATION_DOUBLE_SRC,
    TDE_PATTERN_OPERATION_BUTT
} tde_pattern_operation_category;

typedef struct {
    td_u8 alpha_bits;
    td_u8 red_bits;
    td_u8 green_bits;
    td_u8 blue_bits;
    td_u8 alpha_offset;
    td_u8 red_offset;
    td_u8 green_offset;
    td_u8 blue_offset;
} tde2_fmt_bitoffset;

#if (TDE_CAPABILITY & RESIZE)
typedef struct {
    td_s32 ori_in_width;    /* original image width */
    td_s32 ori_in_height;   /* original image height */
    td_s32 zme_out_width;   /* output full image width */
    td_s32 zme_out_height;  /* output full image height */

    td_s32 update_instart_w;  /* the start_x of update area in original image */
    td_s32 update_instart_h;  /* the start_y of update area in original image */
    td_s32 update_in_width;   /* the width of update area in original image */
    td_s32 update_in_height;  /* the height of update area in original image */
} tde_update_config;

typedef struct {
    td_s32 zme_instart_w;  /* the start_x of needed readin area in original image */
    td_s32 zme_instart_h;  /* the start_y of needed readin area in original image */
    td_s32 zme_in_width;   /* the width of needed readin area in original image */
    td_s32 zme_in_height;  /* the height of needed readin area in original image */

    td_s32 zme_outstart_w;  /* the start_x of needed update area in output image */
    td_s32 zme_outstart_h;  /* the start_y of needed update area in output image */
    td_s32 zme_out_width;   /* the width of needed update area in output image */
    td_s32 zme_out_height;  /* the height of needed update area in output image */

    td_s32 zme_hphase;      /* the start phase of horizontal scale */
    td_s32 zme_vphase;      /* the start phase of vertical scale */
    td_s32 def_offsetup;    /* the up offset of deflicker */
    td_s32 def_offsetdown;  /* the down offset of deflicker */
} tde_update_info;
#endif

/* TDE osi ctl inner variables definition */
tde_color_fmt g_tde_common_drv_color_fmt[DRV_TDE_COLOR_FMT_MAX + 1] = {
    TDE_DRV_COLOR_FMT_RGB444, TDE_DRV_COLOR_FMT_RGB444,
    TDE_DRV_COLOR_FMT_RGB555, TDE_DRV_COLOR_FMT_RGB555,
    TDE_DRV_COLOR_FMT_RGB565, TDE_DRV_COLOR_FMT_RGB565,
    TDE_DRV_COLOR_FMT_RGB888, TDE_DRV_COLOR_FMT_RGB888,
    TDE_DRV_COLOR_FMT_ARGB4444, TDE_DRV_COLOR_FMT_ARGB4444, TDE_DRV_COLOR_FMT_ARGB4444, TDE_DRV_COLOR_FMT_ARGB4444,
    TDE_DRV_COLOR_FMT_ARGB1555, TDE_DRV_COLOR_FMT_ARGB1555, TDE_DRV_COLOR_FMT_ARGB1555, TDE_DRV_COLOR_FMT_ARGB1555,
    TDE_DRV_COLOR_FMT_ARGB8565, TDE_DRV_COLOR_FMT_ARGB8565, TDE_DRV_COLOR_FMT_ARGB8565, TDE_DRV_COLOR_FMT_ARGB8565,
    TDE_DRV_COLOR_FMT_ARGB8888, TDE_DRV_COLOR_FMT_ARGB8888, TDE_DRV_COLOR_FMT_ARGB8888, TDE_DRV_COLOR_FMT_ARGB8888,
    TDE_DRV_COLOR_FMT_ARGB8888,
    TDE_DRV_COLOR_FMT_CLUT1, TDE_DRV_COLOR_FMT_CLUT2, TDE_DRV_COLOR_FMT_CLUT4,
    TDE_DRV_COLOR_FMT_CLUT8,
    TDE_DRV_COLOR_FMT_ACLUT44, TDE_DRV_COLOR_FMT_ACLUT88,
    TDE_DRV_COLOR_FMT_A1, TDE_DRV_COLOR_FMT_A8,
    TDE_DRV_COLOR_FMT_YCBCR888, TDE_DRV_COLOR_FMT_AYCBCR8888, TDE_DRV_COLOR_FMT_YCBCR422, TDE_DRV_COLOR_FMT_PKGVYUY,
    TDE_DRV_COLOR_FMT_BYTE, TDE_DRV_COLOR_FMT_HALFWORD,
    TDE_DRV_COLOR_FMT_YCBCR400MBP,
    TDE_DRV_COLOR_FMT_YCBCR422MBH, TDE_DRV_COLOR_FMT_YCBCR422MBV,
    TDE_DRV_COLOR_FMT_YCBCR420MB, TDE_DRV_COLOR_FMT_YCBCR420MB, TDE_DRV_COLOR_FMT_YCBCR420MB,
    TDE_DRV_COLOR_FMT_YCBCR420MB,
    TDE_DRV_COLOR_FMT_YCBCR444MB, TDE_DRV_COLOR_FMT_MAX
};

static tde_argb_order_mode g_tde_argb_order[DRV_TDE_COLOR_FMT_MAX + 1] = {
    TDE_DRV_ORDER_ARGB, TDE_DRV_ORDER_ABGR,
    TDE_DRV_ORDER_ARGB, TDE_DRV_ORDER_ABGR,
    TDE_DRV_ORDER_ARGB, TDE_DRV_ORDER_ABGR,
    TDE_DRV_ORDER_ARGB, TDE_DRV_ORDER_ABGR,
    TDE_DRV_ORDER_ARGB, TDE_DRV_ORDER_ABGR, TDE_DRV_ORDER_RGBA, TDE_DRV_ORDER_BGRA,
    TDE_DRV_ORDER_ARGB, TDE_DRV_ORDER_ABGR, TDE_DRV_ORDER_RGBA, TDE_DRV_ORDER_BGRA,
    TDE_DRV_ORDER_ARGB, TDE_DRV_ORDER_ABGR, TDE_DRV_ORDER_RGBA, TDE_DRV_ORDER_BGRA,
    TDE_DRV_ORDER_ARGB, TDE_DRV_ORDER_ABGR, TDE_DRV_ORDER_RGBA, TDE_DRV_ORDER_BGRA,
    TDE_DRV_ORDER_RABG,
    TDE_DRV_ORDER_MAX, TDE_DRV_ORDER_MAX, TDE_DRV_ORDER_MAX,
    TDE_DRV_ORDER_MAX,
    TDE_DRV_ORDER_MAX, TDE_DRV_ORDER_MAX,
    TDE_DRV_ORDER_MAX, TDE_DRV_ORDER_MAX,
    TDE_DRV_ORDER_MAX, TDE_DRV_ORDER_MAX, TDE_DRV_ORDER_MAX, TDE_DRV_ORDER_MAX,
    TDE_DRV_ORDER_MAX, TDE_DRV_ORDER_MAX,
    TDE_DRV_ORDER_MAX,
    TDE_DRV_ORDER_MAX, TDE_DRV_ORDER_MAX,
    TDE_DRV_ORDER_MAX, TDE_DRV_ORDER_MAX, TDE_DRV_ORDER_MAX,
    TDE_DRV_ORDER_MAX,
    TDE_DRV_ORDER_MAX
};

/* if local deflicker flag */
static td_bool g_region_deflicker = TD_FALSE;
static td_bool g_is_resize_filter = TD_TRUE;

/* TDE osi ctl inner interface definition */
static td_void tde_unify_rect(drv_tde_rect *src_rect, drv_tde_rect *dst_rect);
static tde_colorfmt_category tde_osi_get_fmt_category(drv_tde_color_fmt fmt);

static tde_colorfmt_transform tde_osi_get_fmt_trans_type(drv_tde_color_fmt src2_fmt, drv_tde_color_fmt dst_fmt);

static td_s32 tde_osi_set_clut_opt(const drv_tde_surface *clut_sur, const drv_tde_surface *out_sur,
                                   tde_clut_usage *clut_usage, td_bool clut_reload, tde_hw_node *hw_node);

static tde_clut_usage tde_osi_get_clut_usage(drv_tde_color_fmt src_fmt, drv_tde_color_fmt dst_fmt);

static td_s32 tde_osi_get_scan_info_ex(const drv_tde_single_src *single_src, const drv_tde_opt *opt,
                                       tde_scandirection_mode *src_direction,
                                       tde_scandirection_mode *dst_direction);

static td_s32 tde_osi_get_inter_rect(const drv_tde_rect *rect1, const drv_tde_rect *rect2, drv_tde_rect *inter_rect);

static td_s32 tde_osi_set_mb_para(td_s32 handle, const drv_tde_mb_src *mb_src, const drv_tde_mb_opt *mb_opt);

static td_s32 tde_osi_set_filter_node(td_s32 handle, tde_hw_node *node, drv_tde_double_src *double_src,
                                      drv_tde_deflicker_mode deflicker_mode, drv_tde_deflicker_mode enFliterMode);

static td_s32 tde_osi_1_source_fill(td_s32 handle, drv_tde_surface *dst_surface,
                                    drv_tde_rect *dst_rect, drv_tde_fill_color *fill_color,
                                    const drv_tde_opt *opt);

static td_s32 tde_osi_single_src_2_blit(td_s32 handle, const drv_tde_single_src *single_src,
                                        const drv_tde_opt *opt, td_bool mmz_for_src, td_bool mmz_for_dst);

static td_s32 tde_osi_2_source_fill(td_s32 handle, const drv_tde_single_src *single_src,
                                    const drv_tde_fill_color *fill_color, const drv_tde_opt *opt);

static td_s32 tde_osi_set_color_key(const drv_tde_double_src *double_src,
                                    tde_hw_node *hw_node, drv_tde_color_key colorkey_value,
                                    drv_tde_color_key_mode colorkey_mode,
                                    tde_clut_usage clut_usage);
static td_s32 tde_osi_set_blend(tde_hw_node *hw_node, drv_tde_alpha_blending alpha_blending_cmd,
    drv_tde_blend_opt blend_opt, tde_alu_mode *enAluMode, td_bool bCheckBlend);

#if (TDE_CAPABILITY & ROP)
static td_s32 tde_osi_set_rop(tde_hw_node *hw_node, const tde_rop_opt *rop_opt, tde_alu_mode *alu_mode);
#endif

#if (TDE_CAPABILITY & COLORIZE)
static td_s32 tde_osi_set_colorize(tde_hw_node *hw_node, drv_tde_alpha_blending alpha_blending_cmd,
    td_s32 color_resize);
#endif

static td_s32 tde_osi_check_double_src_pattern_fill_para(const drv_tde_double_src *double_src,
                                                         const drv_tde_pattern_fill_opt *opt);

static td_s32 tde_osi_check_single_src_pattern_fill_para(const drv_tde_single_src *single_src,
                                                         const drv_tde_pattern_fill_opt *opt);

static tde_pattern_operation_category tde_osi_check_single_src_pattern_operation(const drv_tde_double_src *double_src,
    const drv_tde_pattern_fill_opt *opt);

static tde_pattern_operation_category tde_osi_double_src_pattern_operation(const drv_tde_double_src *double_src,
                                                                           const drv_tde_pattern_fill_opt *opt);

static td_s32 tde_osi_single_src_1_blit(td_s32 handle, const drv_tde_single_src *single_src,
                                        td_bool mmz_for_src, td_bool mmz_for_dst);

static td_s32 tde_osi_set_foreground_color_key(tde_hw_node *hw_node, const drv_tde_surface *src_surface,
                                               const drv_tde_opt *opt, tde_clut_usage clut_usage);

static tde_operation_category tde_osi_single_src_operation(const drv_tde_double_src *double_src,
                                                           const drv_tde_opt *opt);

static tde_operation_category tde_osi_double_src_operation(drv_tde_double_src *double_src, const drv_tde_opt *opt);

static td_s32 tde_osi_check_surface(const drv_tde_surface *surface, drv_tde_rect *rect);

static tde_operation_category tde_osi_get_opt_category(drv_tde_double_src *double_src, const drv_tde_opt *opt);

static td_void tde_osi_convert_surface(const drv_tde_surface *sur, const drv_tde_rect *rect,
                                       const tde_scandirection_mode *scan_info,
                                       tde_surface_msg *drv_sur);

static td_s32 tde_osi_set_clip_para(drv_tde_double_src *double_src, const drv_tde_opt *opt, tde_hw_node *hw_node);

static td_s32 tde_osi_set_base_opt_para_for_blit(const drv_tde_opt *opt, const drv_tde_surface *src1,
                                                 const drv_tde_surface *src2, tde_operation_category opt_category,
                                                 tde_hw_node *hw_node);

static td_void tde_osi_adj_clip_para(tde_hw_node *hw_node);

static td_s32 tde_osi_set_node_finish(td_s32 handle, tde_hw_node *hw_node,
                                      td_u32 work_buf_num, tde_node_subm_type subm_type);
static td_s32 tde_osi_check_resize_para(td_u32 in_width, td_u32 in_height,
                                        td_u32 out_width, td_u32 out_height);
static td_bool tde_osi_whether_contain_alpha(drv_tde_color_fmt color_fmt);
static td_void tde_osi_set_ext_alpha(const drv_tde_surface *back_ground, const drv_tde_surface *fore_ground,
                                     tde_hw_node *hw_node);

static td_s32 tde_osi_pre_check_surface_ex(const drv_tde_surface *surface, drv_tde_rect *rect);

static td_s32 tde_osi_raster_fmt_check_align(const drv_tde_surface *surface);

static td_s32 tde_osi_check_mb_blit_para(const drv_tde_mb_src *mb_src, const drv_tde_mb_opt *mb_opt);

static td_s32 tde_osi_get_double_cycle_data(const drv_tde_rect *mb_rect, td_u32 *i, td_u32 *j);
static td_s32 tde_osi_get_bpp_by_fmt_1(drv_tde_color_fmt fmt);
static td_s32 tde_osi_get_bpp_by_fmt_2(drv_tde_color_fmt fmt);
static td_s32 tde_osi_get_bpp_by_fmt_4(drv_tde_color_fmt fmt);
static td_s32 tde_osi_get_bpp_by_fmt_8(drv_tde_color_fmt fmt);
static td_s32 tde_osi_get_bpp_by_fmt_16(drv_tde_color_fmt fmt);
static td_s32 tde_osi_get_bpp_by_fmt_24(drv_tde_color_fmt fmt);
static td_s32 tde_osi_get_bpp_by_fmt_32(drv_tde_color_fmt fmt);
static td_s32 tde_osi_check_single_src_to_para(const drv_tde_surface *fore_ground, const drv_tde_rect *fore_ground_rect,
                                               const drv_tde_surface *dst_surface, const drv_tde_rect *dst_rect,
                                               const drv_tde_opt *opt);

static td_s32 tde_osi_double_src_2_blit(td_s32 handle, drv_tde_double_src *double_src, const drv_tde_opt *opt);

static td_void tde_unify_rect(drv_tde_rect *src_rect, drv_tde_rect *dst_rect)
{
    if (src_rect->height != dst_rect->height) {
        src_rect->height = tde_min(src_rect->height, dst_rect->height);
        dst_rect->height = src_rect->height;
    }
    if (src_rect->width != dst_rect->width) {
        src_rect->width = tde_min(src_rect->width, dst_rect->width);
        dst_rect->width = src_rect->width;
    }

    return;
}

static tde2_fmt_bitoffset g_fmt_bit_and_offset_array[DRV_TDE_COLOR_FMT_AYCBCR8888 + 1] = {
    { 0, 4, 4, 4, 12, 8,  4,  0 },  /* DRV_TDE_COLOR_FMT_RGB444 */
    { 0, 4, 4, 4, 12, 0,  4,  8 },  /* DRV_TDE_COLOR_FMT_BGR444 */
    { 0, 5, 5, 5, 15, 10, 5,  0 }, /* DRV_TDE_COLOR_FMT_RGB555 */
    { 0, 5, 5, 5, 15, 0,  5,  10 }, /* DRV_TDE_COLOR_FMT_BGR555 */
    { 0, 5, 6, 5, 16, 11, 5,  0 }, /* DRV_TDE_COLOR_FMT_RGB565 */
    { 0, 5, 6, 6, 16, 0,  5,  11 }, /* DRV_TDE_COLOR_FMT_BGR565 */
    { 0, 8, 8, 8, 24, 16, 8,  0 }, /* DRV_TDE_COLOR_FMT_RGB888 */
    { 0, 8, 8, 8, 24, 0,  8,  16 }, /* DRV_TDE_COLOR_FMT_BGR888 */
    { 4, 4, 4, 4, 12, 8,  4,  0 },  /* DRV_TDE_COLOR_FMT_ARGB4444 */
    { 4, 4, 4, 4, 12, 0,  4,  8 },  /* DRV_TDE_COLOR_FMT_ABGR4444 */
    { 4, 4, 4, 4, 0,  12, 8,  4 },  /* DRV_TDE_COLOR_FMT_RGBA4444 */
    { 4, 4, 4, 4, 0,  4,  8,  12 },  /* DRV_TDE_COLOR_FMT_BGRA4444 */
    { 1, 5, 5, 5, 15, 10, 5,  0 }, /* DRV_TDE_COLOR_FMT_ARGB1555 */
    { 1, 5, 5, 5, 15, 0,  5,  10 }, /* DRV_TDE_COLOR_FMT_ABGR1555 */
    { 1, 5, 5, 5, 0,  11, 6,  1 },  /* DRV_TDE_COLOR_FMT_RGBA1555 */
    { 1, 5, 5, 5, 0,  1,  6,  11 },  /* DRV_TDE_COLOR_FMT_BGRA1555 */
    { 8, 5, 6, 5, 16, 11, 5,  0 }, /* DRV_TDE_COLOR_FMT_ARGB8565 */
    { 8, 5, 6, 5, 16, 0,  5,  11 }, /* DRV_TDE_COLOR_FMT_ABGR8565 */
    { 8, 5, 6, 5, 0,  19, 13, 8 }, /* DRV_TDE_COLOR_FMT_RGBA8565 */
    { 8, 5, 6, 6, 0,  8,  13, 19 }, /* DRV_TDE_COLOR_FMT_BGRA8565 */
    { 8, 8, 8, 8, 24, 16, 8,  0 }, /* DRV_TDE_COLOR_FMT_ARGB8888 */
    { 8, 8, 8, 8, 24, 0,  8,  16 }, /* DRV_TDE_COLOR_FMT_ABGR8888 */
    { 8, 8, 8, 8, 0,  24, 16, 8 }, /* DRV_TDE_COLOR_FMT_RGBA8888 */
    { 8, 8, 8, 8, 0,  8,  16, 24 }, /* DRV_TDE_COLOR_FMT_BGRA8888 */
    { 8, 8, 8, 8, 16, 24, 0,  8 }, /* DRV_TDE_COLOR_FMT_RABG8888 */

    { 0, 0, 0, 0, 0,  0,  0, 0 },
    { 0, 0, 0, 0, 0,  0,  0, 0 },
    { 0, 0, 0, 0, 0,  0,  0, 0 },
    { 0, 0, 0, 0, 0,  0,  0, 0 },
    { 0, 0, 0, 0, 0,  0,  0, 0 },
    { 0, 0, 0, 0, 0,  0,  0, 0 },
    { 0, 0, 0, 0, 0,  0,  0, 0 },
    { 0, 0, 0, 0, 0,  0,  0, 0 },
    { 0, 8, 8, 8, 24, 16, 8, 0 }, /* DRV_TDE_COLOR_FMT_YCBCR888 */
    { 8, 8, 8, 8, 24, 16, 8, 0 }, /* DRV_TDE_COLOR_FMT_AYCBCR8888 */
};

tde_color_fmt tde_get_common_drv_color_fmt(td_u32 count)
{
    return g_tde_common_drv_color_fmt[count];
}

static td_s32 color_convert_check(const drv_tde_fill_color *fill_color, const drv_tde_surface *sur,
                                  const td_u32 *out_color, tde_colorfmt_transform *color_trans)
{
    *color_trans = tde_osi_get_fmt_trans_type(fill_color->color_format, sur->color_format);

    if (((fill_color->color_format >= DRV_TDE_COLOR_FMT_CLUT1) &&
        (fill_color->color_format <= DRV_TDE_COLOR_FMT_A8)) ||
        (fill_color->color_format >= DRV_TDE_COLOR_FMT_YCBCR422)) {
        tde_error("Unsupported color!\n");
        return -1;
    }
    return 0;
}

typedef struct {
    td_u8 a;
    td_u8 r;
    td_u8 g;
    td_u8 b;
} tde_osi_argb;

static td_void tde_osi_get_fill_argb(const drv_tde_fill_color *fill_color, const drv_tde_surface *sur,
    tde_osi_argb *argb)
{
    argb->a = (fill_color->color_value >> g_fmt_bit_and_offset_array[fill_color->color_format].alpha_offset) &
        (0xff >> (8 - g_fmt_bit_and_offset_array[fill_color->color_format].alpha_bits)); /* 8 Data from the */
    argb->r = (fill_color->color_value >> g_fmt_bit_and_offset_array[fill_color->color_format].red_offset) &
        (0xff >> (8 - g_fmt_bit_and_offset_array[fill_color->color_format].red_bits)); /* 8 Data from the */
    argb->g = (fill_color->color_value >> g_fmt_bit_and_offset_array[fill_color->color_format].green_offset) &
        (0xff >> (8 - g_fmt_bit_and_offset_array[fill_color->color_format].green_bits)); /* 8 Data from the */
    argb->b = (fill_color->color_value >> g_fmt_bit_and_offset_array[fill_color->color_format].blue_offset) &
        (0xff >> (8 - g_fmt_bit_and_offset_array[fill_color->color_format].blue_bits)); /* 8 Data from the */

    if ((DRV_TDE_COLOR_FMT_ARGB1555 <= fill_color->color_format) &&
        (fill_color->color_format <= DRV_TDE_COLOR_FMT_BGRA1555)) {
        argb->a = (argb->a != 0) ? sur->alpha1 : sur->alpha0;
    } else {
        argb->a = argb->a << (8 - g_fmt_bit_and_offset_array[fill_color->color_format].alpha_bits); /* 8 Data bit */
    }
    argb->r = argb->r << (8 - g_fmt_bit_and_offset_array[fill_color->color_format].red_bits);   /* 8 Data bit */
    argb->g = argb->g << (8 - g_fmt_bit_and_offset_array[fill_color->color_format].green_bits); /* 8 Data bit */
    argb->b = argb->b << (8 - g_fmt_bit_and_offset_array[fill_color->color_format].blue_bits);  /* 8 Data bit */
}

static td_s32 tde_osi_color_convert(const drv_tde_fill_color *fill_color, const drv_tde_surface *sur, td_u32 *out_color)
{
    tde_osi_argb argb;
    td_u8 y;
    td_u8 cb;
    td_u8 cr;
    td_s32 ret;
    tde_colorfmt_transform color_trans;

    ret = color_convert_check(fill_color, sur, out_color, &color_trans);
    if (ret != 0) {
        return ret;
    }
    tde_osi_get_fill_argb(fill_color, sur, &argb);

    switch (color_trans) {
        case TDE_COLORFMT_TRANSFORM_ARGB_ARGB:
        case TDE_COLORFMT_TRANSFORM_YCBCR_YCBCR:
            *out_color = (argb.a << 24) + (argb.r << 16) + (argb.g << 8) + argb.b; /* 8 16 24 bit */
            return 0;
        case TDE_COLORFMT_TRANSFORM_ARGB_YCBCR:
            rgb2ycc(argb.r, argb.g, argb.b, y, cb, cr);
            *out_color = (argb.a << 24) + (y << 16) + (cb << 8) + cr; /* 8 16 24 bit */
            return 0;
        case TDE_COLORFMT_TRANSFORM_YCBCR_ARGB:
            ycc2rgb(argb.r, argb.g, argb.b, y, cb, cr);
            *out_color = (argb.a << 24) + (y << 16) + (cb << 8) + cr; /* 8 16 24 bit */
            return 0;
        default:
            tde_error("Unsupported color transport!\n");
            return -1;
    }
}

/*
 * Function:      tde_osi_check_resize_para
 * Description:   check zoom ratio limit
 * Return:        TDE_COLORFMT_CATEGORY_E   pixel format category
 */
static td_s32 tde_osi_check_resize_para(td_u32 in_width, td_u32 in_height,
                                        td_u32 out_width, td_u32 out_height)
{
    if (((in_width > TDE_MAX_RECT_WIDTH)) ||
        (in_height > TDE_MAX_RECT_HEIGHT) || ((out_width > TDE_MAX_RECT_WIDTH)) ||
        (out_height > TDE_MAX_RECT_HEIGHT)) {
        if ((in_width != out_width) || (in_height != out_height)) {
            tde_error("input width/height(%d, %d) not equal to output width/height(%d, %d) and greater than max "
                "width/height (%d, %d)\n", in_width, in_height, out_width, out_height,
                TDE_MAX_RECT_WIDTH, TDE_MAX_RECT_HEIGHT);
            return DRV_ERR_TDE_INVALID_PARA;
        }
    }

    if (((out_width * TDE_MAX_MINIFICATION_H) < in_width) ||
        ((out_height * TDE_MAX_MINIFICATION_V) < in_height)) {
        tde_error("Resize parameter error!The zoom-out ratio is less than or equal to 255!"
            "input width/height(%d, %d), output width/height(%d, %d), \n", in_width, in_height, out_width, out_height);
        return -1;
    } else {
        return 0;
    }
}

#if (TDE_CAPABILITY & ROTATE)
static td_s32 tde_osi_check_rotate_para(const drv_tde_single_src *single_src, drv_tde_rotate_angle rotate_angle)
{
    td_bool is_unsupported_format = ((single_src->src_surface->color_format != DRV_TDE_COLOR_FMT_YCBCR422) &&
                                   (single_src->src_surface->color_format != DRV_TDE_COLOR_FMT_ARGB8888) &&
                                   (single_src->src_surface->color_format != DRV_TDE_COLOR_FMT_ARGB4444) &&
                                   (single_src->src_surface->color_format != DRV_TDE_COLOR_FMT_ARGB1555));
    td_bool is_invalid;

    if (rotate_angle >= DRV_TDE_ROTATE_MAX) {
        tde_error("rotate mode error! please choose TDE_ROTATE_CLOCKWISE_90 or 180 or 270\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (single_src->src_surface->color_format != single_src->dst_surface->color_format) {
        tde_error("rotate only support src_surface->color_format == dst_surface->color_format\n");
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    if (is_unsupported_format) {
        tde_error("rotate operation can not support the format!\n");
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    is_invalid = ((single_src->src_surface->color_format == DRV_TDE_COLOR_FMT_YCBCR422) ||
        (single_src->src_surface->color_format == DRV_TDE_COLOR_FMT_PKGVYUY)) &&
        ((single_src->src_rect->height & 0x1) || (single_src->src_rect->width & 0x1));
    if (is_invalid == TD_TRUE) {
        tde_error("height, width of YCbCr422R couldn't be odd!\n");
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    is_invalid = (single_src->src_surface->phys_addr % 4) || (single_src->dst_surface->phys_addr % 4) || /* 4 align */
        (single_src->src_surface->stride % 4) || (single_src->dst_surface->stride % 4); /* 4 align */
    if (is_invalid == TD_TRUE) {
        tde_error("bitmap address is not 4 aligned nor stride is not 4 aligned!\n");
        return DRV_ERR_TDE_NOT_ALIGNED;
    }

    is_invalid = (rotate_angle != DRV_TDE_ROTATE_CLOCKWISE_180) &&
        ((single_src->src_rect->height != single_src->dst_rect->width) ||
        (single_src->src_rect->width != single_src->dst_rect->height));
    if (is_invalid == TD_TRUE) {
        tde_error("rotate operation rect is wrong!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    is_invalid = (rotate_angle == DRV_TDE_ROTATE_CLOCKWISE_180) &&
        ((single_src->src_rect->height != single_src->dst_rect->height) ||
        (single_src->src_rect->width != single_src->dst_rect->width));
    if (is_invalid == TD_TRUE) {
        tde_error("rotate 180 operation rect is wrong!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    return 0;
}
#endif

#if (TDE_CAPABILITY & COMPRESS)
static td_s32 tde_osi_check_compress_para(const drv_tde_surface *fg_surface, const drv_tde_rect *fg_rect,
                                          const drv_tde_surface *dst_surface, const drv_tde_rect *dst_rect,
                                          const drv_tde_opt *opt)
{
    ot_unused(fg_surface);
    ot_unused(fg_rect);
    ot_unused(dst_rect);
    if ((dst_surface->color_format != DRV_TDE_COLOR_FMT_ARGB1555) &&
        (dst_surface->color_format != DRV_TDE_COLOR_FMT_ARGB8888) &&
        (dst_surface->color_format != DRV_TDE_COLOR_FMT_ARGB4444) &&
        (dst_surface->color_format != DRV_TDE_COLOR_FMT_RGB888)   &&
        (dst_surface->color_format != DRV_TDE_COLOR_FMT_YCBCR888) &&
        (dst_surface->color_format != DRV_TDE_COLOR_FMT_RGB565)) {
        tde_error("Compress operation can not support the fmt !\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    if (opt->mirror) {
        tde_error("Could not support Mirror\n!");
        return -1;
    }
    if (opt->clip_mode != DRV_TDE_CLIP_MODE_NONE) {
        tde_error("Could not support Clip\n!");
        return -1;
    }
    return 0;
}

static td_s32 tde_osi_check_decompress_para(const drv_tde_surface *fg_surface, const drv_tde_rect *fg_rect,
                                            const drv_tde_surface *dst_surface, const drv_tde_rect *dst_rect,
                                            const drv_tde_opt *opt)
{
    ot_unused(fg_rect);
    ot_unused(dst_surface);
    ot_unused(dst_rect);
    if ((fg_surface->color_format != DRV_TDE_COLOR_FMT_ARGB1555) &&
        (fg_surface->color_format != DRV_TDE_COLOR_FMT_ARGB8888) &&
        (fg_surface->color_format != DRV_TDE_COLOR_FMT_ARGB4444) &&
        (fg_surface->color_format != DRV_TDE_COLOR_FMT_RGB888)   &&
        (fg_surface->color_format != DRV_TDE_COLOR_FMT_YCBCR888) &&
        (fg_surface->color_format != DRV_TDE_COLOR_FMT_RGB565)) {
        tde_error("Compress operation can not support the fmt !\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    if (opt->mirror) {
        tde_error("Could not support Mirror\n!");
        return -1;
    }
    return 0;
}

#endif

static drv_tde_color_fmt tde_osi_covert_mb_fmt(drv_tde_mb_color_fmt fmt)
{
    switch (fmt) {
        case DRV_TDE_MB_COLOR_FMT_JPG_YCBCR400MBP:
            return DRV_TDE_COLOR_FMT_JPG_YCBCR400MBP;
        case DRV_TDE_MB_COLOR_FMT_JPG_YCBCR422MBHP:
            return DRV_TDE_COLOR_FMT_JPG_YCBCR422MBHP;
        case DRV_TDE_MB_COLOR_FMT_JPG_YCBCR422MBVP:
            return DRV_TDE_COLOR_FMT_JPG_YCBCR422MBVP;
        case DRV_TDE_MB_COLOR_FMT_MP1_YCBCR420MBP:
            return DRV_TDE_COLOR_FMT_MP1_YCBCR420MBP;
        case DRV_TDE_MB_COLOR_FMT_MP2_YCBCR420MBP:
            return DRV_TDE_COLOR_FMT_MP2_YCBCR420MBP;
        case DRV_TDE_MB_COLOR_FMT_MP2_YCBCR420MBI:
            return DRV_TDE_COLOR_FMT_MP2_YCBCR420MBI;
        case DRV_TDE_MB_COLOR_FMT_JPG_YCBCR420MBP:
            return DRV_TDE_COLOR_FMT_JPG_YCBCR420MBP;
        case DRV_TDE_MB_COLOR_FMT_JPG_YCBCR444MBP:
            return DRV_TDE_COLOR_FMT_JPG_YCBCR444MBP;
        default:
            return DRV_TDE_COLOR_FMT_MAX;
    }
}

/*
 * Function:      tde_osi_get_fmt_category
 * Description:   get pixel format category info
 * Input:         fmt: pixel format
 * Return:        TDE_COLORFMT_CATEGORY_E  pixel format category
 */
static tde_colorfmt_category tde_osi_get_fmt_category(drv_tde_color_fmt fmt)
{
    /* target is ARGB format */
    if (fmt <= DRV_TDE_COLOR_FMT_RABG8888) {
        return TDE_COLORFMT_CATEGORY_ARGB;
    } else if (fmt <= DRV_TDE_COLOR_FMT_ACLUT88) {
        /* target is CLUT table format */
        return TDE_COLORFMT_CATEGORY_CLUT;
    } else if (fmt <= DRV_TDE_COLOR_FMT_A8) {
        /* target is alpha CLUT table format */
        return TDE_COLORFMT_CATEGORY_AN;
    } else if (fmt <= DRV_TDE_COLOR_FMT_PKGVYUY) {
        /* target is YCbCr format */
        return TDE_COLORFMT_CATEGORY_YCBCR;
    } else if (fmt == DRV_TDE_COLOR_FMT_BYTE) {
        /* byte format */
        return TDE_COLORFMT_CATEGORY_BYTE;
    } else if (fmt == DRV_TDE_COLOR_FMT_HALFWORD) {
        /* halfword  format */
        return TDE_COLORFMT_CATEGORY_HALFWORD;
    } else if (fmt <= DRV_TDE_COLOR_FMT_JPG_YCBCR444MBP) {
        return TDE_COLORFMT_CATEGORY_YCBCR;
    } else {
        /* error format */
        return TDE_COLORFMT_CATEGORY_BUTT;
    }
}

/*
 * Function:      tde_osi_get_fmt_trans_type
 * Description:   get pixel format transform type
 * Input:         src2_fmt: foreground pixel format
                  dst_fmt: target pixel format
 * Return:        TDE_COLORFMT_TRANSFORM_E pixel format transform type
 */
static tde_colorfmt_transform tde_osi_get_fmt_trans_type(drv_tde_color_fmt src2_fmt, drv_tde_color_fmt dst_fmt)
{
    tde_colorfmt_category src_category;
    tde_colorfmt_category dst_category;

    /* get foreground pixel format category */
    src_category = tde_osi_get_fmt_category(src2_fmt);

    /* get target pixel format category */
    dst_category = tde_osi_get_fmt_category(dst_fmt);

    switch (src_category) {
        case TDE_COLORFMT_CATEGORY_ARGB:
            if (dst_category == TDE_COLORFMT_CATEGORY_ARGB) {
                return TDE_COLORFMT_TRANSFORM_ARGB_ARGB;
            } else if (dst_category == TDE_COLORFMT_CATEGORY_YCBCR) {
                return TDE_COLORFMT_TRANSFORM_ARGB_YCBCR;
            } else if (dst_category == TDE_COLORFMT_CATEGORY_AN) {
                return TDE_COLORFMT_TRANSFORM_ARGB_AN;
            }
            return TDE_COLORFMT_TRANSFORM_BUTT;

        case TDE_COLORFMT_CATEGORY_CLUT:
            if (dst_category == TDE_COLORFMT_CATEGORY_ARGB) {
                return TDE_COLORFMT_TRANSFORM_CLUT_ARGB;
            } else if (dst_category == TDE_COLORFMT_CATEGORY_YCBCR) {
                return TDE_COLORFMT_TRANSFORM_CLUT_YCBCR;
            } else if (dst_category == TDE_COLORFMT_CATEGORY_CLUT) {
                return TDE_COLORFMT_TRANSFORM_CLUT_CLUT;
            }
            return TDE_COLORFMT_TRANSFORM_BUTT;

        case TDE_COLORFMT_CATEGORY_YCBCR:
            if (dst_category == TDE_COLORFMT_CATEGORY_ARGB) {
                return TDE_COLORFMT_TRANSFORM_YCBCR_ARGB;
            } else if (dst_category == TDE_COLORFMT_CATEGORY_YCBCR) {
                return TDE_COLORFMT_TRANSFORM_YCBCR_YCBCR;
            } else if (dst_category == TDE_COLORFMT_CATEGORY_AN) {
                return TDE_COLORFMT_TRANSFORM_ARGB_AN;
            }
            return TDE_COLORFMT_TRANSFORM_BUTT;

        case TDE_COLORFMT_CATEGORY_AN:
            if (dst_category == TDE_COLORFMT_CATEGORY_AN) {
                return TDE_COLORFMT_TRANSFORM_AN_AN;
            }
            return TDE_COLORFMT_TRANSFORM_BUTT;

        default:
            return TDE_COLORFMT_TRANSFORM_BUTT;
    }
}

/*
 * Function:      tde_osi_is_single_src_to_rop
 * Description:   query if ROP operate is if single source2 operate
 * Input:         rop: rop operate type
 * Return:        TD_TRUE: single ROP;TD_FALSE: non single ROP
 */
#if (TDE_CAPABILITY & ROP)
static td_bool tde_osi_is_single_src_to_rop(drv_tde_rop_mode rop)
{
    switch (rop) {
        case DRV_TDE_ROP_BLACK:
        case DRV_TDE_ROP_NOTCOPYPEN:
        case DRV_TDE_ROP_COPYPEN:
        case DRV_TDE_ROP_WHITE:
            return TD_TRUE;

        default:
            return TD_FALSE;
    }
}
#endif

/*
 * Function:      tde_osi_get_clut_usage
 * Description:   get CLUT table usage
 * Input:         src_fmt foreground pixel format
                  dst_fmt  target pixel format
 * Return:        TDE_CLUT_USAGE_E:  clut  usage
 */
static tde_clut_usage tde_osi_get_clut_usage(drv_tde_color_fmt src_fmt, drv_tde_color_fmt dst_fmt)
{
    tde_colorfmt_transform color_trans_type;

    color_trans_type = tde_osi_get_fmt_trans_type(src_fmt, dst_fmt);

    switch (color_trans_type) {
        case TDE_COLORFMT_TRANSFORM_CLUT_ARGB:
        case TDE_COLORFMT_TRANSFORM_CLUT_YCBCR:
            return TDE_CLUT_COLOREXPENDING; /* color expand */

        case TDE_COLORFMT_TRANSFORM_ARGB_ARGB:
        case TDE_COLORFMT_TRANSFORM_YCBCR_YCBCR:
            return TDE_CLUT_COLORCORRECT; /* color adjust */

        case TDE_COLORFMT_TRANSFORM_CLUT_CLUT:
            return TDE_CLUT_CLUT_BYPASS;

        default:
            return TDE_CLUT_USAGE_BUTT;
    }
}

static td_bool tde_osi_whether_contain_alpha(drv_tde_color_fmt color_format)
{
    switch (color_format) {
        case DRV_TDE_COLOR_FMT_ARGB4444:
        case DRV_TDE_COLOR_FMT_ABGR4444:
        case DRV_TDE_COLOR_FMT_RGBA4444:
        case DRV_TDE_COLOR_FMT_BGRA4444:
        case DRV_TDE_COLOR_FMT_ARGB1555:
        case DRV_TDE_COLOR_FMT_ABGR1555:
        case DRV_TDE_COLOR_FMT_RGBA1555:
        case DRV_TDE_COLOR_FMT_BGRA1555:
        case DRV_TDE_COLOR_FMT_ARGB8565:
        case DRV_TDE_COLOR_FMT_ABGR8565:
        case DRV_TDE_COLOR_FMT_RGBA8565:
        case DRV_TDE_COLOR_FMT_BGRA8565:
        case DRV_TDE_COLOR_FMT_ARGB8888:
        case DRV_TDE_COLOR_FMT_ABGR8888:
        case DRV_TDE_COLOR_FMT_RGBA8888:
        case DRV_TDE_COLOR_FMT_BGRA8888:
        case DRV_TDE_COLOR_FMT_AYCBCR8888:
        case DRV_TDE_COLOR_FMT_RABG8888:
            return TD_TRUE;
        default:
            return TD_FALSE;
    }
}

static td_s32 tde_osi_set_clut_opt(const drv_tde_surface *clut_sur, const drv_tde_surface *out_sur,
                                   tde_clut_usage *pen_clut_usage, td_bool clut_reload, tde_hw_node *hw_node)
{
    tde_colorfmt_category fmt_cate;
    tde_clut_cmd clut_cmd;
    td_phys_addr_t clut_phyaddr;

    if (!((clut_sur->clut_phys_addr != (td_phys_addr_t)0xffffffffffffffffUL) && (clut_sur->clut_phys_addr != 0))) {
        return TD_SUCCESS;
    }
    clut_phyaddr = clut_sur->clut_phys_addr;
    fmt_cate = tde_osi_get_fmt_category(out_sur->color_format);
    /* when user input the type of clut is not consistent with output format,return error */
    if ((!clut_sur->is_ycbcr_clut && (fmt_cate == TDE_COLORFMT_CATEGORY_YCBCR)) ||
        (clut_sur->is_ycbcr_clut && (fmt_cate == TDE_COLORFMT_CATEGORY_ARGB))) {
        tde_error("clut fmt not same\n");
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    /*
     * Because of unsupported output CSC, input and background color zone is consistent.
     * In trine sources operation,clut need use background, so output color zone instand of background's
     */
    *pen_clut_usage = tde_osi_get_clut_usage(clut_sur->color_format, out_sur->color_format);

    if (*pen_clut_usage >= TDE_CLUT_CLUT_BYPASS) {
        return TD_SUCCESS;
    }

    if (*pen_clut_usage == TDE_CLUT_COLOREXPENDING) {
        clut_cmd.clut_mode = TDE_COLOR_EXP_CLUT_MODE;
    } else {
        clut_cmd.clut_mode = TDE_COLOR_CORRCT_CLUT_MODE;
    }

    if (osal_div_u64_rem(clut_phyaddr, 4)) { /* 4 alg data */
        tde_error("clut_phys_addr=0x%lx is not aligned!\n", (td_ulong)clut_sur->clut_phys_addr);
        return DRV_ERR_TDE_NOT_ALIGNED;
    }

    clut_cmd.phy_clut_addr = clut_sur->clut_phys_addr;

    if (tde_hal_node_set_clut_opt(hw_node, &clut_cmd, clut_reload) != TD_SUCCESS) {
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    return TD_SUCCESS;
}

static td_s32 tde_osi_check_single_opt(const drv_tde_opt *opt)
{
    td_bool real = (((opt->blend_opt.global_alpha_en != TD_TRUE) && (opt->blend_opt.global_alpha_en != TD_FALSE)) ||
        ((opt->blend_opt.pixel_alpha_en != TD_TRUE) && (opt->blend_opt.pixel_alpha_en != TD_FALSE)) ||
        ((opt->blend_opt.src1_alpha_premulti != TD_TRUE) && (opt->blend_opt.src1_alpha_premulti != TD_FALSE)) ||
        ((opt->blend_opt.src2_alpha_premulti != TD_TRUE) && (opt->blend_opt.src2_alpha_premulti != TD_FALSE)) ||
        ((opt->clut_reload != TD_TRUE) && (opt->clut_reload != TD_FALSE)) ||
        ((opt->resize != TD_TRUE) && (opt->resize != TD_FALSE)) ||
        ((opt->is_compress != TD_TRUE) && (opt->is_compress != TD_FALSE)) ||
        ((opt->is_decompress != TD_TRUE) && (opt->is_decompress != TD_FALSE)));

    /* return error, if enable color key */
    if (opt->colorkey_mode != DRV_TDE_COLOR_KEY_MODE_NONE) {
        tde_error("It doesn't support colorkey in single source mode!\n");
        return -1;
    }

    if (opt->alpha_blending_cmd >= DRV_TDE_ALPHA_BLENDING_MAX) {
        tde_error("alpha_blending_cmd error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (real) {
        tde_error("bool should be TRUE or FALSE!!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if ((opt->deflicker_mode >= DRV_TDE_DEFLICKER_LEVEL_MODE_MAX) ||
        (opt->deflicker_mode < DRV_TDE_DEFLICKER_LEVEL_MODE_NONE)) {
        tde_error("deflicker_mode error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if ((opt->mirror >= DRV_TDE_MIRROR_MAX) || (opt->mirror < DRV_TDE_MIRROR_NONE)) {
        tde_error("mirror error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if ((opt->filter_mode >= DRV_TDE_FILTER_MODE_MAX) || (opt->filter_mode < DRV_TDE_FILTER_MODE_COLOR)) {
        tde_error("filter_mode error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    return TD_SUCCESS;
}
static td_s32 tde_osi_check_dst_fmt(drv_tde_color_fmt fmt)
{
    if (((fmt >= DRV_TDE_COLOR_FMT_JPG_YCBCR400MBP) && (fmt <= DRV_TDE_COLOR_FMT_JPG_YCBCR422MBVP)) ||
        (fmt >= DRV_TDE_COLOR_FMT_JPG_YCBCR444MBP) || ((fmt >= DRV_TDE_COLOR_FMT_CLUT1) &&
        (fmt <= DRV_TDE_COLOR_FMT_A8))) {
        tde_error("This fmt %d doesn't support!\n", fmt);
        return DRV_ERR_TDE_INVALID_PARA;
    }
    return TD_SUCCESS;
}
/*
 * Function:      tde_osi_check_single_src_to_opt
 * Description:   check if valid of foreground single source operate
 * Input:         src2_fmt foreground pixel format
                  dst_fmt  target pixel format
                  opt     operate attribute pointer
 * Return:        0  valid parameter;
                  -1 invalid parameter;
 * Others:        none
 */
static td_s32 tde_osi_check_single_src_to_opt(drv_tde_color_fmt src2_fmt, drv_tde_color_fmt dst_fmt,
                                              const drv_tde_opt *opt)
{
    tde_colorfmt_transform color_trans_type;
    td_s32 ret = tde_osi_check_single_opt(opt);
    if (ret != TD_SUCCESS) {
        return ret;
    }

#if (TDE_CAPABILITY & ROP)
    /* if operate type is ROP and it is not single operate,return error */
    if ((td_u32)opt->alpha_blending_cmd & DRV_TDE_ALPHA_BLENDING_ROP) {
        if (((opt->rop_alpha >= DRV_TDE_ROP_MAX) || (opt->rop_alpha < DRV_TDE_ROP_BLACK)) ||
            ((opt->rop_color >= DRV_TDE_ROP_MAX) || (opt->rop_color < DRV_TDE_ROP_BLACK))) {
            return DRV_ERR_TDE_INVALID_PARA;
        }

        if ((!tde_osi_is_single_src_to_rop(opt->rop_alpha)) || (!tde_osi_is_single_src_to_rop(opt->rop_color))) {
            tde_error("Only support single s2 rop!\n");
            return -1;
        }
    }
#endif
    /* single source can not do blend operate */
    if ((td_u32)(opt->alpha_blending_cmd) & DRV_TDE_ALPHA_BLENDING_BLEND) {
        tde_error("Alu mode error!\n");
        return -1;
    }
    ret = tde_osi_check_dst_fmt(dst_fmt);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    color_trans_type = tde_osi_get_fmt_trans_type(src2_fmt, dst_fmt);
    if (color_trans_type == TDE_COLORFMT_TRANSFORM_BUTT) {
        tde_error("Unknown color transport type!\n");
        return -1;
    }

    if (color_trans_type == TDE_COLORFMT_TRANSFORM_CLUT_CLUT) {
        /* unsupported deflicker,zoom, Rop, mirror,colorize */
        if ((opt->resize) || (opt->deflicker_mode != DRV_TDE_DEFLICKER_LEVEL_MODE_NONE) ||
            ((td_u32)opt->alpha_blending_cmd & DRV_TDE_ALPHA_BLENDING_ROP) ||
            ((td_u32)opt->alpha_blending_cmd & DRV_TDE_ALPHA_BLENDING_COLORIZE)) {
            tde_error("It doesn't support deflicker or ROP or mirror!\n");
            return -1;
        }
    }

    return 0;
}

static td_s32 tde_osi_check_bool_single_opt(const drv_tde_opt *opt)
{
    td_bool real = (((opt->blend_opt.global_alpha_en != TD_TRUE) && (opt->blend_opt.global_alpha_en != TD_FALSE)) ||
        ((opt->blend_opt.pixel_alpha_en != TD_TRUE) && (opt->blend_opt.pixel_alpha_en != TD_FALSE)) ||
        ((opt->blend_opt.src1_alpha_premulti != TD_TRUE) && (opt->blend_opt.src1_alpha_premulti != TD_FALSE)) ||
        ((opt->blend_opt.src2_alpha_premulti != TD_TRUE) && (opt->blend_opt.src2_alpha_premulti != TD_FALSE)) ||
        ((opt->clut_reload != TD_TRUE) && (opt->clut_reload != TD_FALSE)) ||
        ((opt->resize != TD_TRUE) && (opt->resize != TD_FALSE)) ||
        ((opt->is_compress != TD_TRUE) && (opt->is_compress != TD_FALSE)) ||
        ((opt->is_decompress != TD_TRUE) && (opt->is_decompress != TD_FALSE)));
    if (real) {
        tde_error("bool should be TRUE or FALSE!!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if ((opt->deflicker_mode >= DRV_TDE_DEFLICKER_LEVEL_MODE_MAX) ||
        (opt->deflicker_mode < DRV_TDE_DEFLICKER_LEVEL_MODE_NONE)) {
        tde_error("deflicker_mode (%d) error!should be in [%d,%d]\n", opt->deflicker_mode,
            DRV_TDE_DEFLICKER_LEVEL_MODE_NONE, DRV_TDE_DEFLICKER_LEVEL_MODE_BOTH);
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if ((opt->mirror >= DRV_TDE_MIRROR_MAX) || (opt->mirror < DRV_TDE_MIRROR_NONE)) {
        tde_error("mirror (%d) error!should be in [%d,%d]\n", opt->mirror,
            DRV_TDE_MIRROR_NONE, DRV_TDE_MIRROR_BOTH);
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if ((opt->filter_mode >= DRV_TDE_FILTER_MODE_MAX) || (opt->filter_mode < DRV_TDE_FILTER_MODE_COLOR)) {
        tde_error("filter_mode (%d) error!should be in [%d,%d]\n", opt->filter_mode,
            DRV_TDE_FILTER_MODE_COLOR, DRV_TDE_FILTER_MODE_NONE);
        return DRV_ERR_TDE_INVALID_PARA;
    }
    return TD_SUCCESS;
}

static td_s32 tde_osi_check_fmt_single_opt(drv_tde_color_fmt src1_fmt, const drv_tde_opt *opt)
{
    td_s32 ret;

    if ((src1_fmt == DRV_TDE_COLOR_FMT_YCBCR422) || (src1_fmt == DRV_TDE_COLOR_FMT_PKGVYUY)) {
        tde_error("This operation doesn't support PKG!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    if (opt->alpha_blending_cmd >= DRV_TDE_ALPHA_BLENDING_MAX) {
        tde_error("alpha_blending_cmd (%d) error!\n", opt->alpha_blending_cmd);
        return DRV_ERR_TDE_INVALID_PARA;
    }
    ret = tde_osi_check_bool_single_opt(opt);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    return TD_SUCCESS;
}

static td_s32 tde_osi_check_category(tde_colorfmt_category src1_category, tde_colorfmt_category src2_category,
    tde_colorfmt_category dst_category, const drv_tde_opt *opt, td_bool temp_fmt)
{
    td_bool is_invalid;

    is_invalid = (src1_category >= TDE_COLORFMT_CATEGORY_BYTE) || (src2_category >= TDE_COLORFMT_CATEGORY_BYTE) ||
        (dst_category >= TDE_COLORFMT_CATEGORY_BYTE);
    if (is_invalid == TD_TRUE) {
        tde_error("unknown format!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    is_invalid = ((src1_category == TDE_COLORFMT_CATEGORY_ARGB) || (src1_category == TDE_COLORFMT_CATEGORY_YCBCR)) &&
        (src2_category == TDE_COLORFMT_CATEGORY_AN) && (!temp_fmt);
    if (is_invalid == TD_TRUE) {
        tde_error("Target must have alpha component!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    is_invalid = (src1_category == TDE_COLORFMT_CATEGORY_CLUT) && ((src2_category != TDE_COLORFMT_CATEGORY_CLUT) ||
        (dst_category != TDE_COLORFMT_CATEGORY_CLUT));
    if (is_invalid == TD_TRUE) {
        tde_error("Unsupported operation!\n");
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    is_invalid = ((opt->deflicker_mode != DRV_TDE_DEFLICKER_LEVEL_MODE_NONE) || (opt->resize) ||
        (opt->alpha_blending_cmd != DRV_TDE_ALPHA_BLENDING_NONE)) && (src1_category == TDE_COLORFMT_CATEGORY_CLUT);
    if (is_invalid == TD_TRUE) {
        tde_error("It doesn't support deflicker or ROP or mirror!\n");
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    is_invalid = (src1_category == TDE_COLORFMT_CATEGORY_AN) && (src2_category == TDE_COLORFMT_CATEGORY_AN) &&
        (dst_category == TDE_COLORFMT_CATEGORY_AN) && (opt->alpha_blending_cmd != DRV_TDE_ALPHA_BLENDING_NONE);
    if (is_invalid == TD_TRUE) {
        tde_error("It doesn't support ROP or mirror!\n");
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    return TD_SUCCESS;
}

static td_s32 tde_osi_check_cmd_single_opt(const drv_tde_opt *opt)
{
#if (TDE_CAPABILITY & ROP)
    if ((td_u32)opt->alpha_blending_cmd & DRV_TDE_ALPHA_BLENDING_ROP) {
        if (((opt->rop_alpha >= DRV_TDE_ROP_MAX) || (opt->rop_alpha < DRV_TDE_ROP_BLACK)) ||
            ((opt->rop_color >= DRV_TDE_ROP_MAX) || (opt->rop_color < DRV_TDE_ROP_BLACK))) {
            tde_error("enRopCode error!\n");
            return DRV_ERR_TDE_INVALID_PARA;
        }
    }
#endif
    if ((td_u32)opt->alpha_blending_cmd & DRV_TDE_ALPHA_BLENDING_BLEND) {
        if ((opt->blend_opt.blend_cmd >= DRV_TDE_BLEND_CMD_MAX) ||
            (opt->blend_opt.blend_cmd < DRV_TDE_BLEND_CMD_NONE)) {
            tde_error("Unknown blend cmd!\n");
            return DRV_ERR_TDE_INVALID_PARA;
        }

        if (opt->blend_opt.blend_cmd == DRV_TDE_BLEND_CMD_CONFIG) {
            if ((opt->blend_opt.src1_blend_mode >= DRV_TDE_BLEND_MAX) ||
                (opt->blend_opt.src2_blend_mode >= DRV_TDE_BLEND_MAX)) {
                tde_error("Unknown blend mode!\n");
                return DRV_ERR_TDE_INVALID_PARA;
            }
        }
    }
    return TD_SUCCESS;
}

/*
 * Function:      tde_osi_check_double_src_opt
 * Description:   check if valid of dual source operate
 * Input:         src1_fmt background pixel format
                  src2_fmt foreground pixel format
                  dst_fmt  target pixel format
                  opt    operate attribute operate
 * Return:        0  valid parameter;
                  -1 invalid parameter;
 */
static td_s32 tde_osi_check_double_src_opt(drv_tde_color_fmt src1_fmt, drv_tde_color_fmt src2_fmt,
                                           drv_tde_color_fmt dst_fmt, const drv_tde_opt *opt)
{
    tde_colorfmt_category src1_category;
    tde_colorfmt_category src2_category;
    tde_colorfmt_category dst_category;
    td_bool temp_fmt;
    td_s32 ret = tde_osi_check_dst_fmt(dst_fmt);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = tde_osi_check_fmt_single_opt(src1_fmt, opt);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    /* get background pixel format category */
    src1_category = tde_osi_get_fmt_category(src1_fmt);

    /* get foreground pixel format category  */
    src2_category = tde_osi_get_fmt_category(src2_fmt);

    /* get target pixel format category  */
    dst_category = tde_osi_get_fmt_category(dst_fmt);

    temp_fmt = tde_osi_whether_contain_alpha(dst_fmt);

    ret = tde_osi_check_category(src1_category, src2_category, dst_category, opt, temp_fmt);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = tde_osi_check_cmd_single_opt(opt);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    return TD_SUCCESS;
}

static td_u16 tde_osi_get_code1(tde_colorfmt_transform color_trans_type)
{
    switch (color_trans_type) {
        case TDE_COLORFMT_TRANSFORM_ARGB_ARGB:
            return 0x0;
        case TDE_COLORFMT_TRANSFORM_ARGB_YCBCR:
            return 0x5;
        case TDE_COLORFMT_TRANSFORM_CLUT_ARGB:
            return 0x8;
        case TDE_COLORFMT_TRANSFORM_CLUT_YCBCR:
            return 0x8 | 0x10 | 0x1;
        case TDE_COLORFMT_TRANSFORM_YCBCR_ARGB:
            return 0x1;
        case TDE_COLORFMT_TRANSFORM_YCBCR_YCBCR:
            return 0x0;
        default:
            return 0x8000;
    }
}

static td_u16 tde_osi_get_code2(tde_colorfmt_transform color_trans_type, drv_tde_color_fmt src2_fmt)
{
    td_u16 code2;
    switch (color_trans_type) {
        case TDE_COLORFMT_TRANSFORM_ARGB_ARGB:
            return 0x0;

        case TDE_COLORFMT_TRANSFORM_ARGB_YCBCR:
            return 0x2;

        case TDE_COLORFMT_TRANSFORM_CLUT_ARGB:
            return 0x8;

        case TDE_COLORFMT_TRANSFORM_CLUT_YCBCR:
            return 0xa;

        case TDE_COLORFMT_TRANSFORM_YCBCR_ARGB:
            code2 = 0x2 | 0x4;
            if ((src2_fmt >= DRV_TDE_COLOR_FMT_CLUT1) && (src2_fmt <= DRV_TDE_COLOR_FMT_ACLUT88)) {
                code2 = 0;
            }
            return code2;

        case TDE_COLORFMT_TRANSFORM_YCBCR_YCBCR:
            code2 = 0x0;
            if ((src2_fmt >= DRV_TDE_COLOR_FMT_CLUT1) && (src2_fmt <= DRV_TDE_COLOR_FMT_ACLUT88)) {
                code2 = 0x2;
            }
            return code2;

        default:
            return 0x8000;
    }
}

/*
 * Function:      tde_osi_double_src_get_opt_code
 * Description:   get dual source operate encode
 * Input:         src1_fmt background pixel format
                  src2_fmt foreground pixel format
                  dst_fmt: target pixel format
 * Return:        code value
 */
static td_u16 tde_osi_double_src_get_opt_code(drv_tde_color_fmt src1_fmt, drv_tde_color_fmt src2_fmt,
                                              drv_tde_color_fmt dst_fmt)
{
    td_u16 code1;
    td_u16 code2;
    tde_colorfmt_transform color_trans_type;

    color_trans_type = tde_osi_get_fmt_trans_type(src2_fmt, src1_fmt);
    code1 = tde_osi_get_code1(color_trans_type);
    if (code1 == 0x8000) {
        return code1;
    }

    color_trans_type = tde_osi_get_fmt_trans_type(src1_fmt, dst_fmt);

    code2 = tde_osi_get_code2(color_trans_type, src2_fmt);
    if (code2 == 0x8000) {
        return code2;
    }
    return (code1 | code2);
}

static td_u16 tde_osi_single_src2_get_opt_code(drv_tde_color_fmt src2_fmt, drv_tde_color_fmt dst_fmt)
{
    td_u16 code;
    tde_colorfmt_transform color_trans_type;

    color_trans_type = tde_osi_get_fmt_trans_type(src2_fmt, dst_fmt);

    switch (color_trans_type) {
        case TDE_COLORFMT_TRANSFORM_ARGB_ARGB:
            code = 0x0;
            break;

        case TDE_COLORFMT_TRANSFORM_ARGB_YCBCR:
            code = 0x5;
            break;

        case TDE_COLORFMT_TRANSFORM_CLUT_ARGB:
            code = 0x8;
            break;

        case TDE_COLORFMT_TRANSFORM_CLUT_CLUT:
            code = 0x0;
            break;

        case TDE_COLORFMT_TRANSFORM_CLUT_YCBCR:
            code = 0xA;
            break;

        case TDE_COLORFMT_TRANSFORM_YCBCR_ARGB:
            code = 0x1;
            break;

        case TDE_COLORFMT_TRANSFORM_YCBCR_YCBCR:
            code = 0x0;
            break;

        default:
            code = 0x8000;
    }
    return code;
}

/*
 * Function:      tde_osi_get_conv_by_code
 * Description:   get format conversion manner by format conversion code
 * Input:         code  format conversion code
 *                conv  format conversion struct
 * Return:        encode value
 */
static td_void tde_osi_get_conv_by_code(td_u16 code, tde_conv_mode_cmd *conv)
{
    conv->in_conv = code & 0x1;
    conv->out_conv = (code >> 1) & 0x1;
    conv->in_rgb2_yc = ((code >> 2) & 0x1);   /* 2 Expand the digits */
    conv->in_src1_conv = ((code >> 4) & 0x1); /* 4 Expand the digits */

    return;
}

/*
 * Function:      tde_osi_get_bpp_by_fmt
 * Description:   get pixel bit of pixel format
 * Input:         fmt  target pixel format
 * Return:        -1 fail; other:pixel bit
 */
td_s32 tde_osi_get_bpp_by_fmt(drv_tde_color_fmt fmt)
{
    if (tde_osi_get_bpp_by_fmt_16(fmt) == 16) { /* 16 bpp fmt */
        return 16;
    } else if (tde_osi_get_bpp_by_fmt_24(fmt) == 24) { /* 24 bpp fmt */
        return 24;
    } else if (tde_osi_get_bpp_by_fmt_32(fmt) == 32) { /* 32 bpp fmt */
        return 32;
    } else if (tde_osi_get_bpp_by_fmt_1(fmt) == 1) {
        return 1;
    } else if (tde_osi_get_bpp_by_fmt_2(fmt) == 2) { /* 2 bpp fmt */
        return 2;
    } else if (tde_osi_get_bpp_by_fmt_4(fmt) == 4) { /* 4 bpp fmt */
        return 4;
    } else if (tde_osi_get_bpp_by_fmt_8(fmt) == 8) { /* 8 bpp fmt */
        return 8;
    } else {
        return -1;
    }
}

static td_s32 tde_osi_get_bpp_by_fmt_16(drv_tde_color_fmt fmt)
{
    switch (fmt) {
        case DRV_TDE_COLOR_FMT_RGB444:
        case DRV_TDE_COLOR_FMT_BGR444:
        case DRV_TDE_COLOR_FMT_RGB555:
        case DRV_TDE_COLOR_FMT_BGR555:
        case DRV_TDE_COLOR_FMT_RGB565:
        case DRV_TDE_COLOR_FMT_BGR565:
        case DRV_TDE_COLOR_FMT_ARGB4444:
        case DRV_TDE_COLOR_FMT_ABGR4444:
        case DRV_TDE_COLOR_FMT_RGBA4444:
        case DRV_TDE_COLOR_FMT_BGRA4444:
        case DRV_TDE_COLOR_FMT_ARGB1555:
        case DRV_TDE_COLOR_FMT_ABGR1555:
        case DRV_TDE_COLOR_FMT_RGBA1555:
        case DRV_TDE_COLOR_FMT_BGRA1555:
        case DRV_TDE_COLOR_FMT_ACLUT88:
        case DRV_TDE_COLOR_FMT_YCBCR422:
        case DRV_TDE_COLOR_FMT_HALFWORD:
        case DRV_TDE_COLOR_FMT_PKGVYUY:
            return 16; /* 16 bpp fmt */
        default:
            return -1;
    }
}

static td_s32 tde_osi_get_bpp_by_fmt_24(drv_tde_color_fmt fmt)
{
    switch (fmt) {
        case DRV_TDE_COLOR_FMT_RGB888:
        case DRV_TDE_COLOR_FMT_BGR888:
        case DRV_TDE_COLOR_FMT_ARGB8565:
        case DRV_TDE_COLOR_FMT_ABGR8565:
        case DRV_TDE_COLOR_FMT_RGBA8565:
        case DRV_TDE_COLOR_FMT_BGRA8565:
        case DRV_TDE_COLOR_FMT_YCBCR888:
            return 24; /* 24 bpp fmt */
        default:
            return -1;
    }
}

static td_s32 tde_osi_get_bpp_by_fmt_32(drv_tde_color_fmt fmt)
{
    switch (fmt) {
        case DRV_TDE_COLOR_FMT_ARGB8888:
        case DRV_TDE_COLOR_FMT_ABGR8888:
        case DRV_TDE_COLOR_FMT_RGBA8888:
        case DRV_TDE_COLOR_FMT_BGRA8888:
        case DRV_TDE_COLOR_FMT_AYCBCR8888:
        case DRV_TDE_COLOR_FMT_RABG8888:
            return 32; /* 32 bpp fmt */
        default:
            return -1;
    }
}

static td_s32 tde_osi_get_bpp_by_fmt_8(drv_tde_color_fmt fmt)
{
    switch (fmt) {
        case DRV_TDE_COLOR_FMT_CLUT8:
        case DRV_TDE_COLOR_FMT_ACLUT44:
        case DRV_TDE_COLOR_FMT_A8:
        case DRV_TDE_COLOR_FMT_BYTE:
            return 8; /* 8 bpp fmt */
        default:
            return -1;
    }
}

static td_s32 tde_osi_get_bpp_by_fmt_4(drv_tde_color_fmt fmt)
{
    switch (fmt) {
        case DRV_TDE_COLOR_FMT_CLUT4:
            return 4; /* 4 bpp fmt */
        default:
            return -1;
    }
}

static td_s32 tde_osi_get_bpp_by_fmt_2(drv_tde_color_fmt fmt)
{
    switch (fmt) {
        case DRV_TDE_COLOR_FMT_CLUT2:
            return 2; /* 2 bpp fmt */
        default:
            return -1;
    }
}

static td_s32 tde_osi_get_bpp_by_fmt_1(drv_tde_color_fmt fmt)
{
    switch (fmt) {
        case DRV_TDE_COLOR_FMT_CLUT1:
        case DRV_TDE_COLOR_FMT_A1:
            return 1;
        default:
            return -1;
    }
}

/*
 * Function:      tde_osi_check_src
 * Description:   get scanning direction, avoid lap
 * Input:         pSrc source bitmap
                  dst_surface target bitmap
                  mirror mirror type
 * Output:        pstSrcDirection source scanning information
                  pstDstDirection target scanning information
 * Return:        0  success
                  -1 fail
 * Others:        add  antiscan handle to YCbCr422R
 */
static td_s32 tde_osi_check_src(const drv_tde_single_src *single_src, const tde_scandirection_mode *src_direction,
                                const tde_scandirection_mode *dst_direction)
{
    td_s32 bpp;

    if (single_src->src_surface == TD_NULL) {
        tde_error("single_src->src_surface is null !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }

    if (single_src->src_rect == TD_NULL) {
        tde_error("single_src->src_rect is null !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }

    if (single_src->dst_surface == TD_NULL) {
        tde_error("single_src->dst_surface is null !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }

    if (single_src->dst_rect == TD_NULL) {
        tde_error("single_src->dst_rect is null !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }

    bpp = tde_osi_get_bpp_by_fmt(single_src->dst_surface->color_format);
    if (bpp < 0) {
        tde_error("Unknown color format!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (bpp < 8) { /* 8 is bpp */
        /* when writing, 8 subbyte format align ask start point byte align */
        if ((single_src->dst_rect->pos_x * bpp % 8) || (single_src->dst_rect->width * bpp % 8)) {
            tde_error("The input start or write width does not meet the 8 single byte alignment!\n");
            return DRV_ERR_TDE_INVALID_PARA;
        }
    }
    return TD_SUCCESS;
}

static td_void tde_check_reverse_scan(td_phys_addr_t src_addr, td_phys_addr_t dst_addr,
    tde_scandirection_mode *src_direction, tde_scandirection_mode *dst_direction)
{
    /* There is no overlap between target and source. */
    if ((src_addr <= dst_addr)) {
        src_direction->ver_scan = TDE_SCAN_DOWN_UP;
        dst_direction->ver_scan = TDE_SCAN_DOWN_UP;

        src_direction->hor_scan = TDE_SCAN_RIGHT_LEFT;
        dst_direction->hor_scan = TDE_SCAN_RIGHT_LEFT;
    }
    return;
}

static td_void tde_osi_get_src_direction(drv_tde_mirror_mode mirror, tde_scandirection_mode *src_direction)
{
    switch (mirror) {
        case DRV_TDE_MIRROR_HORIZONTAL:
            src_direction->hor_scan = !(src_direction->hor_scan);
            break;
        case DRV_TDE_MIRROR_VERTICAL:
            src_direction->ver_scan = !(src_direction->ver_scan);
            break;
        case DRV_TDE_MIRROR_BOTH:
            src_direction->hor_scan = !(src_direction->hor_scan);
            src_direction->ver_scan = !(src_direction->ver_scan);
            break;
        default:
            break;
    }
}

static td_s32 tde_osi_get_scan_info_ex(const drv_tde_single_src *single_src, const drv_tde_opt *opt,
    tde_scandirection_mode *src_direction, tde_scandirection_mode *dst_direction)
{
    td_phys_addr_t src_addr;
    td_phys_addr_t dst_addr;
    drv_tde_mirror_mode mirror = DRV_TDE_MIRROR_NONE;
    td_s32 srcd_bpp;
    td_s32 dst_bpp;
    td_s32 ret;

    ret = tde_osi_check_src(single_src, src_direction, dst_direction);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    /* default scanning direction */
    src_direction->hor_scan = TDE_SCAN_LEFT_RIGHT;
    src_direction->ver_scan = TDE_SCAN_UP_DOWN;
    dst_direction->hor_scan = TDE_SCAN_LEFT_RIGHT;
    dst_direction->ver_scan = TDE_SCAN_UP_DOWN;

    if (opt != TD_NULL) {
        mirror = opt->mirror;
    }

    if (mirror != DRV_TDE_MIRROR_NONE) {
        tde_osi_get_src_direction(mirror, src_direction);
    } else {
        if ((opt != TD_NULL) && (opt->clip_mode == DRV_TDE_CLIP_MODE_OUTSIDE)) {
            return 0;
        }

        /* only if stride is the same, can be do conversion */
        if ((single_src->src_surface->stride != single_src->dst_surface->stride) ||
            (single_src->src_surface->color_format >= DRV_TDE_COLOR_FMT_JPG_YCBCR400MBP)) {
            return 0;
        }
        srcd_bpp = tde_osi_get_bpp_by_fmt(single_src->src_surface->color_format);
        dst_bpp = tde_osi_get_bpp_by_fmt(single_src->dst_surface->color_format);
        if ((dst_bpp < 0) || (srcd_bpp < 0)) {
            tde_error("bits per pixel less than 0!src fmt:%d,dst fmt:%d\n",
                single_src->src_surface->color_format, single_src->dst_surface->color_format);
            return -1;
        }

        src_addr = single_src->src_surface->phys_addr + single_src->src_rect->pos_y *
            single_src->src_surface->stride + ((single_src->src_rect->pos_x * srcd_bpp) / 8); /* 8 bits */
        dst_addr = single_src->dst_surface->phys_addr + single_src->dst_rect->pos_y *
            /* 8 bits */
            single_src->dst_surface->stride  + ((single_src->dst_rect->pos_x * dst_bpp) / 8);

        /* clip和反向不能同时打开，效果会异常 */
        if ((opt != TD_NULL) && (opt->clip_mode != DRV_TDE_CLIP_MODE_NONE) && (opt->resize)) {
            return 0;
        }

        /* source is above of target or on the left of the same direction */
        tde_check_reverse_scan(src_addr, dst_addr, src_direction, dst_direction);
    }
    return 0;
}

/*
 * Function:      tde_osi_get_inter_rect
 * Description:   get inter rect of two rectangles
 * Output:        inter_rect output inter rectangle
 * Return:        0  have inter zone
                  -1 no inter zone
 */
static td_s32 tde_osi_get_inter_rect(const drv_tde_rect *dst_rect, const drv_tde_rect *clip_rect,
                                     drv_tde_rect *inter_rect)
{
    td_s32 left;
    td_s32 top;
    td_s32 right;
    td_s32 bottom;
    td_s32 right1;
    td_s32 bottom1;
    td_s32 right2;
    td_s32 bottom2;
    td_s32 pos_x;
    td_s32 pos_y;

    pos_x = dst_rect->pos_x;
    pos_y = dst_rect->pos_y;
    left = (pos_x > clip_rect->pos_x) ? pos_x : clip_rect->pos_x;
    top = (pos_y > clip_rect->pos_y) ? pos_y : clip_rect->pos_y;

    right1 = pos_x + dst_rect->width - 1;
    right2 = clip_rect->pos_x + clip_rect->width - 1;
    right = (right1 > right2) ? right2 : right1;

    bottom1 = pos_y + dst_rect->height - 1;
    bottom2 = clip_rect->pos_y + clip_rect->height - 1;
    bottom = (bottom1 > bottom2) ? bottom2 : bottom1;

    if ((left > right) || (top > bottom)) {
        return DRV_ERR_TDE_INVALID_PARA;
    }

    inter_rect->pos_x = left;
    inter_rect->pos_y = top;
    inter_rect->width = right - left + 1;
    inter_rect->height = bottom - top + 1;
    return TD_SUCCESS;
}

/*
 * Function:      tde_osi_is_rect1_in_rect2
 * Description:   Rect1 is if inside Rect2
 * Input:         rect1  input rectangle1
                  rect2  input rectangle2
 * Return:        1  have inter zone
                  0  no inter zone
 */
static td_bool tde_osi_is_rect1_in_rect2(const drv_tde_rect *rect1, const drv_tde_rect *rect2)
{
    td_s32 right1 = rect1->pos_x + rect1->width - 1;
    td_s32 right2 = rect2->pos_x + rect2->width - 1;

    td_s32 bottom1 = rect1->pos_y + rect1->height - 1;
    td_s32 bottom2 = rect2->pos_y + rect2->height - 1;

    if ((rect1->pos_x >= rect2->pos_x) &&
        (rect1->pos_y >= rect2->pos_y) &&
        (right1 <= right2) &&
        (bottom1 <= bottom2)) {
        return TD_TRUE;
    }

    return TD_FALSE;
}

/*
 * Function:      tde_osi_set_mb_para
 * Description:   MB operate setting parameter interface
 * Input:         handle: task handle
 *                dst_surface:  target bitmap information struct
 *                dst_rect: target bitmap operate zone
 *                mb_opt:  operate parameter setting struct
 */
static td_s32 tde_osi_set_mb_para(td_s32 handle, const drv_tde_mb_src *mb_src, const drv_tde_mb_opt *mb_opt)
{
    drv_tde_surface *fg_surface = TD_NULL;
    drv_tde_opt *opt = TD_NULL;
    td_s32 ret;
    drv_tde_single_src single_src;

    fg_surface = (drv_tde_surface *)tde_malloc(sizeof(drv_tde_surface));
    if (fg_surface == TD_NULL) {
        tde_error ("malloc fg_surface failed, size=%ld!\n", (unsigned long)(sizeof(drv_tde_surface)));
        return DRV_ERR_TDE_NO_MEM;
    }
    opt = (drv_tde_opt *)tde_malloc(sizeof(drv_tde_opt));
    if (opt == TD_NULL) {
        tde_error ("malloc pstOpt failed, size=%ld!\n", (unsigned long)(sizeof(drv_tde_opt)));
        tde_free(fg_surface);
        return DRV_ERR_TDE_NO_MEM;
    }
    fg_surface->phys_addr = mb_src->mb_surface->y_addr;
    fg_surface->stride = mb_src->mb_surface->y_stride;
    fg_surface->width = mb_src->mb_surface->y_width;
    fg_surface->color_format = tde_osi_covert_mb_fmt(mb_src->mb_surface->mb_color_format);
    fg_surface->height = mb_src->mb_surface->y_height;
    fg_surface->cbcr_phys_addr = mb_src->mb_surface->cbcr_phys_addr;
    fg_surface->cbcr_stride = mb_src->mb_surface->cbcr_stride;

#if (TDE_CAPABILITY & DEFLICKER)
    opt->deflicker_mode = (mb_opt->is_deflicker) ? DRV_TDE_DEFLICKER_LEVEL_MODE_BOTH :
	    DRV_TDE_DEFLICKER_LEVEL_MODE_NONE;
#endif

    opt->out_alpha_from = (mb_opt->is_set_out_alpha) ? DRV_TDE_OUT_ALPHA_FROM_GLOBALALPHA :
	    DRV_TDE_OUT_ALPHA_FROM_NORM;
    opt->resize = (mb_opt->resize_en == DRV_TDE_MB_RESIZE_NONE) ? TD_FALSE : TD_TRUE;
    opt->clip_rect = mb_opt->clip_rect;
    opt->global_alpha = mb_opt->out_alpha;
    opt->clip_mode = mb_opt->clip_mode;

    single_src.src_surface      = fg_surface;
    single_src.src_rect         = mb_src->src_rect;
    single_src.dst_surface      = mb_src->dst_surface;
    single_src.dst_rect         = mb_src->dst_rect;
    ret = tde_osi_single_src_2_blit(handle, &single_src, opt, TD_FALSE, TD_FALSE);
    tde_free((td_void *)fg_surface);
    tde_free((td_void *)opt);
    return ret;
}
#if (TDE_CAPABILITY & SLICE)
/* data_in :positive or negetive  ;  bit:complement width */
static td_u32 tde_true_value_to_complement(td_s32 data_in, td_u32 bit)
{
    td_u32 data_out;
    td_u32 data_in_tmp;

    if (data_in >> (bit - 1)) {
        data_in_tmp = 0 - data_in;
        data_out = ((1 << (bit - 1)) | (((~data_in_tmp) & ((1 << (bit - 1)) - 1)) + 1)) & ((1 << bit) - 1);
    } else {
        data_out = data_in;
    }

    return data_out;
}

/* data_in :positive or negetive  ;  bit:complement width */
static td_s32 tde_complement_to_true_value(td_u32 data_in, td_u32 bit)
{
    td_s32 data_out;
    td_u32 data_in_tmp;

    if (data_in >> (bit - 1)) {
        data_in_tmp = data_in & ((1 << (bit - 1)) - 1);
        data_out = (((~data_in_tmp) & ((1 << (bit - 1)) - 1)) + 1) * (-1);
    } else {
        data_out = data_in;
    }

    return data_out;
}

static td_void tde_osi_init_slice_data(tde_slice_data *slice_data, const tde_hw_node *child_node, td_u32 i)
{
    if (i == 0) {
        slice_data->fmt = child_node->src1_ctrl.bits.src1_fmt;
        slice_data->hor_scan_ord = child_node->src1_ctrl.bits.src1_h_scan_ord;
        slice_data->width = child_node->src1_imgsize.bits.src1_width + 1;

        slice_data->u32422v_pro = child_node->src1_ctrl.bits.src1_422v_pro;

        slice_data->hlmsc_en = child_node->src1_hsp.bits.hlmsc_en;
        slice_data->hchmsc_en = child_node->src1_hsp.bits.hchmsc_en;
        slice_data->hratio = child_node->src1_hsp.bits.hratio;
        slice_data->hor_loffset = child_node->src1_hloffset;
        slice_data->hor_coffset = child_node->src1_hcoffset;
        slice_data->zme_ow = child_node->src1_zmeoreso.bits.ow + 1;
        slice_data->zme_iw = child_node->src1_zmeireso.bits.iw + 1;
        slice_data->hpzme_en = child_node->src1_hpzme.bits.src1_hpzme_en;
        slice_data->hpzme_mode = child_node->src1_hpzme.bits.src1_hpzme_mode;
        slice_data->hpzme_width = child_node->src1_hpzme_size.bits.src1_hpzme_width + 1;
    } else {
        slice_data->fmt = child_node->src2_ctrl.bits.src2_fmt;
        slice_data->hor_scan_ord = child_node->src2_ctrl.bits.src2_h_scan_ord;
        slice_data->width = child_node->src2_imgsize.bits.src2_width + 1;
        slice_data->u32422v_pro = child_node->src2_ctrl.bits.src2_422v_pro;
        slice_data->hlmsc_en =  child_node->src2_hsp.bits.hlmsc_en;
        slice_data->hchmsc_en = child_node->src2_hsp.bits.hchmsc_en;
        slice_data->hratio = child_node->src2_hsp.bits.hratio;
        slice_data->hor_loffset =  child_node->src2_hloffset;
        slice_data->hor_coffset =  child_node->src2_hcoffset;
        slice_data->zme_ow = child_node->src2_zmeoreso.bits.ow + 1;
        slice_data->zme_iw = child_node->src2_zmeireso.bits.iw + 1;
        slice_data->hpzme_en =  child_node->src2_hpzme.bits.src2_hpzme_en;
        slice_data->hpzme_mode = child_node->src2_hpzme.bits.src2_hpzme_mode;
        slice_data->hpzme_width = child_node->src2_hpzme_size.bits.src2_hpzme_width + 1;
    }

    return;
}

static td_void tde_osi_calc_slice_s1(tde_slice_data *slice_data)
{
    slice_data->xdpos = (slice_data->hlmsc_en | slice_data->hchmsc_en) ? (slice_data->zme_ow - 1) :
        (slice_data->hpzme_en ? (slice_data->hpzme_width - 1) : (slice_data->width - 1));
    slice_data->xed_pos_blk = MIN2(slice_data->des_xed_pos_blk, slice_data->xdpos);
    /* step2 : out pos relative to s1 disp start */
    slice_data->xst_pos_cord = slice_data->des_xst_pos_blk;
    slice_data->xed_pos_cord = slice_data->xed_pos_blk;

    /* setp2 : calc s1 zme parameter */
    slice_data->xst_pos_cord_in = (slice_data->xst_pos_cord * slice_data->hratio) >> 20; /* 20 alg data */
    slice_data->xed_pos_cord_in = (slice_data->xed_pos_cord * slice_data->hratio) >> 20; /* 20 alg data */
    slice_data->hor_loffset_cfg_int_comp = slice_data->hor_loffset >> 20; /* 20 alg data */
    /* 8 alg data */
    slice_data->hor_loffset_cfg_int = tde_complement_to_true_value(slice_data->hor_loffset_cfg_int_comp, 8);
    slice_data->hor_loffset_cfg_fraction = slice_data->hor_loffset & 0xfffff;
    slice_data->hor_loffset_pix_fraction = (slice_data->xst_pos_cord * slice_data->hratio) & 0xfffff;
    slice_data->hor_loffset_fraction = (slice_data->hor_loffset_cfg_fraction +
                                        slice_data->hor_loffset_pix_fraction) & 0xfffff;

    slice_data->xst_pos_cord_in_offset = slice_data->xst_pos_cord_in + slice_data->hor_loffset_cfg_int + (
        ((slice_data->hor_loffset_cfg_fraction + slice_data->hor_loffset_pix_fraction) & 0xfff00000) != 0);
    slice_data->xed_pos_cord_in_offset = slice_data->xed_pos_cord_in + slice_data->hor_loffset_cfg_int + (
        ((slice_data->hor_loffset_cfg_fraction + slice_data->hor_loffset_pix_fraction) & 0xfff00000) != 0);
    slice_data->xst_pos_cord_in_tap_rgb = (slice_data->xst_pos_cord_in_offset < 0) ? 0 : (
        (slice_data->xst_pos_cord_in_offset >= (TDE_MAX_ZOOM_OUT_STEP / 2 - 1)) ?  /* 2 alg data */
        (slice_data->xst_pos_cord_in_offset - (TDE_MAX_ZOOM_OUT_STEP / 2 - 1)) : 0); /* 2 alg data */
    slice_data->xed_pos_cord_in_tap_rgb = (slice_data->xed_pos_cord_in_offset + TDE_MAX_ZOOM_OUT_STEP / 2) < 0 ?
        0 : (((slice_data->xed_pos_cord_in_offset + TDE_MAX_ZOOM_OUT_STEP / 2) >= /* 2 alg data */
        (slice_data->zme_iw - 1)) ? (slice_data->zme_iw - 1) : (slice_data->xed_pos_cord_in_offset +
        TDE_MAX_ZOOM_OUT_STEP / 2)); /* 2 alg data */
    slice_data->node_cfg_zme_iw_rgb = slice_data->xed_pos_cord_in_tap_rgb - slice_data->xst_pos_cord_in_tap_rgb + 1;
    slice_data->hor_loffset_int = (slice_data->xst_pos_cord_in_offset - slice_data->xst_pos_cord_in_tap_rgb);
    /* 8 alg data */
    slice_data->hor_loffset_int_complement = tde_true_value_to_complement(slice_data->hor_loffset_int, 8);
    slice_data->node_cfg_hor_loffset_rgb = (slice_data->hor_loffset_int_complement << 20) + /* 20 alg data */
                                            slice_data->hor_loffset_fraction;
    slice_data->xst_pos_cord_in_tap_luma = ((slice_data->xst_pos_cord_in_tap_rgb % 2) == 1) ? /* 2 alg data */
        (slice_data->xst_pos_cord_in_tap_rgb - 1) : slice_data->xst_pos_cord_in_tap_rgb;
    slice_data->xed_pos_cord_in_tap_luma = (((slice_data->xed_pos_cord_in_tap_rgb % 2) == 0) ? /* 2 alg data */
        (slice_data->xed_pos_cord_in_tap_rgb + 1) : slice_data->xed_pos_cord_in_tap_rgb) > (
        /* 2 alg data */
        slice_data->zme_iw - 1) ? (slice_data->zme_iw - 1) : (((slice_data->xed_pos_cord_in_tap_rgb % 2) == 0) ?
        (slice_data->xed_pos_cord_in_tap_rgb + 1) : slice_data->xed_pos_cord_in_tap_rgb);
    slice_data->xst_pos_cord_in_chroma = (slice_data->fmt == 50) ? (slice_data->xst_pos_cord * /* 50 alg data */
        (slice_data->hratio)) >> 20 : (slice_data->xst_pos_cord * (slice_data->hratio / 2)) >> 20; /* 20 alg data */
    slice_data->xed_pos_cord_in_chroma = (slice_data->fmt == 50) ? (slice_data->xed_pos_cord * /* 50 alg data */
        (slice_data->hratio)) >> 20 : (slice_data->xed_pos_cord * (slice_data->hratio / 2)) >> 20; /* 20 alg data */
    slice_data->hor_coffset_cfg_int_comp = slice_data->hor_coffset >> 20; /* 20 alg data */
    /* 8 alg data */
    slice_data->hor_coffset_cfg_int = tde_complement_to_true_value(slice_data->hor_coffset_cfg_int_comp, 8);
    slice_data->hor_coffset_cfg_fraction = slice_data->hor_coffset & 0xfffff;
    return;
}

static td_void tde_osi_calc_slice_sp(tde_slice_data *slice_data)
{
    /* 50 alg data */
    slice_data->hor_coffset_pix_fraction = ((slice_data->fmt == 50) ? (slice_data->xst_pos_cord *
        (slice_data->hratio)) : (slice_data->xst_pos_cord * (slice_data->hratio / 2))) & 0xfffff; /* 2 alg data */
    slice_data->hor_coffset_fraction = (slice_data->hor_coffset_cfg_fraction +
        slice_data->hor_coffset_pix_fraction) & 0xfffff;
    slice_data->xst_pos_cord_in_offset_chroma = slice_data->xst_pos_cord_in_chroma +
        slice_data->hor_coffset_cfg_int + (((slice_data->hor_coffset_cfg_fraction +
        slice_data->hor_coffset_pix_fraction) & 0xfff00000) != 0);
    slice_data->xed_pos_cord_in_offset_chroma = slice_data->xed_pos_cord_in_chroma +
        slice_data->hor_coffset_cfg_int + (((slice_data->hor_coffset_cfg_fraction +
        slice_data->hor_coffset_pix_fraction) & 0xfff00000) != 0);
    slice_data->xst_pos_cord_in_tap_chroma = (slice_data->xst_pos_cord_in_offset_chroma < 0) ? 0 :
        ((slice_data->xst_pos_cord_in_offset_chroma >= (TDE_MAX_ZOOM_OUT_STEP / 2 - 1)) ? /* 2 alg data */
        (slice_data->xst_pos_cord_in_offset_chroma - (TDE_MAX_ZOOM_OUT_STEP / 2 - 1)) : 0); /* 2 alg data */
    /* 2 alg data */
    slice_data->xed_pos_cord_in_tap_chroma = (slice_data->xed_pos_cord_in_offset_chroma + TDE_MAX_ZOOM_OUT_STEP / 2)
        < 0 ? 0 : (((slice_data->xed_pos_cord_in_offset_chroma + TDE_MAX_ZOOM_OUT_STEP / 2) >= /* 2 alg data */
        /* 2 50 alg data */
        ((slice_data->fmt == 50) ? ((slice_data->zme_iw - 1)) : ((slice_data->zme_iw - 1) / 2))) ? ((
        /* 2 50 alg data */
        slice_data->fmt == 50) ? ((slice_data->zme_iw - 1)) : ((slice_data->zme_iw - 1) / 2)) :
        (slice_data->xed_pos_cord_in_offset_chroma + TDE_MAX_ZOOM_OUT_STEP / 2)); /* 2 alg data */
    /* 50 alg data */
    slice_data->xst_pos_cord_in_tap_chroma_x2 = (slice_data->fmt == 50) ? slice_data->xst_pos_cord_in_tap_chroma :
                                                 (slice_data->xst_pos_cord_in_tap_chroma * 2); /* 2 alg data */
    /* 50 alg data */
    slice_data->xed_pos_cord_in_tap_chroma_x2 = (slice_data->fmt == 50) ? slice_data->xed_pos_cord_in_tap_chroma :
                                                 (slice_data->xed_pos_cord_in_tap_chroma * 2 + 1); /* 2 alg data */

    slice_data->xst_pos_cord_in_tap_sp = MIN2(slice_data->xst_pos_cord_in_tap_luma,
                                              slice_data->xst_pos_cord_in_tap_chroma_x2);
    slice_data->xed_pos_cord_in_tap_sp = MAX2(slice_data->xed_pos_cord_in_tap_luma,
                                              slice_data->xed_pos_cord_in_tap_chroma_x2);

    slice_data->node_cfg_zme_iw_sp = slice_data->xed_pos_cord_in_tap_sp - slice_data->xst_pos_cord_in_tap_sp + 1;
    slice_data->hor_loffset_int_sp = (slice_data->xst_pos_cord_in_offset - slice_data->xst_pos_cord_in_tap_sp);
    slice_data->hor_coffset_int_sp = (slice_data->xst_pos_cord_in_offset_chroma - (
        /* 50 2 alg data */
        (slice_data->fmt == 50) ? slice_data->xst_pos_cord_in_tap_sp : (slice_data->xst_pos_cord_in_tap_sp / 2)));
    /* 8 alg data */
    slice_data->hor_loffset_int_sp_complent = tde_true_value_to_complement(slice_data->hor_loffset_int_sp, 8);
    /* 8 alg data */
    slice_data->hor_coffset_int_sp_complent = tde_true_value_to_complement(slice_data->hor_coffset_int_sp, 8);
    slice_data->node_cfg_hor_loffset_sp = (slice_data->hor_loffset_int_sp_complent << 20) + /* 20 alg data */
                                           slice_data->hor_loffset_fraction;
    slice_data->node_cfg_hor_coffset_sp = (slice_data->hor_coffset_int_sp_complent << 20) + /* 20 alg data */
                                           slice_data->hor_coffset_fraction;
    return;
}

static td_void tde_osi_calc_slice_node(tde_slice_data *slice_data, tde_hw_node *child_node, td_u32 i)
{
    td_bool set_slice_size;
    /* 0 2 1 alg data */
    set_slice_size = ((slice_data->hpzme_en == 0) && (slice_data->slice_width % 2 == 1) &&
        ((slice_data->fmt == 48) || ((slice_data->fmt == 50) && !slice_data->u32422v_pro) || /* 48 50 alg data */
        (slice_data->fmt == 52)) && (slice_data->xed_pos_cord_in_tap_hpzme_hso == slice_data->width - 1) && /* 52 alg */
        (slice_data->width % 2 == 0)); /* 2 alg data */
    if (set_slice_size) {
        if (slice_data->hor_scan_ord) {
            slice_data->slice_wi = slice_data->slice_wi + 1;
            slice_data->slice_width = slice_data->slice_width + 1;
            slice_data->slice_hoffset = slice_data->slice_hoffset - 1;
        } else {
            slice_data->slice_wi = slice_data->slice_wi + 1;
            slice_data->slice_width = slice_data->slice_width + 1;
            slice_data->slice_hoffset = slice_data->slice_hoffset - 1;
            slice_data->slice_l_ofst = slice_data->slice_l_ofst + (
            slice_data->hor_loffset_int_beyond_complent << 20); /* 20 alg data */
            slice_data->slice_c_ofst = slice_data->slice_c_ofst + (
            slice_data->hor_loffset_int_beyond_complent << 20); /* 20 alg data */
        }
    }

    if (i == 0) {
        child_node->src1_zmeoreso.bits.ow = slice_data->slice_wo;
        child_node->src1_hloffset = slice_data->slice_l_ofst;
        child_node->src1_hcoffset = slice_data->slice_c_ofst;
        child_node->src1_pix_offset.bits.src1_hoffset_pix = slice_data->slice_hoffset;
        child_node->src1_imgsize.bits.src1_width = slice_data->slice_width - 1;
        child_node->src1_zmeireso.bits.iw = slice_data->slice_wi;
        child_node->src1_hpzme_size.bits.src1_hpzme_width = slice_data->slice_w_hpzme - 1;

        return;
    }

    child_node->src2_zmeoreso.bits.ow = slice_data->slice_wo;
    child_node->src2_hloffset = slice_data->slice_l_ofst;
    child_node->src2_hcoffset = slice_data->slice_c_ofst;
    child_node->src2_pix_offset.bits.src2_hoffset_pix = slice_data->slice_hoffset;
    child_node->src2_imgsize.bits.src2_width = slice_data->slice_width - 1;
    child_node->src2_zmeireso.bits.iw = slice_data->slice_wi;
    child_node->src2_hpzme_size.bits.src2_hpzme_width = slice_data->slice_w_hpzme - 1;
}

static td_void tde_osi_calc_slice_mux(tde_slice_data *slice_data, tde_hw_node *child_node, td_u32 i)
{
    td_bool cfg_zme_iw_sp;
    td_bool cfg_hor_loffset_sp;
    td_bool cfg_hor_coffset_sp;

    slice_data->slice_wo = slice_data->xed_pos_cord - slice_data->xst_pos_cord + 1;
    /* 32 48 50 alg data */
    cfg_zme_iw_sp = ((slice_data->fmt >= 32) && (slice_data->fmt != 48) && (!((slice_data->fmt == 50) &&
                     !slice_data->u32422v_pro)) && (slice_data->fmt != 52)); /* 52 alg data */
    slice_data->slice_wi = (cfg_zme_iw_sp) ? slice_data->node_cfg_zme_iw_sp : ((slice_data->hlmsc_en ||
                            slice_data->hchmsc_en) ? slice_data->node_cfg_zme_iw_rgb : slice_data->slice_wo);
    /* 32 48 50 alg data */
    cfg_hor_loffset_sp = ((slice_data->fmt >= 32) && (slice_data->fmt != 48) && (!((slice_data->fmt == 50) &&
                          !slice_data->u32422v_pro)) && (slice_data->fmt != 52)); /* 52 alg data */
    slice_data->slice_l_ofst = (cfg_hor_loffset_sp) ? slice_data->node_cfg_hor_loffset_sp :
                                slice_data->node_cfg_hor_loffset_rgb;

    cfg_hor_coffset_sp = cfg_hor_loffset_sp;
    slice_data->slice_c_ofst = (cfg_hor_coffset_sp) ? slice_data->node_cfg_hor_coffset_sp :
                                slice_data->node_cfg_hor_loffset_rgb;

    slice_data->slice_w_hpzme = slice_data->slice_wi;
    slice_data->slice_wo = slice_data->slice_wo - 1;
    slice_data->slice_wi = slice_data->slice_wi - 1;

    slice_data->xst_pos_cord_in_tap = (slice_data->hlmsc_en || slice_data->hchmsc_en) ?
        /* 32 48 50 alg data */
        (((slice_data->fmt >= 32) && (slice_data->fmt != 48) && (!((slice_data->fmt == 50) &&
        /* 52 alg data */
        !slice_data->u32422v_pro)) && (slice_data->fmt != 52)) ? slice_data->xst_pos_cord_in_tap_sp :
        slice_data->xst_pos_cord_in_tap_rgb) : slice_data->xst_pos_cord;

    slice_data->xed_pos_cord_in_tap = (slice_data->hlmsc_en || slice_data->hchmsc_en) ?
        /* 32 48 50 alg data */
        (((slice_data->fmt >= 32) && (slice_data->fmt != 48) && (!((slice_data->fmt == 50) &&
        /* 52 alg data */
        !slice_data->u32422v_pro)) && (slice_data->fmt != 52)) ? slice_data->xed_pos_cord_in_tap_sp :
        slice_data->xed_pos_cord_in_tap_rgb) : slice_data->xed_pos_cord;

    slice_data->xst_pos_cord_in_tap_hpzme = slice_data->hpzme_en ? (slice_data->xst_pos_cord_in_tap *
       (slice_data->hpzme_mode + 1)) : slice_data->xst_pos_cord_in_tap;
    slice_data->xed_pos_cord_in_tap_hpzme = slice_data->hpzme_en ? ((slice_data->xed_pos_cord_in_tap + 1) *
       (slice_data->hpzme_mode + 1) - 1) : slice_data->xed_pos_cord_in_tap;

    if (slice_data->hor_scan_ord) {
        slice_data->xst_pos_cord_in_tap_hpzme_hso = slice_data->width - 1 - slice_data->xed_pos_cord_in_tap_hpzme;
        slice_data->xed_pos_cord_in_tap_hpzme_hso = slice_data->width - 1 - slice_data->xst_pos_cord_in_tap_hpzme;
    } else {
        slice_data->xst_pos_cord_in_tap_hpzme_hso = slice_data->xst_pos_cord_in_tap_hpzme;
        slice_data->xed_pos_cord_in_tap_hpzme_hso = slice_data->xed_pos_cord_in_tap_hpzme;
    }

    slice_data->slice_width = (slice_data->xed_pos_cord_in_tap_hpzme_hso -
                               slice_data->xst_pos_cord_in_tap_hpzme_hso + 1);
    slice_data->slice_hoffset = slice_data->xst_pos_cord_in_tap_hpzme_hso;

    slice_data->hor_loffset_int_beyond = 1;
    /* 8 alg data */
    slice_data->hor_loffset_int_beyond_complent = tde_true_value_to_complement(slice_data->hor_loffset_int_beyond, 8);
}

static td_void tde_osi_finalize_slice_calculation(tde_slice_data *slice_data, tde_hw_node *child_node)
{
    slice_data->slice_dst_width = slice_data->des_xed_pos_blk - slice_data->des_xst_pos_blk + 1;
    slice_data->slice_dst_hoffset = (slice_data->des_h_scan_ord ? (slice_data->des_width - 1 -
        slice_data->des_xed_pos_blk) : slice_data->des_xst_pos_blk) + slice_data->des_hoffset_pix;
    slice_data->des_crop_en = (!((slice_data->des_crop_start_x > slice_data->des_xed_pos_blk) ||
        (slice_data->des_crop_end_x < slice_data->des_xst_pos_blk))) && slice_data->des_crop_en;
    slice_data->des_crop_start_x = slice_data->des_crop_en ? (MAX2(slice_data->des_xst_pos_blk,
        slice_data->des_crop_start_x) - slice_data->des_xst_pos_blk) : 0;
    slice_data->des_crop_end_x = slice_data->des_crop_en ? (MIN2(slice_data->des_xed_pos_blk,
        slice_data->des_crop_end_x) - slice_data->des_xst_pos_blk) : 0;

    child_node->des_imgsize.bits.des_width = slice_data->slice_dst_width - 1;
    child_node->des_pix_offset.bits.des_hoffset_pix = slice_data->slice_dst_hoffset;
    child_node->des_alpha.bits.des_crop_en = slice_data->des_crop_en;
    child_node->des_crop_pos_st.bits.des_crop_start_x = slice_data->des_crop_start_x;
    child_node->des_crop_pos_ed.bits.des_crop_end_x = slice_data->des_crop_end_x;

    return;
}

static td_s32 tde_osi_calc_slice(td_s32 handle, const tde_hw_node *node)
{
    tde_slice_data slice_data = { 0 };
    td_u32 n;
    td_u32 i;
    td_s32 ret;

    slice_data.des_width = node->des_imgsize.bits.des_width + 1;
    slice_data.des_hoffset_pix = 0;
    slice_data.des_h_scan_ord = node->des_ctrl.bits.des_h_scan_ord;

    slice_data.des_crop_en = node->des_alpha.bits.des_crop_en;
    slice_data.des_crop_start_x = node->des_crop_pos_st.bits.des_crop_start_x;
    slice_data.des_crop_end_x = node->des_crop_pos_ed.bits.des_crop_end_x;
    /* 256 alg data */
    slice_data.node_num = slice_data.des_width / 256 + ((slice_data.des_width % 256) != 0);

    for (n = 0; n < slice_data.node_num; n++) {
        tde_hw_node *child_node;
        td_void *buf = TD_NULL;
        buf = (td_void *)tde_malloc(sizeof(tde_hw_node) + TDE_NODE_HEAD_BYTE + TDE_NODE_TAIL_BYTE);
        if (buf == TD_NULL) {
            tde_error("malloc (%d) failed, wgetfreenum(%d)!\n", (sizeof(tde_hw_node)), wgetfreenum());
            return DRV_ERR_TDE_NO_MEM;
        }

        child_node = (tde_hw_node *)(buf + TDE_NODE_HEAD_BYTE);

        if (memcpy_s(child_node, sizeof(tde_hw_node), node, sizeof(tde_hw_node)) != EOK) {
            tde_hal_free_node_buf(child_node);
            return DRV_ERR_TDE_INVALID_PARA;
        }

        slice_data.des_xst_pos_blk = n * TDE_MAX_SLICE_WIDTH;
        slice_data.des_xed_pos_blk = (((n + 1) * TDE_MAX_SLICE_WIDTH) <= slice_data.des_width) ? (
            ((n + 1) * TDE_MAX_SLICE_WIDTH) - 1) : (slice_data.des_width - 1);

        for (i = 0; i < 2; i++) { /* 2 alg data */
            tde_osi_init_slice_data(&slice_data, child_node, i);

            tde_osi_calc_slice_s1(&slice_data);

            tde_osi_calc_slice_sp(&slice_data);

            tde_osi_calc_slice_mux(&slice_data, child_node, i);
        }

        tde_osi_finalize_slice_calculation(&slice_data, child_node);
        ret = tde_osi_set_node_finish(handle, child_node, 0, TDE_NODE_SUBM_ALONE);
        if (ret != TD_SUCCESS) {
            tde_hal_free_node_buf(child_node);
            return ret;
        }
    }
    return TD_SUCCESS;
}

#endif
#if (TDE_CAPABILITY & RESIZE)
#define TDE_OSI_ZME_SHIFT TDE_FLOAT_BITLEN

typedef struct {
    td_s32 in_start;
    td_s32 in_stop;
    td_s32 out_start;
    td_s32 out_stop;
    td_s32 phase;
} tde_osi_zme_range;

static td_void tde_osi_set_value_to_clear(td_s32 *zme_vin, td_s32 *zme_vout, td_s32 *zme_vphase)
{
    *zme_vin = 0;
    *zme_vout = 0;
    *zme_vphase = 0;
}
static td_void tde_osi_get_info_zme(const tde_update_config *reg, tde_update_info *info)
{
    info->zme_instart_w = reg->update_instart_w;
    info->zme_outstart_w = reg->update_instart_w;
    info->zme_in_width = reg->update_in_width;
    info->zme_out_width = reg->update_in_width;
    info->zme_hphase = 0;
}
static td_void tde_osi_set_hupdate_info(tde_update_info *info, const tde_osi_zme_range *zme)
{
    info->zme_instart_w = zme->in_start;
    info->zme_outstart_w = zme->out_start;
    info->zme_in_width = zme->in_stop - zme->in_start + 1;
    info->zme_out_width = zme->out_stop - zme->out_start + 1;
    info->zme_hphase = zme->phase;
}
static td_void tde_osi_get_hupdate_info(const tde_update_config *reg, tde_update_info *info, td_bool scaler)
{
    tde_osi_zme_range zme;
    td_s32 ratio;
    td_s32 dratio;
    td_s32 update_hstart = reg->update_instart_w;
    td_s32 update_hstop = update_hstart + reg->update_in_width - 1;

    if (((reg->zme_out_width - 1) == 0) || ((reg->ori_in_width - 1) == 0)) {
        return;
    }
    /* 4096 2 alg data */
    ratio = (td_s32)(4096 * (reg->ori_in_width - 1) / (reg->zme_out_width - 1) + 1 / 2);
    /* 4096 alg data */
    dratio = 4096 * (reg->zme_out_width - 1) / (reg->ori_in_width - 1);
    /* hor_scaler not enable */
    if (scaler != TD_TRUE) {
        tde_osi_get_info_zme(reg, info);
        return;
    }
    if ((update_hstart >= 0) && (update_hstart < 3)) { /* 3 alg data */
        /* update outstretched area exceed left limit bordline */
        tde_osi_set_value_to_clear(&zme.in_start, &zme.out_start, &zme.phase);
    } else {
        /* update outstretched area didn't exceed the left limit bordline */
        zme.in_start = (update_hstart - 3) * dratio; /* 3 alg data */
        /* 12 4096 alg data */
        zme.out_start = (zme.in_start % 4096) == 0 ? ((td_u32)zme.in_start >> TDE_OSI_ZME_SHIFT) :
            (((td_u32)zme.in_start >> TDE_OSI_ZME_SHIFT) + 1);
        zme.in_start = (td_u32)(zme.out_start * ratio) >> TDE_OSI_ZME_SHIFT; /* 12 alg data */
        if (zme.in_start - 2 < 0) { /* 2 alg data */
            /* 4096 the left few point need mirror pixels when scale */
            zme.phase = (zme.out_start * ratio) % 4096 + zme.in_start * 4096;
            zme.in_start = 0;
        } else {
            /* 4096 3 the left few point not need mirror pixels when scale */
            zme.phase = (zme.out_start * ratio) % 4096 + 3 * 4096;
            zme.in_start = zme.in_start - 2; /* 2 alg data */
        }
    }
    if ((update_hstop > (reg->ori_in_width - 3)) && (update_hstop < reg->ori_in_width)) { /* 3 alg data */
        /* update outstretched area exceed the right limit bordline */
        zme.in_stop = reg->ori_in_width - 1;
        zme.out_stop = reg->zme_out_width - 1;
    } else {
        /* 2 1 alg data, update outstretched area didn't exceed the right limit bordline */
        zme.in_stop = (update_hstop + 2 + 1) * dratio;
        /* 12 4096 alg data */
        zme.out_stop = (zme.in_stop % 4096) == 0 ? (((td_u32)zme.in_stop >> TDE_OSI_ZME_SHIFT) - 1) :
            ((td_u32)zme.in_stop >> TDE_OSI_ZME_SHIFT);
        zme.in_stop = (td_u32)(zme.out_stop * ratio) >> TDE_OSI_ZME_SHIFT;  /* 12 alg data */
        if (zme.in_stop + 3 > (reg->ori_in_width - 1)) { /* 3 alg data */
            /* the right few point need mirror pixels when scale */
            zme.in_stop = reg->ori_in_width - 1;
        } else {
            /* the right few point need mirror pixels when scale */
            zme.in_stop = zme.in_stop + 3; /* 3 alg data */
        }
    }
    tde_osi_set_hupdate_info(info, &zme);
}
static td_void tde_osi_get_info_to_reg(const tde_update_config *reg, tde_update_info *info)
{
    info->zme_instart_h = reg->update_instart_h;
    info->zme_in_height = reg->update_in_height;
    info->zme_outstart_h = reg->update_instart_h;
    info->zme_out_height = reg->update_in_height;
    info->zme_vphase = 0;
    info->def_offsetup = 0;
    info->def_offsetdown = 0;
}
static td_void tde_osi_set_vupdate_info(tde_update_info *info, const tde_osi_zme_range *zme)
{
    info->zme_in_height = zme->in_stop - zme->in_start + 1;
    info->zme_instart_h = zme->in_start;
    info->zme_outstart_h = zme->out_start;
    info->zme_out_height = zme->out_stop - zme->out_start + 1;
    info->zme_vphase = zme->phase;
    info->def_offsetup = 0;
    info->def_offsetdown = 0;
}
static td_void tde_osi_get_vupdate_info(const tde_update_config *reg, tde_update_info *info, int scaler)
{
    tde_osi_zme_range zme;
    td_s32 ratio;
    td_s32 dratio;
    td_s32 update_vstart = reg->update_instart_h;
    td_s32 update_vstop = update_vstart + reg->update_in_height - 1;

    if ((reg->zme_out_height == 0) || (reg->ori_in_height == 0)) {
        return;
    }
    ratio = (td_s32)(4096 * (reg->ori_in_height) / (reg->zme_out_height)); /* 4096 alg data */
    dratio = 4096 * (reg->zme_out_height) / (reg->ori_in_height); /* 4096 alg data */

    if (scaler != TD_TRUE) {
        tde_osi_get_info_to_reg(reg, info);
        return;
    }
    /* ver_scale enable & deflicker not enable */
    if ((update_vstart >= 0) && (update_vstart < 2)) { /* 2 Vstart */
        /* the update outstreatched area exceed the up limit bordline */
        tde_osi_set_value_to_clear(&zme.in_start, &zme.out_start, &zme.phase);
    } else {
        /* the update outstreatched area didn't exceed the up limit bordline */
        zme.in_start = (update_vstart - 2) * dratio; /* 2 alg data */
        /* 4096 12 alg data */
        zme.out_start = (zme.in_start % 4096) == 0 ? ((td_u32)zme.in_start >> TDE_OSI_ZME_SHIFT) :
            (((td_u32)zme.in_start >> TDE_OSI_ZME_SHIFT) + 1);
        zme.in_start = (td_u32)(zme.out_start * ratio) >> TDE_OSI_ZME_SHIFT; /* 12 alg data */
        if (zme.in_start - 1 < 0) {
            /* the up few point need mirror pixels when scale */
            zme.phase = (zme.out_start * ratio) % 4096 + zme.in_start * 4096; /* 4096 alg data */
            zme.in_start = 0;
        } else {
            /* the up few point not need mirror pixels when scale */
            zme.phase = (zme.out_start * ratio) % 4096 + 1 * 4096; /* 4096 alg data */
            zme.in_start = zme.in_start - 1;
        }
    }
    /* 2 alg data */
    if ((update_vstop > (reg->ori_in_height - 2)) && (update_vstop < reg->ori_in_height)) {
        /* the update outstreatched area exceed the down limit bordline */
        zme.in_stop = reg->ori_in_height - 1;
        zme.out_stop = reg->zme_out_height - 1;
    } else {
        /* the update outstreatched area didn't exceed the down limit bordline */
        zme.in_stop = (update_vstop + 1 + 1) * dratio;
        /* 4096 12 alg data */
        zme.out_stop = (zme.in_stop % 4096) == 0 ? (((td_u32)zme.in_stop >> TDE_OSI_ZME_SHIFT) - 1) :
            ((td_u32)zme.in_stop >> TDE_OSI_ZME_SHIFT);
        zme.in_stop = (td_u32)(zme.out_stop * ratio) >> TDE_OSI_ZME_SHIFT; /* 12 alg data */
        if (zme.in_stop + 2 > (reg->ori_in_height - 1)) { /* 2 alg data */
            /* the down few point need mirror pixels when scale */
            zme.in_stop = reg->ori_in_height - 1;
        } else {
            /* the down few point not need mirror pixels when scale */
            zme.in_stop = zme.in_stop + 2; /* 2 alg data */
        }
    }
    tde_osi_set_vupdate_info(info, &zme);
}

#undef TDE_OSI_ZME_SHIFT
#endif

static td_void tde_osi_src2_filter_reg(const drv_tde_double_src *double_src, tde_update_config *reg)
{
    reg->ori_in_height = double_src->fg_surface->height;
    reg->ori_in_width = double_src->fg_surface->width;
    reg->zme_out_height = double_src->dst_surface->height;
    reg->zme_out_width = double_src->dst_surface->width;
    reg->update_instart_w = double_src->fg_rect->pos_x;
    reg->update_instart_h = double_src->fg_rect->pos_y;
    reg->update_in_width = double_src->fg_rect->width;
    reg->update_in_height = double_src->fg_rect->height;
}
static td_void tde_osi_src2_filter_rect_opt(tde_rect_opt *rect_opt, const tde_update_info *info)
{
    rect_opt->in_rect->pos_x = info->zme_instart_w;
    rect_opt->in_rect->pos_y = info->zme_instart_h;
    rect_opt->in_rect->width = info->zme_in_width;
    rect_opt->in_rect->height = info->zme_in_height;

    rect_opt->out_rect->pos_x = info->zme_outstart_w;
    rect_opt->out_rect->pos_y = info->zme_outstart_h;
    rect_opt->out_rect->width = info->zme_out_width;
    rect_opt->out_rect->height = info->zme_out_height;
}
static td_s32 tde_osi_src2_filter_opt(tde_hw_node *node, drv_tde_double_src *double_src, tde_rect_opt *rect_opt,
    drv_tde_deflicker_mode deflicker_mode, drv_tde_deflicker_mode filter_mode)
{
    td_s32 ret;
    tde_update_config reg = { 0 };
    tde_update_info info = { 0 };
    td_bool is_deflicker;
    td_bool scale = TD_FALSE;

#if (TDE_CAPABILITY & DEFLICKER)
    is_deflicker = (deflicker_mode == DRV_TDE_DEFLICKER_LEVEL_MODE_NONE) ? TD_FALSE : TD_TRUE;
#endif
    tde_osi_src2_filter_reg(double_src, &reg);
    if (node->src2_hsp.bits.hratio != TDE_NO_SCALE_HSTEP) {
        scale = TD_TRUE;
    }
    tde_info("is_h_scale:%x\n", scale);
    tde_osi_get_hupdate_info(&reg, &info, scale);

    scale = TD_FALSE;
    if (node->src2_vsr.bits.vratio != TDE_NO_SCALE_VSTEP) {
        scale = TD_TRUE;
    }
    tde_info("is_v_scale:%x\n", scale);
    tde_osi_get_vupdate_info(&reg, &info, scale);

    node->src2_imgsize.bits.src2_width = info.zme_in_width - 1;
    node->src2_zmeireso.bits.iw = info.zme_in_width - 1;

    node->src2_imgsize.bits.src2_height = info.zme_in_height - 1;
    node->src2_zmeireso.bits.ih = info.zme_in_height - 1;

    double_src->dst_rect->pos_x = info.zme_outstart_w;
    double_src->dst_rect->pos_y = info.zme_outstart_h;
    double_src->dst_rect->width = info.zme_out_width;
    double_src->dst_rect->height = info.zme_out_height;

    node->src2_hcoffset = info.zme_hphase;
    node->src2_voffset.bits.vluma_offset = info.zme_vphase;

    tde_osi_src2_filter_rect_opt(rect_opt, &info);
    if ((rect_opt->in_rect->width > MAX_LINE_BUFFER) && (rect_opt->out_rect->width > MAX_LINE_BUFFER)) {
    tde_error("it does not support local resize in width %u && out width %u over max line buffer %d!! \n",
              rect_opt->in_rect->width, rect_opt->out_rect->width, MAX_LINE_BUFFER);
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    ret = tde_hal_calc_src2_filter_opt(node, rect_opt, is_deflicker, filter_mode);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    return TD_SUCCESS;
}
static td_s32 tde_osi_filter_reg(tde_hw_node *node, drv_tde_double_src *double_src,
    drv_tde_deflicker_mode deflicker_mode, drv_tde_deflicker_mode filter_mode)
{
    td_s32 ret;
    tde_rect_opt rect_opt;
    td_phys_addr_t src2_addr;
    td_phys_addr_t dst_addr;
    drv_tde_rect in_rect;
    drv_tde_rect out_rect;
    drv_tde_color_fmt out_fmt = double_src->dst_surface->color_format;
    td_bool back_ground_operation = ((double_src->bg_surface != TD_NULL) && (double_src->bg_rect != TD_NULL));

    rect_opt.in_fmt = double_src->fg_surface->color_format;
    rect_opt.in_rect = &in_rect;
    rect_opt.out_rect = &out_rect;
    if (((double_src->fg_surface->width != double_src->fg_rect->width) ||
        (double_src->fg_surface->height != double_src->fg_rect->height)) && g_region_deflicker) {
        ret = tde_osi_src2_filter_opt(node, double_src, &rect_opt, deflicker_mode, filter_mode);
        if (ret != TD_SUCCESS) {
            return ret;
        }
        if (double_src->dst_surface->width <= 1) {
            node->src2_hsp.bits.hratio = 0;
        } else {
            node->src2_hsp.bits.hratio = osal_div_u64(((td_u64)(double_src->fg_surface->width) <<
                TDE_HAL_HSTEP_FLOATLEN), (double_src->dst_surface->width));
        }
        if (double_src->dst_surface->height <= 1) {
            node->src2_vsr.bits.vratio = 0;
        } else {
            node->src2_vsr.bits.vratio = osal_div_u64(((td_u64)(double_src->fg_surface->height) << TDE_FLOAT_BITLEN),
                (double_src->dst_surface->height));
        }
        src2_addr = double_src->fg_surface->phys_addr;
        dst_addr = double_src->dst_surface->phys_addr;
        src2_addr += rect_opt.in_rect->pos_y * node->src2_ch0_stride.bits.src2_ch0_stride  +
            ((rect_opt.in_rect->pos_x * tde_osi_get_bpp_by_fmt(rect_opt.in_fmt)) / 8); /* 8 bits */
        dst_addr += rect_opt.out_rect->pos_y * node->des_ch0_stride.bits.des_ch0_stride +
            ((rect_opt.out_rect->pos_x * tde_osi_get_bpp_by_fmt(out_fmt)) / 8); /* 8 bits */

        node->src2_ch0_addr_low.bits.src2_ch0_addr_low = get_low_addr(src2_addr);
        node->src2_ch0_addr_high.bits.src2_ch0_addr_high = get_high_addr(src2_addr);

        node->des_ch0_addr_low.bits.des_ch0_addr_low = get_low_addr(dst_addr);
        node->des_ch0_addr_high.bits.des_ch0_addr_high = get_high_addr(dst_addr);
        if (back_ground_operation) {
            node->src1_imgsize.bits.src1_width = node->des_imgsize.bits.des_width;
            node->src1_imgsize.bits.src1_height = node->des_imgsize.bits.des_height;
        }
    }
    return TD_SUCCESS;
}
static td_s32 tde_osi_filter_opera(tde_hw_node *node, drv_tde_double_src *double_src,
    drv_tde_deflicker_mode deflicker_mode, drv_tde_deflicker_mode filter_mode)
{
    td_s32 ret;
    td_bool is_deflicker;
    tde_rect_opt rect_opt;
    td_bool fore_ground_operation = ((double_src->fg_surface != TD_NULL) && (double_src->fg_rect != TD_NULL));

#if (TDE_CAPABILITY & DEFLICKER)
    is_deflicker = (deflicker_mode == DRV_TDE_DEFLICKER_LEVEL_MODE_NONE) ? TD_FALSE : TD_TRUE;
#endif
    if (fore_ground_operation) {
        if ((double_src->fg_rect->width > MAX_LINE_BUFFER) && (double_src->dst_rect->width > MAX_LINE_BUFFER)) {
            tde_error("it does not support resize in width %u && out width %u over max line buffer %d!! \n",
                      double_src->fg_rect->width, double_src->dst_rect->width, MAX_LINE_BUFFER);
            return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
        }
        rect_opt.in_fmt = double_src->fg_surface->color_format;
        rect_opt.in_rect = double_src->fg_rect;
        rect_opt.out_rect = double_src->dst_rect;
        ret = tde_hal_calc_src2_filter_opt(node, &rect_opt, is_deflicker, filter_mode);
        if (ret != TD_SUCCESS) {
            return ret;
        }
        ret = tde_osi_filter_reg(node, double_src, deflicker_mode, filter_mode);
        if (ret != TD_SUCCESS) {
            return ret;
        }
    }
    return TD_SUCCESS;
}
static td_s32 tde_osi_set_filter_node(td_s32 handle, tde_hw_node *node, drv_tde_double_src *double_src,
    drv_tde_deflicker_mode deflicker_mode, drv_tde_deflicker_mode filter_mode)
{
    td_s32 ret;
    td_bool back_ground_operation = ((double_src->bg_surface != TD_NULL) && (double_src->bg_rect != TD_NULL));

    if (back_ground_operation && (g_region_deflicker == TD_FALSE)) {
        if ((double_src->bg_rect->width != double_src->dst_rect->width) ||
            (double_src->bg_rect->height != double_src->dst_rect->height)) {
            tde_error("it does not support  src1 resize!! \n");
            return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
        }
    }
    ret = tde_osi_filter_opera(node, double_src, deflicker_mode, filter_mode);
    if (ret != TD_SUCCESS) {
        return ret;
    }
#if (TDE_CAPABILITY & SLICE)
    return tde_osi_calc_slice(handle, node);
#else
    ret = tde_osi_set_node_finish(handle, node, 0, TDE_NODE_SUBM_ALONE);
    if (ret < 0) {
        return ret;
    }
    return TD_SUCCESS;
#endif
}
/*
 * Function:      tde_osi_adj_para4_ycb_cr422_r
 * Description:   when fill color is YCbCr422R, fill by word
 * Input:         dst_surface: target bitmap info struct
 *                dst_rect: target operate zone
 *                fill_color: fill color
 * Others:        add software fill, when YCbCr422, fill function by word
 */
static td_void tde_osi_adj_para4_ycb_cr422_r(drv_tde_surface *dst_surface, drv_tde_rect *dst_rect,
                                             drv_tde_fill_color *fill_color)
{
    if ((dst_surface->color_format != DRV_TDE_COLOR_FMT_YCBCR422) &&
        (dst_surface->color_format != DRV_TDE_COLOR_FMT_PKGVYUY)) {
        return;
    }
    dst_surface->color_format = DRV_TDE_COLOR_FMT_AYCBCR8888;
    dst_surface->alpha_max_is_255 = TD_TRUE;
    fill_color->color_format = DRV_TDE_COLOR_FMT_AYCBCR8888;

    dst_surface->width /= 2; /* 2 width */

    dst_rect->width /= 2; /* 2 width */
    dst_rect->pos_x /= 2; /* 2 pos_x */

    fill_color->color_value = tde_get_yc422r_fillvalue(fill_color->color_value);
}
static td_s32 tde_osi_check_fill_opt(const drv_tde_opt *opt)
{
    td_bool real = (((opt->blend_opt.global_alpha_en != TD_TRUE) && (opt->blend_opt.global_alpha_en != TD_FALSE)) ||
        ((opt->blend_opt.pixel_alpha_en != TD_TRUE) && (opt->blend_opt.pixel_alpha_en != TD_FALSE)) ||
        ((opt->blend_opt.src1_alpha_premulti != TD_TRUE) && (opt->blend_opt.src1_alpha_premulti != TD_FALSE)) ||
        ((opt->blend_opt.src2_alpha_premulti != TD_TRUE) && (opt->blend_opt.src2_alpha_premulti != TD_FALSE)) ||
        ((opt->clut_reload != TD_TRUE) && (opt->clut_reload != TD_FALSE)) ||
        ((opt->is_compress != TD_TRUE) && (opt->is_compress != TD_FALSE)) ||
        ((opt->is_decompress != TD_TRUE) && (opt->is_decompress != TD_FALSE)));

    if ((opt->colorkey_mode >= DRV_TDE_COLOR_KEY_MODE_MAX) ||
        (opt->colorkey_mode < DRV_TDE_COLOR_KEY_MODE_NONE)) {
        tde_error("color_key_mode error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    if (real) {
        tde_error("bool should be TRUE or FALSE!!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    if ((opt->deflicker_mode >= DRV_TDE_DEFLICKER_LEVEL_MODE_MAX) ||
        (opt->deflicker_mode < DRV_TDE_DEFLICKER_LEVEL_MODE_NONE)) {
        tde_error("deflicker_mode error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    if ((opt->filter_mode >= DRV_TDE_FILTER_MODE_MAX) || (opt->filter_mode < DRV_TDE_FILTER_MODE_COLOR)) {
        tde_error("filter_mode error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    if ((opt->mirror >= DRV_TDE_MIRROR_MAX) || (opt->mirror < DRV_TDE_MIRROR_NONE)) {
        tde_error("mirror error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (opt->alpha_blending_cmd >= DRV_TDE_ALPHA_BLENDING_MAX ||
        opt->alpha_blending_cmd < DRV_TDE_ALPHA_BLENDING_NONE) {
        tde_error("alpha_blending_cmd error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (opt->alpha_blending_cmd != DRV_TDE_ALPHA_BLENDING_NONE) {
        tde_error("invalid alu command!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (opt->resize) {
        tde_error("Not support resize in single source fill operation!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    return TD_SUCCESS;
}

static td_s32 tde_osi_1_source_src1(drv_tde_surface *dst_surface, const drv_tde_opt *opt, tde_hw_node *hw_node,
                                    drv_tde_rect *dst_rect)
{
    td_s32 ret;
    drv_tde_double_src double_src;
    ret = tde_osi_check_fill_opt(opt);
    if (ret != TD_SUCCESS) {
        return DRV_ERR_TDE_INVALID_PARA;
    }
    if ((opt->out_alpha_from >= DRV_TDE_OUT_ALPHA_FROM_MAX) ||
        (opt->out_alpha_from == DRV_TDE_OUT_ALPHA_FROM_FOREGROUND)) {
        tde_error("out_alpha_from error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    tde_hal_node_set_global_alpha(hw_node, opt->global_alpha, opt->blend_opt.global_alpha_en);

    double_src.bg_surface = TD_NULL;
    double_src.bg_rect = TD_NULL;
    double_src.fg_surface = dst_surface;
    double_src.fg_rect = dst_rect;
    double_src.dst_surface = dst_surface;
    double_src.dst_rect = dst_rect;
    if (tde_osi_set_clip_para(&double_src, opt, hw_node) != TD_SUCCESS) {
        return DRV_ERR_TDE_CLIP_AREA;
    }
    return TD_SUCCESS;
}

static td_s32 tde_osi_1_source_check(drv_tde_surface *dst_surface, const drv_tde_rect *dst_rect,
                                     drv_tde_fill_color *fill_color, drv_tde_rect *rect)
{
    td_bool null_ptr = ((dst_surface == TD_NULL) || (dst_rect == TD_NULL) || (fill_color == TD_NULL));
    td_s32 bpp;

    if (null_ptr) {
        tde_error("NULL pointer!\n");
        return DRV_ERR_TDE_NULL_PTR;
    }
    if (memcpy_s(rect, sizeof(drv_tde_rect), dst_rect, sizeof(drv_tde_rect)) != EOK) {
        tde_error("secure function failure\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if ((dst_surface->color_format >= DRV_TDE_COLOR_FMT_JPG_YCBCR400MBP) ||
        (fill_color->color_format >= DRV_TDE_COLOR_FMT_JPG_YCBCR400MBP)) {
        tde_error("This operation doesn't support Semi-plannar!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

#ifdef TDE_BACKGROUND_COLORFOMATSUPPORT_YCBCR422
    if (dst_surface->color_format >= DRV_TDE_COLOR_FMT_YCBCR422) {
        tde_error("Background  doesn't support the colorfmt!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
#endif

    if (tde_osi_check_surface(dst_surface, rect) != TD_SUCCESS) {
        return DRV_ERR_TDE_INVALID_PARA;
    }

    bpp = tde_osi_get_bpp_by_fmt(dst_surface->color_format);
    if (bpp < 0) {
        tde_error("Unknown color format!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (bpp < 8) { /* 8 is bpp */
        /* when writing, 8 subbyte format align ask start point byte align */
        if ((rect->pos_x * bpp % 8) || (rect->width * bpp % 8)) {
            tde_error("The input start or write width does not meet the single byte alignment!\n");
            return DRV_ERR_TDE_INVALID_PARA;
        }
    }
    tde_osi_adj_para4_ycb_cr422_r(dst_surface, rect, fill_color);
    return TD_SUCCESS;
}

static td_void tde_osi_1_source_drv(const drv_tde_surface *dst_surface, tde_base_opt_mode base_mode,
                                    const drv_tde_rect *dst_rect, tde_hw_node *hw_node,
                                    tde_drv_outalpha_from out_alpha_from)
{
    tde_scandirection_mode scan_info = { 0 };
    tde_surface_msg drv_surface = { 0 };
    scan_info.hor_scan = TDE_SCAN_LEFT_RIGHT;
    scan_info.ver_scan = TDE_SCAN_UP_DOWN;
    tde_osi_convert_surface(dst_surface, dst_rect, &scan_info, &drv_surface);

    if (base_mode == TDE_NORM_FILL_1OPT) {
        tde_hal_node_set_src1(hw_node, &drv_surface);
    }

    tde_hal_node_set_tqt(hw_node, &drv_surface, out_alpha_from);

    tde_osi_set_ext_alpha(dst_surface, TD_NULL, hw_node);
    return;
}

/*
 * Function:      tde_osi_1_source_fill
 * Description:   single source fill operate,source1 is fill color,target bitmap is dst_surface,
                  support source1 and fill color do ROP or alpha blending to target bitmap,
                  unsupported mirror,colorkey
 *                src bitmap is not support MB color format
 * Input:         handle: task handle
 *                dst_surface: foreground bitmap info struct
 *                fill_color:  target bitmap info struct
 *                opt: operate parameter setting struct
 * Return:        TD_SUCCESS/TD_FAILURE
 */
static td_s32 tde_osi_1_source_fill(td_s32 handle, drv_tde_surface *dst_surface, drv_tde_rect *dst_rect,
                                    drv_tde_fill_color *fill_color, const drv_tde_opt *opt)
{
    tde_base_opt_mode base_mode = { 0 };
    tde_hw_node *hw_node = TD_NULL;
    tde_drv_outalpha_from out_alpha_from = DRV_TDE_OUT_ALPHA_FROM_NORM;
    drv_tde_rect rect = { 0 };
    tde_color_fill drv_color_fill = { 0 };
    td_s32 ret = tde_osi_1_source_check(dst_surface, dst_rect, fill_color, &rect);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    if (tde_hal_node_init_nd(&hw_node) != TD_SUCCESS) {
        return DRV_ERR_TDE_NO_MEM;
    }

    if (opt == TD_NULL) {
        base_mode = (fill_color->color_format == dst_surface->color_format) ? TDE_QUIKE_FILL : TDE_NORM_FILL_1OPT;
        out_alpha_from = DRV_TDE_OUT_ALPHA_FROM_NORM;
    } else {
        ret = tde_osi_1_source_src1(dst_surface, opt, hw_node, &rect);
        if (ret != TD_SUCCESS) {
            tde_hal_free_node_buf(hw_node);
            return ret;
        }
        out_alpha_from = opt->out_alpha_from;
        base_mode = (fill_color->color_format == dst_surface->color_format) ? TDE_QUIKE_FILL : TDE_NORM_FILL_1OPT;
    }
    if (base_mode == TDE_NORM_FILL_1OPT) {
        if (tde_osi_color_convert(fill_color, dst_surface, &drv_color_fill.fill_data) != TD_SUCCESS) {
            tde_hal_free_node_buf(hw_node);
            return DRV_ERR_TDE_INVALID_PARA;
        }
        drv_color_fill.drv_color_fmt = TDE_DRV_COLOR_FMT_ARGB8888;
    } else {
        drv_color_fill.fill_data = fill_color->color_value;
        drv_color_fill.drv_color_fmt = g_tde_common_drv_color_fmt[fill_color->color_format];
    }
    if (tde_hal_node_set_base_operate(hw_node, base_mode, TDE_ALU_NONE, &drv_color_fill) != TD_SUCCESS) {
        tde_hal_free_node_buf(hw_node);
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    tde_osi_1_source_drv(dst_surface, base_mode, &rect, hw_node, out_alpha_from);
    ret = tde_osi_set_node_finish(handle, hw_node, 0, TDE_NODE_SUBM_ALONE);
    if (ret != TD_SUCCESS) {
        tde_hal_free_node_buf(hw_node);
        return ret;
    }
    return TD_SUCCESS;
}

/*
 * Function:      tde_osi_des_dma_draw_rect
 * Description:   single source fill operate,source1 is fill color,target bitmap is dst_surface,support
 *                source1 and fill color do ROP or alpha blending to target bitmap, unsupported mirror,colorkey
 *                 src bitmap is not support MB color format
 * Input:         handle: task handle
 *                pSrc: background bitmap info struct
 *                dst_surface: foreground bitmap info struct
 *                pstFillColor:  target bitmap info struct
 *                pstOpt: operate parameter setting struct
 * Output:        none
 * Return:        TD_SUCCESS/TD_FAILURE
 * Others:        none
 */
static td_s32 tde_osi_des_dma_draw_rect(td_s32 handle, const drv_tde_surface *dst_surface, const drv_tde_rect *dst_rect,
                                        const drv_tde_corner_rect_info *corner_info, const drv_tde_opt *opt)
{
    tde_hw_node *hw_node = TD_NULL;
    tde_surface_msg surface = { 0 };
    tde_corner_rect_info corner_hal_info = { 0 };
    tde_scandirection_mode st_scan_info = { 0 };
    tde_drv_outalpha_from out_alpha_from = DRV_TDE_OUT_ALPHA_FROM_NORM;
    drv_tde_rect tmp_dst_rect = { 0 };
    td_s32 ret = TD_FAILURE;
    ot_unused(opt);
    if ((dst_surface == TD_NULL) || (dst_rect == TD_NULL) || (corner_info == TD_NULL)) {
        tde_error("NULL pointer!\n");
        return DRV_ERR_TDE_NULL_PTR;
    }
    if (memcpy_s(&tmp_dst_rect, sizeof(tmp_dst_rect), dst_rect, sizeof(drv_tde_rect)) != EOK) {
        tde_error("secure function failure\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (tde_osi_check_format_support_draw(dst_surface->color_format) != TD_TRUE) {
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (tde_osi_check_surface(dst_surface, &tmp_dst_rect) != TD_SUCCESS) {
        return DRV_ERR_TDE_INVALID_PARA;
    }
    if (tde_check_subbyte_startx(tmp_dst_rect.pos_x, tmp_dst_rect.width, dst_surface->color_format) != TD_SUCCESS) {
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (tde_hal_node_init_nd(&hw_node) != TD_SUCCESS) {
        return DRV_ERR_TDE_NO_MEM;
    }
    corner_hal_info.drv_color_fmt = g_tde_common_drv_color_fmt[dst_surface->color_format];
    corner_hal_info.width = corner_info->width;
    corner_hal_info.height = corner_info->height;
    corner_hal_info.inner_color = corner_info->inner_color;
    corner_hal_info.outer_color = corner_info->outer_color;

    st_scan_info.hor_scan = TDE_SCAN_LEFT_RIGHT;
    st_scan_info.ver_scan = TDE_SCAN_UP_DOWN;

    tde_osi_convert_surface(dst_surface, &tmp_dst_rect, &st_scan_info, &surface);
    tde_hal_node_set_tqt(hw_node, &surface, out_alpha_from);
    tde_hal_node_set_corner_rect(hw_node, &corner_hal_info);

    if ((ret = tde_osi_set_node_finish(handle, hw_node, 0, TDE_NODE_SUBM_ALONE)) != TD_SUCCESS) {
        tde_hal_free_node_buf(hw_node);
        return ret;
    }
    return TD_SUCCESS;
}

#ifdef CONFIG_TDE_DRD_LINE_SUPPORT
static td_s32 tde_osi_des_dma_draw_line(td_s32 handle, const drv_tde_surface *dst_surface, const drv_tde_line *line,
                                        td_u32 num)
{
    tde_hw_node *hw_node = TD_NULL;
    tde_surface_msg surface = { 0 };
    drv_tde_rect dst_rect;
    tde_line_info line_hal_info[TDE_MAX_LINE_NUM] = { 0 };
    tde_scandirection_mode st_scan_info = { 0 };
    td_s32 ret;
    td_u32 i;
    if ((dst_surface == TD_NULL) || (line == TD_NULL)) {
        tde_error("NULL pointer!\n");
        return DRV_ERR_TDE_NULL_PTR;
    }

    if (tde_hal_node_init_nd(&hw_node) != TD_SUCCESS) {
        return DRV_ERR_TDE_NO_MEM;
    }
    for (i = 0; i < num; i++) {
        line_hal_info[i].start_x = line[i].start_x;
        line_hal_info[i].start_y = line[i].start_y;
        line_hal_info[i].end_x = line[i].end_x;
        line_hal_info[i].end_y = line[i].end_y;
        line_hal_info[i].thick = line[i].thick;
        line_hal_info[i].color = line[i].color;
        line_hal_info[i].enable = 0x1;
    }

    st_scan_info.hor_scan = TDE_SCAN_LEFT_RIGHT;
    st_scan_info.ver_scan = TDE_SCAN_UP_DOWN;

    /* draw line rect default full surface */
    dst_rect.pos_x = 0;
    dst_rect.pos_y = 0;
    dst_rect.width = dst_surface->width;
    dst_rect.height = dst_surface->height;

    tde_osi_convert_surface(dst_surface, &dst_rect, &st_scan_info, &surface);
    tde_hal_node_set_tqt(hw_node, &surface, DRV_TDE_OUT_ALPHA_FROM_NORM);
    tde_hal_node_draw_line(hw_node, line_hal_info, TDE_MAX_LINE_NUM);

    if ((ret = tde_osi_set_node_finish(handle, hw_node, 0, TDE_NODE_SUBM_ALONE)) != TD_SUCCESS) {
        tde_hal_free_node_buf(hw_node);
        return ret;
    }
    return TD_SUCCESS;
}
#else
static td_s32 tde_osi_des_dma_draw_line(td_s32 handle, const drv_tde_surface *dst_surface, const drv_tde_line *line,
                                        td_u32 num)
{
    ot_unused(handle);
    ot_unused(dst_surface);
    ot_unused(line);
    ot_unused(num);
    tde_error("DRD draw line unsupported!\n");
    return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
}
#endif

static td_s32 tde_osi_2_check_opt(const drv_tde_opt *opt)
{
    td_bool real = (((opt->blend_opt.global_alpha_en != TD_TRUE) && (opt->blend_opt.global_alpha_en != TD_FALSE)) ||
        ((opt->blend_opt.pixel_alpha_en != TD_TRUE) && (opt->blend_opt.pixel_alpha_en != TD_FALSE)) ||
        ((opt->blend_opt.src1_alpha_premulti != TD_TRUE) && (opt->blend_opt.src1_alpha_premulti != TD_FALSE)) ||
        ((opt->blend_opt.src2_alpha_premulti != TD_TRUE) && (opt->blend_opt.src2_alpha_premulti != TD_FALSE)) ||
        ((opt->clut_reload != TD_TRUE) && (opt->clut_reload != TD_FALSE)) ||
        ((opt->is_compress != TD_TRUE) && (opt->is_compress != TD_FALSE)) ||
        ((opt->is_decompress != TD_TRUE) && (opt->is_decompress != TD_FALSE)));

    if (opt->out_alpha_from >= DRV_TDE_OUT_ALPHA_FROM_MAX) {
        tde_error("out_alpha_from error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (opt->alpha_blending_cmd >= DRV_TDE_ALPHA_BLENDING_MAX) {
        tde_error("alpha_blending_cmd error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if ((opt->colorkey_mode >= DRV_TDE_COLOR_KEY_MODE_MAX) ||
        (opt->colorkey_mode < DRV_TDE_COLOR_KEY_MODE_NONE)) {
        tde_error("color_key_mode error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (real) {
        tde_error("bool should be TRUE or FALSE!!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if ((opt->deflicker_mode >= DRV_TDE_DEFLICKER_LEVEL_MODE_MAX) ||
        (opt->deflicker_mode < DRV_TDE_DEFLICKER_LEVEL_MODE_NONE)) {
        tde_error("deflicker_mode error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if ((opt->filter_mode >= DRV_TDE_FILTER_MODE_MAX) || (opt->filter_mode < DRV_TDE_FILTER_MODE_COLOR)) {
        tde_error("filter_mode error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if ((opt->mirror >= DRV_TDE_MIRROR_MAX) || (opt->mirror < DRV_TDE_MIRROR_NONE)) {
        tde_error("mirror error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    return TD_SUCCESS;
}

static td_s32 tde_osi_2_source_check(const drv_tde_single_src *single_src, const drv_tde_fill_color *fill_color,
                                     const drv_tde_opt *opt, const drv_tde_single_src *single_surface_dou)
{
    td_s32 ret;
    td_bool null_ptr = ((single_src->dst_surface == TD_NULL) || (single_src->dst_rect == TD_NULL) ||
                        (fill_color == TD_NULL) || ((opt == TD_NULL)) ||
                        (single_src->src_surface == TD_NULL) || (single_src->src_rect == TD_NULL));

    if (null_ptr) {
        return DRV_ERR_TDE_NULL_PTR;
    }

    ret = memcpy_s(single_surface_dou->src_rect, sizeof(drv_tde_rect), single_src->src_rect, sizeof(drv_tde_rect));
    tde_unequal_eok_return(ret);
    ret = memcpy_s(single_surface_dou->dst_rect, sizeof(drv_tde_rect), single_src->dst_rect, sizeof(drv_tde_rect));
    tde_unequal_eok_return(ret);
    if (fill_color->color_format >= DRV_TDE_COLOR_FMT_MAX) {
        tde_error("color format error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    ret = tde_osi_check_dst_fmt(single_src->dst_surface->color_format);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    if ((single_src->src_surface->color_format == DRV_TDE_COLOR_FMT_YCBCR422) ||
        (single_src->src_surface->color_format == DRV_TDE_COLOR_FMT_PKGVYUY)) {
        tde_error("This operation doesn't support PKG!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    ret = tde_osi_2_check_opt(opt);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    if (tde_osi_check_surface(single_src->dst_surface, single_surface_dou->dst_rect) != TD_SUCCESS) {
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (tde_osi_check_surface(single_src->src_surface, single_surface_dou->src_rect) != TD_SUCCESS) {
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (opt->resize) {
        if (tde_osi_check_resize_para(single_src->src_rect->width, single_src->src_rect->height,
            single_src->dst_rect->width, single_src->dst_rect->height) != TD_SUCCESS) {
            return DRV_ERR_TDE_MINIFICATION;
        }
    } else {
        tde_unify_rect(single_surface_dou->src_rect, single_surface_dou->dst_rect);
    }
    return TD_SUCCESS;
}

static td_s32 tde_osi_2_source_para(const drv_tde_single_src *single_src, const drv_tde_fill_color *fill_color,
    const drv_tde_opt *opt, tde_hw_node *hw_node, const drv_tde_single_src *single_surface_dou)
{
    td_s32 ret;
    drv_tde_double_src double_src;
    tde_alu_mode alu_mode = TDE_ALU_NONE;
    tde_color_fill drv_color_fill = {
        0
    };
#if (TDE_CAPABILITY & ROP)
    tde_rop_opt rop_opt = {0};
    rop_opt.alpha_blending_cmd = opt->alpha_blending_cmd;
    rop_opt.rop_code_color = opt->rop_color;
    rop_opt.rop_code_alpha = opt->rop_alpha;
    rop_opt.single_sr2_rop = TD_FALSE;
    ret = tde_osi_set_rop(hw_node, &rop_opt, &alu_mode);
    if (ret != TD_SUCCESS) {
        return ret;
    }
#endif

    ret = tde_osi_set_blend(hw_node, opt->alpha_blending_cmd, opt->blend_opt, &alu_mode, TD_TRUE);
    if (ret != TD_SUCCESS) {
        return ret;
    }
#if (TDE_CAPABILITY & COLORIZE)
    ret = tde_osi_set_colorize(hw_node, opt->alpha_blending_cmd, opt->color_resize);
    if (ret != TD_SUCCESS) {
        return ret;
    }
#endif
    tde_hal_node_set_global_alpha(hw_node, opt->global_alpha, opt->blend_opt.global_alpha_en);

    double_src.bg_surface = TD_NULL;
    double_src.bg_rect = TD_NULL;
    double_src.fg_surface = single_src->src_surface;
    double_src.fg_rect = single_surface_dou->src_rect;
    double_src.dst_surface = single_src->dst_surface;
    double_src.dst_rect = single_surface_dou->dst_rect;
    if (tde_osi_set_clip_para(&double_src, opt, hw_node) != TD_SUCCESS) {
        return DRV_ERR_TDE_CLIP_AREA;
    }

    if (tde_osi_color_convert(fill_color, single_src->src_surface, &drv_color_fill.fill_data) != TD_SUCCESS) {
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (tde_hal_node_set_base_operate(hw_node, TDE_NORM_FILL_2OPT, alu_mode, &drv_color_fill) != TD_SUCCESS) {
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    return TD_SUCCESS;
}

static td_s32 tde_osi_2_source_node(td_s32 handle, const drv_tde_single_src *single_src, const drv_tde_opt *opt,
                                    tde_hw_node *hw_node, const drv_tde_single_src *single_surface_dou)
{
    drv_tde_double_src double_src;
    tde_clut_usage clut_usage = TDE_CLUT_USAGE_BUTT;
    td_bool set_file_node;
    td_s32 ret = tde_osi_set_clut_opt(single_src->src_surface, single_src->dst_surface, &clut_usage,
        opt->clut_reload, hw_node);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = tde_osi_set_foreground_color_key(hw_node, single_src->src_surface, opt, clut_usage);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    set_file_node = ((opt->resize) || (opt->deflicker_mode != DRV_TDE_DEFLICKER_LEVEL_MODE_NONE) ||
                     (single_src->src_surface->color_format >= DRV_TDE_COLOR_FMT_YCBCR422));
    if (set_file_node) {
        double_src.bg_surface = single_src->src_surface;
        double_src.bg_rect = single_surface_dou->src_rect;
        double_src.fg_surface = TD_NULL;
        double_src.fg_rect = TD_NULL;
        double_src.dst_surface = single_src->dst_surface;
        double_src.dst_rect = single_surface_dou->dst_rect;
        ret = tde_osi_set_filter_node(handle, hw_node, &double_src,
                                      opt->deflicker_mode, (drv_tde_deflicker_mode)opt->filter_mode);
        if (ret != TD_SUCCESS) {
            return ret;
        }
#if (TDE_CAPABILITY & SLICE)
        tde_hal_free_node_buf(hw_node);
#endif
        return TD_SUCCESS;
    }
    ret = tde_osi_set_node_finish(handle, hw_node, 0, TDE_NODE_SUBM_ALONE);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    return TD_SUCCESS;
}

static td_s32 tde_osi_2_source_fill(td_s32 handle, const drv_tde_single_src *single_src,
    const drv_tde_fill_color *fill_color, const drv_tde_opt *opt)
{
    tde_hw_node *hw_node = TD_NULL;
    tde_surface_msg drv_surface = { 0 };
    tde_scandirection_mode src_scan_info = { 0 };
    tde_scandirection_mode dst_scan_info = { 0 };
    drv_tde_rect src_rect = { 0 };
    drv_tde_rect dst_rect = { 0 };
    td_s32 ret;
    drv_tde_single_src single_surface_dou;

    single_surface_dou.dst_rect = &dst_rect;
    single_surface_dou.dst_surface = TD_NULL;
    single_surface_dou.src_rect = &src_rect;
    single_surface_dou.src_surface = TD_NULL;
    ret = tde_osi_2_source_check(single_src, fill_color, opt, &single_surface_dou);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    if (tde_hal_node_init_nd(&hw_node) != TD_SUCCESS) {
        return DRV_ERR_TDE_NO_MEM;
    }

    ret = tde_osi_2_source_para(single_src, fill_color, opt, hw_node, &single_surface_dou);
    if (ret != TD_SUCCESS) {
        tde_hal_free_node_buf(hw_node);
        return ret;
    }
    tde_osi_set_ext_alpha(TD_NULL, single_src->src_surface, hw_node);
    single_surface_dou.src_surface = single_src->src_surface;
    single_surface_dou.dst_surface = single_src->dst_surface;
    if (tde_osi_get_scan_info_ex(&single_surface_dou, opt, &src_scan_info, &dst_scan_info) != TD_SUCCESS) {
        tde_hal_free_node_buf(hw_node);
        return DRV_ERR_TDE_INVALID_PARA;
    }

    tde_osi_convert_surface(single_src->src_surface, single_surface_dou.src_rect, &src_scan_info, &drv_surface);

    tde_hal_node_set_src1(hw_node, &drv_surface);

    tde_osi_convert_surface(single_src->dst_surface, single_surface_dou.dst_rect, &dst_scan_info, &drv_surface);

    tde_hal_node_set_tqt(hw_node, &drv_surface, opt->out_alpha_from);
    ret = tde_osi_2_source_node(handle, single_src, opt, hw_node, &single_surface_dou);
    if (ret != TD_SUCCESS) {
        tde_hal_free_node_buf(hw_node);
        return ret;
    }

    return TD_SUCCESS;
}

static td_s32 tde_osi_set_foreground_color_key(tde_hw_node *hw_node, const drv_tde_surface *src_surface,
                                               const drv_tde_opt *opt, tde_clut_usage clut_usage)
{
    tde_color_key_cmd color_key;
    tde_colorfmt_category fmt_category;
    td_bool foreground_colorkey_after_clut;
    color_key.colorkey_value = opt->colorkey_value;
    foreground_colorkey_after_clut = ((clut_usage != TDE_CLUT_COLOREXPENDING) && (clut_usage != TDE_CLUT_CLUT_BYPASS));

    if (opt->colorkey_mode == DRV_TDE_COLOR_KEY_MODE_BACKGROUND) {
        tde_error("Unsupported solidraw colorkey in background mode!\n");

        return DRV_ERR_TDE_INVALID_PARA;
    }

    if ((opt->colorkey_mode != DRV_TDE_COLOR_KEY_MODE_FOREGROUND) &&
        (opt->colorkey_mode != DRV_TDE_COLOR_KEY_MODE_BACKGROUND)) {
        return TD_SUCCESS;
    }

    color_key.colorkey_mode = (foreground_colorkey_after_clut) ? TDE_DRV_COLORKEY_FOREGROUND_AFTER_CLUT :
                                TDE_DRV_COLORKEY_FOREGROUND_BEFORE_CLUT;

    fmt_category = tde_osi_get_fmt_category(src_surface->color_format);
    if (fmt_category >= TDE_COLORFMT_CATEGORY_BUTT) {
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    if (tde_hal_node_set_colorkey(hw_node, fmt_category, &color_key) != TD_SUCCESS) {
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    return TD_SUCCESS;
}

/*
 * Function:      tde_osi_check_surface
 * Description:   adjust right operate zone, according by the size of bitmap and operate zone from user upload
 * Input:         surface: bitmap info
 *                rect: bitmap operate zone
 * Return:        success/fail
 */
static td_s32 tde_osi_check_surface(const drv_tde_surface *surface, drv_tde_rect *rect)
{
    td_bool invalid_operation_area;
    td_s32 ret = tde_osi_pre_check_surface_ex(surface, rect);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    invalid_operation_area = ((TDE_MAX_RECT_WIDTH < rect->width) || (TDE_MAX_RECT_HEIGHT < rect->height));

    if (invalid_operation_area) {
        tde_error("operation area is over maximum!Width:%d,Height:%d\n", rect->width, rect->height);
        return DRV_ERR_TDE_INVALID_PARA;
    }

    return TD_SUCCESS;
}

static td_s32 tde_osi_pre_check_surface(const drv_tde_surface *surface, const drv_tde_rect *rect)
{
    td_bool real = ((surface->stride > TDE_MAX_SURFACE_PITCH) || (surface->stride == 0) || (rect->height == 0) ||
        (rect->width == 0) || (rect->pos_x < 0) || ((td_u32)rect->pos_x >= surface->width) ||
        (rect->pos_y < 0) || ((td_u32)rect->pos_y >= surface->height) || (surface->phys_addr == 0));

    if (surface->color_format >= DRV_TDE_COLOR_FMT_MAX) {
        tde_error("color format error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (real) {
        tde_error("surface_w:%d, surface_h:%d, stride:%d, sphy:%lx, x:%d, y:%d, w:%d, h:%d\n", surface->width,
            surface->height, surface->stride, (td_ulong)surface->phys_addr, rect->pos_x,
            rect->pos_y, rect->width, rect->height);
        tde_error("invalid surface phyaddr or invalid surface size or operation area!\n");
        return -1;
    }
    real = (((surface->support_alpha_ex_1555 != TD_TRUE) && (surface->support_alpha_ex_1555 != TD_FALSE)) ||
        ((surface->alpha_max_is_255 != TD_TRUE) && (surface->alpha_max_is_255 != TD_FALSE)) ||
        ((surface->is_ycbcr_clut != TD_TRUE) && (surface->is_ycbcr_clut != TD_FALSE)));
    if (real) {
        tde_error("bool should be TRUE or FALSE!!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    return TD_SUCCESS;
}

static td_void tde_osi_get_rect_wh(const drv_tde_surface *surface, drv_tde_rect *rect)
{
    if (rect->pos_x + rect->width > surface->width) {
        rect->width = surface->width - rect->pos_x;
    }

    if (rect->pos_y + rect->height > surface->height) {
        rect->height = surface->height - rect->pos_y;
    }
}

static td_s32 tde_osi_pre_check_surface_ex(const drv_tde_surface *surface, drv_tde_rect *rect)
{
    td_bool unknown_color_fmt;
    td_bool raster_fmt;

    td_s32 ret = tde_osi_pre_check_surface(surface, rect);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    unknown_color_fmt = (surface->color_format >= DRV_TDE_COLOR_FMT_MAX);
    if (unknown_color_fmt) {
        tde_error("Unknown color format!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    unknown_color_fmt = ((((td_u32)(rect->pos_x) & 0x1) || (rect->width & 0x1)) &&
        ((surface->color_format == DRV_TDE_COLOR_FMT_YCBCR422) ||
        (surface->color_format == DRV_TDE_COLOR_FMT_PKGVYUY)));
    if (unknown_color_fmt) {
        tde_error("x, width of YCbCr422R couldn't be odd!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    raster_fmt = (surface->color_format <= DRV_TDE_COLOR_FMT_HALFWORD);
    if (raster_fmt) {
        ret = tde_osi_raster_fmt_check_align(surface);
        if (ret != TD_SUCCESS) {
            return ret;
        }
    } else {
        if (((surface->cbcr_stride > TDE_MAX_SURFACE_PITCH) || (surface->cbcr_stride == 0)) &&
            (surface->color_format != DRV_TDE_COLOR_FMT_JPG_YCBCR400MBP)) {
            tde_error("Invalid CbCr stride!\n");
            return DRV_ERR_TDE_INVALID_PARA;
        }
    }
    tde_osi_get_rect_wh(surface, rect);
    return TD_SUCCESS;
}

static td_s32 tde_osi_raster_fmt_check_align(const drv_tde_surface *surface)
{
    td_u32 byte_per_pixel;
    td_s32 bpp = tde_osi_get_bpp_by_fmt(surface->color_format);
    if ((bpp >= 8) && (bpp != 24)) { /* 8 24 Bpp */
        byte_per_pixel = (bpp / 8); /* 8 bits */

        if (osal_div_u64_rem(surface->phys_addr, byte_per_pixel)) {
            tde_error("Bitmap address is not aligned!\n");
            return DRV_ERR_TDE_NOT_ALIGNED;
        }

        if (surface->stride % byte_per_pixel) {
            tde_error("stride is not aligned!\n");
            return DRV_ERR_TDE_NOT_ALIGNED;
        }
    } else if (bpp == 24) { /* 24 Bpp */
        if (osal_div_u64_rem(surface->phys_addr, 4)) { /* 4 align_num */
            tde_error("Bitmap address is not aligned!\n");
            return DRV_ERR_TDE_NOT_ALIGNED;
        }
        if (surface->stride % 4) { /* 4 align_num */
            tde_error("stride is not aligned!\n");
            return DRV_ERR_TDE_NOT_ALIGNED;
        }
    }

    return TD_SUCCESS;
}

/*
 * Function:      tde_osi_check_mb_surface_ex
 * Description:   adjust right operate zone,
                  according by the size of bitmap is more than 4095*4095 and less than 8190*8190
 * Input:         mb_surface: bitmap info
 *                rect: bitmap operate zone
 * Return:        success/fail
 */
static td_s32 tde_osi_check_mb_surface_ex(const drv_tde_mb_surface *mb_surface, drv_tde_rect *rect)
{
    td_bool real = ((rect->pos_x < 0) || (rect->width > TDE_MAX_RECT_WIDTH_EX) ||
        (rect->height > TDE_MAX_RECT_HEIGHT_EX) || (rect->height == 0) || (rect->width == 0) ||
        (mb_surface->y_stride > TDE_MAX_SURFACE_PITCH) || (mb_surface->cbcr_stride > TDE_MAX_SURFACE_PITCH) ||
        ((td_u32)rect->pos_x >= mb_surface->y_width) || (rect->pos_y < 0) ||
        ((td_u32)rect->pos_y >= mb_surface->y_height) || (mb_surface->y_addr == 0));

    if (mb_surface->mb_color_format >= DRV_TDE_MB_COLOR_FMT_MAX) {
        tde_error("mb color format error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if ((mb_surface->mb_color_format > DRV_TDE_MB_COLOR_FMT_JPG_YCBCR400MBP) &&
        (mb_surface->mb_color_format < DRV_TDE_MB_COLOR_FMT_MAX)) {
        if (mb_surface->cbcr_stride == 0) {
            tde_error(" mb cbcr_stride is null !\n");
            return DRV_ERR_TDE_INVALID_PARA;
        }
        if (mb_surface->cbcr_phys_addr == 0) {
            tde_error("mb cbcr_phy_addr is 0!\n");
            return DRV_ERR_TDE_INVALID_PARA;
        }
    }
    if (real) {
        tde_error("syw:%u, syh:%u, systride:%u, syphy:%lx, scbcrstride:%u, scbcrphy:%lx, x:%d, y:%d, w:%u, h:%u\n",
            mb_surface->y_width, mb_surface->y_height, mb_surface->y_stride, (td_ulong)mb_surface->y_addr,
            mb_surface->cbcr_stride, (td_ulong)mb_surface->cbcr_phys_addr, rect->pos_x, rect->pos_y, rect->width,
            rect->height);
        tde_error("invalid mbsurface phyaddr or invalid surface size or operation area!\n");
        return -1;
    }
    if ((mb_surface->y_stride % 4) || (mb_surface->cbcr_stride % 4)) { /* 4 align_num */
        tde_error("stride is not aligned!\n");
        return DRV_ERR_TDE_NOT_ALIGNED;
    }

    if ((((td_u32)(rect->pos_x) & 0x1) || (rect->width & 0x1)) &&
        ((mb_surface->mb_color_format > DRV_TDE_MB_COLOR_FMT_JPG_YCBCR400MBP) &&
        (mb_surface->mb_color_format < DRV_TDE_MB_COLOR_FMT_JPG_YCBCR444MBP))) {
        tde_error("x, width of YCbCr422 420 couldn't be odd!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (rect->pos_x + rect->width > mb_surface->y_width) {
        rect->width = mb_surface->y_width - rect->pos_x;
    }

    if (rect->pos_y + rect->height > mb_surface->y_height) {
        rect->height = mb_surface->y_height - rect->pos_y;
    }

    return TD_SUCCESS;
}

/*
 * Function:      tde_osi_get_opt_category
 * Description:   analyze TDE operate type
 * Return:        TDE operate type
 */
static tde_operation_category tde_osi_get_opt_category(drv_tde_double_src *double_src, const drv_tde_opt *opt)
{
    if ((double_src->dst_surface == TD_NULL) || (double_src->dst_rect == TD_NULL)) {
        tde_error("dst is NULL!\n");
        return TDE_OPERATION_BUTT;
    }

    if (double_src->dst_surface->color_format >= DRV_TDE_COLOR_FMT_JPG_YCBCR400MBP) {
        tde_error("This operation doesn't support Semi-plannar!\n");
        return TDE_OPERATION_BUTT;
    }

    if (tde_osi_check_surface(double_src->dst_surface, double_src->dst_rect) != TD_SUCCESS) {
        return TDE_OPERATION_BUTT;
    }
    if ((double_src->bg_surface == TD_NULL) && (double_src->fg_surface == TD_NULL)) {
        tde_error("null pointer in single src or double src operation!");
        return TDE_OPERATION_BUTT;
    } else if ((double_src->bg_surface != TD_NULL) && (double_src->fg_surface != TD_NULL)) {
        return tde_osi_double_src_operation(double_src, opt);
    } else {
        return tde_osi_single_src_operation(double_src, opt);
    }
}

static tde_operation_category tde_osi_single_src_operation(const drv_tde_double_src *double_src, const drv_tde_opt *opt)
{
    drv_tde_surface *tmp_src2 = TD_NULL;
    drv_tde_rect *tmp_src2_rect = TD_NULL;

    if ((double_src->bg_surface != TD_NULL) && (double_src->fg_surface == TD_NULL)) {
        tmp_src2 = double_src->bg_surface;
        tmp_src2_rect = double_src->bg_rect;
    } else {
        tmp_src2 = double_src->fg_surface;
        tmp_src2_rect = double_src->fg_rect;
    }

    if (tmp_src2_rect == TD_NULL) {
        return TDE_OPERATION_BUTT;
    }

    if (tde_osi_check_surface(tmp_src2, tmp_src2_rect) != TD_SUCCESS) {
        return TDE_OPERATION_BUTT;
    }

    if ((opt == TD_NULL) || (!opt->resize)) {
        tde_unify_rect(tmp_src2_rect, double_src->dst_rect);
    }

    if ((opt == TD_NULL) && (tmp_src2->color_format == double_src->dst_surface->color_format)) {
        return TDE_OPERATION_SINGLE_SRC1;
    }
    if (opt == TD_NULL) {
        tde_error("source format must be the same with dst format!\n");
        return TDE_OPERATION_BUTT;
    }
    return TDE_OPERATION_SINGLE_SRC2;
}
static tde_operation_category tde_osi_double_src_operation(drv_tde_double_src *double_src, const drv_tde_opt *opt)
{
    if ((double_src->bg_rect == TD_NULL) || (double_src->fg_rect == TD_NULL) || (opt == TD_NULL)) {
        tde_error("Null pointer!\n");
        return TDE_OPERATION_BUTT;
    }
    if (double_src->bg_surface->color_format >= DRV_TDE_COLOR_FMT_JPG_YCBCR400MBP) {
        tde_error("This operation doesn't support Semi-plannar!\n");
        return TDE_OPERATION_BUTT;
    }
#ifdef TDE_BACKGROUND_COLORFOMATSUPPORT_YCBCR422
    if (double_src->bg_surface->color_format >= DRV_TDE_COLOR_FMT_YCBCR422) {
        tde_error("Background  doesn't support the colorfmt!\n");
        return TDE_OPERATION_BUTT;
    }
#endif

    if (tde_osi_check_surface(double_src->bg_surface, double_src->bg_rect) != TD_SUCCESS) {
        return TDE_OPERATION_BUTT;
    }
    if (tde_osi_check_surface(double_src->fg_surface, double_src->fg_rect) != TD_SUCCESS) {
        return TDE_OPERATION_BUTT;
    }
    if ((double_src->bg_rect->height != double_src->dst_rect->height) ||
        (double_src->bg_rect->width != double_src->dst_rect->width)) {
        tde_error("BackGroundRect is not the same with DstRect!\n");
        return TDE_OPERATION_BUTT;
    }
    if (!opt->resize) {
        if (double_src->bg_rect->height != double_src->fg_rect->height) {
            double_src->bg_rect->height = tde_min(double_src->bg_rect->height, double_src->fg_rect->height);
            double_src->dst_rect->height = double_src->bg_rect->height;
            double_src->fg_rect->height = double_src->bg_rect->height;
        }
        if (double_src->bg_rect->width != double_src->fg_rect->width) {
            double_src->bg_rect->width = tde_min(double_src->bg_rect->width, double_src->fg_rect->width);
            double_src->dst_rect->width = double_src->bg_rect->width;
            double_src->fg_rect->width = double_src->bg_rect->width;
        }
    }
    return TDE_OPERATION_DOUBLE_SRC;
}
/*
 * Function:      tde_osi_convert_surface
 * Description:   raster bitmap info by user upload translate to bitmap info which driver and hardware need
 * Input:         sur: raster bitmap info by user upload
 *                rect: raster bitmap operate zone by user upload
 *                scan_info: scanning direction info
 *                drv_sur: bitmap info which driver and hardware need
 * Output:        pstOperationArea: new operate zone fixed by scanning direction
 */
static td_void tde_osi_convert_surface(const drv_tde_surface *sur, const drv_tde_rect *rect,
                                       const tde_scandirection_mode *scan_info,
                                       tde_surface_msg *drv_sur)
{
    if (sur->color_format > DRV_TDE_COLOR_FMT_MAX) {
        tde_error("sur->color_format is invalid !\n");
        return;
    }
    drv_sur->color_format = g_tde_common_drv_color_fmt[sur->color_format];
    drv_sur->width = rect->width;
    drv_sur->height = rect->height;
    drv_sur->pitch = sur->stride;
    drv_sur->alpha_max_is_255 = sur->alpha_max_is_255;
    drv_sur->hor_scan = scan_info->hor_scan;
    drv_sur->ver_scan = scan_info->ver_scan;
    drv_sur->phys_addr = sur->phys_addr;
    drv_sur->cbcr_phys_addr = sur->cbcr_phys_addr;
    drv_sur->cb_cr_pitch = sur->cbcr_stride;
    drv_sur->rgb_order = g_tde_argb_order[sur->color_format];

    drv_sur->xpos = (td_u32)rect->pos_x;

    drv_sur->ypos = (td_u32)rect->pos_y;

    return;
}
static td_s32 tde_osi_check_clip_para(const drv_tde_double_src *double_src, const drv_tde_opt *opt,
                                      const tde_hw_node *hw_node)
{
    if (double_src->fg_surface == TD_NULL) {
        tde_error("double_src->fg_surface is null !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }
    if (double_src->dst_surface == TD_NULL) {
        tde_error("double_src->dst_surface is null !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }
    if (opt == TD_NULL) {
        tde_error("pstOpt is null !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }
    if (hw_node == TD_NULL) {
        tde_error("pstHwNode is null !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }
    if (opt->clip_mode >= DRV_TDE_CLIP_MODE_MAX) {
        tde_error("clip_mode error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    if (opt->clip_mode != DRV_TDE_CLIP_MODE_NONE) {
        if ((opt->clip_rect.height == 0) || (opt->clip_rect.width == 0) || (opt->clip_rect.pos_x < 0) ||
            ((td_u32)opt->clip_rect.pos_x > TDE_MAX_RECT_WIDTH) ||
            (opt->clip_rect.pos_y < 0) || ((td_u32)opt->clip_rect.pos_y > TDE_MAX_RECT_HEIGHT) ||
            ((td_u32)opt->clip_rect.width > TDE_MAX_RECT_WIDTH) ||
            ((td_u32)opt->clip_rect.height > TDE_MAX_RECT_HEIGHT)) {
            tde_error(" Clip rect x:%d, y:%d, w:%u, h:%u error!\n", opt->clip_rect.pos_x,
                opt->clip_rect.pos_y, opt->clip_rect.width, opt->clip_rect.height);
            return -1;
        }
    }
    return TD_SUCCESS;
}
static td_void tde_osi_double_src(drv_tde_double_src *double_src, const drv_tde_rect *inter_rect)
{
    if (double_src->bg_surface != TD_NULL) {
        double_src->bg_rect->pos_x += inter_rect->pos_x - double_src->dst_rect->pos_x;
        double_src->bg_rect->pos_y += inter_rect->pos_y - double_src->dst_rect->pos_y;
        double_src->bg_rect->height = inter_rect->height;
        double_src->bg_rect->width = inter_rect->width;
    }
    double_src->fg_rect->pos_x += inter_rect->pos_x - double_src->dst_rect->pos_x;
    double_src->fg_rect->pos_y += inter_rect->pos_y - double_src->dst_rect->pos_y;
    double_src->fg_rect->height = inter_rect->height;
    double_src->fg_rect->width = inter_rect->width;

    *(double_src->dst_rect) = *inter_rect;
}
static td_void tde_osi_get_clip_pos(tde_clip_cmd *clip, const drv_tde_opt *opt, const drv_tde_double_src *double_src,
    const drv_tde_rect *inter_rect)
{
    clip->clip_start_x = (opt->clip_rect.pos_x > double_src->dst_rect->pos_x) ?
        (opt->clip_rect.pos_x - double_src->dst_rect->pos_x) : 0;
    clip->clip_start_y = (opt->clip_rect.pos_y > double_src->dst_rect->pos_y) ?
        (opt->clip_rect.pos_y - double_src->dst_rect->pos_y) : 0;
    clip->clip_end_x = clip->clip_start_x + inter_rect->width - 1;
    clip->clip_end_y = clip->clip_start_y + inter_rect->height - 1;
    return;
}
/*
 * Function:      tde_osi_set_clip_para
 * Description:   set clip zone parameter
 * Input:         double_src:  bitmap info
 *                opt: operate option
 *                hw_node: hardware operate node
 */
static td_s32 tde_osi_set_clip_para(drv_tde_double_src *double_src, const drv_tde_opt *opt, tde_hw_node *hw_node)
{
    tde_clip_cmd clip = { 0 };
    drv_tde_rect inter_rect = { 0 };
    td_s32 ret;
    ret = tde_osi_check_clip_para(double_src, opt, hw_node);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    if ((opt->clip_mode == DRV_TDE_CLIP_MODE_INSIDE) && (!opt->resize)) {
        if (tde_osi_get_inter_rect(double_src->dst_rect, &opt->clip_rect, &inter_rect) != TD_SUCCESS) {
            tde_error("clip and operation area have no inrerrect!\n");
            return DRV_ERR_TDE_CLIP_AREA;
        }
        tde_osi_double_src(double_src, &inter_rect);
    } else if (opt->clip_mode == DRV_TDE_CLIP_MODE_INSIDE) {
        if (tde_osi_get_inter_rect(double_src->dst_rect, &opt->clip_rect, &inter_rect) != TD_SUCCESS) {
            tde_error("clip and operation area have no inter-rect!\n");
            return DRV_ERR_TDE_CLIP_AREA;
        }
        clip.inside_clip = TD_TRUE;
        tde_osi_get_clip_pos(&clip, opt, double_src, &inter_rect);
        if (tde_hal_node_set_clipping(hw_node, &clip) < 0) {
            return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
        }
    } else if (opt->clip_mode == DRV_TDE_CLIP_MODE_OUTSIDE) {
        if (tde_osi_is_rect1_in_rect2(double_src->dst_rect, &opt->clip_rect)) {
            tde_error("clip and operation area have no inter-rect!\n");
            return DRV_ERR_TDE_CLIP_AREA;
        }
        if (tde_osi_get_inter_rect(double_src->dst_rect, &opt->clip_rect, &inter_rect) != TD_SUCCESS) {
            return TD_SUCCESS;
        }
        clip.inside_clip = TD_FALSE;
        tde_osi_get_clip_pos(&clip, opt, double_src, &inter_rect);
        if (tde_hal_node_set_clipping(hw_node, &clip) != TD_SUCCESS) {
            return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
        }
    } else if (opt->clip_mode >= DRV_TDE_CLIP_MODE_MAX) {
        tde_error("error clip mode!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    return TD_SUCCESS;
}
static td_s32 tde_osi_check_sre_para(const drv_tde_double_src *double_src, const drv_tde_pattern_fill_opt *opt,
                                     const tde_hw_node *hw_node)
{
    if (double_src->fg_surface == TD_NULL) {
        tde_error("double_src->fg_surface is null !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }
    if (double_src->dst_surface == TD_NULL) {
        tde_error("double_src->dst_surface is null !");
        return DRV_ERR_TDE_NULL_PTR;
    }
    if (opt == TD_NULL) {
        tde_error("opt is null !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }
    if (hw_node == TD_NULL) {
        tde_error("hw_node is null !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }
    if (opt->clip_mode >= DRV_TDE_CLIP_MODE_MAX) {
        tde_error("clip_mode error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    if (opt->clip_mode != DRV_TDE_CLIP_MODE_NONE) {
        if ((opt->clip_rect.height == 0) || (opt->clip_rect.width == 0) || (opt->clip_rect.pos_x < 0) ||
            ((td_u32)opt->clip_rect.pos_x >= TDE_MAX_RECT_WIDTH) ||
            (opt->clip_rect.pos_y < 0) || ((td_u32)opt->clip_rect.pos_y >= TDE_MAX_RECT_HEIGHT) ||
            ((td_u32)opt->clip_rect.width >= TDE_MAX_RECT_WIDTH) ||
            ((td_u32)opt->clip_rect.height >= TDE_MAX_RECT_HEIGHT)) {
            tde_error(" Clip rect x:%d, y:%d, w:%u, h:%u error!\n", opt->clip_rect.pos_x,
                opt->clip_rect.pos_y, opt->clip_rect.width, opt->clip_rect.height);
            return -1;
        }
    }
    return TD_SUCCESS;
}
static td_s32 tde_osi_set_pattern_clip_para(const drv_tde_double_src *double_src, const drv_tde_pattern_fill_opt *opt,
                                            tde_hw_node *hw_node)
{
    tde_clip_cmd clip = {0};
    drv_tde_rect inter_rect = {0};
    td_s32 ret;

    ret = tde_osi_check_sre_para(double_src, opt, hw_node);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    if (opt->clip_mode == DRV_TDE_CLIP_MODE_INSIDE) {
        if (tde_osi_get_inter_rect(double_src->dst_rect, &opt->clip_rect, &inter_rect) != TD_SUCCESS) {
            tde_error("clip and operation area have no inrerrect!\n");
            return DRV_ERR_TDE_CLIP_AREA;
        }
        clip.inside_clip = TD_TRUE;
        clip.clip_start_x = (opt->clip_rect.pos_x > double_src->dst_rect->pos_x) ?
            (opt->clip_rect.pos_x - double_src->dst_rect->pos_x) : 0;
        clip.clip_start_y = (opt->clip_rect.pos_y > double_src->dst_rect->pos_y) ?
            (opt->clip_rect.pos_y - double_src->dst_rect->pos_y) : 0;
        clip.clip_end_x = clip.clip_start_x + inter_rect.width - 1;
        clip.clip_end_y = clip.clip_start_y + inter_rect.height - 1;
        if (tde_hal_node_set_clipping(hw_node, &clip) != TD_SUCCESS) {
            return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
        }
    } else if (opt->clip_mode == DRV_TDE_CLIP_MODE_OUTSIDE) {
        if (tde_osi_is_rect1_in_rect2(double_src->dst_rect, &opt->clip_rect)) {
            tde_error("clip and operation area have no inter-rect!\n");
            return DRV_ERR_TDE_CLIP_AREA;
        }
        if (tde_osi_get_inter_rect(double_src->dst_rect, &opt->clip_rect, &inter_rect) != TD_SUCCESS) {
            return TD_SUCCESS;
        }
        clip.inside_clip = TD_FALSE;
        clip.clip_start_x = (opt->clip_rect.pos_x > double_src->dst_rect->pos_x) ?
            (opt->clip_rect.pos_x - double_src->dst_rect->pos_x) : 0;
        clip.clip_start_y = (opt->clip_rect.pos_y > double_src->dst_rect->pos_y) ?
            (opt->clip_rect.pos_y - double_src->dst_rect->pos_y) : 0;
        clip.clip_end_x = clip.clip_start_x + inter_rect.width - 1;
        clip.clip_end_y = clip.clip_start_y + inter_rect.height - 1;

        if (tde_hal_node_set_clipping(hw_node, &clip) != TD_SUCCESS) {
            return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
        }
    } else if (opt->clip_mode >= DRV_TDE_CLIP_MODE_MAX) {
        tde_error("error clip mode!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    return TD_SUCCESS;
}
static td_void tde_osi_set_ext_alpha(const drv_tde_surface *back_ground, const drv_tde_surface *fore_ground,
                                     tde_hw_node *hw_node)
{
    tde_src_mode src = TDE_DRV_SRC_NONE;
    td_bool real;

    real = ((fore_ground != TD_NULL) && (fore_ground->color_format >= DRV_TDE_COLOR_FMT_ARGB1555) &&
        (fore_ground->color_format <= DRV_TDE_COLOR_FMT_BGRA1555));
    if (real) {
        tde_hal_node_set_src2_alpha(hw_node);
    }
    real = ((back_ground != TD_NULL) &&
        (back_ground->color_format >= DRV_TDE_COLOR_FMT_ARGB1555) &&
        (back_ground->color_format <= DRV_TDE_COLOR_FMT_BGRA1555));
    if (real) {
        tde_hal_node_set_src1_alpha(hw_node);
    }
    real = ((fore_ground != TD_NULL) && (fore_ground->support_alpha_ex_1555) &&
        (fore_ground->color_format >= DRV_TDE_COLOR_FMT_ARGB1555) &&
        (fore_ground->color_format <= DRV_TDE_COLOR_FMT_BGRA1555));
    if (real) {
        src = (td_u32)src | TDE_DRV_SRC_S2;
    }
    real = ((back_ground != TD_NULL) && (back_ground->support_alpha_ex_1555) &&
        (back_ground->color_format >= DRV_TDE_COLOR_FMT_ARGB1555) &&
        (back_ground->color_format <= DRV_TDE_COLOR_FMT_BGRA1555));
    if (real) {
        src = (td_u32)src | TDE_DRV_SRC_S1;
    }
    if ((td_u32)src & TDE_DRV_SRC_S1) {
        tde_hal_node_set_exp_alpha(hw_node, src, back_ground->alpha0, back_ground->alpha1);
    } else if ((td_u32)src & TDE_DRV_SRC_S2) {
        tde_hal_node_set_exp_alpha(hw_node, src, fore_ground->alpha0, fore_ground->alpha1);
    }
}
static td_s32 tde_osi_set_blend_opt_rop(const drv_tde_opt *opt, tde_hw_node *hw_node, tde_alu_mode *alu)
{
    if ((td_u32)opt->alpha_blending_cmd & DRV_TDE_ALPHA_BLENDING_BLEND) {
        *alu = TDE_ALU_BLEND;
        if (tde_hal_node_set_blend(hw_node, &opt->blend_opt) != TD_SUCCESS) {
            return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
        }
#if (TDE_CAPABILITY & ROP)
        if ((td_u32)opt->alpha_blending_cmd & DRV_TDE_ALPHA_BLENDING_ROP) {
            tde_hal_node_enable_alpha_rop(hw_node);
        }
#endif
    }
#if (TDE_CAPABILITY & COLORIZE)
    if ((td_u32)opt->alpha_blending_cmd & DRV_TDE_ALPHA_BLENDING_COLORIZE) {
        if (tde_hal_node_set_colorize(hw_node, opt->color_resize) != TD_SUCCESS) {
            return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
        }
    }
#endif
    return TD_SUCCESS;
}
/*
 * Function:      tde_osi_set_base_opt_para_for_blit
 * Description:   encapsulation function used to set operate type
 * Input:         opt: operate option
 *                opt_category: operate category
 *                hw_node: nareware operate node
 */
static td_s32 tde_osi_set_base_opt_para_for_blit(const drv_tde_opt *opt, const drv_tde_surface *src1,
                                                 const drv_tde_surface *src2, tde_operation_category opt_category,
                                                 tde_hw_node *hw_node)
{
    tde_base_opt_mode base_opt = { 0 };
    tde_alu_mode alu = TDE_ALU_NONE;
    td_s32 ret;

    if ((hw_node == TD_NULL) || (opt == TD_NULL)) {
        return TD_SUCCESS;
    }
    if ((src1 != TD_NULL) && (src2 != TD_NULL)) {
        if ((tde_osi_get_fmt_category(src2->color_format) == TDE_COLORFMT_CATEGORY_AN) &&
            ((tde_osi_get_fmt_category(src1->color_format) == TDE_COLORFMT_CATEGORY_ARGB) ||
            (tde_osi_get_fmt_category(src1->color_format) == TDE_COLORFMT_CATEGORY_YCBCR))) {
            alu = TDE_SRC1_BYPASS;
        }
    }
#if (TDE_CAPABILITY & ROP)
    if ((td_u32)opt->alpha_blending_cmd & DRV_TDE_ALPHA_BLENDING_ROP) {
        alu = TDE_ALU_ROP;

        if (tde_hal_node_set_rop(hw_node, opt->rop_color, opt->rop_alpha) != TD_SUCCESS) {
            return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
        }
    }
#endif
    ret = tde_osi_set_blend_opt_rop(opt, hw_node, &alu);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    base_opt = (opt_category == TDE_OPERATION_DOUBLE_SRC) ? TDE_NORM_BLIT_2OPT : TDE_NORM_BLIT_1OPT;

    tde_hal_node_set_global_alpha(hw_node, opt->global_alpha, opt->blend_opt.global_alpha_en);

    if (tde_hal_node_set_base_operate(hw_node, base_opt, alu, 0) != TD_SUCCESS) {
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    ret = tde_set_node_csc(hw_node, opt->csc_opt);

    return ret;
}
/*
 * Function:      tde_osi_adj_clip_para
 * Description:   set clip parameter to hardware node
 * Input:         hw_node: node need to set
 */
static td_void tde_osi_adj_clip_para(tde_hw_node *hw_node)
{
    td_u32 clip_start_x;
    td_u32 clip_start_y;
    td_u32 clip_end_x;
    td_u32 clip_end_y;
    td_u32 out_start_x;
    td_u32 out_start_y;
    td_u32 out_end_x;
    td_u32 out_end_y;
    td_u32 start_x;
    td_u32 start_y;
    td_u32 end_x;
    td_u32 end_y;

    if (!(hw_node->des_alpha.bits.des_crop_en)) {
        return;
    }

    clip_start_x = hw_node->des_crop_pos_st.bits.des_crop_start_x & 0xfff;
    clip_start_y = hw_node->des_crop_pos_st.bits.des_crop_start_y & 0xfff;
    clip_end_x = hw_node->des_crop_pos_ed.bits.des_crop_end_x & 0xfff;
    clip_end_y = hw_node->des_crop_pos_ed.bits.des_crop_end_y & 0xfff;

    out_start_x = 0;
    out_start_y = 0;
    out_end_x = out_start_x + ((hw_node->des_imgsize.bits.des_width + 1) & 0xfff) - 1;
    out_end_y = out_start_y + ((hw_node->des_imgsize.bits.des_height + 1) & 0xfff) - 1;

    start_x = (clip_start_x > out_start_x) ? clip_start_x : out_start_x;
    start_y = (clip_start_y > out_start_y) ? clip_start_y : out_start_y;
    end_x = (clip_end_x < out_end_x) ? clip_end_x : out_end_x;
    end_y = (clip_end_y < out_end_y) ? clip_end_y : out_end_y;

    if ((start_x > end_x) || (start_y > end_y)) {
        hw_node->des_alpha.bits.des_crop_en = 0;
    } else {
        hw_node->des_crop_pos_st.bits.des_crop_start_x = start_x;
        hw_node->des_crop_pos_st.bits.des_crop_start_y = start_y;
        hw_node->des_crop_pos_ed.bits.des_crop_end_x = end_x;
        hw_node->des_crop_pos_ed.bits.des_crop_end_y = end_y;
    }

    return;
}

static td_void tde_osi_none_cmd(tde_hw_node *hw_node, tde_sw_node *cmd, const tde_sw_job *job)
{
    tde_hw_node *hw_tail_node = TD_NULL;

#ifdef CONFIG_OT_PROC_SHOW_SUPPORT
    tde_proc_record_node(hw_node);
#endif

#ifndef __RTOS__
    cmd->node_buf.buf = (td_void *)hw_node - TDE_NODE_HEAD_BYTE;
#else
    cmd->node_buf.buf = (td_u8 *)hw_node - TDE_NODE_HEAD_BYTE;
#endif

    cmd->node_buf.node_sz = sizeof(tde_hw_node);
    cmd->node_buf.update = (0xffffffff) | ((td_u64)0x000003ff << 32); /* 32 alg data */
    cmd->node_buf.phy_addr = wgetphy(cmd->node_buf.buf) + TDE_NODE_HEAD_BYTE;
    /*
     * If the tail node of the job is not null,add the current job node to the tail of the job list of hardware.
     * That is to say the next node address of the tail\
     * node is the current node address.
     */
    if (job->tail_node != TD_NULL) {
        hw_tail_node = (tde_hw_node *)((td_u8 *)job->tail_node->node_buf.buf + TDE_NODE_HEAD_BYTE);
        hw_tail_node->tde_pnext_low.bits.p_next_low = get_low_addr(cmd->node_buf.phy_addr);
        hw_tail_node->tde_pnext_hi.bits.p_next_hi = get_high_addr(cmd->node_buf.phy_addr);
    }
    return;
}

/*
 * Function:      tde_osi_set_node_finish
 * Description:   complete node config, add node to list signed by handle
 * Input:         handle: task handle, which is sumbit list
 *                hw_node: set node
 *                work_buf_num: temporary buffer number
 *                subm_type: submit node type
 * Return:        return slice number
 * Others:        node struct is following:
 *                 ----------------------
 *                 |  software node pointer(4)   |
 *                 ----------------------
 *                 |  config parameter          |
 *                 ----------------------
 *                 |  physical address of next node(4) |
 *                 ----------------------
 *                 |  update flag of next node(4) |
 *                 ----------------------
 */
static td_s32 tde_osi_set_node_finish(td_s32 handle, tde_hw_node *hw_node,
                                      td_u32 work_buf_num, tde_node_subm_type subm_type)
{
    tde_handle_mgr *handle_mgr = TD_NULL;
    tde_sw_job *job = TD_NULL;
    tde_sw_node *cmd = TD_NULL;
    td_bool valid;

    tde_osi_adj_clip_para(hw_node);
    valid = tde_query_handle(handle, &handle_mgr);
    if (!valid) {
        tde_error("invalid handle %d!\n", handle);
        return DRV_ERR_TDE_INVALID_HANDLE;
    }
    job = (tde_sw_job *)handle_mgr->res;
    if (job->has_submitted) {
        tde_error("job %d already submitted!\n", handle);
        return DRV_ERR_TDE_INVALID_HANDLE;
    }
    cmd = (tde_sw_node *)tde_malloc(sizeof(tde_sw_node));
    if (cmd == TD_NULL) {
        tde_error("malloc failed!\n");
        return DRV_ERR_TDE_NO_MEM;
    }
    if (subm_type != TDE_NODE_SUBM_CHILD) {
        job->cmd_num++;
        if (job->cmd_num == 1) {
            job->first_cmd = cmd;

            OSAL_INIT_LIST_HEAD(&cmd->list_head);
        }
        job->last_cmd = cmd;
    }

    tde_osi_none_cmd(hw_node, cmd, job);
    cmd->handle = job->handle;
    cmd->index = job->cmd_num;
    cmd->submit_type = subm_type;
    cmd->phy_buf_num = work_buf_num;
    *(((td_u64 *)cmd->node_buf.buf) + 1) = handle;
    osal_list_add_tail(&cmd->list_head, &job->first_cmd->list_head); /* Add the cmd to the job list */
    job->tail_node = cmd;
    job->node_num++;
    if (cmd->phy_buf_num != 0) {
        job->aq_use_buf = TD_TRUE;
    }

    return TD_SUCCESS;
}

/*
 * Function:      tde_osi_begin_job
 * Description:   get TDE task handle
 * Return:        created task handle
 */
td_s32 tde_osi_begin_job(td_s32 *handle, td_void *private_data)
{
    return tde_osi_list_begin_job(handle, private_data);
}

td_s32 tde_osi_begin_job_ex(td_s32 *handle)
{
    return tde_osi_list_begin_job(handle, TD_NULL);
}

td_s32 tde_osi_end_job(drv_tde_end_job_cmd *end_job, drv_tde_func_callback func_compl_cb, td_void *func_para)
{
    tde_notify_mode noti_type;

    if (end_job == TD_NULL) {
        tde_error("null pointer!\n");
        return DRV_ERR_TDE_NULL_PTR;
    }
    if (((end_job->is_block != TD_TRUE) && (end_job->is_block != TD_FALSE)) ||
        ((end_job->is_sync != TD_TRUE) && (end_job->is_sync != TD_FALSE))) {
        tde_error("bool should be TRUE or FALSE!!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (end_job->is_block) {
        if (osal_in_interrupt()) {
            tde_error("can not be block in interrupt!\n");
            return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
        }
        noti_type = TDE_JOB_WAKE_NOTIFY;
    } else {
        noti_type = TDE_JOB_COMPL_NOTIFY;
    }
    return tde_osi_list_submit_job(end_job, func_compl_cb, func_para, noti_type, end_job->is_sync);
}

/*
 * Function:      tde_osi_cancel_job
 * Description:   delete created TDE task, only effective for call before endjob
 *                use to release software resource of list of task
 * Input:         handle: task handle
 * Return:        success/fail
 */
td_s32 tde_osi_cancel_job(td_s32 handle)
{
    return tde_osi_list_cancel_job(handle);
}

/*
 * Function:      tde_osi_wait_for_done
 * Description:   wait for completion of submit TDE operate
 * Input:         handle: task handle
 * Return:        success/fail
 */
td_s32 tde_osi_wait_for_done(td_s32 handle, td_u32 time_out)
{
    if (osal_in_interrupt()) {
        tde_error("can not be block in interrupt!\n");
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    return tde_osi_list_wait_for_done(handle, time_out);
}

/*
 * Function:      tde_osi_wait_all_done
 * Description:   wait for all TDE operate completion
 * Return:        success/fail
 */
td_s32 tde_osi_wait_all_done(td_bool is_sync)
{
    ot_unused(is_sync);
    if (osal_in_interrupt()) {
        tde_error("can not wait in interrupt!\n");
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    return tde_osi_list_wait_all_done();
}

/*
 * Function:      tde_osi_quick_copy
 * Description:   quick blit source to target, no any functional operate, the size of source and target are the same
 *                format is not MB format
 */
td_s32 tde_osi_quick_copy(td_s32 handle, const drv_tde_single_src *single_src)
{
    drv_tde_double_src double_src;

    if (single_src == TD_NULL) {
        tde_error("null pointer!\n");
        return DRV_ERR_TDE_NULL_PTR;
    }

    double_src.bg_surface  = TD_NULL;
    double_src.bg_rect  = TD_NULL;
    double_src.fg_surface = single_src->src_surface;
    double_src.fg_rect = single_src->src_rect;
    double_src.dst_surface = single_src->dst_surface;
    double_src.dst_rect = single_src->dst_rect;

    return tde_osi_blit(handle, &double_src, TD_NULL);
}

td_void tde_set_resize_filter(td_bool is_resize_filter)
{
    g_is_resize_filter = is_resize_filter;
}

#if (TDE_CAPABILITY & RESIZE)
/*
 * Function:      tde_osi_quick_resize
 * Description:   zoom the size of source bitmap to the size aasigned by target bitmap,
                  of which source and target can be the same
 * Input:         src_surface: source bitmap info struct
 *                dst_surface: target bitmap info struct
 *                pFuncComplCB: callback function pointer when operate is over;if null, to say to no need to notice
 * Others:        add support for YCbCr422
 */
td_s32 tde_osi_quick_resize(td_s32 handle, drv_tde_surface *src_surface, drv_tde_rect *src_rect,
                            drv_tde_surface *dst_surface, drv_tde_rect *dst_rect)
{
    drv_tde_opt option = { 0 };
    drv_tde_double_src double_src;
    option.resize = TD_TRUE;

    if (src_surface == TD_NULL) {
        tde_error("null pointer!\n");
        return DRV_ERR_TDE_NULL_PTR;
    }

    if (g_is_resize_filter) {
        option.filter_mode = DRV_TDE_FILTER_MODE_COLOR;
    } else {
        option.filter_mode = DRV_TDE_FILTER_MODE_NONE;
    }
    if (src_surface->color_format >= DRV_TDE_COLOR_FMT_JPG_YCBCR400MBP) {
        tde_error("This operation doesn't support Semi-plannar!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    double_src.bg_surface  = TD_NULL;
    double_src.bg_rect  = TD_NULL;
    double_src.fg_surface = src_surface;
    double_src.fg_rect = src_rect;
    double_src.dst_surface = dst_surface;
    double_src.dst_rect = dst_rect;
    return tde_osi_blit(handle, &double_src, &option);
}
#endif
#if (TDE_CAPABILITY & ROTATE)
static td_s32 tde_osi_rotate_check_surface(const tde_surface_msg *surface)
{
    td_u32 bpp;
    td_phys_addr_t phys_addr;
    td_u32 stride;

    bpp = (td_u32)tde_osi_get_bpp_by_fmt((drv_tde_color_fmt)surface->color_format);
    phys_addr = (td_phys_addr_t)(surface->phys_addr + (td_u64)(surface->ypos) * (td_u64)(surface->pitch) +
                  (((td_u64)(surface->xpos) * (td_u64)(bpp)) >> 3)); /* 3 bpp narrow 8 */
    stride = surface->pitch;

    if ((phys_addr % 16 != 0) || (stride % 16 != 0)) { /* 16 align */
        tde_error ("rotate operation:phys_addr and stride must 16 align!\n");
        return DRV_ERR_TDE_NOT_ALIGNED;
    }
    return TD_SUCCESS;
}

static td_s32 tde_osi_quick_rotate_set(tde_hw_node *hw_node, const drv_tde_single_src *single_src,
                                       drv_tde_rotate_angle rotate_angle)
{
    tde_scandirection_mode src_scan_info = {0};
    tde_scandirection_mode dst_scan_info = {0};
    tde_surface_msg src_drv_surface = {0};
    tde_surface_msg dst_drv_surface = {0};

    if (tde_hal_node_set_base_operate(hw_node, TDE_NORM_BLIT_1OPT, TDE_ALU_NONE, 0) < 0) {
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    src_scan_info.hor_scan = TDE_SCAN_LEFT_RIGHT;
    src_scan_info.ver_scan = TDE_SCAN_UP_DOWN;
    dst_scan_info.hor_scan = TDE_SCAN_LEFT_RIGHT;
    dst_scan_info.ver_scan = TDE_SCAN_UP_DOWN;
    if (rotate_angle == DRV_TDE_ROTATE_CLOCKWISE_180) {
        src_scan_info.hor_scan = TDE_SCAN_RIGHT_LEFT;
        src_scan_info.ver_scan = TDE_SCAN_DOWN_UP;
    }

    tde_osi_convert_surface(single_src->src_surface, single_src->src_rect, &src_scan_info, &src_drv_surface);
    /* phys_addr and stride must 16Bytes align */
    if (tde_osi_rotate_check_surface(&src_drv_surface) != TD_SUCCESS) {
        return DRV_ERR_TDE_NOT_ALIGNED;
    }
    tde_hal_node_set_src2(hw_node, &src_drv_surface);

    tde_osi_convert_surface(single_src->dst_surface, single_src->dst_rect, &dst_scan_info, &dst_drv_surface);

    /* phys_addr and stride must 16Bytes align */
    if (tde_osi_rotate_check_surface(&dst_drv_surface) != TD_SUCCESS) {
        return DRV_ERR_TDE_NOT_ALIGNED;
    }
    tde_hal_node_set_tqt(hw_node, &dst_drv_surface, DRV_TDE_OUT_ALPHA_FROM_NORM);

    if (rotate_angle != DRV_TDE_ROTATE_CLOCKWISE_180) {
        tde_hal_node_set_rotate(hw_node, rotate_angle);
    }
    return TD_SUCCESS;
}

td_s32 tde_osi_quick_rotate(td_s32 handle, drv_tde_single_src *single_src, drv_tde_rotate_angle rotate_angle)
{
    td_s32 ret;
    tde_hw_node *hw_node = TD_NULL;
    drv_tde_double_src double_src = {0};
    td_bool is_null = (single_src == TD_NULL) || (single_src->src_surface == TD_NULL) ||
        (single_src->src_rect == TD_NULL) || (single_src->dst_surface == TD_NULL) ||
        (single_src->dst_rect == TD_NULL);

    if (is_null == TD_TRUE) {
        return DRV_ERR_TDE_NULL_PTR;
    }
    if ((single_src->src_surface->color_format >= DRV_TDE_COLOR_FMT_JPG_YCBCR400MBP) ||
        (tde_osi_check_surface(single_src->src_surface, single_src->src_rect) != TD_SUCCESS) ||
        (tde_osi_check_surface(single_src->dst_surface, single_src->dst_rect) != TD_SUCCESS) ||
        (tde_osi_check_rotate_para(single_src, rotate_angle) != TD_SUCCESS)) {
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (tde_hal_node_init_nd(&hw_node) < 0) {
        return DRV_ERR_TDE_NO_MEM;
    }

    /*
     * set:
     * set opt
     * set src2
     * set tqt
     * set rotate
     */
    ret = tde_osi_quick_rotate_set(hw_node, single_src, rotate_angle);
    if (ret != TD_SUCCESS) {
        tde_hal_free_node_buf(hw_node);
        return ret;
    }
    /* set filter node */
    if ((rotate_angle == DRV_TDE_ROTATE_CLOCKWISE_180) && (single_src->src_surface->color_format >=
        DRV_TDE_COLOR_FMT_YCBCR422)) {
        double_src.bg_surface = TD_NULL;
        double_src.bg_rect = TD_NULL;
        double_src.fg_surface = single_src->src_surface;
        double_src.fg_rect = single_src->src_rect;
        double_src.dst_surface = single_src->dst_surface;
        double_src.dst_rect = single_src->dst_rect;
        ret = tde_osi_set_filter_node(handle, hw_node, &double_src, DRV_TDE_DEFLICKER_LEVEL_MODE_NONE,
                                      (drv_tde_deflicker_mode)DRV_TDE_FILTER_MODE_NONE);
        if (ret < 0) {
            tde_hal_free_node_buf(hw_node);
            return ret;
        }
#if (TDE_CAPABILITY & SLICE)
        tde_hal_free_node_buf(hw_node);
#endif
        return TD_SUCCESS;
    }

    ret = tde_osi_set_node_finish(handle, hw_node, 0, TDE_NODE_SUBM_ALONE);
    if (ret < 0) {
        tde_hal_free_node_buf(hw_node);
        return ret;
    }

    return TD_SUCCESS;
}
#endif

#if (TDE_CAPABILITY & DEFLICKER)
/*
 * Function:      tde_osi_quick_flicker
 * Description:   deflicker source bitmap,output to target bitmap,source and target can be the same
 * Input:         src_surface: source bitmap info struct
 *                dst_surface: terget bitmap info struct
 *                pFuncComplCB: callback function pointer when operate is over;if null, to say to no need to notice
 */
td_s32 tde_osi_quick_flicker(td_s32 handle, drv_tde_surface *src_surface, drv_tde_rect *src_rect,
                             drv_tde_surface *dst_surface, drv_tde_rect *dst_rect)
{
    td_s32 ret;
    drv_tde_opt option = { 0 };
    drv_tde_double_src double_src;

    if (src_surface == TD_NULL) {
        tde_error("null pointer!\n");
        return DRV_ERR_TDE_NULL_PTR;
    }

    option.deflicker_mode = DRV_TDE_DEFLICKER_LEVEL_MODE_BOTH;
    option.resize = TD_TRUE;
    if (src_surface->color_format >= DRV_TDE_COLOR_FMT_JPG_YCBCR400MBP) {
        tde_error("This operation doesn't support Semi-plannar!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    double_src.bg_surface  = TD_NULL;
    double_src.bg_rect  = TD_NULL;
    double_src.fg_surface = src_surface;
    double_src.fg_rect = src_rect;
    double_src.dst_surface = dst_surface;
    double_src.dst_rect = dst_rect;
    ret = tde_osi_blit(handle, &double_src, &option);
    if (ret < 0) {
        return ret;
    }
    return TD_SUCCESS;
}
#endif

/*
 * Function:      tde_osi_blit
 * Description:   operate pstBackGround with pstForeGround,which result output to dst_surface,
                  operate setting is in pstOpt
 * Input:         opt:  operate parameter setting struct
 */
td_s32 tde_osi_blit(td_s32 handle, drv_tde_double_src *double_src, const drv_tde_opt *opt)
{
    tde_operation_category opt_category;
    td_s32 ret;
    drv_tde_single_src single_src;

    if (double_src == TD_NULL) {
        return DRV_ERR_TDE_NULL_PTR;
    }

    opt_category = tde_osi_get_opt_category(double_src, opt);
    switch (opt_category) {
        case TDE_OPERATION_SINGLE_SRC1:
            if (double_src->bg_surface == TD_NULL) {
                single_src.src_surface = double_src->fg_surface;
                single_src.src_rect = double_src->fg_rect;
            } else {
                single_src.src_surface = double_src->bg_surface;
                single_src.src_rect = double_src->bg_rect;
            }
            single_src.dst_surface = double_src->dst_surface;
            single_src.dst_rect = double_src->dst_rect;
            return tde_osi_single_src_1_blit(handle, &single_src, TD_FALSE, TD_FALSE);
            break;
        case TDE_OPERATION_SINGLE_SRC2:
            if (double_src->bg_surface == TD_NULL) {
                single_src.src_surface = double_src->fg_surface;
                single_src.src_rect = double_src->fg_rect;
            } else {
                single_src.src_surface = double_src->bg_surface;
                single_src.src_rect = double_src->bg_rect;
            }
            single_src.dst_surface = double_src->dst_surface;
            single_src.dst_rect = double_src->dst_rect;
            ret = tde_osi_single_src_2_blit(handle, &single_src, opt, TD_FALSE, TD_FALSE);
            if (ret < 0) {
                return ret;
            }
            break;
        case TDE_OPERATION_DOUBLE_SRC:
            ret = tde_osi_double_src_2_blit(handle, double_src, opt);
            if (ret < 0) {
                return ret;
            }
            break;
        default:
            return DRV_ERR_TDE_INVALID_PARA;
    }
    return TD_SUCCESS;
}

/*
 * Function:      tde_osi_quick_fill
 * Description:   quick fill fixed value to target bitmap, fill value is referred to target bitmap
 * Input:         dst_surface: target bitmap info struct
 *                fill_data: fill value
 *                pFuncComplCB: callback function pointer when operate is over;if null, to say to no need to notice
 */
td_s32 tde_osi_quick_fill(td_s32 handle, drv_tde_surface *dst_surface, drv_tde_rect *dst_rect,
                          td_u32 fill_data)
{
    drv_tde_fill_color fill_color;

    if (dst_surface == TD_NULL) {
        tde_error("null pointer!\n");
        return DRV_ERR_TDE_NULL_PTR;
    }

    fill_color.color_format = dst_surface->color_format;
    fill_color.color_value = fill_data;

    return tde_osi_1_source_fill(handle, dst_surface, dst_rect, &fill_color, TD_NULL);
}

/*
 * Function:      tde_osi_quick_draw
 * Description:   quick fill fixed value to target bitmap, fill value is referred to target bitmap
 * Input:         dst_surface: target bitmap info struct
 *                fill_data: fill value
 *                func_complete_callback: callback function pointer when operate is over;if null,
 *                to say to no need to notice
 * Output:        none
 * Return:        none
 * Others:        none
 */
td_s32 tde_osi_quick_draw(td_s32 handle, const drv_tde_surface *dst_surface, const drv_tde_rect *dst_rect,
                          const drv_tde_corner_rect_info *corner_rect)
{
    return tde_osi_des_dma_draw_rect(handle, dst_surface, dst_rect, corner_rect, TD_NULL);
}

td_s32 tde_osi_multi_draw(td_s32 handle, const drv_tde_surface *dst_surface, const drv_tde_rect *corner_rect_region,
    const drv_tde_corner_rect_info *corner_rect_list, td_u32 num)
{
    td_u32 i;
    td_s32 ret;
    drv_tde_rect dst_rect = {0};
    drv_tde_corner_rect_info corner_rect_info = {0};

    for (i = 0; i < num; i++) {
        dst_rect.pos_x = corner_rect_region[i].pos_x;
        dst_rect.pos_y = corner_rect_region[i].pos_y;
        dst_rect.width = corner_rect_region[i].width;
        dst_rect.height = corner_rect_region[i].height;
        corner_rect_info.width = corner_rect_list[i].width;
        corner_rect_info.height = corner_rect_list[i].height;
        corner_rect_info.inner_color = corner_rect_list[i].inner_color;
        corner_rect_info.outer_color = corner_rect_list[i].outer_color;
        ret = tde_osi_des_dma_draw_rect(handle, dst_surface, &dst_rect, &corner_rect_info, TD_NULL);
        if (ret != TD_SUCCESS) {
            return ret;
        }
    }
    return ret;
}

static td_s32 tde_osi_draw_line_check_format(const drv_tde_surface *dst_surface)
{
    if (dst_surface->color_format >= DRV_TDE_COLOR_FMT_RGB444 &&
        dst_surface->color_format <= DRV_TDE_COLOR_FMT_RABG8888) {
        return TD_SUCCESS;
    }
    if (dst_surface->color_format >= DRV_TDE_COLOR_FMT_CLUT4 &&
        dst_surface->color_format <= DRV_TDE_COLOR_FMT_ACLUT88) {
        return TD_SUCCESS;
    }
    if (dst_surface->color_format >= DRV_TDE_COLOR_FMT_A8 &&
        dst_surface->color_format <= DRV_TDE_COLOR_FMT_AYCBCR8888) {
        return TD_SUCCESS;
    }
    tde_error("unsupported color format %d\n", dst_surface->color_format);
    return DRV_ERR_TDE_INVALID_PARA;
}

static td_s32 tde_osi_draw_line_check(const drv_tde_line *line)
{
    if (line == TD_NULL) {
        tde_error("The draw_line pointer is null!\n");
        return DRV_ERR_TDE_NULL_PTR;
    }
    /* logic limit */
    if ((line->thick == 0) || (line->thick > DRD_MAX_LINE_WIDTH)) {
        tde_error("invalid line width:%d.\n", line->thick);
        return DRV_ERR_TDE_INVALID_PARA;
    }
    if (line->start_x >= DRD_LEFT_LIMIT && line->start_x <= DRD_RIGHT_LIMIT &&
        line->start_y >= DRD_LEFT_LIMIT && line->start_y <= DRD_RIGHT_LIMIT &&
        line->end_x >= DRD_LEFT_LIMIT && line->end_x <= DRD_RIGHT_LIMIT &&
        line->end_y >= DRD_LEFT_LIMIT && line->end_y <= DRD_RIGHT_LIMIT) {
        return TD_SUCCESS;
    }
    tde_error("invalid (start_x,start_y):(%d,%d);(end_x,end_y):(%d,%d)\n", line->start_x, line->start_y,
        line->end_x, line->end_y);
    return DRV_ERR_TDE_INVALID_PARA;
}

td_s32 tde_osi_draw_line(td_s32 handle, const drv_tde_surface *dst_surface, const drv_tde_line *line, td_u32 num)
{
    td_u32 task_num = num / TDE_MAX_LINE_NUM;
    td_u32 task_num_tail = num % TDE_MAX_LINE_NUM;
    const drv_tde_line *tmp_line = line;
    td_u32 i;
    td_u32 j;
    td_s32 ret;

    ret = tde_osi_draw_line_check_format(dst_surface);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    for (i = 0; i < task_num; i++) {
        for (j = 0; j < TDE_MAX_LINE_NUM; j++) {
            tmp_line = line + TDE_MAX_LINE_NUM * i + j;
            ret = tde_osi_draw_line_check(tmp_line);
            if (ret != TD_SUCCESS) {
                return ret;
            }
        }
        /* one task 4 lines */
        tmp_line = line + TDE_MAX_LINE_NUM * i;
        ret = tde_osi_des_dma_draw_line(handle, dst_surface, tmp_line, TDE_MAX_LINE_NUM);
        if (ret != TD_SUCCESS) {
            return ret;
        }
    }

    for (j = 0; j < task_num_tail; j++) {
        tmp_line = line + TDE_MAX_LINE_NUM * i + j;
        ret = tde_osi_draw_line_check(tmp_line);
        if (ret != TD_SUCCESS) {
            return ret;
        }
    }
    /* the last task be left(1~3) lines */
    tmp_line = line + TDE_MAX_LINE_NUM * i;
    ret = tde_osi_des_dma_draw_line(handle, dst_surface, tmp_line, task_num_tail);
    return ret;
}

/*
 * Function:      tde_osi_single_src_1_blit
 * Description:   source1 operate realization
 * Input:         dst_surface: target bitmap info struct
 *                dst_rect: target bitmap operate zone
 * Return:        success/fail
 */
static td_s32 tde_osi_single_src_1_blit(td_s32 handle, const drv_tde_single_src *single_src,
                                        td_bool mmz_for_src, td_bool mmz_for_dst)
{
    td_s32 ret;
    tde_hw_node *hw_node = TD_NULL;
    tde_surface_msg src_drv_surface = { 0 };
    tde_surface_msg dst_drv_surface = { 0 };
    tde_scandirection_mode src_scan_info = { 0 };
    tde_scandirection_mode dst_scan_info = { 0 };

    if ((single_src->src_surface == TD_NULL) || (single_src->src_rect == TD_NULL) ||
        (single_src->dst_surface == TD_NULL) || (single_src->dst_rect == TD_NULL)) {
        return DRV_ERR_TDE_NULL_PTR;
    }

    if ((single_src->src_surface->color_format == DRV_TDE_COLOR_FMT_YCBCR422) ||
        (single_src->src_surface->color_format == DRV_TDE_COLOR_FMT_PKGVYUY)) {
        tde_error("This operation doesn't support PKG!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (single_src->src_surface->color_format >= DRV_TDE_COLOR_FMT_JPG_YCBCR400MBP) {
        tde_error("This operation doesn't support Semi-plannar!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    tde_unify_rect(single_src->src_rect, single_src->dst_rect);

    if (tde_hal_node_init_nd(&hw_node) < 0) {
        return DRV_ERR_TDE_NO_MEM;
    }
    if (tde_hal_node_set_base_operate(hw_node, TDE_QUIKE_COPY, TDE_SRC1_BYPASS, 0) < 0) {
        tde_hal_free_node_buf(hw_node);
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    if (tde_osi_get_scan_info_ex(single_src, TD_NULL, &src_scan_info, &dst_scan_info) < 0) {
        tde_hal_free_node_buf(hw_node);
        return DRV_ERR_TDE_INVALID_PARA;
    }

    tde_osi_convert_surface(single_src->src_surface, single_src->src_rect, &src_scan_info, &src_drv_surface);
    src_drv_surface.cma = mmz_for_src;

    tde_hal_node_set_src1(hw_node, &src_drv_surface);

    tde_osi_set_ext_alpha(single_src->src_surface, TD_NULL, hw_node);

    tde_osi_convert_surface(single_src->dst_surface, single_src->dst_rect, &dst_scan_info, &dst_drv_surface);
    dst_drv_surface.cma = mmz_for_dst;
    tde_hal_node_set_tqt(hw_node, &dst_drv_surface, DRV_TDE_OUT_ALPHA_FROM_NORM);
    ret = tde_osi_set_node_finish(handle, hw_node, 0, TDE_NODE_SUBM_ALONE);
    if (ret < 0) {
        tde_hal_free_node_buf(hw_node);
        return ret;
    }
    return TD_SUCCESS;
}

static td_s32 tde_osi_single_para(const drv_tde_single_src *single_src, const drv_tde_opt *opt, tde_hw_node **hw_node,
    tde_scandirection_mode *src_scan_info, tde_scandirection_mode *dst_scan_info)
{
    td_s32 ret;
    drv_tde_double_src double_src;

#if (TDE_CAPABILITY & COMPRESS)
    td_bool compress_or_decompress;
#endif
    ret = tde_osi_check_single_src_to_para(single_src->src_surface, single_src->src_rect,
                                           single_src->dst_surface, single_src->dst_rect, opt);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    if (tde_hal_node_init_nd(hw_node) != TD_SUCCESS) {
        return DRV_ERR_TDE_NO_MEM;
    }

    double_src.bg_surface = TD_NULL;
    double_src.bg_rect = TD_NULL;
    double_src.fg_surface = single_src->src_surface;
    double_src.fg_rect = single_src->src_rect;
    double_src.dst_surface = single_src->dst_surface;
    double_src.dst_rect = single_src->dst_rect;
    if (tde_osi_set_clip_para(&double_src, opt, *hw_node) != TD_SUCCESS) {
        tde_hal_free_node_buf(*hw_node);
        return DRV_ERR_TDE_CLIP_AREA;
    }
    if (tde_osi_get_scan_info_ex(single_src, opt, src_scan_info, dst_scan_info) != TD_SUCCESS) {
        tde_hal_free_node_buf(*hw_node);
        return DRV_ERR_TDE_INVALID_PARA;
    }
#if (TDE_CAPABILITY & COMPRESS)
    compress_or_decompress = ((opt->is_compress) || (opt->is_decompress));
    if (compress_or_decompress) {
        src_scan_info->hor_scan = TDE_SCAN_LEFT_RIGHT;
        src_scan_info->ver_scan = TDE_SCAN_UP_DOWN;
        dst_scan_info->hor_scan = TDE_SCAN_LEFT_RIGHT;
        dst_scan_info->ver_scan = TDE_SCAN_UP_DOWN;
    }
#endif
    return TD_SUCCESS;
}

static td_s32 tde_osi_single_opt(const drv_tde_opt *opt, const drv_tde_single_src *single_src,
    tde_surface_msg *drv_surface, tde_hw_node *hw_node)
{
    td_u16 code;
    tde_conv_mode_cmd conv = { 0 };
    td_s32 ret;
    tde_clut_usage clut_usage = TDE_CLUT_USAGE_BUTT;

#if (TDE_CAPABILITY & COMPRESS)
    if (opt->is_compress) {
        tde_hal_node_set_compress_tqt(hw_node, drv_surface, opt->out_alpha_from);
        tde_hal_node_set_compress(hw_node);
    } else {
        tde_hal_node_set_tqt(hw_node, drv_surface, opt->out_alpha_from);
    }
#else
    tde_hal_node_set_tqt(hw_node, drv_surface, opt->out_alpha_from);
#endif
    code = tde_osi_single_src2_get_opt_code(single_src->src_surface->color_format,
                                            single_src->dst_surface->color_format);

    tde_osi_get_conv_by_code(code, &conv);

    if (tde_hal_node_set_color_convert(hw_node, &conv) != TD_SUCCESS) {
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    ret = tde_osi_set_clut_opt(single_src->src_surface, single_src->dst_surface, &clut_usage,
                               opt->clut_reload, hw_node);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = tde_osi_set_base_opt_para_for_blit(opt, TD_NULL, single_src->src_surface, TDE_OPERATION_SINGLE_SRC2, hw_node);
    if (ret != TD_SUCCESS) {
        tde_error("Set base opt para for blit failed, ret = 0x%x\n", ret);
    }
    tde_osi_set_ext_alpha(TD_NULL, single_src->src_surface, hw_node);
    return TD_SUCCESS;
}

static td_s32 tde_osi_single_node(td_s32 handle, const drv_tde_opt *opt, const drv_tde_single_src *single_src,
    tde_surface_msg *drv_surface, tde_hw_node *hw_node)
{
    td_bool set_filter_node;
    drv_tde_double_src double_src;
    td_s32 ret;

    set_filter_node = ((opt->resize) || (opt->deflicker_mode != DRV_TDE_DEFLICKER_LEVEL_MODE_NONE) ||
                       (single_src->src_surface->color_format >= DRV_TDE_COLOR_FMT_YCBCR422));
    if (set_filter_node) {
#if (TDE_CAPABILITY & SLICE)
        if ((single_src->src_surface->color_format >= DRV_TDE_COLOR_FMT_YCBCR422) && (!(opt->resize))) {
            single_src->src_rect->height = single_src->dst_rect->height;
            single_src->src_rect->width = single_src->dst_rect->width;
        }
#endif
        double_src.bg_surface = TD_NULL;
        double_src.bg_rect = TD_NULL;
        double_src.fg_surface = single_src->src_surface;
        double_src.fg_rect = single_src->src_rect;
        double_src.dst_surface = single_src->dst_surface;
        double_src.dst_rect = single_src->dst_rect;
        ret = tde_osi_set_filter_node(handle, hw_node, &double_src, opt->deflicker_mode,
            (drv_tde_deflicker_mode)opt->filter_mode);
        if (ret != TD_SUCCESS) {
            return ret;
        }
#if (TDE_CAPABILITY & SLICE)
        tde_hal_free_node_buf(hw_node);
#endif
        tde_free((td_void *)drv_surface);
        return TD_SUCCESS;
    }
    ret = tde_osi_set_node_finish(handle, hw_node, 0, TDE_NODE_SUBM_ALONE);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    tde_free((td_void *)drv_surface);
    return TD_SUCCESS;
}

/*
 * Function:      tde_osi_single_src_2_blit
 * Description:   source2 operate realization
 * Input:         dst_surface: target bitmap info struct
 *                dst_rect: target bitmap operate zone
 * Return:        success/fail
 */
static td_s32 tde_osi_single_src_2_blit(td_s32 handle, const drv_tde_single_src *single_src,
                                        const drv_tde_opt *opt, td_bool mmz_for_src, td_bool mmz_for_dst)
{
    tde_hw_node *hw_node = TD_NULL;
    tde_surface_msg *drv_surface = TD_NULL;
    td_s32 ret;
    tde_scandirection_mode src_scan_info = { 0 };
    tde_scandirection_mode dst_scan_info = { 0 };

    ret = tde_osi_single_para(single_src, opt, &hw_node, &src_scan_info, &dst_scan_info);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    drv_surface = (tde_surface_msg *)tde_malloc(sizeof(tde_surface_msg));
    if (drv_surface == TD_NULL) {
        tde_error ("malloc pstDrvSurface failed, size=%ld!\n", (unsigned long)(sizeof(tde_surface_msg)));
        tde_hal_free_node_buf(hw_node);
        return DRV_ERR_TDE_NO_MEM;
    }
    tde_osi_convert_surface(single_src->src_surface, single_src->src_rect, &src_scan_info, drv_surface);
    drv_surface->cma = mmz_for_src;

#if (TDE_CAPABILITY & COMPRESS)
    (opt->is_decompress) ? tde_hal_node_set_src_to_decompress(hw_node, drv_surface)
                         : tde_hal_node_set_src2(hw_node, drv_surface);
#else
    tde_hal_node_set_src2(hw_node, drv_surface);
#endif

    tde_osi_convert_surface(single_src->dst_surface, single_src->dst_rect, &dst_scan_info, drv_surface);
    if (opt->out_alpha_from >= DRV_TDE_OUT_ALPHA_FROM_MAX || opt->out_alpha_from == DRV_TDE_OUT_ALPHA_FROM_BACKGROUND) {
        tde_error("out_alpha_from error!\n");
        ret = DRV_ERR_TDE_INVALID_PARA;
        goto err;
    }

    drv_surface->cma = mmz_for_dst;
    ret = tde_osi_single_opt(opt, single_src, drv_surface, hw_node);
    if (ret != TD_SUCCESS) {
        goto err;
    }

    ret = tde_osi_single_node(handle, opt, single_src, drv_surface, hw_node);
    if (ret != TD_SUCCESS) {
        goto err;
    }

    return TD_SUCCESS;
err:
    tde_hal_free_node_buf(hw_node);
    tde_free((td_void *)drv_surface);
    return ret;
}

static td_s32 tde_osi_check_single_src_to_para(const drv_tde_surface *fore_ground, const drv_tde_rect *fore_ground_rect,
                                               const drv_tde_surface *dst_surface, const drv_tde_rect *dst_rect,
                                               const drv_tde_opt *opt)
{
    td_bool null_ptr;

    if (opt == TD_NULL) {
        tde_error("pstOpt is null !\n");
        return DRV_ERR_TDE_NULL_PTR;
    }

    null_ptr = ((dst_surface == TD_NULL) || (dst_rect == TD_NULL) || ((opt == TD_NULL)) ||
                (fore_ground == TD_NULL) || (fore_ground_rect == TD_NULL));

    if (null_ptr) {
        return DRV_ERR_TDE_NULL_PTR;
    }

    if (tde_osi_check_single_src_to_opt(fore_ground->color_format, dst_surface->color_format, opt) != TD_SUCCESS) {
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (opt->resize) {
        if (tde_osi_check_resize_para(fore_ground_rect->width, fore_ground_rect->height,
            dst_rect->width, dst_rect->height) != TD_SUCCESS) {
            return DRV_ERR_TDE_MINIFICATION;
        }
    }
#if TDE_CAPABILITY & COMPRESS
    if (opt->is_compress) {
        if (tde_osi_check_compress_para(fore_ground, fore_ground_rect, dst_surface, dst_rect, opt) !=
            TD_SUCCESS) {
            return DRV_ERR_TDE_INVALID_PARA;
        }
    }

    if (opt->is_decompress) {
        if (tde_osi_check_decompress_para(fore_ground, fore_ground_rect, dst_surface, dst_rect, opt) !=
            TD_SUCCESS) {
            return DRV_ERR_TDE_INVALID_PARA;
        }
    }
#endif

    return TD_SUCCESS;
}

static td_s32 tde_osi_double_para(drv_tde_double_src *double_src, const drv_tde_opt *opt, tde_hw_node **hw_node)
{
    td_bool null_ptr = ((double_src->bg_surface == TD_NULL) || ((double_src->bg_rect == TD_NULL)) ||
                        (double_src->fg_surface == TD_NULL) || ((double_src->fg_rect == TD_NULL)) ||
                        (double_src->dst_surface == TD_NULL) || ((double_src->dst_rect == TD_NULL)) ||
                        (opt == TD_NULL));

    if (null_ptr) {
        return DRV_ERR_TDE_NULL_PTR;
    }

    if (tde_osi_check_double_src_opt(double_src->bg_surface->color_format, double_src->fg_surface->color_format,
        double_src->dst_surface->color_format, opt) != TD_SUCCESS) {
        return DRV_ERR_TDE_INVALID_PARA;
    }
    if (opt->resize) {
        if (tde_osi_check_resize_para(double_src->fg_rect->width, double_src->fg_rect->height,
            double_src->dst_rect->width, double_src->dst_rect->height) != TD_SUCCESS) {
            return DRV_ERR_TDE_MINIFICATION;
        }
    }

#if (TDE_CAPABILITY & COMPRESS)
    if (opt->is_compress) {
        if (tde_osi_check_compress_para(double_src->fg_surface, double_src->fg_rect,
            double_src->dst_surface, double_src->dst_rect, opt) < 0) {
            return DRV_ERR_TDE_INVALID_PARA;
        }
    }

    if (opt->is_decompress) {
        if (tde_osi_check_decompress_para(double_src->fg_surface, double_src->fg_rect,
            double_src->dst_surface, double_src->dst_rect, opt) < 0) {
            return DRV_ERR_TDE_INVALID_PARA;
        }
    }
#endif

    if (tde_hal_node_init_nd(hw_node) != TD_SUCCESS) {
        return DRV_ERR_TDE_NO_MEM;
    }

    if (tde_osi_set_clip_para(double_src, opt, *hw_node) != TD_SUCCESS) {
        tde_hal_free_node_buf(*hw_node);
        return DRV_ERR_TDE_CLIP_AREA;
    }
    return TD_SUCCESS;
}

static td_s32 tde_osi_double_opt(const drv_tde_double_src *double_src, const drv_tde_opt *opt, tde_hw_node *hw_node,
    tde_surface_msg *drv_surface)
{
    td_s32 ret;
    td_u16 code;
    tde_conv_mode_cmd conv = { 0 };
    tde_clut_usage clut_usage = TDE_CLUT_USAGE_BUTT;

    if (opt->out_alpha_from >= DRV_TDE_OUT_ALPHA_FROM_MAX) {
        tde_error("out_alpha_from error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

#if (TDE_CAPABILITY & COMPRESS)
    if (opt->is_compress) {
        tde_hal_node_set_compress_tqt(hw_node, drv_surface, opt->out_alpha_from);
        tde_hal_node_set_compress(hw_node);
    } else {
        tde_hal_node_set_tqt(hw_node, drv_surface, opt->out_alpha_from);
    }
#else
    tde_hal_node_set_tqt(hw_node, drv_surface, opt->out_alpha_from);
#endif

    code = tde_osi_double_src_get_opt_code(double_src->bg_surface->color_format,
        double_src->fg_surface->color_format, double_src->dst_surface->color_format);

    tde_osi_get_conv_by_code(code, &conv);

    if (tde_hal_node_set_color_convert(hw_node, &conv) != TD_SUCCESS) {
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    ret = tde_osi_set_clut_opt(double_src->fg_surface, double_src->dst_surface,
                               &clut_usage, opt->clut_reload, hw_node);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = tde_osi_set_color_key(double_src, hw_node,
        opt->colorkey_value, opt->colorkey_mode, clut_usage);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = tde_osi_set_base_opt_para_for_blit(opt, double_src->bg_surface, double_src->fg_surface,
        TDE_OPERATION_DOUBLE_SRC, hw_node);
    if (ret != TD_SUCCESS) {
        tde_error("Set base opt para for blit failed, ret = 0x%x\n", ret);
    }
    tde_osi_set_ext_alpha(double_src->bg_surface, double_src->fg_surface, hw_node);
    return TD_SUCCESS;
}

static td_s32 tde_osi_double_node(td_s32 handle, drv_tde_double_src *double_src, const drv_tde_opt *opt,
    tde_hw_node *hw_node)
{
    td_s32 ret;
    td_bool set_filter_node;

#if (TDE_CAPABILITY & SLICE)
    set_filter_node = ((opt->resize) || ((opt->deflicker_mode != DRV_TDE_DEFLICKER_LEVEL_MODE_NONE)) ||
                       (double_src->fg_surface->color_format >= DRV_TDE_COLOR_FMT_YCBCR422) ||
                       (double_src->bg_surface->color_format >= DRV_TDE_COLOR_FMT_YCBCR422));
#else
    set_filter_node = ((opt->resize) || (opt->deflicker_mode != DRV_TDE_DEFLICKER_LEVEL_MODE_NONE) ||
                       (double_src->fg_surface->color_format >= DRV_TDE_COLOR_FMT_YCBCR422));
#endif
    if (set_filter_node) {
#if (TDE_CAPABILITY & SLICE)
        double_src->bg_rect->height = double_src->dst_rect->height;
        double_src->bg_rect->width = double_src->dst_rect->width;

        if ((double_src->fg_surface->color_format >= DRV_TDE_COLOR_FMT_YCBCR422) && (!(opt->resize))) {
            double_src->fg_rect->height = double_src->dst_rect->height;
            double_src->fg_rect->width = double_src->dst_rect->width;
        }
        ret = tde_osi_set_filter_node(handle, hw_node, double_src,
                                      opt->deflicker_mode, opt->filter_mode);
        if (ret != TD_SUCCESS) {
            return ret;
        }
        tde_hal_free_node_buf(hw_node);
#else
        ret = tde_osi_set_filter_node(handle, hw_node, double_src, opt->deflicker_mode,
            (drv_tde_deflicker_mode)opt->filter_mode);
        if (ret != TD_SUCCESS) {
            return ret;
        }
#endif
        return TD_SUCCESS;
    }
    ret = tde_osi_set_node_finish(handle, hw_node, 0, TDE_NODE_SUBM_ALONE);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    return TD_SUCCESS;
}

/*
 * Function:      tde_osi_double_src_2_blit
 * Description:   dual source operate realization
 * Input:         double_src:  bitmap info struct
 *                opt: config parameter
 * Return:        success/fail
 * Others:        add support for YCbCr422
 */
static td_s32 tde_osi_double_src_2_blit(td_s32 handle, drv_tde_double_src *double_src, const drv_tde_opt *opt)
{
    tde_hw_node *hw_node = TD_NULL;
    tde_scandirection_mode src_scan_info = { 0 };
    tde_scandirection_mode dst_scan_info = { 0 };
    tde_surface_msg drv_surface = { 0 };
    td_s32 ret;
    drv_tde_single_src single_src;

    ret = tde_osi_double_para(double_src, opt, &hw_node);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    single_src.src_surface = double_src->fg_surface;
    single_src.src_rect = double_src->fg_rect;
    single_src.dst_surface = double_src->dst_surface;
    single_src.dst_rect = double_src->dst_rect;
    if (tde_osi_get_scan_info_ex(&single_src, opt, &src_scan_info, &dst_scan_info) != TD_SUCCESS) {
        tde_hal_free_node_buf(hw_node);
        return DRV_ERR_TDE_INVALID_PARA;
    }

#if (TDE_CAPABILITY & COMPRESS)
    if ((opt->is_compress) || (opt->is_decompress)) {
        src_scan_info.hor_scan = TDE_SCAN_LEFT_RIGHT;
        src_scan_info.ver_scan = TDE_SCAN_UP_DOWN;
        dst_scan_info.hor_scan = TDE_SCAN_LEFT_RIGHT;
        dst_scan_info.ver_scan = TDE_SCAN_UP_DOWN;
    }
#endif
    tde_osi_convert_surface(double_src->bg_surface, double_src->bg_rect, &src_scan_info, &drv_surface);

    tde_hal_node_set_src1(hw_node, &drv_surface);

    tde_osi_convert_surface(double_src->fg_surface, double_src->fg_rect, &src_scan_info, &drv_surface);

    tde_hal_node_set_src2(hw_node, &drv_surface);

    tde_osi_convert_surface(double_src->dst_surface, double_src->dst_rect, &dst_scan_info, &drv_surface);
    ret = tde_osi_double_opt(double_src, opt, hw_node, &drv_surface);
    if (ret != TD_SUCCESS) {
        tde_hal_free_node_buf(hw_node);
        return ret;
    }

    ret = tde_osi_double_node(handle, double_src, opt, hw_node);
    if (ret != TD_SUCCESS) {
        tde_hal_free_node_buf(hw_node);
        return ret;
    }
    return TD_SUCCESS;
}

static td_s32 tde_osi_set_color_key(const drv_tde_double_src *double_src,
                                    tde_hw_node *hw_node, drv_tde_color_key color_key_value,
                                    drv_tde_color_key_mode color_key_mode,
                                    tde_clut_usage clut_usage)
{
    tde_color_key_cmd color_key;
    tde_colorfmt_category fmt_category;
    td_bool unknown_fmt_category;
    td_bool color_key_foreground_before_clut_mode = (clut_usage != TDE_CLUT_COLOREXPENDING) &&
                                                     (clut_usage != TDE_CLUT_CLUT_BYPASS);
    td_bool set_color_key = (color_key_mode != DRV_TDE_COLOR_KEY_MODE_NONE);

    color_key.colorkey_value = color_key_value;

    if (!set_color_key) {
        return TD_SUCCESS;
    }

    switch (color_key_mode) {
        case DRV_TDE_COLOR_KEY_MODE_BACKGROUND:
            color_key.colorkey_mode = TDE_DRV_COLORKEY_BACKGROUND;

            fmt_category = tde_osi_get_fmt_category(double_src->bg_surface->color_format);

            break;

        case DRV_TDE_COLOR_KEY_MODE_FOREGROUND:

            color_key.colorkey_mode = (color_key_foreground_before_clut_mode) ?
                TDE_DRV_COLORKEY_FOREGROUND_AFTER_CLUT : TDE_DRV_COLORKEY_FOREGROUND_BEFORE_CLUT;

            fmt_category = tde_osi_get_fmt_category(double_src->fg_surface->color_format);

            break;

        default:
            tde_error("invalid ColorKeyMode!\n");

            return DRV_ERR_TDE_INVALID_PARA;
    }

    unknown_fmt_category = fmt_category >= TDE_COLORFMT_CATEGORY_BUTT;

    if (unknown_fmt_category) {
        tde_error("Unknown fmt category!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (tde_hal_node_set_colorkey(hw_node, fmt_category, &color_key) != TD_SUCCESS) {
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    return TD_SUCCESS;
}

static td_s32 tde_osi_mb_val(const drv_tde_mb_src *mb_src, drv_tde_mb_opt *mb_opt, td_u32 *cb_cr_height,
    td_u32 *byte_per_pixel, td_u32 *cb_cr_byte_per_pixel)
{
    td_s32 ret;
    td_s32 bpp;

    ret = tde_osi_check_mb_blit_para(mb_src, mb_opt);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    if ((mb_src->dst_rect->height == mb_src->src_rect->height) &&
        (mb_src->dst_rect->width == mb_src->src_rect->width)) {
        mb_opt->resize_en = DRV_TDE_MB_RESIZE_NONE;
    }

    if (mb_opt->resize_en == DRV_TDE_MB_RESIZE_NONE) {
        tde_unify_rect(mb_src->src_rect, mb_src->dst_rect);
    }

    if (tde_osi_check_resize_para(mb_src->src_rect->width, mb_src->src_rect->height,
        mb_src->dst_rect->width, mb_src->dst_rect->height) != TD_SUCCESS) {
        tde_error("The Scale is too large!\n");
        return DRV_ERR_TDE_MINIFICATION;
    }

    *cb_cr_height = TDE_MAX_SLICE_RECT_HEIGHT;
    if ((mb_src->mb_surface->mb_color_format == DRV_TDE_MB_COLOR_FMT_MP1_YCBCR420MBP) ||
        (mb_src->mb_surface->mb_color_format == DRV_TDE_MB_COLOR_FMT_MP2_YCBCR420MBP) ||
        (mb_src->mb_surface->mb_color_format == DRV_TDE_MB_COLOR_FMT_MP2_YCBCR420MBI) ||
        (mb_src->mb_surface->mb_color_format == DRV_TDE_MB_COLOR_FMT_JPG_YCBCR420MBP) ||
        (mb_src->mb_surface->mb_color_format == DRV_TDE_MB_COLOR_FMT_JPG_YCBCR422MBVP)) {
        *cb_cr_height = TDE_MAX_SLICE_RECT_HEIGHT / 2; /* 2 alg data */
    }

    *cb_cr_byte_per_pixel = ((mb_src->mb_surface->mb_color_format == DRV_TDE_MB_COLOR_FMT_JPG_YCBCR444MBP) ||
        (mb_src->mb_surface->mb_color_format == DRV_TDE_MB_COLOR_FMT_JPG_YCBCR422MBVP)) ? 2 : 1; /* 2 1 alg data */

    bpp = tde_osi_get_bpp_by_fmt(mb_src->dst_surface->color_format);
    *byte_per_pixel = ((td_u32)bpp >> 3); /* 3 alg data */
    return TD_SUCCESS;
}

typedef struct {
    td_u32 byte_per_pixel;
    td_u32 cbcr_pixel;
    td_u32 cb_cr_height;
    td_phys_addr_t yphy_addr;
    td_phys_addr_t u64phy;
    td_phys_addr_t cb_crphy_addr;
} tde_mb_blit_addr_info;

static td_void tde_osi_set_mb_addr(drv_tde_mb_src *mb_src, const tde_mb_blit_addr_info *addr_info,
    td_u32 m, td_u32 n)
{
    mb_src->mb_surface->y_addr = addr_info->yphy_addr + (td_u64)m * TDE_MAX_SLICE_RECT_WIDTH +
        (td_u64)n * (td_u64)(mb_src->mb_surface->y_stride) * TDE_MAX_SLICE_RECT_HEIGHT;
    mb_src->dst_surface->phys_addr = addr_info->u64phy + (td_u64)m * (td_u64)addr_info->byte_per_pixel *
        TDE_MAX_SLICE_RECT_WIDTH + (td_u64)n * (td_u64)(mb_src->dst_surface->stride) * TDE_MAX_SLICE_RECT_HEIGHT;
    mb_src->mb_surface->cbcr_phys_addr = addr_info->cb_crphy_addr + (td_u64)m *
        (td_u64)addr_info->cbcr_pixel * TDE_MAX_SLICE_RECT_WIDTH +
        (td_u64)n * (td_u64)(mb_src->mb_surface->cbcr_stride) * (td_u64)(addr_info->cb_cr_height);
}

/*
 * Function:      tde_osi_mb_blit
 * Description:   Mb blit
 * Return:        >0: return task id of current operate; <0: fail
 */
td_s32 tde_osi_mb_blit(td_s32 handle, drv_tde_mb_src *mb_src, drv_tde_mb_opt *mb_opt)
{
    td_u32 height;
    td_u32 width;
    td_u32 i;
    td_u32 j;
    td_u32 m;
    td_u32 n;
    tde_mb_blit_addr_info addr_info = {0};
    td_s32 ret = tde_osi_mb_val(mb_src, mb_opt, &addr_info.cb_cr_height, &addr_info.byte_per_pixel,
        &addr_info.cbcr_pixel);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    drv_tde_mb_src_val(mb_src, height, width);
    drv_tde_mb_src_addr(mb_src, addr_info.yphy_addr, addr_info.cb_crphy_addr, addr_info.u64phy);
    ret = tde_osi_get_double_cycle_data(mb_src->src_rect, &i, &j);
    if (ret != TD_SUCCESS) {
        tde_error("Tde osi Get double cycle data failed, ret = 0x%x\n", ret);
    }
    for (n = 0; n < j; n++) {
        for (m = 0; m < i; m++) {
            if ((i - 1) == m) {
                mb_src->src_rect->width = width - m * TDE_MAX_SLICE_RECT_WIDTH;
            } else {
                mb_src->src_rect->width = TDE_MAX_SLICE_RECT_WIDTH;
            }

            if ((j - 1) == n) {
                mb_src->src_rect->height = height - n * TDE_MAX_SLICE_RECT_HEIGHT;
            } else {
                mb_src->src_rect->height = TDE_MAX_SLICE_RECT_HEIGHT;
            }
            if (!((i == 1) && (j == 1))) {
                mb_src->dst_rect->width = mb_src->src_rect->width;
                mb_src->dst_rect->height = mb_src->src_rect->height;
            }
            tde_osi_set_mb_addr(mb_src, &addr_info, m, n);
            ret = tde_osi_set_mb_para(handle, mb_src, mb_opt);
            if (ret != TD_SUCCESS) {
                return ret;
            }
        }
    }
    return TD_SUCCESS;
}

static td_s32 tde_osi_check_surface_ex(const drv_tde_surface *surface, drv_tde_rect *rect)
{
    td_bool invalid_operation_area;
    td_s32 ret = tde_osi_pre_check_surface_ex(surface, rect);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    invalid_operation_area = ((rect->width > TDE_MAX_RECT_WIDTH_EX) ||
                              (rect->height > TDE_MAX_RECT_HEIGHT_EX));

    if (invalid_operation_area) {
        tde_error("invalid operation SurfaceEX area!width=%d,height=%d\n", rect->width, rect->height);
        return DRV_ERR_TDE_INVALID_PARA;
    }

    return TD_SUCCESS;
}

static td_s32 tde_osi_check_mb_src_opt(const drv_tde_mb_src *mb_src, const drv_tde_mb_opt *mb_opt)
{
    if (mb_src->dst_surface->color_format >= DRV_TDE_COLOR_FMT_JPG_YCBCR400MBP) {
        tde_error("This operation doesn't support Semi-plannar!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (mb_src->mb_surface->mb_color_format >= DRV_TDE_MB_COLOR_FMT_MAX) {
        tde_error("mb color format error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (((mb_opt->is_deflicker != TD_TRUE) && (mb_opt->is_deflicker != TD_FALSE)) ||
        ((mb_opt->is_set_out_alpha != TD_TRUE) && (mb_opt->is_set_out_alpha != TD_FALSE))) {
        tde_error("bool should be TRUE or FALSE!!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (mb_opt->resize_en >= DRV_TDE_MB_RESIZE_MAX) {
        tde_error("enMBResize error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    return TD_SUCCESS;
}

static td_s32 tde_osi_check_mb_blit_para(const drv_tde_mb_src *mb_src, const drv_tde_mb_opt *mb_opt)
{
    td_s32 ret;
    td_bool null_ptr = ((mb_src->mb_surface == TD_NULL) || ((mb_src->src_rect == TD_NULL)) ||
                        (mb_src->dst_surface == TD_NULL) || (mb_src->dst_rect == TD_NULL) || (mb_opt == TD_NULL));

    if (null_ptr) {
        tde_error("Contains NULL ptr!\n");
        return DRV_ERR_TDE_NULL_PTR;
    }

    ret = tde_osi_check_mb_src_opt(mb_src, mb_opt);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    if ((tde_osi_check_surface_ex(mb_src->dst_surface, mb_src->dst_rect) != TD_SUCCESS) ||
        (tde_osi_check_mb_surface_ex(mb_src->mb_surface, mb_src->src_rect) != TD_SUCCESS)) {
        return DRV_ERR_TDE_INVALID_PARA;
    }

    return TD_SUCCESS;
}

static td_s32 tde_osi_get_double_cycle_data(const drv_tde_rect *mb_rect, td_u32 *i, td_u32 *j)
{
    if ((mb_rect->height > TDE_MAX_RECT_HEIGHT) && (mb_rect->width > TDE_MAX_RECT_WIDTH)) {
        *i = 2; /* 2 alg data */
        *j = 2; /* 2 alg data */
    } else if ((mb_rect->height > TDE_MAX_RECT_HEIGHT) && (mb_rect->width <= TDE_MAX_RECT_WIDTH)) {
        *i = 1;
        *j = 2; /* 2 alg data */
    } else if ((mb_rect->height <= TDE_MAX_RECT_HEIGHT) && (mb_rect->width > TDE_MAX_RECT_WIDTH)) {
        *i = 2; /* 2 alg data */
        *j = 1;
    } else {
        *i = 1;
        *j = 1;
    }
    return TD_SUCCESS;
}

/*
 * Function:      tde_osi_bitmap_mask_check_para
 * Description:   check for trinal source operate parameter
 * Return:        =0: success; <0: fail
 */
#if (TDE_CAPABILITY & MASKROP || TDE_CAPABILITY & MASKBLEND)
static td_s32 tde_osi_bitmap_mask_check_triple_src(const drv_tde_triple_src *triple_src)
{
    if (tde_osi_check_surface(triple_src->bg_surface, triple_src->bg_rect) != TD_SUCCESS) {
        tde_error("pstBackGroundRect does not correct!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (tde_osi_check_surface(triple_src->fg_surface, triple_src->fg_rect) != TD_SUCCESS) {
        tde_error("pstForeGroundRect does not correct!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (tde_osi_check_surface(triple_src->mask_surface, triple_src->mask_rect) != TD_SUCCESS) {
        tde_error("pstMaskRect does not correct!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (tde_osi_check_surface(triple_src->dst_surface, triple_src->dst_rect) != TD_SUCCESS) {
        tde_error("dst_rect does not correct!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    return TD_SUCCESS;
}

static td_s32 tde_osi_bitmap_mask_check_para(const drv_tde_triple_src *triple_src)
{
    td_bool is_unsupported_scale;
    td_s32 ret;
    is_unsupported_scale = ((triple_src == TD_NULL) || (triple_src->bg_surface == TD_NULL) ||
        (triple_src->bg_rect == TD_NULL) || (triple_src->fg_surface == TD_NULL) ||
        (triple_src->fg_rect == TD_NULL) || (triple_src->mask_surface == TD_NULL) ||
        (triple_src->mask_rect == TD_NULL) || (triple_src->dst_surface == TD_NULL) ||
        (triple_src->dst_rect == TD_NULL));

    if (is_unsupported_scale) {
        tde_error("Contains NULL ptr!\n");
        return DRV_ERR_TDE_NULL_PTR;
    }

    if ((triple_src->fg_surface->color_format >= DRV_TDE_COLOR_FMT_JPG_YCBCR400MBP) ||
        (triple_src->bg_surface->color_format >= DRV_TDE_COLOR_FMT_JPG_YCBCR400MBP) ||
        (triple_src->mask_surface->color_format >= DRV_TDE_COLOR_FMT_JPG_YCBCR400MBP) ||
        (triple_src->dst_surface->color_format >= DRV_TDE_COLOR_FMT_JPG_YCBCR400MBP)) {
        tde_error("This operation doesn't support Semi-plannar!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if ((triple_src->bg_surface->color_format == DRV_TDE_COLOR_FMT_YCBCR422) ||
        (triple_src->bg_surface->color_format == DRV_TDE_COLOR_FMT_PKGVYUY)) {
        tde_error("This operation doesn't support PKG!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    ret = tde_osi_bitmap_mask_check_triple_src(triple_src);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    is_unsupported_scale = ((triple_src->bg_rect->width != triple_src->fg_rect->width) ||
                          (triple_src->fg_rect->width != triple_src->mask_rect->width) ||
                          (triple_src->mask_rect->width != triple_src->dst_rect->width) ||
                          (triple_src->bg_rect->height != triple_src->fg_rect->height) ||
                          (triple_src->fg_rect->height != triple_src->mask_rect->height) ||
                          (triple_src->mask_rect->height != triple_src->dst_rect->height));

    if (is_unsupported_scale) {
        tde_error("Don't support scale!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    return TD_SUCCESS;
}
#endif

/*
 * Function:      tde_osi_bitmap_mask_rop
 * Description:   Ropmask foreground and mask at firs, and then ropmask background and middle bitmap
 *                output result to target bitmap
 */
#if (TDE_CAPABILITY & MASKROP)
static td_s32 tde_osi_bitmap_mask_rop_set_hw_node(tde_hw_node *hw_node, const drv_tde_triple_src *triple_src,
                                                  drv_tde_surface *mid_surface, drv_tde_rect *mid_rect)
{
    tde_scandirection_mode scan_info = {0};
    tde_surface_msg src_drv_surface = {0};
    tde_surface_msg dst_drv_surface = {0};

    scan_info.hor_scan = TDE_SCAN_LEFT_RIGHT;
    scan_info.ver_scan = TDE_SCAN_UP_DOWN;

    tde_osi_convert_surface(triple_src->fg_surface, triple_src->fg_rect, &scan_info, &src_drv_surface);

    tde_hal_node_set_src1(hw_node, &src_drv_surface);

    tde_osi_convert_surface(triple_src->mask_surface, triple_src->mask_rect, &scan_info, &src_drv_surface);

    tde_hal_node_set_src2(hw_node, &src_drv_surface);

    if (memcpy_s(mid_surface, sizeof(drv_tde_surface), triple_src->fg_surface, sizeof(drv_tde_surface)) != EOK) {
        tde_error("secure function failure\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    mid_surface->phys_addr = tde_osi_list_get_phy_buff(0);
    if (mid_surface->phys_addr == 0) {
        tde_error("There is no temp buffer in tde_osi_bitmap_mask_rop!\n");
        return DRV_ERR_TDE_NO_MEM;
    }
    mid_surface->clut_phys_addr = 0;
    mid_rect->pos_x = 0;
    mid_rect->pos_y = 0;
    mid_rect->height = triple_src->fg_rect->height;
    mid_rect->width = triple_src->fg_rect->width;

    tde_osi_convert_surface(mid_surface, mid_rect, &scan_info, &dst_drv_surface);

    tde_hal_node_set_tqt(hw_node, &dst_drv_surface, DRV_TDE_OUT_ALPHA_FROM_NORM);

    tde_osi_set_ext_alpha(triple_src->fg_surface, mid_surface, hw_node);

    if (tde_hal_node_set_base_operate(hw_node, TDE_NORM_BLIT_2OPT, TDE_ALU_MASK_ROP1, 0) != TD_SUCCESS) {
        tde_osi_list_put_phy_buff(1);
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    return TD_SUCCESS;
}

static td_s32 tde_osi_bitmap_mask_rop_set_hw_node_pass2(tde_hw_node *hw_node_pass2,
                                                        const drv_tde_triple_src *triple_src,
                                                        const drv_tde_none_src *none_src,
                                                        drv_tde_rop_mode rop_color, drv_tde_rop_mode rop_alpha)
{
    tde_scandirection_mode scan_info = {0};
    tde_surface_msg src_drv_surface = {0};
    tde_surface_msg dst_drv_surface = {0};
    td_u16 code;
    tde_conv_mode_cmd conv = {0};

    scan_info.hor_scan = TDE_SCAN_LEFT_RIGHT;
    scan_info.ver_scan = TDE_SCAN_UP_DOWN;

    tde_osi_convert_surface(triple_src->bg_surface, triple_src->bg_rect, &scan_info, &src_drv_surface);

    tde_hal_node_set_src1(hw_node_pass2, &src_drv_surface);

    tde_osi_convert_surface(none_src->dst_surface, none_src->dst_rect, &scan_info, &dst_drv_surface);

    tde_hal_node_set_src2(hw_node_pass2, &dst_drv_surface);

    tde_osi_convert_surface(triple_src->dst_surface, triple_src->dst_rect, &scan_info, &dst_drv_surface);

    tde_hal_node_set_tqt(hw_node_pass2, &dst_drv_surface, DRV_TDE_OUT_ALPHA_FROM_NORM);

    code = tde_osi_single_src2_get_opt_code(triple_src->fg_surface->color_format,
                                            triple_src->dst_surface->color_format);

    tde_osi_get_conv_by_code(code, &conv);

    if (tde_hal_node_set_color_convert(hw_node_pass2, &conv) != TD_SUCCESS) {
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    if (tde_hal_node_set_rop(hw_node_pass2, rop_color, rop_alpha) != TD_SUCCESS) {
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    tde_osi_set_ext_alpha(triple_src->bg_surface, none_src->dst_surface, hw_node_pass2);

    /* logical operation second passs */
    if (tde_hal_node_set_base_operate(hw_node_pass2, TDE_NORM_BLIT_2OPT, TDE_ALU_MASK_ROP2, 0) != TD_SUCCESS) {
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    return TD_SUCCESS;
}

static td_s32 tde_osi_bitmap_mask_rop_check_color(const drv_tde_triple_src *triple_src)
{
    if (!tde_osi_whether_contain_alpha(triple_src->fg_surface->color_format)) {
        tde_error("ForeGround bitmap must contains alpha component!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (triple_src->mask_surface->color_format != DRV_TDE_COLOR_FMT_A1) {
        tde_error("Maskbitmap's colorformat can only be A1 in tde_osi_bitmap_mask_rop!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (triple_src->dst_surface->color_format >= DRV_TDE_COLOR_FMT_CLUT1 &&
        triple_src->dst_surface->color_format <= DRV_TDE_COLOR_FMT_A8) {
        tde_error("dst color format %d not support\n", triple_src->dst_surface->color_format);
        return DRV_ERR_TDE_INVALID_PARA;
    }
    return TD_SUCCESS;
}

td_s32 tde_osi_bitmap_mask_rop(td_s32 handle, const drv_tde_triple_src *triple_src, drv_tde_rop_mode rop_color,
                               drv_tde_rop_mode rop_alpha)
{
    tde_hw_node *hw_node = TD_NULL;
    tde_hw_node *hw_node_pass2 = TD_NULL;
    drv_tde_surface mid_surface = {0};
    drv_tde_rect mid_rect = {0};
    drv_tde_none_src none_src = {&mid_surface, &mid_rect};
    td_s32 ret;

    ret = tde_osi_bitmap_mask_check_para(triple_src);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    if (tde_osi_bitmap_mask_rop_check_color(triple_src) != TD_SUCCESS) {
        return DRV_ERR_TDE_INVALID_PARA;
    }

    /* init hw_node */
    if (tde_hal_node_init_nd(&hw_node) != TD_SUCCESS) {
        return DRV_ERR_TDE_NO_MEM;
    }
    /*
     * set hw_node:
     * set src1
     * set src2
     * set tqt
     * set ext alpha
     * set opt
     * set global alpha
     */
    ret = tde_osi_bitmap_mask_rop_set_hw_node(hw_node, triple_src, &mid_surface, &mid_rect);
    if (ret != TD_SUCCESS) {
        tde_hal_free_node_buf(hw_node);
        return ret;
    }

    ret = tde_osi_set_node_finish(handle, hw_node, 0, TDE_NODE_SUBM_ALONE);
    if (ret != TD_SUCCESS) {
        goto err1;
    }
    /* init hw_node_pass2 */
    if (tde_hal_node_init_nd(&hw_node_pass2) != TD_SUCCESS) {
        ret =  DRV_ERR_TDE_NO_MEM;
        goto err1;
    }

    /*
     * set hw_node:
     * set src1
     * set src2
     * set tqt
     * set color convert
     * set rop
     * set ext alpha
     * set opt
     */
    ret = tde_osi_bitmap_mask_rop_set_hw_node_pass2(hw_node_pass2, triple_src, &none_src,
                                                    rop_color, rop_alpha);
    if (ret != TD_SUCCESS) {
        goto err2;
    }

    ret = tde_osi_set_node_finish(handle, hw_node_pass2, 1, TDE_NODE_SUBM_ALONE);
    if (ret != TD_SUCCESS) {
        goto err2;
    }
    return TD_SUCCESS;
err2:
    tde_hal_free_node_buf(hw_node_pass2);
err1:
    tde_hal_free_node_buf(hw_node);
    tde_osi_list_put_phy_buff(1);
    return ret;
}
#endif

#if (TDE_CAPABILITY & MASKBLEND)
static td_s32 tde_osi_bitmap_mask_blend_set_hw_node(tde_hw_node *hw_node, const drv_tde_triple_src *triple_src,
                                                    drv_tde_surface *mid_surface, drv_tde_rect *mid_rect)
{
    tde_scandirection_mode scan_info = {0};
    tde_surface_msg src_drv_surface = {0};
    tde_surface_msg dst_drv_surface = {0};

    scan_info.hor_scan = TDE_SCAN_LEFT_RIGHT;
    scan_info.ver_scan = TDE_SCAN_UP_DOWN;

    tde_osi_convert_surface(triple_src->fg_surface, triple_src->fg_rect, &scan_info, &src_drv_surface);

    tde_hal_node_set_src1(hw_node, &src_drv_surface);

    tde_osi_convert_surface(triple_src->mask_surface, triple_src->mask_rect, &scan_info, &src_drv_surface);

    tde_hal_node_set_src2(hw_node, &src_drv_surface);

    if (memcpy_s(mid_surface, sizeof(drv_tde_surface), triple_src->fg_surface, sizeof(drv_tde_surface)) != EOK) {
        tde_error("secure function failure\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    mid_surface->phys_addr = tde_osi_list_get_phy_buff(0);
    if (mid_surface->phys_addr == 0) {
        tde_error("There is no temp buffer in tde_osi_bitmap_mask_blend!\n");
        return DRV_ERR_TDE_NO_MEM;
    }
    mid_rect->pos_x = 0;
    mid_rect->pos_y = 0;
    mid_rect->height = triple_src->fg_rect->height;
    mid_rect->width = triple_src->fg_rect->width;

    tde_osi_convert_surface(mid_surface, mid_rect, &scan_info, &dst_drv_surface);

    tde_hal_node_set_tqt(hw_node, &dst_drv_surface, DRV_TDE_OUT_ALPHA_FROM_NORM);

    tde_osi_set_ext_alpha(triple_src->fg_surface, mid_surface, hw_node);

    if (tde_hal_node_set_base_operate(hw_node, TDE_NORM_BLIT_2OPT, TDE_ALU_MASK_BLEND, 0) != TD_SUCCESS) {
        tde_osi_list_put_phy_buff(1);
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    tde_hal_node_set_global_alpha(hw_node, 0xff, TD_TRUE);
    return TD_SUCCESS;
}

static td_s32 tde_osi_bitmap_mask_blend_set_hw_node_pass2(tde_hw_node *hw_node_pass2,
                                                          const drv_tde_triple_src *triple_src,
                                                          const drv_tde_surface *mid_surface,
                                                          const drv_tde_rect *mid_rect,
                                                          td_u8 alpha)
{
    tde_scandirection_mode scan_info = {0};
    tde_surface_msg src_drv_surface = {0};
    tde_surface_msg dst_drv_surface = {0};
    td_u16 code;
    tde_conv_mode_cmd conv = {0};
    tde_alu_mode drv_alu_mode = TDE_ALU_BLEND;
    drv_tde_blend_opt blend_opt = {0};

    scan_info.hor_scan = TDE_SCAN_LEFT_RIGHT;
    scan_info.ver_scan = TDE_SCAN_UP_DOWN;
    tde_osi_convert_surface(triple_src->bg_surface, triple_src->bg_rect, &scan_info, &src_drv_surface);

    tde_hal_node_set_src1(hw_node_pass2, &src_drv_surface);

    tde_osi_convert_surface(mid_surface, mid_rect, &scan_info, &dst_drv_surface);

    tde_hal_node_set_src2(hw_node_pass2, &dst_drv_surface);

    tde_osi_convert_surface(triple_src->dst_surface, triple_src->dst_rect, &scan_info, &dst_drv_surface);

    tde_hal_node_set_tqt(hw_node_pass2, &dst_drv_surface, DRV_TDE_OUT_ALPHA_FROM_NORM);

    code = tde_osi_single_src2_get_opt_code(triple_src->fg_surface->color_format,
        triple_src->dst_surface->color_format);

    tde_osi_get_conv_by_code(code, &conv);

    if (tde_hal_node_set_color_convert(hw_node_pass2, &conv) != TD_SUCCESS) {
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    tde_hal_node_set_global_alpha(hw_node_pass2, alpha, TD_TRUE);

    if (tde_hal_node_set_base_operate(hw_node_pass2, TDE_NORM_BLIT_2OPT, drv_alu_mode, 0) != TD_SUCCESS) {
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    blend_opt.global_alpha_en = TD_TRUE;
    blend_opt.pixel_alpha_en = TD_TRUE;
    blend_opt.blend_cmd = DRV_TDE_BLEND_CMD_NONE;
    if (tde_hal_node_set_blend(hw_node_pass2, &blend_opt) != TD_SUCCESS) {
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    tde_osi_set_ext_alpha(triple_src->bg_surface, mid_surface, hw_node_pass2);
    return TD_SUCCESS;
}

static td_s32 tde_osi_bitmap_mask_blend_check_color(const drv_tde_triple_src *triple_src,
                                                    drv_tde_alpha_blending blend_mode)
{
    if ((triple_src->mask_surface->color_format != DRV_TDE_COLOR_FMT_A1) &&
        (triple_src->mask_surface->color_format != DRV_TDE_COLOR_FMT_A8)) {
        tde_error("Maskbitmap's colorformat can only be An in tde_osi_bitmap_mask_blend!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    if (blend_mode < 0 || blend_mode >= DRV_TDE_ALPHA_BLENDING_MAX) {
        tde_error("Alum mode invalid!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    if (blend_mode != DRV_TDE_ALPHA_BLENDING_BLEND) {
        tde_error("Alum mode can only be blending in tde_osi_bitmap_mask_blend!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    if (triple_src->dst_surface->color_format >= DRV_TDE_COLOR_FMT_CLUT1 &&
        triple_src->dst_surface->color_format <= DRV_TDE_COLOR_FMT_A8) {
        tde_error("dst color format %d not support\n", triple_src->dst_surface->color_format);
        return DRV_ERR_TDE_INVALID_PARA;
    }
    return TD_SUCCESS;
}

td_s32 tde_osi_bitmap_mask_blend(td_s32 handle, const drv_tde_triple_src *triple_src, td_u8 alpha,
                                 drv_tde_alpha_blending blend_mode)
{
    tde_hw_node *hw_node = TD_NULL;
    tde_hw_node *hw_node_pass2 = TD_NULL;
    drv_tde_surface mid_surface = {0};
    drv_tde_rect mid_rect = {0};
    td_s32 ret;

    ret = tde_osi_bitmap_mask_check_para(triple_src);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    /* check color format and blend mode */
    if (tde_osi_bitmap_mask_blend_check_color(triple_src, blend_mode) != TD_SUCCESS) {
        return DRV_ERR_TDE_INVALID_PARA;
    }

    /* init hw_node */
    if (tde_hal_node_init_nd(&hw_node) != TD_SUCCESS) {
        return DRV_ERR_TDE_NO_MEM;
    }

    /*
     * set hw_node:
     * set src1
     * set src2
     * set tqt
     * set ext alpha
     * set opt
     * set global alpha
     */
    ret = tde_osi_bitmap_mask_blend_set_hw_node(hw_node, triple_src, &mid_surface, &mid_rect);
    if (ret != TD_SUCCESS) {
        tde_hal_free_node_buf(hw_node);
        return ret;
    }

    ret = tde_osi_set_node_finish(handle, hw_node, 0, TDE_NODE_SUBM_ALONE);
    if (ret != TD_SUCCESS) {
        tde_osi_list_put_phy_buff(1);
        tde_hal_free_node_buf(hw_node);
        return ret;
    }

    /* init hw_node_pass2 */
    if (tde_hal_node_init_nd(&hw_node_pass2) != TD_SUCCESS) {
        tde_osi_list_put_phy_buff(1);
        tde_hal_free_node_buf(hw_node);
        return DRV_ERR_TDE_NO_MEM;
    }

    /*
     * set hw_node_pass2:
     * set src1
     * set src2
     * set tqt
     * set color convert
     * set global alpha
     * set opt
     * set blend
     * set ext alpha
     */
    ret = tde_osi_bitmap_mask_blend_set_hw_node_pass2(hw_node_pass2, triple_src, &mid_surface, &mid_rect,
                                                      alpha);
    if (ret != TD_SUCCESS) {
        tde_hal_free_node_buf(hw_node);
        tde_hal_free_node_buf(hw_node_pass2);
        return ret;
    }

    ret = tde_osi_set_node_finish(handle, hw_node_pass2, 1, TDE_NODE_SUBM_ALONE);
    if (ret != TD_SUCCESS) {
        tde_hal_free_node_buf(hw_node);
        tde_hal_free_node_buf(hw_node_pass2);
        tde_osi_list_put_phy_buff(1);
        return ret;
    }

    return TD_SUCCESS;
}
#endif

/*
 * Function:      tde_osi_solid_draw
 * Description:   operate src1 with src2, which result to dst_surface,operate setting is in pstOpt
 *                if src is MB, only support single source operate,
                  just to say to only support pstBackGround or pstForeGround
 * Input:         handle: task handle
 *                fill_color:  fill  color
 *                opt: operate parameter setting struct
 * Return:        TD_SUCCESS/TD_FAILURE
 */
td_s32 tde_osi_solid_draw(td_s32 handle, const drv_tde_single_src *single_src, drv_tde_fill_color *fill_color,
                          const drv_tde_opt *opt)
{
    if (single_src == TD_NULL) {
        tde_error("null pointer!\n");
        return DRV_ERR_TDE_NULL_PTR;
    }
    if (single_src->src_surface == TD_NULL) {
        return tde_osi_1_source_fill(handle, single_src->dst_surface, single_src->dst_rect, fill_color, opt);
    } else {
        if ((single_src->dst_surface == TD_NULL) || (fill_color == TD_NULL)) {
            tde_error("when src_surface is not NULL, dst_surface and fill_color should not NULL!\n");
            return DRV_ERR_TDE_NULL_PTR;
        }

        if ((single_src->src_surface->color_format >= DRV_TDE_COLOR_FMT_JPG_YCBCR400MBP) ||
            (single_src->dst_surface->color_format >= DRV_TDE_COLOR_FMT_JPG_YCBCR400MBP) ||
            (fill_color->color_format >= DRV_TDE_COLOR_FMT_JPG_YCBCR400MBP)) {
            tde_error("This operation doesn't support Semi-plannar!\n");
            return DRV_ERR_TDE_INVALID_PARA;
        }

        return tde_osi_2_source_fill(handle, single_src, fill_color, opt);
    }
}

#if (TDE_CAPABILITY & DEFLICKER)
td_s32 tde_osi_set_deflicker_level(drv_tde_deflicker_level deflicker_level)
{
    return tde_hal_set_deflicer_level(deflicker_level);
}

td_s32 tde_osi_get_deflicker_level(drv_tde_deflicker_level *deflicker_level)
{
    return tde_hal_get_deflicer_level(deflicker_level);
}
#endif

td_s32 tde_osi_set_alpha_threshold_value(td_u8 alpha_threshold_value)
{
    return tde_hal_set_alpha_threshold(alpha_threshold_value);
}

td_s32 tde_osi_set_alpha_threshold_state(td_bool alpha_threshold_en)
{
    return tde_hal_set_alpha_threshold_state(alpha_threshold_en);
}

td_s32 tde_osi_get_alpha_threshold_value(td_u8 *alpha_threshold_value)
{
    return tde_hal_get_alpha_threshold(alpha_threshold_value);
}

td_s32 tde_osi_get_alpha_threshold_state(td_bool *alpha_threshold_en)
{
    return tde_hal_get_alpha_threshold_state(alpha_threshold_en);
}

static td_s32 tde_osi_check_src_pattern_fill_opt(const drv_tde_pattern_fill_opt *opt)
{
    td_bool is_invalid;
    if (opt == TD_NULL) {
        return TD_FAILURE;
    }

    is_invalid = (opt->colorkey_mode >= DRV_TDE_COLOR_KEY_MODE_MAX) ||
        (opt->colorkey_mode < DRV_TDE_COLOR_KEY_MODE_NONE);
    if (is_invalid == TD_TRUE) {
        tde_error("color_key_mode error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    is_invalid = ((opt->clut_reload != TD_TRUE) && (opt->clut_reload != TD_FALSE)) ||
        ((opt->blend_opt.global_alpha_en != TD_TRUE) && (opt->blend_opt.global_alpha_en != TD_FALSE)) ||
        ((opt->blend_opt.pixel_alpha_en != TD_TRUE) && (opt->blend_opt.pixel_alpha_en != TD_FALSE)) ||
        ((opt->blend_opt.src1_alpha_premulti != TD_TRUE) && (opt->blend_opt.src1_alpha_premulti != TD_FALSE)) ||
        ((opt->blend_opt.src2_alpha_premulti != TD_TRUE) && (opt->blend_opt.src2_alpha_premulti != TD_FALSE));
    if (is_invalid == TD_TRUE) {
        tde_error("bool should be TRUE or FALSE!!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (opt->colorkey_mode != DRV_TDE_COLOR_KEY_MODE_NONE) {
        tde_error("It doesn't support colorkey in single source pattern mode!\n");
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    is_invalid = (opt->alpha_blending_cmd != DRV_TDE_ALPHA_BLENDING_NONE) &&
        (opt->alpha_blending_cmd != DRV_TDE_ALPHA_BLENDING_BLEND) &&
        (opt->alpha_blending_cmd != DRV_TDE_ALPHA_BLENDING_ROP) &&
        (opt->alpha_blending_cmd != DRV_TDE_ALPHA_BLENDING_COLORIZE);
    if (is_invalid == TD_TRUE) {
        tde_error("alpha_blending_cmd error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    return TD_SUCCESS;
}

/*
 * Function:      tde_osi_check_single_src_pattern_opt
 * Description:   check if single source mode fill operate is valid
 * Input:         src_fmt foreground pixel format
                  dst_fmt  target pixel format
                  opt    operate attribute pointer
 * Return:        0  valid parameter
                  -1 invalid parameter
 */
static td_s32 tde_osi_check_single_src_pattern_opt(drv_tde_color_fmt src_fmt, drv_tde_color_fmt dst_fmt,
                                                   const drv_tde_pattern_fill_opt *opt)
{
    td_s32 ret;
    tde_colorfmt_transform color_trans_type = tde_osi_get_fmt_trans_type(src_fmt, dst_fmt);
    if (color_trans_type == TDE_COLORFMT_TRANSFORM_BUTT) {
        tde_error("Unknown color transport type!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    ret = tde_osi_check_src_pattern_fill_opt(opt);
    if (ret != TD_SUCCESS) {
        if (ret == TD_FAILURE) {
            return TD_SUCCESS;
        }
        return ret;
    }
#if (TDE_CAPABILITY & ROP)
    if ((td_u32)opt->alpha_blending_cmd & DRV_TDE_ALPHA_BLENDING_ROP) {
        if ((!tde_osi_is_single_src_to_rop(opt->rop_alpha)) ||
            (!tde_osi_is_single_src_to_rop(opt->rop_color))) {
            tde_error("Only support single s2 rop!\n");
            return DRV_ERR_TDE_INVALID_PARA;
        }
    } else {
        if (opt->alpha_blending_cmd != DRV_TDE_ALPHA_BLENDING_NONE) {
            tde_error("single src not suppot alpha blending!\n");
            return DRV_ERR_TDE_INVALID_PARA;
        }
    }
#else
    if (opt->alpha_blending_cmd != DRV_TDE_ALPHA_BLENDING_NONE) {
        tde_error("It doesn't ROP/Blend/Colorize!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
#endif
    if (color_trans_type == TDE_COLORFMT_TRANSFORM_CLUT_CLUT) {
        if ((opt->alpha_blending_cmd != DRV_TDE_ALPHA_BLENDING_NONE)) {
            tde_error("It doesn't ROP/Blend/Colorize!\n");
            return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
        }
        if (src_fmt != dst_fmt) {
            tde_error("If src fmt and dst fmt are clut, they shoulod be the same fmt!\n");
            return DRV_ERR_TDE_INVALID_PARA;
        }
    }
    return TD_SUCCESS;
}

static td_s32 tde_osi_check_bool_opt(drv_tde_color_fmt back_ground_fmt, const drv_tde_pattern_fill_opt *opt)
{
    td_bool real = (((opt->clut_reload != TD_TRUE) && (opt->clut_reload != TD_FALSE)) ||
        ((opt->blend_opt.global_alpha_en != TD_TRUE) && (opt->blend_opt.global_alpha_en != TD_FALSE)) ||
        ((opt->blend_opt.pixel_alpha_en != TD_TRUE) && (opt->blend_opt.pixel_alpha_en != TD_FALSE)) ||
        ((opt->blend_opt.src1_alpha_premulti != TD_TRUE) && (opt->blend_opt.src1_alpha_premulti != TD_FALSE)) ||
        ((opt->blend_opt.src2_alpha_premulti != TD_TRUE) && (opt->blend_opt.src2_alpha_premulti != TD_FALSE)));

    if (opt->alpha_blending_cmd >= DRV_TDE_ALPHA_BLENDING_MAX) {
        tde_error("alpha_blending_cmd error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if ((back_ground_fmt == DRV_TDE_COLOR_FMT_YCBCR422) || (back_ground_fmt == DRV_TDE_COLOR_FMT_PKGVYUY)) {
        tde_error("This operation doesn't support PKG!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (real) {
        tde_error("bool should be TRUE or FALSE!!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if ((opt->colorkey_mode >= DRV_TDE_COLOR_KEY_MODE_MAX) ||
        (opt->colorkey_mode < DRV_TDE_COLOR_KEY_MODE_NONE)) {
        tde_error("color_key_mode error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (opt->out_alpha_from >= DRV_TDE_OUT_ALPHA_FROM_MAX) {
        tde_error("out_alpha_from error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    return TD_SUCCESS;
}

static td_s32 tde_osi_check_double_src_alpha_category(tde_colorfmt_category bg_category,
                                                      tde_colorfmt_category fg_category,
                                                      drv_tde_color_fmt back_ground_fmt,
                                                      drv_tde_color_fmt fore_ground_fmt,
                                                      drv_tde_color_fmt dst_fmt)
{
    td_bool dst_have_alpha = tde_osi_whether_contain_alpha(dst_fmt);
    if ((bg_category == TDE_COLORFMT_CATEGORY_ARGB) &&
        (fg_category == TDE_COLORFMT_CATEGORY_AN) && (!dst_have_alpha)) {
        tde_error("Target must have alpha component!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    if ((fg_category == TDE_COLORFMT_CATEGORY_AN) && ((back_ground_fmt == DRV_TDE_COLOR_FMT_YCBCR888) ||
        (fore_ground_fmt == DRV_TDE_COLOR_FMT_AYCBCR8888))) {
        tde_error("Target must have alpha component!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    return TD_SUCCESS;
}

typedef struct {
    tde_colorfmt_category bg_category;
    tde_colorfmt_category fg_category;
    tde_colorfmt_category dst_category;
    drv_tde_color_fmt back_ground_fmt;
    drv_tde_color_fmt fore_ground_fmt;
    drv_tde_color_fmt dst_fmt;
    const drv_tde_pattern_fill_opt *opt;
} tde_osi_double_src_fmt_ctx;

static td_s32 tde_osi_check_double_src_clut_category(const tde_osi_double_src_fmt_ctx *fmt_ctx)
{
    if ((fmt_ctx->bg_category == TDE_COLORFMT_CATEGORY_CLUT) &&
        (fmt_ctx->opt->alpha_blending_cmd != DRV_TDE_ALPHA_BLENDING_NONE)) {
        tde_error("It doesn't support alpha blending!\n");
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    if ((fmt_ctx->bg_category == TDE_COLORFMT_CATEGORY_CLUT) &&
        ((fmt_ctx->back_ground_fmt != fmt_ctx->fore_ground_fmt) ||
        (fmt_ctx->back_ground_fmt != fmt_ctx->dst_fmt))) {
        tde_error("If background, foreground , dst are clut, they should be the same fmt!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    if ((fmt_ctx->bg_category == TDE_COLORFMT_CATEGORY_CLUT) &&
        ((fmt_ctx->fg_category != TDE_COLORFMT_CATEGORY_CLUT) ||
        (fmt_ctx->dst_category != TDE_COLORFMT_CATEGORY_CLUT))) {
        tde_error("Unsupported operation!\n");
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    return TD_SUCCESS;
}

static td_s32 tde_osi_check_double_src_dst_category(tde_colorfmt_category bg_category,
                                                    tde_colorfmt_category fg_category,
                                                    tde_colorfmt_category dst_category,
                                                    const drv_tde_pattern_fill_opt *opt)
{
    if ((bg_category == TDE_COLORFMT_CATEGORY_AN) && (fg_category == TDE_COLORFMT_CATEGORY_AN) &&
        (dst_category == TDE_COLORFMT_CATEGORY_AN) && (opt->alpha_blending_cmd != DRV_TDE_ALPHA_BLENDING_NONE)) {
        tde_error("It doesn't support ROP or mirror!\n");
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    if ((dst_category == TDE_COLORFMT_CATEGORY_CLUT) && ((fg_category != TDE_COLORFMT_CATEGORY_CLUT) ||
        (bg_category != TDE_COLORFMT_CATEGORY_CLUT))) {
        tde_error("Unsupported operation!\n");
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    return TD_SUCCESS;
}

/*
 * Function:      tde_osi_check_double_src_pattern_opt
 * Description:   check if doul source mode fill operate parameter is valid
 * Input:         back_ground_fmt background pixel format
                  fore_ground_fmt foreground pixel format
                  dst_fmt  target pixel format
                  opt     operate attribute pointer
 * Return:        0  valid parameter
                  -1 invalid parameter
 */
static td_s32 tde_osi_check_double_src_pattern_opt(drv_tde_color_fmt back_ground_fmt, drv_tde_color_fmt fore_ground_fmt,
                                                   drv_tde_color_fmt dst_fmt, const drv_tde_pattern_fill_opt *opt)
{
    tde_colorfmt_category bg_category;
    tde_colorfmt_category fg_category;
    tde_colorfmt_category dst_category;
    td_bool is_invalid;
    tde_osi_double_src_fmt_ctx fmt_ctx;

    td_s32 ret = tde_osi_check_bool_opt(back_ground_fmt, opt);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    bg_category = tde_osi_get_fmt_category(back_ground_fmt);
    fg_category = tde_osi_get_fmt_category(fore_ground_fmt);
    dst_category = tde_osi_get_fmt_category(dst_fmt);
    is_invalid = (bg_category >= TDE_COLORFMT_CATEGORY_BYTE) || (fg_category >= TDE_COLORFMT_CATEGORY_BYTE) ||
        (dst_category >= TDE_COLORFMT_CATEGORY_BYTE);
    if (is_invalid == TD_TRUE) {
        tde_error("unknown format!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }
    ret = tde_osi_check_double_src_alpha_category(bg_category, fg_category, back_ground_fmt, fore_ground_fmt, dst_fmt);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    fmt_ctx.bg_category = bg_category;
    fmt_ctx.fg_category = fg_category;
    fmt_ctx.dst_category = dst_category;
    fmt_ctx.back_ground_fmt = back_ground_fmt;
    fmt_ctx.fore_ground_fmt = fore_ground_fmt;
    fmt_ctx.dst_fmt = dst_fmt;
    fmt_ctx.opt = opt;
    ret = tde_osi_check_double_src_clut_category(&fmt_ctx);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    return tde_osi_check_double_src_dst_category(bg_category, fg_category, dst_category, opt);
}

static td_s32 tde_osi_check_pattern_dst_color_format(drv_tde_color_fmt color_format)
{
    if ((color_format == DRV_TDE_COLOR_FMT_YCBCR422) || (color_format == DRV_TDE_COLOR_FMT_PKGVYUY)) {
        tde_error("It doesn't support YCbCr422 or PKGVYUY in pattern fill!\n");
        return TD_FAILURE;
    }

    if (color_format >= DRV_TDE_COLOR_FMT_JPG_YCBCR400MBP) {
        tde_error("This operation doesn't support Semi-plannar!\n");
        return TD_FAILURE;
    }

    if ((color_format >= DRV_TDE_COLOR_FMT_A1) && (color_format != DRV_TDE_COLOR_FMT_YCBCR888) &&
        (color_format != DRV_TDE_COLOR_FMT_AYCBCR8888)) {
        tde_error("pattern fill:dst color format doesn't support format:%d!\n", color_format);
        return TD_FAILURE;
    }

    if (color_format >= DRV_TDE_COLOR_FMT_CLUT1 && color_format <= DRV_TDE_COLOR_FMT_ACLUT88 &&
        color_format != DRV_TDE_COLOR_FMT_CLUT8) {
        tde_error("This operation doesn't support dst color format clut,except clut8!\n");
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

static td_s32 tde_osi_check_pattern_src_color_format(drv_tde_color_fmt color_format)
{
    if (color_format >= DRV_TDE_COLOR_FMT_JPG_YCBCR400MBP) {
        tde_error("pattern fill:bg or fg color format doesn't support Semi-plannar!\n");
        return TD_FAILURE;
    }

    if ((color_format >= DRV_TDE_COLOR_FMT_A1) && (color_format != DRV_TDE_COLOR_FMT_YCBCR888) &&
        (color_format != DRV_TDE_COLOR_FMT_AYCBCR8888)) {
        tde_error("pattern fill:bg or fg color format doesn't support format:%d!\n", color_format);
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

/*
 * Function:      tde_osi_get_pattern_opt_category
 * Description:   analyze pattern fill operate type
 * Input:         dst_surface: target bitmap info
 *                dst_rect: target bitmap operate zone
 *                opt: operate option
 * Return:        TDE operate type
 */
static tde_pattern_operation_category tde_osi_get_pattern_opt_category(const drv_tde_double_src *double_src,
                                                                       const drv_tde_pattern_fill_opt *opt)
{
    if ((double_src == TD_NULL) || (double_src->dst_surface == TD_NULL) || (double_src->dst_rect == TD_NULL)) {
        tde_error("dst_surface/dst_rect should not be null!\n");
        return TDE_PATTERN_OPERATION_BUTT;
    }

    if (tde_osi_check_pattern_dst_color_format(double_src->dst_surface->color_format) != TD_SUCCESS) {
        return TDE_PATTERN_OPERATION_BUTT;
    }

    if (tde_osi_check_surface(double_src->dst_surface, double_src->dst_rect) != TD_SUCCESS) {
        return TDE_PATTERN_OPERATION_BUTT;
    }

    if ((double_src->bg_surface == TD_NULL) && (double_src->fg_surface == TD_NULL)) {
        tde_error("No src:both bg_surface and fg_surface are NULL!\n");
        return TDE_PATTERN_OPERATION_BUTT;
    } else if ((double_src->bg_surface != TD_NULL) && (double_src->fg_surface != TD_NULL)) {
        return tde_osi_double_src_pattern_operation(double_src, opt);
    } else {
        return tde_osi_check_single_src_pattern_operation(double_src, opt);
    }
}

static tde_pattern_operation_category tde_osi_check_single_src_pattern_operation(const drv_tde_double_src *double_src,
                                                                                 const drv_tde_pattern_fill_opt *opt)
{
    drv_tde_surface *tmp_src2 = TD_NULL;
    drv_tde_rect *tmp_src2_rect = TD_NULL;
    ot_unused(opt);

    if (double_src->bg_surface != TD_NULL) {
        if (double_src->bg_rect == TD_NULL) {
            tde_error("Background rect shouldn't be NULL!\n");
            return TDE_PATTERN_OPERATION_BUTT;
        }
        if (tde_osi_check_pattern_src_color_format(double_src->bg_surface->color_format) != TD_SUCCESS) {
            return TDE_PATTERN_OPERATION_BUTT;
        }
        tmp_src2 = double_src->bg_surface;
        tmp_src2_rect = double_src->bg_rect;
    } else if (double_src->fg_surface != TD_NULL) {
        if (double_src->fg_rect == TD_NULL) {
            tde_error("Foreground rect shouldn't be NULL!\n");
            return TDE_PATTERN_OPERATION_BUTT;
        }
        if (tde_osi_check_pattern_src_color_format(double_src->fg_surface->color_format) != TD_SUCCESS) {
            return TDE_PATTERN_OPERATION_BUTT;
        }
        tmp_src2 = double_src->fg_surface;
        tmp_src2_rect = double_src->fg_rect;
    }

    if ((tmp_src2 != TD_NULL) && (tmp_src2_rect != TD_NULL)) {
        if (tmp_src2_rect->width > TDE_MAX_PATTERNWIDTH) {
            tde_error("Max pattern width is 256!\n");
            return TDE_PATTERN_OPERATION_BUTT;
        }
        if (tde_osi_check_surface(tmp_src2, tmp_src2_rect) != TD_SUCCESS) {
            return TDE_PATTERN_OPERATION_BUTT;
        }
    }

    return TDE_PATTERN_OPERATION_SINGLE_SRC;
}

static tde_pattern_operation_category tde_osi_double_src_pattern_operation(const drv_tde_double_src *double_src,
                                                                           const drv_tde_pattern_fill_opt *opt)
{
    if ((double_src->bg_rect == TD_NULL) || (double_src->fg_rect == TD_NULL) || (opt == TD_NULL)) {
        tde_error("bg_rect/fg_rect/opt should not be null in two src pattern fill!\n");
        return TDE_PATTERN_OPERATION_BUTT;
    }

    if (tde_osi_check_pattern_src_color_format(double_src->bg_surface->color_format) != TD_SUCCESS) {
        return TDE_PATTERN_OPERATION_BUTT;
    }

    if (tde_osi_check_pattern_src_color_format(double_src->fg_surface->color_format) != TD_SUCCESS) {
        return TDE_PATTERN_OPERATION_BUTT;
    }

    if ((tde_osi_check_surface(double_src->bg_surface, double_src->bg_rect) != TD_SUCCESS) ||
        (tde_osi_check_surface(double_src->fg_surface, double_src->fg_rect) != TD_SUCCESS)) {
        return TDE_PATTERN_OPERATION_BUTT;
    }

    if ((double_src->bg_rect->height != double_src->dst_rect->height) ||
        (double_src->bg_rect->width != double_src->dst_rect->width)) {
        tde_error("Size of background rect and dst rect should be the same in two src pattern fill.\n");
        tde_error("background x:%d, y:%d, w:%d, h:%d; dst x:%d, y:%d, w:%d, h:%d\n", double_src->bg_rect->pos_x,
            double_src->bg_rect->pos_y, double_src->bg_rect->width, double_src->bg_rect->height,
            double_src->dst_rect->pos_x, double_src->dst_rect->pos_y, double_src->dst_rect->width,
            double_src->dst_rect->height);
        return TDE_PATTERN_OPERATION_BUTT;
    }

    if (double_src->fg_rect->width > TDE_MAX_PATTERNWIDTH) {
        tde_error("Max pattern width is %d!\n", TDE_MAX_PATTERNWIDTH);
        return TDE_PATTERN_OPERATION_BUTT;
    }

    return TDE_PATTERN_OPERATION_DOUBLE_SRC;
}

static td_bool tde_osi_check_overlap(const drv_tde_surface *sur1, drv_tde_rect *rect1, const drv_tde_surface *sur2,
                                     const drv_tde_rect *rect2)
{
    td_phys_addr_t rect1_start_phy;
    td_phys_addr_t rect2_start_phy;
    td_phys_addr_t rect1_end_phy;
    td_phys_addr_t rect2_end_phy;
    td_u32 bpp1;
    td_u32 bpp2;

    if (rect2->height < rect1->height) {
        rect1->height = rect2->height;
    }

    if (rect2->width < rect1->width) {
        rect1->width = rect2->width;
    }

    bpp1 = tde_osi_get_bpp_by_fmt(sur1->color_format) / 8; /* 8 alg data */
    bpp2 = tde_osi_get_bpp_by_fmt(sur2->color_format) / 8; /* 8 alg data */

    rect1_start_phy = sur1->phys_addr + (rect1->pos_y * sur1->stride)  +
        rect1->pos_x * bpp1;
    rect1_end_phy = rect1_start_phy + (rect1->height - 1) * sur1->stride  +
        (rect1->width - 1) * bpp1;

    rect2_start_phy = sur2->phys_addr + (rect2->pos_y * sur2->stride)  +
        rect2->pos_x * bpp2;
    rect2_end_phy = rect2_start_phy + (rect2->height - 1) * sur2->stride  +
        (rect2->width - 1) * bpp2;
    tde_info("rect1_start_phy:%lx, rect1_end_phy:%lx, rect2_start_phy:%lx, rect2_end_phy:%lx\n", (td_ulong)
        rect1_start_phy, (td_ulong)rect1_end_phy, (td_ulong)rect2_start_phy, (td_ulong)rect2_end_phy);

    return TD_FALSE;
}

static td_s32 tde_osi_pattern_fill_opt(tde_hw_node *hw_node, const drv_tde_pattern_fill_opt *opt,
                                       tde_alu_mode *alu_mode, td_bool is_check_single_src2rop,
                                       td_bool is_check_blend)
{
    td_s32 ret;

#if (TDE_CAPABILITY & ROP)
    tde_rop_opt rop_opt = {0};
    rop_opt.alpha_blending_cmd = opt->alpha_blending_cmd;
    rop_opt.rop_code_color = opt->rop_color;
    rop_opt.rop_code_alpha = opt->rop_alpha;
    rop_opt.single_sr2_rop = is_check_single_src2rop;
    ret = tde_osi_set_rop(hw_node, &rop_opt, alu_mode);
    if (ret != TD_SUCCESS) {
        return ret;
    }
#endif
    ret = tde_osi_set_blend(hw_node, opt->alpha_blending_cmd, opt->blend_opt, alu_mode,
                            is_check_blend);
    if (ret != TD_SUCCESS) {
        return ret;
    }
#if (TDE_CAPABILITY & COLORIZE)
    ret = tde_osi_set_colorize(hw_node, opt->alpha_blending_cmd, opt->color_resize);
    if (ret != TD_SUCCESS) {
        return ret;
    }
#endif
    tde_hal_node_set_global_alpha(hw_node, opt->global_alpha, opt->blend_opt.global_alpha_en);
    return TD_SUCCESS;
}

static td_s32 tde_osi_single_pattern_fill_opt(tde_hw_node *hw_node, const drv_tde_single_src *single_src,
                                              const drv_tde_pattern_fill_opt *opt)
{
    td_bool is_check_single_src2rop = TD_TRUE;
    td_bool is_check_blend = TD_FALSE;
    td_s32 ret;
    tde_alu_mode alu_mode = TDE_ALU_NONE;
    tde_base_opt_mode base_mode = TDE_SINGLE_SRC_PATTERN_FILL_OPT;
    drv_tde_double_src double_src = {0};

    if (opt != TD_NULL) {
        if (opt->out_alpha_from >= DRV_TDE_OUT_ALPHA_FROM_MAX) {
            tde_error("out_alpha_from error!\n");
            return DRV_ERR_TDE_INVALID_PARA;
        }

        if (opt->out_alpha_from == DRV_TDE_OUT_ALPHA_FROM_BACKGROUND) {
            tde_error("Single src pattern fill doesn't support out alpha form background!\n");
            return DRV_ERR_TDE_INVALID_PARA;
        }

        ret = tde_osi_pattern_fill_opt(hw_node, opt, &alu_mode, is_check_single_src2rop, is_check_blend);
        if (ret != TD_SUCCESS) {
            return ret;
        }

        double_src.bg_surface = TD_NULL;
        double_src.bg_rect = TD_NULL;
        double_src.fg_surface = single_src->src_surface;
        double_src.fg_rect = single_src->src_rect;
        double_src.dst_surface = single_src->dst_surface;
        double_src.dst_rect = single_src->dst_rect;
        if (tde_osi_set_pattern_clip_para(&double_src, opt, hw_node) != TD_SUCCESS) {
            return DRV_ERR_TDE_CLIP_AREA;
        }
    }

    if (tde_hal_node_set_base_operate(hw_node, base_mode, alu_mode, TD_NULL) != TD_SUCCESS) {
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    return TD_SUCCESS;
}

static td_s32 tde_osi_single_pattern_fill_set_up(tde_hw_node *hw_node, const drv_tde_single_src *single_src,
                                                 const drv_tde_pattern_fill_opt *opt)
{
    td_s32 ret;
    tde_scandirection_mode src_scan_info = {0};
    tde_scandirection_mode dst_scan_info = {0};
    tde_surface_msg src_drv_surface = {0};
    tde_surface_msg dst_drv_surface = {0};
    td_u16 code;
    tde_conv_mode_cmd conv = { 0 };
    tde_clut_usage clut_usage = TDE_CLUT_USAGE_BUTT;

    /* set ext alpha */
    tde_osi_set_ext_alpha(TD_NULL, single_src->src_surface, hw_node);
    /* set src2 and tqt */
    src_scan_info.hor_scan = TDE_SCAN_LEFT_RIGHT;
    src_scan_info.ver_scan = TDE_SCAN_UP_DOWN;
    dst_scan_info.hor_scan = TDE_SCAN_LEFT_RIGHT;
    dst_scan_info.ver_scan = TDE_SCAN_UP_DOWN;

    tde_osi_convert_surface(single_src->src_surface, single_src->src_rect, &src_scan_info, &src_drv_surface);

    tde_hal_node_set_src2(hw_node, &src_drv_surface);

    tde_osi_convert_surface(single_src->dst_surface, single_src->dst_rect, &dst_scan_info, &dst_drv_surface);

    (opt != TD_NULL) ? tde_hal_node_set_tqt(hw_node, &dst_drv_surface, opt->out_alpha_from) :
        tde_hal_node_set_tqt(hw_node, &dst_drv_surface, DRV_TDE_OUT_ALPHA_FROM_FOREGROUND);
    /* set color */
    code = tde_osi_single_src2_get_opt_code(single_src->src_surface->color_format,
        single_src->dst_surface->color_format);

    tde_osi_get_conv_by_code(code, &conv);

    if (tde_hal_node_set_color_convert(hw_node, &conv) != TD_SUCCESS) {
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    /* set clut opt */
    ret = (opt != TD_NULL) ?
           tde_osi_set_clut_opt(single_src->src_surface, single_src->dst_surface, &clut_usage, opt->clut_reload,
                                hw_node) :
           tde_osi_set_clut_opt(single_src->src_surface, single_src->dst_surface, &clut_usage, TD_TRUE, hw_node);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    /* set csc */
    if (opt != TD_NULL) {
        ret = tde_set_node_csc(hw_node, opt->csc_opt);
        if (ret != TD_SUCCESS) {
            return ret;
        }
    }
    return TD_SUCCESS;
}

static td_s32 tde_osi_single_src_pattern_fill(td_s32 handle, const drv_tde_single_src *single_src,
                                              const drv_tde_pattern_fill_opt *opt)
{
    tde_hw_node *hw_node = TD_NULL;
    td_s32 ret;

    ret = tde_osi_check_single_src_pattern_fill_para(single_src, opt);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    if (tde_hal_node_init_nd(&hw_node) != TD_SUCCESS) {
        return DRV_ERR_TDE_NO_MEM;
    }

    /* deal with opt */
    ret = tde_osi_single_pattern_fill_opt(hw_node, single_src, opt);
    if (ret != TD_SUCCESS) {
        tde_hal_free_node_buf(hw_node);
        return ret;
    }

    /* set:
     * set ext alpha
     * set src2
     * set tqt
     * set color format
     * set clut opt
     * set csc
     */
    ret = tde_osi_single_pattern_fill_set_up(hw_node, single_src, opt);
    if (ret != TD_SUCCESS) {
        tde_hal_free_node_buf(hw_node);
        return ret;
    }

    /* set node finish */
    ret = tde_osi_set_node_finish(handle, hw_node, 0, TDE_NODE_SUBM_ALONE);
    if (ret != TD_SUCCESS) {
        tde_hal_free_node_buf(hw_node);
        return ret;
    }
    return TD_SUCCESS;
}

static td_s32 tde_osi_check_single_src_pattern_fill_para(const drv_tde_single_src *single_src,
                                                         const drv_tde_pattern_fill_opt *opt)
{
    td_bool is_null_ptr = ((single_src == TD_NULL) || (single_src->src_surface == TD_NULL) ||
                           ((single_src->src_rect == TD_NULL)) || (single_src->dst_surface == TD_NULL) ||
                           (single_src->dst_rect == TD_NULL));

    if (is_null_ptr) {
        return DRV_ERR_TDE_NULL_PTR;
    }

    if (tde_osi_check_single_src_pattern_opt(single_src->src_surface->color_format,
        single_src->dst_surface->color_format, opt) != TD_SUCCESS) {
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (tde_osi_check_overlap(single_src->src_surface, single_src->src_rect,
        single_src->dst_surface, single_src->dst_rect)) {
        tde_error("Surface overlap!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    return TD_SUCCESS;
}

/*
 * Function:      tde_osi_double_src_pattern_fill
 * Description:   dual resource pattern fill
 * Input:         opt: operate option
 * Return:        success/fail
 */
static td_s32 tde_osi_double_pattern_fill_opt(tde_hw_node *hw_node, const drv_tde_double_src *double_src,
                                              const drv_tde_pattern_fill_opt *opt)
{
    td_bool is_check_single_src2rop = TD_FALSE;
    td_bool is_check_blend = TD_TRUE;
    tde_alu_mode alu_mode = TDE_ALU_NONE;
    td_s32 ret;
    tde_base_opt_mode base_mode = TDE_DOUBLE_SRC_PATTERN_FILL_OPT;

    ret = tde_osi_pattern_fill_opt(hw_node, opt, &alu_mode, is_check_single_src2rop, is_check_blend);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    if (tde_osi_set_pattern_clip_para(double_src, opt, hw_node) != TD_SUCCESS) {
        return DRV_ERR_TDE_CLIP_AREA;
    }

    if (tde_hal_node_set_base_operate(hw_node, base_mode, alu_mode, TD_NULL) != TD_SUCCESS) {
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    return TD_SUCCESS;
}

static td_void tde_osi_get_scan_info(tde_scandirection_mode *src_scan_info, tde_scandirection_mode *dst_scan_info)
{
    src_scan_info->hor_scan = TDE_SCAN_LEFT_RIGHT;
    src_scan_info->ver_scan = TDE_SCAN_UP_DOWN;
    dst_scan_info->hor_scan = TDE_SCAN_LEFT_RIGHT;
    dst_scan_info->ver_scan = TDE_SCAN_UP_DOWN;
}

static td_s32 tde_osi_double_pattern_fill_set_up(tde_hw_node *hw_node, const drv_tde_double_src *double_src,
                                                 const drv_tde_pattern_fill_opt *opt)
{
    tde_scandirection_mode src_scan_info = {0};
    tde_scandirection_mode dst_scan_info = {0};
    tde_surface_msg src_drv_surface = {0};
    tde_surface_msg dst_drv_surface = {0};
    td_u16 code;
    tde_conv_mode_cmd conv = { 0 };
    tde_clut_usage clut_usage = TDE_CLUT_USAGE_BUTT;
    td_s32 ret;

    tde_osi_set_ext_alpha(double_src->bg_surface, double_src->fg_surface, hw_node);

    tde_osi_get_scan_info(&src_scan_info, &dst_scan_info);

    tde_osi_convert_surface(double_src->bg_surface, double_src->bg_rect, &src_scan_info, &src_drv_surface);

    tde_hal_node_set_src1(hw_node, &src_drv_surface);

    tde_osi_convert_surface(double_src->fg_surface, double_src->fg_rect, &src_scan_info, &src_drv_surface);

    tde_hal_node_set_src2(hw_node, &src_drv_surface);

    tde_osi_convert_surface(double_src->dst_surface, double_src->dst_rect, &dst_scan_info, &dst_drv_surface);

    tde_hal_node_set_tqt(hw_node, &dst_drv_surface, opt->out_alpha_from);

    code = tde_osi_double_src_get_opt_code(double_src->bg_surface->color_format,
        double_src->fg_surface->color_format, double_src->dst_surface->color_format);

    tde_osi_get_conv_by_code(code, &conv);

    if (tde_hal_node_set_color_convert(hw_node, &conv) != TD_SUCCESS) {
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    ret = tde_osi_set_clut_opt(double_src->fg_surface, double_src->bg_surface, &clut_usage, opt->clut_reload, hw_node);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    if ((opt->colorkey_mode >= DRV_TDE_COLOR_KEY_MODE_MAX) || (opt->colorkey_mode < DRV_TDE_COLOR_KEY_MODE_NONE)) {
        tde_error("color_key_mode error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    ret = tde_osi_set_color_key(double_src, hw_node, opt->colorkey_value, opt->colorkey_mode, clut_usage);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = tde_set_node_csc(hw_node, opt->csc_opt);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    return TD_SUCCESS;
}

static td_s32 tde_osi_double_src_pattern_fill(td_s32 handle, const drv_tde_double_src *double_src,
                                              const drv_tde_pattern_fill_opt *opt)
{
    tde_hw_node *hw_node = TD_NULL;
    td_s32 ret;

    ret = tde_osi_check_double_src_pattern_fill_para(double_src, opt);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    if (tde_hal_node_init_nd(&hw_node) != TD_SUCCESS) {
        return DRV_ERR_TDE_NO_MEM;
    }

    /* deal with opt */
    ret = tde_osi_double_pattern_fill_opt(hw_node, double_src, opt);
    if (ret != TD_SUCCESS) {
        tde_hal_free_node_buf(hw_node);
        return ret;
    }

    ret = tde_osi_double_pattern_fill_set_up(hw_node, double_src, opt);
    if (ret != TD_SUCCESS) {
        tde_hal_free_node_buf(hw_node);
        return ret;
    }

    ret = tde_osi_set_node_finish(handle, hw_node, 0, TDE_NODE_SUBM_ALONE);
    if (ret != TD_SUCCESS) {
        tde_hal_free_node_buf(hw_node);
        return ret;
    }

    return TD_SUCCESS;
}

static td_s32 tde_osi_check_double_src_pattern_fill_para(const drv_tde_double_src *double_src,
                                                         const drv_tde_pattern_fill_opt *opt)
{
    td_bool is_contain_null_ptr = ((double_src == TD_NULL) || (opt == TD_NULL) ||
                                   (double_src->bg_surface == TD_NULL) || (double_src->bg_rect == TD_NULL) ||
                                   (double_src->fg_surface == TD_NULL) || (double_src->fg_rect == TD_NULL) ||
                                   (double_src->dst_surface == TD_NULL) || (double_src->dst_rect == TD_NULL));

    if (is_contain_null_ptr) {
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (tde_osi_check_double_src_pattern_opt(double_src->bg_surface->color_format,
        double_src->fg_surface->color_format, double_src->dst_surface->color_format, opt) != TD_SUCCESS) {
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (tde_osi_check_overlap(double_src->fg_surface, double_src->fg_rect, double_src->bg_surface,
        double_src->bg_rect) ||
        tde_osi_check_overlap(double_src->fg_surface, double_src->fg_rect, double_src->dst_surface,
        double_src->dst_rect)) {
        tde_error("Surface overlap!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    return TD_SUCCESS;
}

static td_s32 tde_osi_set_blend(tde_hw_node *hw_node, drv_tde_alpha_blending alpha_blending_cmd,
                                drv_tde_blend_opt blend_opt,
                                tde_alu_mode *alu_mode, td_bool check_blend)
{
    td_bool set_blend = ((td_u32)alpha_blending_cmd & DRV_TDE_ALPHA_BLENDING_BLEND) ? TD_TRUE : TD_FALSE;
    td_bool unknown_blend_mode = (blend_opt.blend_cmd == DRV_TDE_BLEND_CMD_CONFIG) &&
                                 ((blend_opt.src1_blend_mode >= DRV_TDE_BLEND_MAX) ||
                                 (blend_opt.src2_blend_mode >= DRV_TDE_BLEND_MAX));
#if (TDE_CAPABILITY & ROP)
    td_bool enable_alpha_rop = ((td_u32)alpha_blending_cmd & DRV_TDE_ALPHA_BLENDING_ROP) ? TD_TRUE : TD_FALSE;
#endif
    td_bool b_unknown_blend_cmd = (blend_opt.blend_cmd >= DRV_TDE_BLEND_CMD_MAX);

    if (!set_blend) {
        return TD_SUCCESS;
    }

    *alu_mode = TDE_ALU_BLEND;

    if (check_blend) {
        if (b_unknown_blend_cmd) {
            tde_error("Unknown blend cmd!\n");

            return DRV_ERR_TDE_INVALID_PARA;
        }

        if (unknown_blend_mode) {
            tde_error("Unknown blend mode!\n");

            return DRV_ERR_TDE_INVALID_PARA;
        }
    }

    if (tde_hal_node_set_blend(hw_node, &blend_opt) != TD_SUCCESS) {
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }
#if (TDE_CAPABILITY & ROP)

    if (enable_alpha_rop) {
        tde_hal_node_enable_alpha_rop(hw_node);
    }
#endif
    return TD_SUCCESS;
}

#if (TDE_CAPABILITY & COLORIZE)
static td_s32 tde_osi_set_colorize(tde_hw_node *hw_node, drv_tde_alpha_blending alpha_blending_cmd,
    td_s32 color_resize)
{
    td_bool set_colorize = ((td_u32)alpha_blending_cmd & DRV_TDE_ALPHA_BLENDING_COLORIZE) ? TD_TRUE : TD_FALSE;

    if (!set_colorize) {
        return TD_SUCCESS;
    }

    if (tde_hal_node_set_colorize(hw_node, color_resize) != TD_SUCCESS) {
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    return TD_SUCCESS;
}
#endif

#if (TDE_CAPABILITY & ROP)
static td_s32 tde_osi_set_rop(tde_hw_node *hw_node, const tde_rop_opt *rop_opt, tde_alu_mode *alu_mode)
{
    td_bool set_rop = ((td_u32)rop_opt->alpha_blending_cmd & DRV_TDE_ALPHA_BLENDING_ROP) ? TD_TRUE : TD_FALSE;
    td_bool error_rop_code = (rop_opt->rop_code_color >= DRV_TDE_ROP_MAX) ||
                              (rop_opt->rop_code_alpha >= DRV_TDE_ROP_MAX);
    td_bool only_support_single_sr2_rop = ((!tde_osi_is_single_src_to_rop(rop_opt->rop_code_alpha)) ||
                                           (!tde_osi_is_single_src_to_rop(rop_opt->rop_code_color)));

    if (!set_rop) {
        return TD_SUCCESS;
    }

    *alu_mode = TDE_ALU_ROP;

    if (error_rop_code) {
        tde_error("enRopCode error!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    if (rop_opt->single_sr2_rop) {
        if (only_support_single_sr2_rop) {
            tde_error("Only support single s2 rop!\n");
            return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
        }
    }

    if (tde_hal_node_set_rop(hw_node, rop_opt->rop_code_color, rop_opt->rop_code_alpha) != TD_SUCCESS) {
        return DRV_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    return TD_SUCCESS;
}
#endif

/*
 * Function:      tde_osi_pattern_fill
 * Description:   pattern fill
 * Input:         handle:task handle
                  opt: operate option
 * Return:        success/fail
 */
td_s32 tde_osi_pattern_fill(td_s32 handle, const drv_tde_double_src *double_src, const drv_tde_pattern_fill_opt *opt)
{
    tde_pattern_operation_category opt_category_en;
    drv_tde_single_src single_src = {0};

    opt_category_en = tde_osi_get_pattern_opt_category(double_src, opt);
    switch (opt_category_en) {
        case TDE_PATTERN_OPERATION_SINGLE_SRC:
            if (double_src->bg_surface != TD_NULL) {
                single_src.src_surface = double_src->bg_surface;
                single_src.src_rect = double_src->bg_rect;
                single_src.dst_surface = double_src->dst_surface;
                single_src.dst_rect = double_src->dst_rect;
            } else {
                single_src.src_surface = double_src->fg_surface;
                single_src.src_rect = double_src->fg_rect;
                single_src.dst_surface = double_src->dst_surface;
                single_src.dst_rect = double_src->dst_rect;
            }
            return tde_osi_single_src_pattern_fill(handle, &single_src, opt);
        case TDE_PATTERN_OPERATION_DOUBLE_SRC:
            return tde_osi_double_src_pattern_fill(handle, double_src, opt);
        default:
            return DRV_ERR_TDE_INVALID_PARA;
    }
}

/*
 * Function:      tde_cal_scale_rect
 * Description:   update zoom rect information
 * Input:         src_rect:source bitmap operate zone
                  dst_rect: target bitmap operate zone
 *                rect_in_src: source bitmap scale zone
 *                rect_in_dst: target bitmap info atfer calculating
 * Return:        success/fail
 */
td_s32 tde_cal_scale_rect(const drv_tde_rect *src_rect, const drv_tde_rect *dst_rect,
                          drv_tde_rect *rect_in_src, drv_tde_rect *rect_in_dst)
{
#if (TDE_CAPABILITY & RESIZE)
    tde_update_config reg = { 0 };
    tde_update_info info = { 0 };

    if (src_rect == TD_NULL || dst_rect == TD_NULL || rect_in_src == TD_NULL || rect_in_dst == TD_NULL) {
        return DRV_ERR_TDE_NULL_PTR;
    }
    reg.ori_in_height = src_rect->height;
    reg.ori_in_width = src_rect->width;
    reg.zme_out_height = dst_rect->height;
    reg.zme_out_width = dst_rect->width;

    reg.update_instart_w = rect_in_src->pos_x;
    reg.update_instart_h = rect_in_src->pos_y;
    reg.update_in_width = rect_in_src->width;
    reg.update_in_height = rect_in_src->height;

    tde_osi_get_hupdate_info(&reg, &info, TD_TRUE);
    tde_osi_get_vupdate_info(&reg, &info, TD_TRUE);

    rect_in_src->pos_x = info.zme_instart_w;
    rect_in_src->pos_y = info.zme_instart_h;
    rect_in_src->width = info.zme_in_width;
    rect_in_src->height = info.zme_in_height;

    rect_in_dst->pos_x = info.zme_outstart_w;
    rect_in_dst->pos_y = info.zme_outstart_h;
    rect_in_dst->width = info.zme_out_width;
    rect_in_dst->height = info.zme_out_height;
#endif
    return TD_SUCCESS;
}

td_s32 tde_osi_enable_region_deflicker(td_bool is_region_deflicker)
{
#if (TDE_CAPABILITY & DEFLICKER)
    if ((is_region_deflicker != TD_TRUE) && (is_region_deflicker != TD_FALSE)) {
        tde_error("bool should be TRUE or FALSE!!\n");
        return DRV_ERR_TDE_INVALID_PARA;
    }

    g_region_deflicker = is_region_deflicker;
#endif
    return TD_SUCCESS;
}

#ifdef CONFIG_TDE_BLIT_EX
td_s32 tde_osi_single_blit_ex(td_s32 handle, drv_tde_surface *src_surface, drv_tde_rect *src_rect,
                              drv_tde_surface *dst_surface, drv_tde_rect *dst_rect,
                              drv_tde_opt *opt, td_bool mmz_for_src, td_bool mmz_for_dst)
{
    td_s32 ret;
    drv_tde_single_src single_src;

    ret = tde_osi_check_surface(src_surface, src_rect);
    if (ret < 0) {
        return ret;
    }
    ret = tde_osi_check_surface(dst_surface, dst_rect);
    if (ret < 0) {
        return ret;
    }

    single_src.src_surface = src_surface;
    single_src.src_rect    = src_rect;
    single_src.dst_surface = dst_surface;
    single_src.dst_rect    = dst_rect;
    return tde_osi_single_src_2_blit(handle, &single_src, opt, mmz_for_src, mmz_for_dst);
}
#endif

#endif /* TDE_OSICTL_K_BODY_INC */
