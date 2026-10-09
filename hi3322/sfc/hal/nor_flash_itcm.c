/*
 * ---------------------------------------------------------------------------
 * ITCM-resident minimal SFC flash data-command executor for the
 * XIP-constrained system.
 *
 * Rationale (NOR_PORTING_PLAN.md 4.1): while the SFC executes a flash
 * write/erase command, the XIP window cannot serve instruction fetches —
 * the CPU takes Illegal instructions from the erasing flash (board-observed
 * Oops, mepc=0x260c0d00, a3=0x20). The data-phase command sequence
 * (command -> addr -> databuf -> start -> poll WIP) therefore runs from
 * ITCM, with interrupts locked by the caller and no XIP access inside
 * this file:
 *   - code: section .nor.itcm.text (board.ld places it in ITCM 0x10800+,
 *     copied there by reset_vector.S like .data; dedicated name so the
 *     generic  .itcm.text convention keeps its flash-XIP mapping);
 *   - data: only DTCM globals (g_sfc_cmd_regs/g_sfc_cmd_databuf, filled by
 *     hal_sfc_regs_init during uapi_sfc_init) and the caller's buffer
 *     (littlefs cache, DTCM heap);
 *   - no libc, no prints; watchdog kick is a direct WDT register store.
 *
 * WREN / status reads are NOT done here: board experiments showed WREN
 * issued from ITCM does not latch WEL (SR1 reads 0x00 on both the ITCM
 * and the  XIP RDSR path), while the  XIP sequence works. The
 * caller therefore issues WREN through the  XIP path
 * (hal_sfc_write_enable + sfc_port_read_flash_reg WEL check) right before
 * each locked ITCM data command — WEL latches in the chip across the
 * call boundary and auto-clears on completion.
 *
 * Data commands use the standard-SPI set of the GD25LQ128E (page program
 * 0x02, 4KB sector erase 0x20) — byte-identical to the fbb
 * g_flash_common_write_cmds/erase_cmds table entries. Single command
 * transfers are capped at 64 bytes (MAX_DATABUF_NUM=16 words; cmd_config
 * data_cnt is 6 bits), so a 256-byte littlefs program request is split by
 * the caller into four 64-byte chunks, each preceded by its own WREN.
 *
 * Register fields are built through the  bitfield unions
 * (cmd_config_t/cmd_ins_t from hal_sfc_v150_regs_def.h) — an earlier
 * hand-rolled bitmask had data_cnt shifted (<<14 instead of the bitfield's
 * bits[14:9]) which corrupted every program transfer (flash read back
 * all-zero).
 * ---------------------------------------------------------------------------
 */
#include <stdint.h>

#include "hal_sfc_v150_regs_def.h"
#include "sfc_v2.h"

/* hal_sfc.c globals (DTCM), initialised by hal_sfc_regs_init() */
extern uintptr_t g_sfc_cmd_regs[SFC_ID_MAX];
extern uintptr_t g_sfc_cmd_databuf[SFC_ID_MAX];

/* WDT v151 @ 0x57020000: RESTART register, key value (board watchdog.h) */
#define ITCM_WDT_RESTART_REG  (*(volatile uint32_t *)0x57020008u)
#define ITCM_WDT_KEY          0x5A5A5A5Au
#define ITCM_WDT_FEED()       (ITCM_WDT_RESTART_REG = ITCM_WDT_KEY)

/* GD25LQ128E standard-SPI data commands ( g_flash_common_*_cmds) */
#define NOR_CMD_PAGE_PROG   0x02u
#define NOR_CMD_ERASE_4K    0x20u
#define NOR_CMD_RDSR        0x05u
#define NOR_WIP_MASK        0x1u
#define NOR_PROG_CHUNK      64u    /* cmd_config.data_cnt is 6 bits (64B max) */
#define NOR_WAIT_LIMIT      0x200000UL

#define ITCM_SECT __attribute__((section(".nor.itcm.text"), noinline))

/*
 * Configure cmd_ins + cmd_config WITHOUT the start bit (data phase prep).
 * Standard SPI, no dummy. Built through the  bitfield unions.
 */
static void itcm_prog_config(uint32_t cmd, uint32_t data_size, uint32_t data_en,
                             uint32_t addr_en) ITCM_SECT;
static void itcm_prog_config(uint32_t cmd, uint32_t data_size, uint32_t data_en,
                             uint32_t addr_en)
{
    volatile cmd_regs_t *regs = (volatile cmd_regs_t *)g_sfc_cmd_regs[0];
    cmd_ins_t ins;
    cmd_config_t config;

    ins.d32 = regs->cmd_ins;
    ins.b.reg_ins = cmd & 0xFFu;
    regs->cmd_ins = ins.d32;

    config.d32 = 0;
    config.b.sel_cs = 1u;
    config.b.addr_en = (addr_en != 0u) ? 1u : 0u;
    config.b.data_en = (data_en != 0u) ? 1u : 0u;
    config.b.rw = 0u;                       /* write */
    config.b.mem_if_type = 0u;              /* standard SPI */
    if (data_size != 0u) {
        config.b.data_cnt = data_size - 1u; /* data length = data_cnt + 1 */
    }
    regs->cmd_config = config.d32;          /* start stays 0 */
}

static void itcm_start(void) ITCM_SECT;
static void itcm_start(void)
{
    volatile cmd_regs_t *regs = (volatile cmd_regs_t *)g_sfc_cmd_regs[0];
    cmd_config_t config;
    config.d32 = regs->cmd_config;
    config.b.start = 1u;
    regs->cmd_config = config.d32;
    while ((regs->cmd_config & 0x1u) != 0u) {
        /* SPI transfer in progress (auto-clears) */
    }
}

