/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides sfc port template \n
 *
 * History: \n
 * 2022-11-30， Create file. \n
 */
#include "hal_sfc_v150.h"
#ifndef BUILD_NOOSAL
#include "soc_osal.h"
#endif
#include <securec.h>
#include "memory_config.h"
#include "tcxo.h"
#include "sfc_v2.h"
#include "chip_io.h"
#include "arch_barrier.h"
#include "osal_interrupt.h"
#ifdef BUILD_APPLICATION_LOADER
#include "boot_serial.h"
#else
#include "debug_print.h"
#endif
#include "sfc_porting.h"
#ifdef SFC_USE_MUTEX
#include "los_hwi.h"
#include "los_exc.h"
#include "panic.h"
#endif

#define SFC_0_MCPU_END          (FLASH_START + FLASH_LENGTH)
#define SFC_0_REG_BASE_ADDR     0x28000000

#define SFC_DELAY_ONCE_US       200
#define SFC_DELAY_TIMES         20000

typedef struct { /* KGD flash use reg0 reg1 */
    uint32_t addr;
    uint8_t reg0_set;
    uint8_t reg0_mask;
    uint8_t reg1_set;
    uint8_t reg1_mask;
} flash_protect_size_cfg;

static flash_protect_size_cfg g_common_protect_lower[] = {
    /* w25q12pw     gd25le128e */
    {0,             0,      0x7C,   0x0,    0x40}, /* No protection */
    {0x40000,       0x24,   0x7C,   0x0,    0x40}, /* Protected 256KB */
    {FLASH_LENGTH,  0x1C,   0x7C,   0x0,    0x40}, /* Protected ALL */
};

static uint32_t g_flash_protect_addr[SFC_ID_MAX] = {FLASH_LENGTH};

static uint32_t g_flash_id_record[SFC_ID_MAX] = {0};

uintptr_t g_sfc_start_addr[SFC_ID_MAX] = {(uintptr_t)FLASH_START};

uintptr_t g_sfc_end_addr[SFC_ID_MAX] = {(uintptr_t)SFC_0_MCPU_END};

uintptr_t g_sfc_global_conf_base_addr[SFC_ID_MAX] = {SFC_0_REG_BASE_ADDR + 0x100};

uintptr_t g_sfc_bus_regs_base_addr[SFC_ID_MAX] = {SFC_0_REG_BASE_ADDR + 0x200};

uintptr_t g_sfc_bus_dma_regs_base_addr[SFC_ID_MAX] = {SFC_0_REG_BASE_ADDR + 0x240};

uintptr_t g_sfc_cmd_regs_base_addr[SFC_ID_MAX] = {SFC_0_REG_BASE_ADDR + 0x300};

uintptr_t g_sfc_cmd_databuf_base_addr[SFC_ID_MAX] = {SFC_0_REG_BASE_ADDR + 0x400};

uint32_t g_sfc_delay_once_us[SFC_ID_MAX] = {SFC_DELAY_ONCE_US};

uint32_t g_sfc_delay_times[SFC_ID_MAX] = {SFC_DELAY_TIMES};

#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
uintptr_t g_sfc_continue_read_base_addr[SFC_ID_MAX] = {SFC_0_REG_BASE_ADDR + 0x2A0};
#endif

#ifdef SFC_USE_MUTEX
static osal_mutex g_sfc_port_mutex;
#endif

uintptr_t sfc_port_get_sfc_start_addr(uint8_t id)
{
    return g_sfc_start_addr[id];
}

uintptr_t sfc_port_get_sfc_end_addr(uint8_t id)
{
    return g_sfc_end_addr[id];
}

uintptr_t sfc_port_get_sfc_global_conf_base_addr(uint8_t id)
{
    return g_sfc_global_conf_base_addr[id];
}

uintptr_t sfc_port_get_sfc_bus_regs_base_addr(uint8_t id)
{
    return g_sfc_bus_regs_base_addr[id];
}

uintptr_t sfc_port_get_sfc_bus_dma_regs_base_addr(uint8_t id)
{
    return g_sfc_bus_dma_regs_base_addr[id];
}

uintptr_t sfc_port_get_sfc_cmd_regs_base_addr(uint8_t id)
{
    return g_sfc_cmd_regs_base_addr[id];
}

uintptr_t sfc_port_get_sfc_cmd_databuf_base_addr(uint8_t id)
{
    return g_sfc_cmd_databuf_base_addr[id];
}

#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
uintptr_t sfc_port_get_sfc_continue_read_base_addr(uint8_t id)
{
    return g_sfc_continue_read_base_addr[id];
}
#endif

