/*
 * File      : board.c
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

/*****************************************************************************
 *  Include Section
 *  add all #include here
 *****************************************************************************/

#include <rthw.h>
#include <rtthread.h>
#include <mmu.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "partition.h"

#ifdef RT_USING_DMA_MEM
#include "dma_mem.h"
#endif

#include "board_def.h"

#include <mmu.h>
#include "fh_def.h"
#include "arch.h"
#include "board_info.h"
#include "inc/fh_driverlib.h"
#include "iomux.h"
#include "fh_pmu.h"
#include <calibrate.h>
#include <clock.h>
#include <uart.h>
#include <timer.h>
#include <dfs_fs.h>
#include <libc.h>
#include <drv_mmc.h>

#ifdef RT_USING_TIMEKEEPING
#include <timekeeping.h>
#endif

#ifdef RT_USING_SADC
#include "sadc.h"
#endif

#ifdef RT_USING_SPI
#include "ssi.h"
#include <spi_flash.h>
#include <spi_flash_sfud.h>
#endif

#ifdef RT_USING_ENC28J60
#include "enc28j60.h"
#include "gpio.h"
#endif

#ifdef RT_USING_GPIO
#include "gpio.h"
#endif

#ifdef RT_USING_DFS
#include "dfs.h"

#if defined(RT_USING_DFS_ELMFAT)
#include "dfs_elm.h"
#endif

#if defined(RT_USING_DFS_DEVFS)
#include "devfs.h"
#endif

#ifdef RT_USING_DFS_RAMFS
#include "dfs_ramfs.h"
#endif // RT_USING_DFS_RAMFS

#ifdef RT_USING_DFS_ROMFS
#include "dfs_romfs.h"
#endif

#endif // RT_USING_DFS

#ifdef RT_USING_FH_ACW
#include "acw.h"
#endif

#ifdef RT_USING_LWIP
#include "lwip/sys.h"
#include "netif/ethernetif.h"
#endif

#ifdef RT_USING_GMAC
#include "fh_gmac.h"
#endif

#ifdef RT_USING_I2C
#include "i2c.h"
#endif

#ifdef RT_USING_PWM
#include "pwm.h"
#endif

#ifdef RT_USING_WDT
#include "wdt.h"
#endif

#ifdef RT_USING_DFS_JFFS2
#include "dfs_jffs2.h"
#endif

#ifndef HW_WIFI_POWER_GPIO
#define HW_WIFI_POWER_GPIO 47  // wifi power on
#endif
#ifndef HW_WIFI_POWER_GPIO_ON_LEVEL
#define HW_WIFI_POWER_GPIO_ON_LEVEL 0
#endif

#ifndef HW_CIS_RST_GPIO
#define HW_CIS_RST_GPIO 45  // cis(sensor) reset
#endif
#ifndef HW_CIS_RST_GPIO_LEVEL
#define HW_CIS_RST_GPIO_LEVEL 1
#endif

#ifndef HW_SDCARD_POWER_GPIO
#define HW_SDCARD_POWER_GPIO 63  // not used
#endif

#ifndef SPI_CRTOLLER0_SLAVE0_CS
#define SPI_CRTOLLER0_SLAVE0_CS (54)
#endif
#ifndef SPI_CRTOLLER0_SLAVE1_CS
#define SPI_CRTOLLER0_SLAVE1_CS (55)
#endif
#ifndef SPI_CRTOLLER1_SLAVE0_CS
#define SPI_CRTOLLER1_SLAVE0_CS (54)
#endif
#ifndef SPI_CRTOLLER1_SLAVE1_CS
#define SPI_CRTOLLER1_SLAVE1_CS (55)
#endif

#ifndef SPI0_TRANSFER_MODE
#define SPI0_TRANSFER_MODE USE_DMA_TRANSFER
#endif
#ifndef SPI1_TRANSFER_MODE
#define SPI1_TRANSFER_MODE USE_ISR_TRANSFER
#endif

/****************************************************************************
 * ADT section
 *  add definition of user defined Data Type that only be used in this file here
 ***************************************************************************/
struct st_platform_info
{
    char *name;
    void *private_data;
};

/******************************************************************************
 * Function prototype section
 * add prototypes for all functions called by this file,execepting those
 * declared in header file
 *****************************************************************************/

