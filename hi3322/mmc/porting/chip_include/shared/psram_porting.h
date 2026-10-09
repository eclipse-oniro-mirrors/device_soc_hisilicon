/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved. \n
 *
 * Description: psram_porting header file \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-06-01, Create file. \n
 */
#ifndef PSRAM_PORTING_H
#define PSRAM_PORTING_H

#include <stdint.h>
#include <stdbool.h>
#include <errcode.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup _
 * @ingroup
 * @{
 */
#define PSRAM_INIT_DELAY_US             150
#define PSRAM_MR_DELAY_US               2
#define PSRAM_MR_DELAY_MS               1
#define PSRAM_MR_VAL                    0x42002012
#define OFFSET_8                        8
#define OFFSET_16                       16
#define OFFSET_24                       24
#define PSRAM_VENDOR_ID_MASK            0x1F

typedef enum {
    PSRAM_ID_0,
    PSRAM_ID_1,
    PSRAM_ID_MAX = PSRAM_ID_1,
} psram_id_t;

uintptr_t psram_port_get_phy_addr(psram_id_t id);
uintptr_t psram_port_get_dmc_addr(psram_id_t id);
uintptr_t psram_port_get_axi_addr(psram_id_t id);
uintptr_t psram_port_get_dmc_sfg_sfc_addr(psram_id_t id);
uintptr_t psram_port_get_dmc_debug_addr(psram_id_t id);
uintptr_t psram_port_get_axi_rgn_addr(psram_id_t id);
uintptr_t psram_port_get_axi_qos_addr(psram_id_t id);
void psram_port_power_on(void);
void psram_port_freq_flag_set(void);
void psram_controller_reinit(void);
errcode_t uapi_psram_exec_cmd_bp(uintptr_t arg);
errcode_t uapi_psram_close_cmd_bp(uintptr_t arg);
uint32_t uapi_psram_read_vendor_id(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif