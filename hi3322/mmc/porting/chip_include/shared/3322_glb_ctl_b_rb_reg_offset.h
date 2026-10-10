/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File name     :  3322_glb_ctl_b_rb_reg_offset.h
 * Project line  :  Platform And Key Technologies Development
 * Department    :  CAD Development Department
 * Author        :  l00444884
 * Version       :  1.0
 * Date          :  2025/10/09
 * Description   :  The description of xxx project
 * Others        :  Generated automatically by nManager V5.1
 * History       :  l00444884 2025/03/05 17:25:13 Create file
 */

#ifndef __3322_GLB_CTL_B_RB_REG_OFFSET_H__
#define __3322_GLB_CTL_B_RB_REG_OFFSET_H__

/* GLB_CTL_B_RB Base address of Module's Register */
#define GLB_CTL_B_RB_BASE                       (0x57000400)

/******************************************************************************/
/*                      GLB_CTL_B_RB Registers' Definitions                            */
/******************************************************************************/

#define GLB_CTL_B_RB_GLB_CTL_B_ID_REG            (GLB_CTL_B_RB_BASE + 0x0)   /* GLB CTL ID寄存器 */
#define GLB_CTL_B_RB_CFG_B_PC_L_REG              (GLB_CTL_B_RB_BASE + 0x4)   /* BCPU pc指针地址低16bit */
#define GLB_CTL_B_RB_CFG_B_PC_H_REG              (GLB_CTL_B_RB_BASE + 0x8)   /* BCPU pc指针地址高16bit */
#define GLB_CTL_B_RB_GLB_GP_B_REG0_REG           (GLB_CTL_B_RB_BASE + 0x10)  /* 通用寄存器 */
#define GLB_CTL_B_RB_GLB_GP_B_REG1_REG           (GLB_CTL_B_RB_BASE + 0x14)  /* 通用寄存器 */
#define GLB_CTL_B_RB_GLB_GP_B_REG2_REG           (GLB_CTL_B_RB_BASE + 0x18)  /* 通用寄存器 */
#define GLB_CTL_B_RB_GLB_GP_B_REG3_REG           (GLB_CTL_B_RB_BASE + 0x1C)  /* 通用寄存器 */
#define GLB_CTL_B_RB_TGLP_STATUS_REG             (GLB_CTL_B_RB_BASE + 0x20)
#define GLB_CTL_B_RB_BSUB_OTP_DEBUG_REG          (GLB_CTL_B_RB_BASE + 0x24)
#define GLB_CTL_B_RB_BT_AUTO_ENG_REG0_L_REG      (GLB_CTL_B_RB_BASE + 0x2C)  /* BT_AUTO_ENG_REG0[15:0] */
#define GLB_CTL_B_RB_BT_AUTO_ENG_REG0_H_REG      (GLB_CTL_B_RB_BASE + 0x30)  /* BT_AUTO_ENG_REG0[31:16] */
#define GLB_CTL_B_RB_BT_AUTO_ENG_REG1_L_REG      (GLB_CTL_B_RB_BASE + 0x34)  /* BT_AUTO_ENG_REG1[15:0] */
#define GLB_CTL_B_RB_BT_AUTO_ENG_REG1_H_REG      (GLB_CTL_B_RB_BASE + 0x38)  /* BT_AUTO_ENG_REG1[31:16] */
#define GLB_CTL_B_RB_BT_LP_ENG_AUTO_EN_H_REG     (GLB_CTL_B_RB_BASE + 0x3C)  /* BT_AUTO_ENG_REG1[15:0] */
#define GLB_CTL_B_RB_BT_LP_ENG_AUTO_EN_L_REG     (GLB_CTL_B_RB_BASE + 0x40)  /* BT_AUTO_ENG_REG0[15:0] */
#define GLB_CTL_B_RB_BT_LP_INIT_VLD_REG          (GLB_CTL_B_RB_BASE + 0x44)  /* INIT */
#define GLB_CTL_B_RB_BT_LP_AUTO_OSC_EN_WKUP_REG  (GLB_CTL_B_RB_BASE + 0x50)
#define GLB_CTL_B_RB_BT_LP_AUTO_ENG_EXT_WKUP_REG (GLB_CTL_B_RB_BASE + 0x54)
#define GLB_CTL_B_RB_LPRB_LPEG_CFG_H_REG         (GLB_CTL_B_RB_BASE + 0x58)  /* BT_LPRB_LPEG_CFG[31:16] */
#define GLB_CTL_B_RB_LPRB_LPEG_CFG_L_REG         (GLB_CTL_B_RB_BASE + 0x5C)  /* BT_LPRB_LPEG_CFG[15:0] */
#define GLB_CTL_B_RB_BT_CBB_PIN_CTL_REG          (GLB_CTL_B_RB_BASE + 0x200) /* BT_CBB_PIN_CTL */
#define GLB_CTL_B_RB_B_LPEG_MEM_START_ADDR_L_REG (GLB_CTL_B_RB_BASE + 0x204)
#define GLB_CTL_B_RB_B_LPEG_MEM_START_ADDR_H_REG (GLB_CTL_B_RB_BASE + 0x208)
#define GLB_CTL_B_RB_BCPU_PC_LR_LOAD_REG         (GLB_CTL_B_RB_BASE + 0x3A0)
#define GLB_CTL_B_RB_BCPU_PC_LR_ENABLE_REG       (GLB_CTL_B_RB_BASE + 0x3A4)
#define GLB_CTL_B_RB_BCPU_PC_L_REG               (GLB_CTL_B_RB_BASE + 0x3A8)
#define GLB_CTL_B_RB_BCPU_PC_H_REG               (GLB_CTL_B_RB_BASE + 0x3AC)
#define GLB_CTL_B_RB_BCPU_LR_L_REG               (GLB_CTL_B_RB_BASE + 0x3B0)
#define GLB_CTL_B_RB_BCPU_LR_H_REG               (GLB_CTL_B_RB_BASE + 0x3B4)
#define GLB_CTL_B_RB_BCPU_CORE_WAIT_REG          (GLB_CTL_B_RB_BASE + 0x3B8)
#define GLB_CTL_B_RB_FEM_CTRL_CFG_REG            (GLB_CTL_B_RB_BASE + 0x3C0)

#endif // __3322_GLB_CTL_B_RB_REG_OFFSET_H__
