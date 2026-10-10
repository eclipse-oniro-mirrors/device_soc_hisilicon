#ifndef SFC_SHIM_COMMON_DEF_H
#define SFC_SHIM_COMMON_DEF_H

#ifndef array_size
#define array_size(x) (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef min
#define min(x, y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef max
#define max(x, y) (((x) > (y)) ? (x) : (y))
#endif

#ifdef __GNUC__
#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif
#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif
#else
#ifndef likely
#define likely(x) (x)
#endif
#ifndef unlikely
#define unlikely(x) (x)
#endif
#endif

#define unused(var) (void)(var)

#endif /* SFC_SHIM_COMMON_DEF_H */
