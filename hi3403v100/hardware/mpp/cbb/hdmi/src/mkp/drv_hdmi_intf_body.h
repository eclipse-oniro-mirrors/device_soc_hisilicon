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

#ifndef DRV_HDMI_INTF_BODY_INC
#define DRV_HDMI_INTF_BODY_INC

#line 1 "drv_hdmi_intf.c"
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

#include "drv_hdmi_intf.h"
#include <linux/kthread.h>
#include "hdmi_hal.h"
#include "hdmi_product_define.h"
#include "drv_hdmi_intf_k.h"
#include "drv_hdmi_edid.h"
#include "drv_hdmi_event.h"
#include "drv_hdmi_ioctl.h"
#include "drv_hdmi_debug.h"
#include "drv_hdmi_compatibility.h"
#include "dev_ext.h"
#ifdef CONFIG_OT_PROC_SHOW_SUPPORT
#include "drv_hdmi_proc.h"
#endif

#define hdmi_multiple_2p0(x)         ((x) *= 2)
#define hdmi_multiple_1p5(x)         ((x) = ((x) * 3) >> 1)
#define hdmi_multiple_1p25(x)        ((x) = ((x) * 5) >> 2)
#define hdmi_multiple_0p5(x)         ((x) = (x) >> 1)
#define MAX_DELAY_TIME_MS            10000
#define SCDC_ENABLE_TMDS_CHR_RATE    (600 * 1000000)
#define SCDC_DESABLE_TMDS_CHR_RATE   (300 * 1000000)
#define SCDC_SCRAMBLE_INTERVAL_RESET 20
#define SCDC_SCRAMBLE_TIMEOUT_RESET  200
#define DEV_HPD_ASSERT_WAIT_TIME     50
#define HDMI_THREAD_STATE_WAIT_TIME  90
#define HDMI_THREAD_DELAY            10
#define HDMI_DEV_FMT_DELAY           500
#define HDMI_DEV_MUTE_DELAY          120
#define HDMI_VIDEO_ATTR_CLK_FS       74250
#define ATTR_LOCK_WAIT_TIME          2
#define ATTR_LOCK_WAIT_TIMEOUT       5
#define HW_PARAM_ARRAY_COUNT         4
#define HDMI_READ_HPD_STATUS_DELAY   6
#define HDMI_READ_HPD_STATUS_DELAY_TIME 20
#define PROC_NAME_MAX                50
#define DEVFS_NAME_LEN               20
#define HDMI_VIC_4K24                4
#define EDID_UPDATA_CNT_MAX          5
#define THREAD_NAME_MAX              20
#define HDMI_14_MAX_TMDS_CLK (300 * 1000)
#define HDMI_TMDS_CLK_MIN            25000

static td_void hdmi_exit(td_void);
static td_s32 hdmi_init(td_void *args);
static td_u32 hdmi_get_ver_magic(td_void);
static td_void hdmi_notify(mod_notice_id notice);
static td_s32 hdmi_file_open(void *private_data);
static td_void hdmi_query_state(mod_state *state);
static td_s32 hdmi_file_close(void *private_data);
static long hdmi_file_ioctl(unsigned int cmd, unsigned long arg, void *private_data);

static td_char *g_hdmi_reg[HDMI_ID_MAX] = {TD_NULL};
static td_char *g_hdmi_phy[HDMI_ID_MAX] = {TD_NULL};
static osal_semaphore_t g_hdmi_mutex;
static osal_dev_t *g_hdmi_device = TD_NULL;
static osal_atomic_t g_hdmi_count = OSAL_ATOMIC_INIT(0);
static hdmi_device g_hdmi_ctrl[HDMI_ID_MAX];

static hdmi_export_func g_hdmi_export_funcs = {
    .pfn_stop = drv_hdmi_stop_export,
    .pfn_csc_param_set = drv_hdmi_csc_param_set_export,
    .pfn_video_param_set = drv_hdmi_video_param_set_export,
};

static umap_module g_module = {
    .export_funcs = &g_hdmi_export_funcs,
    .mod_id = OT_ID_HDMI,
    .mod_name = "hdmi",
    .pfn_init = hdmi_init,
    .pfn_exit = hdmi_exit,
    .pfn_query_state = hdmi_query_state,
    .pfn_notify = hdmi_notify,
    .pfn_ver_checker = hdmi_get_ver_magic,
    .data = TD_NULL,
};

static struct osal_fileops g_hdmi_file_ops = {
    .open = hdmi_file_open,
    .unlocked_ioctl = hdmi_file_ioctl,
    .release = hdmi_file_close,
#ifdef CONFIG_COMPAT
    .compat_ioctl = hdmi_file_ioctl,
#endif
};

#ifdef CONFIG_OT_PROC_SHOW_SUPPORT
static hdmi_proc_item g_hdmi_proc_item = {
    .read = drv_hdmi_proc_show,
#ifdef HDMI_DEBUG_SUPPORT
    .write = drv_hdmi_debug_source_write,
#else
    .write = TD_NULL,
#endif
};

static hdmi_proc_item g_vo_proc_item = {
    .read = drv_hdmi_vo_proc_show,
    .write = TD_NULL,
};

static hdmi_proc_item g_ao_proc_item = {
    .read = drv_hdmi_ao_proc_show,
    .write = TD_NULL,
};

static hdmi_proc_item g_sink_proc_item = {
    .read = drv_hdmi_sink_proc_show,
    .write = TD_NULL,
};
#endif

static td_void drv_hdmi_black_data_set(const hdmi_device *hdmi_dev, td_bool enable)
{
    hdmi_black_frame_info black_info = {0};

#if defined(HDMI_PRODUCT_SS626V100)
    /* SS626V100 8K30 yuv420 solution does not support black frames. */
    if (hdmi_dev->attr.vo_attr.video_timing == HDMI_VIDEO_TIMING_7680X4320P_30000) {
        return;
    }
#endif
    black_info.black_enable = enable;
    black_info.in_bit_depth = hdmi_dev->attr.vo_attr.in_bit_depth;
    black_info.in_color_space = hdmi_dev->attr.vo_attr.in_color_space;
    black_info.in_quantization = (black_info.in_color_space == HDMI_COLORSPACE_RGB) ?
        hdmi_dev->attr.vo_attr.rgb_quantization : hdmi_dev->attr.vo_attr.ycc_quantization;
    hal_call_void(hdmi_dev, hal_hdmi_black_data_set, &black_info);

    return;
}

static td_s32 hdmi_capability_inter_section(hdmi_sink_capability *dest_cap,
                                            const hdmi_tx_capability_data *tx_cap, td_bool auth_mode)
{
    dest_cap->support_hdmi = dest_cap->support_hdmi && tx_cap->tx_hdmi_14;
    dest_cap->support_hdmi_20 = dest_cap->support_hdmi_20 && tx_cap->tx_hdmi_20;
    dest_cap->support_scdc = dest_cap->support_scdc && tx_cap->tx_scdc;
    dest_cap->color_space.rgb444 = dest_cap->color_space.rgb444 && tx_cap->tx_rgb444;
    dest_cap->color_space.ycbcr444 = dest_cap->color_space.ycbcr444 && tx_cap->tx_ycbcr444;
    dest_cap->color_space.ycbcr422 = dest_cap->color_space.ycbcr422 && tx_cap->tx_ycbcr422;
    dest_cap->color_space.ycbcr420 = (dest_cap->color_space.ycbcr420 && tx_cap->tx_ycbcr420) || auth_mode;
    dest_cap->deep_color.deep_color30_bit = dest_cap->deep_color.deep_color30_bit && tx_cap->tx_deep_clr10_bit;
    dest_cap->deep_color.deep_color36_bit = dest_cap->deep_color.deep_color36_bit && tx_cap->tx_deep_clr12_bit;
    dest_cap->deep_color.deep_color48_bit = dest_cap->deep_color.deep_color48_bit && tx_cap->tx_deep_clr16_bit;
    dest_cap->deep_color_y420.deep_color30_bit = (dest_cap->deep_color_y420.deep_color30_bit &&
        (tx_cap->tx_ycbcr420 && tx_cap->tx_deep_clr10_bit)) || auth_mode;
    dest_cap->deep_color_y420.deep_color36_bit = (dest_cap->deep_color_y420.deep_color36_bit &&
        (tx_cap->tx_ycbcr420 && tx_cap->tx_deep_clr12_bit)) || auth_mode;
    dest_cap->deep_color_y420.deep_color48_bit = (dest_cap->deep_color_y420.deep_color48_bit &&
        (tx_cap->tx_ycbcr420 && tx_cap->tx_deep_clr16_bit)) || auth_mode;
    dest_cap->max_tmds_clock = (dest_cap->max_tmds_clock < tx_cap->tx_max_tmds_clk) ? dest_cap->max_tmds_clock :
        tx_cap->tx_max_tmds_clk;
    dest_cap->hdcp_support.hdcp14_support = dest_cap->hdcp_support.hdcp14_support && tx_cap->tx_hdcp_14;
    dest_cap->hdcp_support.hdcp22_support = dest_cap->hdcp_support.hdcp22_support && tx_cap->tx_hdcp_22;

    return TD_SUCCESS;
}

static td_void hdmi_phy_output_enable(const hdmi_device *hdmi_dev, td_bool phy_output_enable)
{
    hal_call_void(hdmi_dev, hal_hdmi_phy_output_enable_set, phy_output_enable);
    return;
}

#ifdef HDMI_SCDC_SUPPORT
static td_s32 hdmi_scdc_status_get(const hdmi_app_attr *app_attr, const hdmi_vo_attr *vo_attr,
                                   hdmi_scdc_status *scdc_status, hdmi_tmds_mode *tmds_mode)
{
    if (app_attr->enable_hdmi == TD_FALSE) {
        /* DVI mode */
        *tmds_mode = HDMI_TMDS_MODE_DVI;
        scdc_status->sink_scramble_on   = TD_FALSE;
        scdc_status->source_scramble_on = TD_FALSE;
        scdc_status->tmds_bit_clk_ratio = SCDC_TMDS_BIT_CLK_RATIO_10X;
        if ((vo_attr->hdmi_adapt_pix_clk > HDMI_EDID_MAX_HDMI14_TMDS_RATE) && (!app_attr->enable_clr_space_adapt)) {
            hdmi_err("tmds_clk=%u can't support in DVI mode. \n", vo_attr->hdmi_adapt_pix_clk);
            return TD_FAILURE;
        }
    } else if (vo_attr->hdmi_adapt_pix_clk > HDMI_EDID_MAX_HDMI14_TMDS_RATE) {
        *tmds_mode = HDMI_TMDS_MODE_HDMI_2_0;
        scdc_status->sink_scramble_on   = TD_TRUE;
        scdc_status->source_scramble_on = TD_TRUE;
        scdc_status->tmds_bit_clk_ratio = SCDC_TMDS_BIT_CLK_RATIO_40X;
    } else {
        *tmds_mode = HDMI_TMDS_MODE_HDMI_1_4;
        scdc_status->sink_scramble_on   = TD_FALSE;
        scdc_status->source_scramble_on = TD_FALSE;
        scdc_status->tmds_bit_clk_ratio = SCDC_TMDS_BIT_CLK_RATIO_10X;
    }

    return TD_SUCCESS;
}
#endif

static td_bool hdmi_ycbcr420_fmt_check(hdmi_device *hdmi_dev)
{
    td_u32 i;
    hdmi_video_code_vic vic;
    td_bool search_out = TD_FALSE;
    hdmi_vo_attr *vo_attr = TD_NULL;
    hdmi_sink_capability *sink_cap = TD_NULL;

    /* auth mode donot do cb_cr420_fmt_check and return TD_TRUE */
    if (hdmi_dev->attr.app_attr.auth_mode == TD_TRUE) {
        hdmi_info("auth_mode: %u \n", hdmi_dev->attr.app_attr.auth_mode);
        return TD_TRUE;
    }
    vo_attr = &hdmi_dev->attr.vo_attr;
    if (drv_hdmi_edid_capability_get(&hdmi_dev->edid_info, &sink_cap) == HDMI_EDID_DATA_INVALID) {
        hdmi_warn("get sink capability fail\n");
    }
    vic = drv_hdmi_vic_search(vo_attr->video_timing, vo_attr->picture_aspect, TD_FALSE);
    if (vic != 0) {
        for (i = 0; i < sink_cap->support_y420_vic_num && i < hdmi_array_size(sink_cap->support_y420_format); i++) {
            if (vic == sink_cap->support_y420_format[i]) {
                search_out = TD_TRUE;
                break;
            }
        }
        for (i = 0; (search_out == TD_FALSE) &&
             i < sink_cap->only_support_y420_vic_num &&
             i < hdmi_array_size(sink_cap->only_support_y420_format); i++) {
            if (vic == sink_cap->only_support_y420_format[i]) {
                search_out = TD_TRUE;
                break;
            }
        }
    }
    hdmi_info("is Y420 support vic=%u :%s\n", vic, search_out ? "YES" : "NO");

    return search_out;
}

