/**
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
 *
 * Description: Provides RISCV mpu register operation api \n
 *
 * History: \n
 * 2022-09-26， Create file. \n
 */

#ifndef HAL_PMP_RISCV31_REGS_OP_H
#define HAL_PMP_RISCV31_REGS_OP_H

#include <stdint.h>
#include "pmp_porting.h"
#include "hal_pmp_riscv31_regs_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_pmp_riscv31_regs_op PMP RISCV31 Regs Operation
 * @ingroup  drivers_hal_pmp
 * @{
 */

#define MEMXATTR_VECTOR         2

/**
 * @brief  Set address
 * @param  [in]  idx Region number
 * @param  [in]  addr Base address, First Address in NAPOT mode and End Address in TOR mode.
 */
void hal_pmp_riscv31_regs_set_pmpaddr(uint32_t idx, uint32_t addr);

/**
 * @if Eng
 * @brief  Set address
 * @param  [in]  idx Region number
 * @param  [in]  attr Read-write mode configuration.
 * @else
 * @brief  写入基地址寄存器
 * @param  [in]  idx 域编号
 * @param  [in]  attr 读写模式配置，预计写入memattr寄存器中的值
 * @endif
 */
void hal_pmp_riscv31_regs_set_memxattr(uint32_t idx, uint8_t attr);

/**
 * @if Eng
 * @brief  Set address
 * @param  [in]  idx Region number
 * @param  [in]  cfg PMP region configuration.
 * @else
 * @brief  写入基地址寄存器
 * @param  [in]  idx 域编号
 * @param  [in]  cfg pmp域配置
 * @endif
 */
void hal_pmp_riscv31_regs_set_pmpxcfg(uint32_t idx, hal_pmpx_config_t cfg);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif