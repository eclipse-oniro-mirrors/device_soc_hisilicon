/* ---------------------------------------------------------------------------
 * dpal shim — minimal dpal framework implementation for morpheus liteos_m
 * (NAND_PORTING_PLAN.md v3 A2). The fbb fmc stack only touches a narrow
 * subset of the dpal API:
 *
 *   dpal_readb/w            dpal.h inline (register deref, no MMU) — no shim
 *   dpal_mux_create/pend/post  -> LOS_MuxCreate/Pend/Post
 *   dpal_platform_*         synchronous driver-model (device registered
 *                           before driver; probe runs on driver register)
 *   osal_kzalloc / osal_kmalloc_align / osal_kfree (dpal.h externs)
 *                           -> liteos system heap (m_aucSysMem0)
 *   mtd_list + ffs          fbb compact mtd_list.c merged here
 * ---------------------------------------------------------------------------
 */
#include <errno.h>
#include <stddef.h>
#include <string.h>

#include "dpal.h"
#include "dpal_driverbase.h"
#include "linux/mtd/mtd_list.h"

#include "los_memory.h"
#include "los_mux.h"
#include "los_task.h"
#include "los_tick.h"

/* ------------------------------------------------------------------ */
/* memory                                                             */
/* ------------------------------------------------------------------ */

void *dpal_malloc(size_t size)
{
    return LOS_MemAlloc(m_aucSysMem0, (UINT32)size);
}

void dpal_free(void *ptr)
{
    if (ptr != NULL) {
        (VOID)LOS_MemFree(m_aucSysMem0, ptr);
    }
}

void *dpal_zalloc(size_t size)
{
    void *p = dpal_malloc(size);
    if (p != NULL) {
        (void)memset_s(p, size, 0, size);
    }
    return p;
}

void *dpal_memalign(size_t boundary, size_t size)
{
    /* rv32imc has no cache; alignment is only a soft requirement here */
    (void)boundary;
    return dpal_malloc(size);
}

/* fbb soc_osal API used directly by hifmc100.c (declared in dpal.h) */
void *osal_kzalloc(size_t size, uint32_t flags)
{
    (void)flags;
    return dpal_zalloc(size);
}

void *osal_kmalloc_align(size_t size, uint32_t flags, uintptr_t align)
{
    (void)flags;
    return LOS_MemAllocAlign(m_aucSysMem0, (UINT32)size, (UINT32)align);
}

void osal_kfree(void *ptr)
{
    dpal_free(ptr);
}

/* ------------------------------------------------------------------ */
/* mutex                                                              */
/* ------------------------------------------------------------------ */

uint32_t dpal_mux_create(uint32_t *mux_handle)
{
    if (mux_handle == NULL) {
        return DPAL_NOK;
    }
    return (LOS_MuxCreate(mux_handle) == LOS_OK) ? DPAL_OK : DPAL_NOK;
}

uint32_t dpal_mux_delete(uint32_t mux_handle)
{
    return (LOS_MuxDelete(mux_handle) == LOS_OK) ? DPAL_OK : DPAL_NOK;
}

uint32_t dpal_mux_pend(uint32_t mux_handle, uint32_t timeout)
{
    /* DPAL_WAIT_FOREVER == LOS_WAIT_FOREVER == 0xFFFFFFFF */
    return (LOS_MuxPend(mux_handle, timeout) == LOS_OK) ? DPAL_OK : DPAL_NOK;
}

uint32_t dpal_mux_post(uint32_t mux_handle)
{
    return (LOS_MuxPost(mux_handle) == LOS_OK) ? DPAL_OK : DPAL_NOK;
}

/* ------------------------------------------------------------------ */
/* platform device/driver — synchronous mini-framework                */
/*                                                                     */
/* fbb call order (nand_init.c): dpal_platform_device_register() then  */
/* nand_fmc100_init() -> dpal_platform_driver_register(); the probe    */
/* fires inside driver_register against every matching device.         */
/* ------------------------------------------------------------------ */

#define DPAL_SHIM_MAX_ITEMS 4

static struct dpal_platform_device *g_shim_devs[DPAL_SHIM_MAX_ITEMS];
static struct dpal_platform_driver *g_shim_drvs[DPAL_SHIM_MAX_ITEMS];

