/*
 * File      : fh_gmac.c
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
#include <rtthread.h>
#include <rthw.h>
#include <rtdevice.h>
#include <netif/ethernetif.h>
#include "lwipopts.h"
#include "fh_arch.h"
#include "fh_def.h"
#include "fh81_gmac_phyt.h"
#include "fh81_gmac_dma.h"
#include "mmu.h"
#include "fh_gmac.h"
#include "mii.h"
#ifdef RT_USING_GMAC
/*****************************************************************************
 * Define section
 * add all #define here
 *****************************************************************************/
#define GMAC_DEBUG_PRINT
#if defined(GMAC_DEBUG_PRINT) && defined(RT_DEBUG)

#define GMAC_DEBUG(fmt, args...) rt_kprintf(fmt, ##args);
#else
#define GMAC_DEBUG(fmt, args...)
#endif

#define EMAC_CACHE_INVALIDATE(addr, size) \
    mmu_invalidate_dcache((rt_uint32_t)addr, size)
#define EMAC_CACHE_WRITEBACK(addr, size) \
    mmu_clean_dcache((rt_uint32_t)addr, size)
#define EMAC_CACHE_WRITEBACK_INVALIDATE(addr, size) \
    mmu_clean_invalidated_dcache((rt_uint32_t)addr, size)

/* EMAC has BD's in cached memory - so need cache functions */
#define BD_CACHE_INVALIDATE(addr, size) \
    mmu_invalidate_dcache((rt_uint32_t)addr, size)
#define BD_CACHE_WRITEBACK(addr, size) mmu_clean_dcache((rt_uint32_t)addr, size)

extern void mmu_clean_invalidated_dcache(rt_uint32_t buffer, rt_uint32_t size);
#define BD_CACHE_WRITEBACK_INVALIDATE(addr, size) \
    mmu_clean_invalidated_dcache((rt_uint32_t)addr, size)

#define DUPLEX_HALF 0x00
#define DUPLEX_FULL 0x01

int g_mac_error_times = 0;
/*
 struct gmac_tx_desc_list {
	//struct fh_board_info obj;
	Gmac_Tx_DMA_Descriptors *tx_desc;
	rt_list_t list;
};


#define CHECK_TEST_LIST_EMPTY		\
		if(rt_list_isempty(&gmac->tx_desc_queue.list)) \
		rt_kprintf("tx queue is empty..\n")



#define list_for_each_entry_safe(pos, n, head, member)			\
	for (pos = rt_list_entry((head)->next, typeof(*pos), member),	\
		n = rt_list_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = rt_list_entry(n->member.next, typeof(*n), member))

 GMAC-MAC
#define		REG_GMAC_CONFIG				(GMAC_REG_BASE +
0x0000)*/
#define REG_GMAC_CONFIG (0x0000)
#define REG_GMAC_FRAME_FILTER (0x0004)
#define REG_GMAC_HASH_HIGH (0x0008)
#define REG_GMAC_HASH_LOW (0x000C)
#define REG_GMAC_GMII_ADDRESS (0x0010)
#define REG_GMAC_GMII_DATA (0x0014)
#define REG_GMAC_DEBUG (0x0024)
#define REG_GMAC_MAC_HIGH (0x0040)
#define REG_GMAC_MAC_LOW (0x0044)
// GMAC-DMA
#define REG_GMAC_BUS_MODE (0x1000)
#define REG_GMAC_TX_POLL_DEMAND (0x1004)
#define REG_GMAC_RX_POLL_DEMAND (0x1008)
#define REG_GMAC_RX_DESC_ADDR (0x100C)
#define REG_GMAC_TX_DESC_ADDR (0x1010)
#define REG_GMAC_STATUS (0x1014)
#define REG_GMAC_OP_MODE (0x1018)
#define REG_GMAC_INTR_EN (0x101C)
#define REG_GMAC_ERROR_COUNT (0x1020)
#define REG_GMAC_AXI_BUS_MODE (0x1028)
#define REG_GMAC_AXI_STATUS (0x102C)
#define REG_GMAC_CURR_TX_DESC (0x1048)
#define REG_GMAC_CURR_RX_DESC (0x104C)

#define GMAC_TIMEOUT_AUTODONE (200000)  // 2s
#define GMAC_TIMEOUT_PHYLINK (200000)   // 2s
#define GMAC_TIMEOUT_RECV (100000)      // 1s
#define GMAC_TIMEOUT_SEND (100000)      // 1s

#define GMAC_EACH_DESC_MAX_TX_SIZE (2048)
#define GMAC_EACH_DESC_MAX_RX_SIZE (2048)

#define NORMAL_INTERRUPT_ENABLE (1 << 16)
#define ABNORMAL_INTERRUPT_ENABLE (1 << 15)
#define ERE_ISR (1 << 14)
#define FBE_ISR (1 << 13)
#define ETE_ISR (1 << 10)
#define RWE_ISR (1 << 9)
#define RSE_ISR (1 << 8)
#define RUE_ISR (1 << 7)
#define RIE_ISR (1 << 6)
#define UNE_ISR (1 << 5)
#define OVE_ISR (1 << 4)
#define TJE_ISR (1 << 3)
#define TUE_ISR (1 << 2)
#define TSE_ISR (1 << 1)
#define TIE_ISR (1 << 0)

#define MAC_RX_ENABLE_POS (1 << 2)
#define MAC_TX_ENABLE_POS (1 << 3)

#ifdef CONFIG_CHIP_FH8620

#define GMAC_TX_RING_SIZE 30
#define GMAC_TX_BUFFER_SIZE 2048
#define GMAC_RX_BUFFER_SIZE 2048
#define GMAC_RX_RING_SIZE 30

#else
#define GMAC_TX_RING_SIZE 120
#define GMAC_TX_BUFFER_SIZE 2048
#define GMAC_RX_BUFFER_SIZE 2048
#define GMAC_RX_RING_SIZE 120

#endif

#define GMAC_WORK_QUEUE_STACK_SIZE 512
#define GMAC_WORK_QUEUE_PRIORITY 12

enum tx_dma_irq_status
{
    tx_hard_error         = 1,
    tx_hard_error_bump_tc = 2,
    handle_tx_rx          = 3,
};

enum rx_frame_status
{
    good_frame    = 0,
    discard_frame = 1,
    csum_none     = 2,
    llc_snap      = 4,
};

enum
{
    gmac_gmii_clock_60_100,
    gmac_gmii_clock_100_150,
    gmac_gmii_clock_20_35,
    gmac_gmii_clock_35_60,
    gmac_gmii_clock_150_250,
    gmac_gmii_clock_250_300
};

enum
{
    gmac_interrupt_all  = 0x0001ffff,
    gmac_interrupt_none = 0x0
};

enum
{
    gmac_mii,
    gmac_rmii
};

/****************************************************************************
 * ADT section
 *  add definition of user defined Data Type that only be used in this file here
 ***************************************************************************/
