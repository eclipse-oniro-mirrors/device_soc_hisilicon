/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Provides flash config info source file.
 *
 * History:
 * 2022-11-30， Create file.
 */

#include "sfc_config_info.h"
#include <securec.h>

#define _4K             0x1000
#define _8K             0x2000
#define _32K            0x8000
#define _64K            0x10000
#define _128K           0x20000
#define CHIP_SIZE       0x3ffff

#define FLASH_ERASE_CMD_NUM_4 4
#define FLASH_ERASE_CMD_NUM_3 3

#define DISABLE         0x0
#define ENABLE          0x1
#define FLASH_MANUFACTURER_MAX 25
#define SPI_CMD_UNSUPPORT {0x0, 0x0, 0x0, 0x1}
#define WR_ENABLE       0x6

#define SFC_SUPPORT_IF_TYPE            0x6

static const flash_cmd_execute_t g_default_quad_enable[] = {
    { FLASH_CMD_TYPE_END,         0, { 0x0 }}
};

static const spi_opreation_t g_default_read_cmds[] = {
    {SPI_CMD_SUPPORT, 0x03, 0x0, 0x0},
};

static const spi_opreation_t g_default_write_cmds[] = {
    {SPI_CMD_SUPPORT, SFC_INVALID_CMD, 0x0, 0},
    {SPI_CMD_SUPPORT, 0x02, 0x0, 0},
};

static const spi_opreation_t g_default_erase_cmds[] = {
    {SPI_CMD_SUPPORT, 0xC7, 0x0, CHIP_SIZE},
    {SPI_CMD_SUPPORT, 0xD8, 0x0, _64K},
    {SPI_CMD_SUPPORT, 0x20, 0x0, _4K},
};

static const flash_cmd_execute_t g_flash_winbond_bus_enable[] = {
    { FLASH_CMD_TYPE_CMD,         1, { WR_ENABLE }},
    { FLASH_CMD_TYPE_CMD,         2, { 0x31, 0x02 } },
    { FLASH_CMD_TYPE_PROCESSING,  3, { 0x05, 0, DISABLE } },
    { FLASH_CMD_TYPE_PROCESSING,  3, { 0x35, 1, ENABLE } },
    { FLASH_CMD_TYPE_END,         0, { 0x0 }}
};

static const flash_cmd_execute_t g_flash_gd_bus_enable[] = {
    { FLASH_CMD_TYPE_CMD,         1, { WR_ENABLE }},
    { FLASH_CMD_TYPE_CMD,         3, { 0x01, 0x0, 0x2} },
    { FLASH_CMD_TYPE_PROCESSING,  3, { 0x05, 0, DISABLE } },
    { FLASH_CMD_TYPE_PROCESSING,  3, { 0x35, 1, ENABLE } },
    { FLASH_CMD_TYPE_END,         0, { 0x0 }}
};

static const flash_cmd_execute_t g_flash_common_bus_enable[] = {
    { FLASH_CMD_TYPE_CMD,         1, { WR_ENABLE }},
    { FLASH_CMD_TYPE_PROCESSING,  3, { 0x05, 0, DISABLE } },
    { FLASH_CMD_TYPE_END,         0, { 0x0 }}
};

static const spi_opreation_t g_flash_common_read_cmds[] = {
    {SPI_CMD_SUPPORT, 0x03, STANDARD_SPI, 0x0},
    {SPI_CMD_SUPPORT, 0x0B, STANDARD_SPI, 0x1},
    {SPI_CMD_SUPPORT, 0x3B, DUAL_INPUT_DUAL_OUTPUT_SPI, 0x1},
    {SPI_CMD_SUPPORT, 0xBB, DUAL_IO_SPI, 0x1},
    {SPI_CMD_SUPPORT, 0x6B, QUAL_INPUT_QUAL_OUTPUT_SPI, 0x1},
    {SPI_CMD_SUPPORT, 0xEB, QUAL_IO_SPI, 0x3}
};

static const spi_opreation_t g_flash_gd25wd40_read_cmds[] = {
    {SPI_CMD_SUPPORT, 0x03, STANDARD_SPI, 0x0},
    {SPI_CMD_SUPPORT, 0x0B, STANDARD_SPI, 0x1},
    {SPI_CMD_SUPPORT, 0x3B, DUAL_INPUT_DUAL_OUTPUT_SPI, 0x1},
    SPI_CMD_UNSUPPORT,
    SPI_CMD_UNSUPPORT,
    SPI_CMD_UNSUPPORT,
};

static const spi_opreation_t g_flash_esmt_read_cmds[] = {
    {SPI_CMD_SUPPORT, 0x03, STANDARD_SPI, 0x0},
    {SPI_CMD_SUPPORT, 0x0B, STANDARD_SPI, 0x1},
    {SPI_CMD_SUPPORT, 0x3B, DUAL_INPUT_DUAL_OUTPUT_SPI, 0x1},
    {SPI_CMD_SUPPORT, 0xBB, DUAL_IO_SPI, 0x1},
    SPI_CMD_UNSUPPORT,
    SPI_CMD_UNSUPPORT,
};

static const spi_opreation_t g_flash_common_write_cmds[] = {
    {SPI_CMD_SUPPORT, SFC_INVALID_CMD, 0x0, 0},
    {SPI_CMD_SUPPORT, 0x02, STANDARD_SPI, 0},
    SPI_CMD_UNSUPPORT,
    SPI_CMD_UNSUPPORT,
    {SPI_CMD_SUPPORT, 0x32, QUAL_INPUT_QUAL_OUTPUT_SPI, 0},
    SPI_CMD_UNSUPPORT,
};

static const spi_opreation_t g_flash_common_erase_cmds[] = {
    {SPI_CMD_SUPPORT, 0xC7, 0x0, CHIP_SIZE},
    {SPI_CMD_SUPPORT, 0xD8, 0x0, _64K},
    {SPI_CMD_SUPPORT, 0x52, 0x0, _32K},
    {SPI_CMD_SUPPORT, 0x20, 0x0, _4K}
};