/*****************************************************************************
 * Global variables section - Exported
 * add declaration of global variables that will be exported here
 * e.g.
 *  int8_t foo;
 ****************************************************************************/

void do_gettimeofday(struct timeval *tv)
{
    if (tv)
    {
        tv->tv_sec  = rt_tick_get() / RT_TICK_PER_SECOND;
        tv->tv_usec = 0;
    }
}

/*****************************************************************************
 * Global variables section - Local
 * define global variables(will be refered only in this file) here,
 * static keyword should be used to limit scope of local variable to this file
 * e.g.
 *  static uint8_t ufoo;
 *****************************************************************************/

void fh_mmc_reset(struct fh_mmc_obj *mmc_obj)
{
    rt_uint32_t value;
    if (mmc_obj->id)
        fh_pmu_write(REG_PMU_SWRST_AHB_CTRL, 0xfffffffd);
    else
        fh_pmu_write(REG_PMU_SWRST_AHB_CTRL, 0xfffffffb);
    do
    {
        fh_pmu_read(REG_PMU_SWRST_AHB_CTRL, &value);
    } while (value != 0xffffffff);
}

static struct fh_mmc_obj mmc0_obj = {
    .id             = 0,
    .irq            = SDC0_IRQn,
    .base           = SDC0_REG_BASE,
    .power_pin_gpio = HW_SDCARD_POWER_GPIO,
    .mmc_reset      = fh_mmc_reset,
};

static struct fh_mmc_obj mmc1_obj = {
    .id             = 1,
    .irq            = SDC1_IRQn,
    .base           = SDC1_REG_BASE,
    .power_pin_gpio = HW_WIFI_POWER_GPIO,
    .mmc_reset      = fh_mmc_reset,
};

#ifdef RT_USING_SPI
#define SPI0_CLK_IN (50000000)
#define SPI0_MAX_BAUD (SPI0_CLK_IN / 2)

static struct spi_control_platform_data spi0_platform_data = {
    .id                         = 0,
    .irq                        = SPI0_IRQn,
    .base                       = SPI0_REG_BASE,
    .max_hz                     = SPI0_MAX_BAUD,
    .slave_no                   = FH_SPI_SLAVE_MAX_NO,
    .clk_in                     = SPI0_CLK_IN,
    .rx_hs_no                   = SPI0_RX,
    .tx_hs_no                   = SPI0_TX,
    .dma_name                   = "fh81_dma",
    .transfer_mode              = SPI0_TRANSFER_MODE,
    .plat_slave[0].cs_pin       = SPI_CRTOLLER0_SLAVE0_CS,
    .plat_slave[0].actice_level = ACTIVE_LOW,
    .plat_slave[1].cs_pin       = SPI_CRTOLLER0_SLAVE1_CS,
    .plat_slave[1].actice_level = ACTIVE_LOW,
};

#define SPI1_CLK_IN (50000000)
#define SPI1_MAX_BAUD (SPI1_CLK_IN / 2)

static struct spi_control_platform_data spi1_platform_data = {
    .id                         = 1,
    .irq                        = SPI1_IRQn,
    .base                       = SPI1_REG_BASE,
    .max_hz                     = SPI1_MAX_BAUD,
    .slave_no                   = FH_SPI_SLAVE_MAX_NO,
    .clk_in                     = SPI1_CLK_IN,
    .rx_hs_no                   = SPI1_RX,
    .tx_hs_no                   = SPI1_TX,
    .dma_name                   = "fh81_dma",
    .transfer_mode              = SPI1_TRANSFER_MODE,
    .plat_slave[0].cs_pin       = SPI_CRTOLLER1_SLAVE0_CS,
    .plat_slave[0].actice_level = ACTIVE_LOW,
    .plat_slave[1].cs_pin       = SPI_CRTOLLER1_SLAVE1_CS,
    .plat_slave[1].actice_level = ACTIVE_LOW,
};

static rt_spi_flash_device_t gd25q128;
#endif

static struct fh_i2c_obj i2c0_obj = {
    .id = 0, .irq = I2C0_IRQn, .base = I2C0_REG_BASE,
};

static struct fh_i2c_obj i2c1_obj = {
    .id = 1, .irq = I2C1_IRQn, .base = I2C1_REG_BASE,
};

static struct fh_gpio_obj gpio0_obj = {
    .id = 0, .irq = GPIO0_IRQn,
};

