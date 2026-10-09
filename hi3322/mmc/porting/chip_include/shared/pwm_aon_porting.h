/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides pwm aon port \n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */

#ifndef PWM_AON_PORTING_H
#define PWM_AON_PORTING_H

#include <stdint.h>
#include <stdbool.h>
#include "pwm_aon.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_port_pwm PWM_AON
 * @ingroup  drivers_port
 * @{
 */

/**
 * @brief  PWM_AON v151 channel ID.
 */
typedef enum {
    PWM_AON_0,                  /* < PWM_AON Peripheral 0. */
    PWM_AON_1,                  /* < PWM_AON Peripheral 1. */
    PWM_AON_2,                  /* < PWM_AON Peripheral 2. */
    PWM_AON_3,                  /* < PWM_AON Peripheral 3. */
    PWM_AON_4,                  /* < PWM_AON Peripheral 4. */
    PWM_AON_5,                  /* < PWM_AON Peripheral 5. */
    PWM_AON_NONE = CONFIG_PWM_AON_CHANNEL_NUM
} pwm_aon_channel_t;

/**
 * @brief  PWM_AON v150 group ID.
 */
typedef enum {
    PWM_AON_GROUP_0,
    PWM_AON_GROUP_1,
    PWM_AON_GROUP_2,
    PWM_AON_GROUP_3,
    PWM_AON_GROUP_4,
    PWM_AON_GROUP_5,
    PWM_AON_GROUP_6,
    PWM_AON_GROUP_7,
    PWM_AON_GROUP_8,
    PWM_AON_GROUP_9,
    PWM_AON_GROUP_10,
    PWM_AON_GROUP_11,
    PWM_AON_GROUP_12,
    PWM_AON_GROUP_13,
    PWM_AON_GROUP_14,
    PWM_AON_GROUP_15
} pwm_aon_v151_group_t;

/**
 * @brief  Get the base address of a specified PWM_AON.
 * @return The base address of specified PWM_AON.
 */
uintptr_t pwm_aon_porting_base_addr_get(void);

#define PWM_AON_MAX_NUMBER      6  /* < Max number of PWM_AON available */

/**
 * @brief  Register hal funcs objects into hal_pwm module.
 */
void pwm_aon_port_register_hal_funcs(void);

/**
 * @brief  Unregister hal funcs objects from hal_pwm module.
 */
void pwm_aon_port_unregister_hal_funcs(void);

/**
 * @brief  Register the interrupt of pwm_aon.
 * @param [in] channel PWM_AON device.
 */
void pwm_aon_port_register_irq(pwm_aon_channel_t channel);

/**
 * @brief  Unregister the interrupt of pwm_aon.
 * @param [in] channel PWM_AON device.
 */
void pwm_aon_port_unregister_irq(pwm_aon_channel_t channel);

/**
 * @brief  Lock of pwm_aon interrupt.
 * @param [in] channel The pwm_aon channel.
 */
void pwm_aon_irq_lock(uint8_t channel);

/**
 * @brief  Unlock of pwm_aon interrupt.
 * @param [in] channel The pwm_aon channel.
 */
void pwm_aon_irq_unlock(uint8_t channel);

/**
 * @brief  Set the divider number of the peripheral device clock.
 * @param [in] on Enable or disable.
 */
void pwm_aon_port_clock_enable(bool on);

/**
 * @brief  Get pwm_aon clock value.
 * @param  [in]  channel PWM_AON device.
 */
uint32_t pwm_aon_port_get_clock_value(pwm_aon_channel_t channel);

errcode_t pwm_aon_port_param_check(const pwm_aon_config_t *cfg);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif