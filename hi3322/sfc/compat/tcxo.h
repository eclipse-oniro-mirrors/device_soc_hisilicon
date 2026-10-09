/*
 * osal shim: tcxo.h — delay/count primitives of the  TCXO driver,
 * mapped to liteos_m tick services.
 */
#ifndef SFC_SHIM_TCXO_H
#define SFC_SHIM_TCXO_H

#include <stdint.h>

#include "los_tick.h"

#ifdef __cplusplus
extern "C" {
#endif

void uapi_tcxo_delay_us(uint64_t us);
void uapi_tcxo_delay_ms(uint64_t ms);
uint64_t uapi_tcxo_get_count(void);

#ifdef __cplusplus
}
#endif

#endif /* SFC_SHIM_TCXO_H */