static const flash_spi_info_t g_flash_spi_info_list[] = {
    {
        FLASH_XM25QU256C,
        FLASH_SIZE_32MB,
        FLASH_ERASE_CMD_NUM_4,
        (spi_opreation_t *)g_flash_common_read_cmds,
        (spi_opreation_t *)g_flash_common_write_cmds,
        (spi_opreation_t *)g_flash_common_erase_cmds,
        (flash_cmd_execute_t *)g_flash_common_bus_enable,
#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
        {false, 0},
#endif
    },
    {
        FLASH_W25Q32,
        FLASH_SIZE_4MB,
        FLASH_ERASE_CMD_NUM_4,
        (spi_opreation_t *)g_flash_common_read_cmds,
        (spi_opreation_t *)g_flash_common_write_cmds,
        (spi_opreation_t *)g_flash_common_erase_cmds,
        (flash_cmd_execute_t *)g_flash_winbond_bus_enable,
#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
        {false, 0},
#endif
    },
    {
        FLASH_W25Q64,
        FLASH_SIZE_8MB,
        FLASH_ERASE_CMD_NUM_4,
        (spi_opreation_t *)g_flash_common_read_cmds,
        (spi_opreation_t *)g_flash_common_write_cmds,
        (spi_opreation_t *)g_flash_common_erase_cmds,
        (flash_cmd_execute_t *)g_flash_winbond_bus_enable,
#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
        {false, 0},
#endif
    },
    {
        FLASH_W25Q128,
        FLASH_SIZE_16MB,
        FLASH_ERASE_CMD_NUM_4,
        (spi_opreation_t *)g_flash_common_read_cmds,
        (spi_opreation_t *)g_flash_common_write_cmds,
        (spi_opreation_t *)g_flash_common_erase_cmds,
        (flash_cmd_execute_t *)g_flash_winbond_bus_enable,
#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
        {true, 0x20},
#endif
    },
    {
        FLASH_W25Q128_IM,
        FLASH_SIZE_16MB,
        FLASH_ERASE_CMD_NUM_4,
        (spi_opreation_t *)g_flash_common_read_cmds,
        (spi_opreation_t *)g_flash_common_write_cmds,
        (spi_opreation_t *)g_flash_common_erase_cmds,
        (flash_cmd_execute_t *)g_flash_winbond_bus_enable,
#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
        {true, 0x20},
#endif
    },
    {
        FLASH_W25Q40,
        FLASH_SIZE_512KB,
        FLASH_ERASE_CMD_NUM_4,
        (spi_opreation_t *)g_flash_common_read_cmds,
        (spi_opreation_t *)g_flash_common_write_cmds,
        (spi_opreation_t *)g_flash_common_erase_cmds,
        (flash_cmd_execute_t *)g_flash_winbond_bus_enable,
#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
        {false, 0},
#endif
    },
    {
        FLASH_W25Q80,
        FLASH_SIZE_1MB,
        FLASH_ERASE_CMD_NUM_4,
        (spi_opreation_t *)g_flash_common_read_cmds,
        (spi_opreation_t *)g_flash_common_write_cmds,
        (spi_opreation_t *)g_flash_common_erase_cmds,
        (flash_cmd_execute_t *)g_flash_winbond_bus_enable,
#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
        {false, 0},
#endif
    },
    {
        FLASH_GD25WD40,
        FLASH_SIZE_512KB,
        FLASH_ERASE_CMD_NUM_4,
        (spi_opreation_t *)g_flash_gd25wd40_read_cmds,
        (spi_opreation_t *)g_flash_common_write_cmds,
        (spi_opreation_t *)g_flash_common_erase_cmds,
        (flash_cmd_execute_t *)g_flash_common_bus_enable,
#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
        {false, 0},
#endif
    },
    {
        FLASH_G25LE80,
        FLASH_SIZE_1MB,
        FLASH_ERASE_CMD_NUM_4,
        (spi_opreation_t *)g_flash_common_read_cmds,
        (spi_opreation_t *)g_flash_common_write_cmds,
        (spi_opreation_t *)g_flash_common_erase_cmds,
        (flash_cmd_execute_t *)g_flash_common_bus_enable,
#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
        {false, 0},
#endif
    },
    {
        FLASH_EN25S80,
        FLASH_SIZE_1MB,
        FLASH_ERASE_CMD_NUM_4,
        (spi_opreation_t *)g_flash_esmt_read_cmds,
        (spi_opreation_t *)g_flash_common_write_cmds,
        (spi_opreation_t *)g_flash_common_erase_cmds,
        (flash_cmd_execute_t *)g_flash_common_bus_enable,
#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
        {false, 0},
#endif
    },
    {
        FLASH_P25Q80,
        FLASH_SIZE_1MB,
        FLASH_ERASE_CMD_NUM_4,
        (spi_opreation_t *)g_flash_common_read_cmds,
        (spi_opreation_t *)g_flash_common_write_cmds,
        (spi_opreation_t *)g_flash_common_erase_cmds,
        (flash_cmd_execute_t *)g_flash_common_bus_enable,
#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
        {false, 0},
#endif
    },
    {
        FLASH_GD25LQ64,
        FLASH_SIZE_8MB,
        FLASH_ERASE_CMD_NUM_4,
        (spi_opreation_t *)g_flash_common_read_cmds,
        (spi_opreation_t *)g_flash_common_write_cmds,
        (spi_opreation_t *)g_flash_common_erase_cmds,
        (flash_cmd_execute_t *)g_flash_gd_bus_enable,
#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
        {true, 0x20},
#endif
    },
    {
        FLASH_GD25LE128E,
        FLASH_SIZE_16MB,
        FLASH_ERASE_CMD_NUM_4,
        (spi_opreation_t *)g_flash_common_read_cmds,
        (spi_opreation_t *)g_flash_common_write_cmds,
        (spi_opreation_t *)g_flash_common_erase_cmds,
        (flash_cmd_execute_t *)g_flash_gd_bus_enable,
#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
        {true, 0x20},
#endif
    },
    {
        FLASH_FM25M4AA,
        FLASH_SIZE_16MB,
        FLASH_ERASE_CMD_NUM_4,
        (spi_opreation_t *)g_flash_common_read_cmds,
        (spi_opreation_t *)g_flash_common_write_cmds,
        (spi_opreation_t *)g_flash_common_erase_cmds,
        (flash_cmd_execute_t *)g_flash_gd_bus_enable,
#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
        {true, 0xA0},
#endif
    },
    {
        FLASH_EN25SX128A,
        FLASH_SIZE_16MB,
        FLASH_ERASE_CMD_NUM_4,
        (spi_opreation_t *)g_flash_common_read_cmds,
        (spi_opreation_t *)g_flash_common_write_cmds,
        (spi_opreation_t *)g_flash_common_erase_cmds,
        (flash_cmd_execute_t *)g_flash_gd_bus_enable,
#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
        {true, 0xA5},
#endif
    },
    {
        FLASH_XT25Q128,
        FLASH_SIZE_16MB,
        FLASH_ERASE_CMD_NUM_4,
        (spi_opreation_t *)g_flash_common_read_cmds,
        (spi_opreation_t *)g_flash_common_write_cmds,
        (spi_opreation_t *)g_flash_common_erase_cmds,
        (flash_cmd_execute_t *)g_flash_common_bus_enable,
#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
        {false, 0},
#endif
    },
    {
        FLASH_DS25M4AE,
        FLASH_SIZE_16MB,
        FLASH_ERASE_CMD_NUM_4,
        (spi_opreation_t *)g_flash_common_read_cmds,
        (spi_opreation_t *)g_flash_common_write_cmds,
        (spi_opreation_t *)g_flash_common_erase_cmds,
        (flash_cmd_execute_t *)g_flash_common_bus_enable,
#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
        {false, 0},
#endif
    },
};

