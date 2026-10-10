/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Provides sfc driver source \n
 *
 * History: \n
 * 2024-10-29, Create file. \n
 */
#include <stdbool.h>
#include <securec.h>
#include <sfc_porting.h>
#include <hal_sfc.h>
#include "sfc_v2.h"
#include "osal_adapt.h"

#define PAGE_BYTES_MASK           0xFFF
#define SFC_PAGE_BYTES            0x1000
#define SFC_PAGE_PROGRAM_BYTES    0x100
#define ASSIGN_BYTES              (min(SFC_PAGE_PROGRAM_BYTES, MAX_SFC_BYTE))
#define ASSIGN_BYTES_MASK         ((ASSIGN_BYTES) - 1)

#define BYTES_4                   0x4

#define ERASE_CHIP                0
#define GREEDY_MIN_ERASE_NUM      2

static bool g_sfc_inited[SFC_ID_MAX] = {};
static bool g_sfc_unknown_flash[SFC_ID_MAX] = {};
static flash_spi_ctrl_t g_flash_ctrl[SFC_ID_MAX];

#if defined(CONFIG_SFC_SUPPORT_LPM)
static sfc_flash_config_t g_sfc_config_store[SFC_ID_MAX];
#endif  /* CONFIG_SFC_SUPPORT_LPM */

#if defined(CONFIG_SFC_ALLOW_ERASE_WRITEBACK)
static uint8_t g_first_buffer[SFC_ID_MAX][4096];
static uint8_t g_last_buffer[SFC_ID_MAX][4096];
#endif /* CONFIG_SFC_ALLOW_ERASE_WRITEBACK */

#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
bool g_sfc_continue_read_is_inited[SFC_ID_MAX] = {};
#endif
STATIC errcode_t check_init_param(uint8_t id, const sfc_flash_config_t *config)
{
    uint32_t mapping_size = min(config->mapping_size, g_flash_ctrl[id].chip_size);
    if (config->mapping_addr < sfc_port_get_sfc_start_addr(id) ||
        config->mapping_addr + mapping_size - 1 > sfc_port_get_sfc_end_addr(id)) {
        return ERRCODE_SFC_ADDRESS_OVERSTEP;
    }
    g_flash_ctrl[id].chip_size = mapping_size;
    return ERRCODE_SUCC;
}

STATIC errcode_t check_opt_param(uint8_t id, uint32_t addr, uint32_t size)
{
    if (id >= SFC_ID_MAX) {
        return ERRCODE_SFC_INVALID_PARAM;
    }
    if (unlikely(!g_sfc_inited[id])) {
        return ERRCODE_SFC_NOT_INIT;
    }
    if ((addr + size) > g_flash_ctrl[id].chip_size || (addr + size) <= addr || (addr + size) < size) {
        return ERRCODE_INVALID_PARAM;
    }
#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
    if (g_sfc_continue_read_is_inited[id]) {
        return ERRCODE_SFC_IN_CONTINUE_READ;
    }
#endif
    return ERRCODE_SUCC;
}

STATIC errcode_t build_flash_ctrl(uint8_t id, const flash_spi_info_t *spi_info, sfc_read_if_t read_type,
    sfc_write_if_t write_type)
{
    spi_opreation_t read_cmd = spi_info->read_cmds[read_type];
    spi_opreation_t write_cmd = spi_info->write_cmds[write_type];
    if (read_cmd.cmd_support != SPI_CMD_SUPPORT || write_cmd.cmd_support != SPI_CMD_SUPPORT) {
        return ERRCODE_SFC_CMD_NOT_SUPPORT;
    }
    g_flash_ctrl[id].read_opreation = spi_info->read_cmds[read_type];
    g_flash_ctrl[id].write_opreation = spi_info->write_cmds[write_type];
    g_flash_ctrl[id].erase_opreation_array = spi_info->erase_cmds;
    g_flash_ctrl[id].chip_size = spi_info->chip_size;
    if (spi_info->erase_cmd_num < GREEDY_MIN_ERASE_NUM) {
        return ERRCODE_SFC_PORT_INVALID_PARAM;
    }
    g_flash_ctrl[id].erase_cmd_num = spi_info->erase_cmd_num;
    g_flash_ctrl[id].quad_mode = spi_info->quad_mode;
#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
    g_flash_ctrl[id].continue_info = spi_info->continue_info;
#endif
    return ERRCODE_SUCC;
}

