/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: porting of fbb drivers/chips/3322/porting/mmc/mmc_init.c for morpheus.
 *
 * Kept:      eMMC partition ruler (same sector layout as fbb evb product config),
 *            platform device registration for sdhci0 (eMMC) only — sdhci1
 *            (SDIO) is deliberately NOT registered on morpheus (shared
 *            0x52060000 register view, no ported clock gating — see
 *            sdio_port_init),
 *            MMC_HostInitById flow, post-init card-clock gating bypass write.
 * Dropped:   dpal bus (morpheus uses the fbb compat linux platform bus instead),
 *            proc_fs_init / SetFatSectorsPerBlock / bcache (no bcache ported),
 *            emmc_drv_try_mount / emmc_drv_format (superseded by the liteos_m
 *            VFS FatFs mount path, see mmc_port_fatfs_mount below),
 *            pre_timer_init (no pre-scheduler eMMC use),
 *            clock router registration (fbb clocks subsystem not ported; the
 *            in-tree hi_set_emmc_clock/host_set_clock covers host clock),
 *            save/recovery/pinmux helpers (no low-power caller in morpheus).
 *
 * Entry points to be called from board init once the kernel scheduler is up:
 *   int mmc_port_init(void);   eMMC (sdhci0)
 *   int sdio_port_init(void);  SDIO (sdhci1) — no-op on morpheus
 */

#include <stdint.h>
#include <errno.h>

#include "linux/platform_device.h"
#include "chip_io.h"
#include "soc/mmc.h"
#include "mmc_porting.h"
#include "3322_cfg_s_hgpio_mode_reg_offset.h"
#include "3322_cfg_s_hgpio_pad_reg_offset.h"
#include "block.h"
#include "disk.h"
#include "host.h"
#include "fs/fs.h"
#include "securec.h"      /* snprintf_s */
#include "los_debug.h"    /* PRINT_ERR */
#include "los_task.h"     /* LOS_TaskDelay: partition-ready polling */

#ifndef ENOERR
#define ENOERR 0
#endif

/* kernel/liteos_m/fs/fatfs/fatfs_conf.h keeps the canonical value; the soc
 * shim copy (shim/fs_include/vfs_extend.h) carries the same 0x02. Kept local
 * so this file needs no kernel fs headers. */
#ifndef FMT_FAT32
#define FMT_FAT32 0x02
#endif

/* Canonical declaration: kernel/liteos_m/fs/vfs/los_fs.h:119. Repeated here
 * because the soc module must not include kernel fs headers ("los_fs.h" would
 * resolve to the fbb shim copy, which lacks this API). Keep in sync. */
extern int LOS_PartitionFormat(const char *partName, char *fsType, void *data);

/* sector layout, mirrors fbb drivers/boards/3322_evb/product/product_evb_standard.h */
#define EMMC_VOLUME_BOOT_SECTOR_COUNT   12000ULL
#define EMMC_VOLUME_SYS_SECTOR_COUNT    3145666ULL
#define EMMC_VOLUME_INNER_SECTOR_COUNT  262144ULL
#define EMMC_VOLUME_SYS_BK_SECTOR_COUNT 524288ULL

#define FF_VOLUMES_NUM 5

static struct {
    const char *name;
    uint64_t sector_count;
} g_ff_volume_infos[FF_VOLUMES_NUM] = {
    { "/boot/", EMMC_VOLUME_BOOT_SECTOR_COUNT },
    { "/system/", EMMC_VOLUME_SYS_SECTOR_COUNT },
    { "/inner/", EMMC_VOLUME_INNER_SECTOR_COUNT },
    { "/systembk/", EMMC_VOLUME_SYS_BK_SECTOR_COUNT },
    { "/user/", 0xffffffff }, /* fixed up at runtime to the remaining sectors */
};

static struct resource sdmmc0_resources[] = {
    {
        .start = EMMC_REG_BASE_ADDESS,
        .end = EMMC_REG_END_ADDRESS,
        .flags = IORESOURCE_MEM,
    },
    {
        .start = EMMC_INTERRUPT_IRQ,
        .end = EMMC_INTERRUPT_IRQ,
        .flags = IORESOURCE_IRQ,
    },
    {
        .start = PERI_EMMC_CRG_RST_CTL,
        .end = PERI_EMMC_CRG_RST_CTL,
        .flags = IORESOURCE_REG,
    },
};