typedef struct fh_gmac_stats
{
    /* Transmit errors */
    unsigned long tx_underflow;
    unsigned long tx_carrier;
    unsigned long tx_losscarrier;
    unsigned long tx_heartbeat;
    unsigned long tx_deferred;
    unsigned long tx_vlan;
    unsigned long tx_jabber;
    unsigned long tx_frame_flushed;
    unsigned long tx_payload_error;
    unsigned long tx_ip_header_error;
    /* Receive errors */
    unsigned long rx_desc;
    unsigned long rx_partial;
    unsigned long rx_runt;
    unsigned long rx_toolong;
    unsigned long rx_collision;
    unsigned long rx_crc;
    unsigned long rx_length;
    unsigned long rx_mii;
    unsigned long rx_multicast;
    unsigned long rx_gmac_overflow;
    unsigned long rx_watchdog;
    unsigned long da_rx_filter_fail;
    unsigned long sa_rx_filter_fail;
    unsigned long rx_missed_cntr;
    unsigned long rx_overflow_cntr;
    /* Tx/Rx IRQ errors */
    unsigned long tx_undeflow_irq;
    unsigned long tx_process_stopped_irq;
    unsigned long tx_jabber_irq;
    unsigned long rx_overflow_irq;
    unsigned long rx_buf_unav_irq;
    unsigned long rx_process_stopped_irq;
    unsigned long rx_watchdog_irq;
    unsigned long tx_early_irq;
    unsigned long fatal_bus_error_irq;
    /* Extra info */
    unsigned long threshold;
    unsigned long tx_pkt_n;
    unsigned long rx_pkt_n;
    unsigned long poll_n;
    unsigned long sched_timer_n;
    unsigned long normal_irq_n;
    unsigned long tx_errors;
} fh_gmac_stats_t;

#define MAX_ADDR_LEN 6
typedef struct fh_gmac_object
{
    /* inherit from ethernet device */
    struct eth_device parent;

    void* p_base_add;
    UINT32 f_base_add;
    UINT8 local_mac_address[MAX_ADDR_LEN];
    unsigned short phy_addr;
    int full_duplex;  // read only
    int speed_100m;   // read only

    // Added by PeterJiang
    UINT8* rx_ring_original;
    UINT8* tx_ring_original;
    UINT8* rx_buffer_original;
    UINT8* tx_buffer_original;

    UINT8* rx_buffer;
    UINT8* tx_buffer;

    Gmac_Rx_DMA_Descriptors* rx_ring;
    Gmac_Tx_DMA_Descriptors* tx_ring;

    unsigned long rx_buffer_dma;
    unsigned long tx_buffer_dma;
    unsigned long rx_ring_dma;
    unsigned long tx_ring_dma;

    unsigned int tx_stop;

    struct rt_semaphore tx_lock;
    struct rt_semaphore rx_lock;
    struct rt_semaphore tx_ack;
    struct rt_semaphore rx_ack;
    struct rt_semaphore mdio_bus_lock;
    int speed;
    int duplex;
    int link;
    int phy_interface;
    struct rt_timer timer;
    struct rt_timer rx_poll_timer;

    fh_gmac_stats_t stats;

    unsigned int rx_cur_desc;
    unsigned int tx_cur_desc;
    unsigned int get_frame_no;
    struct rt_workqueue* rx_queue;
    struct rt_work* rx_work;
    // struct gmac_tx_desc_list tx_desc_queue;

} fh_gmac_object_t;

struct fh_gmac_error_status
{
    UINT32 rx_buff_unavail;
    UINT32 rx_over_flow;
    UINT32 rx_process_stop;
    UINT32 tx_buff_unavail;
    UINT32 tx_under_flow;
    UINT32 tx_over_write_desc;
    UINT32 other_error;
    UINT32 rtt_malloc_failed;
};

struct fh_gmac_error_status g_error_rec = {0};

void tx_desc_init(fh_gmac_object_t* gmac);
void rx_desc_init(fh_gmac_object_t* gmac);

/******************************************************************************
 * Function prototype section
 * add prototypes for all functions called by this file,execepting those
 * declared in header file
 *****************************************************************************/
// extern void mmu_clean_dcache(rt_uint32_t buffer, rt_uint32_t size);
// extern void mmu_invalidate_dcache(rt_uint32_t buffer, rt_uint32_t size);

/*****************************************************************************
 * Global variables section - Exported
 * add declaration of global variables that will be exported here
 * e.g.
 *  int8_t foo;
 ****************************************************************************/

/*****************************************************************************
 * Global variables section - Local
 * define global variables(will be refered only in this file) here,
 * static keyword should be used to limit scope of local variable to this file
 * e.g.
 *  static uint8_t ufoo;
 *****************************************************************************/

/* function body */
/*****************************************************************************
 * Description:
 *      add funtion description here
 * Parameters:
 *      description for each argument, new argument starts at new line
 * Return:
 *      what does this function returned?
 *****************************************************************************/

// static Gmac_Rx_DMA_Descriptors *fh_gmac_get_rx_desc(fh_gmac_object_t* gmac);

static Gmac_Tx_DMA_Descriptors* fh_gmac_get_tx_desc(fh_gmac_object_t* gmac,
                                                    rt_uint32_t frame_len);
static rt_uint32_t fh_gmac_get_rx_desc_no(fh_gmac_object_t* gmac);
static Gmac_Rx_DMA_Descriptors* fh_gmac_get_rx_desc(fh_gmac_object_t* gmac);
/*****************************************************************************
 * Description:
 *      add funtion description here
 * Parameters:
 *      description for each argument, new argument starts at new line
 * Return:
 *      what does this function returned?
 *****************************************************************************/

void fh_gmac_isr_set(fh_gmac_object_t* gmac, rt_uint32_t bit)
{
    rt_uint32_t isr_ret;
    isr_ret = GET_REG(gmac->f_base_add + REG_GMAC_INTR_EN);
    isr_ret |= bit;
    SET_REG(gmac->f_base_add + REG_GMAC_INTR_EN, isr_ret);
}

void fh_gmac_isr_clear(fh_gmac_object_t* gmac, rt_uint32_t bit)
{
    rt_uint32_t isr_ret;
    isr_ret = GET_REG(gmac->f_base_add + REG_GMAC_INTR_EN);
    isr_ret &= ~bit;
    SET_REG(gmac->f_base_add + REG_GMAC_INTR_EN, isr_ret);
}

void fh_gmac_clear_status(fh_gmac_object_t* gmac, rt_uint32_t bit)
{
    SET_REG(gmac->f_base_add + REG_GMAC_STATUS, bit);
}

void fh_gmac_dma_op_set(fh_gmac_object_t* gmac, UINT32 bit)
{
    UINT32 ret;
    ret = GET_REG(gmac->f_base_add + REG_GMAC_OP_MODE);
    ret |= bit;
    SET_REG(gmac->f_base_add + REG_GMAC_OP_MODE, ret);
}

void fh_gmac_dma_op_clear(fh_gmac_object_t* gmac, UINT32 bit)
{
    UINT32 ret;
    ret = GET_REG(gmac->f_base_add + REG_GMAC_OP_MODE);
    ret &= ~bit;
    SET_REG(gmac->f_base_add + REG_GMAC_OP_MODE, ret);
}

void fh_gmac_dma_tx_stop(fh_gmac_object_t* gmac)
{
    fh_gmac_dma_op_clear(gmac, 1 << 1);
}

void fh_gmac_dma_tx_start(fh_gmac_object_t* gmac)
{
    fh_gmac_dma_op_set(gmac, 1 << 1);
}