STATIC errcode_t build_cmds(uint8_t id, uint32_t flash_id, sfc_read_if_t read_type, sfc_write_if_t write_type)
{
    flash_spi_info_t *flash_spi_infos = sfc_port_get_flash_spi_infos();
    uint32_t flash_num = sfc_port_get_flash_num();
    for (uint32_t i = 0; i < flash_num; i++) {
        if (flash_id == flash_spi_infos[i].chip_id) {
            errcode_t ret = build_flash_ctrl(id, &flash_spi_infos[i], read_type, write_type);
            return ret;
        }
    }
    flash_spi_infos = sfc_port_get_unknown_flash_info();
    errcode_t ret = build_flash_ctrl(id, &flash_spi_infos[0], STANDARD_READ, PAGE_PROGRAM);
    if (ret == ERRCODE_SUCC) {
        g_sfc_unknown_flash[id] = true;
    }
    return ret;
}

STATIC errcode_t do_greedy_erase(uint8_t id, uint32_t hal_erase_size, uint32_t start_sector)
{
    uint32_t erase_opt_index;
    uint32_t temp_size = 0;
    errcode_t ret = ERRCODE_FAIL;
    /* All parameters are aligned in 4KB. */
    uint32_t remain_size = hal_erase_size;
    uint32_t current_addr = start_sector;
    spi_opreation_t current_erase_opt = {0};

    while (remain_size > 0) {
        for (erase_opt_index = 1; erase_opt_index < g_flash_ctrl[id].erase_cmd_num; erase_opt_index++) {
            current_erase_opt = g_flash_ctrl[id].erase_opreation_array[erase_opt_index];
            temp_size = current_erase_opt.size;
            if ((remain_size >= temp_size) && ((current_addr & (temp_size - 1)) == 0)) {
                break;
            }
        }
        /* Generally, the 4K erase is not configured for this branch. Check the erase array at the port layer. */
        if (erase_opt_index == g_flash_ctrl[id].erase_cmd_num) {
#if defined(CONFIG_SFC_SUPPORT_COMPLECT)
            erase_opt_index--;
            current_erase_opt = g_flash_ctrl[id].erase_opreation_array[erase_opt_index];
            temp_size = current_erase_opt.size;
            remain_size = 0;
#else
            return ERRCODE_SFC_ERASE_FORM_ERROR;
#endif
        }
        uint32_t lock_sts = sfc_port_lock(id);
        ret = hal_sfc_reg_erase(id, current_addr, current_erase_opt, false);
        sfc_port_unlock(id, lock_sts);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
        if (remain_size >=  temp_size) {
            remain_size -= temp_size;
        }

        current_addr += temp_size;
    }
    return ERRCODE_SUCC;
}

// 决策是否切换到4byte模式，需在porting中自行实现
__attribute__((weak)) errcode_t sfc_port_switch_to_4byte_mode(uint8_t id, uint32_t flash_id, uint32_t chip_size)
{
    unused(id);
    unused(flash_id);
    unused(chip_size);
    return ERRCODE_SUCC;
}

errcode_t uapi_sfc_init(uint8_t sfc_id, sfc_flash_config_t *config)
{
    uint8_t id = sfc_id;
    errcode_t ret;

    if ((id >= SFC_ID_MAX) || (config == NULL)) {
        return ERRCODE_SFC_INVALID_PARAM;
    }
    if (unlikely(g_sfc_inited[id])) {
        return ERRCODE_SFC_ALREADY_INIT;
    }

#if defined(CONFIG_SFC_SUPPORT_COMPLECT)
    hal_sfc_complect_init(id);
#endif
#if defined(CONFIG_SFC_SUPPORT_LPM)
    memcpy_s(&g_sfc_config_store[id], sizeof(sfc_flash_config_t), config, sizeof(sfc_flash_config_t));
#endif  /* CONFIG_SFC_SUPPORT_LPM */
    sfc_port_lock_init(id);

    uint32_t flash_id;
    ret = hal_sfc_get_flash_id(id, &flash_id);
    if (unlikely(ret != ERRCODE_SUCC)) {
        return ret;
    }
    ret = build_cmds(id, flash_id, config->read_type, config->write_type);
    if (unlikely(ret != ERRCODE_SUCC)) {
        return ret;
    }
    ret = check_init_param(id, config);
    if (unlikely(ret != ERRCODE_SUCC)) {
        return ret;
    }
#if !defined(CONFIG_SFC_ALREADY_INIT)
    g_flash_ctrl[id].flash_id = flash_id;
    flash_spi_ctrl_t *spi_ctrl = &g_flash_ctrl[id];
    ret = hal_sfc_init(id, spi_ctrl, config->mapping_addr, g_flash_ctrl[id].chip_size);
    if (ret == ERRCODE_SUCC) {
        g_sfc_inited[id] = true;
        ret = g_sfc_unknown_flash[id] ? ERRCODE_SFC_FLASH_NOT_SUPPORT : ERRCODE_SUCC;
    }
    sfc_port_switch_to_4byte_mode(id, spi_ctrl->flash_id, spi_ctrl->chip_size);
    return ret;
#else
    g_sfc_inited[id] = true;
    return ERRCODE_SUCC;
#endif
}

