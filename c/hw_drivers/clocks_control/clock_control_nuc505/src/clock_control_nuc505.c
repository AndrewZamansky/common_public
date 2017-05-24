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

#include "dev_management_api.h"

#include "clock_control_api.h"
#include "clock_control_nuc505_api.h"
#include "clock_control_nuc505.h"

#include "NUC505Series.h"

#include "clk.h"
#include "spim.h"

#include "_clock_control_nuc505_prerequirements_check.h"

uint32_t SystemCoreClock = __HSI;               /*!< System Clock Frequency (Core Clock)*/
uint32_t CyclesPerUs      = (__HSI / 1000000);  /*!< Cycles per micro second            */

/********  defines *********************/

/********  types  *********************/

typedef uint32_t (*clock_get_func_t)(void )  ;
typedef uint8_t (*clock_set_func_t)(uint32_t )  ;

typedef struct
{
	clock_set_func_t clock_set_func;
	clock_get_func_t clock_get_func;
}clocks_nuc505_t;

/********  externals *********************/


/********  local variables *********************/
static clocks_nuc505_t clocks_array[];


static uint32_t gau32ClkSrcTbl[] = {__HXT, 0}; 				/*!< System clock source table */

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





static uint8_t xtal_set_clock(uint32_t rate)
{
	return 0;
}
static uint32_t xtal_get_clock(void )
{
	return CONFIG_CRYSTAL_CLOCK;
}


static uint8_t core_set_clock(uint32_t rate)
{

	CLK->PWRCTL |= CLK_PWRCTL_HXTEN_Msk;

	CLK_SetCoreClock(rate);
    /* Set PCLK divider */
    CLK_SetModuleClock(PCLK_MODULE, 0 , 1);

    /* Update System Core Clock */
    SystemCoreClockUpdate();
    return 0;
}
static uint32_t core_get_clock(void )
{
	return SystemCoreClock;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        clocks_control_nuc505_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t clock_control_nuc505_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	size_t clock_index;
	clock_control_nuc505_instance_t *config_handle;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);
	clock_index = (size_t)aIoctl_param1 ;
	switch(aIoctl_num)
	{
		case IOCTL_CLOCK_CONTROL_GET_RATE :
			if(NUC505_TOTAL_NUM_OF_CLOCKS <= clock_index )
			{
				*(uint32_t*)aIoctl_param2 = 0;
				return 1;
			}
			else
			{
				*(uint32_t*)aIoctl_param2 = clocks_array[clock_index].clock_get_func();
			}

			break;

		case IOCTL_CLOCK_CONTROL_SET_RATE :
			if(NUC505_TOTAL_NUM_OF_CLOCKS <= clock_index )
			{
				return 1;
			}
			else
			{
				return clocks_array[clock_index].clock_set_func((uint32_t)aIoctl_param2);
			}
			break;

		case IOCTL_DEVICE_START :
			for(clock_index =0; clock_index < NUC505_TOTAL_NUM_OF_CLOCKS ; clock_index++)
			{
				clocks_nuc505_t *p_curr_clock;
				p_curr_clock = &clocks_array[clock_index];
				p_curr_clock->clock_set_func(config_handle->initial_clock_rates[clock_index]);
			}
			break;


		default :
			return 1;
	}
	return 0;
}

static clocks_nuc505_t clocks_array[NUC505_TOTAL_NUM_OF_CLOCKS] =
{
		{xtal_set_clock , xtal_get_clock},
		{core_set_clock , core_get_clock}
};
