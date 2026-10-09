/* ---------------------------------------------------------------------------
 * fmc compat debug print for the fbb fmc stack on morpheus liteos_m
 * (mirrors sfc/compat/debug_print.h with an NAND| log prefix; fmc/compat
 * precedes sfc/compat in the include path, so this file wins for the fmc
 * build).
 * --------------------------------------------------------------------------- */
#ifndef FMC_SHIM_DEBUG_PRINT_H
#define FMC_SHIM_DEBUG_PRINT_H

#include "compat_fix.h"

#ifndef NEWLINE
#define NEWLINE "\r\n"
#endif

#define PRINT(fmt, arg...) dprintf("NAND|" fmt, ##arg)
#define print_init()

#endif /* FMC_SHIM_DEBUG_PRINT_H */
