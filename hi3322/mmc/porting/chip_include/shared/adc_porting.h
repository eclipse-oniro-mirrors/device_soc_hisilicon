/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides adc port \n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */

#ifndef ADC_PORTING_H
#define ADC_PORTING_H

#include <stdint.h>
#include <stdbool.h>
#include "adc.h"
#include "osal_interrupt.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_port_adc ADC
 * @ingroup  drivers_port
 * @{
 */

#define ADC_LOCK_GET_ATTE 0xFFFFFFFF

/* auto scan fifo start addr */
#define HAL_ADC_SCAN_CH0_ADDR         0x52001000
#define HAL_ADC_SCAN_CH2_ADDR         0x52001200
#define HAL_ADC_SCAN_CH4_ADDR         0x52001400
#define HAL_ADC_SCAN_CH5_ADDR         0x52001600
#define HAL_ADC_SCAN_CH6_ADDR         0x52001800
#define HAL_ADC_SCAN_CH7_ADDR         0x52001A00
#define HAL_ADC_SCAN_CH8_ADDR         0x52001C00
#define HAL_ADC_SCAN_CH9_ADDR         0x52001E00
#define HAL_ADC_SCAN_CH10_ADDR        0x52002000
#define HAL_ADC_SCAN_CH11_ADDR        0x52002200

// FPGA手册中只有CH1 CH6 CH7，platfor_core.h中无对应的GPIO，此处先统一写0
#define HAL_ADC_CH0_PIN  0
#define HAL_ADC_CH1_PIN  0
#define HAL_ADC_CH2_PIN  0
#define HAL_ADC_CH3_PIN  0
#define HAL_ADC_CH4_PIN  0
#define HAL_ADC_CH5_PIN  0
#define HAL_ADC_CH6_PIN  0
#define HAL_ADC_CH7_PIN  0
#define HAL_ADC_CH8_PIN  0

/**
 * @brief  ADC channels definition.
 */
typedef enum {
    ADC_CHANNEL_0 = 0,  /* channle type: single without buffer */
    ADC_CHANNEL_1,      /* channle type: single without buffer */
    ADC_CHANNEL_2,      /* channle type: single without buffer */
    ADC_CHANNEL_3,      /* channle type: single without buffer */
    ADC_CHANNEL_4,      /* channle type: single without buffer */
    ADC_CHANNEL_5,      /* channle type: single without buffer */
    ADC_CHANNEL_6,      /* channle type: single without buffer */
    ADC_CHANNEL_7,      /* channle type: single without buffer */
    ADC_CHANNEL_MAX_NUM,
    ADC_CHANNEL_NONE = ADC_CHANNEL_MAX_NUM,
    ADC_CHANNEL_0_1,
    ADC_CHANNEL_2_3,
    ADC_CHANNEL_8
} adc_channel_t;

/**
 * @brief  ADC scan freq.
 */
typedef enum {
    HAL_ADC_SCAN_FREQ_2HZ,
    HAL_ADC_SCAN_FREQ_4HZ,
    HAL_ADC_SCAN_FREQ_8HZ,
    HAL_ADC_SCAN_FREQ_16HZ,
    HAL_ADC_SCAN_FREQ_32HZ,
    HAL_ADC_SCAN_FREQ_64HZ,
    HAL_ADC_SCAN_FREQ_128HZ,
    HAL_ADC_SCAN_FREQ_256HZ,
    HAL_ADC_SCAN_FREQ_MAX,
    HAL_ADC_SCAN_FREQ_NONE = HAL_ADC_SCAN_FREQ_MAX
} port_adc_scan_freq_t;

/**
 * @brief  Get the base address of a specified ADC.
 * @return The base address of specified ADC.
 */
uintptr_t adc_porting_base_addr_get(void);

/**
 * @brief  Register hal funcs objects into hal_adc module.
 */
void adc_port_register_hal_funcs(void);

/**
 * @brief  Unregister hal funcs objects from hal_adc module.
 */
void adc_port_unregister_hal_funcs(void);

/**
 * @brief  Set the divider number of the peripheral device clock.
 * @param [in] clock The clock which is used for adc sample, adc source clock is 2MHz.
 */
void adc_port_init_clock(adc_clock_t clock);

/**
 * @brief  Set the divider number of the peripheral device clock.
 * @param [in] on Enable or disable.
 */
void adc_port_clock_enable(bool on);

/**
 * @brief  Register the interrupt of adc.
 */
void adc_port_register_irq(void);

/**
 * @brief  Unregister the interrupt of adc.
 */
void adc_port_unregister_irq(void);

/**
 * @brief  Power on or power off the peripheral device.
 * @param [in] on Power on or Power off.
 */
void adc_port_power_on(bool on);

/**
 * @brief  ADC calibratio.
 */
void adc_port_calibration(void);

/**
 * @brief  ADC disadle channel pull.
 * @param [in] channel The adc channel.
 */
void adc_port_pull_disable(adc_channel_t channel);

/**
 * @brief  ADC set scan discard number and average number.
 * @param [in] channel The adc channel.
 */
void adc_port_set_scan_discard_and_average_num(adc_channel_t channel);

/**
 * @brief  Lock of the interrupt.
 * @return The irq status.
 */
inline uint32_t adc_irq_lock(void)
{
    return osal_irq_lock();
}

/**
 * @brief  Unlock of the interrupt.
 * @param [in] irq_sts The irq status to restore.
 */
inline void adc_irq_unlock(uint32_t irq_sts)
{
    osal_irq_restore(irq_sts);
}

/**
 * @brief  Stick transfer voltage.
 * @param [in] stick The adc channel manual sample value.
 * @retval transfered voltage.
 */
float adc_port_stick_transfer_voltage(uint16_t stick);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