static td_s32 hdmi_color_space_check(hdmi_device *hdmi_dev, const hdmi_app_attr *user_app)
{
    hdmi_tx_capability_data tx_cap = {0};
    td_bool support_clr_space = TD_FALSE;
    hdmi_sink_capability *sink_cap = TD_NULL;

    hal_call_void(hdmi_dev, hal_hdmi_tx_capability_get, &tx_cap);
    if (drv_hdmi_edid_capability_get(&hdmi_dev->edid_info, &sink_cap) == HDMI_EDID_DATA_INVALID) {
        hdmi_warn("get sink capability fail\n");
    }

    switch (user_app->out_color_space) {
        case HDMI_COLORSPACE_RGB:
            support_clr_space = TD_TRUE;
            if ((sink_cap->color_space.rgb444 && tx_cap.tx_rgb444) == TD_FALSE) {
                hdmi_warn("sink or source not support RGB!\n");
            }
            break;
        case HDMI_COLORSPACE_YCBCR422:
            support_clr_space = sink_cap->color_space.ycbcr422 && tx_cap.tx_ycbcr422;
            break;
        case HDMI_COLORSPACE_YCBCR444:
            support_clr_space = sink_cap->color_space.ycbcr444 && tx_cap.tx_ycbcr444;
            break;
        case HDMI_COLORSPACE_YCBCR420:
            support_clr_space = tx_cap.tx_ycbcr420 && hdmi_ycbcr420_fmt_check(hdmi_dev);
            break;
        default:
            hdmi_err("un-know color_space=%u!\n", user_app->out_color_space);
            return TD_FAILURE;
            break;
    }

    if (support_clr_space == TD_FALSE) {
        hdmi_err("fail, not support color space:%u\n", user_app->out_color_space);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

static td_void check_deep_color_10bit(const hdmi_sink_capability *sink_cap, const hdmi_app_attr *user_app,
    const hdmi_tx_capability_data *tx_cap, td_u32 *fmt_pix_clk, td_bool *support_deep_clr)
{
    td_u32 pix_clk;

    hdmi_if_null_return_void(sink_cap);

    pix_clk = *fmt_pix_clk;

    switch (user_app->out_color_space) {
        case HDMI_COLORSPACE_RGB:
            hdmi_multiple_1p25(pix_clk);
            *support_deep_clr = (sink_cap->deep_color.deep_color30_bit &&
                tx_cap->tx_deep_clr10_bit) ? TD_TRUE : TD_FALSE;
            break;
        case HDMI_COLORSPACE_YCBCR444:
            hdmi_multiple_1p25(pix_clk);
            *support_deep_clr = (sink_cap->deep_color.deep_color30_bit && tx_cap->tx_deep_clr10_bit &&
                sink_cap->deep_color.deep_color_y444) ? TD_TRUE : TD_FALSE;
            break;
        case HDMI_COLORSPACE_YCBCR420:
            hdmi_multiple_1p25(pix_clk);
            *support_deep_clr = (sink_cap->deep_color_y420.deep_color30_bit &&
                tx_cap->tx_deep_clr10_bit) ? TD_TRUE : TD_FALSE;
            break;
        default:
            /* Y422, ignore deepclr */
            *support_deep_clr = TD_TRUE;
            break;
    }
    *fmt_pix_clk = pix_clk;

    return;
}

static td_void check_deep_color_12bit(const hdmi_sink_capability *sink_cap, const hdmi_app_attr *user_app,
    const hdmi_tx_capability_data *tx_cap, td_u32 *fmt_pix_clk, td_bool *support_deep_clr)
{
    td_u32 pix_clk;

    hdmi_if_null_return_void(sink_cap);

    pix_clk = *fmt_pix_clk;

    switch (user_app->out_color_space) {
        case HDMI_COLORSPACE_RGB:
            hdmi_multiple_1p5(pix_clk);
            *support_deep_clr = (sink_cap->deep_color.deep_color36_bit &&
                tx_cap->tx_deep_clr12_bit) ? TD_TRUE : TD_FALSE;
            break;
        case HDMI_COLORSPACE_YCBCR444:
            hdmi_multiple_1p5(pix_clk);
            *support_deep_clr = (sink_cap->deep_color.deep_color36_bit && tx_cap->tx_deep_clr12_bit &&
                sink_cap->deep_color.deep_color_y444) ? TD_TRUE : TD_FALSE;
            break;
        case HDMI_COLORSPACE_YCBCR420:
            hdmi_multiple_1p5(pix_clk);
            *support_deep_clr = (sink_cap->deep_color_y420.deep_color36_bit &&
                tx_cap->tx_deep_clr12_bit) ? TD_TRUE : TD_FALSE;
            break;
        default:
            /* Y422, ignore deepclr */
            *support_deep_clr = TD_TRUE;
            break;
    }
    *fmt_pix_clk = pix_clk;

    return;
}

static td_u32 hdmi_current_id_get(const hdmi_device *hdmi_dev)
{
    ot_unused(hdmi_dev);
    return hdmi_get_current_id();
}

static td_void check_deep_color(const hdmi_sink_capability *sink_cap, const hdmi_app_attr *user_app,
    const hdmi_tx_capability_data *tx_cap, td_u32 *fmt_pix_clk, td_bool *support_deep_clr)
{
    switch (user_app->deep_color_mode) {
        case HDMI_DEEP_COLOR_30BIT:
            check_deep_color_10bit(sink_cap, user_app, tx_cap, fmt_pix_clk, support_deep_clr);
            break;
        case HDMI_DEEP_COLOR_36BIT:
            check_deep_color_12bit(sink_cap, user_app, tx_cap, fmt_pix_clk, support_deep_clr);
            break;
        default:
            *support_deep_clr = TD_TRUE;
            break;
    }

    return;
}

static td_s32 hdmi_deep_color_check(hdmi_device *hdmi_dev, hdmi_app_attr *user_app, td_u32 max_tmds_clk)
{
    td_u32 fmt_pix_clk;
    hdmi_vo_attr *vo_attr = TD_NULL;
    hdmi_sink_capability *sink_cap = TD_NULL;
    hdmi_tx_capability_data tx_cap = {0};
    td_bool adapt_to_y422;
    td_bool support_deep_clr = TD_FALSE;

    vo_attr = &hdmi_dev->attr.vo_attr;
    hal_call_void(hdmi_dev, hal_hdmi_tx_capability_get, &tx_cap);
    if (drv_hdmi_edid_capability_get(&hdmi_dev->edid_info, &sink_cap) == HDMI_EDID_DATA_INVALID) {
        hdmi_warn("get sink capability fail\n");
    }

    fmt_pix_clk = vo_attr->clk_fs;
    if (user_app->out_color_space == HDMI_COLORSPACE_YCBCR420) {
        hdmi_multiple_0p5(fmt_pix_clk);
    }
    check_deep_color(sink_cap, user_app, &tx_cap, &fmt_pix_clk, &support_deep_clr);

    if ((support_deep_clr == TD_TRUE) && (fmt_pix_clk < max_tmds_clk)) {
        vo_attr->hdmi_adapt_pix_clk = fmt_pix_clk;
        return TD_SUCCESS;
    } else {
        vo_attr->hdmi_adapt_pix_clk = vo_attr->clk_fs;
        if (user_app->out_color_space == HDMI_COLORSPACE_YCBCR420) {
            hdmi_multiple_0p5(vo_attr->hdmi_adapt_pix_clk);
        }

        adapt_to_y422 = ((user_app->out_color_space == HDMI_COLORSPACE_YCBCR444) &&
                         (user_app->enable_clr_space_adapt) &&
                         (user_app->deep_color_mode != HDMI_DEEP_COLOR_24BIT) &&
                         (user_app->deep_color_mode != HDMI_DEEP_COLOR_OFF) &&
                         (user_app->deep_color_mode != HDMI_DEEP_COLOR_48BIT) &&
                         (sink_cap->color_space.ycbcr422 && tx_cap.tx_ycbcr422));

        /* Y444 12/10bit */
        if (adapt_to_y422 == TD_TRUE) {
            hdmi_warn("fail, Y444 adapt to Y422!\n");
            user_app->out_color_space = HDMI_COLORSPACE_YCBCR422;
        } else if (user_app->enable_deep_clr_adapt == TD_TRUE) {
            hdmi_warn("fail, deepclr(%u) adapt to 8bit!\n", user_app->deep_color_mode);
            user_app->deep_color_mode = HDMI_DEEP_COLOR_24BIT;
        } else {
            hdmi_err("fail, adapt deepclr fail!\n");
            return TD_FAILURE;
        }

        return TD_SUCCESS;
    }
}

static td_s32 adapt_to_420(const hdmi_vo_attr *vo_attr, hdmi_app_attr *user_app, td_u32 max_tmds_clk)
{
    if (vo_attr->clk_fs <= max_tmds_clk) {
        return TD_SUCCESS;
    }

    hdmi_warn("pix_clk(%u) > max_tmds_clk(%u)\n", vo_attr->clk_fs, max_tmds_clk);
    /* in h8, hdmi can adapt to yuv420 just in yuv444 */
    if (user_app->out_color_space == HDMI_COLORSPACE_YCBCR444) {
        if (user_app->enable_clr_space_adapt == TD_TRUE) {
            hdmi_warn("out_color_space(%u) adapt to Y420.\n", user_app->out_color_space);
            user_app->out_color_space = HDMI_COLORSPACE_YCBCR420;
        } else {
            hdmi_err("adapt Y420 fail! enable_clr_space_adapt is false.\n");
            return TD_FAILURE;
        }
    }

    return TD_SUCCESS;
}

static td_s32 adapt_to_444(const hdmi_vo_attr *vo_attr, hdmi_app_attr *user_app)
{
    if (vo_attr->clk_fs > HDMI_14_MAX_TMDS_CLK) {
        return TD_SUCCESS;
    }

    if (user_app->out_color_space == HDMI_COLORSPACE_YCBCR420) {
        hdmi_warn("pix_clk(%u) <= HDMI_14_MAX_TMDS_CLK(%u)\n", vo_attr->clk_fs, HDMI_14_MAX_TMDS_CLK);
        if (user_app->enable_clr_space_adapt == TD_TRUE) {
            hdmi_warn("out_color_space(%u) adapt to Y444.\n", user_app->out_color_space);
            user_app->out_color_space = HDMI_COLORSPACE_YCBCR444;
        } else {
            hdmi_err("adapt Y444 fail! enable_clr_space_adapt is false.\n");
            return TD_FAILURE;
        }
    }

    return TD_SUCCESS;
}

static td_void edid_invalid_or_auth_mode(hdmi_device *hdmi_dev, hdmi_app_attr *user_app)
{
    hdmi_vo_attr *vo_attr = TD_NULL;

    vo_attr = &hdmi_dev->attr.vo_attr;
    vo_attr->hdmi_adapt_pix_clk =
        (user_app->out_color_space == HDMI_COLORSPACE_YCBCR420) ? (vo_attr->clk_fs >> 1) : vo_attr->clk_fs;

    if (user_app->out_color_space != HDMI_COLORSPACE_YCBCR422) {
        switch (user_app->deep_color_mode) {
            case HDMI_DEEP_COLOR_48BIT:
                hdmi_multiple_2p0(vo_attr->hdmi_adapt_pix_clk);
                break;
            case HDMI_DEEP_COLOR_36BIT:
                hdmi_multiple_1p5(vo_attr->hdmi_adapt_pix_clk);
                break;
            case HDMI_DEEP_COLOR_30BIT:
                hdmi_multiple_1p25(vo_attr->hdmi_adapt_pix_clk);
                break;
            default:
                break;
        }
    } else {
        if ((user_app->deep_color_mode != HDMI_DEEP_COLOR_OFF) &&
            (user_app->deep_color_mode != HDMI_DEEP_COLOR_24BIT)) {
            hdmi_info("Y422 foce deepcolor 8bit");
            user_app->deep_color_mode = HDMI_DEEP_COLOR_24BIT;
        }
        if (hdmi_dev->attr.vo_attr.video_timing == HDMI_VIDEO_TIMING_1440X480I_60000 ||
            hdmi_dev->attr.vo_attr.video_timing == HDMI_VIDEO_TIMING_1440X576I_50000) {
            hdmi_warn("Y422 is not support at pal and ntsc, force adapt to rgb!\n");
            user_app->out_color_space = HDMI_COLORSPACE_RGB;
        }
    }

    return;
}

static td_s32 dvi_color_and_bit_strategy(hdmi_device *hdmi_dev, hdmi_app_attr *user_app)
{
    hdmi_vo_attr *vo_attr = TD_NULL;

    vo_attr = &hdmi_dev->attr.vo_attr;
    if (user_app->out_color_space != HDMI_COLORSPACE_RGB) {
        hdmi_err("DVI mode, but the color space is not RGB!\n");
        return TD_FAILURE;
    }
    user_app->deep_color_mode = HDMI_DEEP_COLOR_OFF;
    vo_attr->hdmi_adapt_pix_clk = vo_attr->clk_fs;

    return TD_SUCCESS;
}

static td_s32 hdmi_deep_color_adapt(const hdmi_device *hdmi_dev, hdmi_app_attr *user_app)
{
    /* Y422 default 12bit output, deep_color force adapt to 8bit(24bit). */
    if (user_app->out_color_space == HDMI_COLORSPACE_YCBCR422) {
        if (hdmi_dev->attr.vo_attr.video_timing == HDMI_VIDEO_TIMING_1440X480I_60000 ||
            hdmi_dev->attr.vo_attr.video_timing == HDMI_VIDEO_TIMING_1440X576I_50000) {
            hdmi_err("Y422 is not support at pal and ntsc!\n");
            return TD_FAILURE;
        }
        if ((user_app->deep_color_mode != HDMI_DEEP_COLOR_24BIT) &&
            (user_app->deep_color_mode != HDMI_DEEP_COLOR_OFF)) {
            user_app->deep_color_mode = HDMI_DEEP_COLOR_OFF;
            hdmi_warn("when Y422, deep_color not support 10/12bit!\n");
        }
    }

    return TD_SUCCESS;
}

static td_s32 hdmi_color_and_bit_strategy(hdmi_device *hdmi_dev, hdmi_app_attr *user_app,
    const hdmi_vo_attr *vo_timing_attr)
{
    td_s32 ret;
    hdmi_edid_data edid_ret;
    td_u32 max_tmds_clk = 0;
    hdmi_vo_attr *vo_attr = TD_NULL;
    hdmi_sink_capability *sink_cap = TD_NULL;
    hdmi_tx_capability_data tx_cap = {0};
    ot_unused(vo_timing_attr);

    vo_attr = &hdmi_dev->attr.vo_attr;
    /* DVI mode, must set RGB & deep_color off */
    if (user_app->enable_hdmi == TD_FALSE) {
        return dvi_color_and_bit_strategy(hdmi_dev, user_app);
    }
    hal_call_void(hdmi_dev, hal_hdmi_tx_capability_get, &tx_cap);
    edid_ret = drv_hdmi_edid_capability_get(&hdmi_dev->edid_info, &sink_cap);
#if defined(HDMI_PRODUCT_SS626V100)
    if (edid_ret == HDMI_EDID_DATA_INVALID || (user_app->auth_mode == TD_TRUE)
        || (vo_timing_attr->disp_fmt == HDMI_VIDEO_FORMAT_7680X4320P_30))
#else
    if (edid_ret == HDMI_EDID_DATA_INVALID || user_app->auth_mode == TD_TRUE)
#endif
    {
        edid_invalid_or_auth_mode(hdmi_dev, user_app);
        /* get edid_capability fail means cannot do strategy, according to appatrr */
        hdmi_warn("get sink capability fail or auth_mode = %u, adapt hdmi_adapt_pix_clk = %u\n",
                  hdmi_dev->attr.app_attr.auth_mode, vo_attr->hdmi_adapt_pix_clk);
        return TD_SUCCESS;
    } else {
        /* get max_tmds_clk, in khz. */
        if (sink_cap->max_tmds_clock == 0) {
            /*
             * when the sink's max_tmds_clock is 0 Mhz, app can not set attr success.
             * then set max_tmds_clk = 300Mhz default.
             */
            max_tmds_clk = 300; /* 300, default clk */
        } else {
            max_tmds_clk =
                (sink_cap->max_tmds_clock < tx_cap.tx_max_tmds_clk) ? sink_cap->max_tmds_clock : tx_cap.tx_max_tmds_clk;
        }
        max_tmds_clk *= 1000; /* 1000, khz */
    }
    /* whether adapt to ycbcr420 */
    ret = adapt_to_420(vo_attr, user_app, max_tmds_clk);
    hdmi_if_failure_return(ret, TD_FAILURE);
    /* whether adapt to ycbcr444 */
    ret = adapt_to_444(vo_attr, user_app);
    hdmi_if_failure_return(ret, TD_FAILURE);
    /* color_space check & adapt */
    ret = hdmi_color_space_check(hdmi_dev, user_app);
    hdmi_if_failure_return(ret, TD_FAILURE);
    /* deep_color check & adapt */
    ret = hdmi_deep_color_check(hdmi_dev, user_app, max_tmds_clk);
    hdmi_if_failure_return(ret, TD_FAILURE);
    /* deep color adapt */
    ret = hdmi_deep_color_adapt(hdmi_dev, user_app);
    hdmi_if_failure_return(ret, TD_FAILURE);
    hdmi_info("clr_space_adapt=%u, deep_clr_adapt=%u, adptclrspace=%u, adptbit=%u, adptclk=%u, max_clk=%u\n",
              user_app->enable_clr_space_adapt, user_app->enable_deep_clr_adapt, user_app->out_color_space,
              user_app->deep_color_mode, vo_attr->hdmi_adapt_pix_clk, max_tmds_clk);

    return TD_SUCCESS;
}

static hdmi_deep_color depth_convert_to_deep_color(hdmi_video_bit_depth bit_depth)
{
    hdmi_deep_color deep_color;

    switch (bit_depth) {
        case HDMI_VIDEO_BITDEPTH_8:
            deep_color = HDMI_DEEP_COLOR_24BIT;
            break;
        case HDMI_VIDEO_BITDEPTH_10:
            deep_color = HDMI_DEEP_COLOR_30BIT;
            break;
        case HDMI_VIDEO_BITDEPTH_12:
            deep_color = HDMI_DEEP_COLOR_36BIT;
            break;
        case HDMI_VIDEO_BITDEPTH_16:
            deep_color = HDMI_DEEP_COLOR_48BIT;
            break;
        case HDMI_VIDEO_BITDEPTH_OFF:
            deep_color = HDMI_DEEP_COLOR_OFF;
            break;
        default:
            deep_color = HDMI_DEEP_COLOR_BUTT;
            break;
    }

    return deep_color;
}

static td_void hdmi_user_attr_construct(hdmi_device *hdmi_dev, hdmi_attr *attr, const hdmi_hardware_status *hw_status)
{
    hdmi_app_attr *app_attr = TD_NULL;

    app_attr = &attr->app_attr;
    hdmi_dev->tmds_mode = hw_status->common_status.tmds_mode;
    switch (hdmi_dev->tmds_mode) {
        case HDMI_TMDS_MODE_HDMI_1_4:
        case HDMI_TMDS_MODE_HDMI_2_0:
            app_attr->enable_hdmi = TD_TRUE;
            app_attr->enable_video = TD_TRUE;
            break;
        case HDMI_TMDS_MODE_DVI:
            app_attr->enable_hdmi = TD_FALSE;
            app_attr->enable_video = TD_TRUE;
            break;
        default:
            app_attr->enable_hdmi = TD_FALSE;
            app_attr->enable_video = TD_FALSE;
            break;
    }

    app_attr->enable_audio = (hw_status->audio_status.audio_enable && hw_status->info_frame_status.audio_enable);
    app_attr->out_color_space = hw_status->video_status.out_color_space;
    app_attr->deep_color_mode = depth_convert_to_deep_color(hw_status->video_status.out_bit_depth);
    app_attr->enable_avi_infoframe = hw_status->info_frame_status.avi_enable;
    app_attr->enable_aud_infoframe = hw_status->info_frame_status.audio_enable;
    app_attr->xvycc_mode = hw_status->info_frame_status.gbd_enable;

    return;
}

static td_void hdmi_video_attr_construct(hdmi_attr *attr, const hdmi_hardware_status *hw_status)
{
    hdmi_app_attr *app_attr = TD_NULL;
    hdmi_vo_attr *video_attr = TD_NULL;
    const hdmi_infoframe_status *status = TD_NULL;
    td_bool in_clr_is_rgb;
    td_bool aspect_is256;

    app_attr = &attr->app_attr;
    video_attr = &attr->vo_attr;
    status = &hw_status->info_frame_status;
    video_attr->in_color_space = app_attr->out_color_space;
    video_attr->v_sync_pol = hw_status->video_status.v_sync_pol;
    video_attr->h_sync_pol = hw_status->video_status.h_sync_pol;
    video_attr->de_pol = hw_status->video_status.de_pol;

    in_clr_is_rgb = ((hw_status->video_status.rgb2ycbcr) || ((hw_status->video_status.ycbcr2rgb == TD_FALSE) &&
                     (hw_status->video_status.out_color_space == HDMI_COLORSPACE_RGB)));
    video_attr->in_color_space = (in_clr_is_rgb == TD_TRUE) ? HDMI_COLORSPACE_RGB : HDMI_COLORSPACE_YCBCR444;
    if (status->avi_enable == TD_TRUE) {
        hdmi_video_code_vic video_code = status->avi[AVI_OFFSET_VIC];
        /*
         * when the timing is 4096*2160, the aspect ratio in AVI infoframe is 0
         * (but the real aspect ratio is 256:135<0x04>, the video_code is 0)
         */
        aspect_is256 = (((video_code == 0) &&
                        (status->vsif[VENDOR_OFFSET_VIC] == HDMI_VIC_4K24)) ||
                        ((video_code >= HDMI_4096X2160P25_256_135) && (video_code <= HDMI_4096X2160P60_256_135)));

        video_attr->picture_aspect = (aspect_is256 == TD_TRUE) ? HDMI_PICTURE_ASPECT_256_135 :
            ((status->avi[AVI_OFFSET_PB2] >> 4) & AVI_FRAME_PIC_ASPECT_MASK); /* 4, offset of byte. */
        video_attr->active_aspect = status->avi[AVI_OFFSET_PB2] & AVI_FRAME_ACTIVE_ASPECT_MASK;
        /* 6, offset of byte. */
        video_attr->colorimetry = (status->avi[AVI_OFFSET_PB2] >> 6) & AVI_FRAME_COLORIMETRY_MASK;
        /* 2, offset of byte. */
        video_attr->rgb_quantization = (status->avi[AVI_OFFSET_PB3] >> 2) & AVI_FRAME_QUANT_RANGE_MASK;
        /* 6, offset of byte. */
        video_attr->ycc_quantization = (status->avi[AVI_OFFSET_PB5] >> 6) & AVI_FRAME_YCC_QUANT_RANGE_MASK;
        video_attr->pixel_repeat = (status->avi[AVI_OFFSET_PB5] & AVI_FRAME_PIXEL_REPET_MASK) + 1;
        /* 4, offset of byte. */
        video_attr->extended_colorimetry = (status->avi[AVI_OFFSET_PB3] >> 4) & AVI_FRAME_EXT_COLORIMETRY_MASK;
        video_attr->video_timing = drv_hdmi_video_timing_lookup(video_code, video_attr->picture_aspect);
        hdmi_info("video_timing: %u, video_code: %u, picture_aspect: %u\n",
            video_attr->video_timing, video_code, video_attr->picture_aspect);
        if ((status->vsif_enable == TD_FALSE) && (!video_code)) {
            video_attr->video_timing = HDMI_VIDEO_TIMING_UNKNOWN;
        }
        app_attr->out_csc_quantization = (app_attr->out_color_space == HDMI_COLORSPACE_RGB) ?
            video_attr->rgb_quantization : (video_attr->ycc_quantization + 1);
    }
    video_attr->stereo_mode = HDMI_3D_BUTT;
    if (status->vsif_enable == TD_TRUE) {
        /* 5, offset of byte. */
        enum hdmi_video_format format = (status->vsif[VENDOR_OFFSET_VIDEO_FMT] >> 5) & VENDOR_FARAME_VIDEO_FMT_MASK;
        if (format == HDMI_VIDEO_FORMAT_4K) {
            hdmi_vsif_vic video_code = status->vsif[VENDOR_OFFSET_VIC];
            video_attr->video_timing = drv_hdmi_vsif_video_timing_lookup(video_code);
        } else if (format == HDMI_VIDEO_FORMAT_3D) {
            /* 4, offset of byte. */
            video_attr->stereo_mode = (status->vsif[VENDOR_OFFSET_VIC] >> 4) & VENDOR_3D_STRUCT_MASK;
        }
    }

    return;
}

static td_void hdmi_attr_construct(hdmi_device *hdmi_dev, hdmi_attr *attr)
{
    hdmi_ao_attr *audio_attr = TD_NULL;
    hdmi_hardware_status *status = TD_NULL;

    status = (hdmi_hardware_status *)osal_vmalloc(sizeof(hdmi_hardware_status));
    if (status == TD_NULL) {
        hdmi_err("alloc hdmi_hardware_status struct memory fail\n");
        return;
    }
    (td_void)memset_s(status, sizeof(hdmi_hardware_status), 0, sizeof(hdmi_hardware_status));

    audio_attr = &attr->ao_attr;
    hal_call_void(hdmi_dev, hal_hdmi_hardware_status_get, status);

    hdmi_user_attr_construct(hdmi_dev, attr, status);
    hdmi_video_attr_construct(attr, status);
    audio_attr->down_sample  = status->audio_status.down_sample;
    audio_attr->channels     = status->audio_status.layout;
    audio_attr->sample_depth = status->audio_status.sample_depth;
    audio_attr->sample_fs    = status->audio_status.sample_fs;
    audio_attr->sound_intf   = status->audio_status.sound_intf;
    if (status->info_frame_status.audio_enable) {
        /* 4, offset of byte. */
        audio_attr->audio_code = (status->info_frame_status.audio[AUDIO_OFFSET_PB1] >> 4) & AUDIO_FRAME_CODE_TYPE_MASK;
    }

    osal_vfree(status);
    status = TD_NULL;

    return;
}

static td_s32 hpd_event(hdmi_device *hdmi_dev)
{
    td_s32 ret = TD_SUCCESS;
    td_u32 timeout_cnt = EDID_UPDATA_CNT_MAX;

    /* update edid from sink */
    if (hdmi_dev->hpd_detected == TD_FALSE) {
        drv_hdmi_edid_reset(&hdmi_dev->edid_info);
        do {
            /*
             * 1.for some TV, EDID need to read more times;
             * 2.for some repeater, EDID is readable after HPD assert 250ms.
             */
            ret = drv_hdmi_edid_update(&hdmi_dev->edid_info, HDMI_EDID_UPDATE_SINK);
            if (ret == TD_SUCCESS) {
                hdmi_dev->hpd_detected = TD_TRUE;
                break;
            }
            osal_msleep(DEV_HPD_ASSERT_WAIT_TIME);
        } while (timeout_cnt--);

        if (ret != TD_SUCCESS) {
            hdmi_warn("update EDID fail, timeout_cnt = %u\n", timeout_cnt);
        }
    }
    compatibility_info_update(hdmi_dev->hdmi_dev_id);

    return ret;
}

static td_void event_notify(hdmi_device *hdmi_dev, hdmi_event event)
{
    if (event <= HDMI_EVENT_HOTUNPLUG) {
        hdmi_mutex_unlock(g_hdmi_mutex);
        hdmi_mutex_lock(hdmi_dev->mutex_thread);
        if (hdmi_dev->k_callback == TD_NULL) { /* notify to user */
            hdmi_info("hdmi_id:%d, notify event(0x%x) to user\n", hdmi_dev->hdmi_dev_id, event);
            drv_hdmi_event_pool_write(hdmi_dev->hdmi_dev_id, event);
        } else { /* notify to kernel */
            hdmi_info("hdmi_id:%d, notify event(0x%x) to kernel\n", hdmi_dev->hdmi_dev_id, event);
            if (hdmi_dev->k_callback != TD_NULL) {
                hdmi_dev->k_callback((td_void *)&(hdmi_dev->hdmi_dev_id), event);
            }
            if (event == HDMI_EVENT_HOTPLUG) {
                hdmi_dev->hpd_notifyed = TD_TRUE;
            }
        }
        hdmi_mutex_unlock(hdmi_dev->mutex_thread);
        hdmi_mutex_lock(g_hdmi_mutex);
    }

    return;
}

static td_s32 hdmi_event_callback(td_void *data, hdmi_event event)
{
    td_s32 ret = TD_SUCCESS;
    hdmi_device *hdmi_dev = (hdmi_device *)data;

    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);

    switch (event) {
        case HDMI_EVENT_HOTPLUG:
            hpd_event(hdmi_dev);
            break;
        case HDMI_EVENT_HOTUNPLUG:
            hdmi_dev->hpd_detected = TD_FALSE;
            break;
        default:
            break;
    }
    event_notify(hdmi_dev, event);

    return ret;
}

static td_s32 hdmi_kthread_timer(void *data)
{
    hdmi_device *hdmi_dev = (hdmi_device *)data;
    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);
    while (!osal_kthread_should_stop()) {
        if (hdmi_dev->thread_info.thread_timer_sate == HDMI_THREAD_STATE_STOP) {
            osal_msleep(HDMI_THREAD_STATE_WAIT_TIME);
            continue;
        }
        if (hdmi_dev->hal != TD_NULL) {
            if (hdmi_dev->hal->hal_hdmi_sequencer_handler_process != TD_NULL &&
                (((td_u32)hdmi_dev->run_state & HDMI_RUN_STATE_OPEN) ||
                ((td_u32)hdmi_dev->run_state & HDMI_RUN_STATE_START) ||
                ((td_u32)hdmi_dev->run_state & HDMI_RUN_STATE_STOP))) {
                hdmi_dev->hal->hal_hdmi_sequencer_handler_process((struct hdmi_hal_ *)hdmi_dev->hal);
            }
        } else {
            hdmi_info("input param is NULL\n");
        }
        osal_msleep(hdmi_dev->debug.intf_status.event_thread_cycle_time);
    }

    return TD_SUCCESS;
}

static td_void hdmi_vo_attr_init(hdmi_vo_attr *video_attr)
{
    td_bool colorimetry_is601;

    colorimetry_is601 = ((video_attr->video_timing == HDMI_VIDEO_TIMING_720X480P_60000) ||
                         (video_attr->video_timing == HDMI_VIDEO_TIMING_720X576P_50000) ||
                         (video_attr->video_timing == HDMI_VIDEO_TIMING_1440X240P_60000) ||
                         (video_attr->video_timing == HDMI_VIDEO_TIMING_1440X480I_60000) ||
                         (video_attr->video_timing == HDMI_VIDEO_TIMING_1440X576I_50000) ||
                         (video_attr->video_timing == HDMI_VIDEO_TIMING_1440X576P_50000) ||
                         (video_attr->video_timing == HDMI_VIDEO_TIMING_1440X576I_60000));

    if (video_attr->video_timing == HDMI_VIDEO_TIMING_640X480P_60000) {
        video_attr->colorimetry = HDMI_COLORIMETRY_ITU_709;
        video_attr->picture_aspect = HDMI_PICTURE_ASPECT_4_3;
    } else if (colorimetry_is601 == TD_TRUE) {
        video_attr->colorimetry = HDMI_COLORIMETRY_ITU_601;
        video_attr->picture_aspect = HDMI_PICTURE_ASPECT_4_3;
    } else if (video_attr->video_timing <= HDMI_VIDEO_TIMING_4096X2160P_120000 &&
               video_attr->video_timing >= HDMI_VIDEO_TIMING_4096X2160P_24000) {
        video_attr->colorimetry = HDMI_COLORIMETRY_ITU_709;
        video_attr->picture_aspect = HDMI_PICTURE_ASPECT_256_135;
    } else {
        video_attr->colorimetry = HDMI_COLORIMETRY_ITU_709;
        video_attr->picture_aspect = HDMI_PICTURE_ASPECT_16_9;
    }

    video_attr->pixel_repeat = 1;
    if (video_attr->video_timing == HDMI_VIDEO_TIMING_1440X480I_60000 ||
        video_attr->video_timing == HDMI_VIDEO_TIMING_1440X576I_50000) {
        video_attr->pixel_repeat = 2; /* 2, pixel repeat count */
    }

    if (video_attr->video_timing <= HDMI_VIDEO_TIMING_640X480P_60000) {
        // cts1.4 test_id 7-24 required rgb_quantization is default or limited when the timing is 640x480p60
        video_attr->rgb_quantization = HDMI_QUANTIZATION_RANGE_DEFAULT;
    } else {
        video_attr->ycc_quantization = HDMI_YCC_QUANTIZATION_RANGE_LIMITED;
    }

    if (video_attr->video_timing != HDMI_VIDEO_TIMING_UNKNOWN &&
        video_attr->video_timing != HDMI_VIDEO_TIMING_640X480P_60000) {
        video_attr->in_color_space = HDMI_COLORSPACE_YCBCR444;
    } else {
        video_attr->in_color_space = HDMI_COLORSPACE_RGB;
    }

    video_attr->stereo_mode   = HDMI_3D_BUTT;
    video_attr->in_bit_depth  = HDMI_VIDEO_BITDEPTH_10;
    video_attr->active_aspect = HDMI_ACTIVE_ASPECT_PICTURE;
}

static td_void hdmi_user_attr_init(hdmi_device *hdmi_dev)
{
    hdmi_app_attr *app_attr = TD_NULL;

    app_attr = &hdmi_dev->attr.app_attr;
    app_attr->enable_hdmi = TD_TRUE;
    app_attr->enable_video = TD_TRUE;
    app_attr->enable_audio = TD_TRUE;
    app_attr->enable_aud_infoframe = TD_TRUE;
    app_attr->enable_avi_infoframe = TD_TRUE;
    app_attr->deep_color_mode = HDMI_DEEP_COLOR_24BIT;
    app_attr->out_color_space = HDMI_COLORSPACE_YCBCR444;
    app_attr->enable_clr_space_adapt = TD_TRUE;
    app_attr->enable_deep_clr_adapt = TD_TRUE;
    app_attr->out_csc_quantization = HDMI_QUANTIZATION_RANGE_LIMITED;

    return;
}

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wenum-conversion"
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wenum-conversion"
#endif
static td_s32 hdmi_device_init(hdmi_device *hdmi_dev)
{
    hdmi_ao_attr *audio_attr = TD_NULL;
    hdmi_vo_attr *video_attr = TD_NULL;

    audio_attr = &hdmi_dev->attr.ao_attr;
    video_attr = &hdmi_dev->attr.vo_attr;
    get_hdmi_default_action_set(hdmi_dev, HDMI_DEFAULT_ACTION_HDMI);

    hdmi_dev->mode_param.emi_en = TD_FALSE;
    hdmi_dev->debug.stop_delay = 0;
    hdmi_dev->delay.fmt_delay  = HDMI_DEV_FMT_DELAY;
    hdmi_dev->delay.mute_delay = HDMI_DEV_MUTE_DELAY;
    hdmi_user_attr_init(hdmi_dev);
    video_attr->video_timing = HDMI_VIDEO_TIMING_1280X720P_50000;
    video_attr->clk_fs       = HDMI_VIDEO_ATTR_CLK_FS;
    video_attr->h_sync_pol   = TD_FALSE;
    video_attr->v_sync_pol   = TD_FALSE;
    video_attr->de_pol       = TD_FALSE;
    (td_void)memset_s(&(hdmi_dev->debug.intf_status), sizeof(hdmi_dev->debug.intf_status), 0, sizeof(hdmi_intf_status));
    hdmi_vo_attr_init(video_attr);
    hdmi_dev->debug.intf_status.event_thread_cycle_time = HDMI_THREAD_DELAY;
    hdmi_dev->attr.app_attr.out_csc_quantization = hdmi_dev->csc_param.quantization;
    hdmi_dev->attr.app_attr.out_color_space = hdmi_dev->csc_param.pixel_encoding;
    video_attr->in_color_space = hdmi_dev->csc_param.pixel_encoding;
    video_attr->colorimetry = hdmi_dev->csc_param.colorimetry;
    audio_attr->sound_intf   = HDMI_AUDIO_INTF_I2S;
    audio_attr->sample_fs    = HDMI_SAMPLE_RATE_48K;
    audio_attr->sample_depth = HDMI_AUDIO_BIT_DEPTH_16;
    audio_attr->channels     = HDMI_AUDIO_FORMAT_2CH;
#if defined(PHY_CRAFT_S28) || defined(HDMI_PRODUCT_SS928V100) || defined(HDMI_PRODUCT_SS626V100)
    hdmi_dev->mode_param.trace_len = HDMI_TRACE_DEFAULT;
#else
    hdmi_dev->mode_param.trace_len = HDMI_TRACE_LEN_1;
#endif

    compatibility_info_default_set(hdmi_dev->hdmi_dev_id);
    hdmi_info("video_timing:%u\n", video_attr->video_timing);

    return TD_SUCCESS;
}
#ifdef __clang__
#pragma clang diagnostic pop
#else
#pragma GCC diagnostic pop
#endif

static td_void hdmi_dev_deinit(hdmi_device *hdmi_dev)
{
    hdmi_dev->mode_param.emi_en = TD_FALSE;
    hdmi_dev->hpd_detected      = TD_FALSE;
    hdmi_dev->hpd_notifyed      = TD_FALSE;
    hdmi_dev->run_state         = HDMI_RUN_STATE_NONE;
    hdmi_dev->tmds_mode         = HDMI_TMDS_MODE_NONE;
    hdmi_dev->transition_state  = HDMI_TRANSITION_NONE;
    hdmi_dev->k_callback        = TD_NULL;
    hdmi_dev->hal               = TD_NULL;
    hdmi_dev->kernel_cnt        = 0;
    hdmi_dev->user_callback_cnt = 0;
    hdmi_dev->user_cnt          = 0;

    (td_void)memset_s(&hdmi_dev->attr, sizeof(hdmi_dev->attr), 0, sizeof(hdmi_dev->attr));
    (td_void)memset_s(&hdmi_dev->debug, sizeof(hdmi_dev->debug), 0, sizeof(hdmi_dev->debug));
    (td_void)memset_s(&hdmi_dev->delay, sizeof(hdmi_dev->delay), 0, sizeof(hdmi_dev->delay));
    (td_void)memset_s(&hdmi_dev->edid_info, sizeof(hdmi_dev->edid_info), 0, sizeof(hdmi_dev->edid_info));
    (td_void)memset_s(&hdmi_dev->info_frame, sizeof(hdmi_dev->info_frame), 0, sizeof(hdmi_dev->info_frame));
    (td_void)memset_s(&hdmi_dev->thread_info, sizeof(hdmi_dev->thread_info), 0, sizeof(hdmi_dev->thread_info));

    return;
}

static td_s32 create_task(hdmi_device *hdmi_dev, hdmi_hal_init *hal_cfg)
{
    td_s32 err_num;
    td_s32 ret = TD_SUCCESS;
    td_char thread_name[THREAD_NAME_MAX] = "hdmi0_kthread";

    if (hdmi_dev->thread_info.thread_timer == TD_NULL) {
        hal_cfg->base_addr      = g_hdmi_reg[hdmi_dev->hdmi_dev_id];
        hal_cfg->phy_addr       = g_hdmi_phy[hdmi_dev->hdmi_dev_id];
        hal_cfg->event_callback = hdmi_event_callback;
        hal_cfg->event_data     = hdmi_dev;
        hal_cfg->hdmi_dev_id    = hdmi_dev->hdmi_dev_id;

        ret = hal_hdmi_open(hal_cfg, &hdmi_dev->hal);
        if (hdmi_dev->hal == TD_NULL || ret != TD_SUCCESS) {
            hdmi_err("hal_hdmi_open fail!\n");
            return TD_FAILURE;
        }
        hdmi_dev->hal->hal_ctx.hdmi_dev = hdmi_dev;
        hdmi_dev->hal->hal_ctx.hdmi_id = hdmi_dev->hdmi_dev_id;

        err_num = snprintf_s(thread_name, THREAD_NAME_MAX,
            THREAD_NAME_MAX - 1, "hdmi%u_kthread", hdmi_dev->hdmi_dev_id);
        if (err_num < 0) {
            hdmi_err("snprintf_s err\n");
            return TD_FAILURE;
        }
        hdmi_dev->thread_info.thread_timer = osal_kthread_create(hdmi_kthread_timer, hdmi_dev, thread_name);
        if (hdmi_dev->thread_info.thread_timer == TD_NULL) {
            hdmi_err("create hdmi%u timer thread fail!\n", hdmi_dev->hdmi_dev_id);
            return TD_FAILURE;
        }
        hdmi_info("create %s success.\n", thread_name);
        hdmi_thread_state_set(hdmi_dev, HDMI_THREAD_STATE_RUN);
        hdmi_device_init(hdmi_dev);
    }

    return ret;
}

static td_s32 hdmi_oe_transition_state_set(hdmi_device *hdmi_dev, td_bool user, td_bool hdmi_on)
{
    td_s32 ret = TD_SUCCESS;

#if !defined(HDMI_FPGA_SUPPORT) || defined(HDMI_PRODUCT_SS626V100)
    if (hdmi_on == TD_FALSE) {
        hdmi_info("hdmi not on, do hardware init\n");
        hdmi_dev->transition_state = HDMI_TRANSITION_NONE;
        hal_call_void(hdmi_dev, hal_hdmi_hardware_init);
    } else {
        hdmi_info("hdmi already on, do not hardware init.\n");
        if (user) {
            hdmi_dev->transition_state = (hdmi_dev->kernel_cnt > 0) ?
                HDMI_TRANSITION_MCE_APP : HDMI_TRANSITION_BOOT_APP;
        } else {
            hdmi_dev->transition_state = HDMI_TRANSITION_BOOT_MCE;
        }
    }
#else
    ot_unused(hdmi_on);
    if (user) {
        hdmi_dev->transition_state = (hdmi_dev->kernel_cnt > 0) ?
            HDMI_TRANSITION_MCE_APP : HDMI_TRANSITION_BOOT_APP;
    } else {
        hdmi_dev->transition_state = HDMI_TRANSITION_BOOT_MCE;
    }
#endif

    return ret;
}

static td_void drv_hdmi_hpd_status_delay_get(const hdmi_device *hdmi_dev, td_bool *hotplug)
{
    td_u32 i;

    for (i = 0; i < HDMI_READ_HPD_STATUS_DELAY; i++) {
        hal_call_void(hdmi_dev, hal_hdmi_hot_plug_status_get, hotplug);
        if ((*hotplug) == TD_TRUE) {
            break;
        }
        osal_msleep(HDMI_READ_HPD_STATUS_DELAY_TIME);
    }
    hdmi_info("delay %d times, hot plug status is 0x%x\n", i, (*hotplug ? HDMI_EVENT_HOTPLUG : HDMI_EVENT_HOTUNPLUG));

    return;
}

static td_void notify_event_in_open(hdmi_device *hdmi_dev, td_bool hdmi_on, td_bool hotplug)
{
    if (hdmi_on == TD_TRUE) { /* need to updata event in open, when there is boot logo. */
        hdmi_event_callback(hdmi_dev, hotplug ? HDMI_EVENT_HOTPLUG : HDMI_EVENT_HOTUNPLUG);
    } else {
        if (hotplug == TD_TRUE) {
            hpd_event(hdmi_dev);
        } else {
            hdmi_dev->hpd_detected = TD_FALSE;
        }
    }

    return;
}

static td_s32 drv_hdmi_open(hdmi_device *hdmi_dev, td_bool user)
{
    td_s32 ret;
    hdmi_hal_init hal_cfg = {0};
    td_u32 event_pool_id = 0;
    td_bool hotplug = TD_FALSE;
    td_bool hdmi_on = TD_FALSE;

    /* create hdmi task, every hdmi device only create once */
    if (create_task(hdmi_dev, &hal_cfg) != TD_SUCCESS) {
        return OT_ERR_HDMI_CREATE_TESK_FAILED;
    }

    drv_hdmi_event_init(hdmi_dev->hdmi_dev_id);

    if (user == TD_TRUE) {
        /* must create event queue first */
        ret = drv_hdmi_event_pool_malloc(hdmi_dev->hdmi_dev_id, &event_pool_id);
        if (ret != TD_SUCCESS) {
            if (ret != HDMI_EVENT_ID_EXIST) {
                hdmi_err("drv_hdmi_event_pool_malloc fail\n");
                return OT_ERR_HDMI_MALLOC_FAILED;
            }
        } else {
            hdmi_dev->user_cnt++;
        }
        hdmi_info("create event queue for process:%u \n", hdmi_current_id_get(hdmi_dev));
    } else {
        hdmi_dev->kernel_cnt++;
    }

    if (hdmi_dev->user_callback_cnt == 0) {
        hdmi_mutex_unlock(g_hdmi_mutex);
        hdmi_mutex_lock(hdmi_dev->mutex_thread);
        hdmi_dev->k_callback = drv_hdmi_kernel_event_callback;
        hdmi_mutex_unlock(hdmi_dev->mutex_thread);
        hdmi_mutex_lock(g_hdmi_mutex);
    }
    hdmi_info("user_cnt: %u, kernel_cnt: %u, user_call_back_cnt: %u\n",
              hdmi_dev->user_cnt, hdmi_dev->kernel_cnt, hdmi_dev->user_callback_cnt);

    hal_call_void(hdmi_dev, hal_hdmi_phy_output_enable_get, &hdmi_on);
    ret = hdmi_oe_transition_state_set(hdmi_dev, user, hdmi_on);
    hdmi_if_failure_return(ret, TD_FAILURE);
    /* insure that get sink caps success immediately after open HDMI when no boot or no output. */
    drv_hdmi_hpd_status_delay_get(hdmi_dev, &hotplug);
    notify_event_in_open(hdmi_dev, hdmi_on, hotplug);
    hdmi_info("transition_state: %u\n", hdmi_dev->transition_state);
    hdmi_dev->run_state = HDMI_RUN_STATE_OPEN;

    return TD_SUCCESS;
}

static td_s32 hdmi_release(hdmi_device *hdmi_dev)
{
    hdmi_info("in\n");

    if ((td_u32)hdmi_dev->run_state & HDMI_RUN_STATE_START) {
        drv_hdmi_stop(hdmi_dev);
    }

    if (hdmi_dev->user_cnt == 0) {
        if (hdmi_dev->kernel_cnt == 0) {
            if (hdmi_dev->thread_info.thread_timer != TD_NULL) {
                hdmi_info("stop hdmi kernel thread timer\n");
                /*
                 * note: in linux,
                 * when call the kthread_stop function,
                 * the thread function cannot be finished, otherwise it will oops.
                 */
                osal_kthread_destroy(hdmi_dev->thread_info.thread_timer, TD_TRUE);
                hdmi_dev->thread_info.thread_timer = TD_NULL;
                hdmi_thread_state_set(hdmi_dev, HDMI_THREAD_STATE_STOP);
            }
            drv_hdmi_event_deinit(hdmi_dev->hdmi_dev_id);
            hdmi_mutex_lock(hdmi_dev->mutex_proc);
            if (hdmi_dev->hal != TD_NULL) {
                hal_hdmi_close(hdmi_dev->hal);
                hdmi_dev->hal = TD_NULL;
            }
            hdmi_dev_deinit(hdmi_dev);
            hdmi_mutex_unlock(hdmi_dev->mutex_proc);
        } else {
            drv_hdmi_event_deinit(hdmi_dev->hdmi_dev_id);
            hdmi_mutex_unlock(g_hdmi_mutex);
            hdmi_mutex_lock(hdmi_dev->mutex_thread);
            hdmi_dev->k_callback = drv_hdmi_kernel_event_callback;
            hdmi_mutex_unlock(hdmi_dev->mutex_thread);
            hdmi_mutex_lock(g_hdmi_mutex);
        }
    }
    hdmi_info("out\n");

    return TD_SUCCESS;
}

static td_s32 drv_hdmi_close(hdmi_device *hdmi_dev, td_bool user)
{
    hdmi_info("in\n");
    hdmi_check_open_return((td_u32)hdmi_dev->run_state);

    hdmi_info("user: %u\n", user);
    hdmi_info("user_cnt: %u, kernel_cnt: %u\n", hdmi_dev->user_cnt, hdmi_dev->kernel_cnt);

    hdmi_dev->hpd_notifyed = TD_FALSE;

    if (user == TD_TRUE && hdmi_dev->user_cnt > 0) {
        hdmi_info("delete event queue for process:%u \n", hdmi_current_id_get(hdmi_dev));
        if (drv_hdmi_event_pool_free(hdmi_dev->hdmi_dev_id, hdmi_current_id_get(hdmi_dev)) != TD_SUCCESS) {
            return OT_ERR_HDMI_FREE_FAILED;
        }
        hdmi_dev->user_cnt--;
        if (hdmi_dev->user_cnt == 0 && hdmi_dev->kernel_cnt > 0) {
            hdmi_mutex_unlock(g_hdmi_mutex);
            hdmi_mutex_lock(hdmi_dev->mutex_thread);
            hdmi_dev->k_callback = drv_hdmi_kernel_event_callback;
            hdmi_mutex_unlock(hdmi_dev->mutex_thread);
            hdmi_mutex_lock(g_hdmi_mutex);
        } else {
            hdmi_mutex_unlock(g_hdmi_mutex);
            hdmi_mutex_lock(hdmi_dev->mutex_thread);
            hdmi_dev->k_callback = TD_NULL;
            hdmi_mutex_unlock(hdmi_dev->mutex_thread);
            hdmi_mutex_lock(g_hdmi_mutex);
        }
    } else if (user == TD_FALSE && hdmi_dev->kernel_cnt > 0) {
        hdmi_dev->kernel_cnt--;
    }

    if (hdmi_dev->user_cnt == 0 && hdmi_dev->kernel_cnt == 0) {
        hdmi_release(hdmi_dev);
        hdmi_dev->run_state = HDMI_RUN_STATE_CLOSE;
    }

    hdmi_info("out\n");

    return TD_SUCCESS;
}

static td_void drv_hdmi_audio_path_enable(const hdmi_device *hdmi_dev, td_bool enable)
{
    hal_call_void(hdmi_dev, hal_hdmi_audio_path_enable_set, enable);
    return;
}

static td_void hdmi_debug_delay(const hdmi_device *hdmi_dev, const td_char *info)
{
    osal_msleep(hdmi_dev->debug.stop_delay);

    if (hdmi_dev->debug.stop_delay > 0) {
        hdmi_info("%s, delay %ums\n", info, hdmi_dev->debug.stop_delay);
    }
}

static td_s32 check_audio_attr(const hdmi_ao_attr *ao_attr)
{
    if (ao_attr->sound_intf != HDMI_AUDIO_INTF_I2S &&
        ao_attr->sound_intf != HDMI_AUDIO_INTF_SPDIF &&
        ao_attr->sound_intf != HDMI_AUDIO_INTF_HBRA) {
        hdmi_err("the audio interface(%u) is invalid\n", ao_attr->sound_intf);
        return TD_FAILURE;
    }

    if (ao_attr->channels < HDMI_AUDIO_FORMAT_2CH || ao_attr->channels > HDMI_AUDIO_FORMAT_8CH) {
        hdmi_err("the audio channel number(%u) is invalid\n", ao_attr->channels);
        return TD_FAILURE;
    }

    if (ao_attr->sample_fs < HDMI_SAMPLE_RATE_32K || ao_attr->sample_fs > HDMI_SAMPLE_RATE_768K) {
        hdmi_err("the input audio frequency(%u) is invalid\n", ao_attr->sample_fs);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

static td_bool hdmi_chip_id(hdmi_device_id hdmi_id)
{
    td_bool ret = TD_TRUE;
#if defined(HDMI_PRODUCT_SS626V100)
    /* The ss623v100 chip supports only hdmi0 port output.  */
    if ((chip_is_ss623v100() == TD_TRUE) && (hdmi_id != 0)) {
        hdmi_err("lite : hdmi%d is not supported!\n", hdmi_id);
        ret = TD_FALSE;
    }
#endif
    return ret;
}

static td_s32 drv_hdmi_ao_attr_set(hdmi_device *hdmi_dev, const hdmi_ao_attr *ao_attr)
{
    errno_t err_code;
    td_s32 ret = TD_SUCCESS;
    hdmi_attr hw_attr = {0};
    hdmi_audio_config audio_cfg  = {0};
    hdmi_app_attr *app_attr = TD_NULL;
    hdmi_ao_attr *audio_attr = TD_NULL;
    hdmi_vo_attr *video_attr = TD_NULL;

    hdmi_info("in\n");

    hdmi_if_false_return(hdmi_chip_id(hdmi_dev->hdmi_dev_id), OT_ERR_HDMI_INVALID_PARA);
    if (check_audio_attr(ao_attr) != TD_SUCCESS) {
        hdmi_err("check_audio_attr fail\n");
        return OT_ERR_HDMI_INVALID_PARA;
    }
    app_attr   = &hdmi_dev->attr.app_attr;
    video_attr = &hdmi_dev->attr.vo_attr;
    audio_attr = &hdmi_dev->attr.ao_attr;

    err_code = memcpy_s(audio_attr, sizeof(hdmi_dev->attr.ao_attr), ao_attr, sizeof(hdmi_ao_attr));
    if (err_code != EOK) {
        hdmi_err("secure function error:%d\n", err_code);
        return OT_ERR_HDMI_INVALID_PARA;
    }
    hdmi_attr_construct(hdmi_dev, &hw_attr);

    hdmi_info("down_sample: %u\n", audio_attr->down_sample);
    hdmi_info("sample_fs: %u\n", audio_attr->sample_fs);
    hdmi_info("channels: %u\n", audio_attr->channels);
    hdmi_info("sound_intf: %u\n", audio_attr->sound_intf);
    hdmi_info("sample_depth: %u\n", audio_attr->sample_depth);
    hdmi_info("audio_code: %u\n", audio_attr->audio_code);

    audio_cfg.down_sample  = ao_attr->down_sample;
    audio_cfg.layout       = ao_attr->channels;
    audio_cfg.sample_depth = ao_attr->sample_depth;
    audio_cfg.sample_fs    = ao_attr->sample_fs;
    audio_cfg.sound_intf   = ao_attr->sound_intf;
    audio_cfg.enable_audio = (hdmi_dev->attr.app_attr.enable_audio) && (app_attr->enable_hdmi);
    audio_cfg.tmds_clk     = video_attr->hdmi_adapt_pix_clk;
    audio_cfg.pixel_repeat = video_attr->pixel_repeat;
    hal_call_void(hdmi_dev, hal_hdmi_audio_path_enable_set, TD_FALSE);
    hal_call_ret(hdmi_dev, ret, hal_hdmi_audio_path_set, &audio_cfg);
    drv_hdmi_audio_infoframe_send(&hdmi_dev->info_frame, (app_attr->enable_audio && app_attr->enable_aud_infoframe));
    hal_call_void(hdmi_dev, hal_hdmi_audio_path_enable_set, audio_cfg.enable_audio);
    hdmi_info("out\n");

    return ret;
}

static td_s32 check_video_attr(const hdmi_vo_attr *vo_attr)
{
    if (vo_attr->video_timing >= HDMI_VIDEO_TIMING_BUTT) {
        hdmi_err("video timing(%u) is wrong\n", vo_attr->video_timing);
        return TD_FAILURE;
    }

    if (vo_attr->in_color_space >= HDMI_COLORSPACE_BUTT) {
        hdmi_err("video in_color_space(%u) is wrong\n", vo_attr->in_color_space);
        return TD_FAILURE;
    }

    if (vo_attr->colorimetry != HDMI_COLORIMETRY_ITU_601 &&
        vo_attr->colorimetry != HDMI_COLORIMETRY_ITU_709 &&
        vo_attr->colorimetry != HDMI_COLORIMETRY_EXTEND) {
        hdmi_err("video colorimetry(%u) is wrong\n", vo_attr->colorimetry);
        return TD_FAILURE;
    }

    if (vo_attr->rgb_quantization > HDMI_QUANTIZATION_RANGE_FULL) {
        hdmi_err("video rgb_quantization(%u) is wrong\n", vo_attr->rgb_quantization);
        return TD_FAILURE;
    }

    if (vo_attr->picture_aspect != HDMI_PICTURE_ASPECT_4_3 &&
        vo_attr->picture_aspect != HDMI_PICTURE_ASPECT_16_9 &&
        vo_attr->picture_aspect != HDMI_PICTURE_ASPECT_64_27 &&
        vo_attr->picture_aspect != HDMI_PICTURE_ASPECT_256_135) {
        hdmi_err("video picture_aspect(%u) is wrong\n", vo_attr->picture_aspect);
        return TD_FAILURE;
    }

    if (vo_attr->in_bit_depth != HDMI_VIDEO_BITDEPTH_8 &&
        vo_attr->in_bit_depth != HDMI_VIDEO_BITDEPTH_10 &&
        vo_attr->in_bit_depth != HDMI_VIDEO_BITDEPTH_12) {
        hdmi_err("video in_bit_depth(%u) is wrong\n", vo_attr->in_bit_depth);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

static td_void hdmi_vo_colorimetry_cfg(const hdmi_vo_attr *vo_attr, hdmi_video_config *video_cfg)
{
    switch (vo_attr->colorimetry) {
        case HDMI_COLORIMETRY_ITU_601:
            video_cfg->conv_std = HDMI_CONV_STD_BT_601;
            break;
        case HDMI_COLORIMETRY_ITU_709:
            video_cfg->conv_std = HDMI_CONV_STD_BT_709;
            break;
        case HDMI_COLORIMETRY_EXTEND:
            if (vo_attr->extended_colorimetry == HDMI_EXTENDED_COLORIMETRY_2020_NON_CONST_LUMINOUS) {
                video_cfg->conv_std = HDMI_CONV_STD_BT_2020_NON_CONST_LUMINOUS;
            } else if (vo_attr->extended_colorimetry == HDMI_EXTENDED_COLORIMETRY_2020_CONST_LUMINOUS) {
                video_cfg->conv_std = HDMI_CONV_STD_BT_2020_CONST_LUMINOUS;
            }
            break;
        default:
            video_cfg->conv_std = HDMI_CONV_STD_BT_709;
            break;
    }

    return;
}

static td_void hdmi_attr_to_vid_cfg(hdmi_vo_attr *vo_attr, const hdmi_app_attr *app_attr, hdmi_video_config *video_cfg)
{
    video_cfg->pixel_clk =
        (app_attr->out_color_space == HDMI_COLORSPACE_YCBCR420) ? (vo_attr->clk_fs >> 1) : vo_attr->clk_fs;
    video_cfg->tmds_clk        = vo_attr->hdmi_adapt_pix_clk;
    video_cfg->in_bit_depth    = vo_attr->in_bit_depth;
    video_cfg->quantization    = vo_attr->rgb_quantization;
    video_cfg->in_color_space  = vo_attr->in_color_space;
    video_cfg->deep_color      = app_attr->deep_color_mode;
    video_cfg->out_color_space = app_attr->out_color_space;
    video_cfg->v_sync_pol      = vo_attr->v_sync_pol;
    video_cfg->h_sync_pol      = vo_attr->h_sync_pol;
    video_cfg->de_pol          = vo_attr->de_pol;

    if (video_cfg->out_color_space == HDMI_COLORSPACE_RGB) {
        vo_attr->rgb_quantization = app_attr->out_csc_quantization;
        vo_attr->ycc_quantization = (vo_attr->rgb_quantization == HDMI_QUANTIZATION_RANGE_FULL) ?
            HDMI_YCC_QUANTIZATION_RANGE_FULL : HDMI_YCC_QUANTIZATION_RANGE_LIMITED;
    } else {
        vo_attr->ycc_quantization = (app_attr->out_csc_quantization == HDMI_QUANTIZATION_RANGE_FULL) ?
            HDMI_YCC_QUANTIZATION_RANGE_FULL : HDMI_YCC_QUANTIZATION_RANGE_LIMITED;
        if (vo_attr->ycc_quantization == HDMI_YCC_QUANTIZATION_RANGE_FULL) {
            vo_attr->rgb_quantization = HDMI_QUANTIZATION_RANGE_FULL;
        } else {
            vo_attr->rgb_quantization = HDMI_QUANTIZATION_RANGE_DEFAULT;
        }
    }

    if (app_attr->out_color_space == HDMI_COLORSPACE_RGB) {
        video_cfg->out_csc_quantization = vo_attr->rgb_quantization;
    } else {
        video_cfg->out_csc_quantization = (vo_attr->ycc_quantization == HDMI_YCC_QUANTIZATION_RANGE_FULL) ?
            HDMI_QUANTIZATION_RANGE_FULL : HDMI_QUANTIZATION_RANGE_LIMITED;
    }
    hdmi_vo_colorimetry_cfg(vo_attr, video_cfg);

    return;
}

static td_void hdmi_phy_set(const hdmi_device *hdmi_dev, const hdmi_vo_attr *vo_attr)
{
    hdmi_phy_cfg phy_cfg  = {0};
    const hdmi_app_attr *app_attr = TD_NULL;

    hdmi_if_fpga_return_void();

    app_attr = &hdmi_dev->attr.app_attr;

    phy_cfg.pixel_clk  =
        (app_attr->out_color_space == HDMI_COLORSPACE_YCBCR420) ? (vo_attr->clk_fs >> 1) : vo_attr->clk_fs;
    phy_cfg.emi_enable = hdmi_dev->mode_param.emi_en;
    phy_cfg.tmds_clk   = vo_attr->hdmi_adapt_pix_clk;
    phy_cfg.deep_color = app_attr->deep_color_mode;
    phy_cfg.trace_len  = hdmi_dev->mode_param.trace_len;
    hal_call_void(hdmi_dev, hal_hdmi_phy_set, &phy_cfg);

    return;
}

static td_void hdmi_csc_set(const hdmi_device *hdmi_dev, hdmi_vo_attr *vo_attr)
{
    hdmi_video_config video_cfg = {0};
    const hdmi_app_attr *app_attr = TD_NULL;

    app_attr = &hdmi_dev->attr.app_attr;
    hdmi_attr_to_vid_cfg(vo_attr, app_attr, &video_cfg);
    hal_call_void(hdmi_dev, hal_hdmi_csc_param_set, &video_cfg);

    return;
}

static td_s32 hdmi_video_path_set(const hdmi_device *hdmi_dev, hdmi_vo_attr *vo_attr)
{
    td_s32 ret;
    hdmi_video_config video_cfg = {0};
    const hdmi_app_attr *app_attr = TD_NULL;

    app_attr = &hdmi_dev->attr.app_attr;
    video_cfg.emi_enable = hdmi_dev->mode_param.emi_en;

    hdmi_attr_to_vid_cfg(vo_attr, app_attr, &video_cfg);
    hal_call_ret(hdmi_dev, ret, hal_hdmi_video_path_set, &video_cfg);
    hdmi_csc_set(hdmi_dev, vo_attr);
    hdmi_phy_set(hdmi_dev, vo_attr);

    return ret;
}

static td_bool app_attr_is_changed(const hdmi_app_attr *hw_app_attr, const hdmi_app_attr *sw_app_attr)
{
    if (hw_app_attr->enable_hdmi != sw_app_attr->enable_hdmi) {
        hdmi_info("hdmi enable change, old(%u)->new(%u)\n", hw_app_attr->enable_hdmi, sw_app_attr->enable_hdmi);
        return TD_TRUE;
    }

    if (hw_app_attr->out_color_space != sw_app_attr->out_color_space) {
        hdmi_info("out color_space change,old(%u)->new(%u)\n",
            hw_app_attr->out_color_space, sw_app_attr->out_color_space);
        return TD_TRUE;
    }

    if (((sw_app_attr->deep_color_mode == HDMI_DEEP_COLOR_OFF) ||
        (sw_app_attr->deep_color_mode == HDMI_DEEP_COLOR_24BIT)) &&
        ((hw_app_attr->deep_color_mode == HDMI_DEEP_COLOR_OFF) ||
        (hw_app_attr->deep_color_mode == HDMI_DEEP_COLOR_24BIT))) {
        hdmi_info("deepcolor mode not change: %u\n", hw_app_attr->deep_color_mode);
    } else if (hw_app_attr->deep_color_mode != sw_app_attr->deep_color_mode) {
        hdmi_info("deepcolor mode change,old(%u)->new(%u)\n",
            hw_app_attr->deep_color_mode, sw_app_attr->deep_color_mode);
        return TD_TRUE;
    }

    if (hw_app_attr->xvycc_mode != sw_app_attr->xvycc_mode) {
        hdmi_info("xvycc_mode mode change,old(%u)->new(%u)\n",
            hw_app_attr->xvycc_mode, sw_app_attr->xvycc_mode);
        return TD_TRUE;
    }

    if (hw_app_attr->enable_avi_infoframe != sw_app_attr->enable_avi_infoframe) {
        hdmi_info("avi infoframe enable change, old(%u)->new(%u)\n",
            hw_app_attr->enable_avi_infoframe, sw_app_attr->enable_avi_infoframe);
        return TD_TRUE;
    }

    if (hw_app_attr->out_csc_quantization != sw_app_attr->out_csc_quantization) {
        hdmi_info("out_csc_quantization change, old(%u)->new(%u)\n",
            hw_app_attr->out_csc_quantization, sw_app_attr->out_csc_quantization);
        return TD_TRUE;
    }

    return TD_FALSE;
}

static td_bool vo_attr_is_changed(const hdmi_vo_attr *hw_vo_attr, hdmi_vo_attr *sw_vo_attr)
{
    if (hw_vo_attr->video_timing != sw_vo_attr->video_timing) {
        hdmi_info("video timing change,old(%u)->new(%u)\n", hw_vo_attr->video_timing, sw_vo_attr->video_timing);
        return TD_TRUE;
    } else {
        /*
         * if timing is change, the sync param need according to user config, or not,
         * should not change the sync param.
         */
        sw_vo_attr->v_sync_pol = hw_vo_attr->v_sync_pol;
        sw_vo_attr->h_sync_pol = hw_vo_attr->h_sync_pol;
        sw_vo_attr->de_pol     = hw_vo_attr->de_pol;
    }

    if (hw_vo_attr->in_color_space != sw_vo_attr->in_color_space) {
        hdmi_info("input colorspace change, old(%u)->new(%u)\n",
            hw_vo_attr->in_color_space, sw_vo_attr->in_color_space);
        return TD_TRUE;
    }
    if (hw_vo_attr->stereo_mode != sw_vo_attr->stereo_mode) {
        hdmi_info("3d mode change, old(%u)->new(%u)\n", hw_vo_attr->stereo_mode, sw_vo_attr->stereo_mode);
        return TD_TRUE;
    }
    if (hw_vo_attr->pixel_repeat != sw_vo_attr->pixel_repeat) {
        hdmi_info("pixel repeation change, old(%u)->new(%u)\n", hw_vo_attr->pixel_repeat, sw_vo_attr->pixel_repeat);
        return TD_TRUE;
    }
    if (hw_vo_attr->colorimetry != sw_vo_attr->colorimetry) {
        hdmi_info("colorimetry change, old(%u)->new(%u)\n", hw_vo_attr->colorimetry, sw_vo_attr->colorimetry);
        return TD_TRUE;
    }
    if (hw_vo_attr->extended_colorimetry != sw_vo_attr->extended_colorimetry) {
        hdmi_info("extended_colorimetry change, old(%u)->new(%u)\n",
            hw_vo_attr->extended_colorimetry, sw_vo_attr->extended_colorimetry);
        return TD_TRUE;
    }
    if (hw_vo_attr->rgb_quantization != sw_vo_attr->rgb_quantization) {
        hdmi_info("RGB quantization change, old(%u)->new(%u)\n",
            hw_vo_attr->rgb_quantization, sw_vo_attr->rgb_quantization);
        return TD_TRUE;
    }
    if (hw_vo_attr->ycc_quantization != sw_vo_attr->ycc_quantization) {
        hdmi_info("YCC quantization change, old(%u)->new(%u)\n",
            hw_vo_attr->ycc_quantization, sw_vo_attr->ycc_quantization);
        return TD_TRUE;
    }
    if (hw_vo_attr->picture_aspect != sw_vo_attr->picture_aspect) {
        hdmi_info("picture aspect change, old(%u)->new(%u)\n", hw_vo_attr->picture_aspect, sw_vo_attr->picture_aspect);
        return TD_TRUE;
    }

    return TD_FALSE;
}

static td_s32 check_app_attr(const hdmi_app_attr *app_attr)
{
    td_s32 ret = TD_SUCCESS;

    if (app_attr->out_color_space != HDMI_COLORSPACE_RGB &&
        app_attr->out_color_space != HDMI_COLORSPACE_YCBCR422 &&
        app_attr->out_color_space != HDMI_COLORSPACE_YCBCR444 &&
        app_attr->out_color_space != HDMI_COLORSPACE_YCBCR420) {
        hdmi_err("out_color_space=%u is wrong\n", app_attr->out_color_space);
        ret = TD_FAILURE;
    }

    if (app_attr->deep_color_mode != HDMI_DEEP_COLOR_24BIT &&
        app_attr->deep_color_mode != HDMI_DEEP_COLOR_30BIT &&
        app_attr->deep_color_mode != HDMI_DEEP_COLOR_36BIT &&
        app_attr->deep_color_mode != HDMI_DEEP_COLOR_48BIT &&
        app_attr->deep_color_mode != HDMI_DEEP_COLOR_OFF) {
        hdmi_err("deep_clr_mode=%u is wrong\n", app_attr->deep_color_mode);
        ret = TD_FAILURE;
    }

    if (app_attr->hdcp_mode != HDMI_HDCP_MODE_1_4 &&
        app_attr->hdcp_mode != HDMI_HDCP_MODE_2_2 &&
        app_attr->hdcp_mode != HDMI_HDCP_MODE_AUTO) {
        hdmi_err("hdcp_mode=%u is wrong\n", app_attr->hdcp_mode);
        ret = TD_FAILURE;
    }

    return ret;
}

static td_void hdmi_vo_attr_info(const hdmi_device *hdmi_dev, const hdmi_vo_attr *video_attr)
{
    ot_unused(hdmi_dev);

    hdmi_info("clk_fs: %u\n", video_attr->clk_fs);
    hdmi_info("pixel_repeat: %u\n", video_attr->pixel_repeat);
    hdmi_info("video_timing: %u\n", video_attr->video_timing);
    hdmi_info("stereo_mode: %u\n", video_attr->stereo_mode);
    hdmi_info("in_color_space: %u\n", video_attr->in_color_space);
    hdmi_info("colorimetry: %u\n", video_attr->colorimetry);
    hdmi_info("extended_colorimetry: %u\n", video_attr->extended_colorimetry);
    hdmi_info("rgb_quantization: %u\n", video_attr->rgb_quantization);
    hdmi_info("ycc_quantization: %u\n", video_attr->ycc_quantization);
    hdmi_info("picture_aspect: %u\n", video_attr->picture_aspect);
    hdmi_info("active_aspect: %u\n", video_attr->active_aspect);
    hdmi_info("picture_scaling: %u\n", video_attr->picture_scaling);
    hdmi_info("in_bit_depth: %u\n", video_attr->in_bit_depth);
    hdmi_info("disp_fmt: %u\n", video_attr->disp_fmt);
    hdmi_info("v_sync_pol: %u\n", video_attr->v_sync_pol);
    hdmi_info("h_sync_pol: %u\n", video_attr->h_sync_pol);
    hdmi_info("de_pol: %u\n", video_attr->de_pol);

    return;
}

static td_s32 hdmi_format_is_change(hdmi_device *hdmi_dev, const hdmi_vo_attr *video_attr,
    const hdmi_vo_attr *vo_attr)
{
    errno_t ret;
    hdmi_app_attr tmp_app = {0};
    hdmi_app_attr *app_attr = TD_NULL;
    td_bool enable_clr_space_adapt_back;
    td_bool enable_deep_clr_adapt_back;
    static hdmi_disp_format disp_fmt = HDMI_VIDEO_FORMAT_BUTT;

    app_attr = &hdmi_dev->attr.app_attr;
    enable_clr_space_adapt_back = app_attr->enable_clr_space_adapt;
    enable_deep_clr_adapt_back = app_attr->enable_deep_clr_adapt;
    ret = memcpy_s(&tmp_app, sizeof(tmp_app), app_attr, sizeof(*app_attr));
    if (ret != EOK) {
        hdmi_err("secure function error:%d\n", ret);
        return OT_ERR_HDMI_INVALID_PARA;
    }

    /* when format is changed, force to enable  hdmi_color_and_bit_strategy */
    if (disp_fmt != video_attr->disp_fmt) {
        tmp_app.enable_clr_space_adapt = TD_TRUE;
        tmp_app.enable_deep_clr_adapt = TD_TRUE;
        disp_fmt = video_attr->disp_fmt;
    }
    if (hdmi_color_and_bit_strategy(hdmi_dev, &tmp_app, vo_attr) != TD_SUCCESS) {
        hdmi_err(" hdmi_color_and_bit_strategy fail\n");
        return OT_ERR_HDMI_STRATEGY_FAILED;
    }

    ret = memcpy_s(app_attr, sizeof(hdmi_dev->attr.app_attr), &tmp_app, sizeof(tmp_app));
    if (ret != EOK) {
        hdmi_err("secure function error:%d\n", ret);
        return OT_ERR_HDMI_INVALID_PARA;
    }
    app_attr->enable_clr_space_adapt = enable_clr_space_adapt_back;
    app_attr->enable_deep_clr_adapt  = enable_deep_clr_adapt_back;

    return TD_SUCCESS;
}
static td_s32 drv_hdmi_vo_attr_get(const hdmi_device *hdmi_dev, hdmi_vo_attr *vo_attr)
{
    errno_t ret;

    hdmi_info("in\n");

    ret = memcpy_s(vo_attr, sizeof(*vo_attr), &hdmi_dev->attr.vo_attr, sizeof(hdmi_vo_attr));
    if (ret != EOK) {
        hdmi_err("secure function error:%d\n", ret);
        return OT_ERR_HDMI_INVALID_PARA;
    }
    hdmi_info("out\n");

    return TD_SUCCESS;
}

static td_s32 drv_hdmi_infoframe_get(const hdmi_device *hdmi_dev,
    hdmi_infoframe_id infoframe_id, hdmi_infoframe *info_frame)
{
    errno_t ret;

    hdmi_info("in\n");

    switch (infoframe_id) {
        case HDMI_INFOFRAME_TYPE_VENDOR:
            ret = memcpy_s(&info_frame->vendor_infoframe, sizeof(info_frame->vendor_infoframe),
                &hdmi_dev->info_frame.vendor_infoframe, sizeof(hdmi_vendor_infoframe));
            if (ret != EOK) {
                hdmi_err("secure function error:%d\n", ret);
                return OT_ERR_HDMI_INVALID_PARA;
            }
            break;
        case HDMI_INFOFRAME_TYPE_AVI:
            ret = memcpy_s(&info_frame->avi_infoframe, sizeof(info_frame->avi_infoframe),
                &hdmi_dev->info_frame.avi_infoframe, sizeof(hdmi_avi_infoframe));
            if (ret != EOK) {
                hdmi_err("secure function error:%d\n", ret);
                return OT_ERR_HDMI_INVALID_PARA;
            }
            break;
        case HDMI_INFOFRAME_TYPE_AUDIO:
            ret = memcpy_s(&info_frame->audio_infoframe, sizeof(info_frame->audio_infoframe),
                &hdmi_dev->info_frame.audio_infoframe, sizeof(hdmi_audio_infoframe));
            if (ret != EOK) {
                hdmi_err("secure function error:%d\n", ret);
                return OT_ERR_HDMI_INVALID_PARA;
            }
            break;
        default:
            return OT_ERR_HDMI_FEATURE_NO_SUPPORT;
    }
    hdmi_info("out\n");

    return TD_SUCCESS;
}

static td_void infoframe_set_vendor(hdmi_device *hdmi_dev, hdmi_infoframe *info_frame)
{
    errno_t ret;
    hdmi_user_vsif_content *src_vsif_content = TD_NULL;
    hdmi_user_vsif_content *dest_vsif_content = TD_NULL;

    src_vsif_content = &info_frame->vendor_infoframe.vsif_content.cea861_vsif;
    dest_vsif_content = &hdmi_dev->info_frame.vendor_infoframe.vsif_content.cea861_vsif;
    /*
     * save the user data to pstHdmiDev->puInfoFrame,
     * because it will get the data form it when user call get infoframe.
     */
    dest_vsif_content->len = src_vsif_content->len;
    ret = memcpy_s(dest_vsif_content->data, HDMI_VENDOR_USER_DATA_MAX_LEN,
        src_vsif_content->data, src_vsif_content->len);
    if (ret != EOK) {
        hdmi_err("secure function error:%d\n", ret);
        return;
    }
    /* first: save user data to pstHdmiDev->vendor_data */
    hdmi_dev->vendor_data.len = src_vsif_content->len;
    (td_void)memset_s(hdmi_dev->vendor_data.data, HDMI_VENDOR_USER_DATA_MAX_LEN, 0, HDMI_VENDOR_USER_DATA_MAX_LEN);
    ret = memcpy_s(hdmi_dev->vendor_data.data, HDMI_VENDOR_USER_DATA_MAX_LEN,
        src_vsif_content->data, src_vsif_content->len);
    if (ret != EOK) {
        hdmi_err("secure function error:%d\n", ret);
        return;
    }
    /* second: copy other information to puInfoFrame, except user data */
    src_vsif_content->ieee             = dest_vsif_content->ieee;
    src_vsif_content->format           = dest_vsif_content->format;
    src_vsif_content->vic              = dest_vsif_content->vic;
    src_vsif_content->_3d_meta_present = dest_vsif_content->_3d_meta_present;
    src_vsif_content->_3d_structure    = dest_vsif_content->_3d_structure;

    return;
}

static td_void infoframe_trace_avi(const hdmi_avi_infoframe *avi_info)
{
    hdmi_info("type: %u\n", avi_info->type);
    hdmi_info("version: %u\n", avi_info->version);
    hdmi_info("length: %u\n", avi_info->length);
    hdmi_info("colorspace: %u\n", avi_info->colorspace);
    hdmi_info("active_info_valid: %u\n", avi_info->active_info_valid);
    hdmi_info("horizontal_bar_valid: %u\n", avi_info->horizontal_bar_valid);
    hdmi_info("vertical_bar_valid: %u\n", avi_info->vertical_bar_valid);
    hdmi_info("scan_mode: %u\n", avi_info->scan_mode);
    hdmi_info("colorimetry: %u\n", avi_info->colorimetry);
    hdmi_info("picture_aspect: %u\n", avi_info->picture_aspect);
    hdmi_info("active_aspect: %u\n", avi_info->active_aspect);
    hdmi_info("extended_colorimetry: %u\n", avi_info->extended_colorimetry);
    hdmi_info("quantization_range: %u\n", avi_info->quantization_range);
    hdmi_info("video_timing: %u\n", avi_info->video_timing);
    hdmi_info("disp_fmt: %u\n", avi_info->disp_fmt);
    hdmi_info("ycc_quantization_range: %u\n", avi_info->ycc_quantization_range);
    hdmi_info("content_type: %u\n", avi_info->content_type);
    hdmi_info("pixel_repeat: %u\n", avi_info->pixel_repeat);
    hdmi_info("top_bar: %u\n", avi_info->top_bar);
    hdmi_info("bottom_bar: %u\n", avi_info->bottom_bar);
    hdmi_info("left_bar: %u\n", avi_info->left_bar);
    hdmi_info("right_bar: %u\n", avi_info->right_bar);
}

static td_void infoframe_sync_avi_vo_attr(hdmi_device *hdmi_dev, const hdmi_avi_infoframe *dest_avi_info)
{
    hdmi_vo_attr *vo_attr = &hdmi_dev->attr.vo_attr;

    if (hdmi_dev->attr.app_attr.auth_mode != TD_TRUE) {
        return;
    }

    vo_attr->picture_aspect       = dest_avi_info->picture_aspect;
    vo_attr->active_aspect        = dest_avi_info->active_aspect;
    vo_attr->colorimetry          = dest_avi_info->colorimetry;
    vo_attr->extended_colorimetry = dest_avi_info->extended_colorimetry;
    vo_attr->rgb_quantization     = dest_avi_info->quantization_range;
    vo_attr->ycc_quantization     = dest_avi_info->ycc_quantization_range;
    (td_void)drv_hdmi_vo_attr_set(hdmi_dev, vo_attr);
}

static td_void infoframe_set_avi(hdmi_device *hdmi_dev, const hdmi_infoframe *info_frame)
{
    errno_t ret;
    const hdmi_avi_infoframe *src_avi_info = &info_frame->avi_infoframe;
    hdmi_avi_infoframe *dest_avi_info = &hdmi_dev->info_frame.avi_infoframe;

    infoframe_trace_avi(src_avi_info);

    if ((src_avi_info->type != dest_avi_info->type) || (src_avi_info->colorspace != dest_avi_info->colorspace) ||
        (src_avi_info->video_timing != dest_avi_info->video_timing) ||
        (src_avi_info->disp_fmt != dest_avi_info->disp_fmt) ||
        (src_avi_info->quantization_range != dest_avi_info->quantization_range) ||
        (src_avi_info->ycc_quantization_range != dest_avi_info->ycc_quantization_range) ||
        (src_avi_info->pixel_repeat != dest_avi_info->pixel_repeat)) {
        hdmi_warn("The current infoframe does not match the original infoframe!\n");
    }
    ret = memcpy_s(dest_avi_info, sizeof(hdmi_dev->info_frame.avi_infoframe),
        src_avi_info, sizeof(hdmi_avi_infoframe));
    if (ret != EOK) {
        hdmi_err("secure function error:%d\n", ret);
        return;
    }

    infoframe_sync_avi_vo_attr(hdmi_dev, dest_avi_info);

    return;
}

static td_s32 drv_hdmi_infoframe_set(hdmi_device *hdmi_dev, hdmi_infoframe_id infoframe_id, hdmi_infoframe *info_frame)
{
    errno_t ret;

    hdmi_info("in, infoframe_id: %u\n", infoframe_id);

    switch (infoframe_id) {
        case HDMI_INFOFRAME_TYPE_VENDOR:
            infoframe_set_vendor(hdmi_dev, info_frame);
            break;
        case HDMI_INFOFRAME_TYPE_AVI:
            infoframe_set_avi(hdmi_dev, info_frame);
            break;
        case HDMI_INFOFRAME_TYPE_AUDIO: {
            hdmi_audio_infoframe *src_audio_info = &info_frame->audio_infoframe;
            hdmi_audio_infoframe *dest_audio_info = &hdmi_dev->info_frame.audio_infoframe;
            ret = memcpy_s(dest_audio_info, sizeof(hdmi_dev->info_frame.audio_infoframe),
                src_audio_info, sizeof(hdmi_audio_infoframe));
            if (ret != EOK) {
                hdmi_err("secure function error:%d\n", ret);
                return OT_ERR_HDMI_INVALID_PARA;
            }

            hdmi_info("type: %u\n", dest_audio_info->type);
            hdmi_info("version: %u\n", dest_audio_info->version);
            hdmi_info("length: %u\n", dest_audio_info->length);
            hdmi_info("channels: %u\n", dest_audio_info->channels);
            hdmi_info("coding_type: %u\n", dest_audio_info->coding_type);
            hdmi_info("sample_size: %u\n", dest_audio_info->sample_size);
            hdmi_info("sample_frequency: %u\n", dest_audio_info->sample_frequency);
            hdmi_info("coding_type_ext: %u\n", dest_audio_info->coding_type_ext);
            hdmi_info("channel_allocation: %u\n", dest_audio_info->channel_allocation);
            hdmi_info("lfe_playback_level: %u\n", dest_audio_info->lfe_playback_level);
            hdmi_info("level_shift_value: %u\n", dest_audio_info->level_shift_value);
            hdmi_info("downmix_inhibit: %u\n", dest_audio_info->downmix_inhibit);
            hdmi_info("level_shift_value: %u\n", dest_audio_info->level_shift_value);
            break;
        }
        default:
            return OT_ERR_HDMI_INVALID_PARA;
    }
    if ((infoframe_id != HDMI_INFOFRAME_TYPE_AVI) || (hdmi_dev->attr.app_attr.auth_mode == TD_FALSE)) {
        drv_hdmi_infoframe_send(&hdmi_dev->info_frame, infoframe_id, info_frame);
    }
    hdmi_info("out\n");

    return TD_SUCCESS;
}

static td_s32 drv_hdmi_status_get(const hdmi_device *hdmi_dev, hdmi_status *status)
{
    hdmi_hardware_status *hw_status = TD_NULL;

    hw_status = (hdmi_hardware_status *)osal_vmalloc(sizeof(hdmi_hardware_status));
    if (hw_status == TD_NULL) {
        hdmi_err("alloc hdmi_hardware_status struct memory fail\n");
        return TD_FAILURE;
    }
    (td_void)memset_s(hw_status, sizeof(hdmi_hardware_status), 0, sizeof(hdmi_hardware_status));

    hal_call_void(hdmi_dev, hal_hdmi_hardware_status_get, hw_status);

    /* some TV sometimes has no hot_plug but has rsen */
    status->connected = (hw_status->common_status.hotplug || hw_status->common_status.rsen);
    status->sink_power_on = hw_status->common_status.rsen;

    osal_vfree(hw_status);
    hw_status = TD_NULL;

    return TD_SUCCESS;
}

static td_s32 drv_hdmi_mod_param_set(hdmi_device *hdmi_dev, const drv_hdmi_mod_param *mode_param)
{
    hdmi_mode_param *param = TD_NULL;

    param = &hdmi_dev->mode_param;
    hdmi_info("set trace length: %u->%u, emi: %u->%u\n",
        param->trace_len, mode_param->trace_len, param->emi_en, mode_param->emi_en);
    param->emi_en = mode_param->emi_en;
    param->trace_len = mode_param->trace_len;

    return TD_SUCCESS;
}

static td_s32 drv_hdmi_mod_param_get(const hdmi_device *hdmi_dev, drv_hdmi_mod_param *mode_param)
{
    const hdmi_mode_param *param = TD_NULL;

    param = &hdmi_dev->mode_param;
    mode_param->emi_en = param->emi_en;
    mode_param->trace_len = param->trace_len;

    return TD_SUCCESS;
}

static td_s32 drv_hdmi_hw_spec_get(const hdmi_device *hdmi_dev, hdmi_hw_spec *hw_spec)
{
    td_s32 ret;

    hal_call_ret(hdmi_dev, ret, hal_hdmi_phy_hw_spec_get, hw_spec);

    return ret;
}

static td_s32 drv_hdmi_hw_spec_set(const hdmi_device *hdmi_dev, const hdmi_hw_spec *hw_spec)
{
    td_u8 i;
    td_u32 tmds_clk;
    td_s32 ret;

    tmds_clk = hdmi_dev->attr.vo_attr.hdmi_adapt_pix_clk;
    hdmi_info("tmds_clk : %u\n", tmds_clk);
    for (i = 0; i < HW_PARAM_ARRAY_COUNT; i++) {
        hdmi_info("stage[%u]: i_de_main_clk:%u i_de_main_data:%u\
            i_main_clk:%u i_main_data:%u ft_cap_clk:%u ft_cap_data:%u\n", i,
            hw_spec->hw_param[i].i_de_main_clk, hw_spec->hw_param[i].i_de_main_data,
            hw_spec->hw_param[i].i_main_clk, hw_spec->hw_param[i].i_main_data,
            hw_spec->hw_param[i].ft_cap_clk, hw_spec->hw_param[i].ft_cap_data);
    }
    hal_call_ret(hdmi_dev, ret, hal_hdmi_phy_hw_spec_set, tmds_clk, hw_spec);

    return ret;
}

static td_s32 hdmi21_caps_check(const hdmi_device *hdmi_dev, const hdmi_property *attr)
{
    td_bool is_4k;

    ot_unused(hdmi_dev);
    is_4k = (attr->disp_fmt == HDMI_VIDEO_FORMAT_3840X2160P_50) ||
            (attr->disp_fmt == HDMI_VIDEO_FORMAT_3840X2160P_60) ||
            (attr->disp_fmt == HDMI_VIDEO_FORMAT_4096X2160P_50) ||
            (attr->disp_fmt == HDMI_VIDEO_FORMAT_4096X2160P_60) ||
            (attr->disp_fmt == HDMI_VIDEO_FORMAT_7680X4320P_30);

    if (is_4k == TD_TRUE &&
        (attr->out_color_space == HDMI_COLORSPACE_RGB || attr->out_color_space == HDMI_COLORSPACE_YCBCR444) &&
        (attr->deep_color_mode == HDMI_DEEP_COLOR_30BIT || attr->deep_color_mode == HDMI_DEEP_COLOR_36BIT)) {
        hdmi_err("param is invalid, fmt = %u, colorspace = %u, deepcolor = %u\n",
            attr->disp_fmt, attr->out_color_space, attr->deep_color_mode);
        return OT_ERR_HDMI_FEATURE_NO_SUPPORT;
    }

    return TD_SUCCESS;
}

static td_s32 hdmi_tmds_check(const hdmi_device *hdmi_dev, const hdmi_property *attr)
{
    td_u32 attr_clk = attr->pix_clk;

    switch (attr->deep_color_mode) {
        case HDMI_DEEP_COLOR_24BIT:
            if (attr_clk < HDMI_TMDS_CLK_MIN) {
                hdmi_err("hdmi tmds clk: %d < 25000hz, not support!\n", attr->pix_clk);
                return OT_ERR_HDMI_FEATURE_NO_SUPPORT;
            }
            break;
        case HDMI_DEEP_COLOR_30BIT:
            hdmi_multiple_1p25(attr_clk);
            if (attr_clk < HDMI_TMDS_CLK_MIN) {
                hdmi_err("hdmi tmds clk: %d < 25000hz, not support!\n", attr_clk);
                return OT_ERR_HDMI_FEATURE_NO_SUPPORT;
            }
            break;
        case HDMI_DEEP_COLOR_36BIT:
            hdmi_multiple_1p5(attr_clk);
            if (attr_clk < HDMI_TMDS_CLK_MIN) {
                hdmi_err("hdmi tmds clk: %d < 25000hz, not support!\n", attr_clk);
                return OT_ERR_HDMI_FEATURE_NO_SUPPORT;
            }
            break;
        default:
            break;
    }
    return TD_SUCCESS;
}

static td_void hdmi_property_vo_attr_adapt(hdmi_vo_attr *vo_attr, const hdmi_property *prop)
{
    td_bool aspect_is43;

    vo_attr->disp_fmt = prop->disp_fmt;
    vo_attr->video_timing = prop->video_timing;
    vo_attr->clk_fs = prop->pix_clk;

    aspect_is43 = ((vo_attr->video_timing == HDMI_VIDEO_TIMING_640X480P_60000) ||
                   (vo_attr->video_timing == HDMI_VIDEO_TIMING_720X480P_60000) ||
                   (vo_attr->video_timing == HDMI_VIDEO_TIMING_720X576P_50000) ||
                   (vo_attr->video_timing == HDMI_VIDEO_TIMING_1440X240P_60000) ||
                   (vo_attr->video_timing == HDMI_VIDEO_TIMING_1440X288P_50000) ||
                   (vo_attr->video_timing == HDMI_VIDEO_TIMING_1440X480I_60000) ||
                   (vo_attr->video_timing == HDMI_VIDEO_TIMING_1440X576I_50000) ||
                   (vo_attr->video_timing == HDMI_VIDEO_TIMING_1440X576P_50000) ||
                   (vo_attr->video_timing == HDMI_VIDEO_TIMING_1440X576I_60000));
    if (aspect_is43 == TD_TRUE) {
        vo_attr->picture_aspect = HDMI_PICTURE_ASPECT_4_3;
    } else if (vo_attr->video_timing <= HDMI_VIDEO_TIMING_4096X2160P_120000 &&
               vo_attr->video_timing >= HDMI_VIDEO_TIMING_4096X2160P_24000) {
        vo_attr->picture_aspect = HDMI_PICTURE_ASPECT_256_135;
    } else {
        vo_attr->picture_aspect = HDMI_PICTURE_ASPECT_16_9;
    }

    return;
}

static td_s32 hdmi_property_backup_attr(const hdmi_device *hdmi_dev, hdmi_app_attr *tmp,
    hdmi_vo_attr *vo_attr, hdmi_ao_attr *ao_attr)
{
    td_s32 err_code;

    err_code = memcpy_s(tmp, sizeof(*tmp), &hdmi_dev->attr.app_attr, sizeof(hdmi_app_attr));
    if (err_code != EOK) {
        hdmi_err("secure function error:%d\n", err_code);
        return OT_ERR_HDMI_INVALID_PARA;
    }
    err_code = memcpy_s(vo_attr, sizeof(*vo_attr), &hdmi_dev->attr.vo_attr, sizeof(hdmi_vo_attr));
    if (err_code != EOK) {
        hdmi_err("secure function error:%d\n", err_code);
        return OT_ERR_HDMI_INVALID_PARA;
    }
    err_code = memcpy_s(ao_attr, sizeof(*ao_attr), &hdmi_dev->attr.ao_attr, sizeof(hdmi_ao_attr));
    if (err_code != EOK) {
        hdmi_err("secure function error:%d\n", err_code);
        return OT_ERR_HDMI_INVALID_PARA;
    }
    return TD_SUCCESS;
}

static td_void hdmi_property_apply_app_attr(hdmi_app_attr *app_attr, const hdmi_property *prop)
{
    app_attr->enable_hdmi = prop->enable_hdmi;
    app_attr->enable_video = prop->enable_video;
    app_attr->enable_audio = prop->enable_audio;
    app_attr->auth_mode = prop->auth_mode;
    app_attr->hdmi_action = prop->hdmi_action;
    app_attr->deep_color_mode = prop->deep_color_mode;
    app_attr->enable_avi_infoframe = prop->enable_avi_infoframe;
    app_attr->enable_aud_infoframe = prop->enable_aud_infoframe;
    app_attr->enable_deep_clr_adapt = prop->enable_deep_clr_adapt;
}

static td_s32 drv_hdmi_property_set(hdmi_device_id hdmi_id, const hdmi_property *prop)
{
    td_s32 ret;
    td_s32 err_code;
    hdmi_app_attr tmp = {0};
    hdmi_ao_attr ao_attr = {0};
    hdmi_vo_attr vo_attr = {0};
    hdmi_device *hdmi_dev = TD_NULL;
    hdmi_app_attr *app_attr = TD_NULL;

    hdmi_dev = get_hdmi_device(hdmi_id);
    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);
    ret = hdmi21_caps_check(hdmi_dev, prop);
    hdmi_if_failure_return(ret, OT_ERR_HDMI_FEATURE_NO_SUPPORT);
    /* hdmi does not support < 25000hz tmds clk */
    ret = hdmi_tmds_check(hdmi_dev, prop);
    hdmi_if_failure_return(ret, OT_ERR_HDMI_FEATURE_NO_SUPPORT);

    app_attr = &hdmi_dev->attr.app_attr;
    ret = hdmi_property_backup_attr(hdmi_dev, &tmp, &vo_attr, &ao_attr);
    hdmi_if_failure_return(ret, OT_ERR_HDMI_INVALID_PARA);

    hdmi_property_apply_app_attr(app_attr, prop);
    hdmi_property_vo_attr_adapt(&vo_attr, prop);
    ret = drv_hdmi_vo_attr_set(hdmi_dev, &vo_attr);
    if (ret != TD_SUCCESS) {
        goto err;
    }

    ao_attr.sample_fs    = prop->sample_rate;
    ao_attr.sample_depth = prop->bit_depth;
    ret = drv_hdmi_ao_attr_set(hdmi_dev, &ao_attr);
    if (ret != TD_SUCCESS) {
        goto err;
    }

    return TD_SUCCESS;

err:
    /* property set failed, recover the app_attr */
    err_code = memcpy_s(app_attr, sizeof(hdmi_dev->attr.app_attr), &tmp, sizeof(hdmi_app_attr));
    if (err_code != EOK) {
        hdmi_err("secure function error:%d\n", err_code);
        return OT_ERR_HDMI_INVALID_PARA;
    }

    return ret;
}

static td_s32 drv_hdmi_property_get(hdmi_device_id hdmi_id, hdmi_property *prop)
{
    hdmi_attr *attr = TD_NULL;
    hdmi_device *hdmi_dev = TD_NULL;

    hdmi_dev = get_hdmi_device(hdmi_id);
    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);

    attr = &hdmi_dev->attr;
    prop->enable_hdmi = attr->app_attr.enable_hdmi;
    prop->enable_video = attr->app_attr.enable_video;
    prop->enable_audio = attr->app_attr.enable_audio;
    prop->video_timing = attr->vo_attr.video_timing;
    prop->disp_fmt = attr->vo_attr.disp_fmt;
    prop->enable_avi_infoframe = attr->app_attr.enable_avi_infoframe;
    prop->enable_aud_infoframe = attr->app_attr.enable_aud_infoframe;
    prop->enable_vid_mode_adapt = attr->app_attr.enable_clr_space_adapt;
    prop->enable_deep_clr_adapt = attr->app_attr.enable_deep_clr_adapt;
    prop->auth_mode = attr->app_attr.auth_mode;
    prop->in_color_space = attr->vo_attr.in_color_space;
    prop->out_color_space = attr->app_attr.out_color_space;
    prop->deep_color_mode = attr->app_attr.deep_color_mode;
    prop->out_csc_quantization = attr->app_attr.out_csc_quantization;
    prop->hdmi_action = attr->app_attr.hdmi_action;
    prop->sample_rate = attr->ao_attr.sample_fs;
    prop->pix_clk = attr->vo_attr.clk_fs;
    prop->bit_depth = attr->ao_attr.sample_depth;

    return TD_SUCCESS;
}

static td_s32 hdmi_ioctrl_open(td_void *arg, td_bool user)
{
    td_s32 ret;
    hdmi_open *open = TD_NULL;
    hdmi_device *hdmi_dev = TD_NULL;

    hdmi_if_null_return(arg, OT_ERR_HDMI_NULL_PTR);

    open = (hdmi_open *)arg;
    hdmi_if_null_return(open, OT_ERR_HDMI_NULL_PTR);
    hdmi_dev = get_hdmi_device(open->hdmi_id);
    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);
    hdmi_if_false_return(hdmi_chip_id(hdmi_dev->hdmi_dev_id), OT_ERR_HDMI_INVALID_PARA);

    hdmi_info("hdmi_id:%d, CMD_HDMI_OPEN user=%u\n", hdmi_dev->hdmi_dev_id, user);

    hdmi_mutex_lock(g_hdmi_mutex);
    get_hdmi_default_action_set(hdmi_dev, open->default_mode);
    ret = drv_hdmi_open(hdmi_dev, user);
    hdmi_mutex_unlock(g_hdmi_mutex);

    return ret;
}

static td_s32 hdmi_ioctrl_close(td_void *arg, td_bool user)
{
    td_s32 ret;
    hdmi_device *hdmi_dev = TD_NULL;

    hdmi_if_null_return(arg, OT_ERR_HDMI_NULL_PTR);
    hdmi_dev = get_hdmi_device(*(hdmi_device_id *)arg);
    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);
    hdmi_if_false_return(hdmi_chip_id(hdmi_dev->hdmi_dev_id), OT_ERR_HDMI_INVALID_PARA);

    hdmi_info("hdmi_id:%d, CMD_HDMI_CLOSE user=%u\n", hdmi_dev->hdmi_dev_id, user);

    hdmi_mutex_lock(g_hdmi_mutex);
    ret = drv_hdmi_close(hdmi_dev, user);
    hdmi_mutex_unlock(g_hdmi_mutex);

    return ret;
}

static td_s32 hdmi_ioctrl_start(td_void *arg, td_bool user)
{
    td_s32 ret;
    hdmi_device *hdmi_dev = TD_NULL;

    hdmi_if_null_return(arg, OT_ERR_HDMI_NULL_PTR);
    hdmi_dev = get_hdmi_device(*(hdmi_device_id *)arg);
    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);
    hdmi_if_false_return(hdmi_chip_id(hdmi_dev->hdmi_dev_id), OT_ERR_HDMI_INVALID_PARA);

    hdmi_info("hdmi_id:%d, CMD_HDMI_START user=%u\n", hdmi_dev->hdmi_dev_id, user);

    hdmi_mutex_lock(g_hdmi_mutex);
    ret = drv_hdmi_start(hdmi_dev);
    hdmi_mutex_unlock(g_hdmi_mutex);

    return ret;
}

static td_s32 hdmi_ioctrl_stop(td_void *arg, td_bool user)
{
    td_s32 ret;
    hdmi_device *hdmi_dev = TD_NULL;

    hdmi_if_null_return(arg, OT_ERR_HDMI_NULL_PTR);
    hdmi_dev = get_hdmi_device(*(hdmi_device_id *)arg);
    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);
    hdmi_if_false_return(hdmi_chip_id(hdmi_dev->hdmi_dev_id), OT_ERR_HDMI_INVALID_PARA);

    hdmi_info("hdmi_id:%d, CMD_HDMI_STOP user=%u\n", hdmi_dev->hdmi_dev_id, user);

    hdmi_mutex_lock(g_hdmi_mutex);
    ret = drv_hdmi_stop(hdmi_dev);
    hdmi_mutex_unlock(g_hdmi_mutex);

    return ret;
}

