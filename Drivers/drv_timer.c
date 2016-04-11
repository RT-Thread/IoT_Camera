/*
 * File      : drv_timer.c
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
 * 2016��4��11��     Urey         the first version
 */


#include <rthw.h>
#include <rtdevice.h>

#include "arch.h"
#include "fh_timer.h"

#include "board.h"
#include "drv_timer.h"


#define TIMER_CLOCK 			1000000

#define CONFIG_PAE_PTS_CLOCK   (1000000)
#define TICKS_PER_USEC         (CONFIG_PAE_PTS_CLOCK / 1000000)
#define REG_PAE_PTS_REG        (0xec100000 + 0x0040)

static unsigned long lastdec;
static unsigned long long timestamp;

rt_uint32_t read_pts(void)
{
	return GET_REG(REG_PAE_PTS_REG);
}


unsigned long long get_ticks(void)
{
	rt_uint32_t now = read_pts();
	if (now >= lastdec)
	{
		/* normal mode */
		timestamp += now - lastdec;
	}
	else
	{
		now = read_pts();
		if (now >= lastdec)
			timestamp += now - lastdec;
		else
			timestamp += now + 0xffffffff - lastdec;	/* we have an overflow ... */
	}
	lastdec = now;
	return timestamp / (TICKS_PER_USEC * 10);
}

void udelay(unsigned long usec)
{
	unsigned long long tmp;
	rt_uint32_t tmo;
	tmo = (usec + 9) / 10;
	tmp = get_ticks() + tmo; /* get current timestamp */

	/* loop till event */
	while (get_ticks() < tmp)
		;
}

void rt_timer_handler(int vector, void *param)
{
	timer *tim = param;

//	rt_interrupt_enter();
	timer_get_eoi(tim);
	rt_tick_increase();
//	rt_interrupt_leave();
}

/**
 * This function will init pit for system ticks
 */
void rt_hw_timer_init()
{
	timer *tim = (timer *) TMR_REG_BASE;
	timer_init(tim);

	/* install interrupt handler */
	rt_hw_interrupt_install(TMR0_IRQn, rt_timer_handler, (void *) tim, "sys_tick");
	rt_hw_interrupt_umask(TMR0_IRQn);

	/* setup tiemr */
	timer_set_mode(tim, TIMER_MODE_PERIODIC);
	timer_set_period(tim, RT_TICK_PER_SECOND, TIMER_CLOCK);
	timer_enable_irq(tim);
	timer_enable(tim);
}
