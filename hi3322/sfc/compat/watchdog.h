/*
 * osal shim: watchdog.h — uapi_watchdog_kick mapped to the hi3322 board
 * watchdog feed (device/board/hihope/hi3322/liteos_m/board).
 */
#ifndef SFC_SHIM_WATCHDOG_H
#define SFC_SHIM_WATCHDOG_H

#include "watchdog.h"

static inline void uapi_watchdog_kick(void)
{
    hi3322_watchdog_feed();
}

#endif /* SFC_SHIM_WATCHDOG_H */
