/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description:  APP PMU DRIVER HEADER FILE
 *
 * Create: 2020-01-13
 */
#ifndef HAL_PMU_PERIPHERAL_APPLICATION_H
#define HAL_PMU_PERIPHERAL_APPLICATION_H

#include "chip_io.h"

/** @defgroup connectivity_drivers_hal_pmu_peripheral_app PMU Peripheral
  * @ingroup  connectivity_drivers_hal
  * @{
  */
/**
 * @brief  Describe pheriphral status.
 */
typedef enum {
    HAL_PMU_PERIP_STATUS_CLOCK_OFF = 0,   // !< The peripheral clock is off
    HAL_PMU_PERIP_STATUS_TURNING_ON = 1,  // !< The peripheral clock is truning on
    HAL_PMU_PERIP_STATUS_ERROR,           // !< Get status error
} hal_pmu_perip_status_type_t;

/**
 * @brief  Describe pheriphral request behavior.
 */
typedef enum {
    HAL_PMU_PERIP_REQUEST_RESET = 0,   // !< Request reset the peripheral.
    HAL_PMU_PERIP_REQUEST_DERESET = 1, // !< Request dereset the peripheral.
    HAL_PMU_PERIP_REQUEST_ENTER_LOW_POWER = 2, // !< Request close the clock of the peripheral.
    HAL_PMU_PERIP_REQUEST_EXIT_LOW_POWER = 3,  // !< Request dereset the peripheral and open clock.
} hal_pmu_perip_request_type_t;

/**
 * @brief  Describe mcpu pheriphrals.
 */
typedef enum {
    HAL_PMU_MCPU_PERP_MTOP0_L_STATT,
    HAL_PMU_MCPU_PERIP_TIMER        = HAL_PMU_MCPU_PERP_MTOP0_L_STATT + 0,    // !< The mcpu peripheral timer
    HAL_PMU_MCPU_PERIP_MDMA         = HAL_PMU_MCPU_PERP_MTOP0_L_STATT + 1,    // !< The mcpu peripheral dma
    HAL_PMU_MCPU_PERIP_SMDMA        = HAL_PMU_MCPU_PERP_MTOP0_L_STATT + 2,    // !< The mcpu peripheral dma

    HAL_PMU_MCPU_PERP_MTOP1_L_STATT,
    HAL_PMU_MCPU_PERIP_I2C0         = HAL_PMU_MCPU_PERP_MTOP1_L_STATT + 0,
    HAL_PMU_MCPU_PERIP_I2C1         = HAL_PMU_MCPU_PERP_MTOP1_L_STATT + 1,    // !< The mcpu peripheral I2C1
    HAL_PMU_MCPU_PERIP_I2C2         = HAL_PMU_MCPU_PERP_MTOP1_L_STATT + 2,    // !< The mcpu peripheral I2C2
    HAL_PMU_MCPU_PERIP_I2C3         = HAL_PMU_MCPU_PERP_MTOP1_L_STATT + 3,    // !< The mcpu peripheral I2C3
    HAL_PMU_MCPU_PERIP_M_GPIO       = HAL_PMU_MCPU_PERP_MTOP1_L_STATT + 5,    // !< The mcpu peripheral M_GPIO
    HAL_PMU_MCPU_PERIP_UART_H1      = HAL_PMU_MCPU_PERP_MTOP1_L_STATT + 7,   // !< The mcpu peripheral UARTH1

    HAL_PMU_MCPU_PERP_MTOP2_L_STATT,
    HAL_PMU_MCPU_PERIP_SPI1_MS      = HAL_PMU_MCPU_PERP_MTOP2_L_STATT + 0,   // !< The mcpu peripheral SPI1
    HAL_PMU_MCPU_PERIP_SPI0_MS      = HAL_PMU_MCPU_PERP_MTOP2_L_STATT + 1,   // !< The mcpu peripheral SPI2

    HAL_PMU_MCPU_PERP_MTOP3_L_STATT,
    HAL_PMU_MCPU_PERIP_UART_H0      = HAL_PMU_MCPU_PERP_MTOP3_L_STATT + 0,   // !< The mcpu peripheral UARTH0
    HAL_PMU_MCPU_PERIP_UART_L0      = HAL_PMU_MCPU_PERP_MTOP3_L_STATT + 1,   // !< The mcpu peripheral UARTL0

    HAL_PMU_MCPU_PERP_MSOFT_RST_N1,
    HAL_PMU_MCPU_PERIP_PWM          = HAL_PMU_MCPU_PERP_MSOFT_RST_N1 + 0,   // !< The mcpu peripheral PWM

    HAL_PMU_MCPU_PERIP_MAX,
} hal_pmu_mcpu_perips_type_t;

/**
 * @brief  Describe mcpu always on pheriphrals.
 */
typedef enum {
    HAL_PMU_MAON_PERIP_M_GPIO0    = 0,
    HAL_PMU_MAON_PERIP_M_GPIO1    = 1,
    HAL_PMU_MAON_PERIP_B_GPIO0    = 2,
    HAL_PMU_MAON_PERIP_DSP_GPIO0  = 3,
} hal_pmu_maon_perips_type_t;

/**
  * @brief  Config mcpu peripheral.
  * @param  perip        The peripheral to config
  * @param  request      The request to operate pheripheral
  */
void hal_pmu_mcpu_perip_config(hal_pmu_mcpu_perips_type_t perip, hal_pmu_perip_request_type_t request);

/**
  * @brief  Config mcpu alwasy on peripheral.
  * @param  perip        The peripheral to config
  * @param  request      The request to operate pheripheral
  */
void hal_pmu_maon_perip_config(hal_pmu_maon_perips_type_t perip, hal_pmu_perip_request_type_t request);


/**
  * @}
  */
#endif
