/*
 * File      : media_proc.c
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
#define FH_DGB_ISP_PROC
#define FH_DGB_DSP_PROC

#include "rtthread.h"

#ifdef FH_DGB_DSP_PROC
extern int media_read_proc();
extern int media_mem_proc();
extern int vpu_read_proc();
extern int vpu_write_proc(char *s);
extern int enc_read_proc();
extern int enc_write_proc(char *s);
extern int jpeg_read_proc();
extern int jpeg_write_proc(char *s);
extern int vou_read_proc();
extern int vou_write_proc(char *s);
extern void cmm_mem_proc(int index);

#ifdef RT_USING_FINSH

#include <finsh.h>

#ifdef FH_DGB_ISP_PROC
extern int isp_read_proc();
FINSH_FUNCTION_EXPORT(isp_read_proc, read proc info);
#endif

FINSH_FUNCTION_EXPORT(media_mem_proc, media mem use info.e.g
                      : media_mem_proc());
FINSH_FUNCTION_EXPORT(jpeg_write_proc, write jpeg proc info);
FINSH_FUNCTION_EXPORT(jpeg_read_proc, read jpeg proc info);
FINSH_FUNCTION_EXPORT(media_read_proc, get media process proc info);
FINSH_FUNCTION_EXPORT(enc_write_proc, write enc proc info);
FINSH_FUNCTION_EXPORT(enc_read_proc, read enc proc info);
FINSH_FUNCTION_EXPORT(vou_write_proc, write vou proc info);
FINSH_FUNCTION_EXPORT(vou_read_proc, read vou proc info);
FINSH_FUNCTION_EXPORT(vpu_write_proc, write vpu proc info);
FINSH_FUNCTION_EXPORT(vpu_read_proc, read vpu proc info);
FINSH_FUNCTION_EXPORT(cmm_mem_proc, reserved mem use info.e.g
                      : cmm_mem_proc(index));
#endif

#endif
