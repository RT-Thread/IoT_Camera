/*
 * File      : pwm.h
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

#ifndef PWM_H_
#define PWM_H_

// #define FH_VPWM_DEBUG
#include <rtthread.h>
#ifdef RT_USING_TIMEKEEPING
#include <timekeeping.h>
#include <hrtimer.h>
#endif
#define ENABLE_PWM (0x10)
#define DISABLE_PWM (0x11)

#define SET_PWM_DUTY_CYCLE (0x12)
#define GET_PWM_DUTY_CYCLE (0x13)

struct pwm_device
{
    int id;
    int working;
    rt_uint32_t period_ns;
    rt_uint32_t counter_ns;
#ifdef RT_USING_VPWM
    unsigned int gpio_id;
    rt_uint32_t period_ticks;
    rt_uint32_t counter_ticks;
    rt_uint32_t invert_ticks[2];
    void *func;
    char name[8];
    struct hrtimer timer;
    int invert;
#endif
};

struct pwm_driver
{
// struct clk          *clk;
#ifdef RT_USING_VPWM
    struct pwm_device pwm[FH_VPWM_COUNT + 3];
#else
    struct pwm_device pwm[3];
#endif
    struct pwm_device *cur;
    void *priv;
};

void rt_hw_pwm_init(void);

#endif /* PWM_H_ */
