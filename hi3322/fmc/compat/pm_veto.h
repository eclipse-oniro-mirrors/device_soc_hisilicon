/* morpheus fmc compat: fbb pm_veto.h shim.
 *
 * liteos_m has no sleep-veto PM framework; the fbb NAND-boot bootloader
 * carries the same fallback as weak no-ops (provision_3322 emmc/core/mmc.c:
 * __attribute__((weak)) uapi_pm_add_sleep_veto). Keep the fbb call sites in
 * nand_ops.c compiling with no-op statics and the PM_ID_NAND token. */
#ifndef FMC_COMPAT_PM_VETO_H
#define FMC_COMPAT_PM_VETO_H

#define PM_ID_NAND 0

static inline void uapi_pm_add_sleep_veto(uint32_t veto_id)
{
    (void)veto_id;
}

static inline void uapi_pm_remove_sleep_veto(uint32_t veto_id)
{
    (void)veto_id;
}

#endif
