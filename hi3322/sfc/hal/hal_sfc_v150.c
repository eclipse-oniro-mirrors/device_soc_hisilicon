/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Provides SFC HAL source \n
 *
 * History: \n
 * 2024-10-01, Create file. \n
 */
#include <stdint.h>
#include <stdbool.h>
#ifdef SUPPORT_SFC_WRITE_NOT_BLOCKED
#include "cmsis_os.h"
#include "interrupt.h"
#include "osal_interrupt.h"
#endif
#include "tcxo.h"
#include "watchdog.h"
#include "securec.h"
#include "hal_sfc_v150_regs_op.h"
#include "hal_sfc.h"

#define HAL_WAIT_FLASH_BUSY_CNT 0xFFFF
#define CMD_QE_BIT_OFF           0x1

#define STANDARD_SPI             0x0

#define ENABLE                   0x1
#define DISABLE                  0x0

#define READ_MODE                0x1
#define WRITE_MODE               0x0

#define FLASH_CMD_INDEX          0x0
#define FLASH_CMD_OFFSET         0x1
#define FLASH_CMD_BIT            0x2

#define FLASH_OPREATION_MAX_NUM  0x2
#define ADDR_SHIFT_BITS          16

#define FLASH_SIZE_D_VALUE        0xF

#define SFC_V150_MIN_SIZE        0x10000
#define FLASH_ID_MASK            0xFFFFFF

#define CHIP_ERASE_WAIT_ROUND    50

#define BYTE_IN_WORD             4

#if defined(CONFIG_SFC_SUPPORT_LPM)
#define SFC_BUS_CONFIG_REG_NUM   5
static uint32_t g_sfc_suspend_regs[SFC_ID_MAX][SFC_BUS_CONFIG_REG_NUM];
#endif

errcode_t hal_sfc_regs_wait_ready(uint8_t id, uint8_t wip_bit)
{
    uint32_t dead_line = 0;
    uint32_t timeout = sfc_port_get_delay_times(id);
    uint32_t delay = sfc_port_get_delay_once_time(id);
    hal_spi_opreation_t hal_opreation;
    hal_opreation.opt.cmd = SPI_CMD_RDSR;
    hal_opreation.opt.iftype = STANDARD_SPI;
#ifdef CONFIG_SFC_SUPPORT_COMPLECT
    hal_opreation.data_size = FLASH_DOUBLE_BYTE;
#else
    hal_opreation.data_size = 1;
#endif
    hal_opreation.dummy_byte = 0;
    do {
        hal_sfc_regs_set_opt(id, hal_opreation);
        hal_sfc_regs_set_opt_attr(id, READ_MODE, ENABLE, DISABLE);
        hal_sfc_regs_wait_config(id);
        uint32_t reg_val = hal_sfc_regs_get_databuf(id, 0);
#ifdef CONFIG_SFC_SUPPORT_COMPLECT
        if (((reg_val >> wip_bit) & FLASH_WIP_READY_BIT) == 0) {
            return ERRCODE_SUCC;
        }
#else
        if (((reg_val >> wip_bit) & 0x1) == 0) {
            return ERRCODE_SUCC;
        }
#endif
        uapi_tcxo_delay_us(delay);
    } while (dead_line++ < timeout);

    return ERRCODE_SFC_FLASH_TIMEOUT_WAIT_READY;
}

STATIC errcode_t hal_sfc_execute_type_cmd(uint8_t id, uint8_t cmd_len, uint8_t* cmd)
{
    hal_spi_opreation_t hal_opreation = { {SPI_CMD_SUPPORT, cmd[FLASH_CMD_INDEX], STANDARD_SPI, 0x0}, cmd_len - 1, 0 };
    uint32_t data_en = cmd_len > 1 ? ENABLE : DISABLE;
    errcode_t ret = hal_sfc_regs_wait_ready(id, 0x0);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    hal_sfc_regs_set_opt(id, hal_opreation);
    cmd_databuf_t databuf;
    databuf.d32 = 0;
    for (uint32_t i = 1; i < cmd_len; i++) {
        databuf.b.databyte[i - 1] = cmd[i];
    }
    hal_sfc_regs_set_databuf(id, 0, databuf.d32);
    hal_sfc_regs_set_opt_attr(id, WRITE_MODE, data_en, DISABLE);
    hal_sfc_regs_wait_config(id);
    return ERRCODE_SUCC;
}

