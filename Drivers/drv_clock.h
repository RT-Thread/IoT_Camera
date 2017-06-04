/*
 * File      : drv_clock.h
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
 * 2016Äê4ÔÂ11ÈÕ     Urey         the first version
 */

#ifndef DRIVERS_DRV_CLOCK_H_
#define DRIVERS_DRV_CLOCK_H_


#include <rtdef.h>


void rt_hw_clock_init(void);
void rt_hw_get_clock(void);
void rt_hw_set_dividor(rt_uint8_t hdivn, rt_uint8_t pdivn);
void rt_hw_set_clock(rt_uint8_t sdiv, rt_uint8_t pdiv, rt_uint8_t mdiv);

struct fh_clk;
/*
void fh_pmu_open(void);
void fh_pmu_close(void);
*/

#define DIFF_SDC_REFCLK_0			(0)
#define DIFF_SDC_REFCLK_90			(1)
#define DIFF_SDC_REFCLK_180			(2)
#define DIFF_SDC_REFCLK_270			(3)
#define SDC_CLK_PARA_ERROR		(0xffff0000)
#define SDC_CLK_PARA_OK			(0)


void clk_gate(struct fh_clk *p_clk);
void clk_ungate(struct fh_clk *p_clk);
struct fh_clk *clk_get(const char *name);
rt_uint32_t clk_get_rate(struct fh_clk *p_clk);
void clk_set_rate(struct fh_clk *p_clk, rt_uint32_t rate_value);
rt_uint32_t sdc_get_phase(struct fh_clk *p_clk);
rt_uint32_t sdc_set_phase(struct fh_clk *p_clk, rt_uint32_t phase);


#endif /* DRIVERS_DRV_CLOCK_H_ */
