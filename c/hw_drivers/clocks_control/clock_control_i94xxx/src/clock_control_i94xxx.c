/*
 *
 * file :   clocks_control_i94xxx.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/
#include "_project.h"

#include "clock_control_api.h"
#include "clock_control_i94xxx_api.h"
#include "clock_control_i94xxx.h"

#include "ISD94XXXSeries.h"

#include "clk.h"
//#include "spim.h"



/********  defines *********************/

/********  types  *********************/

typedef uint32_t (*clock_get_func_t)(void )  ;
typedef uint8_t (*clock_set_func_t)(uint32_t )  ;

typedef struct
{
	clock_set_func_t clock_set_func;
	clock_get_func_t clock_get_func;
}clocks_i94xxx_t;

/********  externals *********************/


/********  local variables *********************/
static clocks_i94xxx_t clocks_array[];

/*!< System Clock Frequency (Core Clock)*/
uint32_t SystemCoreClock  = __SYSTEM_CLOCK;

uint32_t CyclesPerUs      = (__HSI / 1000000); /* Cycles per micro second */
uint32_t PllClock         = __HSI;   /*!< PLL Output Clock Frequency     */
uint32_t gau32ClkSrcTbl[] = {__HXT, __LXT, 0, __LIRC, 0, 0, 0, __HIRC};


/*----------------------------------------------
 *Clock functions
 *-------------------------------------------------*/
void SystemCoreClockUpdate (void)
{

	   uint32_t u32Freq, u32ClkSrc;
	    uint32_t u32HclkDiv;

	    /* Update PLL Clock */
	    PllClock = CLK_GetPLLClockFreq();

	    u32ClkSrc = CLK->CLKSEL0 & CLK_CLKSEL0_HCLKSEL_Msk;

	    if(u32ClkSrc != CLK_CLKSEL0_HCLK_PLL)
	    {
	        /* Use the clock sources directly */
	        u32Freq = gau32ClkSrcTbl[(u32ClkSrc >> CLK_CLKSEL0_HCLKSEL_Pos)];
	    }
	    else
	    {
	        /* Use PLL clock */
	        u32Freq = PllClock;
	    }

	    u32HclkDiv = (CLK->CLKDIV0 & CLK_CLKDIV0_HCLKDIV_Msk) + 1;

	    /* Update System Core Clock */
	    SystemCoreClock = u32Freq/u32HclkDiv;

	    CyclesPerUs = (SystemCoreClock + 500000) / 1000000;

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

    /* Enable HIRC, HXT and LXT clock */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);
    CLK_EnableXtalRC(CLK_PWRCTL_LXTEN_Msk);

    /* Wait for HIRC, HXT and LXT clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);
    CLK_WaitClockReady(CLK_STATUS_LXTSTB_Msk);

    /* Select HCLK clock source as HIRC and and HCLK clock divider as 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    CLK->PWRCTL |= CLK_PWRCTL_HXTEN_Msk;

	CLK_SetCoreClock(rate);
//    /* Set PCLK divider */
//    CLK_SetModuleClock(PCLK_MODULE, 0 , 1);

    /* Update System Core Clock */
    SystemCoreClockUpdate();
    return 0;
}
static uint32_t core_get_clock(void )
{
	return SystemCoreClock;
}


static uint8_t uart0_set_clock(uint32_t rate)
{
    CLK_SetModuleClock(UART0_MODULE,
    		CLK_CLKSEL1_UART0SEL_HXT, CLK_CLKDIV0_UART0(1));
	return 0;
}

static uint32_t uart0_get_clock(void )
{
	return (CONFIG_CRYSTAL_CLOCK/2);
}

/**
 * clock_control_i94xxx_ioctl()
 *
 * return:
 */
uint8_t clock_control_i94xxx_ioctl( pdev_descriptor_t apdev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	size_t clock_index;
	clock_control_i94xxx_instance_t *config_handle;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(apdev);
	clock_index = (size_t)aIoctl_param1 ;
	switch(aIoctl_num)
	{
	case IOCTL_CLOCK_CONTROL_GET_RATE :
		if(I94XXX_TOTAL_NUM_OF_CLOCKS <= clock_index )
		{
			*(uint32_t*)aIoctl_param2 = 0;
			return 1;
		}
		else
		{
			*(uint32_t*)aIoctl_param2 =
					clocks_array[clock_index].clock_get_func();
		}

		break;

	case IOCTL_CLOCK_CONTROL_SET_RATE :
		if(I94XXX_TOTAL_NUM_OF_CLOCKS <= clock_index )
		{
			return 1;
		}
		else
		{
			return clocks_array[clock_index].
					clock_set_func((uint32_t)aIoctl_param2);
		}
		break;

	case IOCTL_DEVICE_START :
		for (clock_index =0;
				clock_index < I94XXX_TOTAL_NUM_OF_CLOCKS ; clock_index++)
		{
			clocks_i94xxx_t *p_curr_clock;
			p_curr_clock = &clocks_array[clock_index];
			p_curr_clock->clock_set_func(
					config_handle->initial_clock_rates[clock_index]);
		}
		break;


	default :
		return 1;
	}
	return 0;
}

static clocks_i94xxx_t clocks_array[I94XXX_TOTAL_NUM_OF_CLOCKS] =
{
		{xtal_set_clock , xtal_get_clock},
		{core_set_clock , core_get_clock},
		{uart0_set_clock , uart0_get_clock}
};