static struct platform_device device_sdmmc0 = {
    .name = "sdhci0",
    .id = 0,
    .resource = sdmmc0_resources,
    .num_resources = sizeof(sdmmc0_resources) / sizeof(sdmmc0_resources[0]),
};

static int32_t emmc_disk_partition_cfg(uint64_t emmc_capacity)
{
    int64_t total_sector_num = (int64_t)emmc_capacity - RESERVED_BOOT_SECTOR;
    int64_t user_sector_count = total_sector_num -
        (EMMC_VOLUME_BOOT_SECTOR_COUNT + EMMC_VOLUME_SYS_SECTOR_COUNT +
         EMMC_VOLUME_INNER_SECTOR_COUNT + EMMC_VOLUME_SYS_BK_SECTOR_COUNT);
    int ret;
    uint64_t part_start_sector = RESERVED_BOOT_SECTOR;
    uint64_t part_count_sector;
    int index;

    if ((total_sector_num <= 0) || (user_sector_count <= 0)) {
        return -EINVAL;
    }
    g_ff_volume_infos[FF_VOLUMES_NUM - 1].sector_count = (uint64_t)user_sector_count;
    for (index = 0; index < FF_VOLUMES_NUM; ++index) {
        part_count_sector = g_ff_volume_infos[index].sector_count;
        ret = add_mmc_partition(&emmc, part_start_sector, part_count_sector);
        if (ret != 0) {
            return -EIO;
        }
        part_start_sector += part_count_sector;
    }
    return ENOERR;
}

int mmc_port_init(void)
{
    /* Pre-scheduler phase: register the partition ruler and the platform
     * device ONLY. Card identification (probe -> mmc_attach) pends on
     * LOS_EventRead (freebsd/core/mmc.c mmc_wait_for_req), which requires
     * task context — the driver registration that triggers the probe lives
     * in mmc_port_host_start(), to be called after LOS_Start(). */
    emmc_partition_ruler_register(emmc_disk_partition_cfg);
    (void)platform_device_register(&device_sdmmc0);
    return ENOERR;
}

/* ------------------------------------------------------------------ *
 * fbb board bring-up prerequisites, not covered by anything else in
 * morpheus.
 *
 * The fbb clocks subsystem and app-level eMMC init (emmc_drv_context_
 * init in mmc_init_fbb_reference.c) are NOT ported (Dropped list above),
 * which leaves three prerequisites undone:
 *
 *  1. eMMC pinmux: the eMMC CMD/DAT pins come up in GPIO mode; fbb
 *     programs s_hgpio0..9 mode = 1 — same registers and value as
 *     emmc_recovery_init (mmc_init_fbb_reference.c:352-354, written at
 *     runtime resume, which is why the normal path must have had them
 *     too). Without it the host clock has no route to the pins.
 *  2. HS (high-speed) subsystem power-on: out of boot the whole HS
 *     analog domain is unpowered. fbb brings it up once via
 *     uapi_pmu_hs_sub_control(HS_SUB_EMMC_ID) -> pm_mcu_hs_perp_power_on
 *     (pmu_sub_control.c:78): power-enable handshake, ISO release,
 *     CRG/LGC reset pulse, auto-gating disable and HS RAM retention
 *     timing (TMOD) per BUCK1 voltage. Replicated below in
 *     emmc_hs_sub_power_on().
 *  3. HS subsystem eMMC host clock enable: HS_CTL_RB_HS_CLK_EN_REG bit
 *     CFG_HS_EMMC_CLKEN — fbb clocks init equivalent. Without it the
 *     SDHCI clock divider (HS_CTL_RB_EMMC_HOST_CCLK_DIV_REG, programmed
 *     by the driver) has no source clock.
 *
 * With any of them missing CMD1 gets no response and OCR negotiates to 0
 * ("No compatible cards found on bus", board-observed). All register
 * blocks touched here (HS_CTL 0x52063xxx, LP_CTL_MCU 0x57007xxx,
 * LP_CTL_GLB 0x57004xxx, GLB_CTL 0x57000xxx) sit in the 0x5206xxxx /
 * 0x5700xxxx device space the driver already accesses. */
#define EMMC_PINMUX_NUM    10
#define EMMC_PIN_FUN       1
#define EMMC_PIN_REG_STEP  4

/* Pad control (pull-up / input-enable / drive strength) lives in a
 * DIFFERENT block from the mode registers: CFG_S_HGPIO_PAD_BASE =
 * 0x57009800, one register per pin (+4 stride), fields PU:0 PD:1 IE:2
 * ST:3 DS[5:4] (2-bit enum). Writing mode=1 above only selects the pin
 * function (HAL_PIO_FUNC_EMMC = 1, pinctrl_porting.h).
 * fbb board_evb.h eMMC table (get_pio_func_config, S_HGPIO0..9):
 *   D0..D7 + CMD: DRIVE_2, PULL_UP, IE_ENABLE
 *   CLK (s_hgpio9): DRIVE_2, PULL_MAX (pull left at reset default), IE
 * fbb applies that with uapi_pin_set_ds/set_pull/set_ie (hal_pinctrl_
 * 3322.c), which write ONLY the PU/PD/IE/DS fields: the ST (schmitt
 * trigger, bit3) field is never touched, and CLK's PULL_MAX falls into
 * the hal default branch (ERRCODE_FAIL, PU/PD left as-is). Replicate
 * exactly that: bit3 stays out of both masks and the CLK mask leaves
 * PU/PD untouched. DS is an enum field: DRIVE_2 -> bits[5:4] = 2. Pins
 * are S_HGPIO0..9 = EMMC_D0..D7/CMD/CLK (product_pin.h:59-68). */
#define EMMC_PAD_NUM            10
#define EMMC_PAD_FIELD_MSK      0x37  /* PU|PD|IE|DS[5:4]; ST(bit3) preserved */
#define EMMC_PAD_VAL            0x25  /* PU | IE | DS=2 (DRIVE_2) */
#define EMMC_PAD_FIELD_MSK_CLK  0x34  /* IE|DS only; CLK pull left as-is (fbb PULL_MAX) */
#define EMMC_PAD_VAL_CLK        0x24  /* IE | DS=2 */

/* HS subsystem power-on, replicating fbb pm_mcu_hs_perp_power_on
 * (drivers/chips/3322/pmu/drivers/pmu_sub_control.c:78), which fbb runs
 * via uapi_pmu_hs_sub_control(HS_SUB_EMMC_ID) before any eMMC access.
 * fbb holds osal_irq_lock across the sequence; morpheus runs this in
 * single-threaded board init before the eMMC IRQ is requested, so no
 * locking is needed. All register macros come from soc_reg.h (via
 * soc/mmc.h) except the FRC debug registers, absent from the morpheus
 * header set and defined locally with the fbb addresses/offsets. */
#define LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ZERO_REG            (LP_CTL_MCU_RB_BASE + 0x600)
#define LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ONE_REG             (LP_CTL_MCU_RB_BASE + 0x650)
#define LP_CTL_MCU_RB_RST_SYS_MCU_HS_CRG_N_DBG_FRC_ZERO_OFFSET   3
#define LP_CTL_MCU_RB_RST_SYS_MCU_HS_LGC_N_DBG_FRC_ZERO_OFFSET   2
#define LP_CTL_MCU_RB_RST_SYS_MCU_HS_CRG_N_DBG_FRC_ONE_OFFSET    3
#define LP_CTL_MCU_RB_RST_SYS_MCU_HS_LGC_N_DBG_FRC_ONE_OFFSET    2

#define EMMC_HS_PWR_ACK_TRIES     100000
#define EMMC_BUCK1_VSET_0V8       8      /* fbb PMU_0P9_VSET_0V8, pmu_buck.h */