#if defined(CONFIG_SFC_SUPPORT_LPM)
#ifdef CONFIG_SFC_SUPPORT_COMPLECT
STATIC errcode_t hal_sfc_execute_type_proc(uint8_t id, uint8_t cmd, uint8_t offset, uint8_t bit)
{
    uint32_t count = HAL_WAIT_FLASH_BUSY_CNT;
#ifdef CONFIG_SFC_SUPPORT_COMPLECT
    uint32_t data_size = FLASH_DOUBLE_BYTE;
#else
    uint32_t data_size = 1;
#endif
    while (count-- > 0) {
        hal_spi_opreation_t hal_opreation = { {SPI_CMD_SUPPORT, cmd, STANDARD_SPI, 0x0}, data_size, 0 };
        hal_sfc_regs_set_opt(id, hal_opreation);
        hal_sfc_regs_set_opt_attr(id, READ_MODE, ENABLE, DISABLE);
        hal_sfc_regs_wait_config(id);
        uint32_t data = hal_sfc_regs_get_databuf(id, 0);
        if (((data >> offset) & 0x1) == bit) {
            return ERRCODE_SUCC;
        }
    }

    return ERRCODE_SFC_CMD_ERROR;
}

STATIC errcode_t hal_sfc_execute_cmds(uint8_t id, flash_cmd_execute_t *command)
{
    errcode_t ret;
    flash_cmd_execute_t *current_cmd = command;
    while (current_cmd != NULL) {
        switch ((current_cmd->cmd_type)) {
            case FLASH_CMD_TYPE_CMD:
                ret = hal_sfc_execute_type_cmd(id, current_cmd->cmd_len, current_cmd->cmd);
                if (ret != ERRCODE_SUCC) {
                    return ret;
                }
                break;
            case FLASH_CMD_TYPE_PROCESSING:
                ret = hal_sfc_execute_type_proc(id, current_cmd->cmd[FLASH_CMD_INDEX],
                                                current_cmd->cmd[FLASH_CMD_OFFSET], current_cmd->cmd[FLASH_CMD_BIT]);
                if (ret != ERRCODE_SUCC) {
                    return ret;
                }
                break;
            case FLASH_CMD_TYPE_END:
                return ERRCODE_SUCC;
            default:
                return ERRCODE_SFC_CMD_ERROR;
        }
        current_cmd++;
    }
    return ERRCODE_SUCC;
}
#endif
#endif

errcode_t hal_sfc_write_enable(uint8_t id, uint8_t cmd)
{
    errcode_t ret;
    ret = hal_sfc_execute_type_cmd(id, 1, &cmd);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    if (cmd == SPI_CMD_VOLATILE_WREN) {
        return ERRCODE_SUCC;
    }
    return hal_sfc_regs_wait_ready(id, 0x0);
}
#if defined(CONFIG_SFC_SUPPORT_DMA)
bool g_dma_busy[SFC_ID_MAX] = {};
errcode_t hal_sfc_dma_read(uint8_t id, uint32_t flash_addr, uint8_t *read_buffer, uint32_t read_size)
{
    if (g_dma_busy[id] == true) {
        return ERRCODE_SFC_DMA_BUSY;
    }
    g_dma_busy[id] = true;

    hal_sfc_regs_set_bus_dma_flash_saddr(id, flash_addr);
    hal_sfc_regs_set_bus_dma_mem_addr(id, read_buffer);
    hal_sfc_regs_set_bus_dma_len(id, read_size);
    hal_sfc_regs_set_bus_dma_ahb_ctrl(id);
    hal_sfc_regs_set_bus_dma_ctrl(id, READ_MODE);
    hal_sfc_dma_wait_done(id);
    g_dma_busy[id] = false;
    return ERRCODE_SUCC;
}