void fh_gmac_dma_rx_stop(fh_gmac_object_t* gmac)
{
    fh_gmac_dma_op_clear(gmac, 1 << 13);
}

void fh_gmac_dma_rx_start(fh_gmac_object_t* gmac)
{
    fh_gmac_dma_op_set(gmac, 1 << 13);
}

rt_inline unsigned long emac_virt_to_phys(unsigned long addr) { return addr; }
static void fh_gmac_set_mac_address(fh_gmac_object_t* p, UINT8* mac)
{
    UINT32 macHigh = mac[5] << 8 | mac[4];
    UINT32 macLow  = mac[3] << 24 | mac[2] << 16 | mac[1] << 8 | mac[0];

    SET_REG(p->f_base_add + REG_GMAC_MAC_HIGH, macHigh);
    SET_REG(p->f_base_add + REG_GMAC_MAC_LOW, macLow);
}

static void fh_gmac_set_phy_register(fh_gmac_object_t* p, int reg, UINT32 data)
{
    SET_REG(p->f_base_add + REG_GMAC_GMII_DATA, data);

    SET_REG(p->f_base_add + REG_GMAC_GMII_ADDRESS,
            0x1 << 1 | p->phy_addr << 11 | gmac_gmii_clock_100_150 << 2 |
                reg << 6 | 0x1);

    while (GET_REG(p->f_base_add + REG_GMAC_GMII_ADDRESS) & 0x1)
    {
    }
}

static UINT32 fh_gmac_get_phy_register(fh_gmac_object_t* p, int reg)
{
    SET_REG(p->f_base_add + REG_GMAC_GMII_ADDRESS,
            p->phy_addr << 11 | gmac_gmii_clock_100_150 << 2 | reg << 6 | 0x1);
    while (GET_REG(p->f_base_add + REG_GMAC_GMII_ADDRESS) & 0x1)
    {
    }
    return GET_REG(p->f_base_add + REG_GMAC_GMII_DATA);
}

static void udelay(rt_uint32_t us)
{
    rt_uint32_t len;
    for (; us > 0; us--)
        for (len = 0; len < 10; len++)
            ;
}

static int fh_gamc_get_phy_auto_negotiation_status(fh_gmac_object_t* gmac)
{
    Reg_Phyt_Basic_Ctrl basic_ctrl;
    Reg_Phyt_Basic_Status basic_status;
    basic_ctrl.dw   = 0;
    basic_status.dw = 0;

    int i = 0;
    while (!basic_status.bit.auto_negotiate_complete)
    {
        basic_status.dw =
            fh_gmac_get_phy_register(gmac, gmac_phyt_reg_basic_status);
        udelay(10);
        i++;
        if (i > GMAC_TIMEOUT_AUTODONE)
        {
            rt_kprintf("***ERROR: auto negotiation timeout\n");
            return -1;
        }
    }

    while (!basic_status.bit.link_status)
    {
        basic_status.dw =
            fh_gmac_get_phy_register(gmac, gmac_phyt_reg_basic_status);
        udelay(10);
        i++;
        if (i > GMAC_TIMEOUT_PHYLINK)
        {
            rt_kprintf("***ERROR: auto negotiation timeout\n");
            return -1;
        }
    }

    basic_ctrl.dw = fh_gmac_get_phy_register(gmac, gmac_phyt_reg_basic_ctrl);

    gmac->full_duplex = basic_ctrl.bit.duplex_mode;
    gmac->speed_100m  = basic_ctrl.bit.speed_select;

    rt_kprintf("operating at ");
    switch (basic_ctrl.bit.speed_select)
    {
    case 0:
        rt_kprintf("10M ");
        break;
    case 1:
        rt_kprintf("100M ");
        break;
    default:
        rt_kprintf("unknown: %d ", basic_ctrl.bit.speed_select);
        break;
    }

    switch (basic_ctrl.bit.duplex_mode)
    {
    case 0:
        rt_kprintf("half duplex ");
        break;
    case 1:
        rt_kprintf("full duplex ");
        break;
    default:
        rt_kprintf("unknown: %d ", basic_ctrl.bit.duplex_mode);
        break;
    }
    rt_kprintf("mode\n");

    return 0;
}

void fh_gmac_dma_reset(fh_gmac_object_t* gmac)
{
    fh_gmac_dma_rx_stop(gmac);
    fh_gmac_dma_tx_stop(gmac);

    SET_REG(gmac->f_base_add + REG_GMAC_BUS_MODE, 1 << 0);
    while (GET_REG(gmac->f_base_add + REG_GMAC_BUS_MODE) & 0x01)
        ;

    while (GET_REG(gmac->f_base_add + REG_GMAC_AXI_STATUS) & 0x3)
        ;
}

int fh_gmac_dma_int(fh_gmac_object_t* gmac)
{
//    unsigned int ret;

    // soft reset
    // fh_gmac_dma_reset(gmac);

    SET_REG(gmac->f_base_add + REG_GMAC_BUS_MODE, 1 << 25 | 1 << 16 | 32 << 8);
    SET_REG(gmac->f_base_add + REG_GMAC_RX_DESC_ADDR,
            (UINT32)gmac->rx_ring_dma);
    SET_REG(gmac->f_base_add + REG_GMAC_TX_DESC_ADDR,
            (UINT32)gmac->tx_ring_dma);
    // SET_REG(gmac->f_base_add + REG_GMAC_STATUS, 0xffffffff);
    fh_gmac_clear_status(gmac, 0xffffffff);

    // enable isr
    fh_gmac_isr_set(gmac, NORMAL_INTERRUPT_ENABLE | RIE_ISR);
    return 0;
}

void fh_set_mac_config_reg(fh_gmac_object_t* gmac, UINT32 bit)
{
    UINT32 config_ret;

    config_ret = GET_REG(gmac->f_base_add + REG_GMAC_CONFIG);
    config_ret |= bit;
    SET_REG(gmac->f_base_add + REG_GMAC_CONFIG, config_ret);
}

void fh_clear_mac_config_reg(fh_gmac_object_t* gmac, UINT32 bit)
{
    UINT32 config_ret;

    config_ret = GET_REG(gmac->f_base_add + REG_GMAC_CONFIG);
    config_ret &= ~bit;
    SET_REG(gmac->f_base_add + REG_GMAC_CONFIG, config_ret);
}

int fh_gmac_mac_int(fh_gmac_object_t* gmac)
{
    if (fh_gamc_get_phy_auto_negotiation_status(gmac) < 0)
    {
        return -1;
    }

    fh_gmac_set_mac_address(gmac, gmac->local_mac_address);
    // SET_REG(gmac->f_base_add + REG_GMAC_FRAME_FILTER, 1 << 5 | 1 << 9);
    SET_REG(gmac->f_base_add + REG_GMAC_FRAME_FILTER, 1 << 31 | 1 << 0);
    // enable mac rx and tx...
    fh_clear_mac_config_reg(gmac, 0xffffffff);
    fh_set_mac_config_reg(gmac, 1 << 15 | gmac->speed_100m << 14 |
                                    gmac->full_duplex << 11 | 1 << 7 |
                                    MAC_RX_ENABLE_POS | MAC_TX_ENABLE_POS);

    return 0;
}

