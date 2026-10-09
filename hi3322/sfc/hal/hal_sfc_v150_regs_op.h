/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Provides v150 SFC register operation api \n
 *
 * History: \n
 * 2024-10-01， Create file. \n
 */
#ifndef HAL_SFC_V150_REGS_OP_H
#define HAL_SFC_V150_REGS_OP_H

#include <stdint.h>
#include "errcode.h"
#include "hal_sfc_v150_regs_def.h"
#include "sfc_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_sfc_v150_regs_op SFC v150 Regs Operation
 * @ingroup  drivers_hal_sfc
 * @{
 */

extern uintptr_t g_sfc_global_conf_regs[SFC_ID_MAX];
extern uintptr_t g_sfc_bus_regs[SFC_ID_MAX];
extern uintptr_t g_sfc_bus_dma_regs[SFC_ID_MAX];
extern uintptr_t g_sfc_cmd_regs[SFC_ID_MAX];
extern uintptr_t g_sfc_cmd_databuf[SFC_ID_MAX];
#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
extern uintptr_t g_sfc_continue_read[SFC_ID_MAX];
#endif

/**
 * @brief  SPI mode opreation base info.
 */
typedef struct hal_spi_opreation {
    spi_opreation_t opt;            /*!< SPI opreation info */
    uint32_t data_size;             /*!< SPI opreation data size */
    uint32_t dummy_byte;            /*!< SPI opreation dummy byte */
} hal_spi_opreation_t;

/**
 * @brief  Set cmd config and cmd ins by basic command info
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in] hal_opt  SPI mode opreation parameters.
 */
void hal_sfc_regs_set_opt(uint8_t id, hal_spi_opreation_t hal_opt);

/**
 * @brief  Set cmd config by command attribute
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in] rw  region rw of @ref cmd_config_t
 * @param  [in] data_en  region data_en of @ref cmd_config_t
 * @param  [in] addr_en  region addr_en of @ref cmd_config_t
 */
void hal_sfc_regs_set_opt_attr(uint8_t id, uint32_t rw, uint32_t data_en, uint32_t addr_en);

/**
 * @brief  Wait region start of @ref cmd_config_t be 0
 * @param  [in]  SFC ID. @ref sfc_id_t
 */
void hal_sfc_regs_wait_config(uint8_t id);

/**
 * @brief  Wait region dma_start of @ref bus_dma_ctrl_t be 0
 * @param  [in]  SFC ID. @ref sfc_id_t
 */
void hal_sfc_dma_wait_done(uint8_t id);

/**
 * @brief  Set read command info in bus mode
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in] opt_read  read opreation info.
 */
void hal_sfc_regs_set_bus_read(uint8_t id, spi_opreation_t opt_read);

/**
 * @brief  Diasble read in bus mode
 * @param  [in]  SFC ID. @ref sfc_id_t
 */
void hal_sfc_regs_disable_bus_read(uint8_t id);

/**
 * @brief  Set write command info in bus mode
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in] opt_write  write opreation info.
 */
void hal_sfc_regs_set_bus_write(uint8_t id, spi_opreation_t opt_write);

/**
 * @brief  Diasble write in bus mode
 * @param  [in]  SFC ID. @ref sfc_id_t
 */
void hal_sfc_regs_disable_bus_write(uint8_t id);

/**
 * @brief  Set addr mode
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  mode SFC addr mode.
 */
static inline void hal_sfc_regs_set_addr_mode(uint8_t id, uint8_t mode)
{
    global_config_t config;
    config.d32 = ((global_conf_regs_t *)g_sfc_global_conf_regs[id])->global_config;
    config.b.flash_addr_mode = mode;
    ((global_conf_regs_t *)g_sfc_global_conf_regs[id])->global_config = config.d32;
}

/**
 * @brief  Set rd delay
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  mode SFC rd delay.
 */
static inline void hal_sfc_regs_set_rd_delay(uint8_t id, uint8_t delay)
{
    global_config_t config;
    config.d32 = ((global_conf_regs_t *)g_sfc_global_conf_regs[id])->global_config;
    config.b.rd_delay = delay;
    ((global_conf_regs_t *)g_sfc_global_conf_regs[id])->global_config = config.d32;
}

/**
 * @brief  Set timing
 * @param  [in]  SFC ID. @ref sfc_id_t
 */
static inline void hal_sfc_regs_set_timing(uint8_t id, uint32_t timing)
{
    ((global_conf_regs_t *)g_sfc_global_conf_regs[id])->timing = timing;
}

/**
 * @brief  Set the value of @ref bus_dma_regs_t.bus_dma_flash_saddr
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  flash_saddr The value of @ref bus_dma_regs_t.bus_dma_flash_saddr
 */
static inline void hal_sfc_regs_set_bus_dma_flash_saddr(uint8_t id, uint32_t flash_saddr)
{
    ((bus_dma_regs_t *)g_sfc_bus_dma_regs[id])->bus_dma_flash_saddr = flash_saddr;
}

/**
 * @brief  Set the value of @ref bus_dma_regs_t.bus_dma_mem_saddr
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  dma_buffer data buffer to read or write
 */
static inline void hal_sfc_regs_set_bus_dma_mem_addr(uint8_t id, uint8_t *dma_buffer)
{
    ((bus_dma_regs_t *)g_sfc_bus_dma_regs[id])->bus_dma_mem_saddr = (uintptr_t)dma_buffer;
}

/**
 * @brief  Set the value of @ref bus_dma_regs_t.bus_dma_len
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  length The value of @ref bus_dma_regs_t.bus_dma_len
 */
static inline void hal_sfc_regs_set_bus_dma_len(uint8_t id, uint32_t length)
{
    bus_dma_len_t len_conf;
    len_conf.d32 = ((bus_dma_regs_t *)g_sfc_bus_dma_regs[id])->bus_dma_len;
    len_conf.b.dma_len = length - 1;
    ((bus_dma_regs_t *)g_sfc_bus_dma_regs[id])->bus_dma_len = len_conf.d32;
}

/**
 * @brief  Set the value of @ref bus_dma_regs_t.bus_dma_ahb_ctrl
 * @param  [in]  SFC ID. @ref sfc_id_t
 */
static inline void hal_sfc_regs_set_bus_dma_ahb_ctrl(uint8_t id)
{
    bus_dma_ahb_ctrl_t ahb_ctrl;
    ahb_ctrl.d32 = 0x7;
    ((bus_dma_regs_t *)g_sfc_bus_dma_regs[id])->bus_dma_ahb_ctrl = ahb_ctrl.d32;
}

/**
 * @brief  Set the value of @ref bus_dma_regs_t.bus_dma_ctrl
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  rw The value of @ref bus_dma_ctrl_t.dma_rw
 */
static inline void hal_sfc_regs_set_bus_dma_ctrl(uint8_t id, uint32_t rw)
{
    bus_dma_ctrl_t dma_ctrl;
    dma_ctrl.d32 = ((bus_dma_regs_t *)g_sfc_bus_dma_regs[id])->bus_dma_ctrl;
    dma_ctrl.b.dma_sel_cs = 0x1;
    dma_ctrl.b.dma_rw = rw;
    dma_ctrl.b.dma_start = 0x1;
    ((bus_dma_regs_t *)g_sfc_bus_dma_regs[id])->bus_dma_ctrl = dma_ctrl.d32;
}

#if defined(CONFIG_SFC_SUPPORT_COMPLECT)
/**
 * @brief  Set the value of @ref sfc_global_conf_regs
 * @param  [in]  index The index of @ref cmd_databufs_t.global_config
 * @param  [in]  val The value of @ref cmd_databufs_t.global_config
 */
static inline void hal_sfc_regs_enable_complect(void)
{
    ((global_conf_regs_t *)g_sfc_global_conf_regs[0])->global_config = 0x80;
}

/**
 * @brief  Get the value of @ref sfc_global_conf_regs
 */
static inline uint32_t hal_sfc_regs_get_complect_status(void)
{
    return ((global_conf_regs_t *)g_sfc_global_conf_regs[0])->global_config;
}
#endif

/**
 * @brief  Set the value of @ref cmd_databufs_t
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  index The index of @ref cmd_databufs_t.cmd_databuf
 * @param  [in]  val The value of @ref cmd_databufs_t.cmd_databuf[index]
 */
static inline void hal_sfc_regs_set_databuf(uint8_t id, uint32_t index, uint32_t val)
{
    ((cmd_databufs_t *)g_sfc_cmd_databuf[id])->cmd_databuf[index] = val;
}

/**
 * @brief  Get the value of @ref cmd_databufs_t
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  index The index of @ref cmd_databufs_t.cmd_databuf
 */
static inline uint32_t hal_sfc_regs_get_databuf(uint8_t id, uint32_t index)
{
    return ((cmd_databufs_t *)g_sfc_cmd_databuf[id])->cmd_databuf[index];
}

/**
 * @brief  Set the value of @ref bus_regs_t.bus_base_addr_cs1
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  val The value of @ref bus_regs_t.bus_base_addr_cs1
 */
static inline void hal_sfc_regs_set_bus_baseaddr(uint8_t id, uint32_t val)
{
    bus_base_addr_t base_addr;
    base_addr.d32 = ((bus_regs_t *)g_sfc_bus_regs[id])->bus_base_addr_cs1;
    base_addr.b.bus_base_addr_high_cs = val;
    ((bus_regs_t *)g_sfc_bus_regs[id])->bus_base_addr_cs1 = base_addr.d32;
}

/**
 * @brief  Set the value of @ref bus_regs_t.bus_flash_size
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  val The value of @ref bus_flash_size_t.flash_size_cs1
 */
static inline void hal_sfc_regs_set_bus_flash_size(uint8_t id, uint32_t val)
{
    bus_flash_size_t flash_size;
    flash_size.d32 = ((bus_regs_t *)g_sfc_bus_regs[id])->bus_flash_size;
#if defined(CONFIG_SFC_SUPPORT_COMPLECT)
    flash_size.b.flash_size_cs0 = val;
#endif
    flash_size.b.flash_size_cs1 = val;
    ((bus_regs_t *)g_sfc_bus_regs[id])->bus_flash_size = flash_size.d32;
}

/**
 * @brief  Set the value of @ref cmd_regs_t.cmd_addr
 * @param  [in]  SFC ID. @ref sfc_id_t
 * @param  [in]  val The value of @ref cmd_addr_t.cmd_addr
 */
static inline void hal_sfc_regs_set_cmd_addr(uint8_t id, uint32_t val)
{
    cmd_addr_t addr;
    addr.d32 = ((cmd_regs_t *)g_sfc_cmd_regs[id])->cmd_addr;
    addr.b.cmd_addr = val;
    ((cmd_regs_t *)g_sfc_cmd_regs[id])->cmd_addr = addr.d32;
}

static inline uint32_t hal_sfc_regs_get_timing(uint8_t id)
{
    return ((global_conf_regs_t *)g_sfc_global_conf_regs[id])->timing;
}

static inline uint32_t hal_sfc_regs_get_sfc_bus_config1(uint8_t id)
{
    return ((bus_regs_t *)g_sfc_bus_regs[id])->bus_config1;
}

static inline void hal_sfc_regs_set_sfc_bus_config1(uint8_t id, uint32_t val)
{
    ((bus_regs_t *)g_sfc_bus_regs[id])->bus_config1 = val;
}

static inline uint32_t hal_sfc_regs_get_sfc_bus_config2(uint8_t id)
{
    return ((bus_regs_t *)g_sfc_bus_regs[id])->bus_config2;
}

static inline void hal_sfc_regs_set_sfc_bus_config2(uint8_t id, uint32_t val)
{
    ((bus_regs_t *)g_sfc_bus_regs[id])->bus_config2 = val;
}

static inline uint32_t hal_sfc_regs_get_sfc_bus_flash_size(uint8_t id)
{
    return ((bus_regs_t *)g_sfc_bus_regs[id])->bus_flash_size;
}

static inline void hal_sfc_regs_set_sfc_bus_flash_size(uint8_t id, uint32_t val)
{
    ((bus_regs_t *)g_sfc_bus_regs[id])->bus_flash_size = val;
}

static inline uint32_t hal_sfc_regs_get_bus_base_addr_cs0(uint8_t id)
{
    return ((bus_regs_t *)g_sfc_bus_regs[id])->bus_base_addr_cs0;
}

static inline void hal_sfc_regs_set_bus_base_addr_cs0(uint8_t id, uint32_t val)
{
    ((bus_regs_t *)g_sfc_bus_regs[id])->bus_base_addr_cs0 = val;
}

static inline uint32_t hal_sfc_regs_get_bus_base_addr_cs1(uint8_t id)
{
    return ((bus_regs_t *)g_sfc_bus_regs[id])->bus_base_addr_cs1;
}

static inline void hal_sfc_regs_set_bus_base_addr_cs1(uint8_t id, uint32_t val)
{
    ((bus_regs_t *)g_sfc_bus_regs[id])->bus_base_addr_cs1 = val;
}

#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
static inline void hal_sfc_regs_set_mbyte(uint8_t id, uint8_t byte)
{
    continue_read_mbyte_t config;
    config.d32 = ((continue_read_t *)g_sfc_continue_read[id])->m_config;
    config.b.mbyte = byte;
    ((continue_read_t *)g_sfc_continue_read[id])->m_config = config.d32;
}

static inline void hal_sfc_regs_ctrl_continue_read(uint8_t id, bool enable)
{
    continue_read_status_t status;
    status.d32 = ((continue_read_t *)g_sfc_continue_read[id])->status;
    status.b.enable = enable;
    ((continue_read_t *)g_sfc_continue_read[id])->status = status.d32;
}

static inline bool hal_sfc_get_continue_read_status(uint8_t id)
{
    continue_read_status_t status;
    status.d32 = ((continue_read_t *)g_sfc_continue_read[id])->status;
    return (status.b.enable == 1) ? true : false;
}
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