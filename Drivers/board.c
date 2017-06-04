/*
 * File      : board.c
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

#include <mmu.h>
#include "arch.h"
#include "fh_def.h"
#include "fh_driverlib.h"

#include "board.h"

static struct mem_desc fh_mem_desc[] = {

		{ 0xA0000000, FH_RTT_OS_MEM_END-1, 0xA0000000, SECT_RWX_CB, 0, SECT_MAPPED },
		{ FH_RTT_OS_MEM_END, FH_DDR_END-1, FH_RTT_OS_MEM_END, SECT_RWNX_NCNB, 0, SECT_MAPPED },
		{ 0xFFFF0000, 0xFFFF1000-1, 0xA0000000, SECT_TO_PAGE, PAGE_ROX_CB, PAGE_MAPPED }, /* isr vector table */
		{ 0xE0000000, 0xF1300000-1, 0xE0000000, SECT_RWNX_NCNB, 0, SECT_MAPPED },       /* io table */
		{ 0xF4000000, 0xF4100000-1, 0xF4000000, SECT_RWNX_NCNB, 0, SECT_MAPPED },       /* GPIO#1 io table */
};


#if defined(__CC_ARM)
	extern int Image$$ER_ZI$$ZI$$Base;
	extern int Image$$ER_ZI$$ZI$$Length;
	extern int Image$$ER_ZI$$ZI$$Limit;
#elif (defined (__GNUC__))
	rt_uint8_t _irq_stack_start[1024];
	rt_uint8_t _fiq_stack_start[1024];
	rt_uint8_t _undefined_stack_start[512];
	rt_uint8_t _abort_stack_start[512];
	rt_uint8_t _svc_stack_start[4096] SECTION(".nobss");
	extern unsigned char __bss_start;
	extern unsigned char __bss_end;
#endif



void rt_hw_board_init(void)
{
	/* disable interrupt first */
	rt_hw_interrupt_disable();
	/* initialize hardware interrupt */
	rt_hw_interrupt_init();

	/* initialize mmu */
	rt_hw_mmu_init(fh_mem_desc, sizeof(fh_mem_desc)/sizeof(fh_mem_desc[0]));

	rt_system_heap_init((void*)&__bss_end, (void*)FH_RTT_OS_MEM_END);
#ifdef RT_USING_DMA_MEM
	//just use the last 100KB
	dma_memheap_init((rt_uint32_t *)FH_RTT_OS_MEM_END, FH_DMA_MEM_SIZE);
#endif

	/* initialize the system clock */
	rt_hw_clock_init();
    fh_iomux_init(PMU_REG_BASE + 0x5c);

	/* initialize timer1 */
	rt_hw_timer_init();

	/* initialize UART */


}
