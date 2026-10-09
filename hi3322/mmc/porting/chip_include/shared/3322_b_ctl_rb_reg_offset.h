/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File name     :  3322_b_ctl_rb_reg_offset.h
 * Project line  :  Platform And Key Technologies Development
 * Department    :  CAD Development Department
 * Author        :  l00444884
 * Version       :  1.0
 * Date          :  2025/10/09
 * Description   :  The description of xxx project
 * Others        :  Generated automatically by nManager V5.1
 * History       :  l00444884 2025/03/05 17:25:13 Create file
 */

#ifndef __3322_B_CTL_RB_REG_OFFSET_H__
#define __3322_B_CTL_RB_REG_OFFSET_H__

/* B_CTL_RB Base address of Module's Register */
#define B_CTL_RB_BASE                       (0x59000000)

/******************************************************************************/
/*                      B_CTL_RB Registers' Definitions                            */
/******************************************************************************/

#define B_CTL_RB_B_CTL_ID_REG                         (B_CTL_RB_BASE + 0x0)   /* B CTL ID寄存器 */
#define B_CTL_RB_B_GP_REG0_REG                        (B_CTL_RB_BASE + 0x10)  /* 通用寄存器 */
#define B_CTL_RB_B_GP_REG1_REG                        (B_CTL_RB_BASE + 0x14)  /* 通用寄存器 */
#define B_CTL_RB_B_GP_REG2_REG                        (B_CTL_RB_BASE + 0x18)  /* 通用寄存器 */
#define B_CTL_RB_B_GP_REG3_REG                        (B_CTL_RB_BASE + 0x1C)  /* 通用寄存器 */
#define B_CTL_RB_LINX132_BCPU_ST_REG                  (B_CTL_RB_BASE + 0x20)  /* CPU输出状态 */
#define B_CTL_RB_LINX132_BCPU_GPREG_SEL_REG           (B_CTL_RB_BASE + 0x24)  /* 保留 */
#define B_CTL_RB_LINX132_BCPU_INT_EXCP_RED_HDLR_H_REG (B_CTL_RB_BASE + 0x28)  /* CPU通用寄存高16bit */
#define B_CTL_RB_LINX132_BCPU_INT_EXCP_RED_HDLR_L_REG (B_CTL_RB_BASE + 0x2C)  /* CPU通用寄存低16bit */
#define B_CTL_RB_B_FEM_CTL_EN_REG                     (B_CTL_RB_BASE + 0x30)
#define B_CTL_RB_BTOP0_L_REG                          (B_CTL_RB_BASE + 0x60)  /* 时钟门控 */
#define B_CTL_RB_BTOP0_H_REG                          (B_CTL_RB_BASE + 0x64)  /* 时钟门控 */
#define B_CTL_RB_BWDT_DIS_REG                         (B_CTL_RB_BASE + 0x70)
#define B_CTL_RB_BWDT_HALT_IN_REG                     (B_CTL_RB_BASE + 0x74)
#define B_CTL_RB_BT_FEM_CTL_REG_CFG_0_3_REG           (B_CTL_RB_BASE + 0x78)
#define B_CTL_RB_BT_FEM_CTL_REG_CFG_7_4_REG           (B_CTL_RB_BASE + 0x7C)
#define B_CTL_RB_BT_FEM_CTL_REG_CFG_11_8_REG          (B_CTL_RB_BASE + 0x80)
#define B_CTL_RB_BT_FEM_CTL_REG_CFG_12_15_REG         (B_CTL_RB_BASE + 0x84)
#define B_CTL_RB_BT_FEM_CTL_REG_CFG_19_16_REG         (B_CTL_RB_BASE + 0x88)
#define B_CTL_RB_BT_FEM_CTL_REG_CFG_20_23_REG         (B_CTL_RB_BASE + 0x8C)
#define B_CTL_RB_BT_FEM_CTL_REG_CFG_24_27_REG         (B_CTL_RB_BASE + 0x90)
#define B_CTL_RB_BT_FEM_CTL_REG_CFG_28_31_REG         (B_CTL_RB_BASE + 0x94)
#define B_CTL_RB_B_TOP_AHB_PRIORITY_S2_REG            (B_CTL_RB_BASE + 0x130)
#define B_CTL_RB_B_TOP_AHB_PRIORITY_S3_REG            (B_CTL_RB_BASE + 0x134)
#define B_CTL_RB_B_TOP_AHB_PRIORITY_S1_REG            (B_CTL_RB_BASE + 0x138)
#define B_CTL_RB_B_TOP_AHB_PRIORITY_S4_REG            (B_CTL_RB_BASE + 0x13C)
#define B_CTL_RB_B_TOP_AHB_PRIORITY_S5_REG            (B_CTL_RB_BASE + 0x140)
#define B_CTL_RB_B_TOP_AHB_PRIORITY_S6_REG            (B_CTL_RB_BASE + 0x144)
#define B_CTL_RB_B_TOP_AHB_AUTO_CG_CFG_REG            (B_CTL_RB_BASE + 0x148)
#define B_CTL_RB_BSOC_BUS_DFS_CFG_REG                 (B_CTL_RB_BASE + 0x14C)
#define B_CTL_RB_BSOC_BUS_DFS_TIME_REG                (B_CTL_RB_BASE + 0x150)
#define B_CTL_RB_B_TOP_AHB_PRIORITY_S7_REG            (B_CTL_RB_BASE + 0x154)
#define B_CTL_RB_BT_CBB_POLAR_MEM_RET1N_REG           (B_CTL_RB_BASE + 0x250)
#define B_CTL_RB_BT_EM_POWER_MODE_REG                 (B_CTL_RB_BASE + 0x270)
#define B_CTL_RB_BT_NMI_MASK_REG                      (B_CTL_RB_BASE + 0x300)
#define B_CTL_RB_BT_NMI_RAW_STS_REG                   (B_CTL_RB_BASE + 0x304)
#define B_CTL_RB_BT_NMI_MASK_STS_REG                  (B_CTL_RB_BASE + 0x308)
#define B_CTL_RB_BCPU_PCLR_OK_MASK_REG                (B_CTL_RB_BASE + 0x30C)
#define B_CTL_RB_CLK32K_DET_DONE_MASK_REG             (B_CTL_RB_BASE + 0x310)
#define B_CTL_RB_BT_LPEG_SEQ_STS_REG                  (B_CTL_RB_BASE + 0x318)
#define B_CTL_RB_BT_LPEG_INT_STS_REG                  (B_CTL_RB_BASE + 0x31C)
#define B_CTL_RB_B_TCXO_CNT_CFG_REG                   (B_CTL_RB_BASE + 0x320)
#define B_CTL_RB_B_TCXO_VALUE_0_REG                   (B_CTL_RB_BASE + 0x324)
#define B_CTL_RB_B_TCXO_VALUE_1_REG                   (B_CTL_RB_BASE + 0x328)
#define B_CTL_RB_X2B_SOFT_INT_EN_REG                  (B_CTL_RB_BASE + 0x600) /* 软中断寄存器 */
#define B_CTL_RB_X2B_SOFT_INT_REG                     (B_CTL_RB_BASE + 0x604) /* 软中断 */
#define B_CTL_RB_X2B_SOFT_INT_STS_REG                 (B_CTL_RB_BASE + 0x608) /* 软中断 */
#define B_CTL_RB_X2B_SOFT_INT_CLR_REG                 (B_CTL_RB_BASE + 0x60C) /* 软中断寄存器 */
#define B_CTL_RB_B2X_SOFT_INT_CLR_STS_REG             (B_CTL_RB_BASE + 0x610) /* 软中断 */
#define B_CTL_RB_B2X_SOFT_INT_SET_REG                 (B_CTL_RB_BASE + 0x614) /* 软中断寄存器 */
#define B_CTL_RB_B2X_SOFT_RST_SET_REG                 (B_CTL_RB_BASE + 0x618) /* 软中断寄存器 */
#define B_CTL_RB_B_SOC_DIAG_REG                       (B_CTL_RB_BASE + 0x700) /* BT SOC DIAG SEL */
#define B_CTL_RB_BRAM_SHARE_REG                       (B_CTL_RB_BASE + 0x780) /* BRAM SHARE MODE */
#define B_CTL_RB_B_CRG_CLKEN_REG                      (B_CTL_RB_BASE + 0x800)
#define B_CTL_RB_B_CRG_CLKSEL_REG                     (B_CTL_RB_BASE + 0x804)
#define B_CTL_RB_B_CRG_SOFT_TRG_REG                   (B_CTL_RB_BASE + 0x808)
#define B_CTL_RB_B_CRG_SOFT_RST_N_REG                 (B_CTL_RB_BASE + 0x80C)
#define B_CTL_RB_B_CRG_DLL_REG                        (B_CTL_RB_BASE + 0x810)
#define B_CTL_RB_B_32M_CG_MX_DIV_REG                  (B_CTL_RB_BASE + 0x814)
#define B_CTL_RB_BPERP_BUS_DIV_REG                    (B_CTL_RB_BASE + 0x818)
#define B_CTL_RB_B_CPU_SOFT_RST_N_REG                 (B_CTL_RB_BASE + 0x81C)
#define B_CTL_RB_BT_TSENSOR_DIV_REG                   (B_CTL_RB_BASE + 0x820)
#define B_CTL_RB_BT_ADC_CLK_CTL_REG                   (B_CTL_RB_BASE + 0x824)
#define B_CTL_RB_BT_MEM_BANK_CG_GEN_REG               (B_CTL_RB_BASE + 0x828)
#define B_CTL_RB_BT_TCXO_DIV_CTL_REG                  (B_CTL_RB_BASE + 0x830)
#define B_CTL_RB_AHB_MONITOR_EN_0_REG                 (B_CTL_RB_BASE + 0x920) /* 内存监控开关寄存器 */
#define B_CTL_RB_ACCESS_MEM_INT_0_REG                 (B_CTL_RB_BASE + 0x924) /* 内存监控一级中断状态查询寄存器 */
#define B_CTL_RB_ACCESS_MEM_INT_MASK_0_REG            (B_CTL_RB_BASE + 0x928)
#define B_CTL_RB_ACCESS_MEM_INT_STS_0_REG             (B_CTL_RB_BASE + 0x92C)
#define B_CTL_RB_ACCESS_MEM_INT_CLR_0_REG             (B_CTL_RB_BASE + 0x930)
#define B_CTL_RB_REC_ADDR_L_0_REG                     (B_CTL_RB_BASE + 0x934)
#define B_CTL_RB_REC_ADDR_H_0_REG                     (B_CTL_RB_BASE + 0x938)
#define B_CTL_RB_REC_ID_0_REG                         (B_CTL_RB_BASE + 0x93C)
#define B_CTL_RB_MONITOR_MEM_ADDR_EN_0_REG            (B_CTL_RB_BASE + 0x940)
#define B_CTL_RB_PKT_MONITOR_INT_DLY_0_REG            (B_CTL_RB_BASE + 0x944)
#define B_CTL_RB_MONITOR_MEM_ADDR0_L_L_0_REG          (B_CTL_RB_BASE + 0x948)
#define B_CTL_RB_MONITOR_MEM_ADDR0_L_H_0_REG          (B_CTL_RB_BASE + 0x94C)
#define B_CTL_RB_MONITOR_MEM_ADDR0_H_L_0_REG          (B_CTL_RB_BASE + 0x950)
#define B_CTL_RB_MONITOR_MEM_ADDR0_H_H_0_REG          (B_CTL_RB_BASE + 0x954)
#define B_CTL_RB_MONITOR_MEM_ADDR1_L_L_0_REG          (B_CTL_RB_BASE + 0x958)
#define B_CTL_RB_MONITOR_MEM_ADDR1_L_H_0_REG          (B_CTL_RB_BASE + 0x95C)
#define B_CTL_RB_MONITOR_MEM_ADDR1_H_L_0_REG          (B_CTL_RB_BASE + 0x960)
#define B_CTL_RB_MONITOR_MEM_ADDR1_H_H_0_REG          (B_CTL_RB_BASE + 0x964)
#define B_CTL_RB_MONITOR_MEM_ADDR2_L_L_0_REG          (B_CTL_RB_BASE + 0x968)
#define B_CTL_RB_MONITOR_MEM_ADDR2_L_H_0_REG          (B_CTL_RB_BASE + 0x96C)
#define B_CTL_RB_MONITOR_MEM_ADDR2_H_L_0_REG          (B_CTL_RB_BASE + 0x970)
#define B_CTL_RB_MONITOR_MEM_ADDR2_H_H_0_REG          (B_CTL_RB_BASE + 0x974)
#define B_CTL_RB_MONITOR_MEM_ADDR3_L_L_0_REG          (B_CTL_RB_BASE + 0x978)
#define B_CTL_RB_MONITOR_MEM_ADDR3_L_H_0_REG          (B_CTL_RB_BASE + 0x97C)
#define B_CTL_RB_MONITOR_MEM_ADDR3_H_L_0_REG          (B_CTL_RB_BASE + 0x980)
#define B_CTL_RB_MONITOR_MEM_ADDR3_H_H_0_REG          (B_CTL_RB_BASE + 0x984)
#define B_CTL_RB_AHB_MONITOR_EN_1_REG                 (B_CTL_RB_BASE + 0x988) /* 内存监控开关寄存器 */
#define B_CTL_RB_ACCESS_MEM_INT_1_REG                 (B_CTL_RB_BASE + 0x98C) /* 内存监控一级中断状态查询寄存器 */
#define B_CTL_RB_ACCESS_MEM_INT_MASK_1_REG            (B_CTL_RB_BASE + 0x990)
#define B_CTL_RB_ACCESS_MEM_INT_STS_1_REG             (B_CTL_RB_BASE + 0x994)
#define B_CTL_RB_ACCESS_MEM_INT_CLR_1_REG             (B_CTL_RB_BASE + 0x998)
#define B_CTL_RB_REC_ADDR_L_1_REG                     (B_CTL_RB_BASE + 0x99C)
#define B_CTL_RB_REC_ADDR_H_1_REG                     (B_CTL_RB_BASE + 0x9A0)
#define B_CTL_RB_REC_ID_1_REG                         (B_CTL_RB_BASE + 0x9A4)
#define B_CTL_RB_MONITOR_MEM_ADDR_EN_1_REG            (B_CTL_RB_BASE + 0x9A8)
#define B_CTL_RB_PKT_MONITOR_INT_DLY_1_REG            (B_CTL_RB_BASE + 0x9AC)
#define B_CTL_RB_MONITOR_MEM_ADDR0_L_L_1_REG          (B_CTL_RB_BASE + 0x9B0)
#define B_CTL_RB_MONITOR_MEM_ADDR0_L_H_1_REG          (B_CTL_RB_BASE + 0x9B4)
#define B_CTL_RB_MONITOR_MEM_ADDR0_H_L_1_REG          (B_CTL_RB_BASE + 0x9B8)
#define B_CTL_RB_MONITOR_MEM_ADDR0_H_H_1_REG          (B_CTL_RB_BASE + 0x9BC)
#define B_CTL_RB_MONITOR_MEM_ADDR1_L_L_1_REG          (B_CTL_RB_BASE + 0x9C0)
#define B_CTL_RB_MONITOR_MEM_ADDR1_L_H_1_REG          (B_CTL_RB_BASE + 0x9C4)
#define B_CTL_RB_MONITOR_MEM_ADDR1_H_L_1_REG          (B_CTL_RB_BASE + 0x9C8)
#define B_CTL_RB_MONITOR_MEM_ADDR1_H_H_1_REG          (B_CTL_RB_BASE + 0x9CC)
#define B_CTL_RB_MONITOR_MEM_ADDR2_L_L_1_REG          (B_CTL_RB_BASE + 0x9D0)
#define B_CTL_RB_MONITOR_MEM_ADDR2_L_H_1_REG          (B_CTL_RB_BASE + 0x9D4)
#define B_CTL_RB_MONITOR_MEM_ADDR2_H_L_1_REG          (B_CTL_RB_BASE + 0x9D8)
#define B_CTL_RB_MONITOR_MEM_ADDR2_H_H_1_REG          (B_CTL_RB_BASE + 0x9DC)
#define B_CTL_RB_MONITOR_MEM_ADDR3_L_L_1_REG          (B_CTL_RB_BASE + 0x9E0)
#define B_CTL_RB_MONITOR_MEM_ADDR3_L_H_1_REG          (B_CTL_RB_BASE + 0x9E4)
#define B_CTL_RB_MONITOR_MEM_ADDR3_H_L_1_REG          (B_CTL_RB_BASE + 0x9E8)
#define B_CTL_RB_MONITOR_MEM_ADDR3_H_H_1_REG          (B_CTL_RB_BASE + 0x9EC)
#define B_CTL_RB_BT_H2H_SEL_REG                       (B_CTL_RB_BASE + 0xA50) /* BT SUB H2H桥的选择 */
#define B_CTL_RB_BT_DMA_HRDATA_EN_BYPASS_REG          (B_CTL_RB_BASE + 0xA54) /* DMA的CG选择 */
#define B_CTL_RB_BSOC_UART_CFG_REG                    (B_CTL_RB_BASE + 0xA58) /* UART配置 */
#define B_CTL_RB_SYS_TICK_CFG_BSOC_REG                (B_CTL_RB_BASE + 0xA60)
#define B_CTL_RB_SYS_TICK_VALUE_BSOC_0_REG            (B_CTL_RB_BASE + 0xA64)
#define B_CTL_RB_SYS_TICK_VALUE_BSOC_2_REG            (B_CTL_RB_BASE + 0xA68)
#define B_CTL_RB_BSUB_TRIM_WORK_0_REG                 (B_CTL_RB_BASE + 0xA94)
#define B_CTL_RB_BSUB_TRIM_WORK_1_REG                 (B_CTL_RB_BASE + 0xA98)
#define B_CTL_RB_BSUB_TRIM_IDLE_0_REG                 (B_CTL_RB_BASE + 0xA9C)
#define B_CTL_RB_BSUB_TRIM_IDLE_1_REG                 (B_CTL_RB_BASE + 0xAA0)
#define B_CTL_RB_BSUB_TRIM_SEL_MAN_REG                (B_CTL_RB_BASE + 0xAA4)

#endif // __3322_B_CTL_RB_REG_OFFSET_H__
