/*
 * File      : fh81_gmac_dma.h
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

#ifndef FH81_GMAC_DMA_H_
#define FH81_GMAC_DMA_H_

/* DMA Status register defines */
#define DMA_STATUS_GPI 0x10000000 /* PMT interrupt */
#define DMA_STATUS_GMI 0x08000000 /* MMC interrupt */
#define DMA_STATUS_GLI 0x04000000 /* GMAC Line interface int */
#define DMA_STATUS_GMI 0x08000000
#define DMA_STATUS_GLI 0x04000000
#define DMA_STATUS_EB_MASK 0x00380000     /* Error Bits Mask */
#define DMA_STATUS_EB_TX_ABORT 0x00080000 /* Error Bits - TX Abort */
#define DMA_STATUS_EB_RX_ABORT 0x00100000 /* Error Bits - RX Abort */
#define DMA_STATUS_TS_MASK 0x00700000     /* Transmit Process State */
#define DMA_STATUS_TS_SHIFT 20
#define DMA_STATUS_RS_MASK 0x000e0000 /* Receive Process State */
#define DMA_STATUS_RS_SHIFT 17
#define DMA_STATUS_NIS 0x00010000  /* Normal Interrupt Summary */
#define DMA_STATUS_AIS 0x00008000  /* Abnormal Interrupt Summary */
#define DMA_STATUS_ERI 0x00004000  /* Early Receive Interrupt */
#define DMA_STATUS_FBI 0x00002000  /* Fatal Bus Error Interrupt */
#define DMA_STATUS_ETI 0x00000400  /* Early Transmit Interrupt */
#define DMA_STATUS_RWT 0x00000200  /* Receive Watchdog Timeout */
#define DMA_STATUS_RPS 0x00000100  /* Receive Process Stopped */
#define DMA_STATUS_RU 0x00000080   /* Receive Buffer Unavailable */
#define DMA_STATUS_RI 0x00000040   /* Receive Interrupt */
#define DMA_STATUS_UNF 0x00000020  /* Transmit Underflow */
#define DMA_STATUS_OVF 0x00000010  /* Receive Overflow */
#define DMA_STATUS_TJT 0x00000008  /* Transmit Jabber Timeout */
#define DMA_STATUS_TU 0x00000004   /* Transmit Buffer Unavailable */
#define DMA_STATUS_TPS 0x00000002  /* Transmit Process Stopped */
#define DMA_STATUS_TI 0x00000001   /* Transmit Interrupt */
#define DMA_CONTROL_FTF 0x00100000 /* Flush transmit FIFO */

/*
enum
{
        gmac_error_underflow_error,
        gmac_error_excessive_deferral,
        gmac_error_excessive_collision,
        gmac_error_late_collision,
        gmac_error_no_carrier,
        gmac_error_carrier_loss,
        gmac_error_payload_checksum_error,
        gmac_error_frame_flush,
        gmac_error_jabber_timeout,
        gmac_error_ip_header_error

};
*/
typedef union {
    struct
    {
        UINT32 deferred : 1;  // 0~31
        UINT32 underflow_error : 1;
        UINT32 excessive_deferral : 1;
        UINT32 collision_count : 4;
        UINT32 vlan_frame : 1;
        UINT32 excessive_collision : 1;
        UINT32 late_collision : 1;
        UINT32 no_carrier : 1;
        UINT32 loss_of_carrier : 1;
        UINT32 payload_checksum_error : 1;
        UINT32 frame_flushed : 1;
        UINT32 jabber_timeout : 1;
        UINT32 error_summary : 1;
        UINT32 ip_header_error : 1;
        UINT32 tx_timestamp_status : 1;
        UINT32 reserved_30_18 : 13;
        UINT32 own : 1;
    } bit;
    UINT32 dw;
} Gmac_Tx_Descriptor0;

