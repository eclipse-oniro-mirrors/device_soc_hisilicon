/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Provides HAL SFC \n
 *
 * History: \n
 * 2024-10-01, Create file. \n
 */
#include "common_def.h"
#include "hal_sfc.h"

uintptr_t g_sfc_global_conf_regs[SFC_ID_MAX];
uintptr_t g_sfc_bus_regs[SFC_ID_MAX];
uintptr_t g_sfc_bus_dma_regs[SFC_ID_MAX];
uintptr_t g_sfc_cmd_regs[SFC_ID_MAX];
uintptr_t g_sfc_cmd_databuf[SFC_ID_MAX];
#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
uintptr_t g_sfc_continue_read[SFC_ID_MAX];
#endif

errcode_t hal_sfc_regs_init(uint8_t id)
{
    g_sfc_global_conf_regs[id] = sfc_port_get_sfc_global_conf_base_addr(id);
    g_sfc_bus_regs[id] = sfc_port_get_sfc_bus_regs_base_addr(id);
    g_sfc_bus_dma_regs[id] = sfc_port_get_sfc_bus_dma_regs_base_addr(id);
    g_sfc_cmd_regs[id] = sfc_port_get_sfc_cmd_regs_base_addr(id);
    g_sfc_cmd_databuf[id] = sfc_port_get_sfc_cmd_databuf_base_addr(id);
#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
    g_sfc_continue_read[id] = sfc_port_get_sfc_continue_read_base_addr(id);
#endif
    if ((g_sfc_global_conf_regs[id] == NULL) || (g_sfc_bus_regs[id] == NULL) ||
        (g_sfc_bus_dma_regs[id] == NULL) || (g_sfc_cmd_regs[id] == NULL) ||
#ifdef CONFIG_SUPPORT_SFC_CONTINUE_READ
        (g_sfc_cmd_databuf[id] == NULL) || (g_sfc_continue_read[id] == NULL)) {
#else
        (g_sfc_cmd_databuf[id] == NULL)) {
#endif
        return ERRCODE_SFC_REG_ADDR_INVALID;
    }

    return ERRCODE_SUCC;
}

void hal_sfc_regs_deinit(uint8_t id)
{
    g_sfc_global_conf_regs[id] = NULL;
    g_sfc_bus_regs[id] = NULL;
    g_sfc_bus_dma_regs[id] = NULL;
    g_sfc_cmd_regs[id] = NULL;
    g_sfc_cmd_databuf[id] = NULL;
}