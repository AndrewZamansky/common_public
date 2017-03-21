/******************************************************************************
 * @file     system_M451Series.h
 * @version  V0.10
 * $Revision: 1 $
 * $Date: 16/06/14 10:24a $ 
 * @brief    CMSIS Cortex-M4 Core Peripheral Access Layer Header File for M451 Series MCU
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#ifndef __SYSTEM_M451SERIES_H__   
#define __SYSTEM_M451SERIES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <_project.h>


/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/

	
#define __HSI       	(12000000UL)    /*!< PLL default output is 72MHz */
#ifdef DEF_FPGA	
	#define __HXT       (CONFIG_CRYSTAL_CLOCK) /*(12000000UL)*/    /*!< External Crystal Clock Frequency     */
#else
	#define __HXT       (CONFIG_CRYSTAL_CLOCK) /*(12000000UL)*/    /*!< External Crystal Clock Frequency     */
#endif
#define __LXT       	(32768UL)       /*!< External Crystal Clock Frequency 32.768KHz */

#ifdef DEF_FPGA
	#define __HIRC      (12000000UL)    /*!< FPGA - from OSC5 */
#else
	#define __HIRC      (22118400UL)    /*!< Internal 22M RC Oscillator Frequency */
#endif
#define __LIRC      	(10000UL)       /*!< Internal 10K RC Oscillator Frequency */
#define __SYS_OSC_CLK     (    ___HSI)    /* Main oscillator frequency        */


#define __SYSTEM_CLOCK    (1*__HXT)

	
#define __XTAL      (CONFIG_CRYSTAL_CLOCK) /*(12000000UL)*/   /*!< External Crystal Clock Frequency     */
#define __SYS_OSC_CLK     (    ___HSI)    /* Main oscillator frequency        */




extern uint32_t SystemCoreClock;     /*!< System Clock Frequency (Core Clock)  */
extern uint32_t CyclesPerUs;         /*!< Cycles per micro second              */
extern uint32_t PllClock;            /*!< PLL Output Clock Frequency           */


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

#endif /* __SYSTEM_M451SERIES_H__ */
/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
