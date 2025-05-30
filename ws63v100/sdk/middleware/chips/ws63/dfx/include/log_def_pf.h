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
 * Description:  common logging producer interface - need to change name of log.h in all the protocol core files
 */

#ifndef _LOG_DEF_PF_H_
#define _LOG_DEF_PF_H_

typedef enum {
    // Platform 0-1024
    SECURITY_MAIN_C = 0,
    APP_OS_INIT_C,
    THREAD_INIT_C,
    SEC_OS_INIT_C,
    LOG_OAM_MSG_C,
    LOG_OML_EXCEPTION_C,
    BACKTRACE_C_NOT_USED_C,
    PANIC_C,
    RUNTIME_MONITOR_C,
    LOS_TASK_C,
    LOG_C,
    LOW_POWER_CONTROL_C,
    CPU_UTILS_C,
    HAL_PVSENSOR_C,
    PMU_CMU_C,
    GPIO_C,
    IR_C,
    CPU_TRACE_C,
    XIP_C,
    HAL_LPC_CORE_C,
    MPU_C,
    HAL_CPU_HIFI_C,
    IPC_C,
    FIRMWARE_VERSION_C,
    HAL_ADC_C,
    PATCH_C,
    PMU_INTERRUPT_C,
    HAL_CPU_CORE_C,
    COMMU_INTERFACE_C,
    EPMU_COUL_DRV_C,
    EPMU_COUL_CORE_C,
    EPMU_C,
    GNSS_OS_INIT_C,
    HAL_IPC_IRQ_C,
    PAL_LPM_C,
    PRESERVE_C,
    LITEOS_INFO_C,
    CLOCKS_CORE_C,
    SYSTEM_INFO_C,
    CPU_LOAD_C,
    PMU_C,
    LOS_STATUS_C,
    GNSS_PMU_C,
    PATCH_RISCV_C,
    PM_CONTROL_C,
    BLE_HID_SERVER_C,
    MAIN_C,
    DIAG_DFX_C,
    LOG_OAM_REG_QUERY_C,
    LOG_BUFFER_READER_C,
    LOG_UART_C,
    UPG_AB_C,
    UPG_ALLOC_C,
    UPG_COMMON_C,
    UPG_VERIFY_C,
    UPG_LZMADEC_C,
    UPG_PATCH_C,
    UPG_PROCESS_C,
    UPG_RESOURCE_C,
    UPG_UPGRADE_C,
    UPG_STORAGE_C,
    UPG_COMMON_PORTING_C,
    UPG_BACKUP_C,
    PF_FILE_ID_MAX = 1024,
} log_file_list_enum_pf_app_t;
#endif
