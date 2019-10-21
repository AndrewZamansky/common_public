/*
 * file : cpu_config.h
 *
 *
 */

#ifndef _cpu_config_H
#define _cpu_config_H

/*  just copy from I94100.h */

#define ARM_MATH_CM4	1

#ifndef __ASSEMBLER__
/*IRQn_Type can be defined in different files describing different socs .
 so it needs to be disabled in api functions */
#define IRQn_Type IRQn_Type_TMP
#include "I94100.h"
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
/******  Cortex-M4 Processor Exceptions Numbers ****************************/
	NonMaskableInt_IRQn = -14,      /*!<  2 Non Maskable Interrupt          */
	MemoryManagement_IRQn  = -12,      /*!<  4 Memory Management Interrupt  */
	BusFault_IRQn       = -11,      /*!<  5 Bus Fault Interrupt             */
	UsageFault_IRQn     = -10,      /*!<  6 Usage Fault Interrupt           */
	SVCall_IRQn         = -5,       /*!< 11 SV Call Interrupt               */
	DebugMonitor_IRQn   = -4,       /*!< 12 Debug Monitor Interrupt         */
	PendSV_IRQn         = -2,       /*!< 14 Pend SV Interrupt               */
	SysTick_IRQn        = -1,       /*!< 15 System Tick Interrupt           */

/******  ISD94XXX Specific Interrupt Numbers *******************************/

	BODOUT_IRQn     = 0,
	IRC_IRQn        = 1,
	PWRWU_IRQn      = 2,
	SRAM_IRQn       = 3,
	CLKFAIL_IRQn    = 4,
	Reserved   	    = 5,
	RTC_IRQn        = 6,
	WDT_IRQn        = 8,
	WWDT_IRQn       = 9,
	EINT0_IRQn      = 10,
	EINT1_IRQn      = 11,
	EINT2_IRQn      = 12,
	EINT3_IRQn      = 13,
	EINT4_IRQn      = 14,
	EINT5_IRQn      = 15,
	GPA_IRQn        = 16,
	GPB_IRQn        = 17,
	GPC_IRQn        = 18,
	GPD_IRQn        = 19,
	SPI0_INT        = 22,
	SPI1_INT        = 23,
	PWM0_P0_IRQn    = 25,
	PWM0_P1_IRQn    = 26,
	PWM0_P2_IRQn    = 27,
	TMR0_IRQn       = 32,
	TMR1_IRQn       = 33,
	TMR2_IRQn       = 34,
	TMR3_IRQn       = 35,
	UART0_IRQn      = 36,
	I2C0_IRQn       = 38,
	I2C1_IRQn       = 39,
	PDMA_IRQn       = 40,
	EADC0_IRQn      = 42,
	EADC1_IRQn      = 43,
	EADC2_IRQn      = 46,
	EADC3_IRQn      = 47,
	DMIC_IRQn       = 52,
	USBD_IRQn       = 53,
	DPWM_IRQn       = 62,
	I2S0_IRQn       = 68,

} IRQn_Type;

#endif

#define INTERRUPT_LOWEST_PRIORITY   ((1 << __NVIC_PRIO_BITS) - 1)
#define INTERRUPT_PRIORITY_ABOVE_LOWEST(level)  \
								(INTERRUPT_LOWEST_PRIORITY - level)
#endif /* */
