/*
 * Copyright (c) 2020 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * Description: hal tx.
 */

#ifndef HAL_TX_ROM_H
#define HAL_TX_ROM_H

#include "osal_types.h"
#include "hal_device_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define RPT_TX_QUEUE_AL_EMPTY_INTR_BK_MASK 0x20      // bit5
#define RPT_TX_QUEUE_AL_EMPTY_INTR_BE_MASK 0x800     // bit11
#define RPT_TX_QUEUE_AL_EMPTY_INTR_VI_MASK 0x1000000 // bit24
#define RPT_TX_QUEUE_AL_EMPTY_INTR_VO_MASK 0x2000000 // bit25
#define TIMESTAMP_FROM_REG_UNIT_CHANGE     4 // backoff time和tx done time时间戳单位为16us, 左移四位转换

osal_void hh503_irq_tx_queue_al_empty_isr(hal_device_stru *device, osal_u8 data);
osal_void hh503_irq_tx_complete_isr(hal_device_stru *device, osal_u8 data);
osal_void hal_log_txq_overrun(hal_to_dmac_device_stru *hal_device, osal_u32 param);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of HAL_TX_ROM_H */
