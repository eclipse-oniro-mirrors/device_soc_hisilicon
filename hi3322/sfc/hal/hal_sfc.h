/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Provides hal sfc \n
 *
 * History: \n
 * 2024-10-01, Create file. \n
 */
#ifndef HAL_SFC_H
#define HAL_SFC_H

#include <stdint.h>
#include <stdbool.h>
#include <common_def.h>
#include <errcode.h>
#include <hal_sfc_v150_regs_op.h>
#include <sfc_porting.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_sfc_api SFC
 * @ingroup  drivers_hal_sfc
 * @{
 */

/**
 * @if Eng
 * @brief  Get the Flash ID
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [out]  flash_id  flash ID is stored in this address.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  获取Flash ID操作
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [out]  flash_id  读取到的flash id。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_sfc_get_flash_id(uint8_t id, uint32_t *flash_id);

#if defined(CONFIG_SFC_SUPPORT_COMPLECT)
/**
 * @if Eng
 * @brief  SFC complect initialization.
 * @param  [in]  sfc_id SFC ID.
 * @else
 * @brief  SFC 交织初始化操作。
 * @param  [in]  sfc_id SFC ID.
 * @endif
 */
void hal_sfc_complect_init(uint8_t id);
#endif

/**
 * @if Eng
 * @brief  SFC initialization.
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  spi_ctrl  For details, see @ref flash_spi_ctrl_t.
 * @param  [in]  mapping_address   Flash mapping address.
 * @param  [in]  flash_size   For details, see @ref bus_flash_size_t.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  SFC 初始化操作。
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  spi_ctrl  参考 @ref flash_spi_ctrl_t 。
 * @param  [in]  mapping_address   Flash映射地址。
 * @param  [in]  flash_size   参考 @ref bus_flash_size 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_sfc_init(uint8_t id, flash_spi_ctrl_t *spi_ctrl, uint32_t mapping, uint32_t flash_size);

/**
 * @if Eng
 * @brief  SFC deinitialize.
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @else
 * @brief  SFC 去初始化操作。
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @endif
 */
void hal_sfc_deinit(uint8_t id);

/**
 * @if Eng
 * @brief  SFC read operation in SPI mode.
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  flash_addr  Start address of the flash memory to be read.
 * @param  [out] read_buffer Pointer to data buffer.
 * @param  [in]  read_size   Amount of data to be read.
 * @param  [in]  read_opreation Read command information.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  SFC 内嵌SPI模式读操作。
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  flash_addr  读数据首地址。
 * @param  [out] read_buffer 读操作数据缓冲区。
 * @param  [in]  read_size   读数据总字节数，在driver层已做参数检查。
 * @param  [in]  read_opreation 读指令信息。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_sfc_reg_read(uint8_t id, uint32_t flash_addr, uint8_t *read_buffer, uint32_t read_size,
                           spi_opreation_t read_opreation);

/**
 * @if Eng
 * @brief  SFC write operation in SPI mode.
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  flash_addr  Start address of the flash memory to be write.
 * @param  [in]  write_data  Pointer to data buffer.
 * @param  [in]  write_size  Amount of data to be write.
 * @param  [in]  write_opreation Write command information.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  SFC 内嵌SPI模式写操作。
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  flash_addr   写数据首地址。
 * @param  [in]  write_data   预计写入的数据。
 * @param  [in]  write_size   写数据总字节数，在driver层已做参数检查。
 * @param  [in]  write_opreation 写操作信息。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_sfc_reg_write(uint8_t id, uint32_t flash_addr, uint8_t *write_data, uint32_t write_size,
                            spi_opreation_t write_opreation);

/**
 * @if Eng
 * @brief  SFC erase operation in SPI mode.
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  flash_addr  Start address of the flash memory to be erase.
 * @param  [in]  erase_opreation  Erase command information.
 * @param  [in]  delete_chip  Indicates whether to erase the entire chip.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  SFC 内嵌SPI模式擦除操作。
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  flash_addr        预擦除的首地址。
 * @param  [in]  erase_opreation   擦数据信息。
 * @param  [in]  delete_chip       是否为整片擦除。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_sfc_reg_erase(uint8_t id, uint32_t flash_addr, spi_opreation_t erase_opreation, bool delete_chip);

/**
 * @if Eng
 * @brief  SFC other flash operations in SPI mode.
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  opt  Flash operation type&dummy&spi_mode.
 * @param  [in]  cmd  Flash command code.
 * @param  [in,out]  buffer Pointer to data buffer
 * @param  [in]  length Length of data buffer. The value is less than 4. Processed at the driver layer
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  SFC 内嵌SPI模式Flash其他操作。
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  opt Flash操作类型。
 * @param  [in]  cmd Flash操作指令码,dummy,spi_mode。
 * @param  [in,out]  buffer 数据缓冲区。
 * @param  [in]  length 缓冲区长度，driver层限制其值小于4。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
typedef errcode_t (*hal_sfc_reg_flash_opreation_t)(uint8_t id, uint32_t opt_type, spi_opreation_t opt, uint8_t *buffer,
    uint32_t length);

/**
 * @if Eng
 * @brief  SFC other flash operations in SPI mode.
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  opt_type  Flash operation type.
 * @param  [in]  cmd  Flash command code.
 * @param  [in,out]  buffer Pointer to data buffer
 * @param  [in]  length Length of data buffer. The value is less than 4. Processed at the driver layer
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  SFC 内嵌SPI模式Flash其他操作。
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  opt_type Flash操作类型。
 * @param  [in]  cmd Flash操作指令码。
 * @param  [in,out]  buffer 数据缓冲区。
 * @param  [in]  length 缓冲区长度，driver层限制其值小于4。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_sfc_reg_flash_opreations(uint8_t id, uint32_t opt_type, spi_opreation_t opt, uint8_t *buffer,
    uint32_t length);

#if defined(CONFIG_SFC_SUPPORT_DMA)

/**
 * @if Eng
 * @brief  SFC read operation in DMA mode
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  flash_addr Start address of the flash memory to be read.
 * @param  [out] read_buffer Pointer to data buffer
 * @param  [in]  read_size Amount of data to be read.
 * @retval ERRCODE_SUCC      Success.
 * @retval Other             Failure. For details, see @ref errcode_t
 * @else
 * @brief  SFC dma模式读操作
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  flash_addr 读操作flash首地址。
 * @param  [out] read_buffer 读数据缓冲区。
 * @param  [in]  read_size 读数据总字节数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_sfc_dma_read(uint8_t id, uint32_t flash_addr, uint8_t *read_buffer, uint32_t read_size);

/**
 * @if Eng
 * @brief  SFC read operation in DMA mode.
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  flash_addr Start address of the flash memory to be write.
 * @param  [in]  write_data Pointer to data buffer
 * @param  [in]  write_size Length of the write data.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  SFC dma模式读操作。
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  flash_addr  写操作flash首地址。
 * @param  [in]  write_data  预计写入的数据。
 * @param  [in]  write_size  写数据字节数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_sfc_dma_write(uint8_t id, uint32_t flash_addr, uint8_t *write_data, uint32_t write_size);
#endif /* CONFIG_SFC_SUPPORT_DMA */

#if defined(CONFIG_SFC_SUPPORT_LPM)
/**
 * @if Eng
 * @brief  SFC suspend.
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  挂起SFC。
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_sfc_suspend(uint8_t id);

/**
 * @if Eng
 * @brief  SFC resume.
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  quad_mode Initialization operation instruction for enabling the QSPI mode of the flash memory.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  恢复SFC。
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  quad_mode Flash开启QSPI模式的初始化操作指令。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_sfc_resume(uint8_t id, flash_cmd_execute_t *quad_mode);
#endif

/**
 * @if Eng
 * @brief  Set the base address of registers.
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t
 * @else
 * @brief  设置SFC寄存器的基地址
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_sfc_regs_init(uint8_t id);

/**
 * @if Eng
 * @brief  Clear the base address of registers has been set by @ref hal_watchdog_regs_init.
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @else
 * @brief  清除由 @ref hal_sfc_regs_init 设置的基地址
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @endif
 */
void hal_sfc_regs_deinit(uint8_t id);

#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
/**
 * @if Eng
 * @brief  Enable continue read.
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  read_opreation Read command information.
 * @param  [in]  m_byte SFC continue read m_byte.
 * @else
 * @brief  使能连续读模式
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  read_opreation 读指令信息.
 * @param  [in]  m_byte 连续读指令中的m字节.
 * @endif
 */
void hal_sfc_enable_continue_read(uint8_t id, spi_opreation_t read_opreation, uint8_t m_byte);

/**
 * @if Eng
 * @brief  Disable continue read.
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  read_opreation Read command information.
 * @else
 * @brief  去使能连续读模式
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  read_opreation 读指令信息.
 * @endif
 */
void hal_sfc_disable_continue_read(uint8_t id, spi_opreation_t read_opreation);
#endif

/**
 * @if Eng
 * @brief  Send Flash write enable CMD.
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  cmd Write enable command, use SPI_CMD_WREN or SPI_CMD_VOLATILE_WREN.
 * @else
 * @brief  发送Flash写使能
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  cmd Flash写使能命令，使用SPI_CMD_WREN 或 SPI_CMD_VOLATILE_WREN.
 * @endif
 */
errcode_t hal_sfc_write_enable(uint8_t id, uint8_t cmd);

/**
 * @if Eng
 * @brief  Wait Flash ready idle.
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  wip_bit Flash reg0 busy bit offset.
 * @else
 * @brief  等待flash空闲
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  wip_bit Flash状态寄存器0的busy bit位置.
 * @endif
 */
errcode_t hal_sfc_regs_wait_ready(uint8_t id, uint8_t wip_bit);

#if defined(CONFIG_SFC_SUPPORT_DEEP_POWERDOWN)
/**
 * @if Eng
 * @brief  External flash enter deep power down state.
 * @param  [in]  sfc_id SFC ID.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  Flash进入掉电模式。
 * @param  [in]  sfc_id SFC ID.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_sfc_enter_deep_powerdown(uint8_t id);

/**
 * @if Eng
 * @brief  External flash exit deep power down state.
 * @param  [in]  sfc_id SFC ID.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  Flash退出掉电模式。
 * @param  [in]  sfc_id SFC ID.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_sfc_release_from_deep_powerdown(uint8_t id);
#endif
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif