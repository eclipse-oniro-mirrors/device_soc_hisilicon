/*
 * Copyright (C) 2021 HiSilicon (Shanghai) Technologies CO., LIMITED.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef _DRV_IOCTL_H
#define _DRV_IOCTL_H

/* ioctl command encoding: 32 bits total, command in lower 16 bits,
 * size of the parameter structure in the lower 14 bits of the
 * upper 16 bits.
 * Encoding the size of the parameter structure in the ioctl request
 * is useful for catching programs compiled with old versions
 * and to avoid overwriting user space outside the user buffer area.
 * The highest 2 bits are reserved for indicating the ``access mode''.
 * NOTE: This limits the max parameter size to 16kB -1 !
 */
/*
 * The following is for compatibility across the various Linux
 * platforms.  The generic ioctl numbering scheme doesn't really enforce
 * a type field.  De facto, however, the top 8 bits of the lower 16
 * bits are indeed used as a type field, so we might just as well make
 * this explicit here.  Please be sure to use the decoding macros
 * below from now on.
 */
#define _IOC_NRBITS    8
#define _IOC_TYPEBITS  8

/*
 * Let any architecture override either of the following before
 * including this file.
 */

#ifndef _IOC_SIZEBITS
#define _IOC_SIZEBITS  14
#endif

#ifndef _IOC_DIRBITS
#define _IOC_DIRBITS   2
#endif

#ifndef _IOC_NRMASK
#define _IOC_NRMASK    ((1 << _IOC_NRBITS)-1)
#endif
#ifndef _IOC_TYPEMASK
#define _IOC_TYPEMASK    ((1 << _IOC_TYPEBITS)-1)
#endif
#ifndef _IOC_SIZEMASK
#define _IOC_SIZEMASK    ((1 << _IOC_SIZEBITS)-1)
#endif
#ifndef _IOC_DIRMASK
#define _IOC_DIRMASK    ((1 << _IOC_DIRBITS)-1)
#endif

#ifndef _IOC_NRSHIFT
#define _IOC_NRSHIFT    0
#endif
#ifndef _IOC_TYPESHIFT
#define _IOC_TYPESHIFT    (_IOC_NRSHIFT + _IOC_NRBITS)
#endif
#ifndef _IOC_SIZESHIFT
#define _IOC_SIZESHIFT    (_IOC_TYPESHIFT + _IOC_TYPEBITS)
#endif
#ifndef _IOC_DIRSHIFT
#define _IOC_DIRSHIFT    (_IOC_SIZESHIFT + _IOC_SIZEBITS)
#endif

/*
 * Direction bits, which any architecture can choose to override
 * before including this file.
 */

#ifndef _IOC_NONE
#define _IOC_NONE      0U
#endif

#ifndef _IOC_WRITE
#define _IOC_WRITE     1U
#endif

#ifndef _IOC_READ
#define _IOC_READ      2U
#endif

#ifndef _IOC
#define _IOC(dir, type, nr, size)    \
    (((dir) << _IOC_DIRSHIFT) |  \
        ((type) << _IOC_TYPESHIFT) |  \
        ((nr) << _IOC_NRSHIFT) |  \
        ((size) << _IOC_SIZESHIFT))
#endif

#ifdef __CHECKER__
#define _IOC_TYPECHECK(t) (sizeof(t))
#else
/* provoke compile error for invalid uses of size argument */
extern unsigned int __invalid_size_argument_for_IOC;
#define _IOC_TYPECHECK(t)                  \
    ((sizeof(t) == sizeof(t[1]) &&         \
         sizeof(t) < (1 << _IOC_SIZEBITS)) \
            ? sizeof(t)                    \
            : __invalid_size_argument_for_IOC)
#endif

/* used to create numbers */
#ifndef _IO
#define _IO(type, nr)        _IOC(_IOC_NONE, (type), (nr), 0)
#endif

#ifndef _IOR
#define _IOR(type, nr, size)    _IOC(_IOC_READ, (type), (nr), (_IOC_TYPECHECK(size)))
#endif

#ifndef _IOW
#define _IOW(type, nr, size)    _IOC(_IOC_WRITE, (type), (nr), (_IOC_TYPECHECK(size)))
#endif

#ifndef _IOWR
#define _IOWR(type, nr, size)    _IOC(_IOC_READ | _IOC_WRITE, (type), (nr), (_IOC_TYPECHECK(size)))
#endif

#ifndef _IOR_BAD
#define _IOR_BAD(type, nr, size)    _IOC(_IOC_READ, (type), (nr), sizeof(size))
#endif

#ifndef _IOW_BAD
#define _IOW_BAD(type, nr, size)    _IOC(_IOC_WRITE, (type), (nr), sizeof(size))
#endif

#ifndef _IOWR_BAD
#define _IOWR_BAD(type, nr, size)    _IOC(_IOC_READ | _IOC_WRITE, (type), (nr), sizeof(size))
#endif

/* used to decode ioctl numbers.. */

#ifndef _IOC_DIR
#define _IOC_DIR(nr)        (((nr) >> _IOC_DIRSHIFT) & _IOC_DIRMASK)
#endif

#ifndef _IOC_TYPE

#define _IOC_TYPE(nr)        (((nr) >> _IOC_TYPESHIFT) & _IOC_TYPEMASK)
#endif

#ifndef _IOC_NR
#define _IOC_NR(nr)        (((nr) >> _IOC_NRSHIFT) & _IOC_NRMASK)
#endif

#ifndef _IOC_SIZE
#define _IOC_SIZE(nr)        (((nr) >> _IOC_SIZESHIFT) & _IOC_SIZEMASK)
#endif


#ifndef IOC_IN
#define IOC_IN        (_IOC_WRITE << _IOC_DIRSHIFT)
#endif

#ifndef IOC_OUT
#define IOC_OUT        (_IOC_READ << _IOC_DIRSHIFT)
#endif

#ifndef IOC_INOUT
#define IOC_INOUT    ((_IOC_WRITE | _IOC_READ) << _IOC_DIRSHIFT)
#endif

#ifndef IOCSIZE_MASK
#define IOCSIZE_MASK    (_IOC_SIZEMASK << _IOC_SIZESHIFT)
#endif

#ifndef IOCSIZE_SHIFT
#define IOCSIZE_SHIFT    (_IOC_SIZESHIFT)
#endif

#endif /* _DRV_IOCTL_H */
