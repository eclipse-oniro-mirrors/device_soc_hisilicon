/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: pmu ulp header file
 */

#ifndef PMU_ULP_H
#define PMU_ULP_H

#include <stdbool.h>
#include <stdint.h>
#include "osal_interrupt.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef enum {
    PMU_ULP_VBUS_POWER_UP = 0x51,                        // vbus 开机
    PMU_ULP_PWRKEY_POWER_UP = 0x52,                      // power key 开机
    PMU_ULP_PWRKEY_LONG_PRESS_POWER_UP = 0x53,           // power key 长按键重启重启开机
    PMU_ULP_GPIO_INT_POWER_UP = 0x54,                    // ulp gpio int 开机
    PMU_ULP_SYS_TICK_ALARM_POWER_UP = 0x55,              // systick alarm 开机
    PMU_ULP_DEFAULT_REASON = 0xFF,
} ulp_reboot_reason_t;

typedef enum {
    LONG_PRESS_8S = 0x0,
    LONG_PRESS_10S = 0x1,
    LONG_PRESS_12S = 0x2,
    LONG_PRESS_16S = 0x3,
    LONG_PRESS_MAX,
} power_key_long_press_time_t;

typedef enum {
    ULP_SYS_TICK_ALARM_IRQ,
    ULP_VBUS_INSERT_IRQ,
    ULP_VBUS_DIS_IRQ,
    ULP_PWR_KEY_PRESS_IRQ,
    ULP_PWR_KEY_DIS_IRQ,
    ULP_GPIO_IRQ,
    ULP_PMU_IRQ_MAX,
} ulp_pmu_irq_t;

/**
 * @if Eng
 * @brief The system enters the shipmode mode.
 * @param arg Reserved parameter.
 * @else
 * @brief 系统进shipmode
 * @param arg 保留参数.
 * @endif
 */
void uapi_sys_shipmode(uint32_t arg);

/**
 * @if Eng
 * @brief Get the insert state of vbus.
 * @retval true         vbus insert.
 * @retval false        vbus no insert.
 * @else
 * @brief 获取当前vbus插入状态
 * @retval true         Vbus插入。
 * @retval false        Vbus未插入。
 * @endif
 */
bool uapi_pmu_ulp_get_vbus_insert_state(void);

/**
 * @if Eng
 * @brief Get the press state of power key.
 * @retval true         Power key is pressed.
 * @retval false        Power key is not pressed.
 * @else
 * @brief 获取当前power key按键状态
 * @retval true         power key处于按键状态。
 * @retval false        power key处于松开状态。
 * @endif
 */
bool uapi_pmu_ulp_get_pwrkey_press_state(void);

/**
 * @if Eng
 * @brief  Set power key long press reboot time.
 * @param  [in]  time_s the reboot time. For details, see @ref power_key_long_press_time_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置power key 长按键重启时间。
 * @param  [in]  time_s 长按键重启时间， 参考@ref power_key_long_press_time_t。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pmu_ulp_set_pwrkey_reboot_time(uint8_t time_s);

/**
 * @if Eng
 * @brief Get the reboot reason of pmu ulp.
 * @retval reboot reason         For details, see @ref ulp_reboot_reason_t.
 * @else
 * @brief 获取PMU ULP的重启原因
 * @retval reboot reason         参考 @ref ulp_reboot_reason_t。
 * @endif
 */
uint8_t uapi_pmu_ulp_get_reboot_reason(void);

/**
 * @if Eng
 * @brief  Initializing the PMU ULP Driver.
 * @else
 * @brief  初始化 PMU ULP 驱动
 * @endif
 */
void uapi_ulp_pmu_init(void);

/**
 * @if Eng
 * @brief  Register pmu ulp irq handler.
 * @param  [in]  irq_id The pmu ulp irq ID. For details, see @ref ulp_pmu_irq_t.
 * @param  [in]  handler The irq handler.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  注册PMU ULP中断回调函数。
 * @param  [in]  irq_id 中断ID， 参考@ref ulp_pmu_irq_t。
 * @param  [in]  handler 中断回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pmu_ulp_register_irq(uint32_t irq_id, osal_irq_handler handler);

/**
 * @if Eng
 * @brief  Unregister pmu ulp irq handler.
 * @param  [in]  irq_id  The pmu ulp irq ID. For details, see @ref ulp_pmu_irq_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  去注册PMU ULP中断回调函数。
 * @param  [in]  irq_id 中断ID， 参考@ref ulp_pmu_irq_t。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pmu_ulp_unregister_irq(uint32_t irq_id);

/**
 * @if Eng
 * @brief  Set and Enable alarm.
 * @param  [in]  timeout_s The timeout time of alarm.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置并使能闹铃。
 * @param  [in]  timeout_s 闹铃超时时间。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_alarm_enable(uint32_t timeout_s);

/**
 * @if Eng
 * @brief  disable alarm.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  关闭闹钟。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_alarm_disable(void);

/**
 * @if Eng
 * @brief  Get alarm left count.
 * @retval uint64_t left count.
 * @else
 * @brief  获取闹铃剩余count值。
 * @retval uint64_t 剩余count值。
 * @endif
 */
uint64_t uapi_alarm_get_left_count(void);

/**
 * @if Eng
 * @brief  Set alarm count value.
 * @param  [in]  count alarm count value.
 * @else
 * @brief  设置闹铃count值。
 * @param  [in]  count 闹铃count值。
 * @endif
 */
void uapi_alarm_set_count(uint64_t count);

/**
 * @}
 */
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif