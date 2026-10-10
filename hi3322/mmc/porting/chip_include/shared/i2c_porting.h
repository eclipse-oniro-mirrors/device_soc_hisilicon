/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Provides i2c port template \n
 *
 * History: \n
 * 2022-08-15， Create file. \n
 */
#ifndef I2C_PORTING_H
#define I2C_PORTING_H

#include <stdint.h>
#include <stdbool.h>
#include "platform_core.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define I2C_BUS_MAX_NUMBER I2C_BUS_MAX_NUM
#define CONFIG_I2C_TX_BUFFER_DEPTH 8
#define CONFIG_I2C_RX_BUFFER_DEPTH 8

#define CONFIG_I2S_HAS_DMA
#define CONFIG_I2S_TX_BUFFER_DEPTH 8
#define CONFIG_I2S_RX_BUFFER_DEPTH 8

#define I2C_0_SCL_PIN                 S_MGPIO6
#define I2C_0_SDA_PIN                 S_MGPIO7
#if I2C_BUS_MAX_NUMBER > 1
#define I2C_3_SCL_PIN                 S_MGPIO17
#define I2C_3_SDA_PIN                 S_MGPIO18
#endif

/**
 * @brief  Base address list for all of the IPs.
 */
extern uintptr_t g_i2c_base_addrs[I2C_BUS_MAX_NUMBER];

/**
 * @brief  Get i2c base address.
 */
uintptr_t i2c_porting_base_addr_get(i2c_bus_t bus);

/**
 * @brief  Get the bus clock of specified i2c.
 * @param  [in]  bus The I2C bus. see @ref i2c_bus_t
 * @return The bus clock of specified I2C.
 */
uint32_t i2c_port_get_clock_value(i2c_bus_t bus);

/**
 * @brief  Register the interrupt of I2C.
 */
void i2c_port_register_irq(i2c_bus_t bus);

/**
 * @brief  Unregister the interrupt of I2C.
 */
void i2c_port_unregister_irq(i2c_bus_t bus);

/**
 * @brief  I2C lock.
 * @param [in]  bus The bus index of I2C.
 * @return The irq lock number of I2C.
 */
uint32_t i2c_porting_lock(i2c_bus_t bus);

/**
 * @brief  I2C unlock.
 * @param [in]  bus The bus index of I2C.
 * @param [in]  irq_sts The irq lock number of I2C.
 */
void i2c_porting_unlock(i2c_bus_t bus, uint32_t irq_sts);

/**
 * @brief  Save I2C register for suspend.
 * @param [in]  bus The bus index of I2C.
 */
void i2c_save_reg(i2c_bus_t bus);

/**
 * @brief  Save I2C register for resume.
 * @param [in]  bus The bus index of I2C.
 */
void i2c_recovery_reg(i2c_bus_t bus);

#ifdef TEST_SUITE
/**
 * @brief  Init i2c pin for test.
 */
void i2c_port_test_i2c_init_pin(void);
#endif

#if defined(CONFIG_I2C_SUPPORT_DMA)
uint8_t i2c_port_get_dma_trans_dest_handshaking(i2c_bus_t bus);

uint8_t i2c_port_get_dma_trans_src_handshaking(i2c_bus_t bus);
#endif  /* CONFIG_I2C_SUPPORT_DMA */

#if defined(CONFIG_I2C_SUPPORT_LPC)
/**
 * @brief enable I2C clock.
 * @param [in]  bus The bus index of I2C.
 * @param [in]  TURN_ON indicates to enable clock, otherwise to disable.
 */
void i2c_port_clock_enable(i2c_bus_t bus, bool on);
#endif
/**
 * @brief  Set I2C sda tx hold value.
 * @param [in]  bus The bus index of I2C.
 * @param [in]  val tx hold value.
 */
void i2c_port_set_sda_tx_hold_value(i2c_bus_t bus, uint32_t val);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