errcode_t hal_sfc_dma_write(uint8_t id, uint32_t flash_addr, uint8_t *write_data, uint32_t write_size)
{
    if (g_dma_busy[id] == true) {
        return ERRCODE_SFC_DMA_BUSY;
    }
    g_dma_busy[id] = true;
    errcode_t ret;
    ret = hal_sfc_write_enable(id, SPI_CMD_WREN);
    if (ret != ERRCODE_SUCC) {
        g_dma_busy[id] = false;
        return ret;
    }
    hal_sfc_regs_set_bus_dma_flash_saddr(id, flash_addr);
    hal_sfc_regs_set_bus_dma_mem_addr(id, write_data);
    hal_sfc_regs_set_bus_dma_len(id, write_size);
    hal_sfc_regs_set_bus_dma_ahb_ctrl(id);
    hal_sfc_regs_set_bus_dma_ctrl(id, WRITE_MODE);
    hal_sfc_dma_wait_done(id);
    g_dma_busy[id] = false;
    return ERRCODE_SUCC;
}
#endif

#ifdef CONFIG_SFC_SUPPORT_COMPLECT
errcode_t hal_sfc_get_flash_id(uint8_t id, uint32_t *flash_id)
{
    uint32_t flash_0_id = 0;
    uint32_t flash_1_id = 0;
    uint32_t buf_0 = 0;
    uint32_t buf_1 = 0;
    uint8_t id_temp = id;
    id = 0;
    hal_spi_opreation_t hal_opreation = { {SPI_CMD_SUPPORT, SPI_CMD_RDID, STANDARD_SPI, 0x0}, FLASH_ID_READ_LEN, 0 };
    hal_sfc_regs_set_opt(id, hal_opreation);
    hal_sfc_regs_set_opt_attr(id, READ_MODE, ENABLE, DISABLE);
    hal_sfc_regs_wait_config(id);
    /*
     * complect flash id example: 0xAABBCC
     * buf0 -> 0xBBBBCCCC
     * buf1 ->0x0000AAAA
     */
    buf_0 = hal_sfc_regs_get_databuf(id, 0);
    buf_1 = hal_sfc_regs_get_databuf(id, 1);

    flash_0_id = (buf_0 & 0xFF) | ((buf_0 & 0xFF0000) >> FLASH_ID_SHIFT_8) | ((buf_1 & 0xFF) << FLASH_ID_SHIFT_16);
    flash_1_id = ((buf_0 & 0xFF00) >> FLASH_ID_SHIFT_8) | ((buf_0 & 0xFF000000) >> FLASH_ID_SHIFT_16) |
        ((buf_1 & 0xFF00) << (FLASH_ID_SHIFT_8));

    if (id_temp == 0) {
        *flash_id = FLASH_ID_MASK & flash_0_id;
    } else {
        *flash_id = FLASH_ID_MASK & flash_1_id;
    }
    if (flash_0_id != flash_1_id) {
        return ERRCODE_SFC_FLASH_NOT_SUPPORT;
    }
    return ERRCODE_SUCC;
}

void hal_sfc_complect_init(uint8_t id)
{
    (void)hal_sfc_regs_init(id);
    hal_sfc_regs_enable_complect();
}
#else
errcode_t hal_sfc_get_flash_id(uint8_t id, uint32_t *flash_id)
{
    (void)hal_sfc_regs_init(id);
    hal_spi_opreation_t hal_opreation = { {SPI_CMD_SUPPORT, SPI_CMD_RDID, STANDARD_SPI, 0x0}, 0x3, 0 };
    hal_sfc_regs_set_opt(id, hal_opreation);
    hal_sfc_regs_set_opt_attr(id, READ_MODE, ENABLE, DISABLE);
    hal_sfc_regs_wait_config(id);
    *flash_id = FLASH_ID_MASK & hal_sfc_regs_get_databuf(id, 0);
    return hal_sfc_regs_wait_ready(id, 0x0); /* 0 : flash status bit */
}
#endif