void uapi_sfc_deinit(uint8_t sfc_id)
{
    uint8_t id = sfc_id;
    if ((id >= SFC_ID_MAX) || unlikely(!g_sfc_inited[id])) {
        return;
    }

#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
    if (g_sfc_continue_read_is_inited[id]) {
        uapi_sfc_disable_continue_read(id);
    }
#endif

#if !defined(CONFIG_SFC_ALREADY_INIT)
    hal_sfc_deinit(id);
#endif

    g_sfc_inited[id] = false;
    g_sfc_unknown_flash[id] = false;
}

errcode_t uapi_sfc_read(uint8_t sfc_id, uint32_t flash_addr, uint8_t *read_buffer, uint32_t read_size)
{
    uint8_t id = sfc_id;
    errcode_t ret = check_opt_param(id, flash_addr, read_size);
    if ((ret != ERRCODE_SUCC) && (ret != ERRCODE_SFC_IN_CONTINUE_READ)) {
        return ret;
    }

    /* Unaligned length */
    uint32_t temp_addr = flash_addr;
    uint8_t *buffer_ptr = read_buffer;
    uint32_t current_size = read_size & ASSIGN_BYTES_MASK;
    uint32_t read_times = (read_size / ASSIGN_BYTES) + 1;
    /* Cyclic read. After the first read, 64 bytes are read. */
    uint32_t lock_sts = sfc_port_lock(id);
    for (uint32_t i = 0; i < read_times; i++) {
        if (current_size == 0) {
            /* Read_size is already aligned, skip one read. */
            current_size = ASSIGN_BYTES;
            continue;
        }
        ret = hal_sfc_reg_read(id, temp_addr, buffer_ptr, current_size, g_flash_ctrl[id].read_opreation);
        if (unlikely(ret != ERRCODE_SUCC)) {
            sfc_port_unlock(id, lock_sts);
            return ret;
        }
        temp_addr += current_size;
        buffer_ptr += current_size;
        current_size = ASSIGN_BYTES;
    }
    sfc_port_unlock(id, lock_sts);
    return ERRCODE_SUCC;
}

errcode_t uapi_sfc_write(uint8_t sfc_id, uint32_t flash_addr, uint8_t *write_data, uint32_t write_size)
{
    uint8_t id = sfc_id;
    errcode_t ret = check_opt_param(id, flash_addr, write_size);
    if (unlikely(ret != ERRCODE_SUCC)) {
        return ret;
    }

    if (g_flash_ctrl[id].write_opreation.cmd == SFC_INVALID_CMD) {
        return ERRCODE_SFC_CMD_NOT_SUPPORT;
    }

    uint32_t unaligned_size = ASSIGN_BYTES - (flash_addr & ASSIGN_BYTES_MASK);
    if (write_size < unaligned_size) {
        unaligned_size = write_size;
    }
    uint32_t temp_addr = flash_addr;
    uint8_t *buffer_ptr = write_data;
    /* Part 1: The start address is not aligned. */
    uint32_t lock_sts = sfc_port_lock(id);
    ret = hal_sfc_reg_write(id, temp_addr, buffer_ptr, unaligned_size, g_flash_ctrl[id].write_opreation);
    if (unlikely(ret != ERRCODE_SUCC)) {
        sfc_port_unlock(id, lock_sts);
        return ret;
    }
    uint32_t remained_size = write_size - unaligned_size;
    buffer_ptr += unaligned_size;
    temp_addr += unaligned_size;
    /*
     * Part 2: Follow-up data processing. The cross-page problem does not need to be considered.
     * Any flash memory larger than 64 bytes/page can be used.
     */
    while (remained_size > 0) {
        uint32_t current_size = remained_size <= ASSIGN_BYTES ? remained_size : ASSIGN_BYTES;
        ret = hal_sfc_reg_write(id, temp_addr, buffer_ptr, current_size, g_flash_ctrl[id].write_opreation);
        if (unlikely(ret != ERRCODE_SUCC)) {
            sfc_port_unlock(id, lock_sts);
            return ret;
        }
        buffer_ptr += current_size;
        temp_addr += current_size;
        remained_size -= current_size;
    }
#ifdef CONFIG_SUPPORT_DATA_CACHE
        osal_dcache_flush_all();
#endif
    sfc_port_unlock(id, lock_sts);

    return ERRCODE_SUCC;
}