static void emmc_hs_sub_power_on(void)
{
    uint32_t tries;
    uint16_t vset;

    PRINTK("emmc_hs: pwr_ack=0x%04x soft_rst6=0x%04x (pre)\n",
           readw(LP_CTL_MCU_RB_LP_CTL_MCU_PWR_ACK_REG),
           readw(PERI_EMMC_CRG_RST_CTL));

    /* 1. Assert the SDIO/eMMC CRG/LGC soft reset (GLB_CTL soft-rst-6 =
     *    PERI_EMMC_CRG_RST_CTL: bit0 CRG_N, bit1 LGC_N, 0 = in reset). */
    reg16_clrbit(PERI_EMMC_CRG_RST_CTL, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_SDIO_LGC_N_OFFSET);
    reg16_clrbit(PERI_EMMC_CRG_RST_CTL, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_SDIO_CRG_N_OFFSET);

    /* 2. HS domain power enable (manual value + manual select). */
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_2_REG, LP_CTL_MCU_RB_MCU_HS_PWR_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_2_REG,
                 LP_CTL_MCU_RB_MCU_HS_PWR_EN_MAN_SEL_OFFSET);

    /* 3. Wait for power-ack (fbb spins forever; keep it bounded). */
    for (tries = 0; tries < EMMC_HS_PWR_ACK_TRIES; ++tries) {
        if (reg16_getbit(LP_CTL_MCU_RB_LP_CTL_MCU_PWR_ACK_REG,
                         LP_CTL_MCU_RB_MCU_HS_PWR_ACK_OFFSET) != 0) {
            break;
        }
    }
    if (tries == EMMC_HS_PWR_ACK_TRIES) {
        PRINT_ERR("emmc_hs: HS power-ack timeout, pwr_ack=0x%04x\n",
                  readw(LP_CTL_MCU_RB_LP_CTL_MCU_PWR_ACK_REG));
    }

    /* 4. Release the HS domain isolation. */
    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_2_REG, LP_CTL_MCU_RB_MCU_HS_ISO_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_2_REG,
                 LP_CTL_MCU_RB_MCU_HS_ISO_EN_MAN_SEL_OFFSET);

    /* 5. Reset pulse on the HS CRG/LGC domains (force low, then high). */
    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ZERO_REG,
                 LP_CTL_MCU_RB_RST_SYS_MCU_HS_CRG_N_DBG_FRC_ZERO_OFFSET);
    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ZERO_REG,
                 LP_CTL_MCU_RB_RST_SYS_MCU_HS_LGC_N_DBG_FRC_ZERO_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ONE_REG,
                 LP_CTL_MCU_RB_RST_SYS_MCU_HS_CRG_N_DBG_FRC_ONE_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ONE_REG,
                 LP_CTL_MCU_RB_RST_SYS_MCU_HS_LGC_N_DBG_FRC_ONE_OFFSET);

    /* 6. Release the soft-rst-6 CRG/LGC reset (1 = out of reset). */
    reg16_setbit(PERI_EMMC_CRG_RST_CTL, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_SDIO_CRG_N_OFFSET);
    reg16_setbit(PERI_EMMC_CRG_RST_CTL, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_SDIO_LGC_N_OFFSET);

    /* 7. Disable HS auto clock gating (fbb clears PWM_CFG + AXI bridge
     *    LP_CFG0..4 so the domain keeps running). */
    writew(HS_CTL_RB_PWM_CFG_REG, 0);
    writew(HS_CTL_RB_HS_AXI_BRG_LP_CFG0_REG, 0);
    writew(HS_CTL_RB_HS_AXI_BRG_LP_CFG1_REG, 0);
    writew(HS_CTL_RB_HS_AXI_BRG_LP_CFG2_REG, 0);
    writew(HS_CTL_RB_HS_AXI_BRG_LP_CFG3_REG, 0);
    writew(HS_CTL_RB_HS_AXI_BRG_LP_CFG4_REG, 0);

    /* 8. HS RAM retention timing (TMOD) per BUCK1 voltage. fbb
     *    uapi_pmu_buck_ldo_get_voltage(PMU_BUCK_LDO_BUCK_0P8) reads
     *    LP_CTL_GLB_STS_1 BUCK1_VSET_STS; vset > PMU_0P9_VSET_0V8 (8)
     *    selects the 0.9V table (uapi_pmu_mem_hs_0p9_set), else the
     *    0.8V table (uapi_pmu_mem_hs_0p8_set), pmu_mem.c. The PWR_EN
     *    guard those fbb helpers check is satisfied by step 2 above. */
    vset = (uint16_t)reg16_getbits(LP_CTL_GLB_RB_LP_CTL_GLB_STS_1_REG,
                                   LP_CTL_GLB_RB_BUCK1_VSET_STS_OFFSET,
                                   LP_CTL_GLB_RB_BUCK1_VSET_STS_LEN);
    if (vset > EMMC_BUCK1_VSET_0V8) {
        writew(HS_CTL_RB_USB_CTRL_SP_TMOD_H_REG, 0x0B);
        writew(HS_CTL_RB_USB_CTRL_SP_TMOD_MH_REG, 0x03);
        writew(HS_CTL_RB_USB_CTRL_SP_TMOD_ML_REG, 0x0B);
        writew(HS_CTL_RB_USB_CTRL_SP_TMOD_L_REG, 0x0B);
        writew(HS_CTL_RB_HS_PERP_TCM_RAM_TMOD_CFG0_REG, 0x0B);
        writew(HS_CTL_RB_HS_PERP_TCM_RAM_TMOD_CFG1_REG, 0x0B);
        writew(HS_CTL_RB_HS_PERP_TCM_RAM_TMOD_CFG2_REG, 0x03);
        writew(HS_CTL_RB_HS_PERP_TCM_RAM_TMOD_CFG3_REG, 0x0B);
        writew(HS_CTL_RB_HS_PERP_TCM_TP_TMOD_CFG2_REG, 0x24);
        writew(HS_CTL_RB_HS_PERP_TCM_TP_TMOD_CFG3_REG, 0x43);
    } else {
        writew(HS_CTL_RB_USB_CTRL_SP_TMOD_H_REG, 0x54);
        writew(HS_CTL_RB_USB_CTRL_SP_TMOD_MH_REG, 0x0C);
        writew(HS_CTL_RB_USB_CTRL_SP_TMOD_ML_REG, 0x54);
        writew(HS_CTL_RB_USB_CTRL_SP_TMOD_L_REG, 0x0C);
        writew(HS_CTL_RB_HS_PERP_TCM_RAM_TMOD_CFG0_REG, 0x54);
        writew(HS_CTL_RB_HS_PERP_TCM_RAM_TMOD_CFG1_REG, 0x0C);
        writew(HS_CTL_RB_HS_PERP_TCM_RAM_TMOD_CFG2_REG, 0x54);
        writew(HS_CTL_RB_HS_PERP_TCM_RAM_TMOD_CFG3_REG, 0x0C);
        writew(HS_CTL_RB_HS_PERP_TCM_TP_TMOD_CFG2_REG, 0x2D);
        writew(HS_CTL_RB_HS_PERP_TCM_TP_TMOD_CFG3_REG, 0x44);
    }

    PRINTK("emmc_hs: pwr_ack=0x%04x soft_rst6=0x%04x vset=%u (post)\n",
           readw(LP_CTL_MCU_RB_LP_CTL_MCU_PWR_ACK_REG),
           readw(PERI_EMMC_CRG_RST_CTL), vset);
}

