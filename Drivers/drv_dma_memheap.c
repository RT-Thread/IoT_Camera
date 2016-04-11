/*
 * File      : drv_dma_memheap.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2008 - 2012, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2016-04-11     Urey         the first version
 */

#include <rthw.h>
#include <rtthread.h>

#include "board.h"

#ifdef RT_USING_DMA_MEM

static struct rt_memheap dma_heap = {0};

rt_err_t dma_memheap_init(rt_uint32_t *mem_start, rt_uint32_t size)
{
	return rt_memheap_init(&dma_heap, "dma_heap", mem_start, size);
}

void *dma_memheap_malloc(rt_uint32_t size)
{
	return rt_memheap_alloc(&dma_heap, size);
}

void dma_memheap_free(void *ptr)
{
	rt_memheap_free(ptr);
}


#endif
