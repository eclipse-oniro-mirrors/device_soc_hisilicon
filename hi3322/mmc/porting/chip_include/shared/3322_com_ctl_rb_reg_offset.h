/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File name     :  3322_com_ctl_rb_reg_offset.h
 * Project line  :  Platform And Key Technologies Development
 * Department    :  CAD Development Department
 * Author        :  l00444884
 * Version       :  1.0
 * Date          :  2025/10/09
 * Description   :  The description of xxx project
 * Others        :  Generated automatically by nManager V5.1
 * History       :  l00444884 2025/03/05 17:25:13 Create file
 */

#ifndef __3322_COM_CTL_RB_REG_OFFSET_H__
#define __3322_COM_CTL_RB_REG_OFFSET_H__

/* COM_CTL_RB Base address of Module's Register */
#define COM_CTL_RB_BASE                       (0x55000000)

/******************************************************************************/
/*                      COM_CTL_RB Registers' Definitions                            */
/******************************************************************************/

#define COM_CTL_RB_COM_CTL_ID_REG                  (COM_CTL_RB_BASE + 0x0)   /* COM CTL ID寄存器 */
#define COM_CTL_RB_COM_GP_REG0_REG                 (COM_CTL_RB_BASE + 0x10)  /* 通用寄存器 */
#define COM_CTL_RB_COM_GP_REG1_REG                 (COM_CTL_RB_BASE + 0x14)  /* 通用寄存器 */
#define COM_CTL_RB_COM_GP_REG2_REG                 (COM_CTL_RB_BASE + 0x18)  /* 通用寄存器 */
#define COM_CTL_RB_COM_GP_REG3_REG                 (COM_CTL_RB_BASE + 0x1C)  /* 通用寄存器 */
#define COM_CTL_RB_CODEC_DIG_RW_REG_L_REG          (COM_CTL_RB_BASE + 0x150) /* DPS_SUB_CFG_L */
#define COM_CTL_RB_CODEC_DIG_RW_REG_H_REG          (COM_CTL_RB_BASE + 0x154) /* DSP_SUB_CFG_H */
#define COM_CTL_RB_CODEC_DIG_RO_REG0_L_REG         (COM_CTL_RB_BASE + 0x158) /* 状态查询寄存器 */
#define COM_CTL_RB_CODEC_DIG_RO_REG0_H_REG         (COM_CTL_RB_BASE + 0x15C) /* 状态查询寄存器 */
#define COM_CTL_RB_CODEC_DIG_RO_REG1_L_REG         (COM_CTL_RB_BASE + 0x160) /* 状态查询寄存器 */
#define COM_CTL_RB_CODEC_DIG_RO_REG1_H_REG         (COM_CTL_RB_BASE + 0x164) /* 状态查询寄存器 */
#define COM_CTL_RB_CODEC_ROM_CFG_REG               (COM_CTL_RB_BASE + 0x170) /* ROM配置寄存器 */
#define COM_CTL_RB_CODEC_MEM_1W2R_CFG_REG          (COM_CTL_RB_BASE + 0x178) /* 1W2R RAM配置寄存器 */
#define COM_CTL_RB_CODEC_RSVD_EC_H_REG             (COM_CTL_RB_BASE + 0x180) /* audio eco rsvd */
#define COM_CTL_RB_CODEC_PWR_CFG0_REG              (COM_CTL_RB_BASE + 0x184) /* RAM配置寄存器 */
#define COM_CTL_RB_CODEC_PWR_CFG1_REG              (COM_CTL_RB_BASE + 0x188) /* RAM配置寄存器 */
#define COM_CTL_RB_CODEC_JTAG_MODE_REG             (COM_CTL_RB_BASE + 0x18C) /* DJTAG配置寄存器 */
#define COM_CTL_RB_DEAR_DMA_POS_CNT_L_REG          (COM_CTL_RB_BASE + 0x190) /* 状态查询寄存器 */
#define COM_CTL_RB_DEAR_DMA_POS_CNT_H_REG          (COM_CTL_RB_BASE + 0x194) /* 状态查询寄存器 */
#define COM_CTL_RB_DEAR_TOG_POS_CNT_L_REG          (COM_CTL_RB_BASE + 0x198) /* 状态查询寄存器 */
#define COM_CTL_RB_DEAR_TOG_POS_CNT_H_REG          (COM_CTL_RB_BASE + 0x19C) /* 状态查询寄存器 */
#define COM_CTL_RB_CODEC_DIG_RW_REG1_L_REG         (COM_CTL_RB_BASE + 0x1A0) /* CODEC控制 */
#define COM_CTL_RB_CODEC_DIG_RW_REG1_H_REG         (COM_CTL_RB_BASE + 0x1A4) /* CODEC控制 */
#define COM_CTL_RB_CODEC_MEM_CTRL_REG              (COM_CTL_RB_BASE + 0x1B0) /* CODEC RAM 控制 */
#define COM_CTL_RB_CODEC_DUAL_MEM_CTRL_REG         (COM_CTL_RB_BASE + 0x1B4) /* CODEC DUAL RAM 控制 */
#define COM_CTL_RB_AUDIO_BRG_LP_CFG_REG            (COM_CTL_RB_BASE + 0x1B8)
#define COM_CTL_RB_CODEC_RFS_MEM_CTRL_REG          (COM_CTL_RB_BASE + 0x1BC) /* CODEC RAM 控制 */
#define COM_CTL_RB_COM_MEM_TOMD_H_REG              (COM_CTL_RB_BASE + 0x404) /* RAM调速寄存器 */
#define COM_CTL_RB_COM_MEM_LS_REG                  (COM_CTL_RB_BASE + 0x408) /* RAM调速寄存器 */
#define COM_CTL_RB_CLK_FORCE_CFG_REG               (COM_CTL_RB_BASE + 0x414)
#define COM_CTL_RB_COM_BUS_CFG0_REG                (COM_CTL_RB_BASE + 0x418)
#define COM_CTL_RB_COM_BUS_CFG1_REG                (COM_CTL_RB_BASE + 0x41C)
#define COM_CTL_RB_COM_BUS_CFG2_REG                (COM_CTL_RB_BASE + 0x420)
#define COM_CTL_RB_COM_BUS_CFG3_REG                (COM_CTL_RB_BASE + 0x424)
#define COM_CTL_RB_COM_BUS_CFG4_REG                (COM_CTL_RB_BASE + 0x428)
#define COM_CTL_RB_MEM_AHB_BUS_LP_TIME_REG         (COM_CTL_RB_BASE + 0x468)
#define COM_CTL_RB_C2D_HSEL_SEL_REG                (COM_CTL_RB_BASE + 0x46C)
#define COM_CTL_RB_AON_BUS_LP_CFG_REG              (COM_CTL_RB_BASE + 0x480)
#define COM_CTL_RB_SDIO_M_BUS_LP_STS_REG           (COM_CTL_RB_BASE + 0x4B8)
#define COM_CTL_RB_SDIO_M_BUS_LP_CLR_REG           (COM_CTL_RB_BASE + 0x4BC)
#define COM_CTL_RB_SDIO_CFG_BUS_LP_CFG_REG         (COM_CTL_RB_BASE + 0x4C0)
#define COM_CTL_RB_SDIO_CFG_BUS_LP_TIME_REG        (COM_CTL_RB_BASE + 0x4C4)
#define COM_CTL_RB_SDIO_CFG_BUS_LP_STS_REG         (COM_CTL_RB_BASE + 0x4C8)
#define COM_CTL_RB_SDIO_CFG_BUS_LP_CLR_REG         (COM_CTL_RB_BASE + 0x4CC)
#define COM_CTL_RB_DIS_BUS_LP_CFG_REG              (COM_CTL_RB_BASE + 0x4D0)
#define COM_CTL_RB_DIS_BUS_LP_TIME_REG             (COM_CTL_RB_BASE + 0x4D4)
#define COM_CTL_RB_DIS_BUS_LP_STS_REG              (COM_CTL_RB_BASE + 0x4D8)
#define COM_CTL_RB_DIS_BUS_LP_CLR_REG              (COM_CTL_RB_BASE + 0x4DC)
#define COM_CTL_RB_MCU_CFG_BUS_LP_STS_REG          (COM_CTL_RB_BASE + 0x4E8)
#define COM_CTL_RB_MCU_CFG_BUS_LP_CLR_REG          (COM_CTL_RB_BASE + 0x4EC)
#define COM_CTL_RB_AXI_BRG_LP_CFG_REG              (COM_CTL_RB_BASE + 0x4FC)
#define COM_CTL_RB_EH2H_AUTO_CG_CFG_REG            (COM_CTL_RB_BASE + 0x500)
#define COM_CTL_RB_DISPLAY_MEM_CFG_REG             (COM_CTL_RB_BASE + 0x504)
#define COM_CTL_RB_TCXO_CNT_CFG_M_REG              (COM_CTL_RB_BASE + 0x580)
#define COM_CTL_RB_TCXO_VALUE_0_M_REG              (COM_CTL_RB_BASE + 0x584)
#define COM_CTL_RB_TCXO_VALUE_1_M_REG              (COM_CTL_RB_BASE + 0x588)
#define COM_CTL_RB_TCXO_VALUE_2_M_REG              (COM_CTL_RB_BASE + 0x58C)
#define COM_CTL_RB_TCXO_VALUE_3_M_REG              (COM_CTL_RB_BASE + 0x590)
#define COM_CTL_RB_TCXO_CNT_CFG_B_REG              (COM_CTL_RB_BASE + 0x5A0)
#define COM_CTL_RB_TCXO_VALUE_0_B_REG              (COM_CTL_RB_BASE + 0x5A4)
#define COM_CTL_RB_TCXO_VALUE_1_B_REG              (COM_CTL_RB_BASE + 0x5A8)
#define COM_CTL_RB_TCXO_VALUE_2_B_REG              (COM_CTL_RB_BASE + 0x5AC)
#define COM_CTL_RB_TCXO_VALUE_3_B_REG              (COM_CTL_RB_BASE + 0x5B0)
#define COM_CTL_RB_TCXO_CNT_CFG_D_REG              (COM_CTL_RB_BASE + 0x5C0)
#define COM_CTL_RB_TCXO_VALUE_0_D_REG              (COM_CTL_RB_BASE + 0x5C4)
#define COM_CTL_RB_TCXO_VALUE_1_D_REG              (COM_CTL_RB_BASE + 0x5C8)
#define COM_CTL_RB_TCXO_VALUE_2_D_REG              (COM_CTL_RB_BASE + 0x5CC)
#define COM_CTL_RB_TCXO_VALUE_3_D_REG              (COM_CTL_RB_BASE + 0x5D0)
#define COM_CTL_RB_TCXO_CNT_CFG_G_REG              (COM_CTL_RB_BASE + 0x5E0)
#define COM_CTL_RB_TCXO_VALUE_0_G_REG              (COM_CTL_RB_BASE + 0x5E4)
#define COM_CTL_RB_TCXO_VALUE_1_G_REG              (COM_CTL_RB_BASE + 0x5E8)
#define COM_CTL_RB_TCXO_VALUE_2_G_REG              (COM_CTL_RB_BASE + 0x5EC)
#define COM_CTL_RB_TCXO_VALUE_3_G_REG              (COM_CTL_RB_BASE + 0x5F0)
#define COM_CTL_RB_COM_DLL2_REG                    (COM_CTL_RB_BASE + 0x600) /* COM DLL2控制寄存器 */
#define COM_CTL_RB_C_CRG_CLKEN_REG                 (COM_CTL_RB_BASE + 0x604) /* C_CRG_CLKEN */
#define COM_CTL_RB_GLB_CLKEN_REG                   (COM_CTL_RB_BASE + 0x608) /* 时钟使能寄存器 */
#define COM_CTL_RB_MCU_CORE_CR_CH0_REG             (COM_CTL_RB_BASE + 0x60C) /* MCU_CORE CR CH0寄存器 */
#define COM_CTL_RB_MCU_CORE_CR_CH1_REG             (COM_CTL_RB_BASE + 0x610) /* MCU_CORE CR CH1寄存器 */
#define COM_CTL_RB_MCU_PERP_LS_CR_REG              (COM_CTL_RB_BASE + 0x614) /* MCU_PERP LS CR寄存器 */
#define COM_CTL_RB_MCU_PERP_UART_CR_REG            (COM_CTL_RB_BASE + 0x618) /* MCU_PERP UART CR寄存器 */
#define COM_CTL_RB_MCU_PERP_SPI_CR_REG             (COM_CTL_RB_BASE + 0x61C) /* MCU_PERP SPI CR寄存器 */
#define COM_CTL_RB_COM_BUS_CR_CH0_REG              (COM_CTL_RB_BASE + 0x620) /* COM_BUS CR CH0寄存器 */
#define COM_CTL_RB_COM_BUS_CR_CH1_REG              (COM_CTL_RB_BASE + 0x624) /* COM_BUS CR CH1寄存器 */
#define COM_CTL_RB_SDIOM_CR_REG                    (COM_CTL_RB_BASE + 0x628) /* SDIOM CR寄存器 */
#define COM_CTL_RB_NPU_CR_REG                      (COM_CTL_RB_BASE + 0x62C) /* SUC_CORE_CHO CR寄存器 */
#define COM_CTL_RB_SEC_CR_REG                      (COM_CTL_RB_BASE + 0x630) /* SCU_CORE_CH1 CR寄存器 */
#define COM_CTL_RB_XIP_QSPI_CR_REG                 (COM_CTL_RB_BASE + 0x634) /* XIP_QSPI CR寄存器 */
#define COM_CTL_RB_EMMC_CR_REG                     (COM_CTL_RB_BASE + 0x638) /* EMMC CR寄存器 */
#define COM_CTL_RB_HIFI_CR_CH0_REG                 (COM_CTL_RB_BASE + 0x63C) /* HIFI CR CH0寄存器 */
#define COM_CTL_RB_HIFI_CR_CH1_REG                 (COM_CTL_RB_BASE + 0x640) /* HIFI CR CH1寄存器 */
#define COM_CTL_RB_CODEC_CR_CH0_REG                (COM_CTL_RB_BASE + 0x644) /* CODEC CR CH0寄存器 */
#define COM_CTL_RB_CODEC_CR_CH1_REG                (COM_CTL_RB_BASE + 0x648) /* CODEC CR CH1寄存器 */
#define COM_CTL_RB_PAD_CLK_OUT0_CR_REG             (COM_CTL_RB_BASE + 0x64C) /* PAD_CLK_OUT0 CR寄存器 */
#define COM_CTL_RB_PAD_CLK_OUT1_CR_REG             (COM_CTL_RB_BASE + 0x650) /* PAD_CLK_OUT1 CR寄存器 */
#define COM_CTL_RB_CAN_CR_REG                      (COM_CTL_RB_BASE + 0x654) /* CAN CR寄存器 */
#define COM_CTL_RB_XIP_OPI0_CR_REG                 (COM_CTL_RB_BASE + 0x658) /* XIP_OPI0 CR寄存器 */
#define COM_CTL_RB_XIP_OPI1_CR_REG                 (COM_CTL_RB_BASE + 0x65C) /* XIP_OPI1 CR寄存器 */
#define COM_CTL_RB_DPU_CR_REG                      (COM_CTL_RB_BASE + 0x660) /* DPU_CR CR寄存器 */
#define COM_CTL_RB_SCR_USB_BUS_ROUTER_REG          (COM_CTL_RB_BASE + 0x664) /* SCR_USB_BUS_ROUTER寄存器 */
#define COM_CTL_RB_C_CRG_DIV_REG                   (COM_CTL_RB_BASE + 0x668) /* C_CRG_DIV */
#define COM_CTL_RB_COM_AHB_MONITOR_EN_0_REG        (COM_CTL_RB_BASE + 0x820) /* 内存监控开关寄存器 */
#define COM_CTL_RB_COM_ACCESS_MEM_INT_0_REG        (COM_CTL_RB_BASE + 0x824) /* 内存监控一级中断状态查询寄存器 */
#define COM_CTL_RB_COM_ACCESS_MEM_INT_MASK_0_REG   (COM_CTL_RB_BASE + 0x828)
#define COM_CTL_RB_COM_ACCESS_MEM_INT_STS_0_REG    (COM_CTL_RB_BASE + 0x82C)
#define COM_CTL_RB_COM_ACCESS_MEM_INT_CLR_0_REG    (COM_CTL_RB_BASE + 0x830)
#define COM_CTL_RB_COM_REC_ADDR_L_0_REG            (COM_CTL_RB_BASE + 0x834)
#define COM_CTL_RB_COM_REC_ADDR_H_0_REG            (COM_CTL_RB_BASE + 0x838)
#define COM_CTL_RB_COM_REC_ID_0_REG                (COM_CTL_RB_BASE + 0x83C)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR_EN_0_REG   (COM_CTL_RB_BASE + 0x840)
#define COM_CTL_RB_COM_PKT_MONITOR_INT_DLY_0_REG   (COM_CTL_RB_BASE + 0x844)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR0_L_L_0_REG (COM_CTL_RB_BASE + 0x848)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR0_L_H_0_REG (COM_CTL_RB_BASE + 0x84C)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR0_H_L_0_REG (COM_CTL_RB_BASE + 0x850)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR0_H_H_0_REG (COM_CTL_RB_BASE + 0x854)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR1_L_L_0_REG (COM_CTL_RB_BASE + 0x858)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR1_L_H_0_REG (COM_CTL_RB_BASE + 0x85C)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR1_H_L_0_REG (COM_CTL_RB_BASE + 0x860)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR1_H_H_0_REG (COM_CTL_RB_BASE + 0x864)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR2_L_L_0_REG (COM_CTL_RB_BASE + 0x868)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR2_L_H_0_REG (COM_CTL_RB_BASE + 0x86C)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR2_H_L_0_REG (COM_CTL_RB_BASE + 0x870)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR2_H_H_0_REG (COM_CTL_RB_BASE + 0x874)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR3_L_L_0_REG (COM_CTL_RB_BASE + 0x878)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR3_L_H_0_REG (COM_CTL_RB_BASE + 0x87C)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR3_H_L_0_REG (COM_CTL_RB_BASE + 0x880)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR3_H_H_0_REG (COM_CTL_RB_BASE + 0x884)
#define COM_CTL_RB_COM_AHB_MONITOR_EN_1_REG        (COM_CTL_RB_BASE + 0x888) /* 内存监控开关寄存器 */
#define COM_CTL_RB_COM_ACCESS_MEM_INT_1_REG        (COM_CTL_RB_BASE + 0x88C) /* 内存监控一级中断状态查询寄存器 */
#define COM_CTL_RB_COM_ACCESS_MEM_INT_MASK_1_REG   (COM_CTL_RB_BASE + 0x890)
#define COM_CTL_RB_COM_ACCESS_MEM_INT_STS_1_REG    (COM_CTL_RB_BASE + 0x894)
#define COM_CTL_RB_COM_ACCESS_MEM_INT_CLR_1_REG    (COM_CTL_RB_BASE + 0x898)
#define COM_CTL_RB_COM_REC_ADDR_L_1_REG            (COM_CTL_RB_BASE + 0x89C)
#define COM_CTL_RB_COM_EC_ADDR_H_1_REG             (COM_CTL_RB_BASE + 0x8A0)
#define COM_CTL_RB_COM_REC_ID_1_REG                (COM_CTL_RB_BASE + 0x8A4)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR_EN_1_REG   (COM_CTL_RB_BASE + 0x8A8)
#define COM_CTL_RB_COM_PKT_MONITOR_INT_DLY_1_REG   (COM_CTL_RB_BASE + 0x8AC)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR0_L_L_1_REG (COM_CTL_RB_BASE + 0x8B0)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR0_L_H_1_REG (COM_CTL_RB_BASE + 0x8B4)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR0_H_L_1_REG (COM_CTL_RB_BASE + 0x8B8)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR0_H_H_1_REG (COM_CTL_RB_BASE + 0x8BC)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR1_L_L_1_REG (COM_CTL_RB_BASE + 0x8C0)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR1_L_H_1_REG (COM_CTL_RB_BASE + 0x8C4)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR1_H_L_1_REG (COM_CTL_RB_BASE + 0x8C8)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR1_H_H_1_REG (COM_CTL_RB_BASE + 0x8CC)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR2_L_L_1_REG (COM_CTL_RB_BASE + 0x8D0)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR2_L_H_1_REG (COM_CTL_RB_BASE + 0x8D4)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR2_H_L_1_REG (COM_CTL_RB_BASE + 0x8D8)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR2_H_H_1_REG (COM_CTL_RB_BASE + 0x8DC)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR3_L_L_1_REG (COM_CTL_RB_BASE + 0x8E0)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR3_L_H_1_REG (COM_CTL_RB_BASE + 0x8E4)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR3_H_L_1_REG (COM_CTL_RB_BASE + 0x8E8)
#define COM_CTL_RB_COM_MONITOR_MEM_ADDR3_H_H_1_REG (COM_CTL_RB_BASE + 0x8EC)
#define COM_CTL_RB_XO_CALI_HFREQ_CLK_EN_REG        (COM_CTL_RB_BASE + 0x900)
#define COM_CTL_RB_AON_AHB_HREADY_MUX_REG          (COM_CTL_RB_BASE + 0x940) /* AON内部AHB总线自动CG是否与上hready选择信号 */
#define COM_CTL_RB_MCU_SUB_AHB_HREADY_MUX_REG      (COM_CTL_RB_BASE + 0x9C8) /* MCU_SUB内部AHB总线自动CG是否与上hready选择信号 */
#define COM_CTL_RB_SDIO_AUTO_CG_CFG_REG            (COM_CTL_RB_BASE + 0xA00)
#define COM_CTL_RB_SDIO_SUB_AHB_HREADY_MUX_REG     (COM_CTL_RB_BASE + 0xA04) /* SDIO_SUB内部AHB总线自动CG是否与上hready选择信号 */
#define COM_CTL_RB_DISPLAY_AUTO_CG_CFG_REG         (COM_CTL_RB_BASE + 0xA40)
#define COM_CTL_RB_DISPLAY_SUB_AHB_HREADY_MUX_REG  (COM_CTL_RB_BASE + 0xA44) /* DISPLAY_SUB内部AHB总线自动CG是否与上hready选择信号 */
#define COM_CTL_RB_DISPLAY_SUB_AHB_AUTOCG_MUX_REG  (COM_CTL_RB_BASE + 0xA48) /* DISPLAY_SUB内部AHB总线自动CG信号用老的（CS）还是新的 */
#define COM_CTL_RB_GTOP_SUB_AHB_HREADY_MUX_REG     (COM_CTL_RB_BASE + 0xA94) /* GTOP_SUB内部AHB总线自动CG是否与上hready选择信号 */
#define COM_CTL_RB_GTOP_SUB_AHB_AUTOCG_MUX_REG     (COM_CTL_RB_BASE + 0xA98) /* GTOP_SUB内部AHB总线自动CG信号用老的（CS）还是新的 */
#define COM_CTL_RB_COM_BUS_LP_CFG_REG              (COM_CTL_RB_BASE + 0xAC8)
#define COM_CTL_RB_COM_BUS_LP_TIME_REG             (COM_CTL_RB_BASE + 0xACC)
#define COM_CTL_RB_COM_BUS_LP_STS_REG              (COM_CTL_RB_BASE + 0xAD0)
#define COM_CTL_RB_COM_BUS_LP_CLR_REG              (COM_CTL_RB_BASE + 0xAD4)
#define COM_CTL_RB_BRAM_SHARE_RESERVED_REG         (COM_CTL_RB_BASE + 0xAE4) /* BRAM SHARE MODE */
#define COM_CTL_RB_EXCLKEN_24M_REG                 (COM_CTL_RB_BASE + 0xAEC)
#define COM_CTL_RB_C_CRG_CH_SEL_REG                (COM_CTL_RB_BASE + 0xAF0)
#define COM_CTL_RB_COM_UART_REG                    (COM_CTL_RB_BASE + 0xAF4)
#define COM_CTL_RB_NPU_MEM_RET1N_REG               (COM_CTL_RB_BASE + 0xAF8)

#endif // __3322_COM_CTL_RB_REG_OFFSET_H__