const flash_spi_info_t g_flash_spi_unknown_info = {
    FLASH_UNKOWN,
    FLASH_SIZE_16MB,
    FLASH_ERASE_CMD_NUM_3,
    (spi_opreation_t *)g_default_read_cmds,
    (spi_opreation_t *)g_default_write_cmds,
    (spi_opreation_t *)g_default_erase_cmds,
    (flash_cmd_execute_t *)g_default_quad_enable,
#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
    {false, 0},
#endif
};

static flash_cmd_qe_enable_t g_flash_cmd_qe_enable[] = {
    {FLASH_W25Q128, 1, { SPI_CMD_RDSR_2, 0}, SPI_CMD_WDSR_2, {0x2, 0}, {0x2, 0}},
    {FLASH_W25Q128_IM, 1, { SPI_CMD_RDSR_2, 0}, SPI_CMD_WDSR_2, {0x2, 0}, {0x2, 0}},
    {FLASH_GD25LQ64, 2, { SPI_CMD_RDSR, SPI_CMD_RDSR_2}, SPI_CMD_WDSR, {0x0, 0x2}, {0x0, 0x2}},
    {FLASH_GD25LE128E, 2, { SPI_CMD_RDSR, SPI_CMD_RDSR_2}, SPI_CMD_WDSR, {0x0, 0x2}, {0x0, 0x2}},
    {FLASH_FM25M4AA,   2, { SPI_CMD_RDSR, SPI_CMD_RDSR_2}, SPI_CMD_WDSR, {0x0, 0x2}, {0x0, 0x2}},
    {FLASH_XM25QU256C,   2, { SPI_CMD_RDSR, SPI_CMD_RDSR_2}, SPI_CMD_WDSR, {0x0, 0x2}, {0x0, 0x2}},
    {FLASH_EN25SX128A, 1, { SPI_CMD_RDSR_2, 0}, SPI_CMD_WDSR_2, {0x2, 0}, {0x2, 0}},
    {FLASH_XT25Q128, 2, { SPI_CMD_RDSR, SPI_CMD_RDSR_2}, SPI_CMD_WDSR, {0x0, 0x2}, {0x0, 0x2}},
    {FLASH_DS25M4AE, 2, { SPI_CMD_RDSR, SPI_CMD_RDSR_2}, SPI_CMD_WDSR, {0x0, 0x2}, {0x0, 0x2}},
};


flash_spi_info_t *sfc_port_get_flash_spi_infos(void)
{
    return (flash_spi_info_t *)g_flash_spi_info_list;
}

uint32_t sfc_port_get_flash_num(void)
{
    return sizeof(g_flash_spi_info_list) / sizeof(flash_spi_info_t);
}

flash_spi_info_t *sfc_port_get_unknown_flash_info(void)
{
    return (flash_spi_info_t *)&g_flash_spi_unknown_info;
}

flash_cmd_qe_enable_t *sfc_port_get_flash_qe_cmd(uint32_t flash_id)
{
    uint32_t idx;
    for (idx = 0; idx < sizeof(g_flash_cmd_qe_enable) / sizeof(flash_cmd_qe_enable_t); idx++) {
        if (flash_id == g_flash_cmd_qe_enable[idx].flash_id) {
            return &g_flash_cmd_qe_enable[idx];
        }
    }
    return NULL;
}