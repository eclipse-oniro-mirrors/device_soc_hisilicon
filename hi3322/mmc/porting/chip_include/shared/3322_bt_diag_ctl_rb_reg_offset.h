/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File name     :  3322_bt_diag_ctl_rb_reg_offset.h
 * Project line  :  Platform And Key Technologies Development
 * Department    :  CAD Development Department
 * Author        :  l00444884
 * Version       :  1.0
 * Date          :  2025/10/09
 * Description   :  The description of xxx project
 * Others        :  Generated automatically by nManager V5.1
 * History       :  l00444884 2025/03/05 17:25:13 Create file
 */

#ifndef __3322_BT_DIAG_CTL_RB_REG_OFFSET_H__
#define __3322_BT_DIAG_CTL_RB_REG_OFFSET_H__

/* BT_DIAG_CTL_RB Base address of Module's Register */
#define BT_DIAG_CTL_RB_BASE                       (0x59008000)

/******************************************************************************/
/*                      BT_DIAG_CTL_RB Registers' Definitions                            */
/******************************************************************************/

#define BT_DIAG_CTL_RB_BT_DIAG_CTL_ID_REG                  (BT_DIAG_CTL_RB_BASE + 0x0)   /* BT DIAG CTL ID寄存器 */
#define BT_DIAG_CTL_RB_BT_DIAG_GP_REG0_REG                 (BT_DIAG_CTL_RB_BASE + 0x10)  /* 通用寄存器 */
#define BT_DIAG_CTL_RB_BT_DIAG_GP_REG1_REG                 (BT_DIAG_CTL_RB_BASE + 0x14)  /* 通用寄存器 */
#define BT_DIAG_CTL_RB_BT_DIAG_GP_REG2_REG                 (BT_DIAG_CTL_RB_BASE + 0x18)  /* 通用寄存器 */
#define BT_DIAG_CTL_RB_BT_DIAG_GP_REG3_REG                 (BT_DIAG_CTL_RB_BASE + 0x1C)  /* 通用寄存器 */
#define BT_DIAG_CTL_RB_CFG_BT_SAMPLE_REORDER_REG           (BT_DIAG_CTL_RB_BASE + 0x100) /* 数采本地memory顺序配置 */
#define BT_DIAG_CTL_RB_CFG_BT_DIAG_SAMPLE_SEL_REG          (BT_DIAG_CTL_RB_BASE + 0x200) /* 数据采集源选择 */
#define BT_DIAG_CTL_RB_CFG_BT_DIAG_SAMPLE_MODE_REG         (BT_DIAG_CTL_RB_BASE + 0x204) /* 数据采集模式配置 */
#define BT_DIAG_CTL_RB_CFG_BT_DIAG_SAMPLE_LENGTH_L_REG     (BT_DIAG_CTL_RB_BASE + 0x208) /* 数据采集长度配置 */
#define BT_DIAG_CTL_RB_CFG_BT_DIAG_SAMPLE_START_ADDR_L_REG (BT_DIAG_CTL_RB_BASE + 0x210) /* 数据采集起始地址配置 */
#define BT_DIAG_CTL_RB_CFG_BT_DIAG_SAMPLE_START_ADDR_H_REG (BT_DIAG_CTL_RB_BASE + 0x214) /* 数据采集起始地址配置 */
#define BT_DIAG_CTL_RB_BT_DIAG_SAMPLE_DONE_ADDR_L_REG      (BT_DIAG_CTL_RB_BASE + 0x220) /* 数采完成上报地址 */
#define BT_DIAG_CTL_RB_BT_DIAG_SAMPLE_DONE_ADDR_H_REG      (BT_DIAG_CTL_RB_BASE + 0x224) /* 数采完成上报地址 */
#define BT_DIAG_CTL_RB_BT_DIAG_SAMPLE_DONE_REG             (BT_DIAG_CTL_RB_BASE + 0x228) /* 数据完成指示 */
#define BT_DIAG_CTL_RB_CFG_TEST_GEN_SEL_REG                (BT_DIAG_CTL_RB_BASE + 0x300) /* 数据生成选择 */
#define BT_DIAG_CTL_RB_CFG_TEST_GEN_MODE_REG               (BT_DIAG_CTL_RB_BASE + 0x304) /* 数据生成模式配置 */
#define BT_DIAG_CTL_RB_CFG_TEST_GEN_LENGTH_L_REG           (BT_DIAG_CTL_RB_BASE + 0x308) /* 数据生成长度配置 */
#define BT_DIAG_CTL_RB_CFG_TEST_GEN_START_ADDR_L_REG       (BT_DIAG_CTL_RB_BASE + 0x310) /* 数据生成起始地址配置 */
#define BT_DIAG_CTL_RB_CFG_TEST_GEN_START_ADDR_H_REG       (BT_DIAG_CTL_RB_BASE + 0x314) /* 数据生成起始地址配置 */
#define BT_DIAG_CTL_RB_GEN_DONE_ADDR_L_REG                 (BT_DIAG_CTL_RB_BASE + 0x32C) /* 数据生成完成上报地址 */
#define BT_DIAG_CTL_RB_GEN_DONE_ADDR_H_REG                 (BT_DIAG_CTL_RB_BASE + 0x330) /* 数据生成完成上报地址 */
#define BT_DIAG_CTL_RB_GEN_DONE_REG                        (BT_DIAG_CTL_RB_BASE + 0x334) /* 数据生成完成指示 */
#define BT_DIAG_CTL_RB_MONITOR_FIFO_EMPTY_STATUS_REG       (BT_DIAG_CTL_RB_BASE + 0x400) /* FIFO状态指示 */
#define BT_DIAG_CTL_RB_SAMPLE_MONITOR_FIFO_OVF_REG         (BT_DIAG_CTL_RB_BASE + 0x404) /* FIFO写溢出指示 */
#define BT_DIAG_CTL_RB_GEN_MONITOR_FIFO_UNF_REG            (BT_DIAG_CTL_RB_BASE + 0x408) /* FIFO读溢出指示 */
#define BT_DIAG_CTL_RB_AHBBUS_ERR_MONITOR_BCPU_I_REG       (BT_DIAG_CTL_RB_BASE + 0x420) /* BCPU AHB Master监测配置寄存器 */
#define BT_DIAG_CTL_RB_BDIAG_AHBBUS_ERR_HADDR_I_H_REG      (BT_DIAG_CTL_RB_BASE + 0x424) /* 出现错误时haddr的高16bit */
#define BT_DIAG_CTL_RB_BDIAG_AHBBUS_ERR_HADDR_I_L_REG      (BT_DIAG_CTL_RB_BASE + 0x428) /* 出现错误时haddr的低16bit */
#define BT_DIAG_CTL_RB_BDIAG_AHBBUS_ERR_INFO_I_REG         (BT_DIAG_CTL_RB_BASE + 0x42C) /* 出现错误时的hwrite，hsize与hburst */
#define BT_DIAG_CTL_RB_AHBBUS_ERR_MONITOR_BCPU_D_REG       (BT_DIAG_CTL_RB_BASE + 0x430) /* BCPU AHB Master监测配置寄存器 */
#define BT_DIAG_CTL_RB_BDIAG_AHBBUS_ERR_HADDR_D_H_REG      (BT_DIAG_CTL_RB_BASE + 0x434) /* 出现错误时haddr的高16bit */
#define BT_DIAG_CTL_RB_BDIAG_AHBBUS_ERR_HADDR_D_L_REG      (BT_DIAG_CTL_RB_BASE + 0x438) /* 出现错误时haddr的低16bit */
#define BT_DIAG_CTL_RB_BDIAG_AHBBUS_ERR_INFO_D_REG         (BT_DIAG_CTL_RB_BASE + 0x43C) /* 出现错误时的hwrite，hsize与hburst */
#define BT_DIAG_CTL_RB_AHBBUS_ERR_MONITOR_BCPU_S_REG       (BT_DIAG_CTL_RB_BASE + 0x440) /* BCPU AHB Master监测配置寄存器 */
#define BT_DIAG_CTL_RB_BDIAG_AHBBUS_ERR_HADDR_S_H_REG      (BT_DIAG_CTL_RB_BASE + 0x444) /* 出现错误时haddr的高16bit */
#define BT_DIAG_CTL_RB_BDIAG_AHBBUS_ERR_HADDR_S_L_REG      (BT_DIAG_CTL_RB_BASE + 0x448) /* 出现错误时haddr的低16bit */
#define BT_DIAG_CTL_RB_BDIAG_AHBBUS_ERR_INFO_S_REG         (BT_DIAG_CTL_RB_BASE + 0x44C) /* 出现错误时的hwrite，hsize与hburst */

#endif // __3322_BT_DIAG_CTL_RB_REG_OFFSET_H__
