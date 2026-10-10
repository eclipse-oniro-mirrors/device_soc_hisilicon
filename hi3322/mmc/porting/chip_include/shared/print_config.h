/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: print config interface for LiteOS
 *
 * Create:  2021-10-20
 */
#ifndef PRINT_CONFIG_H
#define PRINT_CONFIG_H

#if defined(HSO_SUPPORT)
#include "soc_diag_util.h"
#include "log_oam_logger.h"
#include "log_def.h"
#include "log_module_id.h"
#else
#include "debug_print.h"
#endif

#define print_liteos(fmt, args...)      PRINT(fmt, ##args)

#if defined(HSO_SUPPORT)
#define oam_none_log(fmt, args...)
#define oam_info_log(fmt, arg...)       uapi_diag_info_log(0, fmt, ##arg)
#define oam_warning_log(fmt, arg...)    uapi_diag_warning_log(0, fmt, ##arg)
#define oam_err_log(fmt, arg...)        uapi_diag_error_log(0, fmt, ##arg)
#define oam_trace_print(fmt, args...)
#else
#define oam_none_log(fmt, args...)      PRINT(fmt, ##args)
#define oam_info_log(fmt, args...)      PRINT(fmt, ##args)
#define oam_warning_log(fmt, args...)   PRINT(fmt, ##args)
#define oam_err_log(fmt, args...)       PRINT(fmt, ##args)
#define oam_trace_print(fmt, args...)   PRINT(fmt, ##args)
#endif /* defined(HSO_SUPPORT) */
#endif /* PRINT_CONFIG_H */