static td_s32 hdmi_ioctrl_get_sink_capability(td_void *arg, td_bool user)
{
    td_s32 ret;
    errno_t err_code;
    hdmi_tx_capability_data tx_cap = {0};
    hdmi_device *hdmi_dev = TD_NULL;
    hdmi_sink_capability *sink_cap = TD_NULL;
    drv_hdmi_sink_capability *drv_sink_cap = TD_NULL;

    ot_unused(user);

    drv_sink_cap = (drv_hdmi_sink_capability *)arg;
    hdmi_if_null_return(drv_sink_cap, OT_ERR_HDMI_NULL_PTR);
    hdmi_dev = get_hdmi_device(drv_sink_cap->hdmi_id);
    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);
    hdmi_if_false_return(hdmi_chip_id(hdmi_dev->hdmi_dev_id), OT_ERR_HDMI_INVALID_PARA);

    hdmi_info("hdmi_id:%d, CMD_HDMI_GET_SINK_CAPABILITY user=%u\n", hdmi_dev->hdmi_dev_id, user);

    (td_void)memset_s(&drv_sink_cap->cap, sizeof(drv_sink_cap->cap), 0, sizeof(hdmi_sink_capability));
    hdmi_mutex_lock(g_hdmi_mutex);
    if (hdmi_dev->hal == TD_NULL || hdmi_dev->hal->hal_hdmi_tx_capability_get == TD_NULL) {
        hdmi_err("null pointer!\n");
        hdmi_mutex_unlock(g_hdmi_mutex);
        return OT_ERR_HDMI_NULL_PTR;
    }
    hdmi_dev->hal->hal_hdmi_tx_capability_get(hdmi_dev->hal, &tx_cap);
    if (drv_hdmi_edid_capability_get(&hdmi_dev->edid_info, &sink_cap) != HDMI_EDID_DATA_INVALID) {
        err_code = memcpy_s(&drv_sink_cap->cap, sizeof(drv_sink_cap->cap), sink_cap, sizeof(hdmi_sink_capability));
        hdmi_unlock_unequal_eok_return(err_code, g_hdmi_mutex, OT_ERR_HDMI_INVALID_PARA);
        ret = hdmi_capability_inter_section(&drv_sink_cap->cap, &tx_cap, hdmi_dev->attr.app_attr.auth_mode);
    } else {
        ret = OT_ERR_HDMI_READ_SINK_FAILED;
        hdmi_warn("no HPD, get sink capabity fail\n");
    }
    hdmi_mutex_unlock(g_hdmi_mutex);

    return ret;
}

static td_s32 hdmi_ioctrl_poll_event(td_void *arg, td_bool user)
{
    drv_hdmi_event *event = TD_NULL;
    hdmi_device *hdmi_dev = TD_NULL;

    ot_unused(user);
    event = (drv_hdmi_event *)arg;
    hdmi_if_null_return(event, OT_ERR_HDMI_NULL_PTR);
    hdmi_dev = get_hdmi_device(event->hdmi_id);
    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);
    hdmi_if_false_return(hdmi_chip_id(hdmi_dev->hdmi_dev_id), OT_ERR_HDMI_INVALID_PARA);

    return drv_hdmi_event_pool_read(event->hdmi_id, hdmi_current_id_get(hdmi_dev), &event->event);
}

static td_s32 hdmi_ioctrl_set_attr(td_void *arg, td_bool user)
{
    td_s32 ret;
    drv_hdmi_property *attr = TD_NULL;
    hdmi_device *hdmi_dev = TD_NULL;

    attr = (drv_hdmi_property *)arg;
    hdmi_if_null_return(attr, OT_ERR_HDMI_NULL_PTR);
    hdmi_dev = get_hdmi_device(attr->hdmi_id);
    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);
    hdmi_if_false_return(hdmi_chip_id(hdmi_dev->hdmi_dev_id), OT_ERR_HDMI_INVALID_PARA);

    hdmi_info("hdmi_id:%d, CMD_HDMI_SET_ATTR user=%u\n", hdmi_dev->hdmi_dev_id, user);

    hdmi_mutex_lock(g_hdmi_mutex);
    ret = drv_hdmi_property_set(attr->hdmi_id, &attr->prop);
    hdmi_mutex_unlock(g_hdmi_mutex);

    return ret;
}

static td_s32 hdmi_ioctrl_get_attr(td_void *arg, td_bool user)
{
    td_s32 ret;
    drv_hdmi_property *attr = TD_NULL;

    attr = (drv_hdmi_property *)arg;
    hdmi_if_null_return(attr, OT_ERR_HDMI_NULL_PTR);
    hdmi_if_false_return(hdmi_chip_id(attr->hdmi_id), OT_ERR_HDMI_INVALID_PARA);

    hdmi_info("hdmi_id:%d, CMD_HDMI_GET_ATTR user=%u\n", attr->hdmi_id, user);

    hdmi_mutex_lock(g_hdmi_mutex);
    ret = drv_hdmi_property_get(attr->hdmi_id, &attr->prop);
    hdmi_mutex_unlock(g_hdmi_mutex);

    return ret;
}

static td_s32 hdmi_ioctrl_get_vo_attr(td_void *arg, td_bool user)
{
    td_s32 ret;
    hdmi_device *hdmi_dev = TD_NULL;
    drv_hdmi_vo_attr *attr = TD_NULL;

    attr = (drv_hdmi_vo_attr *)arg;
    hdmi_if_null_return(attr, OT_ERR_HDMI_NULL_PTR);
    hdmi_dev = get_hdmi_device(attr->hdmi_id);
    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);
    hdmi_if_false_return(hdmi_chip_id(hdmi_dev->hdmi_dev_id), OT_ERR_HDMI_INVALID_PARA);
    (td_void)memset_s(&attr->vo_attr, sizeof(attr->vo_attr), 0, sizeof(hdmi_vo_attr));

    hdmi_info("hdmi_id:%d, CMD_HDMI_GET_VO_ATTR user=%u\n", hdmi_dev->hdmi_dev_id, user);

    hdmi_mutex_lock(g_hdmi_mutex);
    ret = drv_hdmi_vo_attr_get(hdmi_dev, &attr->vo_attr);
    hdmi_mutex_unlock(g_hdmi_mutex);

    return ret;
}

static td_s32 hdmi_ioctrl_set_vo_attr(td_void *arg, td_bool user)
{
    td_s32 ret;
    hdmi_device *hdmi_dev = TD_NULL;
    drv_hdmi_vo_attr *attr = TD_NULL;

    attr = (drv_hdmi_vo_attr *)arg;
    hdmi_if_null_return(attr, OT_ERR_HDMI_NULL_PTR);
    hdmi_dev = get_hdmi_device(attr->hdmi_id);
    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);
    hdmi_if_false_return(hdmi_chip_id(hdmi_dev->hdmi_dev_id), OT_ERR_HDMI_INVALID_PARA);

    hdmi_info("hdmi_id:%d, CMD_HDMI_SET_VO_ATTR user=%u\n", hdmi_dev->hdmi_dev_id, user);

    hdmi_mutex_lock(g_hdmi_mutex);
    ret = drv_hdmi_vo_attr_set(hdmi_dev, &attr->vo_attr);
    hdmi_mutex_unlock(g_hdmi_mutex);

    return ret;
}

static td_s32 hdmi_ioctrl_get_infoframe(td_void *arg, td_bool user)
{
    td_s32 ret;
    hdmi_device *hdmi_dev = TD_NULL;
    drv_hdmi_infoframe *tmp = TD_NULL;

    tmp = (drv_hdmi_infoframe *)arg;
    hdmi_if_null_return(tmp, OT_ERR_HDMI_NULL_PTR);
    hdmi_dev = get_hdmi_device(tmp->hdmi_id);
    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);
    hdmi_if_false_return(hdmi_chip_id(hdmi_dev->hdmi_dev_id), OT_ERR_HDMI_INVALID_PARA);

    hdmi_info("hdmi_id:%d, CMD_HDMI_GET_INFOFRAME user=%u\n", hdmi_dev->hdmi_dev_id, user);

    hdmi_mutex_lock(g_hdmi_mutex);
    ret = drv_hdmi_infoframe_get(hdmi_dev, tmp->infoframe_id, &tmp->infoframe);
    hdmi_mutex_unlock(g_hdmi_mutex);

    return ret;
}

static td_s32 hdmi_ioctrl_set_infoframe(td_void *arg, td_bool user)
{
    td_s32 ret;
    hdmi_device *hdmi_dev = TD_NULL;
    drv_hdmi_infoframe *infoframe = TD_NULL;

    infoframe = (drv_hdmi_infoframe *)arg;
    hdmi_if_null_return(infoframe, OT_ERR_HDMI_NULL_PTR);
    hdmi_dev = get_hdmi_device(infoframe->hdmi_id);
    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);
    hdmi_if_false_return(hdmi_chip_id(hdmi_dev->hdmi_dev_id), OT_ERR_HDMI_INVALID_PARA);

    hdmi_info("hdmi_id:%d, CMD_HDMI_SET_INFOFRAME user=%u\n", hdmi_dev->hdmi_dev_id, user);

    hdmi_mutex_lock(g_hdmi_mutex);
    ret = drv_hdmi_infoframe_set(hdmi_dev, infoframe->infoframe_id, &infoframe->infoframe);
    hdmi_mutex_unlock(g_hdmi_mutex);

    return ret;
}

