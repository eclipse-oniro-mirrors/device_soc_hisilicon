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
 */

#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* Physical memory address base and size */
#ifdef LOSCFG_TEE_ENABLE
#define DDR_MEM_ADDR            0x81000000
#define DDR_MEM_SIZE            0x1f000000
#else
#define DDR_MEM_ADDR            0x80000000
#define DDR_MEM_SIZE            0x20000000
#endif

/* Peripheral register address base and size */
#define PERIPH_PMM_BASE         0x10000000
#define PERIPH_PMM_SIZE         0x10000000

#ifdef LOSCFG_TEE_ENABLE
#define KERNEL_VADDR_BASE       0x41000000
#else
#define KERNEL_VADDR_BASE       0x40000000
#endif
#define KERNEL_VADDR_SIZE       DDR_MEM_SIZE

#define SYS_MEM_BASE            DDR_MEM_ADDR
#define SYS_MEM_SIZE_DEFAULT    0x07f00000
#define SYS_MEM_END             (SYS_MEM_BASE + SYS_MEM_SIZE_DEFAULT)

#define EXC_INTERACT_MEM_SIZE        0x100000

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
