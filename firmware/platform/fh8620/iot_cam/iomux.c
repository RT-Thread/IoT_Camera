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

#include "rtdef.h"
#include "iomux.h"
#include "rtconfig.h"

Iomux_Pad fh_iomux_cfg[] = {
    PAD_MUX(0, -1, -1, RESETN, RSV, RSV, RSV, 9, IOMUX_PUPD_UP, -1),
    PAD_MUX(0, -1, -1, CIS_CLK, RSV, RSV, RSV, 5, IOMUX_PUPD_NONE, 1),
    PAD_MUX(0, 20, 1, CIS_HSYNC, GPIO_20, RSV, RSV, 20, IOMUX_PUPD_DOWN, 1),
    PAD_MUX(0, 21, 1, CIS_VSYNC, GPIO_21, RSV, RSV, 20, IOMUX_PUPD_DOWN, 1),
    PAD_MUX(0, -1, 1, CIS_PCLK, RSV, RSV, RSV, 9, IOMUX_PUPD_DOWN, 0),
    PAD_MUX(0, 22, 1, CIS_D_0, GPIO_22, RSV, RSV, 20, IOMUX_PUPD_DOWN, 1),
    PAD_MUX(0, 23, 1, CIS_D_1, GPIO_23, RSV, RSV, 20, IOMUX_PUPD_DOWN, 1),
    PAD_MUX(0, 24, 1, CIS_D_2, GPIO_24, RSV, RSV, 20, IOMUX_PUPD_DOWN, 1),
    PAD_MUX(0, 25, 1, CIS_D_3, GPIO_25, RSV, RSV, 20, IOMUX_PUPD_DOWN, 1),
    PAD_MUX(0, 26, 1, CIS_D_4, GPIO_26, RSV, RSV, 20, IOMUX_PUPD_DOWN, 1),
    PAD_MUX(0, 27, 1, CIS_D_5, GPIO_27, RSV, RSV, 20, IOMUX_PUPD_DOWN, 1),
    PAD_MUX(0, 28, 1, CIS_D_6, GPIO_28, RSV, RSV, 20, IOMUX_PUPD_DOWN, 1),
    PAD_MUX(0, 29, 1, CIS_D_7, GPIO_29, RSV, RSV, 20, IOMUX_PUPD_DOWN, 1),
    PAD_MUX(0, 30, 1, CIS_D_8, GPIO_30, RSV, RSV, 20, IOMUX_PUPD_DOWN, 1),
    PAD_MUX(0, 31, 1, CIS_D_9, GPIO_31, RSV, RSV, 20, IOMUX_PUPD_DOWN, 1),
    PAD_MUX(0, 32, 1, CIS_D_10, GPIO_32, RSV, RSV, 20, IOMUX_PUPD_DOWN, 1),
    PAD_MUX(0, 33, 1, CIS_D_11, GPIO_33, RSV, RSV, 20, IOMUX_PUPD_DOWN, 1),
    PAD_MUX(0, 34, 1, MAC_MDC, GPIO_34, RSV, RSV, 20, IOMUX_PUPD_NONE, 0),
    PAD_MUX(0, 45, 1, MAC_TXD3, GPIO_45, RSV, RSV, 20, IOMUX_PUPD_DOWN, 1),
    PAD_MUX(0, 47, 1, MAC_RXER, GPIO_47, RSV, RSV, 20, IOMUX_PUPD_DOWN, 1),
    PAD_MUX(1, 5, 1, JTAG_TCK, GPIO_5, RSV, RSV, 20, IOMUX_PUPD_NONE, 1),
    PAD_MUX(1, 6, 1, JTAG_TRSTN, GPIO_6, RSV, RSV, 20, IOMUX_PUPD_NONE, 1),
    PAD_MUX(1, 7, 1, JTAG_TMS, GPIO_7, RSV, RSV, 20, IOMUX_PUPD_DOWN, 1),
    PAD_MUX(1, 8, 1, JTAG_TDI, GPIO_8, RSV, RSV, 20, IOMUX_PUPD_NONE, 1),
    PAD_MUX(1, 9, 1, JTAG_TDO, GPIO_9, RSV, RSV, 20, IOMUX_PUPD_NONE, 1),
    PAD_MUX(1, 10, 0, GPIO_10, UART1_OUT, RSV, RSV, 20, IOMUX_PUPD_UP, 1),
    PAD_MUX(1, 11, 0, GPIO_11, UART1_IN, RSV, RSV, 20, IOMUX_PUPD_UP, 1),
    PAD_MUX(1, 12, 0, GPIO_12, PWM_OUT0, RSV, RSV, 20, IOMUX_PUPD_NONE, 1),
    PAD_MUX(1, 13, 0, GPIO_13, PWM_OUT1, RSV, RSV, 20, IOMUX_PUPD_NONE, 1),
    PAD_MUX(1, 14, 0, GPIO_14, PWM_OUT2, RSV, RSV, 20, IOMUX_PUPD_NONE, 1),
    PAD_MUX(0, 56, 1, CIS_SCL, GPIO_56, CIS_SCL, CIS_SSI0_CLK, 13, IOMUX_PUPD_NONE, 1),
    PAD_MUX(0, 57, 1, CIS_SDA, GPIO_57, CIS_SDA, CIS_SSI0_TXD, 13, IOMUX_PUPD_NONE, 1),
    PAD_MUX(1, 50, 1, SCL1, GPIO_50, SCL1, I2S_DI, 21, IOMUX_PUPD_NONE, 1),
    PAD_MUX(1, 51, 1, SDA1, GPIO_51, I2S_DO, RSV, 21, IOMUX_PUPD_NONE, 1),
    PAD_MUX(0, -1, -1, SSI0_CLK, RSV, RSV, RSV, 5, IOMUX_PUPD_NONE, 1),
    PAD_MUX(0, -1, -1, SSI0_TXD, RSV, RSV, RSV, 5, IOMUX_PUPD_NONE, 1),
    PAD_MUX(1, 54, 1, SSI0_CSN_0, GPIO_54, RSV, RSV, 20, IOMUX_PUPD_DOWN, 1),
    PAD_MUX(1, 55, 1, SSI0_CSN_1, GPIO_55, RSV, RSV, 20, IOMUX_PUPD_DOWN, 1),
    PAD_MUX(0, -1, -1, SSI0_RXD, RSV, RSV, RSV, 17, IOMUX_PUPD_DOWN, -1),
    PAD_MUX(0, 52, 1, SD0_CD, GPIO_52, RSV, RSV, 20, IOMUX_PUPD_DOWN, 1),
    PAD_MUX(0, 53, 1, SD0_WP, GPIO_53, RSV, RSV, 20, IOMUX_PUPD_DOWN, 1),
    PAD_MUX(0, -1, -1, SD0_CLK, RSV, RSV, RSV, 5, IOMUX_PUPD_NONE, 3),
    PAD_MUX(0, -1, -1, SD0_CMD_RSP, RSV, RSV, RSV, 17, IOMUX_PUPD_UP, 3),
    PAD_MUX(0, -1, -1, SD0_DATA_0, RSV, RSV, RSV, 17, IOMUX_PUPD_UP, 3),
    PAD_MUX(0, -1, -1, SD0_DATA_1, RSV, RSV, RSV, 17, IOMUX_PUPD_UP, 2),
    PAD_MUX(0, -1, -1, SD1_CLK, SSI1_CLK, RSV, RSV, 8, IOMUX_PUPD_NONE, 1),
    PAD_MUX(0, 58, 1, SD1_CD, GPIO_58, RSV, RSV, 20, IOMUX_PUPD_DOWN, 1),
    PAD_MUX(0, 59, 1, SD1_WP, GPIO_59, RSV, RSV, 20, IOMUX_PUPD_DOWN, 1),
    PAD_MUX(0, -1, -1, SD1_DATA_0, SSI1_TXD, RSV, RSV, 20, IOMUX_PUPD_UP, 3),
    PAD_MUX(0, -1, -1, SD1_DATA_1, SSI1_CSN0, RSV, RSV, 20, IOMUX_PUPD_UP, 3),
    PAD_MUX(0, -1, -1, SD1_DATA_2, SSI1_CSN1, RSV, RSV, 20, IOMUX_PUPD_UP, 3),
    PAD_MUX(0, -1, -1, SD1_DATA_3, RSV, RSV, RSV, 17, IOMUX_PUPD_UP, 3),
    PAD_MUX(0, -1, -1, SD1_CMD_RSP, SSI1_RXD, RSV, RSV, 20, IOMUX_PUPD_UP, 3),
};

const int fh_iomux_cfg_count = ARRAY_SIZE(fh_iomux_cfg);
