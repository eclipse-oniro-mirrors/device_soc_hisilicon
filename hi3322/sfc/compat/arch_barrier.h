#ifndef SFC_SHIM_ARCH_BARRIER_H
#define SFC_SHIM_ARCH_BARRIER_H

#ifdef isb
#undef isb
#endif
#define isb() __asm__ __volatile__("fence":::"memory")

#endif /* SFC_SHIM_ARCH_BARRIER_H */
