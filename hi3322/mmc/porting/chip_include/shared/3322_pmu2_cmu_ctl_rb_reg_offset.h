/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File name     :  3322_pmu2_cmu_ctl_rb_reg_offset.h
 * Project line  :  Platform And Key Technologies Development
 * Department    :  CAD Development Department
 * Author        :  l00444884
 * Version       :  1.0
 * Date          :  2025/10/09
 * Description   :  The description of xxx project
 * Others        :  Generated automatically by nManager V5.1
 * History       :  l00444884 2025/03/05 17:25:13 Create file
 */

#ifndef __3322_PMU2_CMU_CTL_RB_REG_OFFSET_H__
#define __3322_PMU2_CMU_CTL_RB_REG_OFFSET_H__

/* PMU2_CMU_CTL_RB Base address of Module's Register */
#define PMU2_CMU_CTL_RB_BASE                       (0x57023000)

/******************************************************************************/
/*                      PMU2_CMU_CTL_RB Registers' Definitions                            */
/******************************************************************************/

#define PMU2_CMU_CTL_RB_PMU2_CMU_CTL_ID_REG                 (PMU2_CMU_CTL_RB_BASE + 0x0)   /* PMU2_CMU CTL ID寄存器 */
#define PMU2_CMU_CTL_RB_PMU2_GP_REG0_REG                    (PMU2_CMU_CTL_RB_BASE + 0x10)  /* 通用寄存器 */
#define PMU2_CMU_CTL_RB_PMU2_GP_REG1_REG                    (PMU2_CMU_CTL_RB_BASE + 0x14)  /* 通用寄存器 */
#define PMU2_CMU_CTL_RB_PLL_FBDIV_TARGET_REG                (PMU2_CMU_CTL_RB_BASE + 0x600) /* PLL控制寄存器 */
#define PMU2_CMU_CTL_RB_PLL_FRAC_TARGET_H_REG               (PMU2_CMU_CTL_RB_BASE + 0x604) /* PLL控制寄存器 */
#define PMU2_CMU_CTL_RB_PLL_FRAC_TARGET_L_REG               (PMU2_CMU_CTL_RB_BASE + 0x608) /* PLL控制寄存器 */
#define PMU2_CMU_CTL_RB_PLL_FBDIV_MAN_REG                   (PMU2_CMU_CTL_RB_BASE + 0x610) /* PLL控制寄存器 */
#define PMU2_CMU_CTL_RB_PLL_FRAC_MAN_H_REG                  (PMU2_CMU_CTL_RB_BASE + 0x614) /* PLL控制寄存器 */
#define PMU2_CMU_CTL_RB_PLL_FRAC_MAN_L_REG                  (PMU2_CMU_CTL_RB_BASE + 0x618) /* PLL控制寄存器 */
#define PMU2_CMU_CTL_RB_PLL_FBDIV_SEL_REG                   (PMU2_CMU_CTL_RB_BASE + 0x620) /* PLL控制寄存器 */
#define PMU2_CMU_CTL_RB_PLL_FRAC_SEL_REG                    (PMU2_CMU_CTL_RB_BASE + 0x624) /* PLL控制寄存器 */
#define PMU2_CMU_CTL_RB_PLL_CFG_VLD_REG                     (PMU2_CMU_CTL_RB_BASE + 0x628) /* PLL控制寄存器 */
#define PMU2_CMU_CTL_RB_PLL_FRAC_STEP_H_REG                 (PMU2_CMU_CTL_RB_BASE + 0x634) /* PLL控制寄存器 */
#define PMU2_CMU_CTL_RB_PLL_FRAC_STEP_L_REG                 (PMU2_CMU_CTL_RB_BASE + 0x638) /* PLL控制寄存器 */
#define PMU2_CMU_CTL_RB_PLL_FBDIV_STS_REG                   (PMU2_CMU_CTL_RB_BASE + 0x640) /* PLL控制寄存器 */
#define PMU2_CMU_CTL_RB_PLL_FRAC_STS_H_REG                  (PMU2_CMU_CTL_RB_BASE + 0x644) /* PLL控制寄存器 */
#define PMU2_CMU_CTL_RB_PLL_FRAC_STS_L_REG                  (PMU2_CMU_CTL_RB_BASE + 0x648) /* PLL控制寄存器 */
#define PMU2_CMU_CTL_RB_EFUSE_IP_CFG_REG                    (PMU2_CMU_CTL_RB_BASE + 0x650) /* EFUSE寄存器 */
#define PMU2_CMU_CTL_RB_AON_BUS_CFG_REG                     (PMU2_CMU_CTL_RB_BASE + 0x654) /* EFUSE寄存器 */
#define PMU2_CMU_CTL_RB_FAST_XO_INIT_CNT_CLKIN_CLR_REG      (PMU2_CMU_CTL_RB_BASE + 0x658) /* FAST_XO启动时间清除 */
#define PMU2_CMU_CTL_RB_FAST_XO_INIT_CNT_CLKIN_H_REG        (PMU2_CMU_CTL_RB_BASE + 0x65C) /* FAST_XO启动时间 */
#define PMU2_CMU_CTL_RB_FAST_XO_INIT_CNT_CLKIN_L_REG        (PMU2_CMU_CTL_RB_BASE + 0x660) /* FAST_XO启动时间 */
#define PMU2_CMU_CTL_RB_FAST_XO_INIT_CNT_TARGET_CLKIN_H_REG (PMU2_CMU_CTL_RB_BASE + 0x664) /* FAST_XO启动时间目标值 */
#define PMU2_CMU_CTL_RB_FAST_XO_INIT_CNT_TARGET_CLKIN_L_REG (PMU2_CMU_CTL_RB_BASE + 0x668) /* FAST_XO启动时间目标值 */
#define PMU2_CMU_CTL_RB_AON_BUS_IP_CLKEN_REG                (PMU2_CMU_CTL_RB_BASE + 0x66C) /* AON BUS 配置使能 */

#endif // __3322_PMU2_CMU_CTL_RB_REG_OFFSET_H__