static void emmc_board_prepare(void)
{
    uint32_t i;
    uint32_t addr;
    uint16_t v;

    /* HS subsystem power-on FIRST — fbb app order is pmu power-on ->
     * clocks -> pinmux/pads. The S_HGPIO mode registers (0x52064000, HS
     * domain) swallow writes while the HS auto-gating is still armed
     * (bootloader only powers the domain, it never runs the full fbb
     * sequence): the first boot attempt wrote mode=1 there BEFORE this
     * sequence and read back 0x0000 with CMD/DAT lines stuck on pad
     * pull-ups. */
    emmc_hs_sub_power_on();

    writel(HS_CTL_RB_HS_CLK_EN_REG,
           readl(HS_CTL_RB_HS_CLK_EN_REG) | BIT(HS_CTL_RB_CFG_HS_EMMC_CLKEN_OFFSET));

    /* eMMC function-clock CCRG, COM_CTL region. This is the REAL fbb clock
     * chain (clocks_switch.c: switch_emmc_router -> clocks_set_emmc_freq ->
     * clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_EMMC) ->
     * hal_clocks_ccrg_module_config on COM_CTL_RB_EMMC_CR_REG
     * (3322_com_ctl_rb_reg_offset.h: COM_CTL_RB_BASE + 0x638)); the
     * M_CTL-region EMMC_CCLK_DIV/EMMC_DIV macros in soc/mmc.h are legacy
     * and unused by the fbb clocks code — and the 0x5200xxxx region reads
     * 0 and swallows writes from LiteOS (board-observed), so stop touching
     * it. Register layout (hal_clocks_ccrg.c): bit0 CH_EN, bits[3:1]
     * source (0 = XO 32M), bit4 DIV_EN, bits[11:5] divider. fbb identify
     * level = g_system_emmc_freq_config[0] = XO/div1 = 32 MHz; the
     * driver's HS_CTL div=80 then yields the 400 kHz identify clock. The
     * clocks subsystem is NOT ported, so replicate its exact sequence:
     * disable channel -> select source -> clear divider -> enable channel.
     * COM_CTL_RB_C_CRG_CLKEN bit CFG_COM_EMMC_CLKEN (offset 2, fbb
     * hal_clocks_glb_clken_config) is the upstream function-clock gate;
     * turn it on too in case boot left it cleared. COM_CTL registers are
     * verified accessible (GLB_CLKEN 0x1dae -> 0x7ffe board-observed). */
#define COM_CTL_RB_C_CRG_CLKEN_REG_LOCAL  (COM_CTL_RB_BASE + 0x0604)
#define COM_CTL_RB_EMMC_CR_REG_LOCAL      (COM_CTL_RB_BASE + 0x0638)
#define COM_EMMC_CLKEN_OFFSET             2
    PRINTK("emmc_diag: c_crg_clken=0x%04x emmc_cr=0x%04x sdiom_cr=0x%04x (pre)\n",
           readw(COM_CTL_RB_C_CRG_CLKEN_REG_LOCAL), readw(COM_CTL_RB_EMMC_CR_REG_LOCAL),
           readw(SDIOM_CR_REG));
    writew(COM_CTL_RB_C_CRG_CLKEN_REG_LOCAL,
           (uint16_t)(readw(COM_CTL_RB_C_CRG_CLKEN_REG_LOCAL) | BIT(COM_EMMC_CLKEN_OFFSET)));
    addr = COM_CTL_RB_EMMC_CR_REG_LOCAL;
    v = (uint16_t)(readw(addr) & ~0x0001);        /* channel disable */
    v = (uint16_t)((v & ~0x000E) | (0 << 1));     /* src = XO (0) */
    v = (uint16_t)(v & ~0x3FF0);                  /* div = 1: DIV_EN=0, DIV_NUM=0 */
    writew(addr, v);
    writew(addr, (uint16_t)(v | 0x0001));         /* channel enable */
    PRINTK("emmc_diag: c_crg_clken=0x%04x emmc_cr=0x%04x (post)\n",
           readw(COM_CTL_RB_C_CRG_CLKEN_REG_LOCAL), readw(COM_CTL_RB_EMMC_CR_REG_LOCAL));

    /* Pin programming LAST, after the power sequence and the clocks —
     * the fbb app order (pmu -> clocks init -> uapi_pin_set_*). The
     * mode write is replicated from fbb hal_pinctrl_3322.c
     * (hal_pin_3322_set_aon_mode: uapi_reg_setbits16, mode field
     * bit[3:0], one 32-bit register per pin, +4 stride). Read back all
     * ten registers before and after: if the post values do not follow
     * the writes the HS domain is still gating the register block. */
    PRINTK("emmc_diag: pinmux pre =");
    for (i = 0; i < EMMC_PINMUX_NUM; ++i) {
        PRINTK(" %04x", readw(CFG_S_HGPIO_MODE_CFG_S_HGPIO0_MODE_REG + i * EMMC_PIN_REG_STEP));
    }
    PRINTK("\n");
    for (i = 0; i < EMMC_PINMUX_NUM; ++i) {
        writew(CFG_S_HGPIO_MODE_CFG_S_HGPIO0_MODE_REG + i * EMMC_PIN_REG_STEP,
               EMMC_PIN_FUN);
    }
    PRINTK("emmc_diag: pinmux post=");
    for (i = 0; i < EMMC_PINMUX_NUM; ++i) {
        PRINTK(" %04x", readw(CFG_S_HGPIO_MODE_CFG_S_HGPIO0_MODE_REG + i * EMMC_PIN_REG_STEP));
    }
    PRINTK("\n");
    for (i = 0; i < EMMC_PAD_NUM; ++i) {
        addr = CFG_S_HGPIO_PAD_CFG_S_HGPIO0_CTRL_REG + i * EMMC_PIN_REG_STEP;
        if (i == EMMC_PAD_NUM - 1) {
            /* CLK (s_hgpio9): IE|DS only, PU/PD left at reset default. */
            writel(addr, (readl(addr) & ~EMMC_PAD_FIELD_MSK_CLK) | EMMC_PAD_VAL_CLK);
        } else {
            writel(addr, (readl(addr) & ~EMMC_PAD_FIELD_MSK) | EMMC_PAD_VAL);
        }
    }

    /* One-shot board diagnosis for the card-identify bring-up (temporary).
     * All addresses live in the 0x5206xxxx/0x5700xxxx device space, both
     * covered by PMP entry 28 — safe under PMP. PRESENT_STATE bit24 = CMD
     * line level, bits[23:16] = DAT line levels: with pads configured the
     * idle CMD/DAT lines must all read high. */
    PRINTK("emmc_diag: clken=0x%08x cclkdiv=0x%04x\n",
           readl(HS_CTL_RB_HS_CLK_EN_REG),
           readw(HS_CTL_RB_EMMC_HOST_CCLK_DIV_REG));
    PRINTK("emmc_diag: pstate=0x%08x caps=0x%08x\n",
           readl(EMMC_REG_BASE_ADDESS + 0x24),
           readl(EMMC_REG_BASE_ADDESS + 0x40));
    PRINTK("emmc_diag: pad0..1=0x%02x/0x%02x\n",
           readb(CFG_S_HGPIO_PAD_CFG_S_HGPIO0_CTRL_REG),
           readb(CFG_S_HGPIO_PAD_CFG_S_HGPIO1_CTRL_REG));
}

