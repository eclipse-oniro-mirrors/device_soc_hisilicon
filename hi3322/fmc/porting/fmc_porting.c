/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: hi3322 FMC (SPI-NAND controller) board porting for morpheus.
 *
 * Sequence ported VERBATIM from the fbb NAND-boot bootloader
 * (hs-fbb/src/bootloader/provision_3322/drivers/porting/fmc/fmc_porting.c,
 * fmc_reg_config): it is fully self-contained register manipulation and does
 * not depend on the fbb PMU framework (uapi_pmu_*), which morpheus
 * liteos_m does not carry. On fbb NAND-boot boards this exact sequence
 * runs in SSB, and the RTOS-side porting (chips/3322) inherits its state —
 * so the bootloader sequence defines the fbb-verified runtime register
 * state for the FMC/NAND subsystem:
 *
 *   1. MCU HS sub-system power force-on   (LP_CTL_MCU_MAN_FRC_ON_SEQ bit2)
 *   2. HS sub bus clock                   (M_CTL_HS_BUS_CLK_EN)
 *   3. HS sub soft reset release          (GLB_CTL_M_SOFT_RST_6 SDIO CRG/LGC)
 *   4. FMC router: shared CCRG route       (COM_CTL C_CRG_CLKEN gate +
 *                                          COM_CTL_EMMC_CR = 1)
 *   5. HS FMC clock enable + divider div=1 (HS_CTL_HS_CLK_EN bit8,
 *                                          HS_CTL_HS_FMC_DIV off->div->load->on)
 *   6. Six pins S_HGPIO0..3/4/9 -> QSPI1_M2 (mode 2)
 *
 * morpheus note: SSB runs "nand boot config:0" on this board (NOR boot) and
 * does NOT execute the FMC sequence — the reset default of COM_CTL_EMMC_CR
 * is 0x00d5 and the LiteOS side must do steps 1-6 itself (unlike fbb
 * NAND-boot boards where SSB already did).
 */

#include "soc_reg.h"
#include "errcode.h"
#include "common_def.h"
#include "chip_io.h"
#include "tcxo.h"
#include "fmc_porting.h"

/* fbb bootloader fmc_reg_config constants */
#define TCXO_DELAY_400 400U
#define TCXO_DELAY_10 10U
#define TCXO_DELAY_50 50U
#define TCXO_DELAY_240UL 240UL
#define MODE_QSPI1_VAL 0x2

static uint8_t g_fmc_init;

