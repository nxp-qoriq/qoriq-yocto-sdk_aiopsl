/* Copyright 2014 Freescale Semiconductor Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Freescale Semiconductor nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _FSL_DPLIB_SYS_H
#define _FSL_DPLIB_SYS_H

#ifdef __linux__
#ifdef __uboot__

#define dmb()           (__asm__ __volatile__ ("" : : : "memory"))
#define __iormb()       dmb()
#define __iowmb()       dmb()
#define __arch_getq(a)                  (*(volatile unsigned long *)(a))
#define __arch_putq(v, a)                (*(volatile unsigned long *)(a) = (v))
#define readq(c)        ({ u64 __v = __arch_getq(c); __iormb(); __v; })
#define writeq(v, c)     ({ u64 __v = v; __iowmb(); __arch_putq(__v, c); __v; })
#include <common.h>
#include <errno.h>
#include <asm/io.h>

#else

#include <linux/errno.h>
#include <asm/io.h>
#include <linux/slab.h>

#endif /* __uboot__ */

#ifndef ENOTSUP
#define ENOTSUP		95
#endif

#define ioread64(_p)	    readq(_p)
#define iowrite64(_v, _p)   writeq(_v, _p)

static inline uint64_t iova_alloc(size_t size)
{
	return (uintptr_t)kmalloc(size, GFP_ATOMIC);
}

#else /* __linux__ */

#include "common/types.h"
#include "common/errors.h"
#include "common/fsl_malloc.h"
#include "common/io.h"
#define __iomem

static inline void * iova_alloc(size_t size)
{
	return fsl_os_malloc(size);
}


static inline uint64_t virt_to_phys(void *vaddr)
{
//	return (uint64_t)(0x080000000LL + (uint64_t)vaddr);
	return (uint64_t)vaddr;
}

static inline uint64_t phys_to_virt(void *paddr)
{
//	return (uint64_t)(0x080000000LL + (uint64_t)vaddr);
	return (uint64_t)paddr;
}


#define cpu_to_le64 CPU_TO_LE64

#endif /* __linux__ */


#define MAKE_UMASK64(_width) \
	((uint64_t)((_width) < 64 ? ((uint64_t)1 << (_width)) - 1 : -1))

static inline uint64_t u64_enc(int lsoffset, int width, uint64_t val)
{
	return (uint64_t)(((uint64_t)val & MAKE_UMASK64(width)) << lsoffset);
}
static inline uint64_t u64_dec(uint64_t val, int lsoffset, int width)
{
	return (uint64_t)((val >> lsoffset) & MAKE_UMASK64(width));
}

struct fsl_mc_io {
	void *regs;
};

#endif /* _FSL_DPLIB_SYS_H */