#if defined(CONFIG_SFC_SUPPORT_DMA)
errcode_t uapi_sfc_dma_read(uint8_t id, uint32_t flash_addr, uint8_t *read_buffer, uint32_t read_size)
{
    errcode_t ret = check_opt_param(id, flash_addr, read_size);
    if (unlikely(ret != ERRCODE_SUCC)) {
        return ret;
    }
    uint32_t lock_sts = sfc_port_lock(id);
    ret = hal_sfc_dma_read(id, flash_addr, read_buffer, read_size);
    sfc_port_unlock(id, lock_sts);
    return ret;
}

errcode_t uapi_sfc_dma_write(uint8_t id, uint32_t flash_addr, uint8_t *write_buffer, uint32_t write_size)
{
    errcode_t ret = check_opt_param(id, flash_addr, write_size);
    if (unlikely(ret != ERRCODE_SUCC)) {
        return ret;
    }

    if (g_flash_ctrl[id].write_opreation.cmd == SFC_INVALID_CMD) {
        return ERRCODE_SFC_CMD_NOT_SUPPORT;
    }

    uint32_t lock_sts = sfc_port_lock(id);
    ret = hal_sfc_dma_write(id, flash_addr, write_buffer, write_size);
    sfc_port_unlock(id, lock_sts);

#ifdef CONFIG_SUPPORT_DATA_CACHE
        osal_dcache_flush_all();
#endif
    return ret;
}
#endif /* CONFIG_SFC_SUPPORT_DMA */

errcode_t uapi_sfc_erase(uint8_t sfc_id, uint32_t flash_addr, uint32_t erase_size)
{
    uint8_t id = sfc_id;
    errcode_t ret = check_opt_param(id, flash_addr, erase_size);
    if (unlikely(ret != ERRCODE_SUCC)) {
        return ret;
    }

    uint32_t end_addr = flash_addr + erase_size;
    uint32_t start_sector = flash_addr & ~PAGE_BYTES_MASK;
    uint32_t end_sector = (end_addr & PAGE_BYTES_MASK) == 0 ? end_addr : (end_addr & ~PAGE_BYTES_MASK) + SFC_PAGE_BYTES;
    uint32_t hal_erase_size = end_sector - start_sector;

#if defined(CONFIG_SFC_ALLOW_ERASE_WRITEBACK)
    /* Backup data to RAM */
    uint32_t first_size = flash_addr - start_sector;
    if (likely(first_size != 0)) {
        uapi_sfc_read(id, start_sector, g_first_buffer[id], first_size);
    }
    uint32_t last_size = end_sector - end_addr;
    if (likely(last_size != 0)) {
        uapi_sfc_read(id, end_addr, g_last_buffer[id], last_size);
    }
#else
    if (flash_addr != start_sector || end_addr != end_sector) {
        return ERRCODE_INVALID_PARAM;
    }
#endif /* CONFIG_SFC_ALLOW_ERASE_WRITEBACK */

    /* Erasing with greedy algorithms */
    ret = do_greedy_erase(id, hal_erase_size, start_sector);

#if defined(CONFIG_SFC_ALLOW_ERASE_WRITEBACK)
    /* Write back data from RAM */
    if (likely(first_size != 0)) {
        uapi_sfc_write(id, start_sector, g_first_buffer[id], first_size);
    }
    if (likely(last_size != 0)) {
        uapi_sfc_write(id, end_addr, g_last_buffer[id], last_size);
    }
#endif /* CONFIG_SFC_ALLOW_ERASE_WRITEBACK */

#ifdef CONFIG_SUPPORT_DATA_CACHE
    osal_dcache_flush_all();
#endif
    return ret;
}

