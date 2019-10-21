/*
 * file : cpu_config.h
 *
 *
 */

#ifndef _cpu_config_H
#define _cpu_config_H



#ifndef __ASSEMBLER__
/*IRQn_Type can be defined in different files describing different socs .
 so it needs to be disabled in api functions */
#define IRQn_Type IRQn_Type_TMP
#include "I96100.h"
#undef IRQn_Type

#endif  // for __ASSEMBLER__

#include "dt_interrupts_num.h"


#if 0 // defined in ISD94XXXSeries.h

/* Configuration of the Cortex-M# Processor and Core Peripherals */
#define __CM4_REV              0x0201    /*!< Core Revision r2p1    */
#define __NVIC_PRIO_BITS       4  /*!< Number of Bits used for Priority Levels*/
#define __Vendor_SysTickConfig 1  /*!< 1 if different SysTick Config is used*/
#define __MPU_PRESENT          1         /*!< MPU present or not */
#define __FPU_PRESENT          1         /*!< FPU present or not  */

typedef enum IRQn
{
	/* Cortex-M0 Processor Exceptions Numbers   */
	/*!< 1 Reset Vector, invoked on Power up and warm reset   */
	Reset_IRQn           = -15,

	NonMaskableInt_IRQn  = -14, /*!< 2 Non Maskable Interrupt*/
	HardFault_IRQn       = -13, /*!< 3 Cortex-M0 Hard Fault Interrupt*/
	SVCall_IRQn          = -5, /*!< 11 Cortex-M0 SV Call Interrupt*/
	PendSV_IRQn          = -2, /*!< 14 Cortex-M0 Pend SV Interrupt*/
	SysTick_IRQn         = -1, /*!< 15 Cortex-M0 System Tick Interrupt*/

	/******  I96100 Specific Interrupt Numbers ******************************/

	BOD_IRQn   = 0, /*!< 16 Brown-Out low voltage detected interrupt      */

	/*!< 17 Brown-Out low voltage detected, IRC TRIM and Clock fail
	 * detected interrupt  */
	BDCK_IRQn  = 1,
	RTC_IRQn   = 2, /*!< 18 Real time clock interrupt*/
	WDT_IRQn   = 3, /*!< 19 Watchdog Timer and Window Watchdog Timer interrupt*/
	EINT0_IRQn  = 4, /*!< 20 External interrupt 0*/
	EINT1_IRQn  = 5, /*!< 21 External interrupt 1*/
	GPA_IRQn   = 6, /*!< 22 External interrupt from PA[15:0] pin*/
	GPB_IRQn   = 7, /*!< 23 External interrupt from PB[15:0] pin*/
	GPC_IRQn   = 8, /*!< 24 External interrupt from PC[15:0] pin*/
	GPD_IRQn   = 9, /*!< 25 External interrupt from PD[15:0] pin*/
	SPI0_IRQn  = 10, /*!< 26 SPI0 interrupt*/
	PWM0_IRQn  = 11, /*!< 27 PWM0 interrupt*/
	TMR0_IRQn  = 12, /*!< 28 Timer 0 interrupt */
	TMR1_IRQn  = 13, /*!< 29 Timer 1 interrupt */
	TMR2_IRQn  = 14, /*!< 30 Timer 2 interrupt */
	TMR3_IRQn  = 15, /*!< 31 Timer 3 interrupt */
	UART0_IRQn  = 16, /*!< 32 UART0 interrupt */
	UART1_IRQn  = 17, /*!< 33 UART1 interrupt */
	I2C0_IRQn  = 18, /*!< 34 I2C0 interrupt*/
	I2C1_IRQn  = 19, /*!< 35 I2C1 interrupt*/
	PDMA_IRQn  = 20, /*!< 36 PDMA interrupt 0*/
	//Reserved = 21, /*!< 37 Reserved Interrupt*/
	SPI2_IRQn  = 22, /*!< 38 SPI2 interrupt*/
	USBD_IRQn  = 23, /*!< 39 USB device interrupt*/
	DMVD_IRQn  = 24, /*!< 39 DMIC, VAD and AED interrupt*/
	P2P0_IRQn  = 25, /*!< 40 P2P0 interrupt*/
	P2P1_IRQn  = 26, /*!< 41 P2P1 interrupt*/
	DPWM_IRQn  = 27, /*!< 42 DPWM interrupt*/
	I2S0_IRQn  = 28, /*!< 43 I2S0 interrupt*/
	I2S1_IRQn  = 29, /*!< 44 I2S1 interrupt*/
	CRPT_IRQn  = 30, /*!< 45 Crypto interrupt*/
	SPIM_IRQn  = 31, /*!< 46 SPI Master interrupt*/

} IRQn_Type;

#endif

#define INTERRUPT_LOWEST_PRIORITY   ((1 << __NVIC_PRIO_BITS) - 1)
#define INTERRUPT_PRIORITY_ABOVE_LOWEST(level)  \
								(INTERRUPT_LOWEST_PRIORITY - level)
#endif /* */