int fh_gmac_probe(fh_gmac_object_t* gmac)
{
    UINT32 phyReg2, phyReg3, ouiVal;
    UINT32 rmii_mode;

    phyReg2 = fh_gmac_get_phy_register(gmac, gmac_phyt_reg_phy_id1);
    phyReg3 = fh_gmac_get_phy_register(gmac, gmac_phyt_reg_phy_id2);

    ouiVal = ((phyReg3 & 0xfc00) << 6) | phyReg2;

    rt_kprintf("PHY ID: 0x%x\n", ouiVal);

// GMAC_GetPhyID(gmac);
#ifdef CONFIG_PHY_RTL8201
    // switch to page7
    fh_gmac_set_phy_register(gmac, gmac_phyt_rtl8201_page_select, 7);

    // phy rmii register settings
    rmii_mode = fh_gmac_get_phy_register(gmac, gmac_phyt_rtl8201_rmii_mode);
#endif

#ifdef CONFIG_PHY_IP101G
    fh_gmac_set_phy_register(gmac, gmac_phyt_ip101g_page_select, 16);
    rmii_mode = fh_gmac_get_phy_register(gmac, gmac_phyt_rtl8201_rmii_mode);
#endif

#ifdef CONFIG_PHY_TI83848
    rmii_mode = fh_gmac_get_phy_register(gmac, gmac_phyt_ti83848_rmii_mode);
#endif
    if (gmac->phy_interface == gmac_rmii)
    {
#ifdef CONFIG_PHY_RTL8201
        // rmii_mode |= 0x1008;
        // 0x7ffb when an external clock inputs to the CKXTAL2 pin
        rmii_mode = 0x7ffb;
#endif

#ifdef CONFIG_PHY_IP101G
        rmii_mode = 0x1006;
#endif

#ifdef CONFIG_PHY_TI83848
        rmii_mode |= 0x20;
#endif
    }
    else if (gmac->phy_interface == gmac_mii)
    {
#ifdef CONFIG_PHY_RTL8201
        // rmii_mode &= ~(0x1008);
        rmii_mode = 0x6ff3;
// rmii_mode = 0xff2;
#endif

#ifdef CONFIG_PHY_IP101G
        rmii_mode = 0x2;
#endif

#ifdef CONFIG_PHY_TI83848
        rmii_mode &= ~(0x20);
#endif
    }
#ifdef CONFIG_PHY_RTL8201
    fh_gmac_set_phy_register(gmac, gmac_phyt_rtl8201_rmii_mode, rmii_mode);
    // back to page0
    fh_gmac_set_phy_register(gmac, gmac_phyt_rtl8201_page_select, 0);
#endif

#ifdef CONFIG_PHY_IP101G
    fh_gmac_set_phy_register(gmac, gmac_phyt_rtl8201_rmii_mode, rmii_mode);
    //back to page0
    fh_gmac_set_phy_register(gmac, gmac_phyt_ip101g_page_select, 0x10);
#endif

#ifdef CONFIG_PHY_TI83848
    fh_gmac_set_phy_register(gmac, gmac_phyt_ti83848_rmii_mode, rmii_mode);
#endif

    return 0;
}

int fh_gmac_init(rt_device_t dev)
{
    fh_gmac_object_t* gmac;
    gmac = (fh_gmac_object_t*)dev->user_data;

    // phy init .....
    if (fh_gmac_probe(gmac) < 0)
    {
        return -1;
    }

    // control init || auto negotiation enable mac rx and tx
    if (fh_gmac_dma_int(gmac) < 0 || fh_gmac_mac_int(gmac) < 0)
    {
        return -1;
    }

    return 0;
}

void fh_gmac_halt(rt_device_t dev)
{
#if (0)
    fh_gmac_object_t* gmac;
    gmac = (fh_gmac_object_t*)dev->user_data;

    SET_REG(gmac->f_base_add + REG_GMAC_STATUS, 0xffffffff);
    SET_REG(gmac->f_base_add + REG_GMAC_INTR_EN, 0x0);
    SET_REG_M(gmac->f_base_add + REG_GMAC_CONFIG, 0 << 3, 1 << 3);
    SET_REG_M(gmac->f_base_add + REG_GMAC_CONFIG, 0 << 2, 1 << 2);
#endif
}

static int fh_gmac_mdio_read(fh_gmac_object_t* gmac, int phyreg)
{
    int data;
    int phyaddr = gmac->phy_addr;

    rt_sem_take(&gmac->mdio_bus_lock, RT_WAITING_FOREVER);

    SET_REG(gmac->f_base_add + REG_GMAC_GMII_ADDRESS,
            phyaddr << 11 | gmac_gmii_clock_100_150 << 2 | phyreg << 6 | 0x1);

    while (GET_REG(gmac->f_base_add + REG_GMAC_GMII_ADDRESS) & 0x1)
        ;

    data = GET_REG(gmac->f_base_add + REG_GMAC_GMII_DATA);
    rt_sem_release(&gmac->mdio_bus_lock);

    return data;
}

#if 0
static int fh_gmac_mdio_write(fh_gmac_object_t* gmac, int phyreg, short phydata)
{
    int phyaddr = gmac->phy_addr;

    rt_sem_take(&gmac->mdio_bus_lock, RT_WAITING_FOREVER);
    SET_REG(gmac->f_base_add + REG_GMAC_GMII_DATA, phydata);

    SET_REG(gmac->f_base_add + REG_GMAC_GMII_ADDRESS,
            0x1 << 1 | phyaddr << 11 | gmac_gmii_clock_100_150 << 2 |
                phyreg << 6 | 0x1);

    while (GET_REG(gmac->f_base_add + REG_GMAC_GMII_ADDRESS) & 0x1)
        ;

    rt_sem_release(&gmac->mdio_bus_lock);

    return 0;
}
#endif

void fh_gmac_update_link(void* param)
{
    fh_gmac_object_t* gmac;
    gmac = (fh_gmac_object_t*)param;

    rt_device_t dev = &gmac->parent.parent;
    int status, status_change = 0;
    rt_uint32_t link;
    rt_uint32_t media;
    rt_uint16_t adv, lpa;

    /* Do a fake read */
    status = fh_gmac_mdio_read(gmac, MII_BMSR);
    if (status < 0) return;

    /* Read link and autonegotiation status */
    status = fh_gmac_mdio_read(gmac, MII_BMSR);
    if (status < 0) return;

    if ((status & BMSR_LSTATUS) == 0)
        link = 0;
    else
        link = 1;

    if (link != gmac->link)
    {
        gmac->link    = link;
        status_change = 1;
    }

    if (status_change)
    {
        if (gmac->link)
        {
            adv   = fh_gmac_mdio_read(gmac, MII_ADVERTISE);
            lpa   = fh_gmac_mdio_read(gmac, MII_LPA);
            media = mii_nway_result(lpa & adv);
            gmac->speed =
                (media & (ADVERTISE_100FULL | ADVERTISE_100HALF) ? 100 : 10);
            gmac->duplex = (media & ADVERTISE_FULL) ? 1 : 0;
            rt_kprintf("%s: link up (%dMbps/%s-duplex)\n", dev->parent.name,
                       gmac->speed,
                       DUPLEX_FULL == gmac->duplex ? "Full" : "Half");
            eth_device_linkchange(&gmac->parent, RT_TRUE);
        }
        else
        {
            rt_kprintf("%s: link down\n", dev->parent.name);
            eth_device_linkchange(&gmac->parent, RT_FALSE);
        }
    }
}

