/*
 * ---------------------------------------------------------------------------
 * osal-shim debug print for the SFC stack on morpheus liteos_m.
 * Replaces the middleware debug_print.h (whose PRINT macro depends on
 * the CORE/BUILD_* build-system macros); routes everything to the
 * kernel dprintf (see the eMMC-porting compat_fix.h).
 * ---------------------------------------------------------------------------
 */
#ifndef SFC_SHIM_DEBUG_PRINT_H
#define SFC_SHIM_DEBUG_PRINT_H

#include "compat_fix.h"

#ifndef NEWLINE
#define NEWLINE "\r\n"
#endif

#define PRINT(fmt, arg...) dprintf("SFC|" fmt, ##arg)
#define print_init()

#endif /* SFC_SHIM_DEBUG_PRINT_H */
