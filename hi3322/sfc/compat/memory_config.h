/*
 * osal shim: memory_config.h — the SFC porting needs the NOR window
 * base/size (matching the 3322_evb norflash/normal layout and the
 * morpheus pack_fwpkg.sh flash map, 16MB @ 0x26000000) and the LP_CTL
 * flash power-on register block (flash_hw_reset IOLDO6 manual force).
 */
#ifndef SFC_SHIM_MEMORY_CONFIG_H
#define SFC_SHIM_MEMORY_CONFIG_H

#include "3322_lp_ctl_mcu_rb_reg_offset.h"
#include "3322_lp_ctl_mcu_rb_reg_offset_field.h"

#define FLASH_START   0x26000000
#define FLASH_LENGTH  0x1000000

#endif /* SFC_SHIM_MEMORY_CONFIG_H */