#if (0)

// this func check if gmac rx data...and wake the uplevel func
void fh_gmac_rx_poll_daemon(void* param)
{
    fh_gmac_object_t* gmac;
    gmac = (fh_gmac_object_t*)param;
    Gmac_Rx_DMA_Descriptors* desc;
    rt_device_t dev = &gmac->parent.parent;

    int i;
    // SET_REG_M(gmac->f_base_add + REG_GMAC_OP_MODE, 1 << 1, 1 << 1);
    // SET_REG(gmac->f_base_add + REG_GMAC_RX_POLL_DEMAND, 0);

    desc = &gmac->rx_ring[0];
    BD_CACHE_WRITEBACK_INVALIDATE(
        desc, sizeof(Gmac_Rx_DMA_Descriptors) * GMAC_RX_RING_SIZE);

    for (i = 0; i < GMAC_RX_RING_SIZE; i++)
    {
        if (!desc->desc0.bit.own)
        {
            //			rt_kprintf("daemon:i is %x\n",i);
            //			rt_kprintf("daemon:desc0.dw is
            //%x\n",desc->desc0.dw);
            eth_device_ready(&(gmac->parent));
            break;
        }
        desc++;
    }
}
#endif

/*********************
 *
 * up level use interface
 *
 *********************/
static rt_err_t rt_fh_gmac_init(rt_device_t dev)
{
    int ret;
    fh_gmac_object_t* gmac;
    gmac = (fh_gmac_object_t*)dev->user_data;

    ret = fh_gmac_init(dev);
    if (ret < 0)
    {
        rt_kprintf("GMAC init failed\n");
    }

    rt_timer_init(&gmac->timer, "link_timer", fh_gmac_update_link, (void*)gmac,
                  RT_TICK_PER_SECOND, RT_TIMER_FLAG_PERIODIC);

    rt_timer_start(&gmac->timer);

    fh_gmac_dma_rx_start(gmac);
    fh_gmac_dma_tx_start(gmac);
    //
    //	rt_timer_init(&gmac->rx_poll_timer, "gmac_rx_poll_timer",
    //			fh_gmac_rx_poll_daemon,
    //		(void *)gmac,
    //		1,
    //		RT_TIMER_FLAG_PERIODIC);
    // open rx and tx
    // SET_REG_M(gmac->f_base_add + REG_GMAC_OP_MODE, 1 << 1, 1 << 1);
    // SET_REG_M(gmac->f_base_add + REG_GMAC_OP_MODE, 1 << 13, 1 << 13);
    // rt_timer_start(&gmac->rx_poll_timer);

    return RT_EOK;
}

static rt_err_t rt_fh_gmac_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_err_t rt_fh_gmac_close(rt_device_t dev) { return RT_EOK; }
static rt_size_t rt_fh_gmac_read(rt_device_t dev, rt_off_t pos, void* buffer,
                                 rt_size_t size)
{
    rt_set_errno(-RT_ENOSYS);
    return 0;
}

static rt_size_t rt_fh_gmac_write(rt_device_t dev, rt_off_t pos,
                                  const void* buffer, rt_size_t size)
{
    rt_set_errno(-RT_ENOSYS);
    return 0;
}

static rt_err_t rt_fh_gmac_control(rt_device_t dev, rt_uint8_t cmd, void* args)
{
    fh_gmac_object_t* gmac;
    gmac = (fh_gmac_object_t*)dev->user_data;

    switch (cmd)
    {
    case NIOCTL_GADDR:
        /* get mac address */
        if (args)
            rt_memcpy(args, gmac->local_mac_address, 6);
        else
            return -RT_ERROR;

        break;

    default:
        break;
    }

    return RT_EOK;
}

/* ethernet device interface */
/* transmit packet. */

static rt_err_t rt_fh81_gmac_tx(rt_device_t dev, struct pbuf* p)
{
    Gmac_Tx_DMA_Descriptors* desc;
    fh_gmac_object_t* gmac;
    gmac = (fh_gmac_object_t*)dev->user_data;
    struct pbuf* temp_pbuf;
    rt_uint8_t* bufptr;
//    rt_uint32_t time_out = 0;
//    rt_uint32_t ret, isr_ret;
    rt_uint32_t cur_tx_desc;

    rt_sem_take(&gmac->tx_lock, RT_WAITING_FOREVER);

    // SET_REG(gmac->f_base_add + REG_GMAC_STATUS, 0xffffffff);

    // fh_gmac_clear_status(gmac,0xffffffff);

    // desc = &gmac->tx_ring[0];

    cur_tx_desc = gmac->tx_cur_desc;
    desc        = fh_gmac_get_tx_desc(gmac, p->tot_len);

    if (!desc)
    {
        rt_sem_release(&gmac->tx_lock);
        return RT_EBUSY;
    }

    bufptr = (rt_uint8_t*)&gmac->tx_buffer[cur_tx_desc * GMAC_TX_BUFFER_SIZE];
    for (temp_pbuf = p; temp_pbuf != NULL; temp_pbuf = temp_pbuf->next)
    {
        rt_memcpy(bufptr, temp_pbuf->payload, temp_pbuf->len);
        bufptr += temp_pbuf->len;
    }

    // clean cache to mem
    BD_CACHE_WRITEBACK_INVALIDATE(
        (rt_uint32_t)&gmac->tx_buffer[cur_tx_desc * GMAC_TX_BUFFER_SIZE],
        p->tot_len);

    desc->desc1.bit.buffer1_size = p->tot_len;
    desc->desc2.bit.buffer_address_pointer =
        (rt_uint32_t)&gmac->tx_buffer[cur_tx_desc * GMAC_TX_BUFFER_SIZE];
    desc->desc0.bit.own = 1;

    desc->desc1.bit.first_segment = 1;
    desc->desc1.bit.last_segment  = 1;
    // desc->desc1.bit.intr_on_completion =1;
    BD_CACHE_WRITEBACK_INVALIDATE(desc, sizeof(Gmac_Tx_DMA_Descriptors));

    SET_REG(gmac->f_base_add + REG_GMAC_TX_POLL_DEMAND, 0);
    rt_sem_release(&gmac->tx_lock);

    return RT_EOK;
}

void fh81_gmac_rx_error_handle(fh_gmac_object_t* gmac)
{
    fh_gmac_clear_status(gmac, 1 << 7);
    fh_gmac_dma_rx_stop(gmac);
    rx_desc_init(gmac);
    fh_gmac_isr_set(gmac, RIE_ISR);
    fh_gmac_dma_rx_start(gmac);
    SET_REG(gmac->f_base_add + REG_GMAC_RX_POLL_DEMAND, 1);
}

/* reception packet. */
static struct pbuf* rt_fh81_gmac_rx(rt_device_t dev)
{
//    int i;
//    int p_buf_index = 0;
    Gmac_Rx_DMA_Descriptors* desc;
    fh_gmac_object_t* gmac;
    gmac = (fh_gmac_object_t*)dev->user_data;

    struct pbuf* temp_pbuf = RT_NULL;
//    rt_uint32_t src_base_add;
    rt_uint32_t each_len = 0;
    rt_sem_take(&gmac->rx_lock, RT_WAITING_FOREVER);
//    rt_uint32_t isr_ret;
//    rt_uint32_t get_desc_no;

