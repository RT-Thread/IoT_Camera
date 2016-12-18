/*
 * File      : iomux.c
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

#include "rtthread.h"
#include "rtdebug.h"
#include "iomux.h"

static void fh_iomux_setmfs(Iomux_Pad *pad)
{
    switch (pad->reg_type)
    {
    case 8:
        (IOMUX_PADTYPE(8) pad->reg)->bit.mfs = pad->func_sel;
        break;
    case 13:
        (IOMUX_PADTYPE(13) pad->reg)->bit.mfs = pad->func_sel;
        break;
    case 20:
        (IOMUX_PADTYPE(20) pad->reg)->bit.mfs = pad->func_sel;
        break;
    case 21:
        (IOMUX_PADTYPE(21) pad->reg)->bit.mfs = pad->func_sel;
        break;
    default:
        break;
    }
}

#if defined(IOMUX_DEBUG) && defined(RT_DEBUG)

static int fh_iomux_getmfs(Iomux_Pad *pad)
{
    int mfs;

    switch (pad->reg_type)
    {
    case 8:
        mfs = (IOMUX_PADTYPE(8) pad->reg)->bit.mfs;
        break;
    case 13:
        mfs = (IOMUX_PADTYPE(13) pad->reg)->bit.mfs;
        break;
    case 20:
        mfs = (IOMUX_PADTYPE(20) pad->reg)->bit.mfs;
        break;
    case 21:
        mfs = (IOMUX_PADTYPE(21) pad->reg)->bit.mfs;
        break;
    default:
        mfs = -1;
        break;
    }
    return mfs;
}

static void fh_iomux_print()
{
    int i;
    UINT32 reg;

    for (i = 0; i < ARRAY_SIZE(fh81_iomux_cfg); i++)
    {
        int curr_func;

        curr_func = fh81_iomux_getmfs(iomux_obj, &iomux_obj.pads[i]);
        reg       = readl((UINT32)iomux_obj.pads[i].reg);

        if (curr_func < 0)
            rt_kprintf("\t%d\t\t%-8s(no mfs)\t0x%08x\n", i,
                       iomux_obj.pads[i].func_name[0], reg);
        else
            rt_kprintf("\t%d\t\t%-16s\t0x%08x\n", i,
                       iomux_obj.pads[i].func_name[curr_func], reg);
    }
}

#endif

static void fh_iomux_setcur(Iomux_Pad *pad)
{
    switch (pad->reg_type)
    {
    case 5:
        (IOMUX_PADTYPE(5) pad->reg)->bit.e8_e4 = pad->drv_cur;
        break;
    case 8:
        (IOMUX_PADTYPE(8) pad->reg)->bit.e8_e4 = pad->drv_cur;
        break;
    case 13:
        (IOMUX_PADTYPE(13) pad->reg)->bit.e4_e2 = pad->drv_cur;
        break;
    case 17:
        (IOMUX_PADTYPE(17) pad->reg)->bit.e8_e4 = pad->drv_cur;
        break;
    case 20:
        (IOMUX_PADTYPE(20) pad->reg)->bit.e4_e2 = pad->drv_cur;
        break;
    case 21:
        (IOMUX_PADTYPE(21) pad->reg)->bit.e4_e2 = pad->drv_cur;
        break;
    default:
        break;
    }
}

static void fh_iomux_setpupd(Iomux_Pad *pad)
{
    switch (pad->reg_type)
    {
    case 9:
        (IOMUX_PADTYPE(9) pad->reg)->bit.pu_pd = pad->pupd;
        break;
    case 17:
        (IOMUX_PADTYPE(17) pad->reg)->bit.pu_pd = pad->pupd;
        break;
    case 20:
        (IOMUX_PADTYPE(20) pad->reg)->bit.pu_pd = pad->pupd;
        break;
    case 21:
        (IOMUX_PADTYPE(21) pad->reg)->bit.pu_pd = pad->pupd;
        break;
    default:
        break;
    }
}

static void fh_iomux_setrest(Iomux_Pad *pad)
{
    switch (pad->reg_type)
    {
    case 5:
        (IOMUX_PADTYPE(5) pad->reg)->bit.sr = 0;
        break;
    case 8:
        (IOMUX_PADTYPE(8) pad->reg)->bit.sr = 0;
        break;
    case 9:
        (IOMUX_PADTYPE(9) pad->reg)->bit.ie  = 1;
        (IOMUX_PADTYPE(9) pad->reg)->bit.smt = 1;
        break;
    case 13:
        (IOMUX_PADTYPE(13) pad->reg)->bit.ie  = 1;
        (IOMUX_PADTYPE(13) pad->reg)->bit.smt = 1;
        break;
    case 17:
        (IOMUX_PADTYPE(17) pad->reg)->bit.sr  = 0;
        (IOMUX_PADTYPE(17) pad->reg)->bit.ie  = 1;
        (IOMUX_PADTYPE(17) pad->reg)->bit.e   = 1;
        (IOMUX_PADTYPE(17) pad->reg)->bit.smt = 1;
        break;
    case 20:
        (IOMUX_PADTYPE(20) pad->reg)->bit.sr  = 0;
        (IOMUX_PADTYPE(20) pad->reg)->bit.ie  = 1;
        (IOMUX_PADTYPE(20) pad->reg)->bit.smt = 1;
        break;
    case 21:
        (IOMUX_PADTYPE(21) pad->reg)->bit.sr  = 0;
        (IOMUX_PADTYPE(21) pad->reg)->bit.ie  = 1;
        (IOMUX_PADTYPE(21) pad->reg)->bit.smt = 1;
        break;
    default:
        break;
    }
}

static Iomux_Object iomux_obj;
extern Iomux_Pad fh_iomux_cfg[];
extern const int fh_iomux_cfg_count;

void __fh_setiomux(Iomux_Pad *pad, void *iobase)
{
    UINT32 regvalue = 0;
    pad->reg        = &regvalue;
    fh_iomux_setmfs(pad);
    fh_iomux_setcur(pad);
    fh_iomux_setpupd(pad);
    fh_iomux_setrest(pad);
    SET_REG(iobase, regvalue);
}

static UINT32 g_iomux_base;

void fh_iomux_init(UINT32 base)
{
    int i;
    UINT32 reg;
    g_iomux_base = base;

    iomux_obj.pbase = (void *)base;
    iomux_obj.pads  = fh_iomux_cfg;

    for (i = 0; i < fh_iomux_cfg_count; i++)
    {
        //		iomux_obj.pads[i].id = i;
        iomux_obj.pads[i].reg_offset = iomux_obj.pads[i].id * 4;
        iomux_obj.pads[i].reg =
            &reg;  //(UINT32 *)(iomux_obj.vbase + iomux_obj.pads[i].reg_offset);
        fh_iomux_setmfs(&fh_iomux_cfg[i]);
        fh_iomux_setcur(&fh_iomux_cfg[i]);
        fh_iomux_setpupd(&fh_iomux_cfg[i]);
        fh_iomux_setrest(&fh_iomux_cfg[i]);
        SET_REG(iomux_obj.pbase + iomux_obj.pads[i].reg_offset, reg);
    }

#ifdef CONFIG_RMII
    //(IOMUX_PADTYPE(17)(iomux_obj.pads[18]).reg)->bit.e = 1;
    reg = GET_REG(0xf00000a4);
    reg |= (1 << 13);
    SET_REG(0xf00000a4, reg);
#else
    //(IOMUX_PADTYPE(17)(iomux_obj.pads[18]).reg)->bit.e = 0;
    reg = GET_REG(0xf00000a4);
    reg &= ~(1 << 13);
    SET_REG(0xf00000a4, reg);
#endif
#if defined(IOMUX_DEBUG) && defined(RT_DEBUG)
    fh_iomux_print(iomux_obj);
#endif

    // rt_free(iomux_obj.vbase);
    // iomux_obj.vbase = 0;
}

void fh_iomux_pin_switch(int pin_num, int func_num)
{

    int i;
    for (i = 0; i < fh_iomux_cfg_count; i++)
    {
        if (iomux_obj.pads[i].id == pin_num)
        {
            iomux_obj.pads[i].func_sel = func_num;
            __fh_setiomux(&iomux_obj.pads[i], (void *)g_iomux_base + pin_num * 4);
            break;
        }
    }

    if (i == fh_iomux_cfg_count)
    {
        rt_kprintf("pin: %d does not exist.\n", pin_num);
        return;
    }

    /*
       fh_iomux_cfg[pin_num].func_sel = func_num;
        fh_iomux_setmfs(&fh_iomux_cfg[pin_num]);
        SET_REG(iomux_obj.pbase + iomux_obj.pads[pin_num].reg_offset, *((UINT32
       *)(iomux_obj.vbase + iomux_obj.pads[pin_num].reg_offset)));
    */
}

void fh_select_gpio_before_init(int gpio_no)
{
    int i;
    for (i = 0; i < fh_iomux_cfg_count; i++)
    {
        if (iomux_obj.pads[i].gpio_no == gpio_no)
        {
            iomux_obj.pads[i].func_sel = iomux_obj.pads[i].gpio_sel;
            break;
        }
    }
    if (i == fh_iomux_cfg_count)
        rt_kprintf("ERROR: Can't support gpio #%d\n", gpio_no);
}

void fh_select_gpio(int gpio_no)
{
    int i;
    for (i = 0; i < fh_iomux_cfg_count; i++)
    {
        if (iomux_obj.pads[i].gpio_no == gpio_no)
        {
            fh_iomux_pin_switch(iomux_obj.pads[i].id, iomux_obj.pads[i].gpio_sel);
            break;
        }
    }
    if (i == fh_iomux_cfg_count)
        rt_kprintf("ERROR: Can't support gpio #%d\n", gpio_no);
}
