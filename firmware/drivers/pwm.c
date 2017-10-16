/*
 * File      : pwm.c
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
#include <rtdevice.h>
#include <gcc/delay.h>
#include "inc/fh_driverlib.h"
#include "fh_def.h"
#include "interrupt.h"
#include "board_info.h"
#include "pwm.h"
#include "gpio.h"
#if defined(FH_PWM_DEBUG) && defined(RT_DEBUG)
#define PRINT_PWM_DBG(fmt, args...)   \
    do                                \
    {                                 \
        rt_kprintf("FH_PWM_DEBUG: "); \
        rt_kprintf(fmt, ##args);      \
    } while (0)
#else
#define PRINT_PWM_DBG(fmt, args...) \
    do                              \
    {                               \
    } while (0)
#endif

#if defined(FH_VPWM_DEBUG) && defined(RT_DEBUG)
#define PRINT_VPWM_DBG(fmt, args...)   \
    do                                 \
    {                                  \
        rt_kprintf("FH_VPWM_DEBUG: "); \
        rt_kprintf(fmt, ##args);       \
    } while (0)
#else
#define PRINT_VPWM_DBG(fmt, args...) \
    do                               \
    {                                \
    } while (0)
#endif

static struct pwm_driver pwm_drv = {

};
#ifdef RT_USING_VPWM

static unsigned int pwm_gpio_ids[FH_VPWM_COUNT] = {
    RT_VPWM_GPIO0,
#if FH_VPWM_COUNT > 1
    RT_VPWM_GPIO1,
#endif
#if FH_VPWM_COUNT > 2
    RT_VPWM_GPIO2,
#endif
#if FH_VPWM_COUNT > 3
    RT_VPWM_GPIO3,
#endif
#if FH_VPWM_COUNT > 4
    RT_VPWM_GPIO4,
#endif
#if FH_VPWM_COUNT > 5
    RT_VPWM_GPIO5,
#endif
};
static enum hrtimer_restart pwm_gpio(struct hrtimer *timer)
{
    struct pwm_device *pwm = timer->data;
    pwm->invert = !pwm->invert;
    gpio_set_value(pwm->gpio_id, pwm->invert);

    ktime_t ticks;
    ticks = ktime_set(0, pwm->invert_ticks[pwm->invert]);
    hrtimer_forward(timer, ktime_get(), ticks);
    return HRTIMER_RESTART;
}

static int VPWM_Enable(struct pwm_device *pwm)
{
//    unsigned int status, gpio_num;
//    rt_thread_t threadPwm;
//    rt_timer_t timer;

    pwm_drv.pwm[pwm->id].invert = 0;
    pwm_drv.pwm[pwm->id].invert_ticks[0] =
        pwm_drv.pwm[pwm->id].period_ns - pwm_drv.pwm[pwm->id].counter_ns;
    pwm_drv.pwm[pwm->id].invert_ticks[1] = pwm_drv.pwm[pwm->id].counter_ns;

    rt_kprintf("id: %d, period: %d, counter: %d\n", pwm->id,
               pwm_drv.pwm[pwm->id].period_ns, pwm_drv.pwm[pwm->id].counter_ns);

    if (pwm_drv.pwm[pwm->id].working == 0)
    {
        hrtimer_set_expires(&pwm_drv.pwm[pwm->id].timer,
                            ktime_set(0, pwm_drv.pwm[pwm->id].period_ns / 2));
        hrtimer_start_expires(&pwm_drv.pwm[pwm->id].timer, HRTIMER_MODE_REL);
        pwm_drv.pwm[pwm->id].working = 1;
    }
    return 0;
}

int VPWM_Disable(struct pwm_device *pwm)
{
    unsigned int status/*, gpio_num*/;
//    rt_thread_t threadPwm;

    status = gpio_release(pwm_drv.pwm[pwm->id].gpio_id);
    if (pwm_drv.pwm[pwm->id].working == 1)
        hrtimer_cancel(&pwm_drv.pwm[pwm->id].timer);

    pwm_drv.pwm[pwm->id].working = 0;

    return status;
}

static int vpwm_set_duty_cycle_ns(struct pwm_device *pwm)
{
//    unsigned int status, gpio_num_out, period_ticks, counter_ns_ticks;
    if (pwm->period_ns < 2000)
    {
        PRINT_VPWM_DBG(" period_ns set too little \n");
        return 0;
    }
    pwm_drv.pwm[pwm->id].period_ns  = pwm->period_ns;
    pwm_drv.pwm[pwm->id].counter_ns = pwm->counter_ns;
    return 0;
}