    rt_uint32_t ret;
    ret = GET_REG(gmac->f_base_add + REG_GMAC_STATUS);
    if (ret & 1 << 7)
    {
        fh_gmac_clear_status(gmac, 1 << 7);
        GMAC_DEBUG("rx buff unavailable when process.\n");
        g_error_rec.rx_buff_unavail++;

        fh81_gmac_rx_error_handle(gmac);
        // fh_gmac_isr_set(gmac,RIE_ISR);
        rt_sem_release(&gmac->rx_lock);
        return RT_NULL;
        // SET_REG(gmac->f_base_add + REG_GMAC_TX_POLL_DEMAND,0);
    }

    desc = fh_gmac_get_rx_desc(gmac);
    if (desc)
    {
        each_len  = desc->desc0.bit.frame_length;
        temp_pbuf = pbuf_alloc(PBUF_LINK, each_len, PBUF_RAM);
        if (!temp_pbuf)
        {
            GMAC_DEBUG("alloc pbuf failed\n");
            g_error_rec.rtt_malloc_failed++;
            fh81_gmac_rx_error_handle(gmac);
            rt_sem_release(&gmac->rx_lock);
            return RT_NULL;
        }
        BD_CACHE_WRITEBACK_INVALIDATE(
            (rt_uint32_t)(desc->desc2.bit.buffer_address_pointer), each_len);
        rt_memcpy(temp_pbuf->payload,
                  (rt_uint8_t*)(desc->desc2.bit.buffer_address_pointer),
                  each_len);
        desc->desc0.bit.own = 1;
        BD_CACHE_WRITEBACK_INVALIDATE(desc, sizeof(Gmac_Rx_DMA_Descriptors));

        SET_REG(gmac->f_base_add + REG_GMAC_RX_POLL_DEMAND, 1);
    }
    else
    {
        // read all the desc done...reopen the rx isr..
        fh_gmac_isr_set(gmac, RIE_ISR);
    }

    rt_sem_release(&gmac->rx_lock);

    return temp_pbuf;
}

static rt_uint32_t fh_gmac_get_rx_desc_no(fh_gmac_object_t* gmac)
{
    rt_uint32_t temp_rx_index = 0;
    rt_uint32_t ret           = 0;
    temp_rx_index             = gmac->rx_cur_desc;

    // flush cache first..
    BD_CACHE_WRITEBACK_INVALIDATE(
        gmac->rx_ring, GMAC_RX_RING_SIZE * sizeof(Gmac_Rx_DMA_Descriptors));

    while (!gmac->rx_ring[temp_rx_index].desc0.bit.own)
    {
        ret++;
        temp_rx_index++;
        temp_rx_index %= GMAC_RX_RING_SIZE;
    }
    return ret;
}

static Gmac_Rx_DMA_Descriptors* fh_gmac_get_rx_desc(fh_gmac_object_t* gmac)
{
    //	rt_uint32_t temp_rx_index = 0;
    //	rt_uint32_t ret = 0;
    //	temp_rx_index = gmac->rx_cur_desc;
    //
    //
    //	//flush cache first..

    //	while(!gmac->rx_ring[temp_rx_index].desc0.bit.own){
    //		ret ++;
    //		temp_rx_index++;
    //		temp_rx_index %= GMAC_RX_RING_SIZE;
    //	}
    //	return ret;

    Gmac_Rx_DMA_Descriptors* desc = &gmac->rx_ring[gmac->rx_cur_desc];
    BD_CACHE_WRITEBACK_INVALIDATE(desc, sizeof(Gmac_Rx_DMA_Descriptors));
    if (desc->desc0.bit.own)
    {
        return RT_NULL;
    }
    gmac->rx_cur_desc++;
    gmac->rx_cur_desc %= GMAC_RX_RING_SIZE;
    return desc;
}

void fh81_gmac_tx_error_handle(fh_gmac_object_t* gmac)
{
    fh_gmac_dma_tx_stop(gmac);
    tx_desc_init(gmac);
    fh_gmac_dma_tx_start(gmac);
    SET_REG(gmac->f_base_add + REG_GMAC_TX_POLL_DEMAND, 0);
}

static Gmac_Tx_DMA_Descriptors* fh_gmac_get_tx_desc(fh_gmac_object_t* gmac,
                                                    rt_uint32_t frame_len)
{
    if (frame_len > GMAC_TX_BUFFER_SIZE)
    {
        rt_kprintf("payload len too long.....\n");
        RT_ASSERT(frame_len < GMAC_EACH_DESC_MAX_TX_SIZE);
    }

    Gmac_Tx_DMA_Descriptors* desc = &gmac->tx_ring[gmac->tx_cur_desc];
    BD_CACHE_WRITEBACK_INVALIDATE(desc, sizeof(Gmac_Tx_DMA_Descriptors));
    if (desc->desc0.bit.own)
    {
        GMAC_DEBUG("error :over write tx desc\n");
        // RT_ASSERT(desc->desc0.bit.own != 1);
        g_error_rec.tx_over_write_desc++;
        // tx_desc_init(gmac);
        fh81_gmac_tx_error_handle(gmac);
        return RT_NULL;
    }

    gmac->tx_cur_desc++;
    gmac->tx_cur_desc %= GMAC_TX_RING_SIZE;
    return desc;
}

void gmac_rx_work_func(struct rt_work* work, void* work_data)
{
    fh_gmac_object_t* gmac;
    gmac = (fh_gmac_object_t*)work_data;
    rt_uint32_t get_desc_no, i;
//    rt_uint32_t times, isr_ret;
//    Gmac_Rx_DMA_Descriptors* rx_desc;

    int ret;

    get_desc_no = fh_gmac_get_rx_desc_no(gmac);

    // gmac->get_frame_no = get_desc_no;

    for (i = 0; i < get_desc_no; i++)
    {
        ret = eth_device_ready(&(gmac->parent));
        if (ret != RT_EOK)
        {
            rt_kprintf("i is %d\n", i);
            rt_kprintf("get_desc_no is %d\n", get_desc_no);
            rt_kprintf("call the app to get data failed.\n");
        }
    }

    // open rx isr
    //	isr_ret = GET_REG(gmac->f_base_add + REG_GMAC_INTR_EN);
    //	isr_ret |=1<<6;
    //	SET_REG(gmac->f_base_add + REG_GMAC_INTR_EN, isr_ret);
    fh_gmac_isr_set(gmac, RIE_ISR);
}

void tx_desc_init(fh_gmac_object_t* gmac)
{
    int i;
    for (i = 0; i < GMAC_TX_RING_SIZE; i++)
    {
        Gmac_Tx_DMA_Descriptors* desc = &gmac->tx_ring[i];
        desc->desc0.dw                = 0;
        desc->desc1.dw                = 0;
        desc->desc2.dw                = 0;
        desc->desc3.dw                = 0;
        // desc->desc3.dw = (rt_uint32_t)&gmac->tx_ring[0];
        desc->desc1.bit.first_segment          = 1;
        desc->desc1.bit.last_segment           = 1;
        desc->desc1.bit.end_of_ring            = 0;
        desc->desc1.bit.second_address_chained = 1;
        desc->desc3.bit.buffer_address_pointer =
            (rt_uint32_t)&gmac->tx_ring[i + 1];
    }
    // gmac->tx_ring[GMAC_TX_RING_SIZE -1].desc1.bit.last_segment =1;
    gmac->tx_ring[GMAC_TX_RING_SIZE - 1].desc3.bit.buffer_address_pointer =
        (UINT32)&gmac->tx_ring[0];
    // gmac->tx_ring[GMAC_TX_RING_SIZE -1].desc1.bit.end_of_ring = 1;
    gmac->tx_cur_desc = 0;
    BD_CACHE_WRITEBACK_INVALIDATE(
        gmac->tx_ring, sizeof(Gmac_Tx_DMA_Descriptors) * GMAC_TX_RING_SIZE);
}

void rx_desc_init(fh_gmac_object_t* gmac)
{
    int i;
    for (i = 0; i < GMAC_RX_RING_SIZE; i++)
    {
        Gmac_Rx_DMA_Descriptors* desc          = &gmac->rx_ring[i];
        desc->desc0.dw                         = 0;
        desc->desc1.dw                         = 0;
        desc->desc2.dw                         = 0;
        desc->desc3.dw                         = 0;
        desc->desc1.bit.buffer1_size           = 2000;
        desc->desc1.bit.end_of_ring            = 0;
        desc->desc1.bit.second_address_chained = 1;
        desc->desc2.bit.buffer_address_pointer =
            (rt_uint32_t)gmac->rx_buffer_dma + i * GMAC_RX_BUFFER_SIZE;
        desc->desc3.bit.buffer_address_pointer =
            (rt_uint32_t)&gmac->rx_ring[i + 1];
        desc->desc0.bit.own = 1;
    }
    gmac->rx_ring[GMAC_RX_RING_SIZE - 1].desc3.bit.buffer_address_pointer =
        (rt_uint32_t)&gmac->rx_ring[0];
    // gmac->rx_ring[GMAC_RX_RING_SIZE -1].desc1.bit.end_of_ring =1;
    gmac->rx_cur_desc = 0;

    BD_CACHE_WRITEBACK_INVALIDATE(
        gmac->rx_ring, sizeof(Gmac_Rx_DMA_Descriptors) * GMAC_RX_RING_SIZE);
}

static rt_err_t fh_gmac_initialize(fh_gmac_object_t* gmac)
{
    UINT32 t1;
    UINT32 t2;
    UINT32 t3;
    UINT32 t4;

    t1 = (UINT32)rt_malloc(GMAC_TX_RING_SIZE * sizeof(Gmac_Tx_DMA_Descriptors) +
                           CACHE_LINE_SIZE);
    if (!t1) goto err1;

    t2 = (UINT32)rt_malloc(GMAC_TX_RING_SIZE * GMAC_TX_BUFFER_SIZE +
                           CACHE_LINE_SIZE);
    if (!t2) goto err2;

    t3 = (UINT32)rt_malloc(GMAC_RX_RING_SIZE * sizeof(Gmac_Rx_DMA_Descriptors) +
                           CACHE_LINE_SIZE);
    if (!t3) goto err3;

    t4 = (UINT32)rt_malloc(GMAC_RX_RING_SIZE * GMAC_RX_BUFFER_SIZE +
                           CACHE_LINE_SIZE);
    if (!t4) goto err4;

    gmac->tx_ring_original = (UINT8*)t1;
    gmac->tx_ring = (Gmac_Tx_DMA_Descriptors*)((t1 + CACHE_LINE_SIZE - 1) &
                                               (~(CACHE_LINE_SIZE - 1)));
    rt_memset(gmac->tx_ring, 0,
              GMAC_TX_RING_SIZE * sizeof(Gmac_Tx_DMA_Descriptors));
    BD_CACHE_WRITEBACK_INVALIDATE(
        gmac->tx_ring, GMAC_TX_RING_SIZE * sizeof(Gmac_Tx_DMA_Descriptors));
    gmac->tx_ring_dma = emac_virt_to_phys((unsigned long)gmac->tx_ring);

    gmac->tx_buffer_original = (UINT8*)t2;
    gmac->tx_buffer =
        (UINT8*)((t2 + CACHE_LINE_SIZE - 1) & (~(CACHE_LINE_SIZE - 1)));
    gmac->tx_buffer_dma = emac_virt_to_phys((unsigned long)gmac->tx_buffer);

    gmac->rx_ring_original = (UINT8*)t3;
    gmac->rx_ring = (Gmac_Rx_DMA_Descriptors*)((t3 + CACHE_LINE_SIZE - 1) &
                                               (~(CACHE_LINE_SIZE - 1)));
    rt_memset(gmac->rx_ring, 0,
              GMAC_RX_RING_SIZE * sizeof(Gmac_Rx_DMA_Descriptors));
    BD_CACHE_WRITEBACK_INVALIDATE(
        gmac->rx_ring, GMAC_RX_RING_SIZE * sizeof(Gmac_Rx_DMA_Descriptors));
    gmac->rx_ring_dma = emac_virt_to_phys((unsigned long)gmac->rx_ring);

    gmac->rx_buffer_original = (UINT8*)t4;
    gmac->rx_buffer =
        (UINT8*)((t4 + CACHE_LINE_SIZE - 1) & (~(CACHE_LINE_SIZE - 1)));
    gmac->rx_buffer_dma = emac_virt_to_phys((unsigned long)gmac->rx_buffer);

    // init tx desc
    tx_desc_init(gmac);

    // init rx desc
    rx_desc_init(gmac);

    gmac->phy_addr = PHYT_ID;
    gmac->link     = 0;

#ifdef CONFIG_RMII
    gmac->phy_interface = gmac_rmii;
    rt_kprintf("gmac use rmii..\n");
#else
    gmac->phy_interface = gmac_mii;
    rt_kprintf("gmac use mii..\n");
#endif

    return RT_EOK;

err4:
    rt_free((void*)t3);
err3:
    rt_free((void*)t2);
err2:
    rt_free((void*)t1);
err1:
    return -RT_ENOMEM;
}

