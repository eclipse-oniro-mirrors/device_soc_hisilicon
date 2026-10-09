/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides dma port \n
 *
 * History: \n
 * 2022-10-16， Create file. \n
 */

#ifndef DMA_PORTING_H
#define DMA_PORTING_H

#include <stdint.h>
#include <stdbool.h>
#include "platform_core.h"
#include "dma.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_port_dma_v151 DMA V151
 * @ingroup  drivers_port_dma
 * @{
 */

/**
 * @brief  DMA channel ID.
 */
typedef enum {
    DMA_CHANNEL_0,    /*!< DMA channel 0. */
    DMA_CHANNEL_1,    /*!< DMA channel 1. */
    DMA_CHANNEL_2,    /*!< DMA channel 2. */
    DMA_CHANNEL_3,    /*!< DMA channel 3. */
    DMA_CHANNEL_4,    /*!< DMA channel 4. */
    DMA_CHANNEL_5,    /*!< DMA channel 5. */
    DMA_CHANNEL_6,    /*!< DMA channel 6. */
    DMA_CHANNEL_7,    /*!< DMA channel 7. */
    DMA_CHANNEL_8,    /*!< DMA channel 8 */
    DMA_CHANNEL_9,    /*!< DMA channel 9 */
    DMA_CHANNEL_10,   /*!< DMA channel 10 */
    DMA_CHANNEL_11,   /*!< DMA channel 11 */
    DMA_CHANNEL_NONE = DMA_CHANNEL_MAX_NUM
} dma_channel_t;

/**
 * @brief  DMA handshaking source select.
 */
typedef enum {
#if (CORE == APPS)
    HAL_DMA_HANDSHAKING_SPI1_MS_TX,
    HAL_DMA_HANDSHAKING_SPI1_MS_RX,
    HAL_DMA_HANDSHAKING_M_RESERVED0,
    HAL_DMA_HANDSHAKING_M_RESERVED1,
    HAL_DMA_HANDSHAKING_SPI0_MS_TX,
    HAL_DMA_HANDSHAKING_SPI0_MS_RX,
    HAL_DMA_HANDSHAKING_M_PWM,
    HAL_DMA_HANDSHAKING_AON_PWM,
    HAL_DMA_HANDSHAKING_M_TTCAN_DMA_REQ,
    HAL_DMA_HANDSHAKING_M_RESERVED2,
    HAL_DMA_HANDSHAKING_M_RESERVED3,
    HAL_DMA_HANDSHAKING_M_RESERVED4,
    HAL_DMA_HANDSHAKING_QSPI2_1CS_TX,
    HAL_DMA_HANDSHAKING_QSPI2_1CS_RX,
    HAL_MDMA_HANDSHAKING_MAX_NUM,
    /* SMDMA */
    HAL_DMA_HANDSHAKING_SDMA = HAL_MDMA_HANDSHAKING_MAX_NUM,
    HAL_DMA_HANDSHAKING_I2C0_TX = HAL_DMA_HANDSHAKING_SDMA + 0,
    HAL_DMA_HANDSHAKING_I2C0_RX,
    HAL_DMA_HANDSHAKING_I2C1_TX,
    HAL_DMA_HANDSHAKING_I2C1_RX,
    HAL_DMA_HANDSHAKING_I2C2_TX,
    HAL_DMA_HANDSHAKING_I2C2_RX,
    HAL_DMA_HANDSHAKING_I2C4_TX,
    HAL_DMA_HANDSHAKING_I2C4_RX,
    HAL_DMA_HANDSHAKING_S_RESERVED0,
    HAL_DMA_HANDSHAKING_S_RESERVED1,
    HAL_DMA_HANDSHAKING_UART_H0_TX,
    HAL_DMA_HANDSHAKING_UART_H0_RX,
    HAL_DMA_HANDSHAKING_UART_H1_TX,
    HAL_DMA_HANDSHAKING_UART_H1_RX,
    HAL_DMA_HANDSHAKING_UART_L0_TX,
    HAL_DMA_HANDSHAKING_UART_L0_RX,
    HAL_DMA_HANDSHAKING_MAX_NUM
#elif (CORE == BT)
    HAL_DMA_HANDSHAKING_UART_L0_TX,
    HAL_DMA_HANDSHAKING_UART_L0_RX,
    HAL_DMA_HANDSHAKING_UART_COM_TX,
    HAL_DMA_HANDSHAKING_UART_COM_RX,
    HAL_MDMA_HANDSHAKING_MAX_NUM,
    HAL_DMA_HANDSHAKING_MAX_NUM = HAL_MDMA_HANDSHAKING_MAX_NUM
#else
    HAL_DMA_HANDSHAKING_MAX_NUM
#endif
} hal_dma_handshaking_source_t;

#ifdef TEST_SUITE
#define HAL_DMA_HANDSHAKING_UART_L_TX HAL_MDMA_HANDSHAKING_MAX_NUM
#define HAL_DMA_HANDSHAKING_UART_L_RX HAL_MDMA_HANDSHAKING_MAX_NUM
#define HAL_DMA_HANDSHAKING_UART_H1_TX HAL_DMA_HANDSHAKING_UART_H1_TX
#define HAL_DMA_HANDSHAKING_UART_H1_RX HAL_DMA_HANDSHAKING_UART_H1_RX
#endif /* TEST_SUITE */

/**
 * @brief  DMA Hardshaking channel.
 */
typedef enum hal_dma_hardshaking_channel {
    HAL_DMA_HARDSHAKING_CHANNEL_0,
    HAL_DMA_HARDSHAKING_CHANNEL_1,
    HAL_DMA_HARDSHAKING_CHANNEL_2,
    HAL_DMA_HARDSHAKING_CHANNEL_3,
    HAL_DMA_HARDSHAKING_CHANNEL_4,
    HAL_DMA_HARDSHAKING_CHANNEL_5,
    HAL_DMA_HARDSHAKING_CHANNEL_6,
    HAL_DMA_HARDSHAKING_CHANNEL_7,
    HAL_DMA_HARDSHAKING_CHANNEL_MAX_NUM,
    HAL_DMA_HARDSHAKING_CHANNEL_NONE = HAL_DMA_HARDSHAKING_CHANNEL_MAX_NUM
} hal_dma_hardshaking_channel_t;

extern uintptr_t g_dma_base_addr;
extern uintptr_t g_sdma_base_addr;

/**
 * @brief  Register the interrupt of dma.
 */
void dma_port_register_irq(void);

/**
 * @brief  Unregister the interrupt of dma.
 */
void dma_port_unregister_irq(void);

/**
 * @brief  Set the channel status of handshaking.
 * @param  [in]  channel  The handshaking select. For details, see @ref hal_dma_handshaking_source_t.
 * @param  [in]  on  Set or clear.
 */
void dma_port_set_handshaking_channel_status(hal_dma_handshaking_source_t channel, bool on);

/**
 * @brief  add sleep veto before dma transfer.
 */
void dma_port_add_sleep_veto(void);

/**
 * @brief  remove sleep veto after dma transfer.
 */
void dma_port_remove_sleep_veto(void);

/**
 * @brief  Release the channel source of handshaking.
 * @param  [in]  ch  The handshaking select. For details, see @ref dma_channel_t.
 */
void dma_port_release_handshaking_source(dma_channel_t ch);

uint32_t dma_porting_adapt_lli_cfg(dma_channel_t ch, uint32_t item);

uint32_t dma_porting_convert_address(uint32_t addr);
#if defined(CONFIG_DMA_SUPPORT_SMDMA)
dma_channel_t dma_port_get_sdma_start_ch(uint8_t burst_length);
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
