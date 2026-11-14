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

#ifndef DRV_HDMI_COMMON_H
#define DRV_HDMI_COMMON_H

#include "ot_type.h"
#include "drv_hdmi_infoframe.h"
#include "hdmi_ext.h"
#include "securec.h"
#include "ot_common_hdmi.h"

#define HDMI_VER_MAJOR    2
#define HDMI_VER_MINOR    0
#define HDMI_VER_REVISE   0
#define HDMI_VER_DATE     20240402
#define HDMI_VER_TIMES    0

#define make_ver_bit(x)     #x
#define make_macro2str(exp) make_ver_bit(exp)
#define MAKE_VERSION                    \
    make_macro2str(HDMI_VER_MAJOR) "."  \
    make_macro2str(HDMI_VER_MINOR) "."  \
    make_macro2str(HDMI_VER_REVISE) "." \
    make_macro2str(HDMI_VER_DATE) "."   \
    make_macro2str(HDMI_VER_TIMES)

#define AEN_TX_FFE_LEN              4
#define MAX_FRL_RATE                6
#define HDMI_FRL_LANE_MAX_NUM       4
#define CEA_VIDEO_CODE_MAX          44
#define VESA_VIDEO_CODE_MAX         31
#define CEA861_F_VIDEO_CODES_MAX_4K 4
#define HDMI_INFO_FRAME_MAX_SIZE    31
#define SCDC_TMDS_BIT_CLK_RATIO_10X 10
#define SCDC_TMDS_BIT_CLK_RATIO_40X 40
#define HDMI_DECIMAL                10
#define HDMI_HUNDRED                100
#define HDMI_THOUSAND               1000
#define FMT_PIX_CLK_13400           13400
#define FMT_PIX_CLK_74250           74250
#define FMT_PIX_CLK_165000          165000
#define FMT_PIX_CLK_190000          190000
#define FMT_PIX_CLK_297000          297000
#define FMT_PIX_CLK_340000          340000
#define ZERO_DRMIF_SEND_TIME        2000 /* unit: ms */
#define HDRMODE_CHANGE_TIME         500  /* unit: ms */
#define HDMI_EDID_BLOCK_SIZE        128
#define HDMI_EDID_TOTAL_BLOCKS      4
#define HDMI_EDID_SIZE             (HDMI_EDID_BLOCK_SIZE * HDMI_EDID_TOTAL_BLOCKS)
#define HDMI_REGISTER_SIZE          4
#define HDMI_EDID_BLOCK_SIZE        128
#define HDMI_EDID_MAX_BLOCK_NUM     4
#define HDMI_HW_PARAM_LEN           4
#define HDMI_EDID_TOTAL_SIZE       ((HDMI_EDID_BLOCK_SIZE) * (HDMI_EDID_MAX_BLOCK_NUM))
#define hdmi_array_size(a)         ((sizeof(a)) / (sizeof((a)[0])))
#ifdef HDMI_FPGA_SUPPORT
#define FPGA_SUPPORT                TD_TRUE
#else
#define FPGA_SUPPORT                TD_FALSE
#endif
#define DEBUG_MAX_ARGV_NUM          10
#define hdmi_unused(x)              (x) = (x)

#define PRT_RED                     "\033[31;1m"
#define PRT_GREEN                   "\033[32;1m"
#define PRT_CLEAN                   "\033[0m"

#define FRL_CTRL_TYPE_COMPRESS_ALL   0x00
#define FRL_CTRL_TYPE_COMPRESS_HW    0x01
#define FRL_CTRL_TYPE_COMPRESS_NON   0x03
#define HDMI_FRL_COMPRESS_DEBUG_MASK 0x4

/* AVI InfoFrame Packet byte offset define */
#define AVI_OFFSET_TYPE 0
#define AVI_OFFSET_VERSION 1
#define AVI_OFFSET_LENGTH 2
#define AVI_OFFSET_CHECKSUM 3
/*
 * include :
 * color space
 * active information present
 * bar Info data valid
 * scan Information
 */
#define AVI_OFFSET_PB1 4
/*
 * include :
 * colorimetry
 * picture aspect ratio
 * active format aspect ratio
 */
#define AVI_OFFSET_PB2 5
/*
 * include :
 * IT content
 * extended colorimetry
 * quantization range
 * non-uniform picture scaling
 */
#define AVI_OFFSET_PB3 6
#define AVI_OFFSET_VIC 7
/*
 * include :
 * YCC quantization range
 * content type
 * pixel repetition factor
 */
#define AVI_OFFSET_PB5 8
#define AVI_OFFSET_TOP_BAR_LOWER 9
#define AVI_OFFSET_TOP_BAR_UPPER 10
#define AVI_OFFSET_BOTTOM_BAR_LOWER 11
#define AVI_OFFSET_BOTTOM_BAR_UPPER 12
#define AVI_OFFSET_LEFT_BAR_LOWER 13
#define AVI_OFFSET_LEFT_BAR_UPPER 14
#define AVI_OFFSET_RIGHT_BAR_LOWER 15
#define AVI_OFFSET_RIGHT_BAR_UPPER 16
#define AVI_OFFSET_PB14 17
#define AVI_OFFSET_PB15 18
#define AVI_OFFSET_PB16 19
#define AVI_OFFSET_PB17 20
#define AVI_OFFSET_PB18 21
#define AVI_OFFSET_PB19 22
#define AVI_OFFSET_PB20 23
#define AVI_OFFSET_PB21 24
#define AVI_OFFSET_PB22 25
#define AVI_OFFSET_PB23 26
#define AVI_OFFSET_PB24 27
#define AVI_OFFSET_PB25 28
#define AVI_OFFSET_PB26 29
#define AVI_OFFSET_PB27 30
#define AVI_FRAME_COLORIMETRY_MASK 0x3
#define AVI_FRAME_PIC_ASPECT_MASK 0x3
#define AVI_FRAME_ACTIVE_ASPECT_MASK 0xF
#define AVI_FRAME_EXT_COLORIMETRY_MASK 0x7
#define AVI_FRAME_QUANT_RANGE_MASK 0x3
#define AVI_FRAME_YCC_QUANT_RANGE_MASK 0x3
#define AVI_FRAME_PIXEL_REPET_MASK 0xF

/* audio infoFrame packet byte offset define */
#define AUDIO_OFFSET_TYPE 0
#define AUDIO_OFFSET_VERSION 1
#define AUDIO_OFFSET_LENGHT 2
#define AUDIO_OFFSET_CHECKSUM 3
/*
 * include :
 * channel count
 * coding type
 */
