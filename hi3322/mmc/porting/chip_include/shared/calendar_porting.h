/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides calendar port template \n
 *
 * History: \n
 * 2022-08-03， Create file. \n
 */
#ifndef CALENDAR_PORTING_H
#define CALENDAR_PORTING_H

#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_port_calendar Calendar
 * @ingroup  drivers_port
 * @{
 */

/**
 * @brief  Register hal funcs objects into hal_calendar module.
 */
errcode_t calendar_port_register_hal_funcs(void);

/**
 * @brief  Unregister hal funcs objects from hal_calendar module.
 */
errcode_t calendar_port_unregister_hal_funcs(void);

/**
 * @brief  Get hardware register address.
 */
uintptr_t hal_calendar_hw_base_addr_get(void);

errcode_t calendar_sync_before_shipmode(void);

uint8_t *calendar_get_first_poweron_flag(void);

void calendar_cail_update(void);

// 更新os时间
void update_os_time(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif