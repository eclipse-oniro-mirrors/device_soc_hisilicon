/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description:  \n
 *
 * History: \n
 * 2024-05-31, Create file. \n
 */
#ifndef CPU_TRACE_PORTING_H
#define CPU_TRACE_PORTING_H

#include "chip_core_definition.h"
#include "cpu_trace_unified.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define MCPU_TRACE_CLOCK                 0x1FA
#define BCPU_TRACE_CLOCK                 0x1F

/**
 * @defgroup _
 * @ingroup
 * @{
 */
typedef enum {
    CPU_TRACE_TRACED_PORTING_BCPU = 0,  // trace BCPU
    CPU_TRACE_TRACED_PORTING_MCPU = 1,  // trace MCPU
    CPU_TRACE_CPU_CORE_NUMS,
} cpu_trace_cpu_num_t;

uint32_t cpu_trace_get_self_id(void);
void cpu_trace_port_enable(void);
void cpu_trace_get_trace_info_addr(hal_cpu_trace_info_t **info, uint8_t *length);
uint32_t *cpu_tace_get_pclr_base_addr(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif