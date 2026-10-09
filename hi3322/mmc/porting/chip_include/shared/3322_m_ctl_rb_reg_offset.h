/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File name     :  3322_m_ctl_rb_reg_offset.h
 * Project line  :  Platform And Key Technologies Development
 * Department    :  CAD Development Department
 * Author        :  l00444884
 * Version       :  1.0
 * Date          :  2025/10/09
 * Description   :  The description of xxx project
 * Others        :  Generated automatically by nManager V5.1
 * History       :  l00444884 2025/03/05 17:25:13 Create file
 */

#ifndef __3322_M_CTL_RB_REG_OFFSET_H__
#define __3322_M_CTL_RB_REG_OFFSET_H__

/* M_CTL_RB Base address of Module's Register */
#define M_CTL_RB_BASE                       (0x52000000)

/******************************************************************************/
/*                      M_CTL_RB Registers' Definitions                            */
/******************************************************************************/

#define M_CTL_RB_M_CTL_ID_REG                       (M_CTL_RB_BASE + 0x0)   /* M CTL ID寄存器 */
#define M_CTL_RB_M_GP_REG0_REG                      (M_CTL_RB_BASE + 0x10)  /* 通用寄存器 */
#define M_CTL_RB_M_GP_REG1_REG                      (M_CTL_RB_BASE + 0x14)  /* 通用寄存器 */
#define M_CTL_RB_M_GP_REG2_REG                      (M_CTL_RB_BASE + 0x18)  /* 通用寄存器 */
#define M_CTL_RB_M_GP_REG3_REG                      (M_CTL_RB_BASE + 0x1C)  /* 通用寄存器 */
#define M_CTL_RB_X2M_SOFT_INT_EN_REG                (M_CTL_RB_BASE + 0x20)  /* 软中断寄存器 */
#define M_CTL_RB_X2M_SOFT_INT_REG                   (M_CTL_RB_BASE + 0x24)  /* 软中断 */
#define M_CTL_RB_X2M_SOFT_INT_STS_REG               (M_CTL_RB_BASE + 0x28)  /* 软中断 */
#define M_CTL_RB_X2M_SOFT_INT_CLR_REG               (M_CTL_RB_BASE + 0x2C)  /* 软中断寄存器 */
#define M_CTL_RB_M2X_SOFT_INT_CLR_STS_REG           (M_CTL_RB_BASE + 0x30)  /* 软中断 */
#define M_CTL_RB_M2X_SOFT_INT_SET_REG               (M_CTL_RB_BASE + 0x34)  /* 软中断寄存器 */
#define M_CTL_RB_M_CLKEN0_REG                       (M_CTL_RB_BASE + 0x40)  /* 时钟使能 */
#define M_CTL_RB_M_CLKEN1_REG                       (M_CTL_RB_BASE + 0x44)  /* 时钟使能 */
#define M_CTL_RB_M_CLKEN2_REG                       (M_CTL_RB_BASE + 0x48)  /* 时钟使能 */
#define M_CTL_RB_M_SOFT_TRG_REG                     (M_CTL_RB_BASE + 0x50)
#define M_CTL_RB_M_SOFT_RST_N_REG                   (M_CTL_RB_BASE + 0x54)
#define M_CTL_RB_M_SOFT_RST_N1_REG                  (M_CTL_RB_BASE + 0x58)
#define M_CTL_RB_MCU_MAIN_DFS_CFG0_REG              (M_CTL_RB_BASE + 0x5C)  /* Default Slaver配置 */
#define M_CTL_RB_MCU_MAIN_DFS_CFG1_REG              (M_CTL_RB_BASE + 0x60)
#define M_CTL_RB_MCU_MAIN_DFS_CFG2_REG              (M_CTL_RB_BASE + 0x64)
#define M_CTL_RB_M_DLL2_REG                         (M_CTL_RB_BASE + 0x6C)  /* DLL2 */
#define M_CTL_RB_M_DIV0_REG                         (M_CTL_RB_BASE + 0x70)  /* 时钟分频 */
#define M_CTL_RB_M_DIV1_REG                         (M_CTL_RB_BASE + 0x74)  /* 时钟分频 */
#define M_CTL_RB_M_DIV2_REG                         (M_CTL_RB_BASE + 0x78)  /* 时钟分频 */
#define M_CTL_RB_M_DIV3_REG                         (M_CTL_RB_BASE + 0x7C)  /* 时钟分频 */
#define M_CTL_RB_M_DIV4_REG                         (M_CTL_RB_BASE + 0x80)  /* 时钟分频 */
#define M_CTL_RB_M_DIV_EN_REG                       (M_CTL_RB_BASE + 0x90)  /* 时钟分频 */
#define M_CTL_RB_BT_DIAG_MEM_BUS_CLKEN_REG          (M_CTL_RB_BASE + 0x98)  /* BT_SUB模块维测时的时钟使能信号 */
#define M_CTL_RB_IPC_SOFT_CFG_REG                   (M_CTL_RB_BASE + 0x9C)  /* 核间通信软复位 */
#define M_CTL_RB_MONITOR_TCM_ADDR0_L_REG            (M_CTL_RB_BASE + 0x100) /* MCU_TCM_MONITOR_CFG0 */
#define M_CTL_RB_MONITOR_TCM_ADDR1_L_REG            (M_CTL_RB_BASE + 0x104) /* MCU_TCM_MONITOR_CFG1 */
#define M_CTL_RB_MONITOR_TCM_ADDR2_L_REG            (M_CTL_RB_BASE + 0x108) /* MCU_TCM_MONITOR_CFG2 */
#define M_CTL_RB_MONITOR_TCM_ADDR3_L_REG            (M_CTL_RB_BASE + 0x10C) /* MCU_TCM_MONITOR_CFG3 */
#define M_CTL_RB_MONITOR_TCM_ADDR0_H_REG            (M_CTL_RB_BASE + 0x110) /* MCU_TCM_MONITOR_CFG4 */
#define M_CTL_RB_MONITOR_TCM_ADDR1_H_REG            (M_CTL_RB_BASE + 0x114) /* MCU_TCM_MONITOR_CFG5 */
#define M_CTL_RB_MONITOR_TCM_ADDR2_H_REG            (M_CTL_RB_BASE + 0x118) /* MCU_TCM_MONITOR_CFG6 */
#define M_CTL_RB_MONITOR_TCM_ADDR3_H_REG            (M_CTL_RB_BASE + 0x11C) /* MCU_TCM_MONITOR_CFG7 */
#define M_CTL_RB_TCM_REC_ADDR_REG                   (M_CTL_RB_BASE + 0x120) /* MCU_TCM_MONITOR_READ_BAK_ADDR */
#define M_CTL_RB_TCM_REC_PC_REG                     (M_CTL_RB_BASE + 0x124) /* MCU_TCM_MONITOR_READ_BAK_PC */
#define M_CTL_RB_TCM_REC_LR_REG                     (M_CTL_RB_BASE + 0x128) /* MCU_TCM_MONITOR_READ_BAK_LR */
#define M_CTL_RB_TCM_REC_SP_REG                     (M_CTL_RB_BASE + 0x12C) /* MCU_TCM_MONITOR_READ_BAK_SP */
#define M_CTL_RB_MONITOR_TCM_CFG1_REG               (M_CTL_RB_BASE + 0x130) /* MONITOR_TCM_CFG1 */
#define M_CTL_RB_MONITOR_TCM_REAG_BAK_2_REG         (M_CTL_RB_BASE + 0x134)
#define M_CTL_RB_MCU_BUS_REG                        (M_CTL_RB_BASE + 0x13C)
#define M_CTL_RB_MCU_SUB_AUTO_CG_BYPASS0_REG        (M_CTL_RB_BASE + 0x140) /* AUTOCG_BYPASS0 */
#define M_CTL_RB_MCU_BUS_PRT_CFG_PGCGEN_ALL_REG     (M_CTL_RB_BASE + 0x148) /* PGCGEN_ALL */
#define M_CTL_RB_MCU_BUS_PRT_SOFT_RST_CLKEN_ALL_REG (M_CTL_RB_BASE + 0x14C) /* SOFT_RST_CLKEN_ALL */
#define M_CTL_RB_MTOP0_L_REG                        (M_CTL_RB_BASE + 0x150) /* 时钟门控 */
#define M_CTL_RB_MTOP0_H_REG                        (M_CTL_RB_BASE + 0x154) /* 时钟门控 */
#define M_CTL_RB_MTOP1_L_REG                        (M_CTL_RB_BASE + 0x158) /* 时钟门控 */
#define M_CTL_RB_MTOP1_H_REG                        (M_CTL_RB_BASE + 0x15C) /* 时钟门控 */
#define M_CTL_RB_MTOP2_L_REG                        (M_CTL_RB_BASE + 0x160) /* 时钟门控 */
#define M_CTL_RB_MTOP3_L_REG                        (M_CTL_RB_BASE + 0x168) /* 时钟门控 */
#define M_CTL_RB_MTOP3_H_REG                        (M_CTL_RB_BASE + 0x16C) /* 时钟门控 */
#define M_CTL_RB_DMA_HRDATA_EN_BYPASS_REG           (M_CTL_RB_BASE + 0x170) /* DMA hradat bypass */
#define M_CTL_RB_MTOP4_H_REG                        (M_CTL_RB_BASE + 0x174) /* 时钟门控 */
#define M_CTL_RB_M_AXI_BRG_LP_CFG0_REG              (M_CTL_RB_BASE + 0x180)
#define M_CTL_RB_M_AXI_BRG_LP_CFG1_REG              (M_CTL_RB_BASE + 0x184)
#define M_CTL_RB_MEM_MONITOR_INT_PC_REG             (M_CTL_RB_BASE + 0x188) /* MEM_MONITOR_INT_PC */
#define M_CTL_RB_MEM_MONITOR_INT_LR_REG             (M_CTL_RB_BASE + 0x18C) /* MEM_MONITOR_INT_LR */
#define M_CTL_RB_MEM_MONITOR_INT_SP_REG             (M_CTL_RB_BASE + 0x190) /* MEM_MONITOR_INT_SP */
#define M_CTL_RB_ACCESS_MEM_INT_OR_CLR_REG          (M_CTL_RB_BASE + 0x194) /* ACCESS_MEM_INT_OR_CLR */
#define M_CTL_RB_EH2H_BRG_INT_ST_REG                (M_CTL_RB_BASE + 0x1A8)
#define M_CTL_RB_EH2H_BRG_INT_CLR_REG               (M_CTL_RB_BASE + 0x1F4) /* EH2H中断清除寄存器 */
#define M_CTL_RB_EH2H_BRG_INT_EN_REG                (M_CTL_RB_BASE + 0x1F8) /* EH2H中断使能寄存器 */
#define M_CTL_RB_SEC_CRC_STRB_PROT_REG              (M_CTL_RB_BASE + 0x1FC) /* SEC_CRC_STRB_PROT寄存器 */
#define M_CTL_RB_DELAY_TIME_RST_CRG_REG             (M_CTL_RB_BASE + 0x208) /* 复位计数值 */
#define M_CTL_RB_DELAY_TIME_RST_LOGIC_REG           (M_CTL_RB_BASE + 0x20C) /* 复位计数值 */
#define M_CTL_RB_MCU_A2X_HINCR_WBANT_REG            (M_CTL_RB_BASE + 0x214) /* WBCNT控制寄存器 */
#define M_CTL_RB_MCU_A2X_HINCR_RBANT_REG            (M_CTL_RB_BASE + 0x218) /* WBCNT控制寄存器 */
#define M_CTL_RB_MGPIO_INT_CFG0_REG                 (M_CTL_RB_BASE + 0x2B4)
#define M_CTL_RB_MGPIO_INT_CFG1_REG                 (M_CTL_RB_BASE + 0x2B8)
#define M_CTL_RB_MGPIO_INT_CFG2_REG                 (M_CTL_RB_BASE + 0x2BC)
#define M_CTL_RB_MGPIO_INT_CFG3_REG                 (M_CTL_RB_BASE + 0x2C0)
#define M_CTL_RB_UART_TRX_CFG_REG                   (M_CTL_RB_BASE + 0x2CC)
#define M_CTL_RB_RAM_TMOD_CFG0_REG                  (M_CTL_RB_BASE + 0x2E0)
#define M_CTL_RB_RAM_TMOD_CFG1_REG                  (M_CTL_RB_BASE + 0x2E4)
#define M_CTL_RB_RAM_TMOD_CFG2_REG                  (M_CTL_RB_BASE + 0x2E8)
#define M_CTL_RB_RAM_TMOD_CFG3_REG                  (M_CTL_RB_BASE + 0x2EC)
#define M_CTL_RB_MCU_SC_HPM0_CTRL0_REG              (M_CTL_RB_BASE + 0x2F0) /* HPM配置寄存器，恒电区使用 */
#define M_CTL_RB_MCU_SC_HPM0_CTRL1_REG              (M_CTL_RB_BASE + 0x2F4) /* HPM配置寄存器，恒电区使用 */
#define M_CTL_RB_MCU_SC_HPM0_CTRL2_REG              (M_CTL_RB_BASE + 0x2F8) /* HPM配置寄存器，恒电区使用 */
#define M_CTL_RB_MCU_SC_HPM0_CTRL3_REG              (M_CTL_RB_BASE + 0x2FC) /* HPM配置寄存器，恒电区使用 */
#define M_CTL_RB_MCU_SC_HPM0_CTRL4_REG              (M_CTL_RB_BASE + 0x300) /* HPM配置寄存器，恒电区使用 */
#define M_CTL_RB_MCU_SC_HPM0_CTRL5_REG              (M_CTL_RB_BASE + 0x304) /* HPM配置寄存器，恒电区使用 */
#define M_CTL_RB_MCU_SC_HPM0_CTRL6_REG              (M_CTL_RB_BASE + 0x308) /* HPM配置寄存器，恒电区使用 */
#define M_CTL_RB_MCU_SC_HPM0_CTRL7_REG              (M_CTL_RB_BASE + 0x30C) /* HPM配置寄存器，恒电区使用 */
#define M_CTL_RB_MCU_SC_HPM0_CTRL8_REG              (M_CTL_RB_BASE + 0x310) /* HPM配置寄存器，恒电区使用 */
#define M_CTL_RB_MCU_SC_HPM0_CTRL9_REG              (M_CTL_RB_BASE + 0x314) /* HPM配置寄存器，恒电区使用 */
#define M_CTL_RB_MCPU_ROM_CFG_REG                   (M_CTL_RB_BASE + 0x3C4)
#define M_CTL_RB_MSUB_NOR_CFG_REG                   (M_CTL_RB_BASE + 0x3E0)
#define M_CTL_RB_MCU_RAM_LP_CTRL_LS_REG             (M_CTL_RB_BASE + 0x410)
#define M_CTL_RB_MCU_SYS_TICK_CFG_D_REG             (M_CTL_RB_BASE + 0x414)
#define M_CTL_RB_MCU_SYS_TICK_VALUE_D_0_REG         (M_CTL_RB_BASE + 0x418)
#define M_CTL_RB_MCU_SYS_TICK_VALUE_D_1_REG         (M_CTL_RB_BASE + 0x41C)
#define M_CTL_RB_MEM_EMA_SEL_REG_REG                (M_CTL_RB_BASE + 0x540) /* ULL_MEM_REG_EFUSE选择 */
#define M_CTL_RB_MCU_TCXO_DIV_CFG_REG               (M_CTL_RB_BASE + 0x548) /* TCXO时钟分频配置 */
#define M_CTL_RB_MCU_UART_LO_CRG_CFG0_REG           (M_CTL_RB_BASE + 0x54C) /* MCU_UART_LO_CRG_CFG0 */
#define M_CTL_RB_MCU_CORE_CR_CH1_BAK_REG            (M_CTL_RB_BASE + 0x550) /* MCU_CORE CR CH1寄存器 */
#define M_CTL_RB_MCU_PERP_LS_CR_BAK_REG             (M_CTL_RB_BASE + 0x554) /* MCU_PERP LS CR寄存器 */
#define M_CTL_RB_MCU_PERP_UART_CR_BAK_REG           (M_CTL_RB_BASE + 0x558) /* MCU_PERP UART CR寄存器 */
#define M_CTL_RB_MCU_PERP_SPI_CR_BAK_REG            (M_CTL_RB_BASE + 0x55C) /* MCU_PERP SPI CR寄存器 */
#define M_CTL_RB_COM_BUS_CR_CH0_BAK_REG             (M_CTL_RB_BASE + 0x560) /* COM_BUS CR CH0寄存器 */
#define M_CTL_RB_COM_BUS_CR_CH1_BAK_REG             (M_CTL_RB_BASE + 0x564) /* COM_BUS CR CH1寄存器 */
#define M_CTL_RB_MEM_BUS_CR_BAK_REG                 (M_CTL_RB_BASE + 0x56C) /* MEM_BUS CR寄存器 */
#define M_CTL_RB_XIP_OPI_CR_BAK_REG                 (M_CTL_RB_BASE + 0x570) /* XIP_OPI CR寄存器 */
#define M_CTL_RB_XIP_QSPI_CR_BAK_REG                (M_CTL_RB_BASE + 0x574) /* XIP_QSPI CR寄存器 */
#define M_CTL_RB_GSUB_CR_BAK_REG                    (M_CTL_RB_BASE + 0x57C) /* GSUB CR寄存器 */
#define M_CTL_RB_GCPU_CR_BAK_REG                    (M_CTL_RB_BASE + 0x580) /* GCPU CR寄存器 */
#define M_CTL_RB_GPU_CR_BAK_REG                     (M_CTL_RB_BASE + 0x584) /* GPU CR寄存器 */
#define M_CTL_RB_HIFI_CR_CH0_BAK_REG                (M_CTL_RB_BASE + 0x588) /* HIFI CR CH0寄存器 */
#define M_CTL_RB_HIFI_CR_CH1_BAK_REG                (M_CTL_RB_BASE + 0x58C) /* HIFI CR CH1寄存器 */
#define M_CTL_RB_CODEC_CR_CH0_BAK_REG               (M_CTL_RB_BASE + 0x590) /* CODEC CR CH0寄存器 */
#define M_CTL_RB_CODEC_CR_CH1_BAK_REG               (M_CTL_RB_BASE + 0x594) /* CODEC CR CH1寄存器 */
#define M_CTL_RB_PAD_CLK_OUT0_CR_BAK_REG            (M_CTL_RB_BASE + 0x598) /* PAD_CLK_OUT0 CR寄存器 */
#define M_CTL_RB_PAD_CLK_OUT1_CR_BAK_REG            (M_CTL_RB_BASE + 0x59C) /* PAD_CLK_OUT1 CR寄存器 */
#define M_CTL_RB_PAD_CLK_OUT2_CR_BAK_REG            (M_CTL_RB_BASE + 0x600) /* PAD_CLK_OUT2 CR寄存器 */
#define M_CTL_RB_M_NMI_INT_EN_REG                   (M_CTL_RB_BASE + 0x700) /* MNMI中断使能寄存器 */
#define M_CTL_RB_M_NMI_INT_RAW_STS_REG              (M_CTL_RB_BASE + 0x704) /* MNMI中断原始状态寄存器 */
#define M_CTL_RB_M_NMI_INT_STS_REG                  (M_CTL_RB_BASE + 0x708) /* NMI中断mask后状态寄存器 */
#define M_CTL_RB_MCPU_PCLR_OK_INT_EN_REG            (M_CTL_RB_BASE + 0x70C) /* 中断使能寄存器 */
#define M_CTL_RB_SEC_AUTO_CG_REG                    (M_CTL_RB_BASE + 0x800)
#define M_CTL_RB_SEC_AHB_DFS_EN_REG                 (M_CTL_RB_BASE + 0x804)
#define M_CTL_RB_SEC_AHB_DFS_CLEAR_REG              (M_CTL_RB_BASE + 0x808)
#define M_CTL_RB_S_CFG_AHB_OT_DELAY_REG             (M_CTL_RB_BASE + 0x810)
#define M_CTL_RB_SCPU_SLEEP_ALLOW_REG               (M_CTL_RB_BASE + 0x82C)
#define M_CTL_RB_MCU_IO_TEST_IN_REG                 (M_CTL_RB_BASE + 0x830)
#define M_CTL_RB_MCU_IO_TEST_OUT_REG                (M_CTL_RB_BASE + 0x834)
#define M_CTL_RB_MCU_IO_TEST_OEN_REG                (M_CTL_RB_BASE + 0x838)
#define M_CTL_RB_MCPU0_CFG_REG                      (M_CTL_RB_BASE + 0x940)
#define M_CTL_RB_SPI_CFG_REG                        (M_CTL_RB_BASE + 0x950)
#define M_CTL_RB_DMA_SEL_CFG_REG                    (M_CTL_RB_BASE + 0x960)
#define M_CTL_RB_I2C_CLK_FORCE_ON_REG               (M_CTL_RB_BASE + 0x988)
#define M_CTL_RB_PWM_CLK_FORCE_ON_REG               (M_CTL_RB_BASE + 0x98C)
#define M_CTL_RB_BUS_PRIORITY_0_REG                 (M_CTL_RB_BASE + 0xA00)
#define M_CTL_RB_BUS_PRIORITY_1_REG                 (M_CTL_RB_BASE + 0xA04)
#define M_CTL_RB_BUS_PRIORITY_4_REG                 (M_CTL_RB_BASE + 0xA10)
#define M_CTL_RB_BUS_PRIORITY_5_REG                 (M_CTL_RB_BASE + 0xA14)
#define M_CTL_RB_BUS_PRIORITY_6_REG                 (M_CTL_RB_BASE + 0xA18)
#define M_CTL_RB_BUS_PRIORITY_7_REG                 (M_CTL_RB_BASE + 0xA1C)
#define M_CTL_RB_BUS_PRIORITY_8_REG                 (M_CTL_RB_BASE + 0xA20)
#define M_CTL_RB_BUS_PRIORITY_10_BAK_REG            (M_CTL_RB_BASE + 0xA28)
#define M_CTL_RB_BUS_PRIORITY_11_BAK_REG            (M_CTL_RB_BASE + 0xA2C)
#define M_CTL_RB_BUS_PRIORITY_12_BAK_REG            (M_CTL_RB_BASE + 0xA30)
#define M_CTL_RB_BUS_PRIORITY_13_BAK_REG            (M_CTL_RB_BASE + 0xA34)
#define M_CTL_RB_MEM_SHARE_CTRL_BAK_REG             (M_CTL_RB_BASE + 0xA50)
#define M_CTL_RB_CFG_G_RAM_SEL_REG                  (M_CTL_RB_BASE + 0xA54)
#define M_CTL_RB_MCU_EH2H_ERROR_RESP_REG            (M_CTL_RB_BASE + 0xA60)
#define M_CTL_RB_MCU_EH2H_ERROR_RESP_CLR_REG        (M_CTL_RB_BASE + 0xA64)
#define M_CTL_RB_MEM_SHARE_CTRL_GT_BAK_REG          (M_CTL_RB_BASE + 0xA68)
#define M_CTL_RB_MCU_PERP_AXI_MONITOR_0_REG         (M_CTL_RB_BASE + 0xA90)
#define M_CTL_RB_MCU_PERP_AXI_MONITOR_1_REG         (M_CTL_RB_BASE + 0xA94)
#define M_CTL_RB_MCU_PERP_AXI_MONITOR_2_REG         (M_CTL_RB_BASE + 0xA98)
#define M_CTL_RB_MCU_PERP_AXI_MONITOR_3_REG         (M_CTL_RB_BASE + 0xA9C)
#define M_CTL_RB_MCU_PERP_AXI_MONITOR_4_REG         (M_CTL_RB_BASE + 0xAA0)
#define M_CTL_RB_MCU_PERP_AXI_MONITOR_5_REG         (M_CTL_RB_BASE + 0xAA4)
#define M_CTL_RB_MCU_PERP_AXI_MONITOR_6_REG         (M_CTL_RB_BASE + 0xAA8)
#define M_CTL_RB_AUX_ADC_CFG_REG                    (M_CTL_RB_BASE + 0xB00) /* AUX_ADC控制寄存器 */
#define M_CTL_RB_AUX_ADC_SEL_REG                    (M_CTL_RB_BASE + 0xB04) /* AUX_ADC控制寄存器 */
#define M_CTL_RB_AUX_ADC_EOC_FLAG_REG               (M_CTL_RB_BASE + 0xB08) /* AUX_ADC状态寄存器 */
#define M_CTL_RB_AUX_ADC_DOUT_REG                   (M_CTL_RB_BASE + 0xB0C) /* AUX_ADC状态寄存器 */
#define M_CTL_RB_AUX_ADC_REG_L_REG                  (M_CTL_RB_BASE + 0xB10) /* AUX_ADC控制寄存器 */
#define M_CTL_RB_AUX_ADC_REG_H_REG                  (M_CTL_RB_BASE + 0xAAC) /* AUX_ADC控制寄存器 */
#define M_CTL_RB_AUX_ADC_AUTO_CFG_REG               (M_CTL_RB_BASE + 0xB14) /* AUX_ADC状态寄存器 */
#define M_CTL_RB_AUX_ADC_STICK_REG                  (M_CTL_RB_BASE + 0xB18) /* AUX_ADC状态寄存器 */
#define M_CTL_RB_AUX_ADC_STICK_CLR_REG              (M_CTL_RB_BASE + 0xB1C) /* AUX_ADC状态寄存器 */
#define M_CTL_RB_AUX_ADC_CALI_EN_REG                (M_CTL_RB_BASE + 0xB20) /* AUX_ADC校准使能寄存器 */
#define M_CTL_RB_AUX_ADC_CALI_CFG_REG               (M_CTL_RB_BASE + 0xB24) /* AUX_ADC校准配置寄存器 */
#define M_CTL_RB_AUX_ADC_CALI_PERIOD_REG            (M_CTL_RB_BASE + 0xB28) /* AUX_ADC校准周期寄存器 */
#define M_CTL_RB_AUX_ADC_CALI_CFG_SYNC_REG          (M_CTL_RB_BASE + 0xB30) /* AUX_ADC校准配置使能寄存器 */
#define M_CTL_RB_AUX_ADC_CALI_STATUS_REG            (M_CTL_RB_BASE + 0xB34) /* AUX_ADC校准状态寄存器 */
#define M_CTL_RB_AUX_ADC_CALI_OUT_REG               (M_CTL_RB_BASE + 0xB38) /* AUX_ADC校准结果寄存器 */
#define M_CTL_RB_AUX_ADC_CALI_COEF_GAIN1_REG        (M_CTL_RB_BASE + 0xB48) /* AUX_ADC校准增益寄存器 */
#define M_CTL_RB_AUX_ADC_CALI_COEF_OFST1_REG        (M_CTL_RB_BASE + 0xB4C) /* AUX_ADC校准偏移寄存器 */
#define M_CTL_RB_AUX_ADC_CALI_COEF_GAIN2_REG        (M_CTL_RB_BASE + 0xB50) /* AUX_ADC校准增益寄存器 */
#define M_CTL_RB_AUX_ADC_CALI_COEF_OFST2_REG        (M_CTL_RB_BASE + 0xB54) /* AUX_ADC校准偏移寄存器 */
#define M_CTL_RB_AUX_ADC_CALI_BYPASS_REG            (M_CTL_RB_BASE + 0xB5C) /* AUX_ADC校准旁路寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_EN_REG                (M_CTL_RB_BASE + 0xB60) /* AUX_ADC扫描使能寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_FREQ_REG              (M_CTL_RB_BASE + 0xB64) /* AUX_ADC扫描频率寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_MODE0_REG             (M_CTL_RB_BASE + 0xB68) /* AUX_ADC扫描模式寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_MODE1_REG             (M_CTL_RB_BASE + 0xB6C) /* AUX_ADC扫描模式寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_TH0_L_REG             (M_CTL_RB_BASE + 0xB80) /* AUX_ADC扫描阈值寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_TH0_H_REG             (M_CTL_RB_BASE + 0xB84) /* AUX_ADC扫描阈值寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_TH2_L_REG             (M_CTL_RB_BASE + 0xB88) /* AUX_ADC扫描阈值寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_TH2_H_REG             (M_CTL_RB_BASE + 0xB8C) /* AUX_ADC扫描阈值寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_TH4_L_REG             (M_CTL_RB_BASE + 0xB90) /* AUX_ADC扫描阈值寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_TH4_H_REG             (M_CTL_RB_BASE + 0xB94) /* AUX_ADC扫描阈值寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_TH5_L_REG             (M_CTL_RB_BASE + 0xB98) /* AUX_ADC扫描阈值寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_TH5_H_REG             (M_CTL_RB_BASE + 0xB9C) /* AUX_ADC扫描阈值寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_TH6_L_REG             (M_CTL_RB_BASE + 0xBA0) /* AUX_ADC扫描阈值寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_TH6_H_REG             (M_CTL_RB_BASE + 0xBA4) /* AUX_ADC扫描阈值寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_TH7_L_REG             (M_CTL_RB_BASE + 0xBA8) /* AUX_ADC扫描阈值寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_TH7_H_REG             (M_CTL_RB_BASE + 0xBAC) /* AUX_ADC扫描阈值寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_TH8_L_REG             (M_CTL_RB_BASE + 0xBB0) /* AUX_ADC扫描阈值寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_TH8_H_REG             (M_CTL_RB_BASE + 0xBB4) /* AUX_ADC扫描阈值寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_TH9_L_REG             (M_CTL_RB_BASE + 0xBB8) /* AUX_ADC扫描阈值寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_TH9_H_REG             (M_CTL_RB_BASE + 0xBBC) /* AUX_ADC扫描阈值寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_PTR1_REG              (M_CTL_RB_BASE + 0xBC4) /* AUX_ADC扫描指针寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_PTR2_REG              (M_CTL_RB_BASE + 0xBC8) /* AUX_ADC扫描指针寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_PTR3_REG              (M_CTL_RB_BASE + 0xBCC) /* AUX_ADC扫描指针寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_PTR4_REG              (M_CTL_RB_BASE + 0xBD0) /* AUX_ADC扫描指针寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_INT_EN_REG            (M_CTL_RB_BASE + 0xBD4) /* AUX_ADC中断使能寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_STAT_REG              (M_CTL_RB_BASE + 0xBD8) /* AUX_ADC中断状态寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_INT_CLR_REG           (M_CTL_RB_BASE + 0xBDC) /* AUX_ADC中断清除寄存器 */
#define M_CTL_RB_AUX_ADC_SCAN_EN_STAT_REG           (M_CTL_RB_BASE + 0xBE0) /* AUX_ADC SCAN状态寄存器 */
#define M_CTL_RB_AUX_ADC_CALI_OFFSET2_REG           (M_CTL_RB_BASE + 0xBE8) /* AUX_ADC_CALI_OFFSET2控制 */
#define M_CTL_RB_AUX_ADC_CALI_GAIN2_REG             (M_CTL_RB_BASE + 0xBF0) /* AUX_ADC_CALI_GAIN2控制 */
#define M_CTL_RB_SDIO_DEBUG0_REG                    (M_CTL_RB_BASE + 0xC00) /* SDIO debug状态查询寄存器 */
#define M_CTL_RB_SDIO_DEBUG1_REG                    (M_CTL_RB_BASE + 0xC04) /* SDIO debug状态查询寄存器 */
#define M_CTL_RB_SDIO_DEBUG2_REG                    (M_CTL_RB_BASE + 0xC08) /* SDIO debug状态查询寄存器 */
#define M_CTL_RB_SDIO_DEBUG3_REG                    (M_CTL_RB_BASE + 0xC0C) /* SDIO debug状态查询寄存器 */
#define M_CTL_RB_SDIO_DEV_PAD_STS_REG               (M_CTL_RB_BASE + 0xC10) /* SDIO device PAD状态查询寄存器 */
#define M_CTL_RB_SDIO_HOST_PAD_STS_REG              (M_CTL_RB_BASE + 0xC14) /* SDIO device PAD状态查询寄存器 */
#define M_CTL_RB_EMMC_PAD_STS_REG                   (M_CTL_RB_BASE + 0xC18) /* SDIO device PAD状态查询寄存器 */
#define M_CTL_RB_EMMC_PHY_CFG_REG                   (M_CTL_RB_BASE + 0xC1C) /* EMMC PHY配置寄存器 */
#define M_CTL_RB_PULSE_STAT_CFG_REG                 (M_CTL_RB_BASE + 0xC20) /* PULSE_STAT配置寄存器 */
#define M_CTL_RB_STAT_POS_CNT_REG                   (M_CTL_RB_BASE + 0xC24) /* 上升沿计数查询寄存器 */
#define M_CTL_RB_STAT_NEG_CNT_REG                   (M_CTL_RB_BASE + 0xC28) /* 下降沿计数查询寄存器 */
#define M_CTL_RB_HS_BUS_CLK_EN_REG                  (M_CTL_RB_BASE + 0xC2C)
#define M_CTL_RB_M_TCXO_CNT_CFG_REG                 (M_CTL_RB_BASE + 0xC34)
#define M_CTL_RB_M_TCXO_VALUE_0_REG                 (M_CTL_RB_BASE + 0xC38)
#define M_CTL_RB_M_TCXO_VALUE_1_REG                 (M_CTL_RB_BASE + 0xC3C)
#define M_CTL_RB_M_TCXO_TE_INT_CNT_CFG_REG          (M_CTL_RB_BASE + 0xC50)
#define M_CTL_RB_M_TCXO_TE_INT_VALUE_0_REG          (M_CTL_RB_BASE + 0xC54)
#define M_CTL_RB_M_TCXO_TE_INT_VALUE_1_REG          (M_CTL_RB_BASE + 0xC58)
#define M_CTL_RB_M_PWM_CFG_REG                      (M_CTL_RB_BASE + 0xD04) /* pwm寄存器控制 */
#define M_CTL_RB_VIDEO_PREADY_DFS_OVER_TIME_REG     (M_CTL_RB_BASE + 0xF80) /* VIDEO_PREADY_DFS_OVER_TIME_CFG */
#define M_CTL_RB_AHB_HREADY_MUX_BAK_REG             (M_CTL_RB_BASE + 0xF84) /* CFG_AHB、MCU_AHB总线自动CG是否与上hready选择信号 */
#define M_CTL_RB_VIDEO_PREADY_DFS_CFG_REG           (M_CTL_RB_BASE + 0xF88)
#define M_CTL_RB_TRNG_SAMPLE_CKDIV_REG              (M_CTL_RB_BASE + 0xFA0) /* 时钟分频 */
#define M_CTL_RB_LINX170_CTL_REG                    (M_CTL_RB_BASE + 0xFA4) /* CPU相关寄存器 */
#define M_CTL_RB_TRNG_RING_EN_REG                   (M_CTL_RB_BASE + 0xFD8) /* TRNG_RING_EN */

#endif // __3322_M_CTL_RB_REG_OFFSET_H__
