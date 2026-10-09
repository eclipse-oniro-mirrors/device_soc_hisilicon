/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File name     :  3322_tee_ctl_rb_reg_offset.h
 * Project line  :  Platform And Key Technologies Development
 * Department    :  CAD Development Department
 * Author        :  l00444884
 * Version       :  1.0
 * Date          :  2025/10/09
 * Description   :  The description of xxx project
 * Others        :  Generated automatically by nManager V5.1
 * History       :  l00444884 2025/03/05 17:25:14 Create file
 */

#ifndef __3322_TEE_CTL_RB_REG_OFFSET_H__
#define __3322_TEE_CTL_RB_REG_OFFSET_H__

/* TEE_CTL_RB Base address of Module's Register */
#define TEE_CTL_RB_BASE                       (0x52018000)

/******************************************************************************/
/*                      TEE_CTL_RB Registers' Definitions                            */
/******************************************************************************/

#define TEE_CTL_RB_TEE_CTL_ID_REG                (TEE_CTL_RB_BASE + 0x0)
#define TEE_CTL_RB_TEE_GP_REG0_REG               (TEE_CTL_RB_BASE + 0x10) /* 通用寄存器 */
#define TEE_CTL_RB_TEE_GP_REG1_REG               (TEE_CTL_RB_BASE + 0x14) /* 通用寄存器 */
#define TEE_CTL_RB_TEE_GP_REG2_REG               (TEE_CTL_RB_BASE + 0x18) /* 通用寄存器 */
#define TEE_CTL_RB_TEE_GP_REG3_REG               (TEE_CTL_RB_BASE + 0x1C) /* 通用寄存器 */
#define TEE_CTL_RB_SOFT_SOC_TEE_ENABLE_REG       (TEE_CTL_RB_BASE + 0x20) /* 软控制TEE */
#define TEE_CTL_RB_TCM_APC_EN_REG                (TEE_CTL_RB_BASE + 0x30) /* TMC APC使能信号 */
#define TEE_CTL_RB_ITCM_APC_ADDR_REG             (TEE_CTL_RB_BASE + 0x34)
#define TEE_CTL_RB_DTCM_APC_ADDR_REG             (TEE_CTL_RB_BASE + 0x38)
#define TEE_CTL_RB_ITCM_APC_ST_WR_L_REG          (TEE_CTL_RB_BASE + 0x3C) /* ITCM APC出错访问 */
#define TEE_CTL_RB_ITCM_APC_ST_WR_H_REG          (TEE_CTL_RB_BASE + 0x40) /* ITCM APC出错访问 */
#define TEE_CTL_RB_ITCM_APC_ST_RD_L_REG          (TEE_CTL_RB_BASE + 0x44) /* ITCM APC出错访问 */
#define TEE_CTL_RB_ITCM_APC_ST_RD_H_REG          (TEE_CTL_RB_BASE + 0x48) /* ITCM APC出错访问 */
#define TEE_CTL_RB_DTCM_APC_ST_WR_L_REG          (TEE_CTL_RB_BASE + 0x4C) /* DTCM APC出错访问 */
#define TEE_CTL_RB_DTCM_APC_ST_WR_H_REG          (TEE_CTL_RB_BASE + 0x50) /* DTCM APC出错访问 */
#define TEE_CTL_RB_DTCM_APC_ST_RD_L_REG          (TEE_CTL_RB_BASE + 0x54) /* DTCM APC出错访问 */
#define TEE_CTL_RB_DTCM_APC_ST_RD_H_REG          (TEE_CTL_RB_BASE + 0x58) /* DTCM APC出错访问 */
#define TEE_CTL_RB_TZPC_PROT_CFG_REG             (TEE_CTL_RB_BASE + 0x5C) /* 外设权限控制 */
#define TEE_CTL_RB_START_PC_LOCK_CFG_REG         (TEE_CTL_RB_BASE + 0x60) /* START_PC 配置 */
#define TEE_CTL_RB_TEE_START_PC_H_REG            (TEE_CTL_RB_BASE + 0x64) /* TEE_START_PC_H配置 */
#define TEE_CTL_RB_TEE_START_PC_L_REG            (TEE_CTL_RB_BASE + 0x68) /* TEE_START_PC_L配置 */
#define TEE_CTL_RB_CFG_DMA_PROT_REG              (TEE_CTL_RB_BASE + 0x70) /* DMA prot安全标志位配置 */
#define TEE_CTL_RB_ROM_HIDE_ERR_ADDR_REG         (TEE_CTL_RB_BASE + 0x7C) /* ROM_HIDE_ERR_ADDR地址上报 */
#define TEE_CTL_RB_CFG_DEBUG_INTF_EN_REG         (TEE_CTL_RB_BASE + 0x80) /* 参与SSI/ JTAG/ ROM_HIDE等逻辑 */
#define TEE_CTL_RB_TEE_DEEPSLEEP_VOTE_REG        (TEE_CTL_RB_BASE + 0x84) /* TEE侧MCPU深睡投票 */
#define TEE_CTL_RB_CFG_TEE_TIMER_REG             (TEE_CTL_RB_BASE + 0x88) /* TEE_TIMER配置 */
#define TEE_CTL_RB_CFG_TEE_TIMER_H_REG           (TEE_CTL_RB_BASE + 0x8C) /* TEE_TIMER水线高16bit配置 */
#define TEE_CTL_RB_CFG_TEE_TIMER_L_REG           (TEE_CTL_RB_BASE + 0x90) /* TEE_TIMER水线低16bit配置 */
#define TEE_CTL_RB_TEE_TIMER_CURRENT_VALUE_H_REG (TEE_CTL_RB_BASE + 0x94) /* TEE_TIMER_CURRENT_VALUE当前记数值 */
#define TEE_CTL_RB_TEE_TIMER_CURRENT_VALUE_L_REG (TEE_CTL_RB_BASE + 0x98) /* TEE_TIMER_CURRENT_VALUE当前记数值 */
#define TEE_CTL_RB_QSPI_DIAG_EN_REG              (TEE_CTL_RB_BASE + 0x9C) /* QSPI_DIAG使能，TEE侧管控 */

#endif // __3322_TEE_CTL_RB_REG_OFFSET_H__
