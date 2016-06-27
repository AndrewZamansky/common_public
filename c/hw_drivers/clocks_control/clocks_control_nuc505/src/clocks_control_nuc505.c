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
#include "auto_init_api.h"

#include "clocks_api.h"

#include "NUC505Series.h"

#include "clk.h"
#include "spim.h"

uint32_t SystemCoreClock = __HSI;               /*!< System Clock Frequency (Core Clock)*/
uint32_t CyclesPerUs      = (__HSI / 1000000);  /*!< Cycles per micro second            */

/********  defines *********************/

/********  types  *********************/


/********  externals *********************/


/********  local defs *********************/



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





uint8_t xtal_set_clock(uint32_t rate)
{
	return 0;
}
uint32_t xtal_get_clock(void )
{
	return CONFIG_DT_XTAL_CLOCK_RATE;
}
clocks_common_t input_xtal_clock = {CONFIG_DT_XTAL_CLOCK_RATE , xtal_set_clock , xtal_get_clock};



uint8_t core_set_clock(uint32_t rate)
{

	CLK->PWRCTL |= CLK_PWRCTL_HXTEN_Msk;

	CLK_SetCoreClock(rate);
    /* Set PCLK divider */
    CLK_SetModuleClock(PCLK_MODULE, 0 , 1);

    /* Update System Core Clock */
    SystemCoreClockUpdate();
    return 0;
}
uint32_t core_get_clock(void )
{
	return SystemCoreClock;;
}
clocks_common_t core_clock = {0 , core_set_clock , core_get_clock};


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        clocks_api_init                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void clocks_init(void)
{


    clocks_api_add_clock(CONFIG_DT_XTAL_CLOCK, &input_xtal_clock);
    clocks_api_add_clock(CONFIG_DT_CORE_CLOCK, &core_clock);


}

AUTO_INIT_FUNCTION(clocks_init);