static int32_t shim_probe_if_match(struct dpal_platform_driver *drv,
                                   struct dpal_platform_device *dev)
{
    if ((drv == NULL) || (dev == NULL) || (drv->probe == NULL)) {
        return 0;
    }
    if ((dev->name == NULL) || (drv->drv.name == NULL) ||
        (strcmp(dev->name, drv->drv.name) != 0)) {
        return 0;
    }
    return drv->probe(dev);
}

int32_t dpal_platform_device_register(struct dpal_platform_device *dev)
{
    uint32_t i;

    if (dev == NULL) {
        return DPAL_ERRNO_DRIVER_INPUT_INVALID;
    }
    for (i = 0; i < DPAL_SHIM_MAX_ITEMS; i++) {
        if (g_shim_devs[i] == NULL) {
            g_shim_devs[i] = dev;
            break;
        }
    }
    if (i == DPAL_SHIM_MAX_ITEMS) {
        return DPAL_ERRNO_DRIVER_DEVICE_REGISTERED;
    }
    for (i = 0; i < DPAL_SHIM_MAX_ITEMS; i++) {
        if (g_shim_drvs[i] != NULL) {
            (void)shim_probe_if_match(g_shim_drvs[i], dev);
        }
    }
    return DPAL_OK;
}

void dpal_platform_device_unregister(struct dpal_platform_device *dev)
{
    uint32_t i;

    for (i = 0; i < DPAL_SHIM_MAX_ITEMS; i++) {
        if (g_shim_devs[i] == dev) {
            g_shim_devs[i] = NULL;
        }
    }
}

int32_t dpal_platform_driver_register(struct dpal_platform_driver *drv)
{
    uint32_t i;
    int32_t probe_ret = DPAL_OK;

    if (drv == NULL) {
        return DPAL_ERRNO_DRIVER_INPUT_INVALID;
    }
    for (i = 0; i < DPAL_SHIM_MAX_ITEMS; i++) {
        if (g_shim_drvs[i] == NULL) {
            g_shim_drvs[i] = drv;
            break;
        }
    }
    if (i == DPAL_SHIM_MAX_ITEMS) {
        return DPAL_ERRNO_DRIVER_DRIVER_REGISTERED;
    }
    for (i = 0; i < DPAL_SHIM_MAX_ITEMS; i++) {
        if (g_shim_devs[i] != NULL) {
            probe_ret = shim_probe_if_match(drv, g_shim_devs[i]);
            if (probe_ret != 0) {
                break;
            }
        }
    }
    /* propagate a failing probe: morpheus nand_init.c checks it */
    return probe_ret;
}

int32_t dpal_platform_driver_unregister(struct dpal_platform_driver *drv)
{
    uint32_t i;

    for (i = 0; i < DPAL_SHIM_MAX_ITEMS; i++) {
        if (g_shim_drvs[i] == drv) {
            g_shim_drvs[i] = NULL;
        }
    }
    return DPAL_OK;
}

dpal_resource_t *dpal_platform_get_resource(struct dpal_platform_device *dev,
                                            uint32_t type, uint32_t num)
{
    if ((dev == NULL) || (dev->resource == NULL) || (num >= dev->num_resources)) {
        return NULL;
    }
    if ((dev->resource[num].flags & type) == 0) {
        return NULL;
    }
    return &dev->resource[num];
}

uint32_t dpal_platform_ioremap_resource(struct dpal_resource *res)
{
    /* bare metal RISC-V: peripheral registers are directly addressable */
    if (res == NULL) {
        return 0;
    }
    return res->start;
}

/* ------------------------------------------------------------------ */
/* mtd list (fbb kernel/liteos compact mtd_list.c, merged)             */
/* ------------------------------------------------------------------ */

static linklist g_mtd_head;
static uint32_t g_mtd_list_mux = 0xFFFFFFFF; /* 0xFFFFFFFF: invalid mux id */

int mtd_init_list(void)
{
    if (g_mtd_head != NULL) {
        return 0; /* idempotent */
    }
    g_mtd_head = (linklist)dpal_zalloc(sizeof(Lnode));
    if (g_mtd_head == NULL) {
        return -1;
    }
    if (LOS_MuxCreate(&g_mtd_list_mux) != LOS_OK) {
        dpal_free(g_mtd_head);
        g_mtd_head = NULL;
        return -1;
    }
    return 0;
}

struct mtd_info *get_mtd(const char *type)
{
    linklist node;
    struct mtd_info *mtd = NULL;