void sfc_port_set_delay_once_time(uint8_t id, uint32_t delay_us)
{
    g_sfc_delay_once_us[id] = delay_us;
}

uint32_t sfc_port_get_delay_once_time(uint8_t id)
{
    return g_sfc_delay_once_us[id];
}

void sfc_port_set_delay_times(uint8_t id, uint32_t delay_times)
{
    g_sfc_delay_times[id] = delay_times;
}

uint32_t sfc_port_get_delay_times(uint8_t id)
{
    return g_sfc_delay_times[id];
}

void sfc_port_disable_bus_read(uint8_t id)
{
    hal_sfc_regs_disable_bus_read(id);
}

void sfc_port_disable_bus_write(uint8_t id)
{
    hal_sfc_regs_disable_bus_write(id);
}

void sfc_port_lock_init(uint8_t id)
{
    unused(id);
#ifdef SFC_USE_MUTEX
    int ret = osal_mutex_init(&g_sfc_port_mutex);
    if (ret != OSAL_SUCCESS) {
        PRINT("SFC mutex init error ret=%d\n", ret);
    }
#endif
}

uint32_t sfc_port_lock(uint8_t id)
{
    unused(id);
#ifdef SFC_USE_MUTEX
    if (OS_EXC_ACTIVE) {
        return 0;
    }
    if (OS_INT_ACTIVE) {
        PRINT("ERROR:SFC mutex lock cannot be used in interrupt\n");
        panic(PANIC_XIP, __LINE__);
    }
    osal_mutex_lock(&g_sfc_port_mutex);
    return 0;
#else
    return osal_irq_lock();
#endif
}

void sfc_port_unlock(uint8_t id, uint32_t lock_sts)
{
    unused(id);
#ifdef SFC_USE_MUTEX
    unused(lock_sts);
    if (OS_EXC_ACTIVE) {
        return;
    }
    if (OS_INT_ACTIVE) {
        PRINT("ERROR:SFC mutex unlock cannot be used in interrupt\n");
        panic(PANIC_XIP, __LINE__);
    }
    osal_mutex_unlock(&g_sfc_port_mutex);
#else
    osal_irq_restore(lock_sts);
#endif
}

errcode_t sfc_port_read_flash_reg(uint8_t id, uint8_t *reg_info, uint8_t cmd)
{
    spi_opreation_t opt = {0};

    if ((id >= SFC_ID_MAX) || (reg_info == NULL)) {
        return ERRCODE_SFC_INVALID_PARAM;
    }

    opt.cmd_support = SPI_CMD_SUPPORT;
    opt.cmd = cmd;
    return hal_sfc_reg_flash_opreations(id, READ_TYPE, opt, reg_info, 0x1);
}

errcode_t sfc_port_read_check_flash_reg(uint8_t id, uint8_t *reg_info, uint8_t cmd, uint8_t check_mask)
{
#define MAX_RANDOM_DELAY  100
    uint8_t val0 = 0;
    uint8_t val1 = 0x5a;
    uint8_t val2 = 0xa5;
    errcode_t ret;
    spi_opreation_t opt = {0};

    if ((id >= SFC_ID_MAX) || (reg_info == NULL)) {
        return ERRCODE_SFC_INVALID_PARAM;
    }
    opt.cmd_support = SPI_CMD_SUPPORT;
    opt.cmd = cmd;
    ret = hal_sfc_reg_flash_opreations(id, READ_TYPE, opt, &val0, 0x1);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    uapi_tcxo_delay_us((uint32_t)(uapi_tcxo_get_count() % MAX_RANDOM_DELAY));
    ret = hal_sfc_reg_flash_opreations(id, READ_TYPE, opt, &val1, 0x1);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    uapi_tcxo_delay_us((uint32_t)(uapi_tcxo_get_count() % MAX_RANDOM_DELAY));
    ret = hal_sfc_reg_flash_opreations(id, READ_TYPE, opt, &val2, 0x1);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    if (((val0 & check_mask) != (val1 & check_mask)) || ((val1 & check_mask) != (val2 & check_mask))) {
        return ERRCODE_FAIL;
    }
    *reg_info = val0;
    return ERRCODE_SUCC;
}