static struct fh_gpio_obj gpio1_obj = {
    .id = 1, .irq = GPIO1_IRQn,
};

static struct fh_pwm_obj pwm_obj = {
    .id = 0, .base = PWM_REG_BASE,
};

static struct fh_wdt_obj wdt_obj = {
    .id = 0, .base = WDT_REG_BASE, .irq = WDT_IRQn,
};

#ifdef RT_USING_SADC
static struct wrap_sadc_obj sadc_obj = {
    .id          = 0,
    .regs        = (void *)SADC_REG_BASE,
    .irq_no      = SADC_IRQn,
    .sample_mode = ISR_MODE,
};
#endif

struct st_platform_info plat_mmc0 = {
    .name = "mmc", .private_data = &mmc0_obj,
};

struct st_platform_info plat_mmc1 = {
    .name = "mmc", .private_data = &mmc1_obj,
};
#ifdef RT_USING_SPI
struct st_platform_info plat_spi0 = {
    .name = "spi", .private_data = &spi0_platform_data,
};

struct st_platform_info plat_spi1 = {
    .name = "spi", .private_data = &spi1_platform_data,
};
#endif

struct st_platform_info plat_i2c0 = {
    .name = "i2c", .private_data = &i2c0_obj,
};

struct st_platform_info plat_i2c1 = {
    .name = "i2c", .private_data = &i2c1_obj,
};

struct st_platform_info plat_gpio0 = {
    .name = "gpio", .private_data = &gpio0_obj,
};

struct st_platform_info plat_gpio1 = {
    .name = "gpio", .private_data = &gpio1_obj,
};

struct st_platform_info plat_pwm = {
    .name = "pwm", .private_data = &pwm_obj,
};

struct st_platform_info plat_wdt = {
    .name = "wdt", .private_data = &wdt_obj,
};
#ifdef RT_USING_SADC
struct st_platform_info plat_sadc = {
    .name = "sadc", .private_data = &sadc_obj,
};
#endif

const static struct st_platform_info *platform_info[] = {
    &plat_mmc0,
//&plat_mmc1,//by PeterJiang, wifi don't use SDIO framework...
#ifdef RT_USING_SPI
    &plat_spi0,
#endif
#ifdef RT_USING_SPI1
    &plat_spi1,
#endif
    &plat_i2c0, &plat_i2c1, &plat_gpio0,
    &plat_gpio1, &plat_pwm,  &plat_wdt,
#ifdef RT_USING_SADC
    &plat_sadc,
#endif
};

static const struct rt_partition partitions[] =
{
    {
        /* head & Ramboot */
        .name       = "bootstrap",
        .offset     = 0,
        .size       = 0x4000,
        .flags = PARTITION_RDONLY,
    }, {
        /* isp param */
        .name       = "isp-param",
        .offset     = 0x4000,
        .size       = 0x4000,
        .flags = PARTITION_RDONLY, /* force read-only */
    }, {
        /* pae param */
        .name       = "pae-param",
        .offset     = 0x8000,
        .size       = 0x8000,
        .flags = PARTITION_RDONLY, /* force read-only */
    }, {
        /* Uboot SPL */
        .name       = "uboot-spl",
        .offset     = 0x10000,
        .size       = 0x10000,
        .flags = PARTITION_RDONLY, /* force read-only */
    }, {
        /* U-Boot environment */
        .name       = "uboot-env",
        .offset     = 0x20000,
        .size       = 0x10000,
        .flags = PARTITION_WRITEABLE, /* force read-only */
    }, {
        /* U-Boot */
        .name       = "uboot",
        .offset     = 0x30000,
        .size       = 0x30000,
        .flags = PARTITION_RDONLY, /* force read-only */
    }, {
        .name       = "kernel",     /* 4MB, kernel */
        .offset     = 0x400000,
        .size       = 0x400000,
        .flags = PARTITION_WRITEABLE,
    }, {
        .name       = "rootfs",     /* 8MB, rootfs */
        .offset     = 0x800000,
        .size       = 0x800000,
        .flags = PARTITION_WRITEABLE,
    }, {
        .name       = NULL,
        .offset     = 0,
        .size       = 0,
        .flags = 0,
    }
};

 /* function body */

/*****************************************************************************
 * Description:
 *      add funtion description here
 * Parameters:
 *      description for each argument, new argument starts at new line
 * Return:
 *      what does this function returned?
 *****************************************************************************/

