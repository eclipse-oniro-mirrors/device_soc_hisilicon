/*
 * ---------------------------------------------------------------------------
 * osal shim implementation — see soc_osal.h.
 * ---------------------------------------------------------------------------
 */
#include "soc_osal.h"
#include "panic.h"
#include "tcxo.h"
#include "watchdog.h"

#include "los_memory.h"

int osal_mutex_init(osal_mutex *mutex)
{
    if (mutex == NULL) {
        return -1;
    }
    return (LOS_MuxCreate(&mutex->handle) == LOS_OK) ? OSAL_SUCCESS : -1;
}

int osal_mutex_lock(osal_mutex *mutex)
{
    if (mutex == NULL) {
        return -1;
    }
    return (LOS_MuxPend(mutex->handle, LOS_WAIT_FOREVER) == LOS_OK) ? OSAL_SUCCESS : -1;
}

int osal_mutex_unlock(osal_mutex *mutex)
{
    if (mutex == NULL) {
        return -1;
    }
    return (LOS_MuxPost(mutex->handle) == LOS_OK) ? OSAL_SUCCESS : -1;
}

uint32_t osal_irq_lock(void)
{
    return LOS_IntLock();
}

void osal_irq_restore(uint32_t irq_status)
{
    LOS_IntRestore((UINT32)irq_status);
}

/*
 * hi3322 LinxM rv32imc core has no data cache; kept for API parity with
 * the  SFC driver which flushes before bus-master flash operations.
 */
void osal_dcache_flush_all(void)
{
}

void panic(uint32_t type, uint32_t line)
{
    dprintf("SFC|panic type=0x%x line=%u — halted\n", type, line);
    for (;;) {
        ;
    }
}

void uapi_tcxo_delay_us(uint64_t us)
{
    LOS_UDelay(us);
}

void uapi_tcxo_delay_ms(uint64_t ms)
{
    LOS_UDelay(ms * 1000U);
}

/*
 * free-running counter; only used by the SFC port as a random-delay seed
 */
uint64_t uapi_tcxo_get_count(void)
{
    return LOS_TickCountGet();
}
