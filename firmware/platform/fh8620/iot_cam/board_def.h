/*
 * File      : board_def.h
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

#ifndef BOARD_DEF_H_
#define BOARD_DEF_H_

#define RT_USING_CALIBRATE
// #define RT_USING_VFP

/* ***********************
 * SECTION:	DRIVE
 * ***********************/
// Basic drive..

#ifndef FH_DDR_START
#define FH_DDR_START 0xA0000000
#define FH_DDR_END 0xA1000000

#define FH_RTT_OS_MEM_SIZE 0x00600000
#define FH_DMA_MEM_SIZE 0x20000 /* 128k */

#define FH_RTT_OS_MEM_END (FH_DDR_START + FH_RTT_OS_MEM_SIZE)
#define FH_SDK_MEM_START (FH_RTT_OS_MEM_END + FH_DMA_MEM_SIZE)
#define FH_RTT_OS_HEAP_END FH_SDK_MEM_START
#define FH_SDK_MEM_SIZE (FH_DDR_END - FH_SDK_MEM_START)

#define MMC_USE_INTERNAL_BUF
#define MMC_INTERNAL_DMA_BUF_SIZE (16 * 1024)
#endif
/* ***********************
 * SECTION:	DRIVE COMPONENT
 * ***********************/
#define UART_NAME "uart1"
#define RT_USING_DMA_MEM

#define RT_USING_MCI0
#define RT_USING_GD
#define RT_USING_FLASH_DEFAULT
#define RT_USING_FH_FLASH_ADAPT

#endif /* BOARD_H_ */