void clock_init(void)
{
// UINT32 reg;
// gate enable, spi0, gmac, uart0, timer0, wdt, pts
#ifdef CONFIG_RMII
    fh_pmu_write_mask(REG_PMU_PAD_MAC_TXER_CFG, 0x100000, 0x100000);
#endif
    // SPI0
    fh_pmu_write_mask(REG_PMU_CLK_DIV3, 0xb, 0xff);

    // GMAC
    fh_pmu_write_mask(REG_PMU_CLK_DIV6, 0x5000000, 0xf000000);

    // UART0
    fh_pmu_write_mask(REG_PMU_CLK_DIV4, 0x1, 0xf);

    // TIMER0
    fh_pmu_write_mask(REG_PMU_CLK_DIV5, 0x1d0000, 0x3f0000);

    // PTS
    fh_pmu_write_mask(REG_PMU_CLK_DIV2, 0x23, 0x3f);

    // WDT
    // fh_pmu_write_mask(REG_PMU_CLK_DIV5, 0x1d00, 0x3f00);
    fh_pmu_write_mask(REG_PMU_CLK_DIV5, 0x3500, 0x3f00);

    // clock enable
    fh_pmu_write_mask(REG_PMU_CLK_GATE, 0, 0x720ba080);

    // sd0_drv_sel
    fh_pmu_write_mask(REG_PMU_CLK_SEL, 0x200000, 0x300000);
    // sd0_sample_sel
    fh_pmu_write_mask(REG_PMU_CLK_SEL, 0x00000, 0x30000);

    // sd1_drv_sel
    fh_pmu_write_mask(REG_PMU_CLK_SEL, 0x2000, 0x3000);
    // sd1_sample_sel
    fh_pmu_write_mask(REG_PMU_CLK_SEL, 0x000, 0x300);
}

void fh_platform_info_register(void)
{
    struct fh_board_info *test_info;
    int i;

    for (i = 0; i < sizeof(platform_info) / sizeof(struct st_platform_info *);
         i++)
    {
        test_info = fh_board_info_register(platform_info[i]->name,
                                           platform_info[i]->private_data);
        if (!test_info)
        {
            rt_kprintf("info_name(%s) failed registered\n",
                       platform_info[i]->name);
        }
    }
}


static struct mem_desc fh_mem_desc[] = {

    {0xA0000000, FH_RTT_OS_MEM_END - 1, 0xA0000000, SECT_RWX_CB, 0,
     SECT_MAPPED},
    {FH_RTT_OS_MEM_END, FH_DDR_END - 1, FH_RTT_OS_MEM_END, SECT_RWNX_NCNB, 0,
     SECT_MAPPED},
    {0xFFFF0000, 0xFFFF1000 - 1, 0xA0000000, SECT_TO_PAGE, PAGE_ROX_CB,
     PAGE_MAPPED}, /* isr vector table */
    {0xE0000000, 0xF1300000 - 1, 0xE0000000, SECT_RWNX_NCNB, 0,
     SECT_MAPPED}, /* io table */
    {0xF4000000, 0xF4100000 - 1, 0xF4000000, SECT_RWNX_NCNB, 0,
     SECT_MAPPED}, /* GPIO#1 io table */
};


rt_uint8_t _irq_stack_start[1024];
rt_uint8_t _fiq_stack_start[512];
rt_uint8_t _undefined_stack_start[512];
rt_uint8_t _abort_stack_start[512];
rt_uint8_t _svc_stack_start[4096] SECTION(".nobss");
extern unsigned char __bss_start;
extern unsigned char __bss_end;

int rt_hw_board_init()
{
    /* disable interrupt first */
    rt_hw_interrupt_disable();
    /* initialize hardware interrupt */
    rt_hw_interrupt_init();

    /* initialize mmu */
    rt_hw_mmu_init(fh_mem_desc, sizeof(fh_mem_desc) / sizeof(fh_mem_desc[0]));

    rt_system_heap_init((void *)&__bss_end, (void *)FH_RTT_OS_MEM_END);

#ifdef RT_USING_DMA_MEM
    // use the last 100KB
    fh_dma_mem_init((rt_uint32_t *)FH_RTT_OS_MEM_END, FH_DMA_MEM_SIZE);
#endif

    /* initialize the system clock */
    rt_hw_clock_init();
    fh_iomux_init(PMU_REG_BASE + 0x5c);

    clock_init();
    /* initialize uart */
    rt_hw_uart_init();

    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);

    /* initialize timer1 */
    rt_hw_timer_init();
    // board data info init...
    fh_board_info_init();
    fh_platform_info_register();

    return 0;
}

