/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides spi porting template \n
 *
 * History: \n
 * 2022-08-18， Create file. \n
 */
#ifndef SPI_PORTING_H
#define SPI_PORTING_H

#include <stdint.h>
#include <stdbool.h>
#include "platform_core.h"
#include "dma_porting.h"
#include "hal_spi_v151_regs_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_port_spi SPI
 * @ingroup  drivers_port
 * @{
 */

#define SPI_DMA_TX_DATA_LEVEL_4     4
#define SPI_DMA_RX_DATA_LEVEL_4     3
#define QSPI_DMA_TX_DATA_LEVEL_8    8

/**
 * @brief  Spi dma control register.
 */
typedef enum {
    HAL_SPI_DMA_CONTROL_DISABLE = 0,        //!< Disables the transmit fifo and the receive fifo dma channel.
    HAL_SPI_DMA_CONTROL_RX_ENABLE = 1,      //!< Enables the receive fifo dma channel.
    HAL_SPI_DMA_CONTROL_TX_ENABLE = 2,      //!< Enables the transmit fifo dma channel.
    HAL_SPI_DMA_CONTROL_TXRX_ENABLE = 3,    //!< Enables the transmit fifo and the receive fifo dma channel.
    HAL_SPI_DMA_CONTROL_MAX_NUM,
    HAL_SPI_DMA_CONTROL_NONE = HAL_SPI_DMA_CONTROL_MAX_NUM,
} hal_spi_dma_control_t;

/**
 * @brief  SPI mode.
 */
typedef enum spi_mode {
    SPI_MODE_MASTER,        /*!< SPI Master mode. */
    SPI_MODE_SLAVE,         /*!< SPI Slave mode. */
    SPI_MODE_MAX_NUM,
    SPI_MODE_NONE = SPI_MODE_MAX_NUM
} spi_mode_t;

/**
 * @brief  SPI slave select.
 */
typedef enum spi_slave {
    SPI_SLAVE0 = 0,         /*!< SPI Slave index 0. */
    SPI_SLAVE1,             /*!< SPI Slave index 1. */
    SPI_SLAVE2,             /*!< SPI Slave index 2. */
    SPI_SLAVE_MAX_NUM,
    SPI_SLAVE_NONE = SPI_SLAVE_MAX_NUM
} spi_slave_t;

typedef struct spi_recovery_reg_s {
    volatile uint32_t ctrlr0; // offset0x0
    volatile uint32_t ctrlr1; // Offset: 04h
    volatile uint32_t ser;    // Offset: 10h
    volatile uint32_t baudr;  // Offset: 14h
    volatile uint32_t txftlr; // Offset: 18h
    volatile uint32_t rxftlr; // Offset: 1Ch
    volatile uint32_t txflr;  // Offset: 20h
    volatile uint32_t rxflr;  // Offset: 24h
    volatile uint32_t imr;    // Offset: 2Ch
    volatile uint32_t dmacr;  // Offset: 4Ch
    volatile uint32_t dmatdlr;  // Offset: 50h
    volatile uint32_t dmardlr;  // Offset: 54h
    volatile uint32_t rx_sample_dly; // Offset: F0h
    volatile uint32_t spi_ctrlr0;    // Offset: F4h
} spi_recovery_cfg_t;

/**
 * @brief  Base address list for all of the IPs.
 */
extern spi_v151_regs_t *g_spi_base_addrs[SPI_BUS_MAX_NUM];

/**
 * @brief  Get the base address of specified spi.
 * @param  [in]  bus The bus index of SPI.
 * @return The base address of specified spi.
 */
uintptr_t spi_porting_base_addr_get(spi_bus_t bus);

/**
 * @brief  Get the max slave number can be selected.
 * @param  [in]  bus The bus index of SPI.
 * @return The  max slave number can be selected.
 */
uint32_t spi_porting_max_slave_select_get(spi_bus_t bus);

/**
 * @brief  Set the spi work mode.
 * @param  [in]  bus The bus index of SPI.
 * @param  [in]  mode The mode of SPI.
 */
void spi_porting_set_device_mode(spi_bus_t bus, spi_mode_t mode);

/**
 * @brief  Get the spi work mode.
 * @param  [in]  bus The bus index of SPI.
 * @return The mode of SPI.
 */
spi_mode_t spi_porting_get_device_mode(spi_bus_t bus);

/**
 * @brief  SPI lock.
 * @param [in]  bus The bus index of SPI.
 * @return The irq lock number of SPI.
 */
uint32_t spi_porting_lock(spi_bus_t bus);

/**
 * @brief  SPI unlock.
 * @param [in]  bus The bus index of SPI.
 * @param [in]  irq_sts The irq lock number of SPI.
 */
void spi_porting_unlock(spi_bus_t bus, uint32_t irq_sts);

/**
 * @brief  SPI clock enable or disable.
 * @param [in]  bus The bus index of I2C.
 * @param [in]  on Enable or disable.
 */
void spi_port_dynamic_clock_enable(spi_bus_t bus, bool on);

/**
 * @brief  flash save registers.
 * @param [in]  bus The bus index of SPI.
 * @return none.
 */
void spi_save_reg(spi_bus_t bus);

/**
 * @brief  flash recovery registers.
 * @param [in]  bus The bus index of SPI.
 * @return none.
 */
void spi_recovery_reg(spi_bus_t bus);

#ifdef TEST_SUITE
/**
 * @brief  Init spi pin for test.
 */
void spi_porting_test_spi_init_pin(void);
#endif

uint8_t spi_port_tx_data_level_get(spi_bus_t bus);

uint8_t spi_port_rx_data_level_get(spi_bus_t bus);

uint8_t spi_port_get_dma_trans_dest_handshaking(spi_bus_t bus);

uint8_t spi_port_get_dma_trans_src_handshaking(spi_bus_t bus);

void spi_funcreg_adapt(spi_bus_t bus);

void spi_port_register_irq(spi_bus_t bus);

void spi_port_unregister_irq(spi_bus_t bus);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
