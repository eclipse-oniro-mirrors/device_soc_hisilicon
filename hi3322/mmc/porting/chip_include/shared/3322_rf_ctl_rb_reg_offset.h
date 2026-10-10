/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File name     :  3322_rf_ctl_rb_reg_offset.h
 * Project line  :  Platform And Key Technologies Development
 * Department    :  CAD Development Department
 * Author        :  l00444884
 * Version       :  1.0
 * Date          :  2025/10/09
 * Description   :  The description of xxx project
 * Others        :  Generated automatically by nManager V5.1
 * History       :  l00444884 2025/03/05 17:25:13 Create file
 */

#ifndef __3322_RF_CTL_RB_REG_OFFSET_H__
#define __3322_RF_CTL_RB_REG_OFFSET_H__

/* RF_CTL_RB Base address of Module's Register */
#define RF_CTL_RB_BASE                       (0x59002000)

/******************************************************************************/
/*                      RF_CTL_RB Registers' Definitions                            */
/******************************************************************************/

#define RF_CTL_RB_RF_SYS_CTL_ID_REG       (RF_CTL_RB_BASE + 0x0)   /* SYS CTL ID寄存器 */
#define RF_CTL_RB_RF_GP_REG0_REG          (RF_CTL_RB_BASE + 0x10)  /* 通用寄存器 */
#define RF_CTL_RB_RF_GP_REG1_REG          (RF_CTL_RB_BASE + 0x14)  /* 通用寄存器 */
#define RF_CTL_RB_RF_GP_REG2_REG          (RF_CTL_RB_BASE + 0x18)  /* 通用寄存器 */
#define RF_CTL_RB_RF_GP_REG3_REG          (RF_CTL_RB_BASE + 0x1C)  /* 通用寄存器 */
#define RF_CTL_RB_RF_BT_REG_CLK_CTL_REG   (RF_CTL_RB_BASE + 0x20)  /* RF REG控制寄存器 */
#define RF_CTL_RB_RF_BT_REG_SOFT_RSTN_REG (RF_CTL_RB_BASE + 0x24)  /* RF REG控制寄存器 */
#define RF_CTL_RB_RF_WB_CBB_DIS_REG       (RF_CTL_RB_BASE + 0x28)  /* RF REG控制寄存器 */
#define RF_CTL_RB_WL_RF_RX_STS_REG        (RF_CTL_RB_BASE + 0x2C)  /* WL RF状态寄存器 */
#define RF_CTL_RB_WL_RF_RX_CLR_REG        (RF_CTL_RB_BASE + 0x30)  /* WL RF状态清除寄存器 */
#define RF_CTL_RB_WL_RF_RX_RAW_STS_REG    (RF_CTL_RB_BASE + 0x34)  /* WL RF原始状态寄存器 */
#define RF_CTL_RB_RF_ONLY_SEL_REG         (RF_CTL_RB_BASE + 0x38)  /* RF TEST_ONLY_SEL寄存器 */
#define RF_CTL_RB_RF_REG_PRT_EN_REG       (RF_CTL_RB_BASE + 0x3C)  /* RF保护寄存器 */
#define RF_CTL_RB_RF_PLL_REG_PRT_EN_REG   (RF_CTL_RB_BASE + 0x40)  /* RF PLL保护寄存器 */
#define RF_CTL_RB_RF_REG_PRT_INT_EN_REG   (RF_CTL_RB_BASE + 0x48)  /* RF 保护中断使能寄存器 */
#define RF_CTL_RB_RF_REG_PRT_INT_STS_REG  (RF_CTL_RB_BASE + 0x4C)  /* RF 保护中断状态寄存器 */
#define RF_CTL_RB_RF_REG_PRT_INT_CLR_REG  (RF_CTL_RB_BASE + 0x50)  /* RF 保护中断清除寄存器 */
#define RF_CTL_RB_BF_ABB_IQ_EXCHANGE_REG  (RF_CTL_RB_BASE + 0x54)  /* BT FM ABB配置寄存器 */
#define RF_CTL_RB_BT_RF_DIAG_REG          (RF_CTL_RB_BASE + 0x58)  /* BT RF 电源查看 */
#define RF_CTL_RB_LDO_CTL_MAN_REG         (RF_CTL_RB_BASE + 0x80)  /* LDO_CTL_MAN */
#define RF_CTL_RB_LDO_CTL_ISO_REG         (RF_CTL_RB_BASE + 0x84)  /* LDO_CTL_ISO */
#define RF_CTL_RB_LDO_CTL_MAN_SEL_REG     (RF_CTL_RB_BASE + 0x88)  /* LDO_CTL_MAN_SEL */
#define RF_CTL_RB_RF_LDO_TRIM0_REG        (RF_CTL_RB_BASE + 0x8C)  /* RF_LDO_TRIM0 */
#define RF_CTL_RB_LDO_CTL_STS_REG         (RF_CTL_RB_BASE + 0x90)  /* LDO_CTL_STS */
#define RF_CTL_RB_LDO_CTL_ISO_STS_REG     (RF_CTL_RB_BASE + 0x94)  /* LDO_CTL_ISO_STS */
#define RF_CTL_RB_LDO_CTL_DIS_REG         (RF_CTL_RB_BASE + 0x98)  /* LDO_CTL_DIS */
#define RF_CTL_RB_RF_CTL_1_REG            (RF_CTL_RB_BASE + 0x100) /* RF_CTL_1 */
#define RF_CTL_RB_RF_CTL_2_REG            (RF_CTL_RB_BASE + 0x104) /* RF_CTL_2 */
#define RF_CTL_RB_RF_CTL_3_REG            (RF_CTL_RB_BASE + 0x108) /* RF_CTL_3 */
#define RF_CTL_RB_RF_CTL_4_REG            (RF_CTL_RB_BASE + 0x10C) /* RF_CTL_4 */
#define RF_CTL_RB_RF_CTL_5_REG            (RF_CTL_RB_BASE + 0x110) /* RF_CTL_5 */
#define RF_CTL_RB_RF_CTL_6_REG            (RF_CTL_RB_BASE + 0x114) /* RF_CTL_6 */
#define RF_CTL_RB_RF_CTL_7_REG            (RF_CTL_RB_BASE + 0x118) /* RF_CTL_7 */
#define RF_CTL_RB_RF_CTL_8_REG            (RF_CTL_RB_BASE + 0x11C) /* RF_CTL_8 */
#define RF_CTL_RB_RF_CTL_9_REG            (RF_CTL_RB_BASE + 0x120) /* RF_CTL_9 */
#define RF_CTL_RB_RF_CTL_10_REG           (RF_CTL_RB_BASE + 0x124) /* RF_CTL_10 */
#define RF_CTL_RB_RF_CTL_12_REG           (RF_CTL_RB_BASE + 0x128) /* RF_CTL_12 */
#define RF_CTL_RB_RF_CTL_13_REG           (RF_CTL_RB_BASE + 0x12C) /* RF_CTL_13 */
#define RF_CTL_RB_RF_CTL_14_REG           (RF_CTL_RB_BASE + 0x130) /* RF_CTL_14 */
#define RF_CTL_RB_RF_CTL_15_REG           (RF_CTL_RB_BASE + 0x134) /* RF_CTL_15 */
#define RF_CTL_RB_RF_CTL_16_REG           (RF_CTL_RB_BASE + 0x138) /* RF_CTL_16 */
#define RF_CTL_RB_RF_CTL_17_REG           (RF_CTL_RB_BASE + 0x13C) /* RF_CTL_17 */
#define RF_CTL_RB_RF_CTL_18_REG           (RF_CTL_RB_BASE + 0x140) /* RF_CTL_18 */
#define RF_CTL_RB_RF_CTL_19_REG           (RF_CTL_RB_BASE + 0x144) /* RF_CTL_19 */
#define RF_CTL_RB_RF_CTL_21_REG           (RF_CTL_RB_BASE + 0x148) /* RF_CTL_21 */
#define RF_CTL_RB_RF_CTL_23_REG           (RF_CTL_RB_BASE + 0x14C) /* RF_CTL_23 */
#define RF_CTL_RB_RF_CTL_25_REG           (RF_CTL_RB_BASE + 0x150) /* RF_CTL_25 */
#define RF_CTL_RB_RF_CTL_26_REG           (RF_CTL_RB_BASE + 0x154) /* RF_CTL_26 */
#define RF_CTL_RB_RF_CTL_27_REG           (RF_CTL_RB_BASE + 0x158) /* RF_CTL_27 */
#define RF_CTL_RB_RF_CTL_28_REG           (RF_CTL_RB_BASE + 0x15C) /* RF_CTL_28 */
#define RF_CTL_RB_RF_CTL_29_REG           (RF_CTL_RB_BASE + 0x160) /* RF_CTL_29 */
#define RF_CTL_RB_RF_CTL_30_REG           (RF_CTL_RB_BASE + 0x164) /* RF_CTL_30 */
#define RF_CTL_RB_RF_CTL_31_REG           (RF_CTL_RB_BASE + 0x168) /* RF_CTL_31 */
#define RF_CTL_RB_RF_LDO_TRIM0_STS_REG    (RF_CTL_RB_BASE + 0x16C) /* RF_LDO_TRIM0_STS */
#define RF_CTL_RB_RF_LDO_TRIM1_STS_REG    (RF_CTL_RB_BASE + 0x170) /* RF_LDO_TRIM1_STS */
#define RF_CTL_RB_RF_LDO_TRIM2_STS_REG    (RF_CTL_RB_BASE + 0x174) /* RF_LDO_TRIM2_STS */
#define RF_CTL_RB_RF_LDO_TRIM3_STS_REG    (RF_CTL_RB_BASE + 0x178) /* RF_LDO_TRIM3_STS */
#define RF_CTL_RB_RF_LDO_TRIM4_STS_REG    (RF_CTL_RB_BASE + 0x17C) /* RF_LDO_TRIM4_STS */
#define RF_CTL_RB_BT_RF_RESER_REG         (RF_CTL_RB_BASE + 0x180) /* BT_RF_RESER */
#define RF_CTL_RB_RF_CTL_32_REG           (RF_CTL_RB_BASE + 0x220) /* RF_CTL_32 */
#define RF_CTL_RB_RF_CTL_33_REG           (RF_CTL_RB_BASE + 0x224) /* RF_CTL_33 */

#endif // __3322_RF_CTL_RB_REG_OFFSET_H__