int mmc_port_host_start(void)
{
    int ret;

    /* fbb board bring-up prerequisites (clocks subsystem + app eMMC init
     * were NOT ported, see emmc_board_prepare). Must run before the first
     * SDHCI command, otherwise CMD1 gets no response and the card scan
     * negotiates OCR to 0 ("No compatible cards found on bus"). */
    emmc_board_prepare();

    /* Registering the "sdhci0" driver matches the device registered above
     * and runs hi_sdhci_attach synchronously: slot init, IRQ setup and the
     * CFG_SDIO_EMMC_COEXIST card-identify polling (40 x 50 ms) — valid only
     * in task context. Must run before mmc_port_fatfs_mount(). */
    ret = MMC_HostInitById(0);
    if (ret != 0) {
        return -EIO;
    }

    /* Final card-clock gate unlock, fbb board flow (emmc_drv_context_init,
     * porting/mmc/mmc_init.c:246): right after MMC_HostInitById returns,
     * fbb whole-word writes CKG_CTRL_AND_STS = 0x2000000 — bit25 card
     * clock output enable, [23:16] gate field cleared. The driver's
     * slot_init only setbits [23:16]=0xff (all gates CLOSED during
     * setup); without this board write bit25 stays 0 and the card clock
     * never reaches the pins, while everything else reads healthy — the
     * exact "all programmed, card silent" signature. This is the eMMC
     * window (0x52061124); the asynchronous bus-error hazard documented
     * below applies to the dead SDIO window (0x52060124) only, which
     * morpheus never touches, and fbb performs the same whole-word write
     * on both hosts after its clocks are up (emmc_cr verified writable:
     * 0x00d5 -> 0x0001 board-observed). */
    PRINTK("emmc_diag: ckg=0x%08x (pre)\n", readl(EMMC_REG_BASE_ADDESS + 0x124));
    writel(EMMC_REG_BASE_ADDESS + 0x124, 0x2000000);
    PRINTK("emmc_diag: ckg=0x%08x (post)\n", readl(EMMC_REG_BASE_ADDESS + 0x124));

    /* One-shot diagnosis, phase 2 (temporary): after slot init + clock
     * programming + the first identify round. SDHCI CLOCK_CONTROL 0x2C:
     * bit0 = internal clock enable, bit1 = INTERNAL CLOCK STABLE (hardware
     * sets it only when the clock source really oscillates — the decisive
     * bit for the whole cclk chain), bit2 = SD clock enable. 0x52C
     * (EMMC_CTRL) is not dumped: on hi3322 it reads back bus residue, so
     * its value carries no signal. */
    PRINTK("emmc_diag2: clkctl=0x%08x cclkdiv=0x%04x pstate=0x%08x\n",
           readl(EMMC_REG_BASE_ADDESS + 0x2C),
           readw(HS_CTL_RB_EMMC_HOST_CCLK_DIV_REG),
           readl(EMMC_REG_BASE_ADDESS + 0x24));
    /* The dead-SDIO-window warning stays true for the SDIO window only:
     * never whole-word write 0x52060124 (async bus error, observed as an
     * Oops with mepc drifting past the store). The eMMC window write above
     * is the fbb-original board step. */
    return ENOERR;
}

