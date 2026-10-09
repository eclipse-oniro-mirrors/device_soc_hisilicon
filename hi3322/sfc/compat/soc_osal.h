/*
 * ---------------------------------------------------------------------------
 * osal shim for the  SFC stack (soc_osal.h aggregate).
 * Backed by liteos_m: LOS_Mux* for mutexes, LOS_IntLock/Restore for the
 * IRQ critical section. rv32imc (hi3322 LinxM core) has no data cache, so
 * osal_dcache_flush_all is a no-op kept for API parity.
 * ---------------------------------------------------------------------------
 */
#ifndef SFC_SHIM_SOC_OSAL_H
#define SFC_SHIM_SOC_OSAL_H

#include <stdint.h>
#include <stdbool.h>

#include "los_mux.h"
#include "los_interrupt.h"
#include "los_task.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OSAL_SUCCESS 0

typedef struct {
    UINT32 handle;
} osal_mutex;

int osal_mutex_init(osal_mutex *mutex);
int osal_mutex_lock(osal_mutex *mutex);
int osal_mutex_unlock(osal_mutex *mutex);

uint32_t osal_irq_lock(void);
void osal_irq_restore(uint32_t irq_status);

/* hi3322 LinxM core: no data cache — see NOR_PORTING_PLAN.md 4.2 */
void osal_dcache_flush_all(void);

#ifdef __cplusplus
}
#endif

#endif /* SFC_SHIM_SOC_OSAL_H */
