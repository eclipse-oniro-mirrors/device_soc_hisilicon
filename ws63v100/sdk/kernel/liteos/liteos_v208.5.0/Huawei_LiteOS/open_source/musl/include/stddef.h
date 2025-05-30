#ifndef _STDDEF_H
#define _STDDEF_H

#ifdef __LITEOS__
#undef NULL
#ifdef __cplusplus
#define NULL 0L
#else
#define NULL ((void*)0)
#endif
#else
#if __cplusplus >= 201103L
#define NULL nullptr
#elif defined(__cplusplus)
#define NULL 0L
#else
#define NULL ((void*)0)
#endif
#endif

#define __NEED_ptrdiff_t
#define __NEED_size_t
#define __NEED_wchar_t
#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)) || (defined(__cplusplus) && (__cplusplus >= 201103L))
#define __NEED_max_align_t
#endif

#include <bits/alltypes.h>

#if __GNUC__ > 3
#define offsetof(type, member) __builtin_offsetof(type, member)
#else
#define offsetof(type, member) ((size_t)( (char *)&(((type *)0)->member) - (char *)0 ))
#endif

#endif