int sdio_port_init(void)
{
    /* hi3322 exposes no independent SDIO host on morpheus: the "SDIO host"
     * register view shares the eMMC controller base (0x52060000, the
     * CFG_SDIO_EMMC_COEXIST pairing), and its bus clock is only gated on by
     * the fbb clocks subsystem, which is NOT ported — touching the SDIO
     * view raised an asynchronous bus error (same class as the
     * mmc_port_host_start note below; observed as an Oops at
     * sdhci_slot_init sc_num=1, lw 0x52060108). morpheus uses eMMC only,
     * so sdhci1 is neither registered nor attached. Kept as a no-op so the
     * board init sequence stays unchanged. */
    return ENOERR;
}

int sdio_port_host_start(void)
{
    /* See sdio_port_init: no SDIO host attach on morpheus. */
    return ENOERR;
}

/* ------------------------------------------------------------------ *
 * fatfs mount hook — enabled with LOSCFG_FS_FAT (FATFS_PORTING.md F).
 *
 * liteos_m fatfs (kernel/liteos_m/fs/fatfs) registers the "vfat"
 * filesystem and mounts through POSIX mount(). FatfsMount resolves the
 * device name with GetPartIdByPartName (digits after the LAST 'p' of the
 * device name, 0-based), which equals the shim partition slot. The VFS
 * partition table and LOS_DiskPartition() are NOT involved on this path
 * (FATFS_PORTING.md 3.5/D7: the fdisk hook behind LOS_DiskPartition would
 * write an MBR at shim slot sector 0 — the boot partition VBR).
 *
 * Volume/slot/mount-point order (0-based, must stay in sync with
 * g_ff_volume_infos above and FAT_VOLUME_STRS in
 * kernel/liteos_m/fs/fatfs/fatfs_conf.h):
 *   0:/boot  1:/system  2:/inner  3:/systembk  4:/user
 *
 * First flash: partitions carry no FAT volume, mount fails and the
 * partition is formatted (FAT32) then remounted — fbb emmc_drv_try_mount/
 * emmc_drv_format behaviour (decision 9-D1). A single failing partition
 * does not abort the loop (decision 9-D4); the function returns the
 * failure count and the caller only prints it.
 */
