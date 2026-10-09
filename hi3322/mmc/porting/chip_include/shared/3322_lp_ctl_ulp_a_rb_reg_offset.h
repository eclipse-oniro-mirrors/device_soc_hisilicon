/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File name     :  3322_lp_ctl_ulp_a_rb_reg_offset.h
 * Project line  :  Platform And Key Technologies Development
 * Department    :  CAD Development Department
 * Author        :  l00444884
 * Version       :  1.0
 * Date          :  2025/10/09
 * Description   :  The description of xxx project
 * Others        :  Generated automatically by nManager V5.1
 * History       :  l00444884 2025/03/05 17:25:13 Create file
 */

#ifndef __3322_LP_CTL_ULP_A_RB_REG_OFFSET_H__
#define __3322_LP_CTL_ULP_A_RB_REG_OFFSET_H__

/* LP_CTL_ULP_A_RB Base address of Module's Register */
#define LP_CTL_ULP_A_RB_BASE                       (0x5701A000)

/******************************************************************************/
/*                      LP_CTL_ULP_A_RB Registers' Definitions                            */
/******************************************************************************/

#define LP_CTL_ULP_A_RB_CFG_LP_CTL_ULP_A_ID_REG        (LP_CTL_ULP_A_RB_BASE + 0x0)   /* LP_CTL_ULP_AID寄存器 */
#define LP_CTL_ULP_A_RB_CFG_LP_CTL_ULP_A_GP_REG1_REG   (LP_CTL_ULP_A_RB_BASE + 0x4)   /* 通用寄存器 */
#define LP_CTL_ULP_A_RB_CFG_LP_CTL_ULP_A_GP_REG2_REG   (LP_CTL_ULP_A_RB_BASE + 0x8)   /* 通用寄存器 */
#define LP_CTL_ULP_A_RB_CFG_LP_CTL_ULP_A_GP_REG3_REG   (LP_CTL_ULP_A_RB_BASE + 0xC)   /* 通用寄存器 */
#define LP_CTL_ULP_A_RB_CFG_LP_CTL_ULP_A_GP_REG4_REG   (LP_CTL_ULP_A_RB_BASE + 0x10)  /* 通用寄存器 */
#define LP_CTL_ULP_A_RB_S_UGPIO0_REG                   (LP_CTL_ULP_A_RB_BASE + 0x100) /* s_ugpio0 */
#define LP_CTL_ULP_A_RB_S_UGPIO1_REG                   (LP_CTL_ULP_A_RB_BASE + 0x104) /* s_ugpio1 */
#define LP_CTL_ULP_A_RB_CFG_S_UGPIO0_CTRL_REG          (LP_CTL_ULP_A_RB_BASE + 0x110) /* s_ugpio0 */
#define LP_CTL_ULP_A_RB_CFG_S_UGPIO1_CTRL_REG          (LP_CTL_ULP_A_RB_BASE + 0x114) /* s_ugpio1 */
#define LP_CTL_ULP_A_RB_CFG_ULP_DIAG_SEL_REG           (LP_CTL_ULP_A_RB_BASE + 0x200)
#define LP_CTL_ULP_A_RB_ULP_WKUP_INT_STS_REG           (LP_CTL_ULP_A_RB_BASE + 0x300)
#define LP_CTL_ULP_A_RB_ULP_WKUP_INT_CLR_REG           (LP_CTL_ULP_A_RB_BASE + 0x304)
#define LP_CTL_ULP_A_RB_ULP_WKUP_EVT_STS_REG           (LP_CTL_ULP_A_RB_BASE + 0x308)
#define LP_CTL_ULP_A_RB_ULP_WKUP_EVT_CLR_REG           (LP_CTL_ULP_A_RB_BASE + 0x30C)
#define LP_CTL_ULP_A_RB_ULP_WKUP_EN_REG                (LP_CTL_ULP_A_RB_BASE + 0x310)
#define LP_CTL_ULP_A_RB_ULP_WKUP_INT_EN_REG            (LP_CTL_ULP_A_RB_BASE + 0x314)
#define LP_CTL_ULP_A_RB_ULP_SHIPTOBOOT_BYPASS_CTRL_REG (LP_CTL_ULP_A_RB_BASE + 0x320)
#define LP_CTL_ULP_A_RB_PMU_ABNORMAL_SIGNAL_BYPASS_REG (LP_CTL_ULP_A_RB_BASE + 0x330)
#define LP_CTL_ULP_A_RB_PMU_WATCH_DOG_RST_BYPASS_REG   (LP_CTL_ULP_A_RB_BASE + 0x340)
#define LP_CTL_ULP_A_RB_RC_XO_CLK_MUX_REG              (LP_CTL_ULP_A_RB_BASE + 0x350)
#define LP_CTL_ULP_A_RB_LP_REF_BG_BOOT_TIME_REG        (LP_CTL_ULP_A_RB_BASE + 0x400)
#define LP_CTL_ULP_A_RB_LP_REF_BG_SAV_TIME_REG         (LP_CTL_ULP_A_RB_BASE + 0x404)
#define LP_CTL_ULP_A_RB_LP_REF_BUF_BOOT_TIME_REG       (LP_CTL_ULP_A_RB_BASE + 0x408)
#define LP_CTL_ULP_A_RB_LP_REF_BUF_SAV_TIME_REG        (LP_CTL_ULP_A_RB_BASE + 0x40C)
#define LP_CTL_ULP_A_RB_LP_REF_IBG_BOOT_TIME_REG       (LP_CTL_ULP_A_RB_BASE + 0x410)
#define LP_CTL_ULP_A_RB_LP_REF_IBG_SAV_TIME_REG        (LP_CTL_ULP_A_RB_BASE + 0x414)
#define LP_CTL_ULP_A_RB_IOLDO4_EN_ECO_BOOT_TIME_REG    (LP_CTL_ULP_A_RB_BASE + 0x418)
#define LP_CTL_ULP_A_RB_IOLDO4_EN_ECO_SAV_TIME_REG     (LP_CTL_ULP_A_RB_BASE + 0x41C)
#define LP_CTL_ULP_A_RB_BUCK0_EN_BOOT_TIME_REG         (LP_CTL_ULP_A_RB_BASE + 0x420)
#define LP_CTL_ULP_A_RB_BUCK0_EN_SAV_TIME_REG          (LP_CTL_ULP_A_RB_BASE + 0x424)
#define LP_CTL_ULP_A_RB_BUCK1_EN_BOOT_TIME_REG         (LP_CTL_ULP_A_RB_BASE + 0x428)
#define LP_CTL_ULP_A_RB_BUCK1_EN_SAV_TIME_REG          (LP_CTL_ULP_A_RB_BASE + 0x42C)
#define LP_CTL_ULP_A_RB_OVP_EN_BOOT_TIME_REG           (LP_CTL_ULP_A_RB_BASE + 0x430)
#define LP_CTL_ULP_A_RB_OVP_EN_SAV_TIME_REG            (LP_CTL_ULP_A_RB_BASE + 0x434)
#define LP_CTL_ULP_A_RB_ULVO_EN_BOOT_TIME_REG          (LP_CTL_ULP_A_RB_BASE + 0x438)
#define LP_CTL_ULP_A_RB_ULVO_EN_SAV_TIME_REG           (LP_CTL_ULP_A_RB_BASE + 0x43C)
#define LP_CTL_ULP_A_RB_RST_DBB_BOOT_TIME_REG          (LP_CTL_ULP_A_RB_BASE + 0x440)
#define LP_CTL_ULP_A_RB_RST_DBB_SAV_TIME_REG           (LP_CTL_ULP_A_RB_BASE + 0x444)
#define LP_CTL_ULP_A_RB_AON_TO_ULP_ISO_BOOT_TIME_REG   (LP_CTL_ULP_A_RB_BASE + 0x448)
#define LP_CTL_ULP_A_RB_AON_TO_ULP_ISO_SAV_TIME_REG    (LP_CTL_ULP_A_RB_BASE + 0x44C)
#define LP_CTL_ULP_A_RB_SHIP_MDOE_FLAG_BOOT_TIME_REG   (LP_CTL_ULP_A_RB_BASE + 0x450)
#define LP_CTL_ULP_A_RB_SHIP_MDOE_FLAG_SAV_TIME_REG    (LP_CTL_ULP_A_RB_BASE + 0x454)
#define LP_CTL_ULP_A_RB_ULP_BOOT_COMPLETE_TIME_REG     (LP_CTL_ULP_A_RB_BASE + 0x458)
#define LP_CTL_ULP_A_RB_ULP_SAV_COMPLETE_TIME_REG      (LP_CTL_ULP_A_RB_BASE + 0x45C)
#define LP_CTL_ULP_A_RB_LPREF_CTRL_MAN1_REG            (LP_CTL_ULP_A_RB_BASE + 0x500)
#define LP_CTL_ULP_A_RB_IOLDO4_CTRL_MAN_1_REG          (LP_CTL_ULP_A_RB_BASE + 0x504)
#define LP_CTL_ULP_A_RB_BUCK1_CTRL_MAN_0_REG           (LP_CTL_ULP_A_RB_BASE + 0x508)
#define LP_CTL_ULP_A_RB_OVP_ULVO_CTRL_MAN_REG          (LP_CTL_ULP_A_RB_BASE + 0x50C)
#define LP_CTL_ULP_A_RB_RST_DBB_CTRL_MAN_REG           (LP_CTL_ULP_A_RB_BASE + 0x510)
#define LP_CTL_ULP_A_RB_AON_TO_ULP_ISO_CTRL_MAN_REG    (LP_CTL_ULP_A_RB_BASE + 0x514)
#define LP_CTL_ULP_A_RB_SHIP_MODE_FLAG_CTRL_MAN_REG    (LP_CTL_ULP_A_RB_BASE + 0x518)
#define LP_CTL_ULP_A_RB_PMU_RSV_REG_REG                (LP_CTL_ULP_A_RB_BASE + 0x600)
#define LP_CTL_ULP_A_RB_RC_32K_RESERVE_REG_REG         (LP_CTL_ULP_A_RB_BASE + 0x604)
#define LP_CTL_ULP_A_RB_XO_32K_CFG_REG_REG             (LP_CTL_ULP_A_RB_BASE + 0x608)
#define LP_CTL_ULP_A_RB_SYSLDO_VSET_CFG_REG_REG        (LP_CTL_ULP_A_RB_BASE + 0x60C)
#define LP_CTL_ULP_A_RB_IOLDO4_CFG_REG_REG             (LP_CTL_ULP_A_RB_BASE + 0x610)
#define LP_CTL_ULP_A_RB_UVLO_CFG_REG_REG               (LP_CTL_ULP_A_RB_BASE + 0x614)

#endif // __3322_LP_CTL_ULP_A_RB_REG_OFFSET_H__