#define AUDIO_OFFSET_PB1 4
/*
 * include :
 * sample size
 * sample frequency
 */
#define AUDIO_OFFSET_PB2 5
#define AUDIO_OFFSET_FORMAT 6
#define AUDIO_OFFSET_CA 7
/*
 * include :
 * level shift value
 * downmix inhibit
 * LFE playback level information
 */
#define AUDIO_OFFSET_PB5 8
#define AUDIO_OFFSET_PB6 9
#define AUDIO_OFFSET_PB7 10
#define AUDIO_OFFSET_PB8 11
#define AUDIO_OFFSET_PB9 12
#define AUDIO_OFFSET_PB10 13
#define AUDIO_OFFSET_PB11 14
#define AUDIO_OFFSET_PB12 15
#define AUDIO_OFFSET_PB13 16
#define AUDIO_OFFSET_PB14 17
#define AUDIO_OFFSET_PB15 18
#define AUDIO_OFFSET_PB16 19
#define AUDIO_OFFSET_PB17 20
#define AUDIO_OFFSET_PB18 21
#define AUDIO_OFFSET_PB19 22
#define AUDIO_OFFSET_PB20 23
#define AUDIO_OFFSET_PB21 24
#define AUDIO_OFFSET_PB22 25
#define AUDIO_OFFSET_PB23 26
#define AUDIO_OFFSET_PB24 27
#define AUDIO_OFFSET_PB25 28
#define AUDIO_OFFSET_PB26 29
#define AUDIO_OFFSET_PB27 30
#define AUDIO_FRAME_CODE_TYPE_MASK 0xF

/* gdb infoFrame packet byte offset define */
#define GDB_OFFSET_HB0 0
#define GDB_OFFSET_HB1 1
#define GDB_OFFSET_HB2 2
#define GDB_OFFSET_CHECKSUM 3
#define GDB_OFFSET_PB1 4
#define GDB_OFFSET_PB2 5
#define GDB_OFFSET_PB3 6
#define GDB_OFFSET_PB4 7
#define GDB_OFFSET_PB5 8
#define GDB_OFFSET_PB6 9
#define GDB_OFFSET_PB7 10
#define GDB_OFFSET_PB8 11
#define GDB_OFFSET_PB9 12
#define GDB_OFFSET_PB10 13
#define GDB_OFFSET_PB11 14
#define GDB_OFFSET_PB12 15
#define GDB_OFFSET_PB13 16
#define GDB_OFFSET_PB14 17
#define GDB_OFFSET_PB15 18
#define GDB_OFFSET_PB16 19
#define GDB_OFFSET_PB17 20
#define GDB_OFFSET_PB18 21
#define GDB_OFFSET_PB19 22
#define GDB_OFFSET_PB20 23
#define GDB_OFFSET_PB21 24
#define GDB_OFFSET_PB22 25
#define GDB_OFFSET_PB23 26
#define GDB_OFFSET_PB24 27
#define GDB_OFFSET_PB25 28
#define GDB_OFFSET_PB26 29
#define GDB_OFFSET_PB27 30

/* vendor infoFrame packet byte offset define */
#define VENDOR_OFFSET_TYPE 0
#define VENDOR_OFFSET_VERSION 1
#define VENDOR_OFFSET_LENGHT 2
#define VENDOR_OFFSET_CHECSUM 3
#define VENDOR_OFFSET_IEEE_LOWER 4
#define VENDOR_OFFSET_IEEE_UPPER 5
#define VENDOR_OFFSET_IEEE 6
#define VENDOR_OFFSET_VIDEO_FMT 7
#define VENDOR_OFFSET_VIC 8
#define VENDOR_OFFSET_3D_STRUCT 9
#define VENDOR_OFFSET_3D_EXT_DATA 10
#define VENDOR_OFFSET_PB7 11
#define VENDOR_OFFSET_PB8 12
#define VENDOR_OFFSET_PB9 13
#define VENDOR_OFFSET_PB10 14
#define VENDOR_OFFSET_PB11 15
#define VENDOR_OFFSET_PB12 16
#define VENDOR_OFFSET_PB13 17
#define VENDOR_OFFSET_PB14 18
#define VENDOR_OFFSET_PB15 19
#define VENDOR_OFFSET_PB16 20
#define VENDOR_OFFSET_PB17 21
#define VENDOR_OFFSET_PB18 22
#define VENDOR_OFFSET_PB19 23
#define VENDOR_OFFSET_PB20 24
#define VENDOR_OFFSET_PB21 25
#define VENDOR_OFFSET_PB22 26
#define VENDOR_OFFSET_PB23 27
#define VENDOR_OFFSET_PB24 28
#define VENDOR_OFFSET_PB25 29
#define VENDOR_OFFSET_PB26 30
#define VENDOR_FARAME_VIDEO_FMT_MASK 0x7
#define VENDOR_3D_STRUCT_MASK 0xF

#define HDMI_BKSV_LEN 5
#define HDMI_AKSV_LEN 5