errcode_t uapi_sfc_erase_chip(uint8_t sfc_id)
{
    uint8_t id = sfc_id;
    if (id >= SFC_ID_MAX) {
        return ERRCODE_SFC_INVALID_PARAM;
    }
    if (unlikely(!g_sfc_inited[id])) {
        return ERRCODE_SFC_NOT_INIT;
    }

#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
    if (g_sfc_continue_read_is_inited[id]) {
        return ERRCODE_SFC_IN_CONTINUE_READ;
    }
#endif

    uint32_t lock_sts = sfc_port_lock(id);
    errcode_t ret = hal_sfc_reg_erase(id, 0x0, g_flash_ctrl[id].erase_opreation_array[ERASE_CHIP], true);
    sfc_port_unlock(id, lock_sts);

#ifdef CONFIG_SUPPORT_DATA_CACHE
    osal_dcache_flush_all();
#endif
    return ret;
}

errcode_t uapi_sfc_other_flash_opt(uint8_t sfc_id, sfc_flash_op_t cmd_type, uint8_t cmd, uint8_t *buffer,
    uint32_t length)
{
    uint8_t id = sfc_id;
    spi_opreation_t opt = {0};

    if ((id >= SFC_ID_MAX) || (buffer == NULL)) {
        return ERRCODE_SFC_INVALID_PARAM;
    }

    if (unlikely(!g_sfc_inited[id])) {
        return ERRCODE_SFC_NOT_INIT;
    }

#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
    if (g_sfc_continue_read_is_inited[id]) {
        return ERRCODE_SFC_IN_CONTINUE_READ;
    }
#endif

    opt.cmd_support = SPI_CMD_SUPPORT;
    opt.cmd = cmd;

    uint32_t lock_sts = sfc_port_lock(id);
    errcode_t ret = hal_sfc_reg_flash_opreations(id, cmd_type, opt, buffer, length);
    sfc_port_unlock(id, lock_sts);
    return ret;
}

errcode_t uapi_sfc_get_flash_info(uint8_t id, sfc_info_t *info)
{
    if ((id >= SFC_ID_MAX) || (info == NULL)) {
        return ERRCODE_SFC_INVALID_PARAM;
    }

#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
    if (g_sfc_continue_read_is_inited[id]) {
        return ERRCODE_SFC_IN_CONTINUE_READ;
    }
#endif

    info->flash_size = g_flash_ctrl[id].chip_size;

    uint32_t lock_sts = sfc_port_lock(id);
    errcode_t ret = hal_sfc_get_flash_id(id, (uint32_t *)(&(info->flash_id)));
    sfc_port_unlock(id, lock_sts);
    return ret;
}

#if defined(CONFIG_SFC_SUPPORT_LPM)
errcode_t uapi_sfc_suspend(uint8_t sfc_id, uintptr_t arg)
{
    uint8_t id = sfc_id;
    unused(arg);
    hal_sfc_suspend(id);
    return ERRCODE_SUCC;
}

errcode_t uapi_sfc_resume(uint8_t sfc_id, uintptr_t arg)
{
    uint8_t id = sfc_id;
    unused(arg);
    sfc_port_switch_to_4byte_mode(id, g_flash_ctrl[id].flash_id, g_flash_ctrl[id].chip_size);
    return hal_sfc_resume(id, g_flash_ctrl[id].quad_mode);
}
#endif  /* CONFIG_SFC_SUPPORT_LPM */

#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
/*
 * Be careful when using the interface. When the continuous read function is enabled,
 * all flash operations except the current read function are prohibited.
 * Otherwise, various problems may occur.
 */
errcode_t uapi_sfc_enable_continue_read(uint8_t id)
{
    if (id >= SFC_ID_MAX) {
        return ERRCODE_SFC_INVALID_PARAM;
    }
    if (unlikely(!g_sfc_inited[id])) {
        return ERRCODE_SFC_NOT_INIT;
    }
    if (g_sfc_continue_read_is_inited[id]) {
        return ERRCODE_SUCC;
    }

    if (((g_flash_ctrl[id].read_opreation.iftype != DUAL_IO_SPI) &&
        (g_flash_ctrl[id].read_opreation.iftype != QUAL_IO_SPI)) ||
        !(g_flash_ctrl[id].continue_info.is_support)) {
        return ERRCODE_SFC_CMD_NOT_SUPPORT;
    }

    uint32_t lock_sts = sfc_port_lock(id);
    hal_sfc_enable_continue_read(id, g_flash_ctrl[id].read_opreation, g_flash_ctrl[id].continue_info.m_byte);
    g_sfc_continue_read_is_inited[id] = true;
    sfc_port_unlock(id, lock_sts);

    return ERRCODE_SUCC;
}