errcode_t sfc_port_write_flash_regs(uint8_t id, uint8_t write_cmd, uint8_t wren_cmd, uint8_t *value, uint8_t len)
{
    errcode_t ret;
    spi_opreation_t opt = {0};
    if ((id >= SFC_ID_MAX) || (value == NULL) || (len < 0x1)) {
        return ERRCODE_SFC_INVALID_PARAM;
    }
    ret = hal_sfc_write_enable(id, wren_cmd);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    opt.cmd_support = SPI_CMD_SUPPORT;
    opt.cmd = write_cmd;
    ret = hal_sfc_reg_flash_opreations(id, WRITE_TYPE, opt, value, len);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = hal_sfc_regs_wait_ready(id, 0);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return ERRCODE_SUCC;
}

static bool need_check_qe_status(uint8_t id, flash_cmd_qe_enable_t *cfg)
{
    uint8_t cmd_idx;
    uint8_t value[0x2] = {0};
    errcode_t ret;
    uint8_t status;
    for (cmd_idx = 0; cmd_idx < cfg->cmd_len; cmd_idx++) {
        ret = sfc_port_read_check_flash_reg(id, &status, cfg->read_cmd[cmd_idx], cfg->read_mask[cmd_idx]);
        if (ret != ERRCODE_SUCC) {
#ifdef BUILD_APPLICATION_LOADER
            boot_msg2("read eflash regs fail:", cfg->read_cmd[cmd_idx], ret);
#else
            PRINT("read eflash regs[%x] fail:%d \r\n", cfg->read_cmd[cmd_idx], ret);
#endif
            return true;
        }
        value[cmd_idx] = status;
    }
    if ((cfg->write_mask[cfg->cmd_len - 1] & value[cfg->cmd_len - 1]) == cfg->write_mask[cfg->cmd_len - 1]) {
        return false;
    }
#ifdef BUILD_APPLICATION_LOADER
    boot_msg2("check QE reg::", value[0], value[1]);
#else
    PRINT("check QE reg:%x, %x \r\n", value[0], value[1]);
#endif
    value[0] |= cfg->write_mask[0];
    value[1] |= cfg->write_mask[1];
    ret = sfc_port_write_flash_regs(id, cfg->write_cmd, SPI_CMD_WREN, value, cfg->cmd_len);
    if (ret != ERRCODE_SUCC) {
#ifdef BUILD_APPLICATION_LOADER
        boot_msg2("Write eflash regs fail:", cfg->write_cmd, ret);
#else
        PRINT("Write eflash regs[%x] fail:%d \r\n", cfg->write_cmd, ret);
#endif
    }
    return true;
}

static void flash_hw_reset(uint8_t id)
{
    unused(id);
    // fpga fllash power on
    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_FRC_2_REG, LP_CTL_MCU_RB_D_PMU_IOLDO6_EN_MCU_MAN_FRC_OFFSET);
    uapi_tcxo_delay_ms(15); /* wait 15ms */
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_FRC_2_REG, LP_CTL_MCU_RB_D_PMU_IOLDO6_EN_MCU_MAN_FRC_OFFSET);
    uapi_tcxo_delay_ms(15); /* wait 15ms */
}

errcode_t sfc_port_check_and_enable_qe_status(uint8_t id, uint32_t flash_id)
{
#define TRY_CHECK_QE_MAX_TIMES  5
    uint8_t check_cnt;
    flash_cmd_qe_enable_t *qe_cmd = sfc_port_get_flash_qe_cmd(flash_id);
    if (qe_cmd == NULL) {
        return ERRCODE_SFC_FLASH_NOT_SUPPORT;
    }
    for (check_cnt = 0; check_cnt < TRY_CHECK_QE_MAX_TIMES; check_cnt++) {
        if (need_check_qe_status(id, qe_cmd) == false) {
#ifdef BUILD_APPLICATION_LOADER
            boot_msg1("check QE succss:", check_cnt);
#else
            PRINT("check QE succss:%d \r\n", check_cnt);
#endif
            g_flash_id_record[id] = flash_id;
            return ERRCODE_SUCC;
        }
        flash_hw_reset(id);
    }
#ifdef BUILD_APPLICATION_LOADER
    boot_msg0("SET QE FAIL!");
#else
    PRINT("SET QE FAIL!\r\n");
#endif
    return ERRCODE_FAIL;
}

