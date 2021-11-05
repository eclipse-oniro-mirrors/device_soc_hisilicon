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

#ifndef __SYS_EXT_H__
#define __SYS_EXT_H__

#include "hi_type.h"
#include "mod_ext.h"
#include "hi_comm_video_adapt.h"
#include "hi_comm_sys_adapt.h"

#ifdef __cplusplus
extern "C" {
#endif /* end of #ifdef __cplusplus */

#define SYS_DDR_MAXNUM                16

#define SYS_SEND_DATA_BLOCK_MASK      0x1
#define SYS_SEND_DATA_BLOCK_OFFSET    0
#define SYS_SEND_DATA_NOBLOCK         ((0 << SYS_SEND_DATA_BLOCK_OFFSET) & SYS_SEND_DATA_BLOCK_MASK)
#define SYS_SEND_DATA_BLOCK           ((1 << SYS_SEND_DATA_BLOCK_OFFSET) & SYS_SEND_DATA_BLOCK_MASK)

#define SYS_SEND_DATA_DELAY_MASK      0x6
#define SYS_SEND_DATA_DELAY_OFFSET    1
#define SYS_SEND_DATA_FINISH          ((0 << SYS_SEND_DATA_DELAY_OFFSET) & SYS_SEND_DATA_DELAY_MASK)
#define SYS_SEND_DATA_LOWDELAY        ((1 << SYS_SEND_DATA_DELAY_OFFSET) & SYS_SEND_DATA_DELAY_MASK)
#define SYS_SEND_DATA_LOWDELAY_FINISH ((2 << SYS_SEND_DATA_DELAY_OFFSET) & SYS_SEND_DATA_DELAY_MASK)

typedef struct {
    hi_u32 num;
    hi_void *mmz_name[SYS_DDR_MAXNUM];
} mpp_sys_ddr_name;

typedef enum {
    MPP_DATA_VI_FRAME,
    MPP_DATA_VOU_FRAME,
    MPP_DATA_VDEC_FRAME,
    MPP_DATA_VIDEO_FRAME,
    MPP_DATA_VOU_WBC_FRAME,
    MPP_DATA_AUDIO_FRAME,
    MPP_DATA_VPSS_FRAME,
    MPP_DATA_DPU_RECT_FRAME,
    MPP_DATA_AVS_FRAME,
    MPP_DATA_MCF_FRAME,
    MPP_DATA_BUTT
} mpp_data_type;

typedef struct {
    hi_mod_id mod_id;
    hi_u32 max_dev_cnt;
    hi_u32 max_chn_cnt;
    hi_s32 (*give_bind_call_back)(hi_s32 dev_id, hi_s32 chn_id, hi_mpp_bind_dest *bind_send);
} bind_sender_info;

typedef struct {
    hi_mod_id mod_id;
    hi_u32 max_dev_cnt;
    hi_u32 max_chn_cnt;
    hi_s32 (*call_back)(hi_s32 dev_id, hi_s32 chn_id, hi_bool block, mpp_data_type data_type, hi_void *pv_data);
    hi_s32 (*reset_call_back)(hi_s32 dev_id, hi_s32 chn_id, hi_void *pv_data);
    hi_bool support_delay_data;
} bind_receiver_info;

typedef struct {
    hi_pixel_format pixel_format;
    hi_u32 width;
    hi_u32 height;
    hi_u32 buf_line;
    hi_u32 align;  /* 0:auto */
    hi_dynamic_range dynamic_range;
    hi_compress_mode compress_mode;
    hi_video_format video_format;
    hi_bool is_3dnr_buffer;
} vb_base_info;

typedef enum {
    SYS_VO_BT1120_CLK_SEL,
    SYS_VO_HDDAC_CLK_SEL,
    SYS_VO_HDDATE_CLK_SEL,
    SYS_VO_HDMI_CLK_SEL,
    SYS_VO_GET_HD_CLK_SEL,
    SYS_VO_BT1120_CLK_EN,
    SYS_VO_MIPI_TX_CLK_EN,
    SYS_VO_MIPI_CLK_EN,
    SYS_VO_HDMI_TX_CLK_EN,
    SYS_VO_MIPI_CLK_SEL,
    SYS_VO_MIPI_CHN_SEL,
    SYS_VO_PLL_FRAC_SET,
    SYS_VO_PLL_POSTDIV1_SET,
    SYS_VO_PLL_POSTDIV2_SET,
    SYS_VO_PLL_REFDIV_SET,
    SYS_VO_PLL_FBDIV_SET,
    SYS_VO_SPLL_FRAC_SET,
    SYS_VO_SPLL_POSTDIV1_SET,
    SYS_VO_SPLL_POSTDIV2_SET,
    SYS_VO_SPLL_REFDIV_SET,
    SYS_VO_SPLL_FBDIV_SET,

    SYS_AI_SAMPLECLK_DIVSEL,
    SYS_AI_BITCKL_DIVSEL,
    SYS_AI_SYSCKL_SEL,
    SYS_AI_MASTER,
    SYS_AI_RESET_SEL,
    SYS_AI_CLK_EN,

    SYS_AO_SAMPLECLK_DIVSEL,
    SYS_AO_BITCKL_DIVSEL,
    SYS_AO_SYSCKL_SEL,
    SYS_AO_MASTER,
    SYS_AO_RESET_SEL,
    SYS_AO_CLK_EN,

    SYS_AIO_RESET_SEL,
    SYS_AIO_CLK_EN,
    SYS_AIO_CLK_SEL,

    SYS_AIO_RX0SD_SEL,
    SYS_AIO_HDMISD_SEL,
    SYS_AIO_PADSD_SEL,
    SYS_AIO_CODECSD_SEL,
    SYS_AIO_HDMICLK_SEL,
    SYS_AIO_PADCLK_SEL,
    SYS_AIO_CODECCLK_SEL,

    SYS_ACODEC_RESET_SEL,

    SYS_VI_BUS_RESET_SEL,
    SYS_VI_BUS_CLK_EN,
    SYS_VI_PPC_RESET_SEL,
    SYS_VI_PPC_CLK_EN,
    SYS_VI_PORT_RESET_SEL,
    SYS_VI_PORT_CLK_EN,
    SYS_VI_PPC_PORT_CLK_EN,
    SYS_VI_PPC_CHN_CLK_EN,
    SYS_VI_DEV_RESET_SEL,
    SYS_VI_DEV_CLK_EN,
    SYS_VI_PROC_RESET_SEL,
    SYS_VI_PROC_CLK_EN,
    SYS_VI_PROC_PWR_EN,
    SYS_VI_PROC_PWR_ISO,

    SYS_VI_ISP_CFG_RESET_SEL,
    SYS_VI_ISP_CORE_RESET_SEL,
    SYS_VI_ISP_CLK_EN,
    SYS_VI_CAP_GET_ONLINE_FLAG,
    SYS_VI_CAP_PROC_ONLINE_SEL,
    SYS_VI_PROC_VPSS_ONLINE_SEL,
    SYS_VI_DEV_BIND_MIPI_SEL,
    SYS_VI_DEV_BIND_SLVS_SEL,
    SYS_VI_DEV_INPUT_SEL,
    SYS_VI_DEV_GET_PORT_CLK,
    SYS_VI_DEV_SET_PORT_CLK,
    SYS_VI_DEV_GET_PPC_CLK,
    SYS_VI_DEV_SET_PPC_CLK,
    SYS_VI_PROC_GET_CLK,
    SYS_VI_PROC_SET_CLK,
    SYS_ISPFE_SET_CLK,
    SYS_ISPFE_GET_CLK,
    SYS_SENSOR_CLK_OUT_SEL,

    SYS_VOU_BUS_CLK_EN,
    SYS_VOU_BUS_RESET_SEL,
    SYS_VOU_SD_RESET_SEL,
    SYS_VOU_HD_RESET_SEL,
    SYS_VOU_HD_DATE_RESET_SEL,
    SYS_VOU_HD_CLK_EN,
    SYS_VOU_DEV_CLK_EN,
    SYS_VOU_HD_OUT_CLK_EN,
    SYS_VOU_HD_CLKOUT_PHASIC_REVERSE_EN,
    SYS_VOU_DAC_CLK_PHASIC_REVERSE_EN,
    SYS_VOU_CFG_CLK_EN,
    SYS_VOU_CORE_CLK_EN,
    SYS_VOU_PPC_CLK_EN,
    SYS_VOU_APB_CLK_EN,
    SYS_VOU_OUT_CLK_EN,
    SYS_VOU_OUT_CLK_SEL,
    SYS_VOU_HD_CLK_SEL,
    SYS_VOU_HD0_PPC_SEL,
    SYS_VOU_HD1_PPC_SEL,
    SYS_VOU_HD1_DIV_MOD,
    SYS_VOU_HD0_DIV_MOD,
    SYS_VO_HDMI_CLK_EN,
    SYS_VOU_SD_DATE_CLK_EN,
    SYS_VOU_HD_DATE_CLK_EN,
    SYS_VOU_SD_DAC_EN,
    SYS_VOU_SD_DAC_PWR_EN,
    SYS_VOU_SD_DAC_DETECT_EN,
    SYS_VOU_HD_DAC_EN,
    SYS_VOU_DEV_MODE_SEL,
    SYS_VO_CLK_SEL,
    SYS_VO_SD_CLK_DIV,
    SYS_VOU_WORK_EN,
    SYS_VOU_LCD_CLK_SEL,
    SYS_VOU_HD_LCD_CLK_SEL,
    SYS_VOU_LCD_CLK_EN,
    SYS_LCD_MCLK_DIV,
    SYS_LCD_DATA_MODE,

    SYS_HDMI_RESET_SEL,
    SYS_HDMI_PIXEL_CLK_EN,
    SYS_HDMI_BUS_CLK_EN,

    SYS_VO_LOW_POWER_CTRL,

    SYS_VEDU_RESET_SEL,
    SYS_VEDU_CLK_EN,
    SYS_VEDU_CLK_SEL,
    SYS_VEDU_SED_RESET_SEL,
    SYS_VEDU_SED_CLK_EN,
    SYS_VEDU_PWR_EN,
    SYS_VEDU_PWR_ISO,
    SYS_VEDU_PWR_STATE,

    SYS_VPSS_CLK_SEL,
    SYS_VPSS_RESET_SEL,
    SYS_VPSS_CLK_EN,
    SYS_VPSS_GET_CLK_SEL,
    SYS_VPSS_SET_CLK_SEL,
    SYS_AVS_RESET_SEL,
    SYS_AVS_CLK_EN,
    SYS_AVS_PWR_EN,
    SYS_AVS_PWR_ISO,
    SYS_AVS_PWR_STATE,

    SYS_TDE_RESET_SEL,
    SYS_TDE_CLK_EN,

    SYS_JPGE_RESET_SEL,
    SYS_JPGE_CLK_EN,
    SYS_JPGE_SEL_CLK,

    SYS_JPGD_CLOCK_SEL,
    SYS_JPGD_RESET_SEL,
    SYS_JPGD_CLK_EN,

    SYS_MD_RESET_SEL,
    SYS_MD_CLK_EN,

    SYS_IVE_RESET_SEL,
    SYS_IVE_CLK_EN,
    SYS_IVE_CLK_SET,

    SYS_FD_RESET_SEL,
    SYS_FD_CLK_EN,

    SYS_DSP_CLK_SET,
    SYS_DSP_LOW_POWER_CLK_GT_EN,
    SYS_DSP0_CORE_RESET_SEL,
    SYS_DSP0_CFG_RESET_SEL,
    SYS_DSP0_DBG_RESET_SEL,
    SYS_DSP0_CLK_EN,
    SYS_DSP0_POWER_EN,
    SYS_DSP0_POWER_STATE,
    SYS_DSP1_CORE_RESET_SEL,
    SYS_DSP1_CFG_RESET_SEL,
    SYS_DSP1_DBG_RESET_SEL,
    SYS_DSP1_CLK_EN,
    SYS_DSP1_POWER_EN,
    SYS_DSP1_POWER_STATE,
    SYS_DSP2_CORE_RESET_SEL,
    SYS_DSP2_CFG_RESET_SEL,
    SYS_DSP2_DBG_RESET_SEL,
    SYS_DSP2_CLK_EN,
    SYS_DSP2_POWER_EN,
    SYS_DSP2_POWER_STATE,
    SYS_DSP3_CORE_RESET_SEL,
    SYS_DSP3_CFG_RESET_SEL,
    SYS_DSP3_DBG_RESET_SEL,
    SYS_DSP3_CLK_EN,
    SYS_DSP3_POWER_EN,
    SYS_DSP3_POWER_STATE,

    SYS_EV_CNN_RESET_SEL,
    SYS_EV_CNN_CFG_RESET_SEL,
    SYS_EV_CNN_CLK_EN,

    SYS_WK_CNN_RESET_SEL,
    SYS_WK_CNN_CLK_EN,
    SYS_WK_CNN_GET_CLK_STATE,
    SYS_WK_CNN_CLK_SET,
    SYS_WK_CNN_CLK_GET,
    SYS_WK_CNN_PWR_EN,
    SYS_WK_CNN_PWR_ISO,
    SYS_WK_CNN_PWR_STATE,
    SYS_WK_CNN1_RESET_SEL,
    SYS_WK_CNN1_CLK_EN,
    SYS_WK_CNN1_CLK_SET,
    SYS_WK_CNN1_CLK_GET,
    SYS_WK_CNN1_PWR_EN,
    SYS_WK_CNN1_PWR_ISO,
    SYS_WK_CNN1_PWR_STATE,
    SYS_DPU_CLK_SET,
    SYS_DPU_RESET_SEL,
    SYS_DPU_CLK_EN,

    SYS_PCIE_RESET_SEL,
    SYS_PCIE_CLK_EN,

    SYS_CIPHER_RESET_SEL,
    SYS_CIPHER_CLK_EN,

    SYS_VGS_RESET_SEL,
    SYS_VGS_CLK_EN,
    SYS_VGS_GET_CLK_SEL,
    SYS_VGS_SET_CLK_SEL,
    SYS_VGS_BOOTROOM_RAM_USE,

    SYS_GDC_RESET_SEL,
    SYS_GDC_CLK_EN,
    SYS_GDC_GET_CLK_STATE,
    SYS_GDC_NNIE_RAM_USE,
    SYS_GDC_GET_CLKSEL,
    SYS_GDC_SET_CLKSEL,

    SYS_GDC_VGS_PWR_EN,
    SYS_GDC_VGS_PWR_ISO,
    SYS_GDC_VGS_PWR_STATE,

    SYS_VPSS_PWR_EN,
    SYS_VPSS_PWR_ISO,
    SYS_VPSS_PWR_STATE,

    SYS_DIS_RESET_SEL,
    SYS_DIS_CLK_EN,

    SYS_GDC_NNIE_MUTEX_SEL,
    SYS_VENC_NNIE_MUTEX_SEL,
    SYS_NNIE_GET_MUTEX_STATE,
    SYS_NNIE_GDC_GET_MUTEX_STATE,

    SYS_GET_SYS,
    SYS_GET_SCT,
    SYS_SOFT_INT_EN,

    SYS_GET_UDIR_CLK_STATE,
    SYS_SET_UDIR_CLK_STATE,
    SYS_UDIR_RESET_SEL,

    SYS_BUTT,
} sys_func;

#ifdef CONFIG_HI_TZASC_SUPPORT
typedef enum {
    NON_TEE_VERSION = 0,
    TEE_VERSION = 1,
} sys_security_version;
#endif

typedef struct {
    hi_u64 (*pfn_sys_get_time_stamp)(hi_void);
    hi_u64 (*pfn_sys_get_local_time_stamp)(hi_void);
    hi_void (*pfn_sys_sync_time_stamp)(hi_u64 base, hi_bool init);
    hi_u32 (*pfn_sys_get_chip_version)(hi_void);
    hi_s32 (*pfn_sys_get_stride)(hi_u32 width, hi_u32 *stride);
    hi_s32 (*pfn_sys_drv_ioctrl)(hi_mpp_chn *mpp_chn, sys_func func_id, hi_void *io_args);

    hi_s32 (*pfn_sys_register_sender)(bind_sender_info *info);
    hi_s32 (*pfn_sys_unregister_sender)(hi_mod_id mod_id);
    hi_s32 (*pfn_sys_register_receiver)(bind_receiver_info *info);
    hi_s32 (*pfn_sys_unregister_receiver)(hi_mod_id mod_id);
    hi_s32 (*pfn_sys_send_data)(hi_mod_id mod_id, hi_s32 dev_id, hi_s32 chn_id, hi_u32 flag,
        mpp_data_type data_type, hi_void *pv_data);
    hi_s32 (*pfn_sys_reset_data)(hi_mod_id mod_id, hi_s32 dev_id, hi_s32 chn_id, hi_void *private);

    hi_s32 (*pfn_get_bind_by_src)(hi_mpp_chn *src_chn, hi_mpp_bind_dest *bind_src);
    hi_s32 (*pfn_get_bind_num_by_src)(hi_mpp_chn *src_chn, hi_u32 *bind_num);
    hi_s32 (*pfn_get_bind_by_dest)(hi_mpp_chn *dest_chn, hi_mpp_chn *src_chn);
    hi_s32 (*pfn_get_mmz_name)(hi_mpp_chn *chn, hi_void **mmz_name);
    hi_s32 (*pfn_get_mem_ddr)(hi_mpp_chn *chn, mpp_sys_ddr_name *mem_ddr);
    hi_s32 (*pfn_get_spin_rec)(hi_u32 *spin_rec);

    hi_u32 (*pfn_sys_vreg_read)(hi_u32 addr, hi_u32 bytes);
    hi_s32 (*pfn_sys_vreg_write)(hi_u32 addr, hi_u32 value, hi_u32 bytes);
    hi_u32 (*pfn_sys_get_vreg_addr)(hi_void);
    hi_u32 (*pfn_sys_get_scale_coef)(hi_mod_id mod_id, hi_void *scale_coef_opt, hi_void *pv_rate, hi_void *pv_cfg);
    hi_u64 (*pfn_sys_get_sched_clock)(hi_void);
    hi_s32 (*pfn_sys_get_full_chip_id)(hi_u32 *chip_id);

    hi_u32 (*pfn_sys_get_chip_id)(hi_void);
    hi_u64 (*pfn_sys_get_efuse_addr)(hi_void);

    hi_s32 (*pfn_sys_get_time_zone)(hi_s32 *time_zone);
    hi_s32 (*pfn_sys_get_now_string)(hi_u8 *now_time);
    hi_gps_info *(*pfn_sys_get_gps_info)(hi_void);

    hi_void (*pfn_sys_get_vb_cfg)(hi_void *vb_base_info, hi_void *vb_cal_config);
    hi_void (*pfn_sys_get_vdec_buffer_cfg)(hi_payload_type type, hi_void *vb_base_info, hi_void *vb_cal_config);

    hi_void (*pfn_sys_get_cmp_seg_cfg)(hi_void *sys_cmp_in, hi_void *y_cmp_cfg, hi_void *cmp_cfg);
    hi_void (*pfn_sys_get_cmp_bayer_cfg)(hi_void *sys_cmp_in, hi_void *bayer_cmp_cfg, hi_void *bayer_dcmp_cfg);
    hi_void (*pfn_sys_get_cmp_3dnr_cfg)(hi_void *sys_cmp_in, hi_void *y_cmp_cfg, hi_void *cmp_cfg);
#ifdef CONFIG_HI_AVS_SUPPORT
    hi_void (*pfn_sys_get_cmp_tile16x8_cfg)(hi_void *sys_cmp_in, hi_void *cmp_out);
#endif
#ifdef HI_DEBUG
    hi_void (*pfn_sys_get_cmp_tde_cfg)(hi_void *sys_cmp_in, hi_void *cmp_out);
#endif

#ifdef CONFIG_HI_TZASC_SUPPORT
    hi_s32 (*pfn_sys_get_security_version)(sys_security_version *version);
#endif
} sys_export_func;

#define ckfn_sys_entry() check_func_entry(HI_ID_SYS)

#define ckfn_sys_get_sched_clock() \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_sys_get_sched_clock != HI_NULL)
#define call_sys_get_sched_clock() \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_sys_get_sched_clock())

#define ckfn_sys_get_time_stamp() \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_sys_get_time_stamp != HI_NULL)
#define call_sys_get_time_stamp() \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_sys_get_time_stamp())

#define ckfn_sys_get_local_time_stamp() \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_sys_get_local_time_stamp != HI_NULL)
#define call_sys_get_local_time_stamp() \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_sys_get_local_time_stamp())

#define ckfn_sys_sync_time_stamp() \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_sys_sync_time_stamp != HI_NULL)
#define call_sys_sync_time_stamp(base, init) \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_sys_sync_time_stamp(base, init))
#define ckfn_sys_drv_ioctrl() \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_sys_drv_ioctrl != HI_NULL)
#define call_sys_drv_ioctrl(mpp_chn, func_id, io_args) \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_sys_drv_ioctrl(mpp_chn, func_id, io_args))

#define ckfn_sys_register_sender() \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_sys_register_sender != HI_NULL)
#define call_sys_register_sender(info) \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_sys_register_sender(info))

#define ckfn_sys_unregister_sender() \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_sys_unregister_sender != HI_NULL)
#define call_sys_unregister_sender(mod_id) \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_sys_unregister_sender(mod_id))

#define ckfn_sys_register_receiver() \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_sys_register_receiver != HI_NULL)
#define call_sys_register_receiver(info) \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_sys_register_receiver(info))

#define ckfn_sys_unregister_receiver() \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_sys_unregister_receiver != HI_NULL)
#define call_sys_unregister_receiver(mod_id) \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_sys_unregister_receiver(mod_id))

#define ckfn_sys_send_data() \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_sys_send_data != HI_NULL)
#define call_sys_send_data(mod_id, dev_id, chn_id, flag, data_type, pv_data) \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_sys_send_data(mod_id, dev_id, chn_id, flag, data_type, pv_data))

#define ckfn_sys_reset_data() \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_sys_reset_data != HI_NULL)
#define call_sys_reset_data(mod_id, dev_id, chn_id, private) \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_sys_reset_data(mod_id, dev_id, chn_id, private))

#define ckfn_sys_get_bind_by_src() \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_get_bind_by_src != HI_NULL)
#define call_sys_get_bind_by_src(src_chn, bind_dest) \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_get_bind_by_src(src_chn, bind_dest))

#define ckfn_sys_get_bind_num_by_src() \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_get_bind_num_by_src != HI_NULL)
#define call_sys_get_bind_num_by_src(src_chn, bind_num) \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_get_bind_num_by_src(src_chn, bind_num))

#define ckfn_sys_get_bind_by_dest() \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_get_bind_by_dest != HI_NULL)
#define call_sys_get_bind_by_dest(dest_chn, src_chn) \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_get_bind_by_dest(dest_chn, src_chn))

#define ckfn_sys_get_mmz_name() \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_get_mmz_name != HI_NULL)
#define call_sys_get_mmz_name(src_chn, mmz_name) \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_get_mmz_name(src_chn, mmz_name))

#define call_sys_get_efuse_addr() \
    (func_entry(sys_export_func, HI_ID_SYS)->pfn_sys_get_efuse_addr())

#ifdef __cplusplus
}
#endif /* end of #ifdef __cplusplus */

#endif /* __SYS_EXT_H__ */