static td_s32 hdmi_ioctrl_avmute(td_void *arg, td_bool user)
{
    td_bool audio_enable;
    hdmi_device *hdmi_dev = TD_NULL;
    drv_hdmi_avmute *drv_avmute = TD_NULL;

    drv_avmute = (drv_hdmi_avmute *)arg;
    hdmi_if_null_return(drv_avmute, OT_ERR_HDMI_NULL_PTR);
    hdmi_dev = get_hdmi_device(drv_avmute->hdmi_id);
    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);
    hdmi_if_false_return(hdmi_chip_id(hdmi_dev->hdmi_dev_id), OT_ERR_HDMI_INVALID_PARA);

    hdmi_info("hdmi_id:%d, CMD_HDMI_SET_AVMUTE user=%u\n", hdmi_dev->hdmi_dev_id, user);

    audio_enable = (hdmi_dev->attr.app_attr.enable_hdmi) && (hdmi_dev->attr.app_attr.enable_audio);

    hdmi_mutex_lock(g_hdmi_mutex);
    drv_hdmi_avmute_set(hdmi_dev, drv_avmute->avmute);
    if (drv_avmute->avmute) {
        drv_hdmi_audio_path_enable(hdmi_dev, !drv_avmute->avmute);
    } else {
        drv_hdmi_audio_path_enable(hdmi_dev, audio_enable);
    }
    hdmi_mutex_unlock(g_hdmi_mutex);

    return TD_SUCCESS;
}

static td_s32 hdmi_ioctrl_update_edid(td_void *arg, td_bool user)
{
    td_s32 ret;
    td_s32 edid_len;
    hdmi_device *hdmi_dev = TD_NULL;
    drv_hdmi_edid_raw_data *drv_edid_raw = TD_NULL;

    drv_edid_raw = (drv_hdmi_edid_raw_data *)arg;
    hdmi_if_null_return(drv_edid_raw, OT_ERR_HDMI_NULL_PTR);
    hdmi_dev = get_hdmi_device(drv_edid_raw->hdmi_id);
    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);
    hdmi_if_false_return(hdmi_chip_id(hdmi_dev->hdmi_dev_id), OT_ERR_HDMI_INVALID_PARA);

    hdmi_info("hdmi_id:%d, CMD_HDMI_UPDATE_EDID user=%u\n", hdmi_dev->hdmi_dev_id, user);

    (td_void)memset_s(&drv_edid_raw->edid_raw, sizeof(drv_edid_raw->edid_raw), 0, sizeof(hdmi_edid_raw_data));
    hdmi_mutex_lock(g_hdmi_mutex);
    ret = drv_hdmi_edid_update(&hdmi_dev->edid_info, HDMI_EDID_UPDATE_SINK);
    if (ret == TD_SUCCESS) {
        edid_len = drv_hdmi_edid_raw_get(&hdmi_dev->edid_info, drv_edid_raw->edid_raw.edid, HDMI_EDID_SIZE);
        if (edid_len != OT_ERR_HDMI_NULL_PTR && edid_len != OT_ERR_HDMI_INVALID_PARA && edid_len != TD_FAILURE) {
            drv_edid_raw->edid_raw.edid_len = (td_u32)edid_len;
            drv_edid_raw->edid_raw.edid_valid = 1;
        }
    }
    hdmi_mutex_unlock(g_hdmi_mutex);

    return ret;
}

static td_s32 hdmi_ioctrl_get_status(td_void *arg, td_bool user)
{
    td_s32 ret;
    hdmi_device *hdmi_dev = TD_NULL;
    drv_hdmi_status *hdmi_state = TD_NULL;

    hdmi_state = (drv_hdmi_status *)arg;
    hdmi_if_null_return(hdmi_state, OT_ERR_HDMI_NULL_PTR);
    hdmi_dev = get_hdmi_device(hdmi_state->hdmi_id);
    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);
    hdmi_if_false_return(hdmi_chip_id(hdmi_dev->hdmi_dev_id), OT_ERR_HDMI_INVALID_PARA);

    hdmi_info("hdmi_id:%d, CMD_HDMI_GET_STATUS user=%u\n", hdmi_dev->hdmi_dev_id, user);

    (td_void)memset_s(&hdmi_state->status, sizeof(hdmi_state->status), 0, sizeof(hdmi_status));
    hdmi_mutex_lock(g_hdmi_mutex);
    ret = drv_hdmi_status_get(hdmi_dev, &hdmi_state->status);
    hdmi_mutex_unlock(g_hdmi_mutex);

    return ret;
}

static td_s32 hdmi_ioctrl_register_callback(td_void *arg, td_bool user)
{
    td_bool hpd = TD_FALSE;
    td_bool intr_status = TD_FALSE;
    hdmi_device *hdmi_dev = TD_NULL;

    hdmi_if_null_return(arg, OT_ERR_HDMI_NULL_PTR);
    hdmi_dev = get_hdmi_device(*(hdmi_device_id *)arg);
    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);
    hdmi_if_false_return(hdmi_chip_id(hdmi_dev->hdmi_dev_id), OT_ERR_HDMI_INVALID_PARA);

    hdmi_info("hdmi_id:%d, CMD_HDMI_REGISTER_CALLBACK user=%u\n", hdmi_dev->hdmi_dev_id, user);

    hdmi_mutex_lock(g_hdmi_mutex);
    if ((hdmi_dev->user_cnt != 0) || (hdmi_dev->kernel_cnt != 0)) {
        hdmi_dev->user_callback_cnt++;
        hdmi_mutex_unlock(g_hdmi_mutex);
        hdmi_mutex_lock(hdmi_dev->mutex_thread);
        hdmi_dev->k_callback = TD_NULL;
        /*
         * HPD needs to be reported when the following conditions are met:
         * 1. the driver has reported HPD before registering the callback.
         * 2. there is currently no pending hpd interrupt.
         * 3. hotplug pin level is high.
         */
        hal_call_void(hdmi_dev, hal_hdmi_hdp_intr_status_get, &intr_status);
        if (hdmi_dev->hpd_notifyed == TD_TRUE && intr_status == TD_FALSE) {
            hal_call_void(hdmi_dev, hal_hdmi_hot_plug_status_get, &hpd);
            if (hpd == TD_TRUE) {
                drv_hdmi_event_pool_write(hdmi_dev->hdmi_dev_id, HDMI_EVENT_HOTPLUG);
            }
        }
        hdmi_mutex_unlock(hdmi_dev->mutex_thread);
        hdmi_mutex_lock(g_hdmi_mutex);
    }
    hdmi_mutex_unlock(g_hdmi_mutex);

    return TD_SUCCESS;
}

static td_s32 hdmi_ioctrl_ungister_callback(td_void *arg, td_bool user)
{
    hdmi_device *hdmi_dev = TD_NULL;

    hdmi_if_null_return(arg, OT_ERR_HDMI_NULL_PTR);
    hdmi_dev = get_hdmi_device(*(hdmi_device_id *)arg);
    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);
    hdmi_if_false_return(hdmi_chip_id(hdmi_dev->hdmi_dev_id), OT_ERR_HDMI_INVALID_PARA);

    hdmi_info("hdmi_id:%d, CMD_HDMI_UNREGISTER_CALLBACK user=%u\n", hdmi_dev->hdmi_dev_id, user);

    hdmi_mutex_lock(g_hdmi_mutex);
    if (hdmi_dev->user_callback_cnt > 0) {
        hdmi_dev->user_callback_cnt--;
    }
    if (hdmi_dev->user_callback_cnt == 0) {
        hdmi_mutex_unlock(g_hdmi_mutex);
        hdmi_mutex_lock(hdmi_dev->mutex_thread);
        hdmi_dev->k_callback = drv_hdmi_kernel_event_callback;
        hdmi_mutex_unlock(hdmi_dev->mutex_thread);
        hdmi_mutex_lock(g_hdmi_mutex);
    }
    hdmi_mutex_unlock(g_hdmi_mutex);

    return TD_SUCCESS;
}

static td_s32 hdmi_ioctrl_set_mod_param(td_void *arg, td_bool user)
{
    td_s32 ret;
    hdmi_device *hdmi_dev = TD_NULL;
    drv_hdmi_mod_param *mod_param = TD_NULL;

    mod_param = (drv_hdmi_mod_param *)arg;
    hdmi_if_null_return(mod_param, OT_ERR_HDMI_NULL_PTR);
    hdmi_dev = get_hdmi_device(mod_param->hdmi_id);
    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);
    hdmi_if_false_return(hdmi_chip_id(hdmi_dev->hdmi_dev_id), OT_ERR_HDMI_INVALID_PARA);

    hdmi_info("hdmi_id:%d, CMD_HDMI_SET_MOD_PARAM user=%u\n", hdmi_dev->hdmi_dev_id, user);

    hdmi_mutex_lock(g_hdmi_mutex);
    ret = drv_hdmi_mod_param_set(hdmi_dev, mod_param);
    hdmi_mutex_unlock(g_hdmi_mutex);

    return ret;
}

static td_s32 hdmi_ioctrl_get_mod_param(td_void *arg, td_bool user)
{
    td_s32 ret;
    hdmi_device *hdmi_dev = TD_NULL;
    drv_hdmi_mod_param *mod_param = TD_NULL;

    mod_param = (drv_hdmi_mod_param *)arg;
    hdmi_if_null_return(mod_param, OT_ERR_HDMI_NULL_PTR);
    hdmi_dev = get_hdmi_device(mod_param->hdmi_id);
    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);
    hdmi_if_false_return(hdmi_chip_id(hdmi_dev->hdmi_dev_id), OT_ERR_HDMI_INVALID_PARA);

    hdmi_info("hdmi_id:%d, CMD_HDMI_GET_MOD_PARAM user=%u\n", hdmi_dev->hdmi_dev_id, user);

    hdmi_mutex_lock(g_hdmi_mutex);
    ret = drv_hdmi_mod_param_get(hdmi_dev, mod_param);
    hdmi_mutex_unlock(g_hdmi_mutex);

    return ret;
}

static td_s32 hdmi_ioctrl_get_hw_spec(td_void *arg, td_bool user)
{
    td_s32 ret;
    hdmi_device *hdmi_dev = TD_NULL;
    drv_hdmi_hw_spec *spec = TD_NULL;

    spec = (drv_hdmi_hw_spec *)arg;
    hdmi_if_null_return(spec, OT_ERR_HDMI_NULL_PTR);
    hdmi_dev = get_hdmi_device(spec->hdmi_id);
    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);
    hdmi_if_false_return(hdmi_chip_id(hdmi_dev->hdmi_dev_id), OT_ERR_HDMI_INVALID_PARA);

    hdmi_info("hdmi_id:%d, CMD_HDMI_GET_HW_SPEC user=%u\n", hdmi_dev->hdmi_dev_id, user);

    (td_void)memset_s(&spec->hw_spec, sizeof(spec->hw_spec), 0, sizeof(hdmi_hw_spec));
    hdmi_mutex_lock(g_hdmi_mutex);
    ret = drv_hdmi_hw_spec_get(hdmi_dev, &spec->hw_spec);
    hdmi_mutex_unlock(g_hdmi_mutex);

    return ret;
}

static td_s32 hdmi_ioctrl_set_hw_spec(td_void *arg, td_bool user)
{
    td_s32 ret;
    hdmi_device *hdmi_dev = TD_NULL;
    drv_hdmi_hw_spec *spec = TD_NULL;

    spec = (drv_hdmi_hw_spec *)arg;
    hdmi_if_null_return(spec, OT_ERR_HDMI_NULL_PTR);
    hdmi_dev = get_hdmi_device(spec->hdmi_id);
    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);
    hdmi_if_false_return(hdmi_chip_id(hdmi_dev->hdmi_dev_id), OT_ERR_HDMI_INVALID_PARA);

    hdmi_info("hdmi_id:%d, CMD_HDMI_SET_HW_SPEC user=%u\n", hdmi_dev->hdmi_dev_id, user);

    hdmi_mutex_lock(g_hdmi_mutex);
    ret = drv_hdmi_hw_spec_set(hdmi_dev, &spec->hw_spec);
    hdmi_mutex_unlock(g_hdmi_mutex);

    return ret;
}

#ifndef CONFIG_COMPAT
static long hdmi_ioctl(unsigned int cmd, unsigned long arg, void *private_data)
{
    ot_unused(private_data);
    return drv_hdmi_cmd_process(cmd, (td_void *)(uintptr_t)arg, TD_TRUE);
}
#else
static td_s32 drv_hdmi_compat_cmd_process(unsigned int cmd, td_void *arg, td_bool user)
{
    td_void __user *argp = (td_void __user *)arg;
    return drv_hdmi_cmd_process(cmd, argp, user);
}

static long hdmi_compact_ioctl(unsigned int cmd, unsigned long arg, void *private_data)
{
    ot_unused(private_data);
    return drv_hdmi_compat_cmd_process(cmd, (td_void *)(uintptr_t)arg, TD_TRUE);
}
#endif

static td_s32 hdmi_file_open(void *private_data)
{
    ot_unused(private_data);
    osal_atomic_inc_return(&g_hdmi_count);

    return TD_SUCCESS;
}

static td_s32 hdmi_file_close(void *private_data)
{
    td_u32 hdmi_id = 0;
    hdmi_device *hdmi_dev = TD_NULL;

    ot_unused(private_data);
    if (osal_atomic_read(&g_hdmi_count)) {
        osal_atomic_dec_return(&g_hdmi_count);
        for (; hdmi_id < HDMI_ID_MAX; hdmi_id++) {
            hdmi_dev = get_hdmi_device(hdmi_id);
            hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);
            if ((td_u32)hdmi_dev->run_state & HDMI_RUN_STATE_OPEN) {
                hdmi_mutex_lock(g_hdmi_mutex);
                drv_hdmi_close(hdmi_dev, TD_TRUE);
                hdmi_mutex_unlock(g_hdmi_mutex);
            }
        }
    }

    return TD_SUCCESS;
}

static long hdmi_file_ioctl(unsigned int cmd, unsigned long arg, void *private_data)
{
#ifndef CONFIG_COMPAT
    return (long)hdmi_ioctl(cmd, arg, private_data);
#else
    return (long)hdmi_compact_ioctl(cmd, arg, private_data);
#endif
}

static td_s32 hdmi_init(td_void *args)
{
    ot_unused(args);
    return 0;
}

static td_void hdmi_exit(td_void)
{
    return;
}

static td_void hdmi_notify(mod_notice_id notice)
{
    ot_unused(notice);
    return;
}

static td_void hdmi_query_state(mod_state *state)
{
    *state = MOD_STATE_FREE;
    return;
}

static td_u32 hdmi_get_ver_magic(td_void)
{
    return VERSION_MAGIC;
}

static td_s32 hdmi_dev_register(td_void)
{
    td_char ch_devfs_name[DEVFS_NAME_LEN] = {0};

    if (cmpi_register_module(&g_module)) {
        hdmi_warn("cmpi_register_module hdmi fail \n");
        return TD_FAILURE;
    }

    /* register hdmi device */
    if (snprintf_s(ch_devfs_name, DEVFS_NAME_LEN, DEVFS_NAME_LEN - 1, "%s", UMAP_DEVNAME_HDMI_BASE) < 0) {
        hdmi_err("snprintf_s err\n");
        return TD_FAILURE;
    }
    g_hdmi_device = osal_createdev(ch_devfs_name);
    hdmi_if_null_return(g_hdmi_device, OT_ERR_HDMI_NULL_PTR);
    g_hdmi_device->fops  = &g_hdmi_file_ops;
    g_hdmi_device->minor = UMAP_HDMI_MINOR_BASE;
    if (osal_registerdevice(g_hdmi_device) < 0) {
        hdmi_warn("g_hdmi_device register failed\n");
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

#ifdef CONFIG_OT_PROC_SHOW_SUPPORT
static td_void drv_hdmi_proc_add_module(const td_char *entry_name, const hdmi_proc_item *proc_item)
{
    osal_proc_entry_t *proc = NULL;

    proc = osal_create_proc_entry(entry_name, NULL);
    hdmi_if_null_return_void(proc);
    proc->private = (td_void *)proc_item->private;
    proc->read = (td_void *)proc_item->read;
    proc->write = (td_void *)proc_item->write;

    return;
}

static td_void drv_hdmi_proc_remove_module(const td_char *entry_name, td_u32 arry_len)
{
    ot_unused(arry_len);
    osal_remove_proc_entry(entry_name, NULL);
    return;
}

static td_void drv_hdmi_proc_register(td_u32 hdmi_id)
{
    hdmi_device *hdmi_dev = TD_NULL;
    td_char proc_name[PROC_NAME_MAX] = {0};

    hdmi_dev = get_hdmi_device(hdmi_id);
    hdmi_if_null_return_void(hdmi_dev);

    if (snprintf_s(proc_name, PROC_NAME_MAX, PROC_NAME_MAX - 1, "%s%u", "hdmi", hdmi_id) < 0) {
        hdmi_err("snprintf_s err.\n");
        return;
    }

    g_hdmi_proc_item.private = (td_void *)hdmi_dev;
    drv_hdmi_proc_add_module(proc_name, &g_hdmi_proc_item);
    (td_void)memset_s(proc_name, PROC_NAME_MAX, 0, sizeof(proc_name));
    if (snprintf_s(proc_name, PROC_NAME_MAX, PROC_NAME_MAX - 1, "%s%u%s", "hdmi", hdmi_id, "_vo") < 0) {
        hdmi_err("snprintf_s err.\n");
        return;
    }
    g_vo_proc_item.private = (td_void *)hdmi_dev;
    drv_hdmi_proc_add_module(proc_name, &g_vo_proc_item);
    (td_void)memset_s(proc_name, PROC_NAME_MAX, 0, sizeof(proc_name));
    if (snprintf_s(proc_name, PROC_NAME_MAX, PROC_NAME_MAX - 1, "%s%u%s", "hdmi", hdmi_id, "_ao") < 0) {
        hdmi_err("snprintf_s err.\n");
        return;
    }
    g_ao_proc_item.private = (td_void *)hdmi_dev;
    drv_hdmi_proc_add_module(proc_name, &g_ao_proc_item);
    (td_void)memset_s(proc_name, PROC_NAME_MAX, 0, sizeof(proc_name));
    if (snprintf_s(proc_name, PROC_NAME_MAX, PROC_NAME_MAX - 1, "%s%u%s", "hdmi", hdmi_id, "_sink") < 0) {
        hdmi_err("snprintf_s err.\n");
        return;
    }
    g_sink_proc_item.private = (td_void *)hdmi_dev;
    drv_hdmi_proc_add_module(proc_name, &g_sink_proc_item);

    return;
}

static td_void hdmi_proc_register(td_void)
{
    td_u32 hdmi_id;
    hdmi_device *hdmi_dev = TD_NULL;

    for (hdmi_id = 0; hdmi_id < HDMI_ID_MAX; hdmi_id++) {
        hdmi_dev = get_hdmi_device(hdmi_id);
        if (hdmi_dev != TD_NULL) {
            drv_hdmi_proc_register(hdmi_id);
        }
    }

    return;
}

static td_void drv_hdmi_proc_un_register(td_u32 hdmi_id)
{
    td_char proc_name[PROC_NAME_MAX] = {0};

    if (snprintf_s(proc_name, PROC_NAME_MAX, PROC_NAME_MAX - 1, "%s%u", "hdmi", hdmi_id) < 0) {
        hdmi_err("snprintf_s err.\n");
        return;
    }
    drv_hdmi_proc_remove_module(proc_name, PROC_NAME_MAX);
    (td_void)memset_s(proc_name, PROC_NAME_MAX, 0, sizeof(proc_name));
    if (snprintf_s(proc_name, PROC_NAME_MAX, PROC_NAME_MAX - 1, "%s%u%s", "hdmi", hdmi_id, "_vo") < 0) {
        hdmi_err("snprintf_s err.\n");
        return;
    }
    drv_hdmi_proc_remove_module(proc_name, PROC_NAME_MAX);
    (td_void)memset_s(proc_name, PROC_NAME_MAX, 0, sizeof(proc_name));
    if (snprintf_s(proc_name, PROC_NAME_MAX, PROC_NAME_MAX - 1, "%s%u%s", "hdmi", hdmi_id, "_ao") < 0) {
        hdmi_err("snprintf_s err.\n");
        return;
    }
    drv_hdmi_proc_remove_module(proc_name, PROC_NAME_MAX);
    (td_void)memset_s(proc_name, PROC_NAME_MAX, 0, sizeof(proc_name));
    if (snprintf_s(proc_name, PROC_NAME_MAX, PROC_NAME_MAX - 1, "%s%u%s", "hdmi", hdmi_id, "_sink") < 0) {
        hdmi_err("snprintf_s err.\n");
        return;
    }
    drv_hdmi_proc_remove_module(proc_name, PROC_NAME_MAX);

    return;
}
#endif

static const hdmi_ioctrl_func g_hdmi_cmd_func_tab[] = {
    { CMD_HDMI_OPEN,                hdmi_ioctrl_open },
    { CMD_HDMI_CLOSE,               hdmi_ioctrl_close },
    { CMD_HDMI_START,               hdmi_ioctrl_start },
    { CMD_HDMI_STOP,                hdmi_ioctrl_stop },
    { CMD_HDMI_GET_SINK_CAPABILITY, hdmi_ioctrl_get_sink_capability },
    { CMD_HDMI_POLL_EVENT,          hdmi_ioctrl_poll_event },
    { CMD_HDMI_SET_ATTR,            hdmi_ioctrl_set_attr },
    { CMD_HDMI_GET_ATTR,            hdmi_ioctrl_get_attr },
    { CMD_HDMI_GET_VO_ATTR,         hdmi_ioctrl_get_vo_attr },
    { CMD_HDMI_SET_VO_ATTR,         hdmi_ioctrl_set_vo_attr },
    { CMD_HDMI_GET_INFOFRAME,       hdmi_ioctrl_get_infoframe },
    { CMD_HDMI_SET_INFOFRAME,       hdmi_ioctrl_set_infoframe },
    { CMD_HDMI_SET_AVMUTE,          hdmi_ioctrl_avmute },
    { CMD_HDMI_UPDATE_EDID,         hdmi_ioctrl_update_edid },
    { CMD_HDMI_GET_STATUS,          hdmi_ioctrl_get_status },
    { CMD_HDMI_REGISTER_CALLBACK,   hdmi_ioctrl_register_callback },
    { CMD_HDMI_UNREGISTER_CALLBACK, hdmi_ioctrl_ungister_callback },
    { CMD_HDMI_SET_MOD_PARAM,       hdmi_ioctrl_set_mod_param },
    { CMD_HDMI_GET_MOD_PARAM,       hdmi_ioctrl_get_mod_param },
    { CMD_HDMI_GET_HW_SPEC,         hdmi_ioctrl_get_hw_spec },
    { CMD_HDMI_SET_HW_SPEC,         hdmi_ioctrl_set_hw_spec }
};

td_s32 hdmi_mode_strategy(hdmi_device *hdmi_dev)
{
    hdmi_tmds_mode tmds_mode;
    hdmi_app_attr *app_attr = TD_NULL;
#ifdef HDMI_SCDC_SUPPORT
    hdmi_edid_data edid_ret;
    hdmi_scdc_status scdc_get = {0};
    hdmi_scdc_status scdc_status = {0};
    hdmi_vo_attr *vo_attr = TD_NULL;
    hdmi_sink_capability *sink_cap = TD_NULL;
#endif

    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);
    app_attr = &hdmi_dev->attr.app_attr;

#ifdef HDMI_SCDC_SUPPORT
    vo_attr = &hdmi_dev->attr.vo_attr;
    edid_ret = drv_hdmi_edid_capability_get(&hdmi_dev->edid_info, &sink_cap);
    if (edid_ret == HDMI_EDID_DATA_INVALID) {
        hdmi_warn("invalid edid_capability!\n");
    }
    if (hdmi_scdc_status_get(app_attr, vo_attr, &scdc_status, &tmds_mode) != TD_SUCCESS) {
        return TD_FAILURE;
    }
#else
    tmds_mode = (app_attr->enable_hdmi == TD_TRUE) ? HDMI_TMDS_MODE_HDMI_1_4 : HDMI_TMDS_MODE_DVI;
#endif

    hdmi_info("tmds mode %u->%u.\n", hdmi_dev->tmds_mode, tmds_mode);
    hdmi_dev->tmds_mode = tmds_mode;
    hal_call_void(hdmi_dev, hal_hdmi_tmds_mode_set, hdmi_dev->tmds_mode);
    /* reset controller when the controller and phy configuration is completed. */
    hal_call_void(hdmi_dev, hal_hdmi_ctrl_reset);

#ifdef HDMI_SCDC_SUPPORT
    if (sink_cap->support_scdc == TD_TRUE || hdmi_dev->attr.app_attr.auth_mode == TD_TRUE) {
        hal_call_void(hdmi_dev, hal_hdmi_scdc_status_get, &scdc_get);
        if ((scdc_get.sink_scramble_on != scdc_status.sink_scramble_on ||
            scdc_get.source_scramble_on != scdc_status.source_scramble_on ||
            scdc_get.tmds_bit_clk_ratio != scdc_status.tmds_bit_clk_ratio) ||
            (hdmi_dev->attr.app_attr.auth_mode == TD_TRUE)) {
            scdc_status.scramble_interval = SCDC_SCRAMBLE_INTERVAL_RESET;
            scdc_status.scramble_timeout = SCDC_SCRAMBLE_TIMEOUT_RESET;
            scdc_status.sink_read_quest = TD_FALSE;
            hal_call_void(hdmi_dev, hal_hdmi_scdc_status_set, &scdc_status);
        }
    } else {
        hdmi_info("sink not support SCDC\n");
    }
#endif

    return TD_SUCCESS;
}

td_s32 drv_hdmi_start(hdmi_device *hdmi_dev)
{
    td_bool audio_enable;
    hdmi_app_attr *app_attr = TD_NULL;
    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);
    hdmi_check_open_return((td_u32)hdmi_dev->run_state);

    app_attr = &hdmi_dev->attr.app_attr;
    audio_enable = (hdmi_dev->attr.app_attr.enable_audio && app_attr->enable_hdmi);
#if defined(HDMI_PRODUCT_SS626V100)
    drv_hdmi_low_power_set(hdmi_dev->hdmi_dev_id, TD_FALSE);
#else
    drv_hdmi_low_power_set(TD_FALSE);
#endif

    if (hdmi_mode_strategy(hdmi_dev) != TD_SUCCESS) {
        hdmi_err(" hdmi_mode_strategy fail\n");
    }

    /*
     * 20160415, fix SAMSUNG TV(UA55JU6400JXXZ) doesn't show at 10bit/12bit.
     * this TV clear mute must after phy enable when the deepcolor is 10bit and 12bit.
     */
    hal_call_void(hdmi_dev, hal_hdmi_avmute_set, TD_FALSE);

    hdmi_info("TMDS phy config\n");
    hdmi_phy_output_enable(hdmi_dev, TD_TRUE);
    drv_hdmi_avmute_set(hdmi_dev, TD_FALSE);
    drv_hdmi_audio_path_enable(hdmi_dev, audio_enable);

    if (hdmi_dev->attr.app_attr.enable_video == TD_FALSE) {
        hdmi_info("video was disable by user, so send blackframe.\n");
        drv_hdmi_black_data_set(hdmi_dev, TD_TRUE);
    } else {
        drv_hdmi_black_data_set(hdmi_dev, TD_FALSE);
    }
    hdmi_dev->run_state = (td_u32)hdmi_dev->run_state & (~(HDMI_RUN_STATE_STOP));
    hdmi_dev->run_state = (td_u32)hdmi_dev->run_state | (HDMI_RUN_STATE_START);

    return TD_SUCCESS;
}