static void itcm_start_raw(volatile cmd_regs_t *regs) ITCM_SECT;
static void itcm_start_raw(volatile cmd_regs_t *regs)
{
    regs->cmd_config |= 0x1u;
    while ((regs->cmd_config & 0x1u) != 0u) {
    }
}

/*
 * RDSR poll: WIP (status bit0) must clear before the next command.
 * Kicks the watchdog every 64 polls (4KB erase is up to ~400 ms).
 */
static int itcm_wait_ready(void) ITCM_SECT;
static int itcm_wait_ready(void)
{
    volatile cmd_regs_t *regs = (volatile cmd_regs_t *)g_sfc_cmd_regs[0];
    volatile cmd_databufs_t *databuf = (volatile cmd_databufs_t *)g_sfc_cmd_databuf[0];
    cmd_ins_t ins;
    uint32_t polls = 0;

    ins.d32 = regs->cmd_ins;
    ins.b.reg_ins = NOR_CMD_RDSR;
    regs->cmd_ins = ins.d32;

    do {
        cmd_config_t config;
        uint32_t status;

        config.d32 = 0;
        config.b.sel_cs = 1u;
        config.b.data_en = 1u;
        config.b.rw = 1u;                   /* read */
        config.b.data_cnt = 0u;             /* 1 byte */
        config.b.mem_if_type = 0u;          /* standard SPI */
        regs->cmd_config = config.d32;      /* start stays 0 */
        itcm_start_raw(regs);

        status = databuf->cmd_databuf[0];
        if ((status & NOR_WIP_MASK) == 0u) {
            return 0;
        }
        if ((++polls & 0x3Fu) == 0u) {
            ITCM_WDT_FEED();
        }
    } while (polls < NOR_WAIT_LIMIT);

    return -1;
}

/* Externally callable status read (XIP calls it between locked ITCM ops). */
int nor_itcm_read_status(uint8_t *out) ITCM_SECT;
int nor_itcm_read_status(uint8_t *out)
{
    volatile cmd_regs_t *regs = (volatile cmd_regs_t *)g_sfc_cmd_regs[0];
    volatile cmd_databufs_t *databuf = (volatile cmd_databufs_t *)g_sfc_cmd_databuf[0];
    cmd_ins_t ins;
    cmd_config_t config;

    ins.d32 = regs->cmd_ins;
    ins.b.reg_ins = NOR_CMD_RDSR;
    regs->cmd_ins = ins.d32;

    config.d32 = 0;
    config.b.sel_cs = 1u;
    config.b.data_en = 1u;
    config.b.rw = 1u;
    config.b.mem_if_type = 0u;
    regs->cmd_config = config.d32;
    regs->cmd_config |= 0x1u;               /* start */
    while ((regs->cmd_config & 0x1u) != 0u) {
    }

    *out = (uint8_t)(databuf->cmd_databuf[0] & 0xFFu);
    return 0;
}

/*
 * Erase one 4KB sector at flash-relative offset rel_off. The caller has
 * already issued WREN (WEL latched) through the  XIP path.
 */
int nor_itcm_sector_erase(uint32_t rel_off) ITCM_SECT;
int nor_itcm_sector_erase(uint32_t rel_off)
{
    volatile cmd_regs_t *regs = (volatile cmd_regs_t *)g_sfc_cmd_regs[0];
    cmd_config_t config;
    int ret;

    ret = itcm_wait_ready();
    if (ret != 0) {
        return ret;
    }

    itcm_prog_config(NOR_CMD_ERASE_4K, 0u, 0u, 1u);
    regs->cmd_addr = rel_off;
    config.d32 = regs->cmd_config;
    config.b.start = 1u;
    regs->cmd_config = config.d32;
    while ((regs->cmd_config & 0x1u) != 0u) {
    }
    return itcm_wait_ready();
}

/*
 * Program one 64-byte chunk at flash-relative offset rel_off (both
 * 64-byte aligned; WEL latched by the caller through the  XIP path).
 */
int nor_itcm_page_prog64(uint32_t rel_off, const uint8_t *buf) ITCM_SECT;
int nor_itcm_page_prog64(uint32_t rel_off, const uint8_t *buf)
{
    volatile cmd_regs_t *regs = (volatile cmd_regs_t *)g_sfc_cmd_regs[0];
    volatile cmd_databufs_t *databuf = (volatile cmd_databufs_t *)g_sfc_cmd_databuf[0];
    cmd_config_t config;
    uint32_t i;
    int ret;

    ret = itcm_wait_ready();
    if (ret != 0) {
        return ret;
    }

    itcm_prog_config(NOR_CMD_PAGE_PROG, NOR_PROG_CHUNK, 1u, 1u);
    regs->cmd_addr = rel_off;
    for (i = 0; i < NOR_PROG_CHUNK / 4u; ++i) {
        uint32_t w = (uint32_t)buf[i * 4u] |
                     ((uint32_t)buf[i * 4u + 1u] << 8) |
                     ((uint32_t)buf[i * 4u + 2u] << 16) |
                     ((uint32_t)buf[i * 4u + 3u] << 24);
        databuf->cmd_databuf[i] = w;
    }
    config.d32 = regs->cmd_config;
    config.b.start = 1u;
    regs->cmd_config = config.d32;
    while ((regs->cmd_config & 0x1u) != 0u) {
    }
    return itcm_wait_ready();
}