static void rt_fh_gmac_isr(int irq, void* param)
{
    fh_gmac_object_t* gmac;
    gmac = (fh_gmac_object_t*)param;

    int ret_eth;
    unsigned int ret;/*, isr_ret;*/
//    rt_device_t dev = &(gmac->parent.parent);
//    int status, rsr, tsr;

    ret = GET_REG(gmac->f_base_add + REG_GMAC_STATUS);
    // SET_REG(gmac->f_base_add + REG_GMAC_STATUS, 0xffffffff);
    fh_gmac_clear_status(gmac, 0xffffffff);

    if (ret & 1 << 4)
    {
        rt_kprintf("gmac rx overflow..\n\n");
        g_error_rec.rx_over_flow++;
    }

    if (ret & 1 << 5)
    {
        rt_kprintf("gmac tx underflow..\n\n");
        g_error_rec.tx_under_flow++;
    }

    if (ret & 1 << 2)
    {
        // rt_kprintf("gmac tx buff unavailable\n");
        g_error_rec.tx_buff_unavail++;
        //		fh_gmac_probe(gmac);
        //		fh_gmac_dma_int(gmac);
        //		fh_gmac_mac_int(gmac);
        // tx_desc_init(gmac);
        // SET_REG_M(gmac->f_base_add + REG_GMAC_OP_MODE, 1 << 1, 1 << 1);
        // SET_REG_M(gmac->f_base_add + REG_GMAC_OP_MODE, 1 << 13, 1 << 13);
        // SET_REG(gmac->f_base_add + REG_GMAC_RX_POLL_DEMAND,0);
        // SET_REG(gmac->f_base_add + REG_GMAC_TX_POLL_DEMAND,0);
    }

    if (ret & 1 << 7)
    {
        rt_kprintf("gmac rx buff isr unavailable\n");
        g_error_rec.rx_buff_unavail++;
        //		fh_gmac_probe(gmac);
        //		fh_gmac_dma_int(gmac);
        //		fh_gmac_mac_int(gmac);
        rx_desc_init(gmac);
        SET_REG_M(gmac->f_base_add + REG_GMAC_OP_MODE, 1 << 1, 1 << 1);
        SET_REG_M(gmac->f_base_add + REG_GMAC_OP_MODE, 1 << 13, 1 << 13);
        SET_REG(gmac->f_base_add + REG_GMAC_RX_POLL_DEMAND, 1);
        // SET_REG(gmac->f_base_add + REG_GMAC_TX_POLL_DEMAND,0);
    }

    if (ret & 1 << 8)
    {
        rt_kprintf("gmac rx process stop..\n");
//        g_error_rec.rx_process_stop;
    }

    if (ret & 3 << 23)
    {
        rt_kprintf("error status:%x\n", ret);
        g_error_rec.other_error++;
    }

    if (ret & 1 << 6)
    {
        fh_gmac_isr_clear(gmac, RIE_ISR);
        // wake up rx..
        ret_eth = eth_device_ready(&(gmac->parent));
        if (ret_eth != RT_EOK)
        {
            fh_gmac_isr_set(gmac, RIE_ISR);
            rt_kprintf("eth_device_ready error\n");
        }
        // rt_workqueue_dowork(gmac->rx_queue,gmac->rx_work);
    }
}

int rt_app_fh_gmac_init(void)
{
    // rt_sem_init

    fh_gmac_object_t* gmac;
    gmac = (fh_gmac_object_t*)rt_malloc(sizeof(*gmac));
    if (gmac == NULL)
    {
        rt_kprintf("fh81_eth_initialize: Cannot allocate Gmac_Object %d\n", 1);
        return (-1);
    }

    memset(gmac, 0, sizeof(*gmac));
    gmac->p_base_add = (void*)0xEF000000;
    gmac->f_base_add = 0xEF000000;
    fh_gmac_initialize(gmac);
    // init 0..wait isr release.
    rt_sem_init(&gmac->tx_ack, "tx_ack", 0, RT_IPC_FLAG_FIFO);
    // init 1.
    rt_sem_init(&gmac->tx_lock, "tx_lock", 1, RT_IPC_FLAG_FIFO);
    // init 0.. wait isr release
    rt_sem_init(&gmac->rx_ack, "rx_ack", 0, RT_IPC_FLAG_FIFO);
    // init 1.
    rt_sem_init(&gmac->rx_lock, "rx_lock", 1, RT_IPC_FLAG_FIFO);

    rt_sem_init(&gmac->mdio_bus_lock, "mdio_bus_lock", 1, RT_IPC_FLAG_FIFO);

    gmac->local_mac_address[0] = 0x00;
    gmac->local_mac_address[1] = 0x60;
    gmac->local_mac_address[2] = 0x6E;
    gmac->local_mac_address[3] = 0x11;
    gmac->local_mac_address[4] = 0x22;
    gmac->local_mac_address[5] = 0x33;

    gmac->parent.parent.init      = rt_fh_gmac_init;
    gmac->parent.parent.open      = rt_fh_gmac_open;
    gmac->parent.parent.close     = rt_fh_gmac_close;
    gmac->parent.parent.read      = rt_fh_gmac_read;
    gmac->parent.parent.write     = rt_fh_gmac_write;
    gmac->parent.parent.control   = rt_fh_gmac_control;
    gmac->parent.parent.user_data = (void*)gmac;

    gmac->parent.eth_rx = rt_fh81_gmac_rx;
    gmac->parent.eth_tx = rt_fh81_gmac_tx;

    /* instal interrupt */
    rt_hw_interrupt_install(EMAC_IRQn, rt_fh_gmac_isr, (void*)gmac, "emac_isr");
    rt_hw_interrupt_umask(EMAC_IRQn);

    eth_device_init(&(gmac->parent), "e0");

    //

    return 0;
}

#ifdef RT_USING_FINSH
#include "finsh.h"

void dump_rx_desc()
{
    int i;
    fh_gmac_object_t* gmac;
    rt_device_t dev = rt_device_find("e0");

    if (dev == RT_NULL) return;

    gmac = (fh_gmac_object_t*)dev->user_data;

    for (i = 0; i < GMAC_RX_RING_SIZE; i++)
    {
        Gmac_Rx_DMA_Descriptors* desc = &gmac->rx_ring[i];
        rt_kprintf("desc%d\tadd:%08x\n", i, (unsigned int)desc);
        rt_kprintf("%08x  %08x  %08x  %08x\n", desc->desc0.dw, desc->desc1.dw,
                   desc->desc2.dw, desc->desc3.dw);
        rt_kprintf("\n");
    }
    rt_kprintf("soft current desc is:%d\n", gmac->rx_cur_desc);
}

void dump_tx_desc()
{
    int i;
    fh_gmac_object_t* gmac;
    rt_device_t dev = rt_device_find("e0");

    if (dev == RT_NULL) return;

    gmac = (fh_gmac_object_t*)dev->user_data;

    for (i = 0; i < GMAC_TX_RING_SIZE; i++)
    {
        Gmac_Tx_DMA_Descriptors* desc = &gmac->tx_ring[i];
        rt_kprintf("desc%d\tadd:%08x\n", i, (unsigned int)desc);
        rt_kprintf("%08x  %08x  %08x  %08x\n", desc->desc0.dw, desc->desc1.dw,
                   desc->desc2.dw, desc->desc3.dw);
        rt_kprintf("\n");
    }
    rt_kprintf("soft current desc is:%d\n", gmac->tx_cur_desc);
}

int dump_mac_err()
{
    rt_kprintf("rx buff unavail:\t%08d\n", g_error_rec.rx_buff_unavail);
    rt_kprintf("rtt malloc failed:\t%08d\n", g_error_rec.rtt_malloc_failed);
    rt_kprintf("rx over flow:\t\t%08d\n", g_error_rec.rx_over_flow);
    rt_kprintf("rx process stop:\t%08d\n", g_error_rec.rx_process_stop);
    rt_kprintf("tx under flow:\t\t%08d\n", g_error_rec.tx_under_flow);
    rt_kprintf("other error:\t\t%08d\n", g_error_rec.other_error);
    rt_kprintf("tx over write desc error:\t\t%08d\n",
               g_error_rec.tx_over_write_desc);

    return 0;
}

FINSH_FUNCTION_EXPORT(dump_rx_desc, dump e0 rx desc);
FINSH_FUNCTION_EXPORT(dump_tx_desc, dump e0 tx desc);
FINSH_FUNCTION_EXPORT(dump_mac_err, dump e0 error status);

#endif

#endif