static int fh_vpwm_open(struct fh_pwm_obj *pwm_obj)
{
    unsigned int count, status;

    for (count = 0; count < FH_VPWM_COUNT; count++)
    {
        hrtimer_init(&pwm_drv.pwm[count + 3].timer, CLOCK_MONOTONIC,
                     HRTIMER_MODE_REL);
        pwm_drv.pwm[count + 3].timer.function = pwm_gpio;
        pwm_drv.pwm[count + 3].timer.data     = &pwm_drv.pwm[count + 3],
                            status =
                                gpio_request((unsigned int)pwm_obj->pri[count]);
        if (status < 0)
        {
            PRINT_VPWM_DBG("ERROR can not open GPIO %d\n",
                           (unsigned int)pwm_obj->pri[count]);
            return status;
        }
        gpio_direction_output((unsigned int)pwm_obj->pri[count], 1);
    }
    return 0;
}

static int fh_vpwm_close(struct fh_pwm_obj *pwm_obj)
{
    unsigned int count, status;

    for (count = 0; count < FH_VPWM_COUNT; count++)
    {
        if (pwm_drv.pwm[count + 3].working)
        {
            hrtimer_cancel(&pwm_drv.pwm[count + 3].timer);
            pwm_drv.pwm[count + 3].working = 0;

            gpio_direction_output((unsigned int)pwm_obj->pri[count], 0);
            status = gpio_release((unsigned int)pwm_obj->pri[count]);

            if (status < 0)
            {
                PRINT_VPWM_DBG("ERROR can not open GPIO %d\n",
                               (unsigned int)pwm_obj->pri[count]);
                return status;
            }
        }
    }
    return 0;
}

static int fh_vpwm_probe(struct pwm_driver *pwm_drv)
{
    unsigned int count;

    struct fh_pwm_obj *pwm_obj = (struct fh_pwm_obj *)pwm_drv->priv;
    pwm_obj->pri               = &pwm_gpio_ids[0];

    for (count = 0; count < FH_VPWM_COUNT; count++)
    {
        pwm_drv->pwm[count + 3].id = count + 3;
        rt_sprintf(pwm_drv->pwm[count + 3].name, "%s%d", "vpwm", count);
        pwm_drv->pwm[count + 3].working = 0;
        pwm_drv->pwm[count + 3].gpio_id = (unsigned int)pwm_obj->pri[count];
    }

    return 0;
}
#endif
static int pwm_get_duty_cycle_ns(struct pwm_device *pwm)
{
    struct fh_pwm_obj *pwm_obj = (struct fh_pwm_obj *)pwm_drv.priv;
    rt_uint32_t reg, period, duty;
    rt_uint32_t clk_rate = 1000000 /*todo: clk_get_rate(fh_pwm_ctrl.clk)*/;

    reg    = PWM_GetPwmCmd(pwm_obj, pwm->id);
    period = reg & 0x0fff;
    duty   = (reg >> 16) & 0xfff;
    duty   = period - duty;  // reverse duty cycle

    if (period == 0)
    {
        period = duty;
    }

    pwm->counter_ns = duty * 1000000000 / clk_rate;
    pwm->period_ns  = period * 1000000000 / clk_rate;

    PRINT_PWM_DBG("get duty: %d, period: %d, reg: 0x%x\n", duty, period, reg);

    return 0;
}

static int pwm_set_duty_cycle_ns(struct pwm_device *pwm)
{
    struct fh_pwm_obj *pwm_obj = (struct fh_pwm_obj *)pwm_drv.priv;
    rt_uint32_t period, duty, reg, clk_rate, duty_revert;
    clk_rate = 1000000 /*todo: clk_get_rate(fh_pwm_ctrl.clk)*/;
    if (!clk_rate)
    {
        rt_kprintf("PWM: clock rate is 0\n");
        return -RT_EIO;
    }

    period = pwm->period_ns / (1000000000 / clk_rate);

    if (period < 8)
    {
        rt_kprintf("PWM: min period is 8\n");
        return -RT_EIO;
    }

    duty = pwm->counter_ns / (1000000000 / clk_rate);

    if (period < duty)
    {
        rt_kprintf("PWM: period < duty\n");
        return -RT_EIO;
    }

    duty_revert = period - duty;

    if (duty == period)
    {
        reg = (duty & 0xfff) << 16 | (0 & 0xfff);
    }
    else
    {
        reg = (duty_revert & 0xfff) << 16 | (period & 0xfff);
    }

    PRINT_PWM_DBG("set duty_revert: %d, period: %d, reg: 0x%x\n", duty_revert,
                  period, reg);

    PWM_SetPwmCmd(pwm_obj, pwm->id, reg);
    return 0;
}

