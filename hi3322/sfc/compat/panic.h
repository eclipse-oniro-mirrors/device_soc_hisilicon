/*
 * osal shim: panic.h — the  SFC porting raises PANIC_XIP when the SFC
 * is touched from interrupt context; mapped to a fatal print + halt.
 */
#ifndef SFC_SHIM_PANIC_H
#define SFC_SHIM_PANIC_H

#include <stdint.h>

#include "compat_fix.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PANIC_XIP 0x58495000 /* 'XIP' tag */

void panic(uint32_t type, uint32_t line);

#ifdef __cplusplus
}
#endif

#endif /* SFC_SHIM_PANIC_H */
