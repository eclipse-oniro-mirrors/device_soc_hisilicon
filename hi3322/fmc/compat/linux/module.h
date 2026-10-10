/* morpheus fmc compat: fbb linux/module.h — the fmc stack is linked
 * statically and initialized from board main.c, so module_init/_exit are
 * no-ops. */
#ifndef FMC_COMPAT_LINUX_MODULE_H
#define FMC_COMPAT_LINUX_MODULE_H
#define module_init(fn)
#define module_exit(fn)
#endif
