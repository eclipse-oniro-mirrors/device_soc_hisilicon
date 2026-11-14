/*
 * Copyright (c) 2025 HiSilicon (Shanghai) Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see
 * <https://www.gnu.org/licenses/>.
 */
#ifndef CRYPTO_LOG_UTILS_H
#define CRYPTO_LOG_UTILS_H
#include "ot_osal.h"
/*
 * crypto log.
 * redefine in crypto_osal_lib.h if need.
 */
#define crypto_print osal_printk
#ifndef crypto_print
#define crypto_print(...)
#endif

#define crypto_log_fmt(LOG_LEVEL_LABEL, fmt, ...)   \
    crypto_print("[%s:%d]" LOG_LEVEL_LABEL ": " fmt "\n", __func__, __LINE__, ##__VA_ARGS__)

/* Log Level, default is 0. */
#ifndef CRYPTO_LOG_LEVEL
#define CRYPTO_LOG_LEVEL 0
#endif

#if defined(CRYPTO_LOG_LEVEL) && (CRYPTO_LOG_LEVEL == 0)
#define crypto_log_err(fmt, ...)        crypto_log_fmt("ERROR", fmt, ##__VA_ARGS__)
#define crypto_log_warn(fmt, ...)
#define crypto_log_notice(fmt, ...)
#define crypto_log_dbg(fmt, ...)
#define crypto_log_trace(fmt, ...)
#elif defined(CRYPTO_LOG_LEVEL) && (CRYPTO_LOG_LEVEL == 1)
#define crypto_log_err(fmt, ...)        crypto_log_fmt("ERROR", fmt, ##__VA_ARGS__)
#define crypto_log_warn(fmt, ...)       crypto_log_fmt("WARN:", fmt, ##__VA_ARGS__)
#define crypto_log_notice(fmt, ...)
#define crypto_log_dbg(fmt, ...)
#define crypto_log_trace(fmt, ...)
#elif defined(CRYPTO_LOG_LEVEL) && (CRYPTO_LOG_LEVEL == 2)
#define crypto_log_err(fmt, ...)        crypto_log_fmt("ERROR", fmt, ##__VA_ARGS__)
#define crypto_log_warn(fmt, ...)       crypto_log_fmt("WARN:", fmt, ##__VA_ARGS__)
#define crypto_log_notice(fmt, ...)     crypto_log_fmt("NOTICE", fmt, ##__VA_ARGS__)
#define crypto_log_dbg(fmt, ...)
#define crypto_log_trace(fmt, ...)
#elif defined(CRYPTO_LOG_LEVEL) && (CRYPTO_LOG_LEVEL == 3)
#define crypto_log_err(fmt, ...)        crypto_log_fmt("ERROR", fmt, ##__VA_ARGS__)
#define crypto_log_warn(fmt, ...)       crypto_log_fmt("WARN:", fmt, ##__VA_ARGS__)
#define crypto_log_notice(fmt, ...)     crypto_log_fmt("NOTICE", fmt, ##__VA_ARGS__)
#define crypto_log_dbg(fmt, ...)        crypto_log_fmt("DBG", fmt, ##__VA_ARGS__)
#define crypto_log_trace(fmt, ...)
#elif defined(CRYPTO_LOG_LEVEL) && (CRYPTO_LOG_LEVEL == 4)
#define crypto_log_err(fmt, ...)        crypto_log_fmt("ERROR", fmt, ##__VA_ARGS__)
#define crypto_log_warn(fmt, ...)       crypto_log_fmt("WARN:", fmt, ##__VA_ARGS__)
#define crypto_log_notice(fmt, ...)     crypto_log_fmt("NOTICE", fmt, ##__VA_ARGS__)
#define crypto_log_dbg(fmt, ...)        crypto_log_fmt("DBG", fmt, ##__VA_ARGS__)
#define crypto_log_trace(fmt, ...)      crypto_log_fmt("TRACE", fmt, ##__VA_ARGS__)
#endif

/* PKE Log trace enable. */
#if defined(CONFIG_PKE_TRACE_ENABLE)
#define crypto_pke_print            crypto_print
#define crypto_pke_dump_data        crypto_dump_data
#else
#define crypto_pke_print(...)
#define crypto_pke_dump_data(...)
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* crypto dump functions. */
void crypto_dump_data(const char *name, const uint8_t *data, uint32_t data_len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CRYPTO_LOG_UTILS_H */
