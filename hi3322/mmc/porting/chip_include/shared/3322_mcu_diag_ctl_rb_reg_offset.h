/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File name     :  3322_mcu_diag_ctl_rb_reg_offset.h
 * Project line  :  Platform And Key Technologies Development
 * Department    :  CAD Development Department
 * Author        :  l00444884
 * Version       :  1.0
 * Date          :  2025/10/09
 * Description   :  The description of xxx project
 * Others        :  Generated automatically by nManager V5.1
 * History       :  l00444884 2025/03/05 17:25:13 Create file
 */

#ifndef __3322_MCU_DIAG_CTL_RB_REG_OFFSET_H__
#define __3322_MCU_DIAG_CTL_RB_REG_OFFSET_H__

/* MCU_DIAG_CTL_RB Base address of Module's Register */
#define MCU_DIAG_CTL_RB_BASE                       (0x52004000)

/******************************************************************************/
/*                      MCU_DIAG_CTL_RB Registers' Definitions                            */
/******************************************************************************/

#define MCU_DIAG_CTL_RB_MCU_DIAG_CTL_ID_REG                  (MCU_DIAG_CTL_RB_BASE + 0x0)   /* MCU DIAG CTL ID寄存器 */
#define MCU_DIAG_CTL_RB_MCU_DIAG_GP_REG0_REG                 (MCU_DIAG_CTL_RB_BASE + 0x10)  /* 通用寄存器 */
#define MCU_DIAG_CTL_RB_MCU_DIAG_GP_REG1_REG                 (MCU_DIAG_CTL_RB_BASE + 0x14)  /* 通用寄存器 */
#define MCU_DIAG_CTL_RB_MCU_DIAG_GP_REG2_REG                 (MCU_DIAG_CTL_RB_BASE + 0x18)  /* 通用寄存器 */
#define MCU_DIAG_CTL_RB_MCU_DIAG_GP_REG3_REG                 (MCU_DIAG_CTL_RB_BASE + 0x1C)  /* 通用寄存器 */
#define MCU_DIAG_CTL_RB_CFG_MONITOR_SEL_REG                  (MCU_DIAG_CTL_RB_BASE + 0x100) /* 维测功能配置 */
#define MCU_DIAG_CTL_RB_CFG_MCU_DIAG_TRACE_SAVE_SEL_REG      (MCU_DIAG_CTL_RB_BASE + 0x104) /* TRACE存储位置配置 */
#define MCU_DIAG_CTL_RB_CFG_MCU_DIAG_CPU_TRACE_REG           (MCU_DIAG_CTL_RB_BASE + 0x108) /* CPU TRACE功能配置 */
#define MCU_DIAG_CTL_RB_CFG_MCU_DIAG_MONITOR_CLOCK_REG       (MCU_DIAG_CTL_RB_BASE + 0x10C) /* 数采时钟配置 */
#define MCU_DIAG_CTL_RB_CFG_MCU_DIAG_SAMPLE_SEL_REG          (MCU_DIAG_CTL_RB_BASE + 0x200) /* 数据采集源选择 */
#define MCU_DIAG_CTL_RB_CFG_MCU_DIAG_SAMPLE_MODE_REG         (MCU_DIAG_CTL_RB_BASE + 0x204) /* 数据采集模式配置 */
#define MCU_DIAG_CTL_RB_CFG_MCU_DIAG_SAMPLE_LENGTH_L_REG     (MCU_DIAG_CTL_RB_BASE + 0x208) /* 数据采集长度配置 */
#define MCU_DIAG_CTL_RB_CFG_MCU_DIAG_SAMPLE_LENGTH_H_REG     (MCU_DIAG_CTL_RB_BASE + 0x20C) /* 数据采集长度配置 */
#define MCU_DIAG_CTL_RB_CFG_MCU_DIAG_SAMPLE_START_ADDR_L_REG (MCU_DIAG_CTL_RB_BASE + 0x210) /* 数据采集起始地址配置 */
#define MCU_DIAG_CTL_RB_CFG_MCU_DIAG_SAMPLE_START_ADDR_H_REG (MCU_DIAG_CTL_RB_BASE + 0x214) /* 数据采集起始地址配置 */
#define MCU_DIAG_CTL_RB_CFG_MCU_DIAG_SAMPLE_END_ADDR_L_REG   (MCU_DIAG_CTL_RB_BASE + 0x218) /* 数据采集结束地址配置 */
#define MCU_DIAG_CTL_RB_CFG_MCU_DIAG_SAMPLE_END_ADDR_H_REG   (MCU_DIAG_CTL_RB_BASE + 0x21C) /* 数据采集结束地址配置 */
#define MCU_DIAG_CTL_RB_MCU_DIAG_SAMPLE_DONE_ADDR_L_REG      (MCU_DIAG_CTL_RB_BASE + 0x220) /* 数采完成上报地址 */
#define MCU_DIAG_CTL_RB_MCU_DIAG_SAMPLE_DONE_ADDR_H_REG      (MCU_DIAG_CTL_RB_BASE + 0x224) /* 数采完成上报地址 */
#define MCU_DIAG_CTL_RB_MCU_DIAG_SAMPLE_DONE_REG             (MCU_DIAG_CTL_RB_BASE + 0x228) /* 数据完成指示 */
#define MCU_DIAG_CTL_RB_CFG_AUX_ADC_SAMPLE_PERIOD_REG        (MCU_DIAG_CTL_RB_BASE + 0x22C) /* AUX ADC周期配置 */
#define MCU_DIAG_CTL_RB_CFG_AUX_CAPS_SAMPLE_LENGTH_L_REG     (MCU_DIAG_CTL_RB_BASE + 0x300) /* 数据采集长度配置 */
#define MCU_DIAG_CTL_RB_CFG_AUX_CAPS_SAMPLE_LENGTH_H_REG     (MCU_DIAG_CTL_RB_BASE + 0x304) /* 数据采集长度配置 */
#define MCU_DIAG_CTL_RB_CFG_AUX_CAPS_SAMPLE_START_ADDR_L_REG (MCU_DIAG_CTL_RB_BASE + 0x308) /* 数据采集起始地址配置 */
#define MCU_DIAG_CTL_RB_CFG_AUX_CAPS_SAMPLE_START_ADDR_H_REG (MCU_DIAG_CTL_RB_BASE + 0x30C) /* 数据采集起始地址配置 */
#define MCU_DIAG_CTL_RB_CFG_AUX_CAPS_SAMPLE_END_ADDR_L_REG   (MCU_DIAG_CTL_RB_BASE + 0x310) /* 数据采集结束地址配置 */
#define MCU_DIAG_CTL_RB_CFG_AUX_CAPS_SAMPLE_END_ADDR_H_REG   (MCU_DIAG_CTL_RB_BASE + 0x314) /* 数据采集结束地址配置 */
#define MCU_DIAG_CTL_RB_AUX_CAPS_SAMPLE_DONE_ADDR_L_REG      (MCU_DIAG_CTL_RB_BASE + 0x318) /* 数采完成上报地址 */
#define MCU_DIAG_CTL_RB_AUX_CAPS_SAMPLE_DONE_ADDR_H_REG      (MCU_DIAG_CTL_RB_BASE + 0x31C) /* 数采完成上报地址 */
#define MCU_DIAG_CTL_RB_AUX_CAPS_SAMPLE_DONE_REG             (MCU_DIAG_CTL_RB_BASE + 0x320) /* 数据完成指示 */
#define MCU_DIAG_CTL_RB_CFG_DIAG_MUX_REG                     (MCU_DIAG_CTL_RB_BASE + 0x400) /* DIAG选择寄存器 */
#define MCU_DIAG_CTL_RB_CFG_PIN_SEL_0_3_REG                  (MCU_DIAG_CTL_RB_BASE + 0x404) /* DIAG PIN选择寄存器 */
#define MCU_DIAG_CTL_RB_CFG_PIN_SEL_4_7_REG                  (MCU_DIAG_CTL_RB_BASE + 0x408) /* DIAG PIN选择寄存器 */
#define MCU_DIAG_CTL_RB_CFG_PIN_SEL_8_11_REG                 (MCU_DIAG_CTL_RB_BASE + 0x40C) /* DIAG PIN选择寄存器 */
#define MCU_DIAG_CTL_RB_CFG_PIN_SEL_12_15_REG                (MCU_DIAG_CTL_RB_BASE + 0x410) /* DIAG PIN选择寄存器 */
#define MCU_DIAG_CTL_RB_CFG_CLOCK_TEST_SEL_REG               (MCU_DIAG_CTL_RB_BASE + 0x414) /* 观测时钟选择配置寄存器 */
#define MCU_DIAG_CTL_RB_CFG_CLOCK_TEST_DIV_REG               (MCU_DIAG_CTL_RB_BASE + 0x418) /* 观测时钟分频配置寄存器 */
#define MCU_DIAG_CTL_RB_CFG_CLOCK_TEST_GATE_EN_REG           (MCU_DIAG_CTL_RB_BASE + 0x41C) /* 观测时钟输出使能配置寄存器 */
#define MCU_DIAG_CTL_RB_MCPU_LOAD_DIAG_REG                   (MCU_DIAG_CTL_RB_BASE + 0x500) /* MCPU_LOAD */
#define MCU_DIAG_CTL_RB_MCPU_PC_L_DIAG_REG                   (MCU_DIAG_CTL_RB_BASE + 0x504) /* MCPU_PC低16bit */
#define MCU_DIAG_CTL_RB_MCPU_PC_H_DIAG_REG                   (MCU_DIAG_CTL_RB_BASE + 0x508) /* MCPU_PC高16bit */
#define MCU_DIAG_CTL_RB_MCPU_LR_L_DIAG_REG                   (MCU_DIAG_CTL_RB_BASE + 0x50C) /* MCPU_LR低16bit */
#define MCU_DIAG_CTL_RB_MCPU_LR_H_DIAG_REG                   (MCU_DIAG_CTL_RB_BASE + 0x510) /* MCPU_LR高16bit */
#define MCU_DIAG_CTL_RB_MCPU_SP_L_DIAG_REG                   (MCU_DIAG_CTL_RB_BASE + 0x514) /* MCPU_SP低16bit */
#define MCU_DIAG_CTL_RB_MCPU_SP_H_DIAG_REG                   (MCU_DIAG_CTL_RB_BASE + 0x518) /* MCPU_SP高16bit */
#define MCU_DIAG_CTL_RB_MCU_DIAG_MONITOR_FIFO_STATUS_REG     (MCU_DIAG_CTL_RB_BASE + 0x600) /* FIFO状态指示 */
#define MCU_DIAG_CTL_RB_MCU_DIAG_MONITOR_FIFO_OVF_REG        (MCU_DIAG_CTL_RB_BASE + 0x604) /* FIFO写溢出指示 */
#define MCU_DIAG_CTL_RB_MCU_DIAG_MONITOR_FIFO_UNF_REG        (MCU_DIAG_CTL_RB_BASE + 0x608) /* FIFO读溢出指示 */
#define MCU_DIAG_CTL_RB_MCU_DIAG_MON2LMI_WRITE_DONE_REG      (MCU_DIAG_CTL_RB_BASE + 0x60C) /* mon2lmi模块的写结束信号 */
#define MCU_DIAG_CTL_RB_MCU_DIAG_MON2LMI_WFIFO_CNT_REG       (MCU_DIAG_CTL_RB_BASE + 0x610) /* mon2lmi模块写fifo数据个数 */
#define MCU_DIAG_CTL_RB_AHBBUS_ERR_MONITOR_MCPU_REG          (MCU_DIAG_CTL_RB_BASE + 0x620) /* MCPU AHB Master监测配置寄存器 */
#define MCU_DIAG_CTL_RB_AHBBUS_ERR_HADDR_H_REG               (MCU_DIAG_CTL_RB_BASE + 0x624) /* 出现错误时haddr的高16bit */
#define MCU_DIAG_CTL_RB_AHBBUS_ERR_HADDR_L_REG               (MCU_DIAG_CTL_RB_BASE + 0x628) /* 出现错误时haddr的低16bit */
#define MCU_DIAG_CTL_RB_AHBBUS_ERR_INFO_REG                  (MCU_DIAG_CTL_RB_BASE + 0x62C) /* 出现错误时的hwrite，hsize与hburst */
#define MCU_DIAG_CTL_RB_MCPU_CLOCK_TEST_EN_REG               (MCU_DIAG_CTL_RB_BASE + 0x630) /* mcpu_clock_test */

#endif // __3322_MCU_DIAG_CTL_RB_REG_OFFSET_H__
