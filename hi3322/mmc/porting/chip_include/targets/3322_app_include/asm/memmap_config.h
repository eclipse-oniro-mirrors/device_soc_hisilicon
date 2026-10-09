/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description:  os memmap header.
 * Author:
 * Create: 2023-03-09
 */

#ifndef _MEMMAP_CONFIG_H
#define _MEMMAP_CONFIG_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

extern void *g_intheap_begin;
extern void *g_intheap_size;
extern void *g_ram_begin;
extern void *g_ram_size;
extern void *g_extend_heap_begin;
extern void *g_extend_heap_size;
#ifdef CONFIG_NPU_LIB
extern void *g_psram_heap_begin;
extern void *g_psram_heap_size;
#endif
extern uintptr_t g_usb_mem_addr_start;
extern unsigned long g_usb_mem_size;

#ifdef LOSCFG_LIB_CONFIGURABLE

extern void *g_sysMemAddr;
extern unsigned int g_sysMemSize;
#define OS_SYS_MEM_ADDR     g_sysMemAddr
#define OS_SYS_MEM_SIZE     g_sysMemSize

#else
#define OS_SYS_MEM_ADDR    ((void *)&g_intheap_begin)
#define OS_SYS_MEM_SIZE    ((unsigned)&g_intheap_size)
#define OS_EXTEND_MEM_ADDR    ((void *)&g_extend_heap_begin)
#define OS_EXTEND_MEM_SIZE    ((unsigned)&g_extend_heap_size)
#define OS_SYS_LIBC_MEM_ADDR        OS_EXTEND_MEM_ADDR
#define OS_SYS_LIBC_MEM_POOL_SIZE   OS_EXTEND_MEM_SIZE
#endif  /* LOSCFG_LIB_CONFIGURABLE */

#ifdef CONFIG_NPU_LIB
#define OS_PSRAM_HEAP_MEM_ADDR    ((void *)&g_psram_heap_begin)
#define OS_PSRAM_HEAP_MEM_SIZE    ((unsigned)&g_psram_heap_size)
#endif

#ifdef LOSCFG_MEM_DFX_SHOW_CALLER_RA
extern UINTPTR              __irq_stack_top__;
#ifndef APP_IRQ_STACK_LEN
#define APP_IRQ_STACK_LEN   0xC00
#endif
#define IRQ_STACK_TOP       ((UINTPTR)&__irq_stack_top__)
#define IRQ_STACK_SIZE      APP_IRQ_STACK_LEN
#endif

#ifdef LOSCFG_SHELL_ADAPT_AT
extern void *__psram_data_size;
#define PSRAM_DATA_SIZE ((unsigned)&__psram_data_size)
extern void *__psram_bss_size;
#define PSRAM_BSS_SIZE ((unsigned)&__psram_bss_size)
extern void *__ramtext_size__;
#define RAM_TEXT_SIZE ((unsigned)&__ramtext_size__)
extern void *g_ram_size;
#define RAM_SIZE ((unsigned)&g_ram_size)

#define OS_SYS_TEXT_SIZE (RAM_TEXT_SIZE + RAM_SIZE)
extern void *__data_size__;
#define OS_SYS_DATA_SIZE (((unsigned)&__data_size__) + PSRAM_DATA_SIZE)
extern void *__bss_size__;
#define OS_SYS_BSS_SIZE (((unsigned)&__bss_size__) + PSRAM_BSS_SIZE)
extern void *__rodata_size__;
#define OS_SYS_RODATA_SIZE (RAM_TEXT_SIZE)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _MEMMAP_CONFIG_H */