    if (type == NULL) {
        return NULL;
    }
    if (LOS_MuxPend(g_mtd_list_mux, LOS_WAIT_FOREVER) != LOS_OK) {
        return NULL;
    }
    node = g_mtd_head->next;
    while (node != NULL) {
        if (strcmp(type, node->type) == 0) {
            node->status++;
            mtd = node->mtd;
            break;
        }
        node = node->next;
    }
    (VOID)LOS_MuxPost(g_mtd_list_mux);
    return mtd;
}

int get_mtd_info(const char *type)
{
    linklist node;
    int ret = -1;

    if (type == NULL) {
        return ret;
    }
    if (LOS_MuxPend(g_mtd_list_mux, LOS_WAIT_FOREVER) != LOS_OK) {
        return ret;
    }
    node = g_mtd_head->next;
    while (node != NULL) {
        if (strcmp(type, node->type) == 0) {
            ret = 0;
            break;
        }
        node = node->next;
    }
    (VOID)LOS_MuxPost(g_mtd_list_mux);
    return ret;
}

int free_mtd(struct mtd_info *mtd)
{
    linklist node;
    int ret = -1;

    if (mtd == NULL) {
        return ret;
    }
    if (LOS_MuxPend(g_mtd_list_mux, LOS_WAIT_FOREVER) != LOS_OK) {
        return ret;
    }
    node = g_mtd_head->next;
    while (node != NULL) {
        if (node->mtd == mtd) {
            node->status--;
            ret = 0;
            break;
        }
        node = node->next;
    }
    (VOID)LOS_MuxPost(g_mtd_list_mux);
    return ret;
}

void add_mtd_list(char *type, struct mtd_info *mtd)
{
    linklist new_node;

    if ((type == NULL) || (mtd == NULL)) {
        return;
    }
    if (LOS_MuxPend(g_mtd_list_mux, LOS_WAIT_FOREVER) != LOS_OK) {
        return;
    }
    new_node = (linklist)dpal_zalloc(sizeof(Lnode));
    if (new_node == NULL) {
        (VOID)LOS_MuxPost(g_mtd_list_mux);
        return;
    }
    new_node->type = type;
    new_node->mtd = mtd;
    new_node->status = 0;
    new_node->next = g_mtd_head->next;
    g_mtd_head->next = new_node;
    (VOID)LOS_MuxPost(g_mtd_list_mux);
}

int del_mtd_list(struct mtd_info *mtd)
{
    linklist node;
    linklist prev;
    int ret = -1;

    if (mtd == NULL) {
        return ret;
    }
    if (LOS_MuxPend(g_mtd_list_mux, LOS_WAIT_FOREVER) != LOS_OK) {
        return ret;
    }
    node = g_mtd_head->next;
    prev = g_mtd_head;
    while (node != NULL) {
        if (node->mtd == mtd) {
            if (node->status == 0) {
                prev->next = node->next;
                dpal_free(node);
                ret = 0;
            }
            break;
        }
        prev = node;
        node = node->next;
    }
    (VOID)LOS_MuxPost(g_mtd_list_mux);
    return ret;
}

/* ------------------------------------------------------------------ */
/* misc                                                                */
/* ------------------------------------------------------------------ */

/* fbb mtd_common.h helper (bit index, 1-based, LSB first) */
int ffs(int x)
{
    int i;

    if (x == 0) {
        return 0;
    }
    for (i = 1; i <= 32; i++) {
        if ((x & (1 << (i - 1))) != 0) {
            return i;
        }
    }
    return 0;
}

void dpal_mdelay(uint32_t msecs)
{
    LOS_UDelay((UINT32)msecs * 1000U);
}

void dpal_udelay(uint32_t usecs)
{
    LOS_UDelay((UINT32)usecs);
}

void dpal_msleep(uint32_t msecs)
{
    (VOID)LOS_TaskDelay(msecs);
}

void dpal_dma_cache_clean(uintptr_t start, uintptr_t end)
{
    (void)start;
    (void)end; /* rv32imc: no data cache */
}

void dpal_dma_cache_inv(uintptr_t start, uintptr_t end)
{
    (void)start;
    (void)end;
}

void dpal_dma_cache_flush(uintptr_t start, uintptr_t end)
{
    (void)start;
    (void)end;
}

/* set_errno / get_errno: provided by liteos_m lib/posix (errno.c) —
 * the dpal.h declarations resolve there. Do not redefine them. */
