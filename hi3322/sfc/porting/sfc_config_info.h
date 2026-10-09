/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides sfc port template
 *
 * History:
 * 2022-11-30， Create file.
 */
#ifndef FLASH_CONFIG_INFO_H
#define FLASH_CONFIG_INFO_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define FLASH_SIZE_512KB  0x80000
#define FLASH_SIZE_1MB    0x100000
#define FLASH_SIZE_4MB    0x400000
#define FLASH_SIZE_8MB    0x800000
#define FLASH_SIZE_16MB   0x1000000
#define FLASH_SIZE_32MB   0x2000000

#define EFLASH_CMD_LEN_MAX             4
#define SPI_CMD_SUPPORT                0x1
#define SFC_INVALID_CMD                0x0
#define SFC_RDID_CMD                   0x9F
#define SFC_RDID_LEN                   3

#define SPI_CMD_WDSR             0x01
#define SPI_CMD_WDSR_2           0x31
#define SPI_CMD_WDSR_3           0x11
#define SPI_CMD_RDSR             0x05
#define SPI_CMD_RDID             0x9F
#define SPI_CMD_WREN             0x06
#define SPI_CMD_VOLATILE_WREN    0x50
#define SPI_CMD_RDSR_2           0x35
#define SPI_CMD_RDSR_3           0x15
#define SPI_CMD_WDSR             0x01
#define SPI_CMD_WDSR_2           0x31
#define SPI_CMD_WDSR_3           0x11
#define FLASH_CMD_DP             0xB9
#define FLASH_CMD_RDP            0xAB
#define FLASH_ID_READ_LEN        0x6
#define FLASH_ID_SHIFT_8         8
#define FLASH_ID_SHIFT_16        16
#define FLASH_DOUBLE_BYTE        2
#define FLASH_QUAD_ENABLE_BIT    0x202  /* bit 1 and bit 9 */
#define FLASH_WIP_READY_BIT      0x101  /* bit 0 and bit 8 */
#define FLASH_WAIT_ERASE_MAX_CNT 0xFFFFFFFF
#define FLASH_WAIT_WRITE_EN_MAX_CNT 0xFFFF
#define FLASH_W25Q32            0x1660EF
#define FLASH_W25Q64            0x1760EF
#define FLASH_W25Q128           0x1860EF
#define FLASH_W25Q128_IM        0x1880EF
#define FLASH_W25Q80            0x1460EF
#define FLASH_W25Q40            0x1360EF
#define FLASH_P25Q80            0x146085
#define FLASH_GD25WD40          0x1364C8
#define FLASH_G25LE80           0x1460C8
#define FLASH_EN25S80           0x14381C
#define FLASH_GD25LQ64          0x1760c8
#define FLASH_GD25LE128E        0x1860C8
#define FLASH_FM25M4AA          0x1842F8
#define FLASH_EN25SX128A        0x18781C
#define FLASH_XT25Q128          0x18600B
#define FLASH_DS25M4AE          0x1841E5
#define FLASH_XM25QU256C        0x194120
#define FLASH_UNKOWN            0xFFFFFF

typedef enum {
    STANDARD_SPI,
    DUAL_INPUT_DUAL_OUTPUT_SPI,
    DUAL_IO_SPI,
    QUAL_INPUT_QUAL_OUTPUT_SPI = 5,
    QUAL_IO_SPI = 6,
} flash_spi_interface_type_t;

/**
 * @if Eng
 * @brief  SPI instruction execution mode.
 * @else
 * @brief  表驱动执行Flash指令的指令格式
 * @endif
*/
typedef enum {
    FLASH_CMD_TYPE_CMD,                      /*!<@if Eng Command for setting the flash attribute
                                                 @else   设置flash属性类型的指令         @endif
                                              */
    FLASH_CMD_TYPE_PROCESSING,               /*!<@if Eng Read the flash information and compares a certain bit.
                                                 @else   读取Flash信息并比对某一位的值         @endif
                                              */
    FLASH_CMD_TYPE_END,                      /*!<@if Eng Command end flag
                                                 @else   指令结束标志         @endif
                                              */
    FLASH_CMD_BUFF = 0xFF
} flash_cmd_type_t;

