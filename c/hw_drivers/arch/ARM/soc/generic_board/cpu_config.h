/*
 * file : cpu_config.h
 *
 */

#ifndef _cpu_config_H
#define _cpu_config_H

#include "project_config.h"

#if defined(INCLUDE_FPU_IN_GENERIC_ARM_SOC)
	#define __FPU_PRESENT             1         /*!< FPU present or not                               */
#endif

#ifdef CONFIG_CORTEX_M4
	#define ARM_MATH_CM4   1
	#define __FPU_PRESENT  1
	#define __FPU_USED     1
	/* Configuration of the Cortex-M# Processor and Core Peripherals */
	#define __CM4_REV              0x0201    /*!< Core Revision r2p1    */
	#define __NVIC_PRIO_BITS   4  /*!< Number of Bits used for Priority Levels*/
	#define __Vendor_SysTickConfig 1/*!< 1 if different SysTick Config is used*/
	#define __MPU_PRESENT          1         /*!< MPU present or not */
#endif

#define NUMBER_OF_NVIC_EXTERNAL_INTERRUPTS   255


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

} IRQn_Type;

#define INTERRUPT_LOWEST_PRIORITY    ((1 << __NVIC_PRIO_BITS) - 1)
#define INTERRUPT_PRIORITY_ABOVE_LOWEST(level)  \
								(INTERRUPT_LOWEST_PRIORITY - level)

#endif /* */