STATIC uint32_t get_size(uint32_t size)
{
    uint32_t ret = 0;
    uint32_t size_tmp = size;
    while ((size_tmp & 0x1) == 0) {
        size_tmp >>= 1;
        ret++;
    }
    return ret;
}

errcode_t hal_sfc_init(uint8_t id, flash_spi_ctrl_t *spi_ctrl, uint32_t mapping, uint32_t flash_size)
{
    if (flash_size < SFC_V150_MIN_SIZE) {
        return ERRCODE_INVALID_PARAM;
    }
#ifndef CONFIG_SFC_SUPPORT_COMPLECT
    errcode_t ret = sfc_port_check_and_enable_qe_status(id, spi_ctrl->flash_id);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
#else
    errcode_t ret = hal_sfc_execute_cmds(id, spi_ctrl->quad_mode);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
#endif
    uint32_t addr = mapping >> ADDR_SHIFT_BITS;
    hal_sfc_regs_set_bus_baseaddr(id, addr);
    uint32_t hal_flash_size = get_size(flash_size >> FLASH_SIZE_D_VALUE);
    hal_sfc_regs_set_bus_flash_size(id, hal_flash_size);
    hal_sfc_regs_set_bus_read(id, spi_ctrl->read_opreation);
    if (spi_ctrl->write_opreation.cmd != 0) {
        hal_sfc_regs_set_bus_write(id, spi_ctrl->write_opreation);
    } else {
        hal_sfc_regs_disable_bus_write(id);
    }
    /*
     * The flash requires that tshsl parameter should be greater than 50ns. In max freq-130M,
     * tshsl: (5 + 2)*1000/130=53ns >50ns
     */
    hal_sfc_regs_set_timing(id, 0x1105);
    hal_sfc_regs_wait_ready(id, 0);
    return ERRCODE_SUCC;
}

void hal_sfc_deinit(uint8_t id)
{
    hal_sfc_regs_deinit(id);
}

