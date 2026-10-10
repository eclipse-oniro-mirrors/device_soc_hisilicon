/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File name     :  3322_bcpu_trace_ctl_rb_reg_offset.h
 * Project line  :  Platform And Key Technologies Development
 * Department    :  CAD Development Department
 * Author        :  l00444884
 * Version       :  1.0
 * Date          :  2025/10/09
 * Description   :  The description of xxx project
 * Others        :  Generated automatically by nManager V5.1
 * History       :  l00444884 2025/03/05 17:25:13 Create file
 */

#ifndef __3322_BCPU_TRACE_CTL_RB_REG_OFFSET_H__
#define __3322_BCPU_TRACE_CTL_RB_REG_OFFSET_H__

/* BCPU_TRACE_CTL_RB Base address of Module's Register */
#define BCPU_TRACE_CTL_RB_BASE                       (0x5900C000)

/******************************************************************************/
/*                      BCPU_TRACE_CTL_RB Registers' Definitions                            */
/******************************************************************************/

#define BCPU_TRACE_CTL_RB_BCPU_TRACE_CTL_ID_REG       (BCPU_TRACE_CTL_RB_BASE + 0x0)   /* BCPU TRACE CTL ID寄存器 */
#define BCPU_TRACE_CTL_RB_BCPU_TRACE_GP_REG0_REG      (BCPU_TRACE_CTL_RB_BASE + 0x10)  /* 通用寄存器 */
#define BCPU_TRACE_CTL_RB_BCPU_TRACE_GP_REG1_REG      (BCPU_TRACE_CTL_RB_BASE + 0x14)  /* 通用寄存器 */
#define BCPU_TRACE_CTL_RB_BCPU_TRACE_GP_REG2_REG      (BCPU_TRACE_CTL_RB_BASE + 0x18)  /* 通用寄存器 */
#define BCPU_TRACE_CTL_RB_BCPU_TRACE_GP_REG3_REG      (BCPU_TRACE_CTL_RB_BASE + 0x1C)  /* 通用寄存器 */
#define BCPU_TRACE_CTL_RB_CFG_TRACE_SAVE_SEL_REG      (BCPU_TRACE_CTL_RB_BASE + 0x100) /* 维测功能配置 */
#define BCPU_TRACE_CTL_RB_CFG_CPU_TRACE_REG           (BCPU_TRACE_CTL_RB_BASE + 0x104) /* CPU TRACE功能配置 */
#define BCPU_TRACE_CTL_RB_CFG_MONITOR_CLOCK_REG       (BCPU_TRACE_CTL_RB_BASE + 0x108) /* 数采时钟配置 */
#define BCPU_TRACE_CTL_RB_CFG_SAMPLE_MODE_REG         (BCPU_TRACE_CTL_RB_BASE + 0x200) /* 数据采集模式配置 */
#define BCPU_TRACE_CTL_RB_CFG_SAMPLE_LENGTH_L_REG     (BCPU_TRACE_CTL_RB_BASE + 0x204) /* 数据采集长度配置 */
#define BCPU_TRACE_CTL_RB_CFG_SAMPLE_LENGTH_H_REG     (BCPU_TRACE_CTL_RB_BASE + 0x208) /* 数据采集长度配置 */
#define BCPU_TRACE_CTL_RB_CFG_SAMPLE_START_ADDR_L_REG (BCPU_TRACE_CTL_RB_BASE + 0x20C) /* 数据采集起始地址配置 */
#define BCPU_TRACE_CTL_RB_CFG_SAMPLE_START_ADDR_H_REG (BCPU_TRACE_CTL_RB_BASE + 0x210) /* 数据采集起始地址配置 */
#define BCPU_TRACE_CTL_RB_CFG_SAMPLE_END_ADDR_L_REG   (BCPU_TRACE_CTL_RB_BASE + 0x214) /* 数据采集结束地址配置 */
#define BCPU_TRACE_CTL_RB_CFG_SAMPLE_END_ADDR_H_REG   (BCPU_TRACE_CTL_RB_BASE + 0x218) /* 数据采集结束地址配置 */
#define BCPU_TRACE_CTL_RB_SAMPLE_DONE_ADDR_L_REG      (BCPU_TRACE_CTL_RB_BASE + 0x21C) /* 数采完成上报地址 */
#define BCPU_TRACE_CTL_RB_SAMPLE_DONE_ADDR_H_REG      (BCPU_TRACE_CTL_RB_BASE + 0x220) /* 数采完成上报地址 */
#define BCPU_TRACE_CTL_RB_SAMPLE_DONE_REG             (BCPU_TRACE_CTL_RB_BASE + 0x224) /* 数据完成指示 */
#define BCPU_TRACE_CTL_RB_BCPU_LOAD_DIAG_REG          (BCPU_TRACE_CTL_RB_BASE + 0x300) /* BCPU_LOAD */
#define BCPU_TRACE_CTL_RB_BCPU_PC_L_DIAG_REG          (BCPU_TRACE_CTL_RB_BASE + 0x304) /* BCPU_PC低16bit */
#define BCPU_TRACE_CTL_RB_BCPU_PC_H_DIAG_REG          (BCPU_TRACE_CTL_RB_BASE + 0x308) /* BCPU_PC高16bit */
#define BCPU_TRACE_CTL_RB_BCPU_LR_L_DIAG_REG          (BCPU_TRACE_CTL_RB_BASE + 0x30C) /* BCPU_LR低16bit */
#define BCPU_TRACE_CTL_RB_BCPU_LR_H_DIAG_REG          (BCPU_TRACE_CTL_RB_BASE + 0x310) /* BCPU_LR高16bit */
#define BCPU_TRACE_CTL_RB_MONITOR_FIFO_STATUS_REG     (BCPU_TRACE_CTL_RB_BASE + 0x400) /* FIFO状态指示 */
#define BCPU_TRACE_CTL_RB_MONITOR_FIFO_OVF_REG        (BCPU_TRACE_CTL_RB_BASE + 0x404) /* FIFO写溢出指示 */

#endif // __3322_BCPU_TRACE_CTL_RB_REG_OFFSET_H__