static const char *g_ff_mount_points[FF_VOLUMES_NUM] = {
    "/boot", "/system", "/inner", "/systembk", "/user",
};

/* fbb disk.h: los_part_access — poll-able partition presence check.
 * Declared locally (same policy as LOS_PartitionFormat above): the soc
 * module must not include kernel fs headers. */
extern int los_part_access(const char *dev, unsigned int mode);

/* Partition-ready polling (fbb emmc_drv_try_mount: 20 retries x 20 ms).
 * The eMMC identification (mmcsd_add_part: bio queue + bio thread +
 * los_disk_init with the ruler) completes asynchronously, so the shim
 * partition slots only appear a while after mmc_port_init() returns. */
#define EMMC_PART_MODE          0666
#define EMMC_PART_READY_TRIES   20
#define EMMC_PART_READY_TICKS   2   /* 20 ms at 100 Hz */

int mmc_port_fatfs_mount(void)
{
    int i;
    int failed = 0;

    for (i = 0; i < FF_VOLUMES_NUM; i++) {
        char dev[16];
        int tries;
        int ready = 0;

        (void)snprintf_s(dev, sizeof(dev), sizeof(dev) - 1, "/dev/mmcblk0p%d", i);

        /* Wait for card identification to publish the partition. Without
         * this wait every mount/format runs against unregistered slots and
         * fails (observed on the first board bring-up). */
        for (tries = 0; tries < EMMC_PART_READY_TRIES; tries++) {
            if (los_part_access(dev, EMMC_PART_MODE) == ENOERR) {
                ready = 1;
                break;
            }
            LOS_TaskDelay(EMMC_PART_READY_TICKS);
        }
        if (!ready) {
            PRINT_ERR("fatfs: mmcblk0p%d not ready after %d ms — eMMC card "
                      "identification did not register the disk\n",
                      i, EMMC_PART_READY_TRIES * 20);
            failed++;
            continue;
        }

        if (mount(dev, g_ff_mount_points[i], "vfat", 0, NULL) == 0) {
            continue;
        }
        PRINT_ERR("fatfs: mount %s -> %s failed errno=%d, trying format\n",
                  dev, g_ff_mount_points[i], errno);
        /* First flash: no valid FAT VBR/FAT in the partition -> format, remount */
        int opt = FMT_FAT32;
        if ((LOS_PartitionFormat(dev, "vfat", &opt) != 0) ||
            (mount(dev, g_ff_mount_points[i], "vfat", 0, NULL) != 0)) {
            PRINT_ERR("fatfs: mmcblk0p%d mount failed after format errno=%d\n",
                      i, errno);
            failed++;
        }
    }
    return failed;   /* 0 = all mounted; >0 = failed partition count */
}
