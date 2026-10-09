/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File name     :  3322_hs_ctl_rb_reg_offset.h
 * Project line  :  Platform And Key Technologies Development
 * Department    :  CAD Development Department
 * Author        :  l00444884
 * Version       :  1.0
 * Date          :  2025/10/09
 * Description   :  The description of xxx project
 * Others        :  Generated automatically by nManager V5.1
 * History       :  l00444884 2025/03/05 17:25:13 Create file
 */

#ifndef __3322_HS_CTL_RB_REG_OFFSET_H__
#define __3322_HS_CTL_RB_REG_OFFSET_H__

/* HS_CTL_RB Base address of Module's Register */
#define HS_CTL_RB_BASE                       (0x52063000)

/******************************************************************************/
/*                      HS_CTL_RB Registers' Definitions                            */
/******************************************************************************/

#define HS_CTL_RB_HS_CTL_ID_REG                         (HS_CTL_RB_BASE + 0x0)   /* HS CTL ID寄存器 */
#define HS_CTL_RB_HS_GP_REG0_REG                        (HS_CTL_RB_BASE + 0x10)  /* 通用寄存器 */
#define HS_CTL_RB_HS_GP_REG1_REG                        (HS_CTL_RB_BASE + 0x14)  /* 通用寄存器 */
#define HS_CTL_RB_HS_GP_REG2_REG                        (HS_CTL_RB_BASE + 0x18)  /* 通用寄存器 */
#define HS_CTL_RB_HS_GP_REG3_REG                        (HS_CTL_RB_BASE + 0x1C)  /* 通用寄存器 */
#define HS_CTL_RB_HS_SOFT_RST_N_REG                     (HS_CTL_RB_BASE + 0x20)
#define HS_CTL_RB_HS_CLK_EN_REG                         (HS_CTL_RB_BASE + 0x24)
#define HS_CTL_RB_HS_BUS_DIV_REG                        (HS_CTL_RB_BASE + 0x28)
#define HS_CTL_RB_HS_SUB_CAN_CFG_REG                    (HS_CTL_RB_BASE + 0x2C)
#define HS_CTL_RB_HS_FMC_DIV_REG                        (HS_CTL_RB_BASE + 0x34)
#define HS_CTL_RB_FMC_SOFT_RST_PULSE_WIDTH_REG          (HS_CTL_RB_BASE + 0x38)
#define HS_CTL_RB_HS_SUB_RAM_POWER_CFG_REG              (HS_CTL_RB_BASE + 0x44)
#define HS_CTL_RB_HS_SUB_BUS_PRIORITY_REG               (HS_CTL_RB_BASE + 0x48)
#define HS_CTL_RB_PWM_CFG_REG                           (HS_CTL_RB_BASE + 0x4C)
#define HS_CTL_RB_SDIO_M_AXI_MONITOR_0_REG              (HS_CTL_RB_BASE + 0x50)
#define HS_CTL_RB_SDIO_M_AXI_MONITOR_1_REG              (HS_CTL_RB_BASE + 0x54)
#define HS_CTL_RB_SDIO_M_AXI_MONITOR_2_REG              (HS_CTL_RB_BASE + 0x58)
#define HS_CTL_RB_SDIO_M_AXI_MONITOR_3_REG              (HS_CTL_RB_BASE + 0x5C)
#define HS_CTL_RB_SDIO_M_AXI_MONITOR_4_REG              (HS_CTL_RB_BASE + 0x60)
#define HS_CTL_RB_SDIO_M_AXI_MONITOR_5_REG              (HS_CTL_RB_BASE + 0x64)
#define HS_CTL_RB_HS_AXI_BRG_LP_CFG0_REG                (HS_CTL_RB_BASE + 0x68)
#define HS_CTL_RB_HS_AXI_BRG_LP_CFG1_REG                (HS_CTL_RB_BASE + 0x6C)
#define HS_CTL_RB_HS_AXI_BRG_LP_CFG2_REG                (HS_CTL_RB_BASE + 0x70)
#define HS_CTL_RB_HS_AXI_BRG_LP_CFG3_REG                (HS_CTL_RB_BASE + 0x74)
#define HS_CTL_RB_HS_AXI_BRG_LP_CFG4_REG                (HS_CTL_RB_BASE + 0x78)
#define HS_CTL_RB_HS_AXI_BRG_LP_CFG5_REG                (HS_CTL_RB_BASE + 0x7C)
#define HS_CTL_RB_SD_HOST_CCLK_DIV_REG                  (HS_CTL_RB_BASE + 0x90)
#define HS_CTL_RB_SDIO_TUNING_CCLK_RX_CTL_REG           (HS_CTL_RB_BASE + 0x9C)
#define HS_CTL_RB_SDIO_TUNING_CARD_CLK_CTL_REG          (HS_CTL_RB_BASE + 0x100)
#define HS_CTL_RB_SDIO_TUNING_CLK_DS_CTL_REG            (HS_CTL_RB_BASE + 0x104)
#define HS_CTL_RB_SDIO_DFX_TZPC_REG                     (HS_CTL_RB_BASE + 0x108) /* SDIO DFX 读权限控制寄存器。 */
#define HS_CTL_RB_SDIO_TUNING_CLK_CFG_REG               (HS_CTL_RB_BASE + 0x10C) /* SDIO TUNING控制寄存器。 */
#define HS_CTL_RB_EMMC_TUNING_CLK2CARD_ON_DELAY_CFG_REG (HS_CTL_RB_BASE + 0x110) /* SDIO TUNING控制寄存器。 */
#define HS_CTL_RB_SDIO_TUNING_CLK2CARD_ON_DELAY_CFG_REG (HS_CTL_RB_BASE + 0x114) /* SDIO TUNING控制寄存器。 */
#define HS_CTL_RB_EMMC_HOST_CCLK_DIV_REG                (HS_CTL_RB_BASE + 0x130)
#define HS_CTL_RB_EMMC_TUNING_CCLK_RX_CTL_REG           (HS_CTL_RB_BASE + 0x13C)
#define HS_CTL_RB_EMMC_TUNING_CARD_CLK_CTL_REG          (HS_CTL_RB_BASE + 0x140)
#define HS_CTL_RB_EMMC_TUNING_CLK_DS_CTL_REG            (HS_CTL_RB_BASE + 0x144)
#define HS_CTL_RB_EMMC_DFX_TZPC_REG                     (HS_CTL_RB_BASE + 0x148) /* EMMC DFX 读权限控制寄存器。 */
#define HS_CTL_RB_EMMC_TUNING_CLK_CFG_REG               (HS_CTL_RB_BASE + 0x14C) /* SDIO TUNING控制寄存器。 */
#define HS_CTL_RB_HS_WDT_RST_EN_REG                     (HS_CTL_RB_BASE + 0x170)
#define HS_CTL_RB_HS_CUR_STS_REG                        (HS_CTL_RB_BASE + 0x174)
#define HS_CTL_RB_USB_CTRL_CFG_REG                      (HS_CTL_RB_BASE + 0x200)
#define HS_CTL_RB_USB_PHY_VBUS_REG                      (HS_CTL_RB_BASE + 0x204)
#define HS_CTL_RB_USB_RAM_POWER_MODE_REG                (HS_CTL_RB_BASE + 0x208)
#define HS_CTL_RB_USB_PHY_PCLK_EN_REG                   (HS_CTL_RB_BASE + 0x20C)
#define HS_CTL_RB_USB_PHY_PCLK_BYPASS_REG               (HS_CTL_RB_BASE + 0x210)
#define HS_CTL_RB_USB_PHY_BUS_DIV_REG                   (HS_CTL_RB_BASE + 0x214)
#define HS_CTL_RB_USB_CTRL_SP_TMOD_H_REG                (HS_CTL_RB_BASE + 0x218)
#define HS_CTL_RB_USB_CTRL_SP_TMOD_MH_REG               (HS_CTL_RB_BASE + 0x21C)
#define HS_CTL_RB_USB_CTRL_SP_TMOD_ML_REG               (HS_CTL_RB_BASE + 0x220)
#define HS_CTL_RB_USB_CTRL_SP_TMOD_L_REG                (HS_CTL_RB_BASE + 0x224)
#define HS_CTL_RB_USB_CTRL_CFG_L_REG                    (HS_CTL_RB_BASE + 0x228)
#define HS_CTL_RB_USB_CTRL_CFG_H_REG                    (HS_CTL_RB_BASE + 0x22C)
#define HS_CTL_RB_USB_CTRL_DBG1_L_REG                   (HS_CTL_RB_BASE + 0x230)
#define HS_CTL_RB_USB_CTRL_DBG2_H_REG                   (HS_CTL_RB_BASE + 0x234)
#define HS_CTL_RB_USBC_CRG_CTL_REG                      (HS_CTL_RB_BASE + 0x238) /* USB2 CTRL时钟及软复位控制寄存器。 */
#define HS_CTL_RB_USB_PHY_CRG_CTL_REG                   (HS_CTL_RB_BASE + 0x23C) /* USB2 PHY 时钟及软复位控制寄存器。 */
#define HS_CTL_RB_HS_SUB_DIAG_CTL_REG                   (HS_CTL_RB_BASE + 0x260)
#define HS_CTL_RB_HS_PERP_TCM_RAM_TMOD_CFG0_REG         (HS_CTL_RB_BASE + 0x264)
#define HS_CTL_RB_HS_PERP_TCM_RAM_TMOD_CFG1_REG         (HS_CTL_RB_BASE + 0x268)
#define HS_CTL_RB_HS_PERP_TCM_RAM_TMOD_CFG2_REG         (HS_CTL_RB_BASE + 0x26C)
#define HS_CTL_RB_HS_PERP_TCM_RAM_TMOD_CFG3_REG         (HS_CTL_RB_BASE + 0x270)
#define HS_CTL_RB_HS_PERP_TCM_TP_TMOD_CFG2_REG          (HS_CTL_RB_BASE + 0x274)
#define HS_CTL_RB_HS_PERP_TCM_TP_TMOD_CFG3_REG          (HS_CTL_RB_BASE + 0x278)
#define HS_CTL_RB_M_CAN_SEL_REG                         (HS_CTL_RB_BASE + 0x27C)
#define HS_CTL_RB_M_CAN_IR_STATUS_H_REG                 (HS_CTL_RB_BASE + 0x280)
#define HS_CTL_RB_M_CAN_IR_STATUS_L_REG                 (HS_CTL_RB_BASE + 0x284)
#define HS_CTL_RB_M_CAN_TTBRP_STATUS_H_REG              (HS_CTL_RB_BASE + 0x288)
#define HS_CTL_RB_M_CAN_TTBRP_STATUS_L_REG              (HS_CTL_RB_BASE + 0x28C)
#define HS_CTL_RB_M_CAN_MIS_REG                         (HS_CTL_RB_BASE + 0x290)
#define HS_CTL_RB_VICAP_CTRL_CFG0_REG                   (HS_CTL_RB_BASE + 0x300)
#define HS_CTL_RB_VICAP_CTRL_CFG1_REG                   (HS_CTL_RB_BASE + 0x304)
#define HS_CTL_RB_VICAP_CTRL_CFG2_REG                   (HS_CTL_RB_BASE + 0x308)
#define HS_CTL_RB_VICAP_CTRL_CFG3_REG                   (HS_CTL_RB_BASE + 0x30C)
#define HS_CTL_RB_VICAP_RAM_POWER_MODE_REG              (HS_CTL_RB_BASE + 0x310)
#define HS_CTL_RB_VICAP_SYS_TICK_VALUE_0_REG            (HS_CTL_RB_BASE + 0x320)
#define HS_CTL_RB_VICAP_SYS_TICK_VALUE_1_REG            (HS_CTL_RB_BASE + 0x324)
#define HS_CTL_RB_VICAP_SYS_TICK_VALUE_2_REG            (HS_CTL_RB_BASE + 0x328)
#define HS_CTL_RB_VICAP_SYS_TICK_VALUE_3_REG            (HS_CTL_RB_BASE + 0x32C)

#endif // __3322_HS_CTL_RB_REG_OFFSET_H__