static rt_err_t fh_pwm_open(rt_device_t dev, rt_uint16_t oflag)
{
    struct fh_pwm_obj *pwm_obj = (struct fh_pwm_obj *)pwm_drv.priv;
    PWM_Enable(pwm_obj, RT_TRUE);

#ifdef RT_USING_VPWM
    fh_vpwm_open(pwm_obj);
#endif
    return 0;
}

static rt_err_t fh_pwm_close(rt_device_t dev)
{
    struct fh_pwm_obj *pwm_obj = (struct fh_pwm_obj *)pwm_drv.priv;
//    unsigned int count;
    PWM_Enable(pwm_obj, RT_FALSE);
#ifdef RT_USING_VPWM
    fh_vpwm_close(pwm_obj);
#endif
    return 0;
}

static rt_err_t fh_pwm_ioctl(rt_device_t dev, int cmd, void *arg)
{
    int ret = 0;
    struct pwm_device *pwm;
    struct fh_pwm_obj *pwm_obj = (struct fh_pwm_obj *)pwm_drv.priv;
    switch (cmd)
    {
    case ENABLE_PWM:

        PWM_Enable(pwm_obj, RT_FALSE);
        break;

    case DISABLE_PWM:

        PWM_Enable(pwm_obj, RT_TRUE);

#ifdef RT_USING_VPWM
        fh_vpwm_close(pwm_obj);
#endif
        break;
    case SET_PWM_DUTY_CYCLE:
        pwm = (struct pwm_device *)arg;
#ifdef RT_USING_VPWM
        if (pwm->id < 3)
#endif
            pwm_set_duty_cycle_ns(pwm);
#ifdef RT_USING_VPWM
        else
        {
            vpwm_set_duty_cycle_ns(pwm);
            VPWM_Enable(pwm);
        }
#endif
        break;
    case GET_PWM_DUTY_CYCLE:
        pwm = (struct pwm_device *)arg;
        PRINT_PWM_DBG("ioctl: pwm->id: %d, pwm->counter: %d, pwm->period: %d\n",
                      pwm->id, pwm->counter_ns, pwm->period_ns);
        pwm_get_duty_cycle_ns(pwm);
        break;
    }

    return ret;
}

int fh_pwm_probe(void *priv_data)
{
    rt_device_t pwm_dev;

    struct fh_pwm_obj *pwm_obj = (struct fh_pwm_obj *)priv_data;

    rt_memset(&pwm_drv, 0, sizeof(struct pwm_driver));

    pwm_drv.pwm[0].id = 0;
    pwm_drv.pwm[1].id = 1;
    pwm_drv.pwm[2].id = 2;

    pwm_drv.pwm[0].working = 0;
    pwm_drv.pwm[1].working = 0;
    pwm_drv.pwm[2].working = 0;

    pwm_drv.priv = pwm_obj;
#ifdef RT_USING_VPWM
    PRINT_VPWM_DBG("%d base %x\n", __LINE__, pwm_obj->base);
    fh_vpwm_probe(&pwm_drv);

#endif

    // todo: clk

    PWM_Enable(pwm_obj, RT_FALSE);

    pwm_dev = rt_malloc(sizeof(struct rt_device));
    rt_memset(pwm_dev, 0, sizeof(struct rt_device));

    if (pwm_dev == RT_NULL)
    {
        rt_kprintf("ERROR: %s rt_device malloc failed\n", __func__);
    }

    pwm_dev->user_data = &pwm_drv;
    pwm_dev->open      = fh_pwm_open;
    pwm_dev->close     = fh_pwm_close;
    pwm_dev->control   = fh_pwm_ioctl;
    pwm_dev->type      = RT_Device_Class_Miscellaneous;

    rt_device_register(pwm_dev, "pwm", RT_DEVICE_FLAG_RDWR);

    return 0;
}

int fh_pwm_exit(void *priv_data) { return 0; }
struct fh_board_ops pwm_driver_ops = {
    .probe = fh_pwm_probe, .exit = fh_pwm_exit,
};

void rt_hw_pwm_init(void)
{
    PRINT_PWM_DBG("%s start\n", __func__);
    fh_board_driver_register("pwm", &pwm_driver_ops);
    PRINT_PWM_DBG("%s end\n", __func__);
}
