/*
 *
 * file :   clocks_control_nuc505.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

#include "NUC505Series.h"

#include "clk.h"
#include "spim.h"

uint32_t SystemCoreClock = __HSI;               /*!< System Clock Frequency (Core Clock)*/
uint32_t CyclesPerUs      = (__HSI / 1000000);  /*!< Cycles per micro second            */

/********  defines *********************/

/********  types  *********************/


/********  externals *********************/


/********  local defs *********************/

/*----------------------------------------------------------------------------
  Clock Variable definitions
 *----------------------------------------------------------------------------*/
#define __HXT         (12000000UL)  /*!< High Speed External Crystal Clock Frequency 12MHz */
#define __LXT         (32768UL)     /*!< Low Speed External Crystal Clock Frequency 32.768kHz */
#define __HIRC        (22118400UL)  /*!< High Speed Internal 22MHz RC Oscillator Frequency */
#define __LIRC        (10000UL)     /*!< Low Speed Internal 10kHz RC Oscillator Frequency */
#define __HSI         (__HIRC)      /* Factory Default is internal 12MHz */


uint32_t gau32ClkSrcTbl[] = {__HXT, 0}; 				/*!< System clock source table */

/*----------------------------------------------------------------------------
  Clock functions
 *----------------------------------------------------------------------------*/
void SystemCoreClockUpdate (void)            /* Get Core Clock Frequency      */
{

    uint32_t u32Freq, u32ClkSrc;
    uint32_t u32HclkDiv;

    u32ClkSrc = CLK->CLKDIV0 & CLK_CLKDIV0_HCLKSEL_Msk;

    if(u32ClkSrc != CLK_CLKDIV0_HCLKSEL_Msk) {
        /* Use the clock sources directly */
        u32Freq = gau32ClkSrcTbl[u32ClkSrc];
    } else {
        /* Use PLL clock */
        u32Freq = CLK_GetPLLClockFreq();
    }

    u32HclkDiv = (CLK->CLKDIV0 & CLK_CLKDIV0_HCLKDIV_Msk) + 1;

    /* Update System Core Clock */
    SystemCoreClock = u32Freq/u32HclkDiv;

    CyclesPerUs = (SystemCoreClock + 500000) / 1000000;
    SPIM_SetBusClock(SPIM,SystemCoreClock/2);//added by az to improve flash speed . MAX flash speed is 80Mhz
    SPIM_ENABLE_DMM_MODE(SPIM, SPIM_CTL0_CMDCODE_FAST_READ_QUAD_IO , 0);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        clocks_control_nuc505_init                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t  clocks_control_nuc505_init(void)
{

	CLK->PWRCTL |= CLK_PWRCTL_HXTEN_Msk;

	CLK_SetCoreClock(CONFIG_CORE_CLOCK);
    /* Set PCLK divider */
    CLK_SetModuleClock(PCLK_MODULE, 0 , 1);

    /* Update System Core Clock */
    SystemCoreClockUpdate();

    return 0;

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        clocks_control_nuc505_get_cpu_clock                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t clocks_control_nuc505_get_cpu_clock(void )
{
	//SystemCoreClockUpdate();

	return SystemCoreClock;
}
