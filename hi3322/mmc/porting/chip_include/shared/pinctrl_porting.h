/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides pinctrl port \n
 *
 * History: \n
 * 2022-08-25， Create file. \n
 */
#ifndef PINCTRL_PORTING_H
#define PINCTRL_PORTING_H

#include <stdint.h>
#include <stdbool.h>
#include "securec.h"
#include "platform_core.h"
#include "chip_io.h"
#include "gpio_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_port_pinctrl Pinctrl
 * @ingroup  drivers_port
 * @{
 */

/**
 * @brief  Definition of mode-multiplexing.
 */
typedef enum {
    PIN_MODE_0 = 0,
    PIN_MODE_1 = 1,
    PIN_MODE_2 = 2,
    PIN_MODE_3 = 3,
    PIN_MODE_MAX
} pin_mode_t;

/**
 * @brief  Definition of drive-strength.
 */
typedef enum {
    PIN_DS_0  = 0,
    PIN_DS_1  = 1,
    PIN_DS_2  = 2,
    PIN_DS_3  = 3,
    PIN_DS_4  = 3,
    PIN_DS_5  = 3,
    PIN_DS_6  = 3,
    PIN_DS_7  = 3,
    PIN_DS_8  = 3,
    PIN_DS_9  = 3,
    PIN_DS_10 = 3,
    PIN_DS_11 = 3,
    PIN_DS_12 = 3,
    PIN_DS_13 = 3,
    PIN_DS_14 = 3,
    PIN_DS_15 = 3,
    PIN_DS_MAX
} pin_drive_strength_t;

/**
 * @brief  Definition of pull-up/pull-down.
 */
typedef enum {
    PIN_PULL_NONE = 0,
    PIN_PULL_DOWN = 1,
    PIN_PULL_UP   = 2,
    PIN_PULL_MAX
} pin_pull_t;

/**
 * @brief  Definition of input enable.
 */
typedef enum {
    PIN_IE_DISABLE = 0,
    PIN_IE_ENABLE = 1,
    PIN_IE_MAX
} pin_input_enable_t;

/**
 * @brief  Definition of schmitt-trigger.
 */
typedef enum {
    PIN_ST_DISABLE = 0,
    PIN_ST_ENABLE = 1,
    PIN_ST_MAX
} pin_schmitt_trigger_t;

typedef enum {
    /* pinmux mode 0 funciton */
    HAL_PIO_FUNC_GPIO = 0, // Default NON ULP GPIO
    HAL_PIO_FUNC_OPI = 0,

    /* pinmux mode 1 funciton */
    HAL_PIO_FUNC_QSPI0 = 1,
    HAL_PIO_FUNC_QSPI2 = 1,
    HAL_PIO_FUNC_EMMC = 1,
    HAL_PIO_FUNC_SDIO = 1,
    HAL_PIO_FUNC_BT_WIFI_SW = 1,
    HAL_PIO_FUNC_BT_ACT = 1,
    HAL_PIO_FUNC_BTS_SAMPLE = 1,
    HAL_PIO_FUNC_BT_FREQ = 1,
    HAL_PIO_FUNC_BT_STATUS = 1,
    HAL_PIO_FUNC_BT_LNA_EN = 1,
    HAL_PIO_FUNC_WLAN_ACT = 1,
    HAL_PIO_FUNC_UART3_M1 = 1,
    HAL_PIO_FUNC_CLKOUT0_M1 = 1,
    HAL_PIO_FUNC_CLKOUT1_M1 = 1,
    HAL_PIO_FUNC_UART4_M1 = 1,
    HAL_PIO_FUNC_PWM_M1 = 1,
    HAL_PIO_FUNC_GLP_SYNC_P = 1,
    HAL_PIO_FUNC_USB_VBUS = 1,
    HAL_PIO_FUNC_DSI_TE = 1,
    HAL_PIO_FUNC_CLKIN_M1 = 1,
    HAL_PIO_FUNC_I2S0 = 1,
    HAL_PIO_FUNC_I2S1 = 1,
    HAL_PIO_FUNC_PDM = 1,
    HAL_PIO_FUNC_CAN = 1,
    HAL_PIO_FUNC_UART0_M1 = 1,
    HAL_PIO_FUNC_I2C0_M1 = 1,
    HAL_PIO_FUNC_I2C1_M1 = 1,
    HAL_PIO_FUNC_I2C2_M1 = 1,
    HAL_PIO_FUNC_I2C3_M1 = 1,
    HAL_PIO_FUNC_SPI0_M1 = 1,
    HAL_PIO_FUNC_UART2_M1 = 1,
    HAL_PIO_FUNC_UART1_M1 = 1,
    HAL_PIO_FUNC_SPI1_M1 = 1,
    HAL_PIO_FUNC_SWD = 1,

    /* pinmux mode 2 funciton */
    HAL_PIO_FUNC_QSPI1_M2 = 2,
    HAL_PIO_FUNC_I2C0_M2 = 2,
    HAL_PIO_FUNC_I2C1_M2 = 2,
    HAL_PIO_FUNC_I2C2_M2 = 2,
    HAL_PIO_FUNC_I2C3_M2 = 2,
    HAL_PIO_FUNC_CLKIN_M2 = 2,
    HAL_PIO_FUNC_UART3_TXD_M2 = 2,
    HAL_PIO_FUNC_UART3_RXD_M2 = 2,
    HAL_PIO_FUNC_PWM_M2 = 2,
    HAL_PIO_FUNC_CLKOUT0_M2 = 2,
    HAL_PIO_FUNC_CLKOUT1_M2 = 2,
    HAL_PIO_FUNC_FEM_CTRL = 2,
    HAL_PIO_FUNC_UART1_M2 = 2,
    HAL_PIO_FUNC_SPI1_M2 = 2,
    HAL_PIO_FUNC_UART0_M2 = 2,

    /* pinmux mode 3 funciton */
    HAL_PIO_FUNC_DIAG = 3, // DIAG[0] ~ DIAG[15]
    HAL_PIO_FUNC_USB_OVR_CUR = 3,
    HAL_PIO_FUNC_PWM_M3 = 3,
    HAL_PIO_FUNC_UART1_M3 = 3,
    HAL_PIO_FUNC_VICAP = 3,

    HAL_PIO_FUNC_MAX = 4,
    // input high resistance. Need config as HAL_PIO_FUNC_BT_GPIO and GPIO input mode
    HAL_PIO_FUNC_DEFAULT_HIGH_Z = 0xf,
} hal_pio_func_t;

typedef enum {
    HAL_PIO_PULL_NONE,  //!< No pull down or pull up enabled.
    HAL_PIO_PULL_DOWN,  //!< Pull down enabled for this pin.
    HAL_PIO_PULL_UP,    //!< Pull up enabled for this pin.
    HAL_PIO_PULL_MAX,
    HAL_PIO_PULL_DEFAULT = HAL_PIO_PULL_MAX,
} hal_pio_pull_t;

typedef enum {
    HAL_PIO_DRIVE_0 =  0,   //!< lowest pio current dirve strength.
    HAL_PIO_DRIVE_1 =  1,
    HAL_PIO_DRIVE_2 =  2,
    HAL_PIO_DRIVE_3 =  3,
    HAL_PIO_DRIVE_MAX,
    HAL_PIO_DRIVE_DEFAULT = HAL_PIO_DRIVE_MAX,
} hal_pio_drive_t;

typedef enum {
    HAL_PIO_IE_DISABLE = 0,
    HAL_PIO_IE_ENABLE  = 1,
    HAL_PIO_IE_MAX,
    HAL_PIO_IE_DEFAULT = HAL_PIO_IE_MAX,
} hal_pio_ie_t;

typedef struct {
    hal_pio_func_t func;
    hal_pio_drive_t drive;
    hal_pio_pull_t pull;
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    hal_pio_ie_t ie;
#endif
} hal_pio_config_t;

/**
 * @brief  Check whether the mode configured for the pin is valid.
 * @param  [in]  pin  The index of pins. see @ref pin_t
 * @param  [in]  mode The Multiplexing mode. see @ref pin_mode_t
 * @return The value 'true' indicates that the mode is valid and the value 'false' indicates that the mode is invalid.
 */
bool pin_check_mode_is_valid(pin_t pin, pin_mode_t mode);

/**
 * @brief  Register hal funcs objects into hal_pinctrl module.
 */
void pin_port_register_hal_funcs(void);

/**
 * @brief  Unregister hal funcs objects from hal_pinctrl module.
 */
void pin_port_unregister_hal_funcs(void);

void get_pio_func_config(size_t *pin_num, hal_pio_config_t **pin_func_array);

/**
 * @}
 */
#define HAL_PIO_FUNC_INVALID        HAL_PIO_FUNC_MAX

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
