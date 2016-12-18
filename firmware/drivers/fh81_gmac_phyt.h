/*
 * File      : fh81_gmac_phyt.h
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

#ifndef FH81_GMAC_PHYT_H_
#define FH81_GMAC_PHYT_H_

#include "fh_def.h"

#ifdef CONFIG_PHY_TI83848
#define PHYT_ID (0x3)
#define PHYT_OUI (0x32001c)
#else
#define PHYT_ID (0x0)
#define PHYT_OUI (0x300007)
#endif
enum
{
    gmac_phyt_speed_10M_half_duplex  = 1,
    gmac_phyt_speed_100M_half_duplex = 2,
    gmac_phyt_speed_10M_full_duplex  = 5,
    gmac_phyt_speed_100M_full_duplex = 6
};

enum
{
    gmac_phyt_reg_basic_ctrl       = 0,
    gmac_phyt_reg_basic_status     = 1,
    gmac_phyt_reg_phy_id1          = 2,
    gmac_phyt_reg_phy_id2          = 3,
    gmac_phyt_rtl8201_rmii_mode    = 16,
    gmac_phyt_ti83848_rmii_mode    = 17,
    gmac_phyt_ip101g_page_select   = 20,
    gmac_phyt_rtl8201_power_saving = 24,
    gmac_phyt_rtl8201_page_select  = 31
};

typedef union {
    struct
    {
        UINT32 reserved_6_0 : 7;
        UINT32 collision_test : 1;
        UINT32 duplex_mode : 1;
        UINT32 restart_auto_negotiate : 1;
        UINT32 isolate : 1;
        UINT32 power_down : 1;
        UINT32 auto_negotiate_enable : 1;
        UINT32 speed_select : 1;
        UINT32 loopback : 1;
        UINT32 reset : 1;
        UINT32 reserved_31_16 : 16;
    } bit;
    UINT32 dw;
} Reg_Phyt_Basic_Ctrl;

typedef union {
    struct
    {
        UINT32 extended_capabilities : 1;
        UINT32 jabber_detect : 1;
        UINT32 link_status : 1;
        UINT32 auto_negotiate_ability : 1;
        UINT32 remote_fault : 1;
        UINT32 auto_negotiate_complete : 1;
        UINT32 reserved_10_6 : 5;
        UINT32 base_t_half_duplex_10 : 1;
        UINT32 base_t_full_duplex_10 : 1;
        UINT32 base_tx_half_duplex_100 : 1;
        UINT32 base_tx_full_duplex_100 : 1;
        UINT32 base_t_4 : 1;
        UINT32 reserved_31_16 : 16;
    } bit;
    UINT32 dw;
} Reg_Phyt_Basic_Status;

typedef union {
    struct
    {
        UINT32 scramble_disable : 1;
        UINT32 reserved_1 : 1;
        UINT32 speed_indication : 3;
        UINT32 reserved_5 : 1;
        UINT32 enable_4b5b : 1;
        UINT32 gpo : 3;
        UINT32 reserved_11_10 : 2;
        UINT32 auto_done : 1;
        UINT32 reserved_31_13 : 19;
    } bit;
    UINT32 dw;
} Reg_Phyt_Special_Status;

#endif /* FH81_GMAC_PHYT_H_ */