errcode_t hal_sfc_reg_read(uint8_t id, uint32_t flash_addr, uint8_t *read_buffer, uint32_t read_size,
                           spi_opreation_t read_opreation)
{
    if (unlikely(read_size > MAX_SFC_BYTE)) {
        return ERRCODE_INVALID_PARAM;
    }

    uint32_t dummy = read_opreation.size;
#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
    if (hal_sfc_get_continue_read_status(id) && (dummy > 0)) {
        dummy--;
    }
#endif
    hal_spi_opreation_t opreation = {
        .opt = read_opreation,
        .data_size = read_size,
        .dummy_byte = dummy
    };
    hal_sfc_regs_set_opt(id, opreation);
    hal_sfc_regs_set_cmd_addr(id, flash_addr);
    hal_sfc_regs_set_opt_attr(id, READ_MODE, ENABLE, ENABLE);
    hal_sfc_regs_wait_config(id);

    uint32_t read_buffer_tmp[MAX_DATABUF_NUM] = {0};
    uint32_t end_pos = read_size >> 2;
    for (uint32_t i = 0; i < end_pos; i++) {
        read_buffer_tmp[i] = hal_sfc_regs_get_databuf(id, i);
    }
    if (likely((read_size & 0x3) != 0)) {
        read_buffer_tmp[end_pos] = hal_sfc_regs_get_databuf(id, end_pos);
    }
    if (memcpy_s(read_buffer, read_size, (uint8_t *)read_buffer_tmp, read_size) != EOK) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t hal_sfc_reg_write(uint8_t id, uint32_t flash_addr, uint8_t *write_data, uint32_t write_size,
                            spi_opreation_t write_opreation)
{
    errcode_t ret;
    if (unlikely(write_size > MAX_SFC_BYTE)) {
        return ERRCODE_INVALID_PARAM;
    }
    ret = hal_sfc_write_enable(id, SPI_CMD_WREN);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
#ifdef CONFIG_SUPPORT_FLASH_SUSPEND_RESUME
    return sfc_port_sfc_reg_write(id, flash_addr, write_data, write_size, write_opreation);
#else
    hal_spi_opreation_t opreation = {
        .opt = write_opreation,
        .data_size = write_size,
        .dummy_byte = write_opreation.size
    };
    hal_sfc_regs_set_opt(id, opreation);
    hal_sfc_regs_set_cmd_addr(id, flash_addr);
    uint32_t write_buffer_tmp[MAX_DATABUF_NUM] = {0};
    uint32_t end_pos = write_size >> 2;

    int ret_val = memcpy_s((uint8_t *)write_buffer_tmp, MAX_DATABUF_NUM * sizeof(uint32_t), write_data, write_size);
    if (ret_val == EOVERLAP_AND_RESET) {
        /*
         * 在write_buffer_tmp和write_data存在重叠的情况下，使用memmove_s拷贝
         * 考虑到memmove_s比memcpy_s性能略有降低，重叠情况属于低概率场景，仅在memcpy_s异常时使用memmove_s
         */
        if (memmove_s((uint8_t *)write_buffer_tmp, MAX_DATABUF_NUM * sizeof(uint32_t), write_data, write_size) != EOK) {
            return ERRCODE_MEMCPY;
        }
    } else if (ret_val != EOK) {
        return ERRCODE_MEMCPY;
    }

    for (uint32_t i = 0; i < end_pos; i++) {
        hal_sfc_regs_set_databuf(id, i, write_buffer_tmp[i]);
    }
    if (likely((write_size & 0x3) != 0)) {
        hal_sfc_regs_set_databuf(id, end_pos, write_buffer_tmp[end_pos]);
    }
    hal_sfc_regs_set_opt_attr(id, WRITE_MODE, ENABLE, ENABLE);
    hal_sfc_regs_wait_config(id);
    return hal_sfc_regs_wait_ready(id, 0x0);
#endif
}

errcode_t hal_sfc_reg_erase(uint8_t id, uint32_t flash_addr, spi_opreation_t erase_opreation, bool delete_chip)
{
    errcode_t ret;
    uint32_t addr_en = delete_chip ? DISABLE : ENABLE;
    ret = hal_sfc_write_enable(id, SPI_CMD_WREN);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
#ifdef CONFIG_SUPPORT_FLASH_SUSPEND_RESUME
    return sfc_port_sfc_reg_erase(id, flash_addr, erase_opreation, addr_en);
#else
    hal_spi_opreation_t opreation = { .opt = erase_opreation, .data_size = 0x0, .dummy_byte = 0x0};
    hal_sfc_regs_set_opt(id, opreation);
    hal_sfc_regs_set_cmd_addr(id, flash_addr);
    hal_sfc_regs_set_opt_attr(id, WRITE_MODE, DISABLE, addr_en);
    hal_sfc_regs_wait_config(id);
    if (!delete_chip) {
        return hal_sfc_regs_wait_ready(id, 0x0);
    }

    /* chip erase timeout extend */
    for (uint32_t i = 0; i < CHIP_ERASE_WAIT_ROUND; i++) {
        ret = hal_sfc_regs_wait_ready(id, 0x0);
        if (ret == ERRCODE_SUCC) {
            return ret;
        }
#ifndef CONFIG_WDT_UNIFIED_CONTROL
        (void)uapi_watchdog_kick(); /* ignore return_val when wdt is diasble */
#endif
    }

    return ERRCODE_SFC_FLASH_TIMEOUT_WAIT_READY;
#endif
}

STATIC errcode_t hal_sfc_regs_read_flash_info(uint8_t id, uint32_t opt_type, spi_opreation_t opt, uint8_t *buffer,
    uint32_t length)
{
    unused(opt_type);
    errcode_t ret;
    uint32_t offset = 0;
    hal_spi_opreation_t hal_opreation = { .opt = opt, .data_size = length, .dummy_byte = opt.size};
    ret = hal_sfc_regs_wait_ready(id, 0x0);
    if (unlikely(ret != ERRCODE_SUCC)) {
        return ret;
    };
    hal_sfc_regs_set_opt(id, hal_opreation);
    hal_sfc_regs_set_opt_attr(id, READ_MODE, ENABLE, DISABLE);
    hal_sfc_regs_wait_config(id);
    while (offset < length) {
        uint32_t temp_data = hal_sfc_regs_get_databuf(id, offset / sizeof(uint32_t));
        uint32_t copy_len = (length - offset > sizeof(uint32_t)) ? sizeof(uint32_t) : (length - offset);

        if (memcpy_s(buffer + offset, copy_len, &temp_data, copy_len) != EOK) {
            return ERRCODE_FAIL;
        }

        offset += copy_len;
    }

    return ERRCODE_SUCC;
}

STATIC errcode_t hal_sfc_regs_set_flash_attr(uint8_t id, uint32_t opt_type, spi_opreation_t opt, uint8_t *buffer,
    uint32_t length)
{
    unused(opt_type);
    hal_spi_opreation_t hal_opreation = { .opt = opt, .data_size = length, .dummy_byte = opt.size};

    uint8_t data[BYTE_IN_WORD] = {0};

    if (length > sizeof(uint32_t)) {
        return ERRCODE_SFC_INVALID_PARAM;
    }

    uint32_t data_en = (length > 0) ? ENABLE : DISABLE;
    (void)memcpy_s(data, BYTE_IN_WORD, buffer, length);
    hal_sfc_regs_set_databuf(id, 0, (*(uint32_t *)data));
    hal_sfc_regs_set_opt(id, hal_opreation);
    hal_sfc_regs_set_opt_attr(id, WRITE_MODE, data_en, DISABLE);
    hal_sfc_regs_wait_config(id);
    return ERRCODE_SUCC;
}

const hal_sfc_reg_flash_opreation_t g_flash_opreations[FLASH_OPREATION_MAX_NUM] = {
    hal_sfc_regs_read_flash_info,
    hal_sfc_regs_set_flash_attr
};

errcode_t hal_sfc_reg_flash_opreations(uint8_t id, uint32_t opt_type, spi_opreation_t opt, uint8_t *buffer,
    uint32_t length)
{
    return g_flash_opreations[opt_type](id, opt_type, opt, buffer, length);
}

#if defined(CONFIG_SFC_SUPPORT_LPM)
errcode_t hal_sfc_suspend(uint8_t id)
{
#if defined(CONFIG_SFC_SUPPORT_DEEP_POWERDOWN)
    hal_sfc_enter_deep_powerdown(id);
#endif
    g_sfc_suspend_regs[id][0x0] = hal_sfc_regs_get_sfc_bus_config1(id);
    g_sfc_suspend_regs[id][0x1] = hal_sfc_regs_get_sfc_bus_config2(id);
    g_sfc_suspend_regs[id][0x2] = hal_sfc_regs_get_sfc_bus_flash_size(id);
#ifdef CONFIG_SFC_SUPPORT_COMPLECT
    g_sfc_suspend_regs[id][0x3] = hal_sfc_regs_get_bus_base_addr_cs1(id);
#else
    g_sfc_suspend_regs[id][0x3] = hal_sfc_regs_get_bus_base_addr_cs0(id);
    g_sfc_suspend_regs[id][0x4] = hal_sfc_regs_get_bus_base_addr_cs1(id);
#endif
    return ERRCODE_SUCC;
}

errcode_t hal_sfc_resume(uint8_t id, flash_cmd_execute_t *quad_mode)
{
    unused(quad_mode);
    hal_sfc_regs_set_sfc_bus_config1(id, g_sfc_suspend_regs[id][0x0]);
    hal_sfc_regs_set_sfc_bus_config2(id, g_sfc_suspend_regs[id][0x1]);
    hal_sfc_regs_set_sfc_bus_flash_size(id, g_sfc_suspend_regs[id][0x2]);
#ifdef CONFIG_SFC_SUPPORT_COMPLECT
    hal_sfc_regs_enable_complect();
    hal_sfc_regs_set_bus_base_addr_cs1(id, g_sfc_suspend_regs[id][0x3]);
#else
    hal_sfc_regs_set_bus_base_addr_cs0(id, g_sfc_suspend_regs[id][0x3]);
    hal_sfc_regs_set_bus_base_addr_cs1(id, g_sfc_suspend_regs[id][0x4]);
#endif
    /*
     * The flash requires that tshsl parameter should be greater than 50ns. In max freq-130M,
     * tshsl: (5 + 2)*1000/130=53ns >50ns
     */
    hal_sfc_regs_set_timing(id, 0x1105);
#if defined(CONFIG_SFC_SUPPORT_DEEP_POWERDOWN)
    hal_sfc_release_from_deep_powerdown(id);
#endif
    return ERRCODE_SUCC;
}
#endif

#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
void hal_sfc_enable_continue_read(uint8_t id, spi_opreation_t read_opreation, uint8_t m_byte)
{
    hal_spi_opreation_t opreation = {
        .opt = read_opreation,
#ifdef CONFIG_SFC_SUPPORT_COMPLECT
        .data_size = FLASH_DOUBLE_BYTE,
#else
        .data_size = 1,
#endif
        /* 1: reserve for m_byte */
        .dummy_byte = read_opreation.size - 1
    };
    hal_sfc_regs_set_opt(id, opreation);
    hal_sfc_regs_set_mbyte(id, m_byte);

#ifdef CONFIG_SFC_SUPPORT_COMPLECT
    hal_sfc_regs_set_cmd_addr(id, m_byte << 1);
#else
    hal_sfc_regs_set_cmd_addr(id, m_byte);
#endif
    hal_sfc_regs_set_addr_mode(id, 1); /* 0: 3_byte; 1: 4_byte */
    hal_sfc_regs_set_opt_attr(id, READ_MODE, ENABLE, ENABLE);
    hal_sfc_regs_wait_config(id);

    (void)hal_sfc_regs_get_databuf(id, 0); /* ignore data content */
    read_opreation.size = read_opreation.size - 1;
    hal_sfc_regs_set_bus_read(id, read_opreation);
    hal_sfc_regs_ctrl_continue_read(id, true);
}

void hal_sfc_disable_continue_read(uint8_t id, spi_opreation_t read_opreation)
{
    hal_spi_opreation_t opreation = {
        .opt = read_opreation,
#ifdef CONFIG_SFC_SUPPORT_COMPLECT
        .data_size = FLASH_DOUBLE_BYTE,
#else
        .data_size = 1,
#endif
        /* 1: reserve for m_byte */
        .dummy_byte = read_opreation.size - 1
    };
    hal_sfc_regs_set_opt(id, opreation);
    hal_sfc_regs_set_mbyte(id, 0xFF); /* 0: invalid mbyte */
    hal_sfc_regs_set_cmd_addr(id, 0); /* Read any address content */
    hal_sfc_regs_set_opt_attr(id, READ_MODE, ENABLE, ENABLE);
    hal_sfc_regs_wait_config(id);

    (void)hal_sfc_regs_get_databuf(id, 0); /* ignore data content */
    hal_sfc_regs_ctrl_continue_read(id, false);
    hal_sfc_regs_set_bus_read(id, read_opreation);
    hal_sfc_regs_set_addr_mode(id, 0); /* 0: 3_byte; 1: 4_byte */
}
#endif

#if defined(CONFIG_SFC_SUPPORT_DEEP_POWERDOWN)
errcode_t hal_sfc_enter_deep_powerdown(uint8_t id)
{
    uint8_t cmd[1] = {FLASH_CMD_DP};
    return hal_sfc_execute_type_cmd(id, 1, cmd);
}

errcode_t hal_sfc_release_from_deep_powerdown(uint8_t id)
{
    uint8_t cmd[1] = {FLASH_CMD_RDP};
    return hal_sfc_execute_type_cmd(id, 1, cmd);
}
#endif