/**
 * @if Eng
 * @brief  Parameters related to SPI read, write and erase operation.
 * @else
 * @brief  SPI读写擦操作相关参数
 * @endif
*/
typedef struct spi_opreation {
    uint32_t cmd_support : 3;                /*!<@if Eng SPI command support
                                                 @else   是否支持该索引对应的指令 @endif
                                              */
    uint32_t cmd : 8;                        /*!<@if Eng SPI command
                                                 @else   SPI指令码             @endif
                                              */
    uint32_t iftype : 3;                     /*!<@if Eng SPI interface type
                                                         value:
                                                            000：Standard SPI
                                                            001：Dual-Input/Dual-Output SPI(based on read/write)
                                                            010：Dual-I/O SPI
                                                            101：Quad-Input/Qual-Output SPI(based on read/write)
                                                            110：Quad-I/O SPI
                                                            other: reserved
                                                 @else   SPI 接口类型
                                                         合法值:
                                                            000：标准单线SPI
                                                            001：双线In/双线Out SPI（根据指令的读写模式调整）
                                                            010：双线I/O SPI
                                                            101：四线In/四线Out SPI（根据指令的读写模式调整）
                                                            110：四线I/O SPI
                                                            其他:保留 @endif
                                              */
    uint32_t size : 18;                      /*!<@if Eng   erase size for erase and dummy byte for read.
                                                 @else     擦除指令的大小和读指令的dummy字节数     @endif
                                              */
} spi_opreation_t;

/**
 * @if Eng
 * @brief  Command format for enabling the Quad SPI flash
 * @else
 * @brief  开启Flash的Quad SPI的指令格式
 * @endif
*/
typedef struct flash_cmd_execute_t {
    flash_cmd_type_t cmd_type;               /*!<@if Eng For details, see @ref flash_cmd_type_t
                                                 @else   参考 @ref flash_cmd_type_t @endif
                                              */
    uint8_t cmd_len;                         /*!<@if Eng CMD mode : Length of the SPI command including 1byte data.
                                                         PROCESSING mode : The value is fixed to 3.
                                                 @else   CMD模式包含一字节数据在内的SPI指令长度。
                                                         PROCESSING模式固定为3 @endif
                                              */
    uint8_t cmd[EFLASH_CMD_LEN_MAX];         /*!<@if Eng SPI command. The format is as follows:
                                                         CMD: cmd[0] command code.
                                                              cmd[1] One-byte data.
                                                         PROCESS: cmd[0] command code.
                                                                  cmd[1] Expected Compare Bit.
                                                                  cmd[2] Expected value of this bit.
                                                 @else   SPI指令，格式如下
                                                         CMD: cmd[0] 指令码
                                                              cmd[1] 一字节数据
                                                         PROCESS: cmd[0] 指令码
                                                                  cmd[1] 预计比较的位
                                                                  cmd[2] 该位预计的值 @endif
                                              */
} flash_cmd_execute_t;

#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
typedef struct {
    bool is_support;
    uint8_t m_byte; /* flag to enter continuous read mode */
} continue_read_info_t;
#endif

/**
 * @if Eng
 * @brief  Flash basic information struct
 * @else
 * @brief  Flash基本信息结构
 * @endif
*/
typedef struct flash_spi_info {
    uint32_t chip_id;                        /*!<@if Eng Flash manufacture id.
                                                 @else   Flash 制造id @endif
                                              */
    uint32_t chip_size;                      /*!<@if Eng Actual size of flash
                                                 @else   Flash实际大小 @endif
                                              */
    uint32_t erase_cmd_num;                  /*!<@if Eng Number of erase commands
                                                 @else   擦除指令的个数@endif
                                              */
    spi_opreation_t *read_cmds;              /*!<@if Eng Read command form. index:@ref sfc_read_if_t.
                                                 @else   读指令表单 索引 @ref sfc_read_if_t
                                                 @endif
                                              */
    spi_opreation_t *write_cmds;             /*!<@if Eng Write command form. index:@ref sfc_write_if_t.
                                                 @else   写指令表单 索引 @ref sfc_write_if_t
                                                 @endif
                                              */
    spi_opreation_t *erase_cmds;             /*!<@if Eng Erase command form. Indexes are sorted by erase size in desc.
                                                 @else   写指令表单 索引按照擦除大小降序排列
                                                 @endif
                                              */
    flash_cmd_execute_t *quad_mode;          /*!<@if Eng Enable Quad SPI Mode command Form. @ref flash_cmd_execute_t
                                                 @else   开启四线模式指令表单 @ref flash_cmd_execute_t @endif
                                              */
#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
    continue_read_info_t continue_info;
#endif
} flash_spi_info_t;

typedef struct {
    uint32_t flash_id;
    uint8_t cmd_len;
    uint8_t read_cmd[0x2];
    uint8_t write_cmd;
    uint8_t write_mask[0x2];
    uint8_t read_mask[0x2];
} flash_cmd_qe_enable_t;

flash_spi_info_t *sfc_port_get_flash_spi_infos(void);

uint32_t sfc_port_get_flash_num(void);

flash_spi_info_t *sfc_port_get_unknown_flash_info(void);

flash_cmd_qe_enable_t *sfc_port_get_flash_qe_cmd(uint32_t flash_id);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif