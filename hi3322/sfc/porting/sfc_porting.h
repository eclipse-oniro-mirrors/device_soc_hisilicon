/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides sfc port template \n
 *
 * History: \n
 * 2022-11-30， Create file. \n
 */
#ifndef SFC_PORTING_H
#define SFC_PORTING_H

#include "sfc_config_info.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup drivers_port_sfc SFC
 * @ingroup  drivers_port
 * @{
 */

#define SFC_SAFE_OFFSET             0x10000
#define SFC_WAIT_TIMEOUT_MS         200000
#define FLASH_CHIP_PROTECT_END      0x400000

typedef enum {
    SFC_ID_0,
    SFC_ID_MAX
} sfc_id_t;

/**
 * @if Eng
 * @brief  Read, write and erase operations.
 * @else
 * @brief  Flash的基本信息和操作信息
 * @endif
*/
typedef struct flash_spi_ctrl {
    uint32_t        chip_size;                                   /*!<@if Eng Size of the flash mapped to the SFC
                                                                     @else   Flash映射到SFC的大小      @endif
                                                                  */
    uint32_t        flash_id;                                   /*!<@if Eng Flash id
                                                                     @else   Flash id      @endif
                                                                 */
    spi_opreation_t read_opreation;                              /*!<@if Eng Read operation
                                                                     @else   读操作               @endif
                                                                  */
    uint32_t        erase_cmd_num;                               /*!<@if Eng Number of erase commands
                                                                     @else   擦除指令的个数@endif
                                                                  */
    spi_opreation_t write_opreation;                             /*!<@if Eng Write opreation
                                                                     @else   写操作               @endif
                                                                  */
    spi_opreation_t *erase_opreation_array;                       /*!<@if Eng Erase opreations
                                                                     @else   擦除操作      @endif
                                                                   */
    flash_cmd_execute_t *quad_mode;                              /*!<@if Eng Quad SPI Enable opreations
                                                                     @else   Quad SPI 使能操作      @endif
                                                                  */
#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
    continue_read_info_t continue_info;
#endif
} flash_spi_ctrl_t;

/**
 * @if Eng
 * @brief  Get SFC bus space start address.
 * @param  [in]  sfc_id SFC ID.
 * @retval SFC bus space start address.
 * @else
 * @brief  获取SFC总线空间首地址。
 * @param  [in]  sfc_id SFC ID.
 * @retval SFC总线空间首地址。
 * @endif
 */
uintptr_t sfc_port_get_sfc_start_addr(uint8_t id);

/**
 * @if Eng
 * @brief  Get SFC bus space end address.
 * @param  [in]  sfc_id SFC ID.
 * @retval SFC bus space end address.
 * @else
 * @brief  获取SFC总线空间尾地址。
 * @param  [in]  sfc_id SFC ID.
 * @retval SFC总线空间尾地址。
 * @endif
 */
uintptr_t sfc_port_get_sfc_end_addr(uint8_t id);

/**
 * @if Eng
 * @brief  Get base address for SFC bus registers.
 * @param  [in]  sfc_id SFC ID.
 * @retval Base address for SFC bus registers.
 * @else
 * @brief  获取SFC公共配置相关寄存器基地址。
 * @param  [in]  sfc_id SFC ID.
 * @retval SFC公共配置相关寄存器基地址。
 * @endif
 */
uintptr_t sfc_port_get_sfc_global_conf_base_addr(uint8_t id);

/**
 * @if Eng
 * @brief  Get base address for SFC bus registers.
 * @param  [in]  sfc_id SFC ID.
 * @retval Base address for SFC bus registers.
 * @else
 * @brief  获取SFC总线模式访问相关寄存器基地址。
 * @param  [in]  sfc_id SFC ID.
 * @retval SFC总线模式访问相关寄存器基地址。
 * @endif
 */
uintptr_t sfc_port_get_sfc_bus_regs_base_addr(uint8_t id);

/**
 * @if Eng
 * @brief  Get base address for SFC DMA registers.
 * @param  [in]  sfc_id SFC ID.
 * @retval Base address for SFC DMA registers.
 * @else
 * @param  [in]  sfc_id SFC ID.
 * @brief  获取SFC DMA操作相关寄存器基地址。
 * @retval SFC DMA操作相关寄存器基地址。
 * @endif
 */
uintptr_t sfc_port_get_sfc_bus_dma_regs_base_addr(uint8_t id);

/**
 * @if Eng
 * @brief  Get base address for SFC command registers.
 * @param  [in]  sfc_id SFC ID.
 * @retval Base address for SFC command registers.
 * @else
 * @brief  获取SFC SPI操作寄存器相关基地址。
 * @param  [in]  sfc_id SFC ID.
 * @retval SFC SPI操作寄存器相关基地址。
 * @endif
 */
uintptr_t sfc_port_get_sfc_cmd_regs_base_addr(uint8_t id);

/**
 * @if Eng
 * @brief  Get base address for SFC command data buffer registers.
 * @param  [in]  sfc_id SFC ID.
 * @retval Base address for SFC command data buffer registers.
 * @else
 * @brief  获取SFC SPI操作数据缓冲区寄存器基地址。
 * @param  [in]  sfc_id SFC ID.
 * @retval SFC SPI操作数据缓冲区寄存器基地址。
 * @endif
 */
uintptr_t sfc_port_get_sfc_cmd_databuf_base_addr(uint8_t id);

#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
/**
 * @if Eng
 * @brief  Get base address for SFC command data buffer registers.
 * @param  [in]  sfc_id SFC ID.
 * @retval Base address for SFC command data buffer registers.
 * @else
 * @brief  获取SFC SPI操作数据缓冲区寄存器基地址。
 * @param  [in]  sfc_id SFC ID.
 * @retval SFC SPI操作数据缓冲区寄存器基地址。
 * @endif
 */
uintptr_t sfc_port_get_sfc_continue_read_base_addr(uint8_t id);
#endif

/**
 * @if Eng
 * @brief  Set the single delay time for querying the flash WIP bit.
 * @param  [in]  sfc_id SFC ID.
 * @param  [in]  delay_us Delay time for querying the flash WIP bit.
 * @else
 * @brief  设置查询Flash WIP位的单次延时时间。
 * @param  [in]  sfc_id SFC ID.
 * @param  [in]  delay_us 查询Flash WIP位的单次延时时间。
 * @endif
 */
void sfc_port_set_delay_once_time(uint8_t id, uint32_t delay_us);

/**
 * @if Eng
 * @brief  Get the single delay time for querying the flash WIP bit
 * @param  [in]  sfc_id SFC ID.
 * @retval single delay time for querying the flash WIP bit.The unit is ms.
 * @else
 * @brief  获取查询Flash WIP位的单次延时时间
 * @param  [in]  sfc_id SFC ID.
 * @retval 查询Flash WIP位的单次延时时间，单位为us
 * @endif
 */
uint32_t sfc_port_get_delay_once_time(uint8_t id);

/**
 * @if Eng
 * @brief  Set delay times for querying the flash WIP bit
 * @param  [in]  sfc_id SFC ID.
 * @param  [in]  delay_times Delay times for querying the flash WIP bit
 * @else
 * @brief  配置查询Flash WIP位的延时次数
 * @param  [in]  sfc_id SFC ID.
 * @param  [in]  delay_times 查询Flash WIP位的延时次数
 * @endif
 */
void sfc_port_set_delay_times(uint8_t id, uint32_t delay_times);

/**
 * @if Eng
 * @brief  Get delay times for querying the flash WIP bit
 * @param  [in]  sfc_id SFC ID.
 * @retval delay times for querying the flash WIP bit
 * @else
 * @brief  获取查询Flash WIP位的延时次数
 * @param  [in]  sfc_id SFC ID.
 * @retval 查询Flash WIP位的延时次数
 * @endif
 */
uint32_t sfc_port_get_delay_times(uint8_t id);

/**
 * @if Eng
 * @brief  Disable flash bus read.
 * @param  [in]  sfc_id SFC ID.
 * @else
 * @brief  关闭flash总线读
 * @param  [in]  sfc_id SFC ID.
 * @endif
 */
void sfc_port_disable_bus_read(uint8_t id);

/**
 * @if Eng
 * @brief  Disable flash bus write.
 * @param  [in]  sfc_id SFC ID.
 * @else
 * @brief  关闭flash总线写
 * @param  [in]  sfc_id SFC ID.
 * @endif
 */
void sfc_port_disable_bus_write(uint8_t id);

/**
 * @if Eng
 * @brief  sfc lock initialize.
 * @param  [in]  sfc_id SFC ID.
 * @else
 * @brief  SFC锁初始化。
 * @param  [in]  sfc_id SFC ID.
 * @endif
 */
void sfc_port_lock_init(uint8_t id);

/**
 * @if Eng
 * @brief  sfc lock.
 * @param  [in]  sfc_id SFC ID.
 * @retval lock status.
 * @else
 * @brief  SFC上锁。
 * @param  [in]  sfc_id SFC ID.
 * @retval 锁状态。
 * @endif
 */
uint32_t sfc_port_lock(uint8_t id);

/**
 * @if Eng
 * @brief  sfc unlock.
 * @param  [in]  sfc_id SFC ID.
 * @param [in] lock_sts lock status.
 * @else
 * @brief  SFC解锁。
 * @param  [in]  sfc_id SFC ID.
 * @param [in] lock_sts 锁状态，传入的为由lock接口返回的值
 * @endif
 */
void sfc_port_unlock(uint8_t id, uint32_t lock_sts);

/**
 * @if Eng
 * @brief  sfc check flash QE status and set QE enable.
 * @param  [in]  sfc_id SFC ID.
 * @param [in] flash_id flash id.
 * @else
 * @brief  检查对应Flash ID的QE状态，如果没使能，则使能QE。
 * @param  [in]  sfc_id SFC ID.
 * @param [in] flash_id flash id.
 * @endif
 */
errcode_t sfc_port_check_and_enable_qe_status(uint8_t id, uint32_t flash_id);

/**
 * @if Eng
 * @brief  Write flash reg status.
 * @param  [in]  sfc_id SFC ID.
 * @param [in] write_cmd write cmd.
 * @param [in] wren_cmd Write enable command, use SPI_CMD_WREN or SPI_CMD_VOLATILE_WREN..
 * @param [in] value flash reg status value.
 * @param [in] len value len.
 * @else
 * @brief  写Flash状态寄存器。
 * @param  [in]  sfc_id SFC ID.
 * @param [in] write_cmd Flash写命令.
 * @param [in] wren_cmd 写使能命令， SPI_CMD_WREN 或 SPI_CMD_VOLATILE_WREN..
 * @param [in] value 将要写入的flash状态寄存器值.
 * @param [in] len Flash状态寄存器值的长度.
 * @endif
 */
errcode_t sfc_port_write_flash_regs(uint8_t id, uint8_t write_cmd, uint8_t wren_cmd, uint8_t *value, uint8_t len);

/**
 * @if Eng
 * @brief  read flash reg status.
 * @param  [in]  sfc_id SFC ID.
 * @param [in] reg_info return flash reg status.
 * @param [in] cmd read Flash reg status cmd.
 * @else
 * @brief  读取Flash状态寄存器。
 * @param  [in]  sfc_id SFC ID.
 * @param [in] reg_info 返回的Flash状态寄存器值.
 * @param [in] cmd 读取Flash状态寄存器命令
 * @endif
 */
errcode_t sfc_port_read_flash_reg(uint8_t id, uint8_t *reg_info, uint8_t cmd);

/**
 * @if Eng
 * @brief  read flash reg status and check mask value.
 * @param  [in]  sfc_id SFC ID.
 * @param [in] reg_info return flash reg status.
 * @param [in] cmd read Flash reg status cmd.
 * @param [in] check_mask flash reg mask check value.
 * @else
 * @brief  检查并读取Flash状态寄存器。
 * @param  [in]  sfc_id SFC ID.
 * @param [in] reg_info 返回的Flash状态寄存器值.
 * @param [in] cmd 读取Flash状态寄存器命令
 * @param [in] check_mask Flash状态寄存器掩码比较值.
 * @endif
 */
errcode_t sfc_port_read_check_flash_reg(uint8_t id, uint8_t *reg_info, uint8_t cmd, uint8_t check_mask);

/**
 * @if Eng
 * @brief  Flash unprotect.
 * @param  [in]  sfc_id SFC ID.
 * @param [in] flash_offset flash offset addr.
 * @param [in] is_volatile use volatile or non volatile write enable cmd.
 * @else
 * @brief  根据访问地址对Flash解保护。
 * @param  [in]  sfc_id SFC ID.
 * @param [in] flash_offset Flash访问地址
 * @param [in] is_volatile 易失或非易失写使能选择.
 * @endif
 */
errcode_t sfc_port_flash_unprotect(uint8_t id, uint32_t flash_offset, bool is_volatile);

/**
 * @if Eng
 * @brief  Flash protect all zone.
 * @param  [in]  sfc_id SFC ID.
 * @param [in] is_volatile use volatile or non volatile write enable cmd.
 * @else
 * @brief  Flash保护所有区域。
 * @param  [in]  sfc_id SFC ID.
 * @param [in] is_volatile 易失或非易失写使能选择.
 * @endif
 */
errcode_t sfc_port_flash_protect(uint8_t id, bool is_volatile);

/**
 * @if Eng
 * @brief  sfc write lock.
 * @param [in] start_addr start_addr.
 * @param [in] end_addr end_addr.
 * @retval lock status.
 * @else
 * @brief  SFC写锁。
 * @param [in] start_addr 起始地址。
 * @param [in] end_addr 结束地址。
 * @retval 锁状态。
 * @endif
 */
uint32_t sfc_port_write_lock(uint32_t start_addr, uint32_t end_addr);

/**
 * @if Eng
 * @brief  sfc write unlock.
 * @param [in] lock_sts lock status.
 * @else
 * @brief  SFC写解锁。
 * @param [in] lock_sts 锁状态。
 * @endif
 */
void sfc_port_write_unlock(uint32_t lock_sts);

/**
 * @if Eng
 * @brief  read flash id.
 * @param [in] flash_id flash id pointer.
 * @else
 * @brief  读取Flash ID。
 * @param [in] flash_id flash id指针.
 * @endif
 */
errcode_t sfc_port_get_flash_id(uint32_t *flash_id);

#ifdef CONFIG_SUPPORT_FLASH_SUSPEND_RESUME
/**
 * @if Eng
 * @brief  Enable flash suspend and resume.
 * @else
 * @brief  使能Flash suspend和resume。
 * @endif
 */
void sfc_port_enable_flash_suspend_resume(void);

/**
 * @if Eng
 * @brief  Flash erase porting.
 * @else
 * @brief  Flash擦除扩展接口。
 * @endif
 */
errcode_t sfc_port_sfc_reg_erase(uint32_t flash_addr, spi_opreation_t erase_opreation, uint32_t addr_en);

/**
 * @if Eng
 * @brief  Flash write porting.
 * @else
 * @brief  Flash写扩展接口。
 * @endif
 */
errcode_t sfc_port_sfc_reg_write(uint32_t flash_addr, uint8_t *write_data, uint32_t write_size,
    spi_opreation_t write_opreation);
#endif

/**
 * @}
 */
errcode_t sfc_port_clock_div_set(sfc_id_t id, uint32_t div);

flash_spi_info_t *sfc_get_flash_info(uint32_t flash_id);

/**
 * @if Eng
 * @brief  switch to 4-byte mode.
 * @param  [in]  id SFC ID.
 * @param  [in]  flash_id flash ID.
 * @param  [in]  chip_size The size of the chip.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  决策是否切换到4字节模式。
 * @param  [in]  id SFC ID。
 * @param  [in]  flash_id flash ID。
 * @param  [in]  chip_size 芯片的大小。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t sfc_port_switch_to_4byte_mode(uint8_t id, uint32_t flash_id, uint32_t chip_size);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif