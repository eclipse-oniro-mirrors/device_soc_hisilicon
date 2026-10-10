/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File name     :  3322_lp_ctl_ulp_b_rb_reg_offset.h
 * Project line  :  Platform And Key Technologies Development
 * Department    :  CAD Development Department
 * Author        :  l00444884
 * Version       :  1.0
 * Date          :  2025/10/09
 * Description   :  The description of xxx project
 * Others        :  Generated automatically by nManager V5.1
 * History       :  l00444884 2025/03/05 17:25:13 Create file
 */

#ifndef __3322_LP_CTL_ULP_B_RB_REG_OFFSET_H__
#define __3322_LP_CTL_ULP_B_RB_REG_OFFSET_H__

/* LP_CTL_ULP_B_RB Base address of Module's Register */
#define LP_CTL_ULP_B_RB_BASE                       (0x5701B000)

/******************************************************************************/
/*                      LP_CTL_ULP_B_RB Registers' Definitions                            */
/******************************************************************************/

#define LP_CTL_ULP_B_RB_CFG_LP_CTL_ULP_B_ID_REG         (LP_CTL_ULP_B_RB_BASE + 0x0)   /* LP_CTL_ULP_BID寄存器 */
#define LP_CTL_ULP_B_RB_CFG_LP_CTL_ULP_B_GP_REG1_REG    (LP_CTL_ULP_B_RB_BASE + 0x4)   /* 通用寄存器 */
#define LP_CTL_ULP_B_RB_CFG_LP_CTL_ULP_B_GP_REG2_REG    (LP_CTL_ULP_B_RB_BASE + 0x8)   /* 通用寄存器 */
#define LP_CTL_ULP_B_RB_CFG_LP_CTL_ULP_B_GP_REG3_REG    (LP_CTL_ULP_B_RB_BASE + 0xC)   /* 通用寄存器 */
#define LP_CTL_ULP_B_RB_CFG_LP_CTL_ULP_B_GP_REG4_REG    (LP_CTL_ULP_B_RB_BASE + 0x10)  /* 通用寄存器 */
#define LP_CTL_ULP_B_RB_CFG_LP_CTL_ULP_B_GP_REG5_REG    (LP_CTL_ULP_B_RB_BASE + 0x14)  /* 通用寄存器 */
#define LP_CTL_ULP_B_RB_CFG_LP_CTL_ULP_B_GP_REG6_REG    (LP_CTL_ULP_B_RB_BASE + 0x18)  /* 通用寄存器 */
#define LP_CTL_ULP_B_RB_SYS_TICK_CFG_ULP_REG            (LP_CTL_ULP_B_RB_BASE + 0x30)
#define LP_CTL_ULP_B_RB_CFG_SYS_TICK_VALUE_ULP_0_REG    (LP_CTL_ULP_B_RB_BASE + 0x34)
#define LP_CTL_ULP_B_RB_CFG_SYS_TICK_VALUE_ULP_1_REG    (LP_CTL_ULP_B_RB_BASE + 0x38)
#define LP_CTL_ULP_B_RB_CFG_SYS_TICK_VALUE_ULP_2_REG    (LP_CTL_ULP_B_RB_BASE + 0x3C)
#define LP_CTL_ULP_B_RB_CFG_SYS_TICK_VALUE_ULP_3_REG    (LP_CTL_ULP_B_RB_BASE + 0x40)
#define LP_CTL_ULP_B_RB_CFG_SYS_TICK_VALUE_THR_0_REG    (LP_CTL_ULP_B_RB_BASE + 0x48)
#define LP_CTL_ULP_B_RB_CFG_SYS_TICK_VALUE_THR_1_REG    (LP_CTL_ULP_B_RB_BASE + 0x4C)
#define LP_CTL_ULP_B_RB_CFG_SYS_TICK_VALUE_THR_2_REG    (LP_CTL_ULP_B_RB_BASE + 0x50)
#define LP_CTL_ULP_B_RB_CFG_SYS_TICK_VALUE_THR_3_REG    (LP_CTL_ULP_B_RB_BASE + 0x54)
#define LP_CTL_ULP_B_RB_POWER_UP_DOWN_SHIP_MODE_SEL_REG (LP_CTL_ULP_B_RB_BASE + 0x70)
#define LP_CTL_ULP_B_RB_PWR_ON_N_FILTER_CTRL_REG        (LP_CTL_ULP_B_RB_BASE + 0x74)
#define LP_CTL_ULP_B_RB_PMU1_1MS_GRM_CTRL_REG           (LP_CTL_ULP_B_RB_BASE + 0x78)
#define LP_CTL_ULP_B_RB_PMU1_80MS_GRM_CTRL_REG          (LP_CTL_ULP_B_RB_BASE + 0x7C)
#define LP_CTL_ULP_B_RB_HRESET_MODE_CTRL_REG            (LP_CTL_ULP_B_RB_BASE + 0x80)
#define LP_CTL_ULP_B_RB_RC32K_GRM_BYPASS_CTRL_REG       (LP_CTL_ULP_B_RB_BASE + 0x84)
#define LP_CTL_ULP_B_RB_PDACK_BYPASS_CTRL_REG           (LP_CTL_ULP_B_RB_BASE + 0x88)
#define LP_CTL_ULP_B_RB_VBUS_GRM_CTRL_REG               (LP_CTL_ULP_B_RB_BASE + 0x8C)
#define LP_CTL_ULP_B_RB_REBOOT_CNT_CLR_CTRL_REG         (LP_CTL_ULP_B_RB_BASE + 0x90)
#define LP_CTL_ULP_B_RB_ULP_FSM_CUR_STATE_REG           (LP_CTL_ULP_B_RB_BASE + 0x100)
#define LP_CTL_ULP_B_RB_DEBUG_MODE_DET_RESULT_REG_REG   (LP_CTL_ULP_B_RB_BASE + 0x108) /* BOOT_MODE检测寄存器 */
#define LP_CTL_ULP_B_RB_JTAG_MODE_DET_RESULT_REG_REG    (LP_CTL_ULP_B_RB_BASE + 0x10C) /* JTAG_MODE检测寄存器 */
#define LP_CTL_ULP_B_RB_RC_XO_CLK_MUX_RESULT_REG        (LP_CTL_ULP_B_RB_BASE + 0x114) /* RC_XO时钟无毛刺切换寄存器寄存器 */
#define LP_CTL_ULP_B_RB_RC_PCLKLP_CLK_MUX_RESULT_REG    (LP_CTL_ULP_B_RB_BASE + 0x118) /* RC_PCLK_LP时钟无毛刺切换寄存器寄存器 */
#define LP_CTL_ULP_B_RB_GPIO_ULP_BUS_STS_REG            (LP_CTL_ULP_B_RB_BASE + 0x11C) /* GPIO总线使能信号状态 */
#define LP_CTL_ULP_B_RB_HARDWARE_CTL_SIGNAL_STS_REG     (LP_CTL_ULP_B_RB_BASE + 0x120) /* 硬控信号最终信号状态回读 */
#define LP_CTL_ULP_B_RB_PMU_STICK_CLR_0_REG             (LP_CTL_ULP_B_RB_BASE + 0x200)
#define LP_CTL_ULP_B_RB_PMU_STICK_0_REG                 (LP_CTL_ULP_B_RB_BASE + 0x204)
#define LP_CTL_ULP_B_RB_PMU_STICK_4_REG                 (LP_CTL_ULP_B_RB_BASE + 0x208)
#define LP_CTL_ULP_B_RB_PWR_ON_VBUS_STATE_REG           (LP_CTL_ULP_B_RB_BASE + 0x210)
#define LP_CTL_ULP_B_RB_PWR_HOLD_HRESET_SLEEP_STATE_REG (LP_CTL_ULP_B_RB_BASE + 0x214)

#endif // __3322_LP_CTL_ULP_B_RB_REG_OFFSET_H__
