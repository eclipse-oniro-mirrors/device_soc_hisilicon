/*
 * ---------------------------------------------------------------------------
 * ITCM-resident SFC flash writer — see nor_flash_itcm.c.
 * The functions run from ITCM (section .nor.itcm.text) and must be called with
 * interrupts locked and via the function-pointer wrappers in
 * nor_lfs_bridge.c (the ITCM address is out of JAL range from XIP).
 * ---------------------------------------------------------------------------
 */
#ifndef NOR_FLASH_ITCM_H
#define NOR_FLASH_ITCM_H

#include <stdint.h>

int nor_itcm_sector_erase(uint32_t rel_off);
int nor_itcm_page_prog(uint32_t rel_off, const uint8_t *buf, uint32_t len);
int nor_itcm_read_status(uint8_t *out);

#endif /* NOR_FLASH_ITCM_H */