void fmc_reg_config(void)
{
    /* 1. power on mcu hs sub */
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_FRC_ON_SEQ_REG,
                 LP_CTL_MCU_RB_MCU_HS_PWR_EN_MCU_MAN_FRC_ON_SEQ_OFFSET);
    uapi_tcxo_delay_us(TCXO_DELAY_400);

    /* 2. hs sub bus clock */
    reg16_setbit(M_CTL_RB_HS_BUS_CLK_EN_REG, M_CTL_RB_CFG_M_HS_SUB_BUS_CLKEN_OFFSET);
    uapi_tcxo_delay_us(TCXO_DELAY_10);

    /* 3. mcu hs sub soft rst (fbb reuses the SDIO CRG/LGC soft-rst bits for
     * the whole HS sub-system domain) */
    if (reg32_getbits(GLB_CTL_M_RB_SOFT_RST_6_REG, 0, 0x2) != 0x3) { /* len 2 */
        reg16_setbit(GLB_CTL_M_RB_SOFT_RST_6_REG,
                     GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_SDIO_CRG_N_OFFSET);
        uapi_tcxo_delay_us(TCXO_DELAY_50);
        reg16_setbit(GLB_CTL_M_RB_SOFT_RST_6_REG,
                     GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_SDIO_LGC_N_OFFSET);
        uapi_tcxo_delay_us(TCXO_DELAY_50);
    } else {
        uapi_tcxo_delay_us(TCXO_DELAY_50);
    }

    /* 4. fmc router: gate the shared CCRG clock, write the route register,
     * ungate (fbb bootloader verbatim) */
    reg16_clrbit(COM_CTL_RB_C_CRG_CLKEN_REG, COM_CTL_RB_CFG_COM_EMMC_CLKEN_OFFSET);
    uapi_tcxo_delay_us(TCXO_DELAY_10);
    writew(COM_CTL_RB_EMMC_CR_REG, 1);
    uapi_tcxo_delay_us(TCXO_DELAY_10);
    reg16_setbit(COM_CTL_RB_C_CRG_CLKEN_REG, COM_CTL_RB_CFG_COM_EMMC_CLKEN_OFFSET);
    uapi_tcxo_delay_us(TCXO_DELAY_10);

    /* 5. hs fmc clken + divider: clken off -> load div=1 -> load -> clken on */
    reg16_setbit(HS_CTL_RB_HS_CLK_EN_REG, HS_CTL_RB_CFG_HS_FMC_BUS_CLKEN_OFFSET);
    uapi_tcxo_delay_us(TCXO_DELAY_10);
    reg16_clrbit(HS_CTL_RB_HS_FMC_DIV_REG, HS_CTL_RB_CFG_HS_FMC_DIV_CLKEN_OFFSET);
    reg16_clrbit(HS_CTL_RB_HS_FMC_DIV_REG, HS_CTL_RB_CFG_HS_FMC_LOAD_DIV_EN_OFFSET);
    uapi_tcxo_delay_us(TCXO_DELAY_10);
    reg16_setbits(HS_CTL_RB_HS_FMC_DIV_REG, HS_CTL_RB_CFG_HS_FMC_DIV_NUM_OFFSET,
                  HS_CTL_RB_CFG_HS_FMC_DIV_NUM_LEN, 0x1);
    uapi_tcxo_delay_us(TCXO_DELAY_10);
    reg16_setbit(HS_CTL_RB_HS_FMC_DIV_REG, HS_CTL_RB_CFG_HS_FMC_LOAD_DIV_EN_OFFSET);
    uapi_tcxo_delay_us(TCXO_DELAY_10);
    reg16_setbit(HS_CTL_RB_HS_FMC_DIV_REG, HS_CTL_RB_CFG_HS_FMC_DIV_CLKEN_OFFSET);
    uapi_tcxo_delay_us(TCXO_DELAY_10);

    /* 6. pinmux: S_HGPIO0..3 = QSPI1 D0..3, 4 = CS, 9 = CLK (mode 2).
     * The S_HGPIO group is hard-muxed eMMC(mode 1) vs QSPI1_M2(mode 2);
     * eMMC is frozen on this board (NOR_PORTING_PLAN.md), no conflict. */
    writew(CFG_S_HGPIO_MODE_CFG_S_HGPIO0_MODE_REG, MODE_QSPI1_VAL); /* qspi1_d0 */
    writew(CFG_S_HGPIO_MODE_CFG_S_HGPIO1_MODE_REG, MODE_QSPI1_VAL); /* qspi1_d1 */
    writew(CFG_S_HGPIO_MODE_CFG_S_HGPIO2_MODE_REG, MODE_QSPI1_VAL); /* qspi1_d2 */
    writew(CFG_S_HGPIO_MODE_CFG_S_HGPIO3_MODE_REG, MODE_QSPI1_VAL); /* qspi1_d3 */
    writew(CFG_S_HGPIO_MODE_CFG_S_HGPIO4_MODE_REG, MODE_QSPI1_VAL); /* qspi1_cs */
    writew(CFG_S_HGPIO_MODE_CFG_S_HGPIO9_MODE_REG, MODE_QSPI1_VAL); /* qspi1_clk */

    /* LDO1-SW0 enable in fbb — "if the NAND uses the board 1.8V rail, this
     * is not needed" (fbb comment). Board confirmed 1.8V rail: skipped. */
}

void fmc_port_init(void)
{
    if (g_fmc_init != 0) {
        return;
    }
    fmc_reg_config();
    g_fmc_init = 1;
}

/* Kept for fmc_porting.h API parity (fbb RTOS signatures). The divider is
 * fixed at init; runtime re-divide is not used by morpheus. */
void fmc_clock_div_set(uint8_t div)
{
    (void)div;
}

void fmc_io_clock_div_set(uint8_t div)
{
    (void)div;
}

/* No PM framework in morpheus liteos_m: suspend/resume are no-ops. The
 * driver never races a power transition (polling, task context only). */
errcode_t uapi_fmc_suspend(uintptr_t arg)
{
    (void)arg;
    return ERRCODE_SUCC;
}

errcode_t uapi_fmc_resume(uintptr_t arg)
{
    (void)arg;
    if (g_fmc_init == 0) {
        return ERRCODE_SUCC;
    }
    fmc_reg_config();
    return ERRCODE_SUCC;
}