void uapi_sfc_disable_continue_read(uint8_t id)
{
    if (!g_sfc_continue_read_is_inited[id]) {
        return;
    }

    uint32_t lock_sts = sfc_port_lock(id);
    hal_sfc_disable_continue_read(id, g_flash_ctrl[id].read_opreation);
    g_sfc_continue_read_is_inited[id] = false;
    sfc_port_unlock(id, lock_sts);
}
#endif

#if defined(CONFIG_SFC_SUPPORT_DEEP_POWERDOWN)
errcode_t uapi_sfc_switch_to_deeppower(uint8_t id)
{
    return hal_sfc_enter_deep_powerdown(id);
}

errcode_t uapi_sfc_resume_from_deeppower(uint8_t id)
{
    return hal_sfc_release_from_deep_powerdown(id);
}
#endif

#ifdef SUPPORT_SFC_READ_UNIQUE_ID
uint32_t g_unique_id[SFC_ID_MAX][UNIQUE_ID_MAX_LEN];
uint16_t g_unique_id_len[SFC_ID_MAX];
errcode_t uapi_sfc_read_unique_id(uint8_t id, uintptr_t *unique_id, uint16_t *len)
{
    errcode_t ret;
    sfc_unique_id_cmd uqi_cmd;
    spi_opreation_t opt = {0};

    if (unlikely(!g_sfc_inited[id])) {
        return ERRCODE_SFC_NOT_INIT;
    }

#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
    if (g_sfc_continue_read_is_inited[id]) {
        return ERRCODE_SFC_IN_CONTINUE_READ;
    }
#endif

    /* unique id has been read */
    if (g_unique_id_len[id] > 0) {
        *unique_id = (uintptr_t)(g_unique_id[id]);
        *len = g_unique_id_len[id];
        return ERRCODE_SUCC;
    }

    uqi_cmd = sfc_unique_match(g_flash_ctrl[id].flash_id);
    if (uqi_cmd.cmd == INVALID_CMD) {
        return ERRCODE_SFC_FLASH_NOT_SUPPORT;
    }
    opt.cmd_support = SPI_CMD_SUPPORT;
    opt.cmd = uqi_cmd.cmd;
    opt.size = uqi_cmd.dummy;

    uint32_t lock_sts = sfc_port_lock(id);
    ret = hal_sfc_reg_flash_opreations(id, READ_TYPE, opt, (uint8_t *)(g_unique_id[id]), uqi_cmd.byte);
    sfc_port_unlock(id, lock_sts);
    if (ret != ERRCODE_SUCC) {
        *len = 0;
        return ret;
    }
    g_unique_id_len[id] = uqi_cmd.byte;
#ifdef CONFIG_SFC_SUPPORT_COMPLECT
    uint32_t unique_id_temp[UNIQUE_ID_MAX_LEN] = {0};
    uint32_t unique_id_len_max = UNIQUE_ID_MAX_LEN * sizeof(uint32_t);
    memcpy_s(unique_id_temp, unique_id_len_max, g_unique_id[0], unique_id_len_max);
    for (uint32_t i = 0; i < uqi_cmd.byte / FLASH_DOUBLE_BYTE; i++) {
        ((uint8_t *)g_unique_id[0])[i] = ((uint8_t *)unique_id_temp)[i * FLASH_DOUBLE_BYTE];
        ((uint8_t *)g_unique_id[0])[i + uqi_cmd.byte / FLASH_DOUBLE_BYTE] =
        ((uint8_t *)unique_id_temp)[i * FLASH_DOUBLE_BYTE + 1];
    }
    g_unique_id_len[id] = uqi_cmd.byte / FLASH_DOUBLE_BYTE;
#endif
    *unique_id = (uintptr_t)(g_unique_id[id]);
    *len = g_unique_id_len[id];
    return ERRCODE_SUCC;
}
#endif