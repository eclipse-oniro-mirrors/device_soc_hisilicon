/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File name     :  3322_glb_ctl_m_rb_reg_offset.h
 * Project line  :  Platform And Key Technologies Development
 * Department    :  CAD Development Department
 * Author        :  l00444884
 * Version       :  1.0
 * Date          :  2025/10/09
 * Description   :  The description of xxx project
 * Others        :  Generated automatically by nManager V5.1
 * History       :  l00444884 2025/03/05 17:25:13 Create file
 */

#ifndef __3322_GLB_CTL_M_RB_REG_OFFSET_H__
#define __3322_GLB_CTL_M_RB_REG_OFFSET_H__

/* GLB_CTL_M_RB Base address of Module's Register */
#define GLB_CTL_M_RB_BASE                       (0x57000000)

/******************************************************************************/
/*                      GLB_CTL_M_RB Registers' Definitions                            */
/******************************************************************************/

#define GLB_CTL_M_RB_GLB_CTL_M_ID_REG                     (GLB_CTL_M_RB_BASE + 0x0)   /* GLB M CTL ID寄存器 */
#define GLB_CTL_M_RB_CFG_M_PC_L_REG                       (GLB_CTL_M_RB_BASE + 0x4)   /* MCPU pc指针地址低16bit */
#define GLB_CTL_M_RB_CFG_M_PC_H_REG                       (GLB_CTL_M_RB_BASE + 0x8)   /* MCPU pc指针地址高16bit */
#define GLB_CTL_M_RB_GLB_CTL_CHIP_NAME_REG                (GLB_CTL_M_RB_BASE + 0xC)   /* GLB_CTL_CHIP_NAME寄存器 */
#define GLB_CTL_M_RB_GLB_GP_M_REG0_REG                    (GLB_CTL_M_RB_BASE + 0x10)  /* 通用寄存器 */
#define GLB_CTL_M_RB_GLB_GP_M_REG1_REG                    (GLB_CTL_M_RB_BASE + 0x14)  /* 通用寄存器 */
#define GLB_CTL_M_RB_GLB_GP_M_REG2_REG                    (GLB_CTL_M_RB_BASE + 0x18)  /* 通用寄存器 */
#define GLB_CTL_M_RB_GLB_GP_M_REG3_REG                    (GLB_CTL_M_RB_BASE + 0x1C)  /* 通用寄存器 */
#define GLB_CTL_M_RB_CFG_S_PC_L_REG                       (GLB_CTL_M_RB_BASE + 0x20)  /* SCPU pc指针地址低16bit */
#define GLB_CTL_M_RB_CFG_S_PC_H_REG                       (GLB_CTL_M_RB_BASE + 0x24)  /* SCPU pc指针地址高16bit */
#define GLB_CTL_M_RB_CFG_G_PC_L_REG                       (GLB_CTL_M_RB_BASE + 0x28)  /* GCPU pc指针地址低16bit */
#define GLB_CTL_M_RB_CFG_G_PC_H_REG                       (GLB_CTL_M_RB_BASE + 0x2C)  /* GCPU pc指针地址高16bit */
#define GLB_CTL_M_RB_B_SYS_RST_STS_REG                    (GLB_CTL_M_RB_BASE + 0x30)  /* 通用寄存器 */
#define GLB_CTL_M_RB_M_SYS_RST_STS_REG                    (GLB_CTL_M_RB_BASE + 0x34)  /* 通用寄存器 */
#define GLB_CTL_M_RB_D_SYS_RST_STS_REG                    (GLB_CTL_M_RB_BASE + 0x38)  /* 通用寄存器 */
#define GLB_CTL_M_RB_SYS_RST_STICK_CLR_REG                (GLB_CTL_M_RB_BASE + 0x3C)
#define GLB_CTL_M_RB_S_SYS_RST_STS_REG                    (GLB_CTL_M_RB_BASE + 0x40)  /* 通用寄存器 */
#define GLB_CTL_M_RB_G_SYS_RST_STS_REG                    (GLB_CTL_M_RB_BASE + 0x44)  /* 通用寄存器 */
#define GLB_CTL_M_RB_CFG_LOCK_REG                         (GLB_CTL_M_RB_BASE + 0x48)
#define GLB_CTL_M_RB_LOCK_VAL0_REG                        (GLB_CTL_M_RB_BASE + 0x4C)
#define GLB_CTL_M_RB_LOCK_VAL1_REG                        (GLB_CTL_M_RB_BASE + 0x50)
#define GLB_CTL_M_RB_LOCK_VAL2_REG                        (GLB_CTL_M_RB_BASE + 0x54)
#define GLB_CTL_M_RB_LOCK_VAL3_REG                        (GLB_CTL_M_RB_BASE + 0x58)
#define GLB_CTL_M_RB_CLK_1M_DIV_REG                       (GLB_CTL_M_RB_BASE + 0x60)
#define GLB_CTL_M_RB_CLK_32K_DIV_CFG_REG                  (GLB_CTL_M_RB_BASE + 0x64)
#define GLB_CTL_M_RB_CLK_32K_32M_EN_SEL_REG               (GLB_CTL_M_RB_BASE + 0x68)
#define GLB_CTL_M_RB_AON_CRG_CLK_CFG_0_REG                (GLB_CTL_M_RB_BASE + 0x6C)
#define GLB_CTL_M_RB_RST_PULSE0_REG                       (GLB_CTL_M_RB_BASE + 0x70)
#define GLB_CTL_M_RB_DFT_TOP_REG                          (GLB_CTL_M_RB_BASE + 0x74)
#define GLB_CTL_M_RB_USB_VBUS_CFG_REG                     (GLB_CTL_M_RB_BASE + 0x78)
#define GLB_CTL_M_RB_CLB_CLKEN0_REG                       (GLB_CTL_M_RB_BASE + 0x80)  /* 时钟门控 */
#define GLB_CTL_M_RB_CLB_CLKEN1_REG                       (GLB_CTL_M_RB_BASE + 0x84)  /* 时钟门控 */
#define GLB_CTL_M_RB_CLB_CLKSEL0_REG                      (GLB_CTL_M_RB_BASE + 0x88)  /* 时钟门控 */
#define GLB_CTL_M_RB_HI6535_PWR_HOLD_REG                  (GLB_CTL_M_RB_BASE + 0x8C)  /* Hi6535_pwr_hold */
#define GLB_CTL_M_RB_HI6535_PWR_HOLD_HW_SEL_REG           (GLB_CTL_M_RB_BASE + 0x90)  /* Hi6535_pwr_hold_hw_sel */
#define GLB_CTL_M_RB_FLASH_PWR_ON_LABEL_REG               (GLB_CTL_M_RB_BASE + 0x94)  /* FLASH_PWR_ON_LABEL */
#define GLB_CTL_M_RB_GLB_CRG_CLKEN_REG                    (GLB_CTL_M_RB_BASE + 0xA0)
#define GLB_CTL_M_RB_AON_BUS_DIV_REG                      (GLB_CTL_M_RB_BASE + 0xA8)
#define GLB_CTL_M_RB_LP_CLK_STM_DIV_REG                   (GLB_CTL_M_RB_BASE + 0xAC)
#define GLB_CTL_M_RB_SOFT_RST_0_REG                       (GLB_CTL_M_RB_BASE + 0xB0)  /* 软复位寄存器 */
#define GLB_CTL_M_RB_SOFT_RST_1_REG                       (GLB_CTL_M_RB_BASE + 0xB4)  /* 软复位寄存器 */
#define GLB_CTL_M_RB_SOFT_RST_2_REG                       (GLB_CTL_M_RB_BASE + 0xB8)  /* 软复位寄存器 */
#define GLB_CTL_M_RB_SOFT_RST_3_REG                       (GLB_CTL_M_RB_BASE + 0xBC)  /* 软复位寄存器 */
#define GLB_CTL_M_RB_SOFT_RST_4_REG                       (GLB_CTL_M_RB_BASE + 0xC0)  /* 软复位寄存器 */
#define GLB_CTL_M_RB_SOFT_RST_5_REG                       (GLB_CTL_M_RB_BASE + 0xC4)  /* 软复位寄存器 */
#define GLB_CTL_M_RB_SOFT_RST_6_REG                       (GLB_CTL_M_RB_BASE + 0xC8)  /* 软复位寄存器 */
#define GLB_CTL_M_RB_SOFT_RST_7_REG                       (GLB_CTL_M_RB_BASE + 0xCC)  /* 软复位寄存器 */
#define GLB_CTL_M_RB_DFT_CTL0_REG                         (GLB_CTL_M_RB_BASE + 0xD0)  /* DFT控制 */
#define GLB_CTL_M_RB_DFT_CTL1_REG                         (GLB_CTL_M_RB_BASE + 0xD4)  /* DFT控制 */
#define GLB_CTL_M_RB_DFT_CTL2_REG                         (GLB_CTL_M_RB_BASE + 0xD8)  /* DFT控制 */
#define GLB_CTL_M_RB_DFT_CTL3_REG                         (GLB_CTL_M_RB_BASE + 0xDC)  /* DFT控制 */
#define GLB_CTL_M_RB_DFT_CFG0_REG                         (GLB_CTL_M_RB_BASE + 0xE0)  /* DFT控制 */
#define GLB_CTL_M_RB_MBUS_CLKEN_NOR_REG                   (GLB_CTL_M_RB_BASE + 0xF0)
#define GLB_CTL_M_RB_SOFT_RST_10_REG                      (GLB_CTL_M_RB_BASE + 0xFC)  /* 软复位寄存器 */
#define GLB_CTL_M_RB_ULP_AON_AUTO_CG_REG                  (GLB_CTL_M_RB_BASE + 0x100) /* 总线自动CG控制 */
#define GLB_CTL_M_RB_MEM_POWER_RET1N_REG                  (GLB_CTL_M_RB_BASE + 0x10C)
#define GLB_CTL_M_RB_AON_WDT_CORE_WAIT_REG                (GLB_CTL_M_RB_BASE + 0x110)
#define GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_0_REG           (GLB_CTL_M_RB_BASE + 0x118)
#define GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_1_REG           (GLB_CTL_M_RB_BASE + 0x11C)
#define GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_2_REG           (GLB_CTL_M_RB_BASE + 0x120)
#define GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_3_REG           (GLB_CTL_M_RB_BASE + 0x124)
#define GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_4_REG           (GLB_CTL_M_RB_BASE + 0x128)
#define GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_5_REG           (GLB_CTL_M_RB_BASE + 0x12C)
#define GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_6_REG           (GLB_CTL_M_RB_BASE + 0x130)
#define GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_7_REG           (GLB_CTL_M_RB_BASE + 0x134)
#define GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_8_REG           (GLB_CTL_M_RB_BASE + 0x138)
#define GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_9_REG           (GLB_CTL_M_RB_BASE + 0x13C)
#define GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_10_REG          (GLB_CTL_M_RB_BASE + 0x140)
#define GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_11_REG          (GLB_CTL_M_RB_BASE + 0x144)
#define GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_12_REG          (GLB_CTL_M_RB_BASE + 0x148)
#define GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_13_REG          (GLB_CTL_M_RB_BASE + 0x14C)
#define GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_14_REG          (GLB_CTL_M_RB_BASE + 0x150)
#define GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_15_REG          (GLB_CTL_M_RB_BASE + 0x154)
#define GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_16_REG          (GLB_CTL_M_RB_BASE + 0x158)
#define GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_17_REG          (GLB_CTL_M_RB_BASE + 0x15C)
#define GLB_CTL_M_RB_GPIO_CTL_REG                         (GLB_CTL_M_RB_BASE + 0x160) /* GPIO时钟使能和软复位 */
#define GLB_CTL_M_RB_RTC_CTL_REG                          (GLB_CTL_M_RB_BASE + 0x164) /* RTC时钟使能和软复位 */
#define GLB_CTL_M_RB_WDT_CTL_REG                          (GLB_CTL_M_RB_BASE + 0x168) /* WDT控制信号 */
#define GLB_CTL_M_RB_GPIO_RTC_CURR_REG                    (GLB_CTL_M_RB_BASE + 0x16C) /* GPIO和RTC的状态观测 */
#define GLB_CTL_M_RB_PINMUX_AON_GPIO_SEL_0_REG            (GLB_CTL_M_RB_BASE + 0x170)
#define GLB_CTL_M_RB_PINMUX_AON_GPIO_SEL_1_REG            (GLB_CTL_M_RB_BASE + 0x174)
#define GLB_CTL_M_RB_PINMUX_AON_GPIO_SEL_2_REG            (GLB_CTL_M_RB_BASE + 0x178)
#define GLB_CTL_M_RB_NPU_CTL_REG                          (GLB_CTL_M_RB_BASE + 0x190)
#define GLB_CTL_M_RB_NPU_INTR_EN_REG                      (GLB_CTL_M_RB_BASE + 0x194)
#define GLB_CTL_M_RB_LP_REG_CTL_REG                       (GLB_CTL_M_RB_BASE + 0x198)
#define GLB_CTL_M_RB_SEM0_STS_REG                         (GLB_CTL_M_RB_BASE + 0x19C) /* SEM控制 */
#define GLB_CTL_M_RB_SEM0_FORCE_CLR_REG                   (GLB_CTL_M_RB_BASE + 0x1A0) /* SEM控制 */
#define GLB_CTL_M_RB_SEM1_STS_REG                         (GLB_CTL_M_RB_BASE + 0x1A4) /* SEM控制 */
#define GLB_CTL_M_RB_SEM1_FORCE_CLR_REG                   (GLB_CTL_M_RB_BASE + 0x1A8) /* SEM控制 */
#define GLB_CTL_M_RB_SEM2_STS_REG                         (GLB_CTL_M_RB_BASE + 0x1AC) /* SEM控制 */
#define GLB_CTL_M_RB_SEM2_FORCE_CLR_REG                   (GLB_CTL_M_RB_BASE + 0x1B0) /* SEM控制 */
#define GLB_CTL_M_RB_SEM3_STS_REG                         (GLB_CTL_M_RB_BASE + 0x1B4) /* SEM控制 */
#define GLB_CTL_M_RB_SEM3_FORCE_CLR_REG                   (GLB_CTL_M_RB_BASE + 0x1B8) /* SEM控制 */
#define GLB_CTL_M_RB_SEM4_STS_REG                         (GLB_CTL_M_RB_BASE + 0x1BC) /* SEM控制 */
#define GLB_CTL_M_RB_SEM4_FORCE_CLR_REG                   (GLB_CTL_M_RB_BASE + 0x1C0) /* SEM控制 */
#define GLB_CTL_M_RB_SEM5_STS_REG                         (GLB_CTL_M_RB_BASE + 0x1C4) /* SEM控制 */
#define GLB_CTL_M_RB_SEM5_FORCE_CLR_REG                   (GLB_CTL_M_RB_BASE + 0x1C8) /* SEM控制 */
#define GLB_CTL_M_RB_SEM6_STS_REG                         (GLB_CTL_M_RB_BASE + 0x1CC) /* SEM控制 */
#define GLB_CTL_M_RB_SEM6_FORCE_CLR_REG                   (GLB_CTL_M_RB_BASE + 0x1D0) /* SEM控制 */
#define GLB_CTL_M_RB_SEM7_STS_REG                         (GLB_CTL_M_RB_BASE + 0x1D4) /* SEM控制 */
#define GLB_CTL_M_RB_SEM7_FORCE_CLR_REG                   (GLB_CTL_M_RB_BASE + 0x1D8) /* SEM控制 */
#define GLB_CTL_M_RB_PWM_CTRL_REG                         (GLB_CTL_M_RB_BASE + 0x1E0)
#define GLB_CTL_M_RB_DAP_ID_L_REG                         (GLB_CTL_M_RB_BASE + 0x1F8)
#define GLB_CTL_M_RB_DAP_ID_H_REG                         (GLB_CTL_M_RB_BASE + 0x1FC)
#define GLB_CTL_M_RB_SYS_ABNORMAL_STATUS0_REG             (GLB_CTL_M_RB_BASE + 0x200) /* PMU状态查询 */
#define GLB_CTL_M_RB_SYS_SOFT_WDT_RST_REG                 (GLB_CTL_M_RB_BASE + 0x204) /* 通用寄存器 */
#define GLB_CTL_M_RB_SYS_SOFT_RST_WIDTH_REG               (GLB_CTL_M_RB_BASE + 0x208) /* 通用寄存器 */
#define GLB_CTL_M_RB_MEM_AUTO_PD_CFG_REG                  (GLB_CTL_M_RB_BASE + 0x20C) /* 通用寄存器 */
#define GLB_CTL_M_RB_UART_H_MODE_CFG_REG                  (GLB_CTL_M_RB_BASE + 0x210) /* 通用寄存器 */
#define GLB_CTL_M_RB_AUDIO_INT_MASK_REG                   (GLB_CTL_M_RB_BASE + 0x214)
#define GLB_CTL_M_RB_CFG_S_AGPIO0_CTRL_IE_0_REG           (GLB_CTL_M_RB_BASE + 0x218)
#define GLB_CTL_M_RB_CFG_S_AGPIO0_CTRL_IE_1_REG           (GLB_CTL_M_RB_BASE + 0x21C)
#define GLB_CTL_M_RB_CFG_S_AGPIO0_CTRL_IE_2_REG           (GLB_CTL_M_RB_BASE + 0x220)
#define GLB_CTL_M_RB_CFG_S_HGPIO0_CTRL_IE_0_REG           (GLB_CTL_M_RB_BASE + 0x224)
#define GLB_CTL_M_RB_CFG_S_MGPIO0_CTRL_IE_0_REG           (GLB_CTL_M_RB_BASE + 0x22C)
#define GLB_CTL_M_RB_CFG_S_MGPIO0_CTRL_IE_1_REG           (GLB_CTL_M_RB_BASE + 0x230)
#define GLB_CTL_M_RB_CFG_S_EGPIO0_CTRL_IE_0_REG           (GLB_CTL_M_RB_BASE + 0x234)
#define GLB_CTL_M_RB_CFG_S_EGPIO0_CTRL_IE_1_REG           (GLB_CTL_M_RB_BASE + 0x238)
#define GLB_CTL_M_RB_SSI2RF_DIV_REG                       (GLB_CTL_M_RB_BASE + 0x250)
#define GLB_CTL_M_RB_SSI2RF_RST_REG                       (GLB_CTL_M_RB_BASE + 0x254)
#define GLB_CTL_M_RB_PINMUX_DEBUG_MODE_REG                (GLB_CTL_M_RB_BASE + 0x260)
#define GLB_CTL_M_RB_32K_DET_VAL_REG                      (GLB_CTL_M_RB_BASE + 0x264)
#define GLB_CTL_M_RB_32K_CALI_CFG_REG                     (GLB_CTL_M_RB_BASE + 0x268)
#define GLB_CTL_M_RB_CLK_32K_GOLD_RES_LOW_LIMIT_L_REG     (GLB_CTL_M_RB_BASE + 0x26C)
#define GLB_CTL_M_RB_CLK_32K_GOLD_RES_LOW_LIMIT_H_REG     (GLB_CTL_M_RB_BASE + 0x270)
#define GLB_CTL_M_RB_CLK_32K_GOLD_RES_HIGH_LIMIT_L_REG    (GLB_CTL_M_RB_BASE + 0x274)
#define GLB_CTL_M_RB_CLK_32K_GOLD_RES_HIGH_LIMIT_H_REG    (GLB_CTL_M_RB_BASE + 0x278)
#define GLB_CTL_M_RB_32K_DET_CFG_REG                      (GLB_CTL_M_RB_BASE + 0x27C)
#define GLB_CTL_M_RB_CALI_RC_32M_PAD_CLKIN_CTL_REG        (GLB_CTL_M_RB_BASE + 0x280) /* RC_32M校PAD_CLKIN时钟校准寄存器 */
#define GLB_CTL_M_RB_CALI_RC_32M_PAD_CLKIN_CNT_L_REG      (GLB_CTL_M_RB_BASE + 0x284) /* RC_32M校PAD_CLKIN时钟校准寄存器 */
#define GLB_CTL_M_RB_CALI_RC_32M_PAD_CLKIN_CNT_H_REG      (GLB_CTL_M_RB_BASE + 0x288) /* RC_32M校PAD_CLKIN时钟校准寄存器 */
#define GLB_CTL_M_RB_CALI_RC_32M_PAD_CLKIN_RESULT_L_REG   (GLB_CTL_M_RB_BASE + 0x28C) /* RC_32M校PAD_CLKIN时钟校准寄存器 */
#define GLB_CTL_M_RB_CALI_RC_32M_PAD_CLKINO_RESULT_H_REG  (GLB_CTL_M_RB_BASE + 0x290) /* RC_32M校PAD_CLKIN时钟校准寄存器 */
#define GLB_CTL_M_RB_VIDEO_TE_CTL_REG                     (GLB_CTL_M_RB_BASE + 0x294)
#define GLB_CTL_M_RB_TE_INT_START_CNT_VAL_REG             (GLB_CTL_M_RB_BASE + 0x298)
#define GLB_CTL_M_RB_TE_INT_STOP_CNT_VAL_REG              (GLB_CTL_M_RB_BASE + 0x29C)
#define GLB_CTL_M_RB_MCU_INT_START_CNT_VAL_REG            (GLB_CTL_M_RB_BASE + 0x2A0)
#define GLB_CTL_M_RB_MCU_INT_STOP_CNT_VAL_REG             (GLB_CTL_M_RB_BASE + 0x2A4)
#define GLB_CTL_M_RB_TE_CNT_CTL_REG                       (GLB_CTL_M_RB_BASE + 0x2A8)
#define GLB_CTL_M_RB_AON_DIAG_SEL_REG                     (GLB_CTL_M_RB_BASE + 0x2AC)
#define GLB_CTL_M_RB_AUDIO_RAM_TMOD_0_REG                 (GLB_CTL_M_RB_BASE + 0x2B0)
#define GLB_CTL_M_RB_AUDIO_RAM_TMOD_1_REG                 (GLB_CTL_M_RB_BASE + 0x2B4)
#define GLB_CTL_M_RB_AUDIO_RAM_TMOD_2_REG                 (GLB_CTL_M_RB_BASE + 0x2B8)
#define GLB_CTL_M_RB_AUDIO_RAM_TMOD_3_REG                 (GLB_CTL_M_RB_BASE + 0x2BC)
#define GLB_CTL_M_RB_AUDIO_ROM_CTL_REG                    (GLB_CTL_M_RB_BASE + 0x2C0)
#define GLB_CTL_M_RB_BT_RAM_TMOD_0_REG                    (GLB_CTL_M_RB_BASE + 0x2C4)
#define GLB_CTL_M_RB_BT_RAM_TMOD_1_REG                    (GLB_CTL_M_RB_BASE + 0x2C8)
#define GLB_CTL_M_RB_BT_RAM_TMOD_2_REG                    (GLB_CTL_M_RB_BASE + 0x2CC)
#define GLB_CTL_M_RB_BT_RAM_TMOD_3_REG                    (GLB_CTL_M_RB_BASE + 0x2D0)
#define GLB_CTL_M_RB_BT_ROM_CTL_REG                       (GLB_CTL_M_RB_BASE + 0x2D4)
#define GLB_CTL_M_RB_COM_RAM_TMOD_0_REG                   (GLB_CTL_M_RB_BASE + 0x2D8)
#define GLB_CTL_M_RB_COM_RAM_TMOD_1_REG                   (GLB_CTL_M_RB_BASE + 0x2DC)
#define GLB_CTL_M_RB_COM_RAM_TMOD_2_REG                   (GLB_CTL_M_RB_BASE + 0x2E0)
#define GLB_CTL_M_RB_COM_RAM_TMOD_3_REG                   (GLB_CTL_M_RB_BASE + 0x2E4)
#define GLB_CTL_M_RB_MCU_RAM_TMOD_0_REG                   (GLB_CTL_M_RB_BASE + 0x2E8)
#define GLB_CTL_M_RB_MCU_RAM_TMOD_1_REG                   (GLB_CTL_M_RB_BASE + 0x2EC)
#define GLB_CTL_M_RB_MCU_RAM_TMOD_2_REG                   (GLB_CTL_M_RB_BASE + 0x2F0)
#define GLB_CTL_M_RB_MCU_RAM_TMOD_3_REG                   (GLB_CTL_M_RB_BASE + 0x2F4)
#define GLB_CTL_M_RB_MCU_ROM_CTL_REG                      (GLB_CTL_M_RB_BASE + 0x2F8)
#define GLB_CTL_M_RB_MEM_RAM_TMOD_0_REG                   (GLB_CTL_M_RB_BASE + 0x2FC)
#define GLB_CTL_M_RB_MEM_RAM_TMOD_1_REG                   (GLB_CTL_M_RB_BASE + 0x300)
#define GLB_CTL_M_RB_MEM_RAM_TMOD_2_REG                   (GLB_CTL_M_RB_BASE + 0x304)
#define GLB_CTL_M_RB_MEM_RAM_TMOD_3_REG                   (GLB_CTL_M_RB_BASE + 0x308)
#define GLB_CTL_M_RB_CFG_DVFS_XO_CORE_RC_PD_WKUP_TIME_REG (GLB_CTL_M_RB_BASE + 0x310)
#define GLB_CTL_M_RB_CFG_DVFS_XO_CORE_RSTN_WKUP_TIME_REG  (GLB_CTL_M_RB_BASE + 0x314)
#define GLB_CTL_M_RB_CFG_DVFS_XO_CORE_CLKEN_WKUP_TIME_REG (GLB_CTL_M_RB_BASE + 0x318)
#define GLB_CTL_M_RB_CFG_DVFS_XO_SLP_TIME_1_REG           (GLB_CTL_M_RB_BASE + 0x31C)
#define GLB_CTL_M_RB_CFG_DVFS_XO_SLP_TIME_2_REG           (GLB_CTL_M_RB_BASE + 0x320)
#define GLB_CTL_M_RB_CFG_DVFS_RC_RSTN_WKUP_TIME_REG       (GLB_CTL_M_RB_BASE + 0x330)
#define GLB_CTL_M_RB_CFG_DVFS_RC_SLP_TIME_REG             (GLB_CTL_M_RB_BASE + 0x334)
#define GLB_CTL_M_RB_CFG_DVFS_STS_REG                     (GLB_CTL_M_RB_BASE + 0x338)
#define GLB_CTL_M_RB_CFG_LP_DVFS_WKUP_TIME_REG            (GLB_CTL_M_RB_BASE + 0x33C)
#define GLB_CTL_M_RB_CFG_LP_DVFS_SLP_TIME_REG             (GLB_CTL_M_RB_BASE + 0x340)
#define GLB_CTL_M_RB_NPU_SP_RAM_TMOD_0_REG                (GLB_CTL_M_RB_BASE + 0x344)
#define GLB_CTL_M_RB_NPU_SP_RAM_TMOD_1_REG                (GLB_CTL_M_RB_BASE + 0x348)
#define GLB_CTL_M_RB_NPU_SP_RAM_TMOD_2_REG                (GLB_CTL_M_RB_BASE + 0x34C)
#define GLB_CTL_M_RB_NPU_SP_RAM_TMOD_3_REG                (GLB_CTL_M_RB_BASE + 0x350)
#define GLB_CTL_M_RB_NPU_TP_RAM_TMOD_0_REG                (GLB_CTL_M_RB_BASE + 0x354)
#define GLB_CTL_M_RB_NPU_TP_RAM_TMOD_1_REG                (GLB_CTL_M_RB_BASE + 0x358)
#define GLB_CTL_M_RB_MEM_TP_RAM_TMOD_0_REG                (GLB_CTL_M_RB_BASE + 0x35C)
#define GLB_CTL_M_RB_MEM_TP_RAM_TMOD_1_REG                (GLB_CTL_M_RB_BASE + 0x360)
#define GLB_CTL_M_RB_BUCK_OPT_CTRL0_REG                   (GLB_CTL_M_RB_BASE + 0x364)
#define GLB_CTL_M_RB_BUCK_OPT_CTRL1_REG                   (GLB_CTL_M_RB_BASE + 0x368)
#define GLB_CTL_M_RB_BUCK_OPT_CTRL2_REG                   (GLB_CTL_M_RB_BASE + 0x36C)
#define GLB_CTL_M_RB_BUCK_OPT_CTRL3_REG                   (GLB_CTL_M_RB_BASE + 0x370)
#define GLB_CTL_M_RB_DIAG_CTL_REG                         (GLB_CTL_M_RB_BASE + 0x37C)
#define GLB_CTL_M_RB_GCPU_PC_LR_LOAD_REG                  (GLB_CTL_M_RB_BASE + 0x380)
#define GLB_CTL_M_RB_GCPU_PC_CLR_EN_REG                   (GLB_CTL_M_RB_BASE + 0x384)
#define GLB_CTL_M_RB_GCPU_PC_L_REG                        (GLB_CTL_M_RB_BASE + 0x388)
#define GLB_CTL_M_RB_GCPU_PC_H_REG                        (GLB_CTL_M_RB_BASE + 0x38C)
#define GLB_CTL_M_RB_GCPU_LR_L_REG                        (GLB_CTL_M_RB_BASE + 0x390)
#define GLB_CTL_M_RB_GCPU_LR_H_REG                        (GLB_CTL_M_RB_BASE + 0x394)
#define GLB_CTL_M_RB_GCPU_SP_L_REG                        (GLB_CTL_M_RB_BASE + 0x398)
#define GLB_CTL_M_RB_GCPU_SP_H_REG                        (GLB_CTL_M_RB_BASE + 0x39C)
#define GLB_CTL_M_RB_MCPU_PC_LR_LOAD_REG                  (GLB_CTL_M_RB_BASE + 0x3A0)
#define GLB_CTL_M_RB_MCPU_PC_LR_ENABLE_REG                (GLB_CTL_M_RB_BASE + 0x3A4)
#define GLB_CTL_M_RB_MCPU_PC_L_REG                        (GLB_CTL_M_RB_BASE + 0x3A8)
#define GLB_CTL_M_RB_MCPU_PC_H_REG                        (GLB_CTL_M_RB_BASE + 0x3AC)
#define GLB_CTL_M_RB_MCPU_LR_L_REG                        (GLB_CTL_M_RB_BASE + 0x3B0)
#define GLB_CTL_M_RB_MCPU_LR_H_REG                        (GLB_CTL_M_RB_BASE + 0x3B4)
#define GLB_CTL_M_RB_MCPU_SP_L_REG                        (GLB_CTL_M_RB_BASE + 0x3B8)
#define GLB_CTL_M_RB_MCPU_SP_H_REG                        (GLB_CTL_M_RB_BASE + 0x3BC)
#define GLB_CTL_M_RB_DAP_LINK_REG_REG                     (GLB_CTL_M_RB_BASE + 0x3C0)
#define GLB_CTL_M_RB_DAP_LINK_LOCK_REG                    (GLB_CTL_M_RB_BASE + 0x3D0)
#define GLB_CTL_M_RB_SYS_TICK_CFG_M_REG                   (GLB_CTL_M_RB_BASE + 0x3E4)
#define GLB_CTL_M_RB_SYS_TICK_VALUE_M_0_REG               (GLB_CTL_M_RB_BASE + 0x3E8)
#define GLB_CTL_M_RB_SYS_TICK_VALUE_M_1_REG               (GLB_CTL_M_RB_BASE + 0x3EC)
#define GLB_CTL_M_RB_SYS_TICK_VALUE_M_2_REG               (GLB_CTL_M_RB_BASE + 0x3F0)
#define GLB_CTL_M_RB_SYS_TICK_VALUE_M_3_REG               (GLB_CTL_M_RB_BASE + 0x3F4)

#endif // __3322_GLB_CTL_M_RB_REG_OFFSET_H__
