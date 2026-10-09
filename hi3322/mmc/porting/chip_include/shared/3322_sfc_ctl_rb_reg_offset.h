/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File name     :  3322_sfc_ctl_rb_reg_offset.h
 * Project line  :  Platform And Key Technologies Development
 * Department    :  CAD Development Department
 * Author        :  l00444884
 * Version       :  1.0
 * Date          :  2025/10/09
 * Description   :  The description of xxx project
 * Others        :  Generated automatically by nManager V5.1
 * History       :  l00444884 2025/03/05 17:25:14 Create file
 */

#ifndef __3322_SFC_CTL_RB_REG_OFFSET_H__
#define __3322_SFC_CTL_RB_REG_OFFSET_H__

/* SFC_CTL_RB Base address of Module's Register */
#define SFC_CTL_RB_BASE                       (0x28000000)

/******************************************************************************/
/*                      SFC_CTL_RB Registers' Definitions                            */
/******************************************************************************/

#define SFC_CTL_RB_GLOBAL_CONFIG_REG              (SFC_CTL_RB_BASE + 0x100)  /* 全局配置寄存器。 */
#define SFC_CTL_RB_TIMING_REG                     (SFC_CTL_RB_BASE + 0x110)  /* Timing配置寄存器。 */
#define SFC_CTL_RB_INT_RAW_STATUS_REG             (SFC_CTL_RB_BASE + 0x120)  /* 中断原始状态寄存器。 */
#define SFC_CTL_RB_INT_STATUS_REG                 (SFC_CTL_RB_BASE + 0x124)  /* 经过屏蔽处理的中断状态寄存器。 */
#define SFC_CTL_RB_INT_MASK_REG                   (SFC_CTL_RB_BASE + 0x128)  /* 中断屏蔽寄存器。 */
#define SFC_CTL_RB_INT_CLEAR_REG                  (SFC_CTL_RB_BASE + 0x12C)  /* 中断清除寄存器。 */
#define SFC_CTL_RB_SOFT_RST_MASK_REG              (SFC_CTL_RB_BASE + 0x130)  /* 软复位寄存器屏蔽位 */
#define SFC_CTL_RB_VERSION_REG                    (SFC_CTL_RB_BASE + 0x1F8)  /* 版本寄存器。 */
#define SFC_CTL_RB_VERSION_SEL_REG                (SFC_CTL_RB_BASE + 0x1FC)  /* 版本选择寄存器。 */
#define SFC_CTL_RB_BUS_CONFIG1_REG                (SFC_CTL_RB_BASE + 0x200)  /* 总线操作方式配置1寄存器。 */
#define SFC_CTL_RB_BUS_CONFIG2_REG                (SFC_CTL_RB_BASE + 0x204)  /* 总线操作方式配置2寄存器。 */
#define SFC_CTL_RB_BUS_FLASH_SIZE_REG             (SFC_CTL_RB_BASE + 0x210)  /* 总线操作方式映射尺寸寄存器。 */
#define SFC_CTL_RB_BUS_BASE_ADDR_CS0_REG          (SFC_CTL_RB_BASE + 0x214)  /* 总线操作方式片选0映射基地址寄存器。 */
#define SFC_CTL_RB_BUS_BASE_ADDR_CS1_REG          (SFC_CTL_RB_BASE + 0x218)  /* 总线操作方式片选1映射基地址寄存器。 */
#define SFC_CTL_RB_BUS_ALIAS_ADDR_REG             (SFC_CTL_RB_BASE + 0x21C)  /* 总线操作方式Alias映射基地址寄存器。 */
#define SFC_CTL_RB_BUS_ALIAS_CS_REG               (SFC_CTL_RB_BASE + 0x220)  /* 总线操作方式Alias片选指示寄存器。 */
#define SFC_CTL_RB_BUS_DMA_CTRL_REG               (SFC_CTL_RB_BASE + 0x240)  /* DMA操作控制寄存器。 */
#define SFC_CTL_RB_BUS_DMA_MEM_SADDR_REG          (SFC_CTL_RB_BASE + 0x244)  /* DMA操作DDR起始地址寄存器。 */
#define SFC_CTL_RB_BUS_DMA_FLASH_SADDR_REG        (SFC_CTL_RB_BASE + 0x248)  /* DMA操作Flash起始地址寄存器。 */
#define SFC_CTL_RB_BUS_DMA_LEN_REG                (SFC_CTL_RB_BASE + 0x24C)  /* DMA操作搬运数据长度寄存器。 */
#define SFC_CTL_RB_BUS_DMA_AHB_CTRL_REG           (SFC_CTL_RB_BASE + 0x250)  /* DMA操作AHB时burst操作方式选择控制寄存器。 */
#define SFC_CTL_RB_CONTINU_READ STATUS_REG        (SFC_CTL_RB_BASE + 0x2A0)  /* 连续读模式状态指示寄存器 */
#define SFC_CTL_RB_M BYTE_REG                     (SFC_CTL_RB_BASE + 0x2A4)  /* 连续读模式M字节配置 */
#define SFC_CTL_RB_CMD_CONFIG_REG                 (SFC_CTL_RB_BASE + 0x300)  /* 命令操作方式配置寄存器。 */
#define SFC_CTL_RB_CMD_INS_REG                    (SFC_CTL_RB_BASE + 0x308)  /* 命令操作方式指令寄存器。 */
#define SFC_CTL_RB_CMD_ADDR_REG                   (SFC_CTL_RB_BASE + 0x30C)  /* 命令操作方式地址寄存器。 */
#define SFC_CTL_RB_CMD_DATABUF_N_0_REG            (SFC_CTL_RB_BASE + 0x400)  /* 命令操作方式数据Buffer寄存器。 */
#define SFC_CTL_RB_CMD_DATABUF_N_1_REG            (SFC_CTL_RB_BASE + 0x404)  /* 命令操作方式数据Buffer寄存器。 */
#define SFC_CTL_RB_CMD_DATABUF_N_2_REG            (SFC_CTL_RB_BASE + 0x408)  /* 命令操作方式数据Buffer寄存器。 */
#define SFC_CTL_RB_CMD_DATABUF_N_3_REG            (SFC_CTL_RB_BASE + 0x40C)  /* 命令操作方式数据Buffer寄存器。 */
#define SFC_CTL_RB_CMD_DATABUF_N_4_REG            (SFC_CTL_RB_BASE + 0x410)  /* 命令操作方式数据Buffer寄存器。 */
#define SFC_CTL_RB_CMD_DATABUF_N_5_REG            (SFC_CTL_RB_BASE + 0x414)  /* 命令操作方式数据Buffer寄存器。 */
#define SFC_CTL_RB_CMD_DATABUF_N_6_REG            (SFC_CTL_RB_BASE + 0x418)  /* 命令操作方式数据Buffer寄存器。 */
#define SFC_CTL_RB_CMD_DATABUF_N_7_REG            (SFC_CTL_RB_BASE + 0x41C)  /* 命令操作方式数据Buffer寄存器。 */
#define SFC_CTL_RB_CMD_DATABUF_N_8_REG            (SFC_CTL_RB_BASE + 0x420)  /* 命令操作方式数据Buffer寄存器。 */
#define SFC_CTL_RB_CMD_DATABUF_N_9_REG            (SFC_CTL_RB_BASE + 0x424)  /* 命令操作方式数据Buffer寄存器。 */
#define SFC_CTL_RB_CMD_DATABUF_N_10_REG           (SFC_CTL_RB_BASE + 0x428)  /* 命令操作方式数据Buffer寄存器。 */
#define SFC_CTL_RB_CMD_DATABUF_N_11_REG           (SFC_CTL_RB_BASE + 0x42C)  /* 命令操作方式数据Buffer寄存器。 */
#define SFC_CTL_RB_CMD_DATABUF_N_12_REG           (SFC_CTL_RB_BASE + 0x430)  /* 命令操作方式数据Buffer寄存器。 */
#define SFC_CTL_RB_CMD_DATABUF_N_13_REG           (SFC_CTL_RB_BASE + 0x434)  /* 命令操作方式数据Buffer寄存器。 */
#define SFC_CTL_RB_CMD_DATABUF_N_14_REG           (SFC_CTL_RB_BASE + 0x438)  /* 命令操作方式数据Buffer寄存器。 */
#define SFC_CTL_RB_CMD_DATABUF_N_15_REG           (SFC_CTL_RB_BASE + 0x43C)  /* 命令操作方式数据Buffer寄存器。 */
#define SFC_CTL_RB_TEECPU_LOCK_FLAG_REG           (SFC_CTL_RB_BASE + 0x800)  /* TEECPU锁定flash标记 */
#define SFC_CTL_RB_APC_CFG_START_ADDR_0_REG       (SFC_CTL_RB_BASE + 0x1000) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_START_ADDR_1_REG       (SFC_CTL_RB_BASE + 0x1004) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_START_ADDR_2_REG       (SFC_CTL_RB_BASE + 0x1008) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_START_ADDR_3_REG       (SFC_CTL_RB_BASE + 0x100C) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_START_ADDR_4_REG       (SFC_CTL_RB_BASE + 0x1010) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_START_ADDR_5_REG       (SFC_CTL_RB_BASE + 0x1014) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_START_ADDR_6_REG       (SFC_CTL_RB_BASE + 0x1018) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_START_ADDR_7_REG       (SFC_CTL_RB_BASE + 0x101C) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_START_ADDR_8_REG       (SFC_CTL_RB_BASE + 0x1020) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_START_ADDR_9_REG       (SFC_CTL_RB_BASE + 0x1024) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_START_ADDR_10_REG      (SFC_CTL_RB_BASE + 0x1028) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_START_ADDR_11_REG      (SFC_CTL_RB_BASE + 0x102C) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_START_ADDR_12_REG      (SFC_CTL_RB_BASE + 0x1030) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_START_ADDR_13_REG      (SFC_CTL_RB_BASE + 0x1034) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_START_ADDR_14_REG      (SFC_CTL_RB_BASE + 0x1038) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_START_ADDR_15_REG      (SFC_CTL_RB_BASE + 0x103C) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_END_ADDR_0_REG         (SFC_CTL_RB_BASE + 0x1040) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_END_ADDR_1_REG         (SFC_CTL_RB_BASE + 0x1044) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_END_ADDR_2_REG         (SFC_CTL_RB_BASE + 0x1048) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_END_ADDR_3_REG         (SFC_CTL_RB_BASE + 0x104C) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_END_ADDR_4_REG         (SFC_CTL_RB_BASE + 0x1050) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_END_ADDR_5_REG         (SFC_CTL_RB_BASE + 0x1054) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_END_ADDR_6_REG         (SFC_CTL_RB_BASE + 0x1058) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_END_ADDR_7_REG         (SFC_CTL_RB_BASE + 0x105C) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_END_ADDR_8_REG         (SFC_CTL_RB_BASE + 0x1060) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_END_ADDR_9_REG         (SFC_CTL_RB_BASE + 0x1064) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_END_ADDR_10_REG        (SFC_CTL_RB_BASE + 0x1068) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_END_ADDR_11_REG        (SFC_CTL_RB_BASE + 0x106C) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_END_ADDR_12_REG        (SFC_CTL_RB_BASE + 0x1070) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_END_ADDR_13_REG        (SFC_CTL_RB_BASE + 0x1074) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_END_ADDR_14_REG        (SFC_CTL_RB_BASE + 0x1078) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_END_ADDR_15_REG        (SFC_CTL_RB_BASE + 0x107C) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_RD_ID_0_REG      (SFC_CTL_RB_BASE + 0x1080) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_RD_ID_1_REG      (SFC_CTL_RB_BASE + 0x1084) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_RD_ID_2_REG      (SFC_CTL_RB_BASE + 0x1088) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_RD_ID_3_REG      (SFC_CTL_RB_BASE + 0x108C) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_RD_ID_4_REG      (SFC_CTL_RB_BASE + 0x1090) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_RD_ID_5_REG      (SFC_CTL_RB_BASE + 0x1094) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_RD_ID_6_REG      (SFC_CTL_RB_BASE + 0x1098) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_RD_ID_7_REG      (SFC_CTL_RB_BASE + 0x109C) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_RD_ID_8_REG      (SFC_CTL_RB_BASE + 0x10A0) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_RD_ID_9_REG      (SFC_CTL_RB_BASE + 0x10A4) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_RD_ID_10_REG     (SFC_CTL_RB_BASE + 0x10A8) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_RD_ID_11_REG     (SFC_CTL_RB_BASE + 0x10AC) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_RD_ID_12_REG     (SFC_CTL_RB_BASE + 0x10B0) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_RD_ID_13_REG     (SFC_CTL_RB_BASE + 0x10B4) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_RD_ID_14_REG     (SFC_CTL_RB_BASE + 0x10B8) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_RD_ID_15_REG     (SFC_CTL_RB_BASE + 0x10BC) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_WR_ID_0_REG      (SFC_CTL_RB_BASE + 0x10C0) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_WR_ID_1_REG      (SFC_CTL_RB_BASE + 0x10C4) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_WR_ID_2_REG      (SFC_CTL_RB_BASE + 0x10C8) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_WR_ID_3_REG      (SFC_CTL_RB_BASE + 0x10CC) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_WR_ID_4_REG      (SFC_CTL_RB_BASE + 0x10D0) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_WR_ID_5_REG      (SFC_CTL_RB_BASE + 0x10D4) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_WR_ID_6_REG      (SFC_CTL_RB_BASE + 0x10D8) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_WR_ID_7_REG      (SFC_CTL_RB_BASE + 0x10DC) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_WR_ID_8_REG      (SFC_CTL_RB_BASE + 0x10E0) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_WR_ID_9_REG      (SFC_CTL_RB_BASE + 0x10E4) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_WR_ID_10_REG     (SFC_CTL_RB_BASE + 0x10E8) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_WR_ID_11_REG     (SFC_CTL_RB_BASE + 0x10EC) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_WR_ID_12_REG     (SFC_CTL_RB_BASE + 0x10F0) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_WR_ID_13_REG     (SFC_CTL_RB_BASE + 0x10F4) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_WR_ID_14_REG     (SFC_CTL_RB_BASE + 0x10F8) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_ALLOW_WR_ID_15_REG     (SFC_CTL_RB_BASE + 0x10FC) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_APC_EN_0_REG           (SFC_CTL_RB_BASE + 0x1100) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_APC_EN_1_REG           (SFC_CTL_RB_BASE + 0x1104) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_APC_EN_2_REG           (SFC_CTL_RB_BASE + 0x1108) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_APC_EN_3_REG           (SFC_CTL_RB_BASE + 0x110C) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_APC_EN_4_REG           (SFC_CTL_RB_BASE + 0x1110) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_APC_EN_5_REG           (SFC_CTL_RB_BASE + 0x1114) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_APC_EN_6_REG           (SFC_CTL_RB_BASE + 0x1118) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_APC_EN_7_REG           (SFC_CTL_RB_BASE + 0x111C) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_APC_EN_8_REG           (SFC_CTL_RB_BASE + 0x1120) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_APC_EN_9_REG           (SFC_CTL_RB_BASE + 0x1124) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_APC_EN_10_REG          (SFC_CTL_RB_BASE + 0x1128) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_APC_EN_11_REG          (SFC_CTL_RB_BASE + 0x112C) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_APC_EN_12_REG          (SFC_CTL_RB_BASE + 0x1130) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_APC_EN_13_REG          (SFC_CTL_RB_BASE + 0x1134) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_APC_EN_14_REG          (SFC_CTL_RB_BASE + 0x1138) /* FAPC鉴权 */
#define SFC_CTL_RB_APC_CFG_APC_EN_15_REG          (SFC_CTL_RB_BASE + 0x113C) /* FAPC鉴权 */
#define SFC_CTL_RB_FAPC_AUTH_MAC_SADDR_0_REG      (SFC_CTL_RB_BASE + 0x1140) /* FAPC鉴权 */
#define SFC_CTL_RB_FAPC_AUTH_MAC_SADDR_1_REG      (SFC_CTL_RB_BASE + 0x1144) /* FAPC鉴权 */
#define SFC_CTL_RB_FAPC_AUTH_MAC_SADDR_2_REG      (SFC_CTL_RB_BASE + 0x1148) /* FAPC鉴权 */
#define SFC_CTL_RB_FAPC_AUTH_MAC_SADDR_3_REG      (SFC_CTL_RB_BASE + 0x114C) /* FAPC鉴权 */
#define SFC_CTL_RB_FAPC_AUTH_MAC_SADDR_4_REG      (SFC_CTL_RB_BASE + 0x1150) /* FAPC鉴权 */
#define SFC_CTL_RB_FAPC_AUTH_MAC_SADDR_5_REG      (SFC_CTL_RB_BASE + 0x1154) /* FAPC鉴权 */
#define SFC_CTL_RB_FAPC_AUTH_MAC_SADDR_6_REG      (SFC_CTL_RB_BASE + 0x1158) /* FAPC鉴权 */
#define SFC_CTL_RB_FAPC_AUTH_MAC_SADDR_7_REG      (SFC_CTL_RB_BASE + 0x115C) /* FAPC鉴权 */
#define SFC_CTL_RB_FAPC_AUTH_MAC_SADDR_8_REG      (SFC_CTL_RB_BASE + 0x1160) /* FAPC鉴权 */
#define SFC_CTL_RB_FAPC_AUTH_MAC_SADDR_9_REG      (SFC_CTL_RB_BASE + 0x1164) /* FAPC鉴权 */
#define SFC_CTL_RB_FAPC_AUTH_MAC_SADDR_10_REG     (SFC_CTL_RB_BASE + 0x1168) /* FAPC鉴权 */
#define SFC_CTL_RB_FAPC_AUTH_MAC_SADDR_11_REG     (SFC_CTL_RB_BASE + 0x116C) /* FAPC鉴权 */
#define SFC_CTL_RB_FAPC_AUTH_MAC_SADDR_12_REG     (SFC_CTL_RB_BASE + 0x1170) /* FAPC鉴权 */
#define SFC_CTL_RB_FAPC_AUTH_MAC_SADDR_13_REG     (SFC_CTL_RB_BASE + 0x1174) /* FAPC鉴权 */
#define SFC_CTL_RB_FAPC_AUTH_MAC_SADDR_14_REG     (SFC_CTL_RB_BASE + 0x1178) /* FAPC鉴权 */
#define SFC_CTL_RB_FAPC_AUTH_MAC_SADDR_15_REG     (SFC_CTL_RB_BASE + 0x117C) /* FAPC鉴权 */
#define SFC_CTL_RB_SFC_FAPC_DEC_AUTH_CFG_REG      (SFC_CTL_RB_BASE + 0x1180) /* FAPC鉴权 */
#define SFC_CTL_RB_SFC_FAPC_SADDR_STATUS_REG      (SFC_CTL_RB_BASE + 0x1200) /* FAPC鉴权 */
#define SFC_CTL_RB_SFC_FAPC_EADDR_STATUS_REG      (SFC_CTL_RB_BASE + 0x1204) /* FAPC鉴权 */
#define SFC_CTL_RB_SFC_APC_ERR_INT_REG            (SFC_CTL_RB_BASE + 0x1208) /* FAPC鉴权 */
#define SFC_CTL_RB_SFC_APC_CLR_REG                (SFC_CTL_RB_BASE + 0x120C) /* FAPC鉴权 */
#define SFC_CTL_RB_SFC_APC_INT_MASK_REG           (SFC_CTL_RB_BASE + 0x1210) /* 中断屏蔽寄存器。 */
#define SFC_CTL_RB_FAPC_ONE_WAY_LOCK_REG          (SFC_CTL_RB_BASE + 0x1220) /* FAPC鉴权锁定寄存器 */
#define SFC_CTL_RB_SFC_CMD_LUT0_APC_STATUS_REG    (SFC_CTL_RB_BASE + 0x1224) /* FAPC鉴权 */
#define SFC_CTL_RB_SFC_CMD_LUT1_APC_STATUS_REG    (SFC_CTL_RB_BASE + 0x1228) /* FAPC鉴权 */
#define SFC_CTL_RB_SFC_CMDRB_APC_STATUS_REG       (SFC_CTL_RB_BASE + 0x122C) /* FAPC鉴权 */
#define SFC_CTL_RB_SFC_FAPC_HIT_REGION_STATUS_REG (SFC_CTL_RB_BASE + 0x1230) /* FAPC鉴权 */
#define SFC_CTL_RB_LEA_LP_EN_REG                  (SFC_CTL_RB_BASE + 0x1300) /* LEA控制 */
#define SFC_CTL_RB_LEA_DFX_INFO_REG               (SFC_CTL_RB_BASE + 0x1304) /* LEA DFX */
#define SFC_CTL_RB_FLASH_VOTE_ACPU_REG            (SFC_CTL_RB_BASE + 0x1400) /* 投票控制 */
#define SFC_CTL_RB_FLASH_VOTE_PCPU_REG            (SFC_CTL_RB_BASE + 0x1404) /* 投票控制 */
#define SFC_CTL_RB_FLASH_VOTE_IOMCU_REG           (SFC_CTL_RB_BASE + 0x1408) /* 投票控制 */
#define SFC_CTL_RB_CMD_LUT0_0_REG                 (SFC_CTL_RB_BASE + 0x1500)
#define SFC_CTL_RB_CMD_LUT0_1_REG                 (SFC_CTL_RB_BASE + 0x1504)
#define SFC_CTL_RB_CMD_LUT0_2_REG                 (SFC_CTL_RB_BASE + 0x1508)
#define SFC_CTL_RB_CMD_LUT0_3_REG                 (SFC_CTL_RB_BASE + 0x150C)
#define SFC_CTL_RB_CMD_LUT0_4_REG                 (SFC_CTL_RB_BASE + 0x1510)
#define SFC_CTL_RB_CMD_LUT0_5_REG                 (SFC_CTL_RB_BASE + 0x1514)
#define SFC_CTL_RB_CMD_LUT0_6_REG                 (SFC_CTL_RB_BASE + 0x1518)
#define SFC_CTL_RB_CMD_LUT0_7_REG                 (SFC_CTL_RB_BASE + 0x151C)
#define SFC_CTL_RB_CMD_LUT0_8_REG                 (SFC_CTL_RB_BASE + 0x1520)
#define SFC_CTL_RB_CMD_LUT0_9_REG                 (SFC_CTL_RB_BASE + 0x1524)
#define SFC_CTL_RB_CMD_LUT0_10_REG                (SFC_CTL_RB_BASE + 0x1528)
#define SFC_CTL_RB_CMD_LUT0_11_REG                (SFC_CTL_RB_BASE + 0x152C)
#define SFC_CTL_RB_CMD_LUT0_12_REG                (SFC_CTL_RB_BASE + 0x1530)
#define SFC_CTL_RB_CMD_LUT0_13_REG                (SFC_CTL_RB_BASE + 0x1534)
#define SFC_CTL_RB_CMD_LUT0_14_REG                (SFC_CTL_RB_BASE + 0x1538)
#define SFC_CTL_RB_CMD_LUT0_15_REG                (SFC_CTL_RB_BASE + 0x153C)
#define SFC_CTL_RB_CMD_LUT1_0_REG                 (SFC_CTL_RB_BASE + 0x1540)
#define SFC_CTL_RB_CMD_LUT1_1_REG                 (SFC_CTL_RB_BASE + 0x1544)
#define SFC_CTL_RB_CMD_LUT1_2_REG                 (SFC_CTL_RB_BASE + 0x1548)
#define SFC_CTL_RB_CMD_LUT1_3_REG                 (SFC_CTL_RB_BASE + 0x154C)
#define SFC_CTL_RB_CMD_LUT1_4_REG                 (SFC_CTL_RB_BASE + 0x1550)
#define SFC_CTL_RB_CMD_LUT1_5_REG                 (SFC_CTL_RB_BASE + 0x1554)
#define SFC_CTL_RB_CMD_LUT1_6_REG                 (SFC_CTL_RB_BASE + 0x1558)
#define SFC_CTL_RB_CMD_LUT1_7_REG                 (SFC_CTL_RB_BASE + 0x155C)
#define SFC_CTL_RB_CMD_LUT1_8_REG                 (SFC_CTL_RB_BASE + 0x1560)
#define SFC_CTL_RB_CMD_LUT1_9_REG                 (SFC_CTL_RB_BASE + 0x1564)
#define SFC_CTL_RB_CMD_LUT1_10_REG                (SFC_CTL_RB_BASE + 0x1568)
#define SFC_CTL_RB_CMD_LUT1_11_REG                (SFC_CTL_RB_BASE + 0x156C)
#define SFC_CTL_RB_CMD_LUT1_12_REG                (SFC_CTL_RB_BASE + 0x1570)
#define SFC_CTL_RB_CMD_LUT1_13_REG                (SFC_CTL_RB_BASE + 0x1574)
#define SFC_CTL_RB_CMD_LUT1_14_REG                (SFC_CTL_RB_BASE + 0x1578)
#define SFC_CTL_RB_CMD_LUT1_15_REG                (SFC_CTL_RB_BASE + 0x157C)
#define SFC_CTL_RB_CMD_LUT_EN_REG                 (SFC_CTL_RB_BASE + 0x1580)
#define SFC_CTL_RB_CMD_LUT_CFG_LOCK_REG           (SFC_CTL_RB_BASE + 0x1584)
#define SFC_CTL_RB_CMD_LUT1_RW_REG                (SFC_CTL_RB_BASE + 0x1588)
#define SFC_CTL_RB_LEA_IV_VLD_REG                 (SFC_CTL_RB_BASE + 0x1600) /* LEA控制 */
#define SFC_CTL_RB_LEA_IV_ACPU_0_REG              (SFC_CTL_RB_BASE + 0x1620) /* LEA IV */
#define SFC_CTL_RB_LEA_IV_ACPU_1_REG              (SFC_CTL_RB_BASE + 0x1624) /* LEA IV */
#define SFC_CTL_RB_LEA_IV_ACPU_2_REG              (SFC_CTL_RB_BASE + 0x1628) /* LEA IV */
#define SFC_CTL_RB_LEA_IV_ACPU_3_REG              (SFC_CTL_RB_BASE + 0x162C) /* LEA IV */
#define SFC_CTL_RB_LEA_IV_PCPU_0_REG              (SFC_CTL_RB_BASE + 0x1630) /* LEA IV */
#define SFC_CTL_RB_LEA_IV_PCPU_1_REG              (SFC_CTL_RB_BASE + 0x1634) /* LEA IV */
#define SFC_CTL_RB_LEA_IV_PCPU_2_REG              (SFC_CTL_RB_BASE + 0x1638) /* LEA IV */
#define SFC_CTL_RB_LEA_IV_PCPU_3_REG              (SFC_CTL_RB_BASE + 0x163C) /* LEA IV */
#define SFC_CTL_RB_LEA_IV_ACPU_START_ADDR_0_REG   (SFC_CTL_RB_BASE + 0x1640) /* LEA IV解密起始地址寄存器0 */
#define SFC_CTL_RB_LEA_IV_ACPU_START_ADDR_1_REG   (SFC_CTL_RB_BASE + 0x1644) /* LEA IV解密起始地址寄存器0 */
#define SFC_CTL_RB_LEA_IV_ACPU_START_ADDR_2_REG   (SFC_CTL_RB_BASE + 0x1648) /* LEA IV解密起始地址寄存器0 */
#define SFC_CTL_RB_LEA_IV_ACPU_START_ADDR_3_REG   (SFC_CTL_RB_BASE + 0x164C) /* LEA IV解密起始地址寄存器0 */
#define SFC_CTL_RB_LEA_IV_ACPU_START_ADDR_4_REG   (SFC_CTL_RB_BASE + 0x1650) /* LEA IV解密起始地址寄存器0 */
#define SFC_CTL_RB_LEA_IV_ACPU_START_ADDR_5_REG   (SFC_CTL_RB_BASE + 0x1654) /* LEA IV解密起始地址寄存器0 */
#define SFC_CTL_RB_LEA_IV_ACPU_START_ADDR_6_REG   (SFC_CTL_RB_BASE + 0x1658) /* LEA IV解密起始地址寄存器0 */
#define SFC_CTL_RB_LEA_IV_ACPU_START_ADDR_7_REG   (SFC_CTL_RB_BASE + 0x165C) /* LEA IV解密起始地址寄存器0 */
#define SFC_CTL_RB_LEA_IV_ACPU_START_ADDR_8_REG   (SFC_CTL_RB_BASE + 0x1660) /* LEA IV解密起始地址寄存器0 */
#define SFC_CTL_RB_LEA_IV_ACPU_START_ADDR_9_REG   (SFC_CTL_RB_BASE + 0x1664) /* LEA IV解密起始地址寄存器0 */
#define SFC_CTL_RB_LEA_IV_ACPU_START_ADDR_10_REG  (SFC_CTL_RB_BASE + 0x1668) /* LEA IV解密起始地址寄存器0 */
#define SFC_CTL_RB_LEA_IV_ACPU_START_ADDR_11_REG  (SFC_CTL_RB_BASE + 0x166C) /* LEA IV解密起始地址寄存器0 */
#define SFC_CTL_RB_LEA_IV_ACPU_START_ADDR_12_REG  (SFC_CTL_RB_BASE + 0x1670) /* LEA IV解密起始地址寄存器0 */
#define SFC_CTL_RB_LEA_IV_ACPU_START_ADDR_13_REG  (SFC_CTL_RB_BASE + 0x1674) /* LEA IV解密起始地址寄存器0 */
#define SFC_CTL_RB_LEA_IV_ACPU_START_ADDR_14_REG  (SFC_CTL_RB_BASE + 0x1678) /* LEA IV解密起始地址寄存器0 */
#define SFC_CTL_RB_LEA_IV_ACPU_START_ADDR_15_REG  (SFC_CTL_RB_BASE + 0x167C) /* LEA IV解密起始地址寄存器0 */
#define SFC_CTL_RB_BUS_RD_INS_LUT_REG             (SFC_CTL_RB_BASE + 0x1700) /* BUS_RD_INS_LUT */
#define SFC_CTL_RB_BUS_WR_INS_LUT_REG             (SFC_CTL_RB_BASE + 0x1704) /* BUS_WR_INS_LUT */
#define SFC_CTL_RB_BUS_INS_LUT_APC_EN_REG         (SFC_CTL_RB_BASE + 0x1708) /* BUS_INS_LUT_APC_EN */
#define SFC_CTL_RB_LEA_IV_SCPU_0_REG              (SFC_CTL_RB_BASE + 0x1810) /* LEA IV */
#define SFC_CTL_RB_LEA_IV_SCPU_1_REG              (SFC_CTL_RB_BASE + 0x1814) /* LEA IV */
#define SFC_CTL_RB_LEA_IV_SCPU_2_REG              (SFC_CTL_RB_BASE + 0x1818) /* LEA IV */
#define SFC_CTL_RB_LEA_IV_SCPU_3_REG              (SFC_CTL_RB_BASE + 0x181C) /* LEA IV */
#define SFC_CTL_RB_SFC_MODE_SEC_CTRL_REG          (SFC_CTL_RB_BASE + 0x1880) /* SFC 安全控制 */

#endif // __3322_SFC_CTL_RB_REG_OFFSET_H__
