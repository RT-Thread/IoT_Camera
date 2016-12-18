/*
 * File      : timer.c
 * This file is part of FH8620 BSP for RT-Thread distribution.
 *
 * Copyright (c) 2016 Shanghai Fullhan Microelectronics Co., Ltd.
 * All rights reserved
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
 *  Visit http://www.fullhan.com to get contact with Fullhan.
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include "timer.h"
#include <rtdevice.h>
#include "fh_arch.h"
#include "Libraries/inc/fh_timer.h"
#include "interrupt.h"
#ifdef RT_USING_TIMEKEEPING
#include <timekeeping.h>
#include <hrtimer.h>
#endif

#define TIMER_CLOCK 1000000
#define PAE_PTS_CLK 1000000
#define TIMER1_CLK (1000000)

#define CONFIG_PAE_PTS_CLOCK (1000000)
#define TICKS_PER_USEC (CONFIG_PAE_PTS_CLOCK / 1000000)
#define REG_PAE_PTS_REG (0xec100000 + 0x0040)

// TMR
#define REG_TIMER0_LOADCNT (TMR_REG_BASE + 0x00)
#define REG_TIMER0_CUR_VAL (TMR_REG_BASE + 0x04)
#define REG_TIMER0_CTRL_REG (TMR_REG_BASE + 0x08)
#define REG_TIMER0_EOI_REG (TMR_REG_BASE + 0x0C)
#define REG_TIMER0_INTSTATUS (TMR_REG_BASE + 0x10)

#define REG_TIMER1_LOADCNT (TMR_REG_BASE + 0x14)
#define REG_TIMER1_CUR_VAL (TMR_REG_BASE + 0x18)
#define REG_TIMER1_CTRL_REG (TMR_REG_BASE + 0x1C)
#define REG_TIMER1_EOI_REG (TMR_REG_BASE + 0x20)
#define REG_TIMER1_INTSTATUS (TMR_REG_BASE + 0x24)

#define REG_TIMERS_STATUS (TMR_REG_BASE + 0xa0)

#define REG_PMU_SWRST_MAIN_CTRL (PMU_REG_BASE + 0x040)

rt_uint32_t read_pts(void) { return GET_REG(REG_PAE_PTS_REG); }
#ifdef RT_USING_TIMEKEEPING

static cycle_t fh_pts_read(struct clocksource *cs)
{
    return GET_REG(REG_PAE_PTS_REG);
}

struct clocksource clocksource_pts = {
    .name = "pts", .rating = 300, .read = fh_pts_read, .mask = 0xffffffff,
};

void clocksource_pts_register(void)
{
    clocksource_register_hz(&clocksource_pts, PAE_PTS_CLK);
}

static int fh_set_next_event(unsigned long cycles,
                             struct clock_event_device *dev)
{
//    unsigned int curr_val;

    SET_REG_M(REG_TIMER0_CTRL_REG, 0x00, 0x1);
    SET_REG(REG_TIMER0_LOADCNT, cycles);
    SET_REG(REG_TIMER0_CTRL_REG, 0x3);

    return 0;
}

static void fh_set_mode(enum clock_event_mode mode,
                        struct clock_event_device *dev)
{
    switch (mode)
    {
    case CLOCK_EVT_MODE_PERIODIC:
        SET_REG(REG_TIMER0_CTRL_REG, 0x3);
        SET_REG(REG_TIMER0_LOADCNT, TIMER_CLOCK / RT_TICK_PER_SECOND);

        // pmu reset
        SET_REG(REG_PMU_SWRST_MAIN_CTRL, 0xfff7ffff);
        while (GET_REG(REG_PMU_SWRST_MAIN_CTRL) != 0xffffffff)
        {
        }

        break;
    case CLOCK_EVT_MODE_ONESHOT:
        break;
    case CLOCK_EVT_MODE_UNUSED:
    case CLOCK_EVT_MODE_SHUTDOWN:
        SET_REG(REG_TIMER0_CTRL_REG, 0x0);
        break;
    case CLOCK_EVT_MODE_RESUME:
        SET_REG(REG_TIMER0_CTRL_REG, 0x3);
        break;
    }
}

static struct clock_event_device clockevent_timer0 = {
    .name           = "fh_clockevent",
    .features       = CLOCK_EVT_FEAT_PERIODIC | CLOCK_EVT_FEAT_ONESHOT,
    .shift          = 32,
    .set_next_event = fh_set_next_event,
    .set_mode       = fh_set_mode,
};

void clockevent_handler(int vector, void *param)
{
    rt_interrupt_enter();

    clockevent_timer0.event_handler(&clockevent_timer0);
    GET_REG(REG_TIMER0_EOI_REG);

    rt_interrupt_leave();
}

void clockevent_timer0_register(void)
{
    clockevent_timer0.mult =
        div_sc(TIMER_CLOCK, NSEC_PER_SEC, clockevent_timer0.shift);
    clockevent_timer0.max_delta_ns = 100000000;
    clockevent_timer0.min_delta_ns = 1000;
    clockevents_register_device(&clockevent_timer0);

    rt_hw_interrupt_install(TMR0_IRQn, clockevent_handler,
                            (void *)&clockevent_timer0, "sys_tick");
}
#endif

void rt_timer_handler(int vector, void *param)
{
    timer *tim = param;

    rt_interrupt_enter();

    timer_get_eoi(tim);

    rt_tick_increase();

    rt_interrupt_leave();
}

/**
 * This function will init pit for system ticks
 */
void rt_hw_timer_init()
{
    timer *tim = (timer *)TMR_REG_BASE;
    timer_init(tim);
    /* install interrupt handler */
    rt_hw_interrupt_install(TMR0_IRQn, rt_timer_handler, (void *)tim,
                            "sys_tick");
    rt_hw_interrupt_umask(TMR0_IRQn);

    timer_set_mode(tim, TIMER_MODE_PERIODIC);
    timer_set_period(tim, RT_TICK_PER_SECOND, TIMER_CLOCK);
    timer_enable_irq(tim);
    timer_enable(tim);
}