typedef union {
    struct
    {
        UINT32 buffer1_size : 11;  // 0~31
        UINT32 buffer2_size : 11;
        UINT32 timestamp_enable : 1;
        UINT32 disable_padding : 1;
        UINT32 second_address_chained : 1;
        UINT32 end_of_ring : 1;
        UINT32 disable_crc : 1;
        UINT32 checksum_insertion_ctrl : 2;
        UINT32 first_segment : 1;
        UINT32 last_segment : 1;
        UINT32 intr_on_completion : 1;
    } bit;
    UINT32 dw;
} Gmac_Tx_Descriptor1;

typedef union {
    struct
    {
        UINT32 buffer_address_pointer : 32;  // 0~31
    } bit;
    UINT32 dw;
} Gmac_Tx_Descriptor2;

typedef union {
    struct
    {
        UINT32 buffer_address_pointer : 32;  // 0~31
    } bit;
    UINT32 dw;
} Gmac_Tx_Descriptor3;

typedef union {
    struct
    {
        UINT32 mac_addr_payload_chksum_error : 1;  // 0
        UINT32 crc_error : 1;                      // 1
        UINT32 dribble_error : 1;                  // 2
        UINT32 receive_error : 1;                  // 3
        UINT32 watchdog_timeout : 1;               // 4
        UINT32 frame_type : 1;                     // 5
        UINT32 late_collision : 1;                 // 6
        UINT32 ipc_chksum_error_giant_frame : 1;   // 7
        UINT32 last_descriptor : 1;                // 8
        UINT32 first_descriptor : 1;               // 9
        UINT32 vlan_tag : 1;                       // 10
        UINT32 overflow_error : 1;                 // 11
        UINT32 length_error : 1;                   // 12
        UINT32 sa_filter_fail : 1;                 // 13
        UINT32 descriptor_error : 1;               // 14
        UINT32 error_summary : 1;                  // 15
        UINT32 frame_length : 14;                  // 16~29
        UINT32 da_filter_fail : 1;                 // 30
        UINT32 own : 1;                            // 31
    } bit;
    UINT32 dw;
} Gmac_Rx_Descriptor0;

typedef union {
    struct
    {
        UINT32 buffer1_size : 11;               // 0~10
        UINT32 buffer2_size : 11;               // 11~21
        UINT32 reserved_23_22 : 2;              // 22~23
        UINT32 second_address_chained : 1;      // 24
        UINT32 end_of_ring : 1;                 // 25
        UINT32 reserved_30_26 : 5;              // 26~30
        UINT32 disable_intr_on_completion : 1;  // 31
    } bit;
    UINT32 dw;
} Gmac_Rx_Descriptor1;

typedef union {
    struct
    {
        UINT32 buffer_address_pointer : 32;  // 0~31
    } bit;
    UINT32 dw;
} Gmac_Rx_Descriptor2;

typedef union {
    struct
    {
        UINT32 buffer_address_pointer : 32;  // 0~31
    } bit;
    UINT32 dw;
} Gmac_Rx_Descriptor3;

typedef struct
{
    Gmac_Tx_Descriptor0
        desc0; /* control and status information of descriptor */
    Gmac_Tx_Descriptor1 desc1; /* buffer sizes */
    Gmac_Tx_Descriptor2 desc2; /* physical address of the buffer 1 */
    Gmac_Tx_Descriptor3 desc3; /* physical address of the buffer 2 */
    UINT32 reserved0;
    UINT32 reserved1;
    UINT32 reserved2;
    UINT32 reserved3;
} Gmac_Tx_DMA_Descriptors;

typedef struct
{
    Gmac_Rx_Descriptor0
        desc0; /* control and status information of descriptor */
    Gmac_Rx_Descriptor1 desc1; /* buffer sizes */
    Gmac_Rx_Descriptor2 desc2; /* physical address of the buffer 1 */
    Gmac_Rx_Descriptor3 desc3; /* physical address of the buffer 2 */
    UINT32 reserved0;
    UINT32 reserved1;
    UINT32 reserved2;
    UINT32 reserved3;
} Gmac_Rx_DMA_Descriptors;

#endif /* FH81_GMAC_DMA_H_ */
