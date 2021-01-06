/**************************************************************************//**
 * @file     system_I96100.h
 * @brief    CMSIS Cortex-M0 Device System Header File
 *           for CM0 Device Series
 * @version  V1.01
 * @date     19/04/11
 *
 * @note
 * Copyright (C) 2019 ARM Limited. All rights reserved.
 *
 * @par
 * ARM Limited (ARM) is supplying this software for use with Cortex-M
 * processor based microcontrollers.  This file can be freely distributed
 * within development tools that are supporting such ARM based processors.
 *
 * @par
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/


#ifndef __SYSTEM_I96100_H__
#define __SYSTEM_I96100_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
/*----------------------------------------------------------------------------
  Define system clocks sources
 *----------------------------------------------------------------------------*/
/*!< Low Speed External Crystal Clock Frequency 32.768kHz */
//#define __LXT (32768UL)
extern uint32_t __LXT;

#define __LIRC  (10000UL) /*!< Low speed internal Oscillator Frequency 10kHz */

/*!< High Speed Internal 49MHz RC Oscillator Frequency */
//#define __HIRC        (24576000UL)//(12000000UL)////
extern uint32_t __HIRC;

/*!< High Speed External Crystal Clock Frequency 12.288MHz */
//#define __HXT        	(12000000UL)
extern uint32_t __HXT;
extern uint32_t __MCLKI;
extern uint32_t __XCLK;

#define __HSI         (__HIRC)      /* Factory Default is internal __HIRC */

extern uint32_t SystemCoreClock;    /*!< System Clock Frequency (Core Clock)  */
extern uint32_t CyclesPerUs;        /*!< Cycles per micro second              */
extern uint32_t PllClock;           /*!< PLL Output Clock Frequency           */

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System and update the SystemCoreClock variable.
 */
extern void SystemInit (void);

/**
 * Update SystemCoreClock variable
 *
 * @param  none
 * @return none
 *
 * @brief  Updates the SystemCoreClock with current core Clock
 *         retrieved from cpu registers.
 */
extern void SystemCoreClockUpdate (void);

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_I96XXX_H__ */
