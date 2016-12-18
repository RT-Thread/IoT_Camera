/*
 * File      : vfp.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Develop Team
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
 * 2014-11-07     weety      first version
 */
#include <stdio.h>
#include <rthw.h>
#include <rtthread.h>
#include "vfp.h"

#ifdef RT_USING_VFP

void vfp_init(void)
{
    int ret = 0;
    unsigned int value;
    asm  volatile ("mrc p15, 0, %0, c1, c0, 2"
            :"=r"(value)
            :);
    value |= 0xf00000;/*enable CP10, CP11 user access*/
    asm volatile("mcr p15, 0, %0, c1, c0, 2"
            :
            :"r"(value));

    asm volatile("fmrx %0, fpexc"
            :"=r"(value));
    value |=(1<<30);
    asm volatile("fmxr fpexc, %0"
            :
            :"r"(value));

}

void vfp_regs_load(float arrays[32])
{
    asm volatile("fldmias %0, {s0-s31}\n"
            :
            :"r"(arrays));
}
void vfp_regs_save(float arrays[32])
{
    asm volatile ("fstmias %0, {s0-s31}"
            :
            :"r"(arrays));
}

void print_array(float array[32])
{
    int i;
    for(i=0; i<32; i++)
    {
        if(i%8==0)
            printf("\n");
        printf("%f ",i, array[i]);
    }
    printf("\n");
}

void vfp_test(void)
{
    unsigned int fpscr;
    float f1=1.0, f2=1.0;
    float farrays[32], farrays2[32];
    int i;

    //All instructions are scalar according to manual
    fpscr = 0x000000;

    asm volatile ("fmxr fpscr, %0\n"
            :
            :"r"(fpscr));
    asm volatile ("fmrx %0, fpscr\n"
            :"=r"(fpscr));

    vfp_regs_save(farrays2);
    for(i=0; i<32; i++)
        farrays[i] = f1+f2*(float)i;
    vfp_regs_load(farrays);
    vfp_regs_save(farrays2);
    rt_kprintf("\n1:ScalarA op ScalarB->ScalarD");
    vfp_regs_load(farrays);
    asm volatile("fadds s0, s1, s2");
    vfp_regs_save(farrays2);
    print_array(farrays2);
    rt_kprintf("\n2:VectorA[?] op ScalarB->VectorD[?]");
    vfp_regs_load(farrays);
    asm volatile("fadds s8,  s24, s0");
    vfp_regs_save(farrays2);
    print_array(farrays2);
    rt_kprintf("\n3:VectorA[?] op VectorB[?]->VectorD[?]");
    vfp_regs_load(farrays);
    asm volatile("fadds s8,  s16, s24");
    vfp_regs_save(farrays2);
    print_array(farrays2);
}

#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(vfp_test, vfp);
#endif
#endif
