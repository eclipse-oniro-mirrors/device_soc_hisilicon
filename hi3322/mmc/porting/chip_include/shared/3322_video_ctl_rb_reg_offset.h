/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File name     :  3322_video_ctl_rb_reg_offset.h
 * Project line  :  Platform And Key Technologies Development
 * Department    :  CAD Development Department
 * Author        :  l00444884
 * Version       :  1.0
 * Date          :  2025/10/09
 * Description   :  The description of xxx project
 * Others        :  Generated automatically by nManager V5.1
 * History       :  l00444884 2025/03/05 17:25:13 Create file
 */

#ifndef __3322_VIDEO_CTL_RB_REG_OFFSET_H__
#define __3322_VIDEO_CTL_RB_REG_OFFSET_H__

/* VIDEO_CTL_RB Base address of Module's Register */
#define VIDEO_CTL_RB_BASE                       (0x520C0000)

/******************************************************************************/
/*                      VIDEO_CTL_RB Registers' Definitions                            */
/******************************************************************************/

#define VIDEO_CTL_RB_VIDEO_CTL_ID_REG             (VIDEO_CTL_RB_BASE + 0x0)  /* VIDEO_CTL_ID寄存器 */
#define VIDEO_CTL_RB_VIDEO_GP_REG0_REG            (VIDEO_CTL_RB_BASE + 0x4)  /* 通用寄存器 */
#define VIDEO_CTL_RB_VIDEO_GP_REG1_REG            (VIDEO_CTL_RB_BASE + 0x8)  /* 通用寄存器 */
#define VIDEO_CTL_RB_VIDEO_GP_REG2_REG            (VIDEO_CTL_RB_BASE + 0xC)  /* 通用寄存器 */
#define VIDEO_CTL_RB_VIDEO_GP_REG3_REG            (VIDEO_CTL_RB_BASE + 0x10) /* 通用寄存器 */
#define VIDEO_CTL_RB_JPGD_JPGE_CLKEN_REG          (VIDEO_CTL_RB_BASE + 0x14)
#define VIDEO_CTL_RB_VDH_SRST_REQ_REG             (VIDEO_CTL_RB_BASE + 0x18)
#define VIDEO_CTL_RB_VDH_SRST_REQ_OK_REG          (VIDEO_CTL_RB_BASE + 0x1C)
#define VIDEO_CTL_RB_VDH_BUSY_REG                 (VIDEO_CTL_RB_BASE + 0x20)
#define VIDEO_CTL_RB_JPGE_DIV_CFG_REG             (VIDEO_CTL_RB_BASE + 0x30)
#define VIDEO_CTL_RB_VIDEO_2TO1_AXI_REG           (VIDEO_CTL_RB_BASE + 0x34)
#define VIDEO_CTL_RB_H264_CUT_PULSE_REG           (VIDEO_CTL_RB_BASE + 0x38)
#define VIDEO_CTL_RB_VIDEO_CRG_CLKEN_REG          (VIDEO_CTL_RB_BASE + 0x3C)
#define VIDEO_CTL_RB_VIDEO_CRG_VDH_DIV_REG        (VIDEO_CTL_RB_BASE + 0x40)
#define VIDEO_CTL_RB_VIDEO_CRG_VO_HD0_DIV1_REG    (VIDEO_CTL_RB_BASE + 0x44)
#define VIDEO_CTL_RB_VIDEO_CRG_VO_HD0_DIV2_REG    (VIDEO_CTL_RB_BASE + 0x48)
#define VIDEO_CTL_RB_VIDEO_CRG_PPC_DIV_REG        (VIDEO_CTL_RB_BASE + 0x4C)
#define VIDEO_CTL_RB_VIDEO_CRG_SOFT_RST_REG       (VIDEO_CTL_RB_BASE + 0x50)
#define VIDEO_CTL_RB_VIDEO_CRG_VAU_DIV_REG        (VIDEO_CTL_RB_BASE + 0x54)
#define VIDEO_CTL_RB_MIPITX_CRG_CKEN_SRST_REQ_REG (VIDEO_CTL_RB_BASE + 0x58)
#define VIDEO_CTL_RB_DPU_PPROT_REG                (VIDEO_CTL_RB_BASE + 0x5C)
#define VIDEO_CTL_RB_VAU_PPROT_REG                (VIDEO_CTL_RB_BASE + 0x60)
#define VIDEO_CTL_RB_VAU_DPU_SRST_REQ_REG         (VIDEO_CTL_RB_BASE + 0x64)
#define VIDEO_CTL_RB_VIDEO_TP_RAM_TMOD_L_REG      (VIDEO_CTL_RB_BASE + 0x74)
#define VIDEO_CTL_RB_VIDEO_TP_RAM_TMOD_H_REG      (VIDEO_CTL_RB_BASE + 0x78)
#define VIDEO_CTL_RB_VIDEO_RAM_PWR_CTL_REG        (VIDEO_CTL_RB_BASE + 0x7C)
#define VIDEO_CTL_RB_VIDEO_ULPS_CTL_REG           (VIDEO_CTL_RB_BASE + 0x80)
#define VIDEO_CTL_RB_VIDEO_DEBUG_CTL_REG          (VIDEO_CTL_RB_BASE + 0x84)
#define VIDEO_CTL_RB_VIDEO_HC_RAM_TMOD_REG        (VIDEO_CTL_RB_BASE + 0x88) /* HC_RAM_TMOD */
#define VIDEO_CTL_RB_VIDEO_SP_RAM_TMOD_HH_REG     (VIDEO_CTL_RB_BASE + 0x8C)
#define VIDEO_CTL_RB_VIDEO_SP_RAM_TMOD_HL_REG     (VIDEO_CTL_RB_BASE + 0x90)
#define VIDEO_CTL_RB_VIDEO_SP_RAM_TMOD_LH_REG     (VIDEO_CTL_RB_BASE + 0x94)
#define VIDEO_CTL_RB_VIDEO_SP_RAM_TMOD_LL_REG     (VIDEO_CTL_RB_BASE + 0x98)
#define VIDEO_CTL_RB_MIPITX_CRG_CFG0_REG          (VIDEO_CTL_RB_BASE + 0xA0)
#define VIDEO_CTL_RB_MIPITX_CRG_CFG1_REG          (VIDEO_CTL_RB_BASE + 0xA4)
#define VIDEO_CTL_RB_MIPITX_PWR_REG               (VIDEO_CTL_RB_BASE + 0xA8)
#define VIDEO_CTL_RB_MIPITX_CRG_CFG2_REG          (VIDEO_CTL_RB_BASE + 0xAC)
#define VIDEO_CTL_RB_VIDEO_CRG_PPC_DIV_NUM_REG    (VIDEO_CTL_RB_BASE + 0xB0)

#endif // __3322_VIDEO_CTL_RB_REG_OFFSET_H__
