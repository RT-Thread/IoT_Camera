/*
 * board.h
 *
 *  Created on: 2016Äê4ÔÂ11ÈÕ
 *      Author: Urey
 */

#ifndef DRIVERS_BOARD_H_
#define DRIVERS_BOARD_H_
/******************************************************************************
 * external Kernel Configuration
 ******************************************************************************/
#define RT_USING_DMA_MEM


/******************************************************************************
 * MEM Configuration
 ******************************************************************************/
#define FH_DDR_START			0xA0000000
#define FH_DDR_END				0xA1000000

#define FH_RTT_OS_MEM_SIZE		0x00600000
#define FH_DMA_MEM_SIZE		0x20000 		/* 128k */

#define FH_RTT_OS_MEM_END		(FH_DDR_START + FH_RTT_OS_MEM_SIZE)
#define FH_SDK_MEM_START		(FH_RTT_OS_MEM_END + FH_DMA_MEM_SIZE)
#define FH_RTT_OS_HEAP_END		FH_SDK_MEM_START
#define FH_SDK_MEM_SIZE		(FH_DDR_END - FH_SDK_MEM_START)


/******************************************************************************
 * Interface config..
 ******************************************************************************/
//#define RT_USING_UART0
#define RT_USING_UART1
//#define RT_USING_GPIO
//#define RT_USING_SDIO
//#define RT_USING_FH_DMA
//#define RT_USING_FH_ACW
//#define RT_USING_I2C
//#define RT_USING_PWM
//#define RT_USING_WDT
//#define RT_USING_SPI
//#define RT_USING_SADC

#endif /* DRIVERS_BOARD_H_ */