int rt_board_driver_init()
{
#ifdef RT_USING_CALIBRATE
    calibrate_delay();
#endif

#ifdef RT_USING_FH_DMA
    rt_fh_dma_init();
#endif

#ifdef RT_USING_TIMEKEEPING
    {
        timekeeping_init();
        clocksource_pts_register();
        clockevent_timer0_register();
        hrtimers_init();
    }
#endif

#if defined(RT_USING_DFS) && defined(RT_USING_DFS_DEVFS)
    dfs_mount(RT_NULL, "/dev", "devfs", 0, 0);
#endif

#ifdef RT_USING_GPIO
    {
        rt_hw_gpio_init();

#ifdef RT_USING_SDIO
        // wifi
        fh_select_gpio(HW_WIFI_POWER_GPIO);
        gpio_request(HW_WIFI_POWER_GPIO);
        gpio_direction_output(HW_WIFI_POWER_GPIO, !HW_WIFI_POWER_GPIO_ON_LEVEL);
        // udelay(1000);
        rt_thread_delay(rt_tick_from_millisecond(500));
        gpio_direction_output(HW_WIFI_POWER_GPIO, HW_WIFI_POWER_GPIO_ON_LEVEL);
        // micro sd
        fh_select_gpio(HW_SDCARD_POWER_GPIO);
        gpio_request(HW_SDCARD_POWER_GPIO);
        gpio_direction_output(HW_SDCARD_POWER_GPIO, 0);
#endif
        // sensor
        fh_select_gpio(HW_CIS_RST_GPIO);
        gpio_request(HW_CIS_RST_GPIO);
        gpio_direction_output(HW_CIS_RST_GPIO, HW_CIS_RST_GPIO_LEVEL);
    }
#endif

#ifdef RT_USING_SPI
    {
        rt_hw_spi_init();
        gd25q128 = rt_sfud_flash_probe("flash", "ssi0_0");

        rt_partition_init("flash", &partitions[0]);
        if (dfs_mount("rootfs", "/", "elm", 0, 0) == 0)
        {
            rt_kprintf("root file system initialized!\n");
        }
    }
#endif

#ifdef RT_USING_SDIO
#ifndef RT_USING_WIFI_MARVEL
    rt_mmcsd_blk_init();

    rt_hw_mmc_init();

    rt_thread_delay(RT_TICK_PER_SECOND*1);

    /* mount sd card fat partition 1 as root directory */
    #ifdef RT_USING_DFS_ELMFAT
        if (dfs_mount("sd0", "/sdcard", "elm", 0, 0) == 0)
        {
            rt_kprintf("sdcard file system initialized!\n");
        }
        else
        {
            rt_kprintf("sdcard file system initialization failed! errno=%d\n", errno);
        }
    #endif
#endif
#endif

#ifdef RT_USING_FH_ACW
    fh_audio_init();
#endif

#ifdef RT_USING_I2C
    rt_hw_i2c_init();
#endif

#ifdef RT_USING_PWM
    rt_hw_pwm_init();
#endif

#ifdef RT_USING_WDT
    rt_hw_wdt_init();
#endif

#ifdef RT_USING_SADC
    rt_hw_sadc_init();
#endif

#ifdef RT_USING_DSP
    {
        extern void fh_media_process_module_init();
        extern void fh_pae_module_init();
        extern void fh_vpu_module_init();
        extern void fh_jpeg_module_init();
        extern void fh_vou_module_init();

        fh_media_process_module_init();
        fh_pae_module_init();
        fh_vpu_module_init();
        fh_jpeg_module_init();
        fh_vou_module_init();
    }
#endif

#ifdef RT_USING_ISP
    {
        extern void fh_isp_module_init();
        struct fh_clk *clk;

        clk = (struct fh_clk *)clk_get("cis_clk_out");
        if (!clk)
        {
            rt_kprintf("isp set sensor clk failed\n");
        }
        else
        {
            clk_set_rate(clk, 27000000);
        }
        fh_isp_module_init();
    }
#endif

    return 0;
}
INIT_ENV_EXPORT(rt_board_driver_init);