#define hdmi_unlock_if_null_return(p, mutex, ret)  \
    do {                                           \
        if ((p) == TD_NULL) {                      \
            hdmi_err("%s is null pointer!\n", #p); \
            hdmi_mutex_unlock((mutex));            \
            return (ret);                          \
        }                                          \
    } while (0)

#define hdmi_if_null_return(p, ret)                \
    do {                                           \
        if ((p) == TD_NULL) {                      \
            hdmi_err("%s is null pointer!\n", #p); \
            return (ret);                          \
        }                                          \
    } while (0)

#define hdmi_if_null_return_void(p)                \
    do {                                           \
        if ((p) == TD_NULL) {                      \
            hdmi_err("%s is null pointer!\n", #p); \
            return;                                \
        }                                          \
    } while (0)

#define hdmi_if_null_warn_return(p, ret)            \
    do {                                            \
        if ((p) == TD_NULL) {                       \
            hdmi_warn("%s is null pointer!\n", #p); \
            return (ret);                           \
        }                                           \
    } while (0)

#define hdmi_if_null_warn_return_void(p)            \
    do {                                            \
        if ((p) == TD_NULL) {                       \
            hdmi_warn("%s is null pointer!\n", #p); \
            return;                                 \
        }                                           \
    } while (0)

#define hdmi_if_false_return_void(b)        \
    do {                                    \
        if ((b) != TD_TRUE) {               \
            hdmi_err("%s is FALSE!\n", #b); \
            return;                         \
        }                                   \
    } while (0)

#define hdmi_if_false_return(tmp, ret)        \
    do {                                      \
        if ((tmp) != TD_TRUE) {               \
            hdmi_err("%s is FALSE!\n", #tmp); \
            return (ret);                     \
        }                                     \
    } while (0)

#define hdmi_if_false_warn_return(tmp, ret)    \
    do {                                       \
        if ((tmp) != TD_TRUE) {                \
            hdmi_warn("%s is FALSE!\n", #tmp); \
            return (ret);                      \
        }                                      \
    } while (0)

#define hdmi_if_false_warn_return_void(tmp)    \
    do {                                       \
        if ((tmp) != TD_TRUE) {                \
            hdmi_warn("%s is FALSE!\n", #tmp); \
            return;                            \
        }                                      \
    } while (0)

#define hdmi_check_is_change_return(tmp0, tmp1, ret)                                            \
    do {                                                                                        \
        if ((tmp0) != (tmp1)) {                                                                 \
            hdmi_info("%s change, old(%u)->new(%u) \n", #tmp0, (td_u32)(tmp0), (td_u32)(tmp1)); \
            return (ret);                                                                       \
        }                                                                                       \
    } while (0)

#define hdmi_check_max_return(value, max, ret)                    \
    do {                                                          \
        if ((value) > (max)) {                                    \
            hdmi_warn("value %u exceed max!\n", (td_u32)(value)); \
            return (ret);                                         \
        }                                                         \
    } while (0)

#define hdmi_if_failure_return(tmp, ret)        \
    do {                                        \
        if ((tmp) != TD_SUCCESS) {              \
            hdmi_err("%s is failure!\n", #tmp); \
            return (ret);                       \
        }                                       \
    } while (0)

#define hdmi_if_failure_return_void(tmp)        \
    do {                                        \
        if ((tmp) != TD_SUCCESS) {              \
            hdmi_err("%s is failure!\n", #tmp); \
            return;                             \
        }                                       \
    } while (0)
#define hdmi_if_failure_warn_return_void(tmp)    \
    do {                                         \
        if ((tmp) != TD_SUCCESS) {               \
            hdmi_warn("%s is failure!\n", #tmp); \
            return;                              \
        }                                        \
    } while (0)

#define hdmi_set_bit(var, bit) \
    do {                       \
        (var) |= 1 << (bit);   \
    } while (0)

#define hdmi_clr_bit(var, bit)  \
    do {                        \
        (var) &= ~(1 << (bit)); \
    } while (0)

#ifdef HDMI_FPGA_SUPPORT
#define hdmi_if_fpga_return(ret)      \
    do {                              \
        if (FPGA_SUPPORT) {           \
            hdmi_warn("FPGA CFG!\n"); \
            return(ret);              \
        }                             \
    } while (0)

#define hdmi_if_fpga_return_void()    \
    do {                              \
        if (FPGA_SUPPORT) {           \
            hdmi_warn("FPGA CFG!\n"); \
            return;                   \
        }                             \
    } while (0)
#else
#define hdmi_if_fpga_return(ret)
#define hdmi_if_fpga_return_void()
#endif

#define is_bit_set(var, bit) ({ (var) & (0x1 << (bit)) ? TD_TRUE : TD_FALSE; })

#define hal_call_ret(dev, ret, func, param...)             \
    do {                                                   \
        if ((dev) != TD_NULL && (dev)->hal != TD_NULL &&   \
            (dev)->hal->func != TD_NULL) {                 \
            (ret) = (dev)->hal->func((dev)->hal, ##param); \
        } else {                                           \
            (ret) = OT_ERR_HDMI_NULL_PTR;                  \
        }                                                  \
    } while (0)

#define hal_call_void(dev, func, param...)                 \
    do {                                                   \
        if ((dev) != TD_NULL && (dev)->hal != TD_NULL &&   \
            (dev)->hal->func != TD_NULL) {                 \
            (dev)->hal->func((dev)->hal, ##param);         \
        } else {                                           \
            hdmi_warn("null pointer! \n");                 \
        }                                                  \
    } while (0)

#define hdmi_if_zero_return_void(x)        \
    do {                                   \
        if ((x) == 0) {                    \
            hdmi_err("%s is zero!\n", #x); \
            return;                        \
        }                                  \
    } while (0)
#define hdmi_unequal_eok_return(ret, err_code)             \
    do {                                                   \
        if ((ret) != EOK) {                                \
            hdmi_err("secure function error:%d\n", (ret)); \
            return (err_code);                             \
        }                                                  \
    } while (0)

#define hdmi_unlock_unequal_eok_return(ret, mutex, err_code) \
    do {                                                     \
        if ((ret) != EOK) {                                  \
            hdmi_err("secure function error:%d\n", (ret));   \
            hdmi_mutex_unlock((mutex));                      \
            return (err_code);                               \
        }                                                    \
    } while (0)

#define hdmi_unequal_eok_return_void(ret)                  \
    do {                                                   \
        if ((ret) != EOK) {                                \
            hdmi_err("secure function error:%d\n", (ret)); \
            return;                                        \
        }                                                  \
    } while (0)

#define hdmi_check_open_return(state)           \
    do {                                        \
        if (!((state) & HDMI_RUN_STATE_OPEN)) { \
            hdmi_warn("device not open\n");     \
            return OT_ERR_HDMI_DEV_NOT_OPEN;    \
        }                                       \
    } while (0)

typedef enum {
    HDMI_DEVICE_ID0,
    HDMI_DEVICE_ID1,
    HDMI_DEVICE_ID_BUTT
} hdmi_device_id;

#ifdef HDMI_SUPPORT_DUAL_CHANNEL
#define HDMI_ID_MAX HDMI_DEVICE_ID_BUTT
#else
#define HDMI_ID_MAX HDMI_DEVICE_ID1
#endif

typedef enum {
    HDMI_THREAD_STATE_IDLE,
    HDMI_THREAD_STATE_RUN,
    HDMI_THREAD_STATE_STOP
} hdmi_thread_state;

typedef enum {
    HDMI_EVENT_HOTPLUG = 0x10,
    HDMI_EVENT_HOTUNPLUG,
    HDMI_EVENT_EDID_FAIL,
    HDMI_EVENT_RSEN_CONNECT,
    HDMI_EVENT_RSEN_DISCONNECT,
    HDMI_EVENT_SCRAMBLE_FAIL,
    HDMI_EVENT_SCRAMBLE_SUCCESS,
    HDMI_EVENT_ZERO_DRMIF_TIMEOUT,
    HDMI_EVENT_SWITCH_TO_HDRMODE_TIMEOUT,
    HDMI_EVENT_BUTT
} hdmi_event;

typedef enum {
    HDMI_DEBUG_BASE_OSD = 8,
    HDMI_DEBUG_BASE_DEC = 10,
    HDMI_DEBUG_BASE_HEX = 16
} hdmi_debug_base;

typedef td_s32 (*hdmi_callback)(td_void *, hdmi_event);

typedef struct {
    hdmi_device_id hdmi_id;
    td_char *argv[DEBUG_MAX_ARGV_NUM];
    td_u32 argc;
    td_u32 remain_len;
} hdmi_debug_cmd_arg;

typedef td_s32 (*cmd_func)(const hdmi_debug_cmd_arg *cmd_arg);

typedef struct {
    td_char *name;
    td_char *short_name;
    cmd_func fn_cmd_func;
    td_char *comment_help;
} hdmi_debug_cmd_info;

typedef struct {
    td_u32 ddc_reg_cfg;
    td_u32 approximate_value;
    td_char *read_value;
} hdmi_ddc_freq;

typedef struct {
    td_bool data_valid;
    td_s32 len;
    td_u8 data[HDMI_EDID_TOTAL_SIZE];
} hdmi_debug_edid;

typedef struct {
    td_void *event_data;
    hdmi_callback event_callback;
    td_u32 hdmi_dev_id;
    td_char *base_addr;
    td_char *phy_addr;
} hdmi_hal_init;

typedef struct {
    td_u32 cmd;
    td_s32 (*hdmi_ioctrl_func)(td_void *arg, td_bool user);
} hdmi_ioctrl_func;

typedef struct {
    hdmi_colorimetry colorimetry;
    hdmi_quant_range quantization;
    hdmi_pixel_encoding pixel_encoding;
} hdmi_csc_attr;

typedef struct {
    td_u8  edid_valid;
    td_u32 edid_len;
    td_u8  edid[HDMI_EDID_SIZE];
} hdmi_edid_raw_data;

typedef enum {
    HDMI_HDCP_VERSION_NONE,
    HDMI_HDCP_VERSION_1_4,
    HDMI_HDCP_VERSION_2_2,
    HDMI_HDCP_VERSION_BUTT
} hdmi_hdcp_version;

typedef struct {
    td_bool connected;
    td_bool sink_power_on;
    td_bool authed;
    td_u8 bksv[HDMI_BKSV_LEN];
    hdmi_hdcp_version hdcp_version;
} hdmi_status;

typedef struct {
    td_u32 i_de_main_clk;
    td_u32 i_de_main_data;
    td_u32 i_main_clk;
    td_u32 i_main_data;
    td_u32 ft_cap_clk;
    td_u32 ft_cap_data;
} hdmi_hw_param;

typedef struct {
    hdmi_hw_param hw_param[HDMI_HW_PARAM_LEN];
} hdmi_hw_spec;

typedef struct {
    hdmi_hw_spec hwspec_user;
    hdmi_hw_spec hwspec_def;
    hdmi_hw_param hwparam_cur;
} hdmi_hwspec;

typedef enum {
    HDMI_DEEP_COLOR_24BIT,
    HDMI_DEEP_COLOR_30BIT,
    HDMI_DEEP_COLOR_36BIT,
    HDMI_DEEP_COLOR_48BIT,
    HDMI_DEEP_COLOR_OFF = 0xff,
    HDMI_DEEP_COLOR_BUTT
} hdmi_deep_color;

typedef enum {
    HDMI_VIDEO_BITDEPTH_8,
    HDMI_VIDEO_BITDEPTH_10,
    HDMI_VIDEO_BITDEPTH_12,
    HDMI_VIDEO_BITDEPTH_16,
    HDMI_VIDEO_BITDEPTH_OFF,
    HDMI_VIDEO_BITDEPTH_BUTT
} hdmi_video_bit_depth;

typedef enum {
    HDMI_HV_SYNC_POL_HPVP,
    HDMI_HV_SYNC_POL_HPVN,
    HDMI_HV_SYNC_POL_HNVP,
    HDMI_HV_SYNC_POL_HNVN,
    HDMI_HV_SYNC_POL_BUTT
} hdmi_hvsync_polarity;

typedef enum {
    HDMI_PICTURE_NON_UNIFORM_SCALING,
    HDMI_PICTURE_SCALING_H,
    HDMI_PICTURE_SCALING_V,
    HDMI_PICTURE_SCALING_HV
} hdmi_picture_scaling;

typedef struct {
    td_u32 clk_fs; /* VDP setting(in) */
    td_u32 tmds_clk;
    td_u32 hdmi_adapt_pix_clk; /* HDMI adapt setting(out) */
    td_u32 pixel_repeat;
    td_bool v_sync_pol;
    td_bool h_sync_pol;
    td_bool de_pol;
    hdmi_video_timing video_timing;
    hdmi_3d_mode stereo_mode;
    hdmi_colorspace in_color_space;
    hdmi_colormetry colorimetry;
    hdmi_extended_colormetry extended_colorimetry;
    hdmi_quantization_range rgb_quantization;
    hdmi_ycc_quantization_range ycc_quantization;
    hdmi_picture_aspect picture_aspect;
    hdmi_active_aspect active_aspect;
    hdmi_picture_scaling picture_scaling;
    hdmi_video_bit_depth in_bit_depth;
    hdmi_disp_format disp_fmt;
} hdmi_vo_attr;

typedef enum {
    HDMI_AUDIO_FORMAT_2CH = 0x2,
    HDMI_AUDIO_FORMAT_3CH,
    HDMI_AUDIO_FORMAT_4CH,
    HDMI_AUDIO_FORMAT_5CH,
    HDMI_AUDIO_FORMAT_6CH,
    HDMI_AUDIO_FORMAT_7CH,
    HDMI_AUDIO_FORMAT_8CH,
    HDMI_AUDIO_FORMAT_BUTT
} hdmi_audio_ch;

typedef enum {
    HDMI_AUDIO_INTF_I2S,
    HDMI_AUDIO_INTF_SPDIF,
    HDMI_AUDIO_INTF_HBRA,
    HDMI_AUDIO_INTF_BUTT
} hdmi_audio_interface;

typedef enum {
    HDMI_AUDIO_BIT_DEPTH_UNKNOWN,
    HDMI_AUDIO_BIT_DEPTH_8  = 8,
    HDMI_AUDIO_BIT_DEPTH_16 = 16,
    HDMI_AUDIO_BIT_DEPTH_18 = 18,
    HDMI_AUDIO_BIT_DEPTH_20 = 20,
    HDMI_AUDIO_BIT_DEPTH_24 = 24,
    HDMI_AUDIO_BIT_DEPTH_32 = 32,
    HDMI_AUDIO_BIT_DEPTH_BUTT
} hdmi_audio_bit_depth;

typedef enum {
    HDMI_SAMPLE_RATE_UNKNOWN,
    HDMI_SAMPLE_RATE_8K   = 8000,
    HDMI_SAMPLE_RATE_11K  = 11025,
    HDMI_SAMPLE_RATE_12K  = 12000,
    HDMI_SAMPLE_RATE_16K  = 16000,
    HDMI_SAMPLE_RATE_22K  = 22050,
    HDMI_SAMPLE_RATE_24K  = 24000,
    HDMI_SAMPLE_RATE_32K  = 32000,
    HDMI_SAMPLE_RATE_44K  = 44100,
    HDMI_SAMPLE_RATE_48K  = 48000,
    HDMI_SAMPLE_RATE_88K  = 88200,
    HDMI_SAMPLE_RATE_96K  = 96000,
    HDMI_SAMPLE_RATE_176K = 176400,
    HDMI_SAMPLE_RATE_192K = 192000,
    HDMI_SAMPLE_RATE_768K = 768000,
    HDMI_SAMPLE_RATE_BUTT
} hdmi_sample_rate;

typedef struct {
    td_bool down_sample;
    hdmi_sample_rate sample_fs;
    hdmi_audio_ch channels;
    hdmi_audio_interface sound_intf;
    hdmi_audio_bit_depth sample_depth;
    hdmi_audio_format_code audio_code;
} hdmi_ao_attr;

typedef enum {
    HDMI_TMDS_MODE_NONE,
    HDMI_TMDS_MODE_DVI,
    HDMI_TMDS_MODE_HDMI_1_4,
    HDMI_TMDS_MODE_HDMI_2_0,
    HDMI_TMDS_MODE_AUTO,
    HDMI_TMDS_MODE_HDMI_2_1,
    HDMI_TMDS_MODE_BUTT
} hdmi_tmds_mode;

typedef enum {
    HDMI_HDCP_MODE_AUTO,
    HDMI_HDCP_MODE_1_4,
    HDMI_HDCP_MODE_2_2,
    HDMI_HDCP_MODE_BUTT
} hdmi_hdcp_mode;

typedef enum {
    HDMI_DEFAULT_ACTION_NULL,
    HDMI_DEFAULT_ACTION_HDMI,
    HDMI_DEFAULT_ACTION_DVI,
    HDMI_DEFAULT_ACTION_BUTT
} hdmi_default_action;

typedef enum {
    HDMI_VIDEO_DITHER_12_10,
    HDMI_VIDEO_DITHER_12_8,
    HDMI_VIDEO_DITHER_10_8,
    HDMI_VIDEO_DITHER_DISALBE
} hdmi_video_dither;

typedef struct {
    td_bool enable_hdmi;
    td_bool enable_video;
    td_bool enable_audio;
    hdmi_colorspace out_color_space;
    hdmi_quantization_range out_csc_quantization;
    hdmi_deep_color deep_color_mode;
    td_bool xvycc_mode;
    td_bool enable_avi_infoframe;
    td_bool enable_spd_infoframe;
    td_bool enable_mpeg_infoframe;
    td_bool enable_aud_infoframe;
    td_u32  debug_flag;
    td_bool hdcp_enable;
    hdmi_default_action hdmi_action;
    td_bool enable_clr_space_adapt;
    td_bool enable_deep_clr_adapt;
    td_bool auth_mode;
    hdmi_hdcp_mode hdcp_mode;
} hdmi_app_attr;

typedef struct {
    td_bool enable_hdmi;
    td_bool enable_video;
    hdmi_disp_format disp_fmt;
    hdmi_video_timing video_timing;
    td_u32 pix_clk;
    hdmi_colorspace in_color_space;
    hdmi_colorspace out_color_space;
    hdmi_deep_color deep_color_mode;
    hdmi_quantization_range out_csc_quantization;
    td_bool enable_audio;
    hdmi_sample_rate sample_rate;
    hdmi_audio_bit_depth bit_depth;
    td_bool enable_avi_infoframe;
    td_bool enable_aud_infoframe;
    hdmi_default_action hdmi_action;
    td_bool enable_vid_mode_adapt;
    td_bool enable_deep_clr_adapt;
    td_bool auth_mode;
} hdmi_property;

typedef struct {
    hdmi_ao_attr ao_attr;
    hdmi_vo_attr vo_attr;
    hdmi_app_attr app_attr;
} hdmi_attr;

typedef enum {
    HDMI_TRANSITION_NONE,
    HDMI_TRANSITION_BOOT_MCE,
    HDMI_TRANSITION_MCE_APP,
    HDMI_TRANSITION_BOOT_APP = 0x4
} hdmi_transition_state;

typedef enum {
    HDMI_RUN_STATE_NONE,
    HDMI_RUN_STATE_INIT,
    HDMI_RUN_STATE_OPEN,
    HDMI_RUN_STATE_START = 0x4,
    HDMI_RUN_STATE_STOP = 0x8,
    HDMI_RUN_STATE_CLOSE = 0x10,
    HDMI_RUN_STATE_DEINIT = 0x20
} hdmi_run_state;

typedef struct {
    td_u16 length;
    td_u8 *list;
    td_u8 *list_start;
} hdmi_hdcp_ksv_list;

typedef struct {
    td_bool tx_hdmi_14;
    td_bool tx_hdmi_20;
    td_bool tx_hdmi_21;
    td_bool tx_hdcp_14;
    td_bool tx_hdcp_22;
    td_bool tx_rgb444;
    td_bool tx_ycbcr444;
    td_bool tx_ycbcr422;
    td_bool tx_ycbcr420;
    td_bool tx_deep_clr10_bit;
    td_bool tx_deep_clr12_bit;
    td_bool tx_deep_clr16_bit;
    td_bool tx_rgb_ycbcr444;
    td_bool tx_ycbcr444_422;
    td_bool tx_ycbcr422_420;
    td_bool tx_ycbcr420_422;
    td_bool tx_ycbcr422_444;
    td_bool tx_ycbcr444_rgb;
    td_bool tx_scdc;
    td_u32  tx_max_tmds_clk;
    td_u32  tx_max_frl_rate;
} hdmi_tx_capability_data;

typedef enum {
    HDMI_CONV_STD_BT_709,
    HDMI_CONV_STD_BT_601,
    HDMI_CONV_STD_BT_2020_NON_CONST_LUMINOUS,
    HDMI_CONV_STD_BT_2020_CONST_LUMINOUS,
    HDMI_CONV_STD_BUTT
} hdmi_conversion_stb;

typedef struct {
    hdmi_video_timing timing;
    td_u32 pixel_clk;
    td_u32 tmds_clk;
    td_bool v_sync_pol;
    td_bool h_sync_pol;
    td_bool de_pol;
    hdmi_conversion_stb conv_std;
    hdmi_quantization_range quantization;
    hdmi_colorspace in_color_space;
    hdmi_colorspace out_color_space;
    hdmi_deep_color deep_color;
    hdmi_video_bit_depth in_bit_depth;
    hdmi_quantization_range out_csc_quantization;
    td_bool emi_enable;
} hdmi_video_config;

typedef struct {
    td_bool enable_audio;
    td_bool down_sample;
    td_u32 tmds_clk;
    td_u32 pixel_repeat;
    hdmi_sample_rate sample_fs;
    hdmi_audio_ch layout;
    hdmi_audio_interface sound_intf;
    hdmi_audio_bit_depth sample_depth;
} hdmi_audio_config;

typedef enum {
    HDMI_FRL_MODE_TMDS,
    HDMI_FRL_MODE_FRL,
    HDMI_FRL_MODE_BUTT
} hdmi_frl_mode;

typedef struct {
    hdmi_sample_rate sample_rate;
    hdmi_frl_mode hdmi_mode;
    td_u8 frl_rate;
    td_u32 pixel_clk;
} hdmi_audio_ncts;

typedef struct {
    td_bool phy_oe;
    td_bool phy_power_on;
    hdmi_video_bit_depth deep_color;
} hdmi_phy_status;

typedef struct {
    td_bool sw_emi_enable;
    td_bool hw_emi_enable;
    td_bool debug_enable;
} hdmi_emi_status;

typedef struct {
    td_bool video_mute;
    td_bool ycbcr2rgb;
    td_bool rgb2ycbcr;
    td_bool ycbcr444_422;
    td_bool ycbcr422_420;
    td_bool ycbcr420_422;
    td_bool ycbcr422_444;
    td_bool in420_ydemux;
    td_bool out420_ydemux;
    hdmi_video_dither dither;
    td_bool v_sync_pol;
    td_bool h_sync_pol;
    td_bool sync_pol;
    td_bool de_pol;
    td_bool swap_hs_cs;
    hdmi_colorspace in_color_space;
    hdmi_colorspace out_color_space;
    hdmi_video_bit_depth out_bit_depth;
    hdmi_hvsync_polarity hv_sync_pol;
    hdmi_quantization_range out_csc_quantization;
    /* detect timing */
    td_bool sync_sw_enable;
    td_bool vsync_polarity; /* when sync_sw_enable==0,indicates hw;or ,indicates sw */
    td_bool hsync_polarity; /* when sync_sw_enable==0,indicates hw;or ,indicates sw */
    td_bool progressive;
    td_u32  hsync_total;
    td_u32  hactive_cnt;
    td_u32  vsync_total;
    td_u32  vactive_cnt;
} hdmi_video_status;

typedef struct {
    td_bool hdcp14_support;
    td_bool hdcp22_support;
} hdmi_hdcp_capability;

typedef struct {
    td_bool audio_mute;
    td_bool audio_enable;
    td_bool down_sample;
    hdmi_sample_rate sample_fs;
    hdmi_audio_ch layout;
    hdmi_audio_interface sound_intf;
    hdmi_audio_bit_depth sample_depth;
    td_u32 ref_n;
    td_u32 reg_n;
    td_u32 ref_cts;
    td_u32 reg_cts;
} hdmi_audio_status;

typedef struct {
    td_bool hotplug;
    td_bool rsen;
    td_bool avmute;
    hdmi_tmds_mode tmds_mode;
} hdmi_common_status;

typedef struct {
    td_bool source_scramble_on;
    td_bool sink_scramble_on;
    td_u8   tmds_bit_clk_ratio;
    td_bool sink_read_quest;
    /* in unit of ms.for [0,200], force to default 200; or, set the value cfg(>200). */
    td_u32 scramble_timeout;
    /* in unit of ms, range[20,200). for [0,20] or >=200, force to default 20; or, set the value cfg[20,200). */
    td_u32 scramble_interval;
} hdmi_scdc_status;

typedef struct {
    td_bool avi_enable;
    td_bool audio_enable;
    td_bool vsif_enable;
    td_bool spd_enable;
    td_bool mpeg_enable;
    td_bool gbd_enable;
    td_u8   avi[HDMI_INFO_FRAME_MAX_SIZE];
    td_u8   audio[HDMI_INFO_FRAME_MAX_SIZE];
    td_u8   vsif[HDMI_INFO_FRAME_MAX_SIZE];
    td_u8   spd[HDMI_INFO_FRAME_MAX_SIZE];
    td_u8   mpeg[HDMI_INFO_FRAME_MAX_SIZE];
    td_u8   gdb[HDMI_INFO_FRAME_MAX_SIZE];
} hdmi_infoframe_status;

typedef struct {
    td_bool hdcp22_enable;
    td_bool hdcp14_enable;
    td_bool repeater_on;
    td_u8   bksv[HDMI_BKSV_LEN];
    td_u8   aksv[HDMI_AKSV_LEN];
    td_u8   hdcp_status;
} hdmi_hdcp_status;

typedef enum {
    FRL_WORK_MODE_NONE,
    FRL_WORK_MODE_3L3G,
    FRL_WORK_MODE_3L6G,
    FRL_WORK_MODE_4L6G,
    FRL_WORK_MODE_4L8G,
    FRL_WORK_MODE_4L10G,
    FRL_WORK_MODE_4L12G,
    FRL_WORK_MODE_BUTT
} hdmi_work_mode;

typedef struct {
    td_bool frl_start;
    td_bool work_en;
    hdmi_work_mode work_mode;
} hdmi_frl_status;

typedef enum {
    FRL_TXFFE_MODE_0,
    FRL_TXFFE_MODE_1,
    FRL_TXFFE_MODE_2,
    FRL_TXFFE_MODE_3,
    FRL_TXFFE_MODE_BUTT
} hdmi_txfff_mode;

typedef struct {
    hdmi_common_status common_status;
    hdmi_phy_status phy_status;
    hdmi_video_status video_status;
    hdmi_audio_status audio_status;
    hdmi_infoframe_status info_frame_status;
    hdmi_hdcp_status hdcp_status;
    hdmi_hwspec phy_hwspec;
    hdmi_frl_status frl_status;
} hdmi_hardware_status;

typedef struct {
    td_u32  max_tmds_character_rate;
    td_bool scdc_present;
    td_bool rr_capable;
    td_bool lte340_mcsc_scramble;
    td_bool _3d_osd_disparity;
    td_bool dual_view;
    td_bool independent_view;
    td_bool dc30bit420;
    td_bool dc36bit420;
    td_bool dc48bit420;
    td_bool scdc_enable;
} hdmi_scdc_config;

typedef struct {
    td_u32 mute_delay;       /* delay for avmute */
    td_u32 fmt_delay;        /* delay for setformat */
    td_bool force_fmt_delay;  /* force setformat delay mode */
    td_bool force_mute_delay; /* force avmute delay mode */
} hdmi_delay;

typedef enum {
    HDMI_VIDEO_UNKNOWN,
    HDMI_VIDEO_PROGRESSIVE,
    HDMI_VIDEO_INTERLACE,
    HDMI_VIDEO_BUTT
} hdmi_video_format_type;

typedef struct {
    hdmi_video_code_vic video_code;
    td_u32 pixclk;
    td_u32 rate;
    td_u32 hactive;
    td_u32 vactive;
    td_u32 hblank;
    td_u32 vblank;
    td_u32 hfront;
    td_u32 hsync;
    td_u32 hback;
    td_u32 vfront;
    td_u32 vsync;
    td_u32 vback;
    hdmi_picture_aspect aspect_ratio;
    hdmi_video_timing timing;
    hdmi_video_format_type pi_type;
    td_char *fmt_str;
} hdmi_video_def;

typedef struct {
    hdmi_vsif_vic hdmi_vic;
    hdmi_video_code_vic equivalent_video_code;
    td_u32 pixclk;
    td_u32 rate;
    td_u32 hactive;
    td_u32 vactive;
    hdmi_picture_aspect aspect_ratio;
    hdmi_video_timing timing;
    hdmi_video_format_type pi_type;
    td_char *fmt_str;
} hdmi_video_4k_def;

typedef struct {
    td_u32 attach_in_time;
    td_u32 attach_out_time;
    td_u32 de_attach_in_time;
    td_u32 de_attach_out_time;
    td_u32 preformat_in_time;
    td_u32 preformat_out_time;
    td_u32 setformat_in_time;
    td_u32 setformat_out_time;
    td_u32 suspend_in_time;
    td_u32 suspend_out_time;
    td_u32 resume_in_time;
    td_u32 resume_out_time;
    td_u32 event_thread_cycle_time;
} hdmi_intf_status;

typedef struct {
    td_bool black_enable;
    td_u8 in_color_space;
    td_u8 in_bit_depth;
    td_u8 in_quantization;
} hdmi_black_frame_info;

typedef struct {
    td_u32 stop_delay;
    hdmi_intf_status intf_status;
} hdmi_debug;

typedef enum {
    HDMI_TIMER_ZERO_DRMIF,
    HDMI_TIMER_SDR_TO_HDR10,
    HDMI_TIMER_TYPE_BUTT
} hdmi_timer_type;

typedef struct {
    td_bool mute_pkg_en;
    td_bool mute_set;
    td_bool mute_clr;
    td_bool mute_rpt_en;
    td_u32  rpt_cnt;
} hdmi_avmute_cfg;

typedef enum {
    HDMI_FRL_SCDC_TYPE_SINK_VERSION,
    HDMI_FRL_SCDC_TYPE_SOURCE_VERSION,
    HDMI_FRL_SCDC_TYPE_UPDATE_FLAGS,
    HDMI_FRL_SCDC_TYPE_STATUS_FLAGS,
    HDMI_FRL_SCDC_TYPE_CONFIG,
    HDMI_FRL_SCDC_TYPE_TXFFE_REQ,
    HDMI_FRL_FLAGS_TYPE_BUTT
} hdmi_frl_scdc_type;

typedef enum {
    HDMI_FRL_TRAIN_PATTERN_NONE,
    HDMI_FRL_TRAIN_PATTERN_LP1,
    HDMI_FRL_TRAIN_PATTERN_LP2,
    HDMI_FRL_TRAIN_PATTERN_LP3,
    HDMI_FRL_TRAIN_PATTERN_LP4,
    HDMI_FRL_TRAIN_PATTERN_LP5,
    HDMI_FRL_TRAIN_PATTERN_LP6,
    HDMI_FRL_TRAIN_PATTERN_LP7,
    HDMI_FRL_TRAIN_PATTERN_LP8,
    HDMI_FRL_TRAIN_PATTERN_RESERVED,
    HDMI_FRL_TRAIN_PATTERN_0E = 0xE,
    HDMI_FRL_TRAIN_PATTERN_0F = 0xF,
    HDMI_FRL_TRAIN_PATTERN_BUTT
} hdmi_frl_train_pattern;

typedef enum {
    HDMI_FRL_TRAIN_NONE,
    HDMI_FRL_TRAIN_FAIL,
    HDMI_FRL_TRAIN_SUCCESS,
    HDMI_FRL_TRAIN_BUSY,
    HDMI_FRL_TRAIN_BUTT
} hdmi_frl_train_status;

typedef enum {
    HDMI_FRL_TRAIN_FAIL_NORMAL,
    HDMI_FRL_TRAIN_FAIL_FLTTIMEOUT,
    HDMI_FRL_TRAIN_FAIL_FLTSTEPTIMEOUT,
    HDMI_FRL_TRAIN_FAIL_RATECHANGE,
    HDMI_FRL_TRAIN_FAIL_FFECHANGE,
    HDMI_FRL_TRAIN_FAIL_BUTT
} hdmi_frl_train_fail_code;

typedef enum {
    HDMI_FRL_TRAIN_SEL_SW,
    HDMI_FRL_TRAIN_SEL_HW,
    HDMI_FRL_TRAIN_SEL_BUTT
} hdmi_frl_train_sel;

typedef enum {
    FRL_DEBUG_RATE,
    FRL_DEBUG_SW_TRAIN_SEL,
    FRL_DEBUG_LTP_PATTERN,
    FRL_DEBUG_SELECT_CHANNEL,
    FRL_DEBUG_LTS3_INTERVAL,
    FRL_DEBUG_LTS3_TIMEOUT,
    FRL_DEBUG_TRAINING_BREAK,
    FRL_DEBUG_LM_TABLE_GET,
    FRL_DEBUG_CTRL_TYPE_CONFIG,
    FRL_DEBUG_BUTT
} frl_debug_cmd;

typedef enum {
    FRL_SW_TRAIN_DELAY,
    FRL_SW_TRAIN_TIMER,
    FRL_SW_TRAIN_BUTT
} frl_sw_train_mode;

typedef enum {
    FRL_CHL_SEL_NORMAL,
    FRL_CHL_SEL_RX_TMDS,
    FRL_CHL_SEL_BUTT
} frl_channel_sel;

typedef struct {
    hdmi_frl_train_status frl_train_status;
    hdmi_frl_train_pattern train_pattern[HDMI_FRL_LANE_MAX_NUM];
    hdmi_frl_train_fail_code train_fail_res;
} hdmi_frl_train;

typedef enum {
    HDMI_FRL_MACH_MODE_STEP,
    HDMI_FRL_MACH_MODE_TIMEOUT,
    HDMI_FRL_MACH_MODE_BUTT
} hdmi_frl_mach_mode;

typedef struct {
    td_bool frl_no_timeout;
    td_u8 frl_rate;
    td_u8 ffe_levels;
    td_u32 train_timeout;
    hdmi_frl_mach_mode mach_mode;
    frl_sw_train_mode sw_train_mode;
    td_u8 ctl_type_config;
} hdmi_frl_train_config;

typedef struct {
    frl_debug_cmd debug_cmd;
    td_u8 rate;
    td_u8 ltp;
    td_u8 lane_idx;
    td_u8 training_break;
    frl_sw_train_mode sw_train_mode;
    frl_channel_sel channel_sel;
    td_u32 lts3_interval;
    td_u32 lts3_timeout;
    td_u8 crtl_type_config;
    td_u8 avi_send_by_gen5;
} frl_debug;

typedef enum {
    SCDC_CMD_SET_SOURCE_VER,
    SCDC_CMD_GET_SOURCE_VER,
    SCDC_CMD_GET_SINK_VER,
    SCDC_CMD_SET_FLT_UPDATE,
    SCDC_CMD_GET_FLT_UPDATE,
    SCDC_CMD_SET_FLT_UPDATE_TRIM,
    SCDC_CMD_GET_FLT_UPDATE_TRIM,
    SCDC_CMD_SET_FRL_START,
    SCDC_CMD_GET_FRL_START,
    SCDC_CMD_SET_CONFIG1,
    SCDC_CMD_GET_CONFIG1,
    SCDC_CMD_GET_TEST_CONFIG,
    SCDC_CMD_GET_FLT_READY,
    SCDC_CMD_GET_LTP_REQ,
    SCDC_CMD_BUTT
} scdc_cmd;

typedef struct {
    td_u8 frl_rate;
    td_u8 ffe_levels;
} scdc_config1;

typedef struct {
    td_bool pre_shoot_only;
    td_bool de_emphasis_only;
    td_bool no_ffe;
    td_bool flt_no_timeout;
    td_bool dsc_frl_max;
    td_bool frl_max;
} scdc_test_config;

typedef struct {
    td_u8 ln0_ltp;
    td_u8 ln1_ltp;
    td_u8 ln2_ltp;
    td_u8 ln3_ltp;
} scdc_ltp_req;

typedef enum {
    HDMI_PHY_MODE_CFG_TMDS,
    HDMI_PHY_MODE_CFG_FRL,
    HDMI_PHY_MODE_CFG_TXFFE
} hdmi_phy_mode_cfg;

typedef enum {
    HDMI_TRACE_LEN_0, /* 1.0 inch */
    HDMI_TRACE_LEN_1, /* 1.5 inch */
    HDMI_TRACE_LEN_2, /* 2.0 inch */
    HDMI_TRACE_LEN_3, /* 2.5 inch */
    HDMI_TRACE_LEN_4, /* 3.0 inch */
    HDMI_TRACE_LEN_5, /* 3.5 inch */
    HDMI_TRACE_LEN_6, /* 4.0 inch */
    HDMI_TRACE_LEN_7, /* 4.5 inch */
    HDMI_TRACE_LEN_8, /* 5.0 inch */
    HDMI_TRACE_DEFAULT, /* default config */
    HDMI_TRACE_BUTT
} hdmi_trace_len;

typedef struct {
    td_bool emi_en;
    hdmi_trace_len trace_len;
} hdmi_mode_param;

typedef struct {
    td_u32 pixel_clk;
    td_u32 tmds_clk; /* in khz */
    td_bool emi_enable;
    hdmi_deep_color deep_color; /* deep color(color depth) */
    hdmi_phy_mode_cfg mode_cfg; /* tmds/frl/tx ffe */
    hdmi_trace_len trace_len;
    hdmi_colorspace color_space;
    hdmi_work_mode rate; /* lane and rate */
    hdmi_txfff_mode aen_tx_ffe[AEN_TX_FFE_LEN]; /* tx ffe */
} hdmi_phy_cfg;

hdmi_video_code_vic drv_hdmi_vic_search(hdmi_video_timing, hdmi_picture_aspect, td_bool);

td_void hdmi_reg_write(volatile td_u32 *reg_addr, td_u32 value);

td_u32 hdmi_reg_read(volatile td_u32 *reg_addr);

td_u32 drv_hdmi_vic_to_index(td_u32 vic);

hdmi_video_timing drv_hdmi_video_timing_get(hdmi_video_code_vic vic, hdmi_picture_aspect aspect);

hdmi_video_timing drv_hdmi_vsif_video_timing_get(hdmi_vsif_vic vic);

hdmi_video_4k_def *drv_hdmi_video_codes_4k_get(td_u32 cnt);

hdmi_video_def *drv_hdmi_comm_format_param_get(hdmi_video_code_vic vic);

#endif /* DRV_HDMI_COMMON_H */