static errcode_t sfc_port_set_protect(uint8_t id, flash_protect_size_cfg *cfg, bool is_volatile)
{
    uint8_t reg_val[0x2];
    uint8_t wren_cmd = is_volatile ? SPI_CMD_VOLATILE_WREN : SPI_CMD_WREN;
    errcode_t ret = sfc_port_read_check_flash_reg(id, &reg_val[0], SPI_CMD_RDSR, cfg->reg0_mask);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = sfc_port_read_check_flash_reg(id, &reg_val[0x1], SPI_CMD_RDSR_2, cfg->reg1_mask);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    if ((cfg->reg0_set == (reg_val[0] & cfg->reg0_mask)) && (cfg->reg1_set == (reg_val[0x1] & cfg->reg1_mask))) {
        return ERRCODE_SUCC;
    }
    reg_val[0] &= ~cfg->reg0_mask;
    reg_val[0] |= cfg->reg0_set;
    reg_val[0x1] &= ~cfg->reg1_mask;
    reg_val[0x1] |= cfg->reg1_set;
    if (g_flash_id_record[id] == FLASH_GD25LE128E) {
        return sfc_port_write_flash_regs(id, SPI_CMD_WDSR, wren_cmd, reg_val, sizeof(reg_val));
    }
    ret = sfc_port_write_flash_regs(id, SPI_CMD_WDSR, wren_cmd, &reg_val[0], 0x1);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return sfc_port_write_flash_regs(id, SPI_CMD_WDSR_2, wren_cmd, &reg_val[0x1], 0x1);
}

errcode_t sfc_port_flash_unprotect(uint8_t id, uint32_t flash_offset, bool is_volatile)
{
    uint8_t idx;
    errcode_t ret;
    uint32_t lock;
    if ((id >= SFC_ID_MAX) || (flash_offset >= FLASH_LENGTH)) {
        return ERRCODE_SFC_INVALID_PARAM;
    }
    if (g_flash_id_record[id] == 0) {
        return ERRCODE_SFC_NOT_INIT;
    }
    if ((g_flash_id_record[id] != FLASH_GD25LE128E) && (g_flash_id_record[id] != FLASH_W25Q128_IM)) {
        return ERRCODE_SFC_FLASH_NOT_SUPPORT;
    }
    if (flash_offset >= g_flash_protect_addr[id]) {
        return ERRCODE_SUCC;  /* already unprotect */
    }
    for (idx = (uint8_t)(sizeof(g_common_protect_lower) / sizeof(flash_protect_size_cfg)); idx > 0; idx--) {
        if (flash_offset >= g_common_protect_lower[idx - 1].addr) {
            break;
        }
    }
    lock = osal_irq_lock();
    isb();
    ret = sfc_port_set_protect(id, &g_common_protect_lower[idx - 1], is_volatile);
    isb();
    osal_irq_restore(lock);
    return ret;
}

errcode_t sfc_port_flash_protect(uint8_t id, bool is_volatile)
{
    errcode_t ret;
    uint32_t lock;
    if (id >= SFC_ID_MAX) {
        return ERRCODE_SFC_INVALID_PARAM;
    }
    if (g_flash_id_record[id] == 0) {
        return ERRCODE_SFC_NOT_INIT;
    }
    if ((g_flash_id_record[id] != FLASH_GD25LE128E) && (g_flash_id_record[id] != FLASH_W25Q128_IM)) {
        return ERRCODE_SFC_FLASH_NOT_SUPPORT;
    }
    lock = osal_irq_lock();
    isb();
    ret = sfc_port_set_protect(id,
        &g_common_protect_lower[sizeof(g_common_protect_lower) / sizeof(flash_protect_size_cfg) - 1], is_volatile);
    isb();
    osal_irq_restore(lock);
    return ret;
}

errcode_t sfc_port_clock_div_set(sfc_id_t id, uint32_t div)
{
    unused(id);
    unused(div);
    return ERRCODE_SUCC;
}

flash_spi_info_t *sfc_get_flash_info(uint32_t flash_id)
{
    flash_spi_info_t *flash_spi_infos = sfc_port_get_flash_spi_infos();
    uint32_t flash_num = sfc_port_get_flash_num();
    for (uint32_t i = 0; i < flash_num; i++) {
        if (flash_id == flash_spi_infos[i].chip_id) {
            return &flash_spi_infos[i];
        }
    }
    return sfc_port_get_unknown_flash_info();
}

errcode_t sfc_port_switch_to_4byte_mode(uint8_t id, uint32_t flash_id, uint32_t chip_size)
{
    unused(flash_id);
    errcode_t ret = ERRCODE_SUCC;
    if (chip_size > FLASH_SIZE_16MB) {
        hal_sfc_regs_set_addr_mode(id, 1); /* 0: 3_byte; 1: 4_byte */
        ret = uapi_sfc_other_flash_opt(id, WRITE_TYPE, 0xb7, (uint8_t *)"", 0);   // 0xb7: enter 4byte mode
    }
    return ret;
}
