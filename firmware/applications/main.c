/*
 *  This file is part of FH8620 BSP for RT-Thread distribution.
 *
 *  Copyright (c) 2016 Shanghai Fullhan Microelectronics Co., Ltd.
 *  All rights reserved
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <msh.h>
#include <rtdef.h>


#ifdef RT_USING_WIFI 
#ifdef PKG_USING_WLAN_WICED

#include "wlan_wiced.h"

int wifi_init(void)
{
    rt_kprintf("wifi init\n");
    wifi_hw_init();
}
INIT_APP_EXPORT(wifi_init);

#endif
#endif


int main(int argc, char** argv)
{
    
	FILE *fp;
    char cmd[] = "sdcard/init.sh";

    fp = fopen(cmd, "rb");
    if (fp >= 0)
    {
        fclose(fp);

        printf("exec init.sh...\n");
        msh_exec(cmd, sizeof(cmd));
    }  
    
    return 0;
}