td_s32 drv_hdmi_stop(hdmi_device *hdmi_dev)
{
    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);

    hdmi_debug_delay(hdmi_dev, "drv_hdmi_stop debug start");
    if (!((td_u32)hdmi_dev->run_state & HDMI_RUN_STATE_START) || ((td_u32)hdmi_dev->run_state & HDMI_RUN_STATE_STOP)) {
        return TD_SUCCESS;
    }
    hdmi_debug_delay(hdmi_dev, "drv_hdmi_stop audio path disable");
    drv_hdmi_audio_path_enable(hdmi_dev, TD_FALSE);
    hdmi_debug_delay(hdmi_dev, "drv_hdmi_stop black data enable");
    drv_hdmi_black_data_set(hdmi_dev, TD_TRUE);
    hdmi_debug_delay(hdmi_dev, "drv_hdmi_stop avmute enable");
    drv_hdmi_avmute_set(hdmi_dev, TD_TRUE);
    hdmi_debug_delay(hdmi_dev, "drv_hdmi_stop phy disable");
    hdmi_phy_output_enable(hdmi_dev, TD_FALSE);
    /* disable clk for low power */
#if defined(HDMI_PRODUCT_SS626V100)
    drv_hdmi_low_power_set(hdmi_dev->hdmi_dev_id, TD_TRUE);
#else
    drv_hdmi_low_power_set(TD_TRUE);
#endif
    hdmi_dev->run_state = (td_u32)hdmi_dev->run_state  & (~(HDMI_RUN_STATE_START));
    hdmi_dev->run_state = (td_u32)hdmi_dev->run_state | HDMI_RUN_STATE_STOP;
    drv_hdmi_compat_stop_delay(hdmi_dev->hdmi_dev_id);
    hdmi_debug_delay(hdmi_dev, "stop delay");

    return TD_SUCCESS;
}

td_void drv_hdmi_avmute_set(const hdmi_device *hdmi_dev, td_bool avmute_en)
{
    hdmi_delay delay = {0};

    hdmi_if_null_return_void(hdmi_dev);

    hdmi_info("avmute: %u\n", avmute_en);

    delay.fmt_delay  = hdmi_dev->delay.fmt_delay;
    delay.mute_delay = hdmi_dev->delay.mute_delay;
    drv_hdmi_compat_delay_get(hdmi_dev->hdmi_dev_id, &delay);

    if (hdmi_dev->attr.app_attr.auth_mode != TD_TRUE) {
        if (avmute_en == TD_FALSE) {
            hdmi_info("format_delay %u ms \n", delay.fmt_delay);
            osal_msleep(delay.fmt_delay);
        }
    }
    hal_call_void(hdmi_dev, hal_hdmi_avmute_set, avmute_en);

    if (hdmi_dev->attr.app_attr.auth_mode != TD_TRUE) {
        if (avmute_en == TD_TRUE) {
            hdmi_info("mute_delay %u ms \n", delay.mute_delay);
            osal_msleep(delay.mute_delay);
        }
    }

    return;
}

td_s32 hdmi_thread_state_set(hdmi_device *hdmi_dev, hdmi_thread_state state)
{
    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);

    hdmi_info("state: %u\n", state);
    hdmi_dev->thread_info.thread_timer_sate = state;

    return TD_SUCCESS;
}

td_s32 drv_hdmi_vo_attr_set(hdmi_device *hdmi_dev, const hdmi_vo_attr *vo_attr)
{
    errno_t ret;
    hdmi_attr hw_attr = {0};
    hdmi_vo_attr *video_attr = TD_NULL;
    hdmi_app_attr *app_attr = TD_NULL;
    hdmi_video_timing timing_bak;

    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);
    hdmi_if_null_return(vo_attr, OT_ERR_HDMI_NULL_PTR);

    if (check_video_attr(vo_attr) != TD_SUCCESS) {
        return OT_ERR_HDMI_INVALID_PARA;
    }
    video_attr = &hdmi_dev->attr.vo_attr;
    app_attr   = &hdmi_dev->attr.app_attr;
    timing_bak = video_attr->video_timing;
    ret = memcpy_s(video_attr, sizeof(hdmi_dev->attr.vo_attr), vo_attr, sizeof(hdmi_vo_attr));
    if (ret != EOK) {
        hdmi_err("secure function error:%d\n", ret);
        return OT_ERR_HDMI_INVALID_PARA;
    }
    hdmi_vo_attr_info(hdmi_dev, video_attr);
    if (hdmi_format_is_change(hdmi_dev, video_attr, vo_attr) != TD_SUCCESS) {
        return OT_ERR_HDMI_STRATEGY_FAILED;
    }

    hdmi_attr_construct(hdmi_dev, &hw_attr);
    hw_attr.vo_attr.video_timing = timing_bak;
    if (vo_attr_is_changed(&hw_attr.vo_attr, video_attr) == TD_FALSE &&
        app_attr_is_changed(&hw_attr.app_attr, app_attr) == TD_FALSE) {
        hdmi_info("the video and app attr is not changed\n");
        return TD_SUCCESS;
    }
    if (app_attr->enable_video == TD_TRUE) {
        hdmi_video_path_set(hdmi_dev, video_attr);
    }
    /*
     * HDMI is not support dither on h7.
     * set output bit to VO. VO decide to whether enable dither or not.
     */
    drv_hdmi_avi_infoframe_send(&hdmi_dev->info_frame, (app_attr->enable_hdmi && app_attr->enable_avi_infoframe));
    drv_hdmi_vendor_infoframe_send(&hdmi_dev->info_frame, app_attr->enable_hdmi);

    return TD_SUCCESS;
}

td_s32 drv_hdmi_attr_get(const hdmi_device *hdmi_dev, hdmi_attr *attr)
{
    errno_t ret;

    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);
    hdmi_if_null_return(attr, OT_ERR_HDMI_NULL_PTR);

    ret = memcpy_s(attr, sizeof(*attr), &hdmi_dev->attr, sizeof(hdmi_attr));
    if (ret != EOK) {
        hdmi_err("secure function error:%d\n", ret);
        return OT_ERR_HDMI_INVALID_PARA;
    }

    return TD_SUCCESS;
}

td_s32 drv_hdmi_attr_set(hdmi_device *hdmi_dev, hdmi_attr *attr)
{
    errno_t ret;
    hdmi_app_attr *app_attr = TD_NULL;

    hdmi_if_null_return(attr, OT_ERR_HDMI_NULL_PTR);
    hdmi_if_null_return(hdmi_dev, OT_ERR_HDMI_NULL_PTR);

    app_attr = &hdmi_dev->attr.app_attr;
    hdmi_info("enable_hdmi: %u\n", attr->app_attr.enable_hdmi);
    hdmi_info("enable_video: %u\n", attr->app_attr.enable_video);
    hdmi_info("enable_audio: %u\n", attr->app_attr.enable_audio);
    hdmi_info("out_color_space: %u\n", attr->app_attr.out_color_space);
    hdmi_info("deep_color_mode: %u\n", attr->app_attr.deep_color_mode);
    hdmi_info("bxv_ycc_mode: %u\n", attr->app_attr.xvycc_mode);
    hdmi_info("enable_avi_info_frame: %u\n", attr->app_attr.enable_avi_infoframe);
    hdmi_info("enable_spd_info_frame: %u\n", attr->app_attr.enable_spd_infoframe);
    hdmi_info("enable_mpeg_info_frame: %u\n", attr->app_attr.enable_mpeg_infoframe);
    hdmi_info("enable_aud_info_frame: %u\n", attr->app_attr.enable_aud_infoframe);
    hdmi_info("debug_flag: %u\n", attr->app_attr.debug_flag);
    hdmi_info("hdcp_mode: %u\n", attr->app_attr.hdcp_mode);
    hdmi_info("enable_clr_space_adapt: %u\n", attr->app_attr.enable_clr_space_adapt);
    hdmi_info("enable_deep_clr_adapt: %u\n", attr->app_attr.enable_deep_clr_adapt);
    hdmi_info("auth_mode: %u\n", attr->app_attr.auth_mode);
    hdmi_info("out_csc_quantization: %u\n", attr->app_attr.out_csc_quantization);

    if (check_app_attr(&attr->app_attr) != TD_SUCCESS) {
        hdmi_err("check hdmi attr fail\n");
        return OT_ERR_HDMI_INVALID_PARA;
    }
    if (hdmi_color_and_bit_strategy(hdmi_dev, &attr->app_attr, &attr->vo_attr) != TD_SUCCESS) {
        hdmi_err(" hdmi_color_and_bit_strategy fail\n");
        return OT_ERR_HDMI_STRATEGY_FAILED;
    }
    ret = memcpy_s(app_attr, sizeof(hdmi_dev->attr.app_attr), &attr->app_attr, sizeof(hdmi_app_attr));
    if (ret != EOK) {
        hdmi_err("secure function error:%d\n", ret);
        return OT_ERR_HDMI_INVALID_PARA;
    }
    if (drv_hdmi_ao_attr_set(hdmi_dev, &attr->ao_attr) != TD_SUCCESS) {
        hdmi_err("drv_hdmi_ao_attr_set fail\n");
        return OT_ERR_HDMI_SET_ATTR_FAILED;
    }
    if (drv_hdmi_vo_attr_set(hdmi_dev, &attr->vo_attr) != TD_SUCCESS) {
        hdmi_err("drv_hdmi_vo_attr_set fail\n");
        return OT_ERR_HDMI_SET_ATTR_FAILED;
    }

    return TD_SUCCESS;
}

td_void get_hdmi_default_action_set(hdmi_device *hdmi_dev, hdmi_default_action action)
{
    hdmi_if_null_return_void(hdmi_dev);
    hdmi_dev->attr.app_attr.hdmi_action = action;

    return;
}

td_s32 drv_hdmi_cmd_process(unsigned int cmd, td_void *arg, td_bool user)
{
    td_u32 index;
    td_s32 ret = TD_FAILURE;

    hdmi_if_null_return(arg, OT_ERR_HDMI_NULL_PTR);

    for (index = 0; index < hdmi_array_size(g_hdmi_cmd_func_tab); index++) {
        if ((g_hdmi_cmd_func_tab[index].cmd == cmd) && (g_hdmi_cmd_func_tab[index].hdmi_ioctrl_func != TD_NULL)) {
            ret = g_hdmi_cmd_func_tab[index].hdmi_ioctrl_func(arg, user);
            break;
        }
    }

    if (index == hdmi_array_size(g_hdmi_cmd_func_tab)) {
        hdmi_err("unknown cmd:0x%x\n", cmd);
        return OT_ERR_HDMI_UNKNOWN_COMMAND;
    }

    return ret;
}

td_s32 hdmi_set_reg(td_u32 id, td_char *reg)
{
    if (id >= HDMI_ID_MAX) {
        return TD_FAILURE;
    }
    g_hdmi_reg[id] = reg;

    return TD_SUCCESS;
}

td_s32 hdmi_set_phy(td_u32 id, td_char *phy)
{
    if (id >= HDMI_ID_MAX) {
        return TD_FAILURE;
    }
    g_hdmi_phy[id] = phy;

    return TD_SUCCESS;
}

hdmi_device *get_hdmi_device(hdmi_device_id hdmi_id)
{
    if ((td_u32)hdmi_id < HDMI_ID_MAX) {
        g_hdmi_ctrl[hdmi_id].hdmi_dev_id = hdmi_id;
        return &g_hdmi_ctrl[hdmi_id];
    }
    return TD_NULL;
}

td_s32 hdmi_drv_mod_init(td_void)
{
    td_s32 i;
    td_s32 ret;
    hdmi_device *hdmi_dev = TD_NULL;

    /* init csc param. */
    for (i = HDMI_DEVICE_ID0; i < HDMI_ID_MAX; i++) {
        hdmi_dev = get_hdmi_device(i);
        if (hdmi_dev != TD_NULL) {
            hdmi_dev->csc_param.colorimetry = HDMI_COLORIMETRY_ITU709;
            hdmi_dev->csc_param.pixel_encoding = HDMI_EXT_COLORSPACE_YCBCR444;
            hdmi_dev->csc_param.quantization = HDMI_QUANT_RANGE_LIMITED;
            osal_sema_init(&hdmi_dev->mutex_proc, 1);
            osal_sema_init(&hdmi_dev->mutex_thread, 1);
        }
    }

    osal_sema_init(&g_hdmi_mutex, 1);
    osal_atomic_init(&g_hdmi_count);
    osal_atomic_set(&g_hdmi_count, 0);

    ret = hdmi_dev_register();
    if (ret == TD_FAILURE) {
        goto exit_dev;
    }
#ifdef CONFIG_OT_PROC_SHOW_SUPPORT
    hdmi_proc_register();
#endif
    OT_PRINT("load hdmi.ko OK!\n");

    return TD_SUCCESS;

exit_dev:
    osal_destroydev(g_hdmi_device);
    cmpi_unregister_module(OT_ID_HDMI);

    return TD_FAILURE;
}

td_void hdmi_drv_mod_exit(td_void)
{
    td_u32 hdmi_id;
    hdmi_device *hdmi_dev = TD_NULL;

    for (hdmi_id = 0; hdmi_id < HDMI_ID_MAX; hdmi_id++) {
        hdmi_dev = get_hdmi_device(hdmi_id);
        if (hdmi_dev != TD_NULL) {
            if ((td_u32)hdmi_dev->run_state & HDMI_RUN_STATE_OPEN) {
                hdmi_dev->user_cnt = 0;
                hdmi_dev->kernel_cnt = 0;
                hdmi_release(hdmi_dev);
            }
            osal_sema_destroy(&hdmi_dev->mutex_proc);
            osal_sema_destroy(&hdmi_dev->mutex_thread);
        }
#ifdef CONFIG_OT_PROC_SHOW_SUPPORT
        drv_hdmi_proc_un_register(hdmi_id);
#endif
        drv_hdmi_hardware_reset(hdmi_id);
    }

    cmpi_unregister_module(OT_ID_HDMI);
    osal_deregisterdevice(g_hdmi_device);
    osal_destroydev(g_hdmi_device);
    osal_atomic_destroy(&g_hdmi_count);
    osal_sema_destroy(&g_hdmi_mutex);

    OT_PRINT("unload hdmi.ko OK!\n");

    return;
}

#endif /* DRV_HDMI_INTF_BODY_INC */
