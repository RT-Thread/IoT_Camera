/*
 * File      : timer.h
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

#ifndef TIMER_H_
#define TIMER_H_

#include <rtdef.h>

struct fh_timer
{
    struct rt_object parent; /**< inherit from rt_object */

    rt_list_t list;

    void (*timeout_func)(void *parameter); /**< timeout function */
    void *parameter;                       /**< timeout function's parameter */

    rt_uint32_t init_cycle;   /**< timer inited cycle */
    rt_uint32_t timeout_tick; /**< timeout cycle*/
};

void rt_hw_timer_init(void);
void clocksource_pts_register(void);
void clockevent_timer0_register(void);
#endif
