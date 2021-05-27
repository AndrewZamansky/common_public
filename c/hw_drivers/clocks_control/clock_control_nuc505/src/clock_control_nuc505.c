/*
 *
 * file :   clocks_control_nuc505.c
 *
 *
 */


#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "clock_control_api.h"
#include "clock_control_nuc505_api.h"
#include "clock_control_nuc505.h"

#include "NUC505Series.h"

#include "clk.h"
#include "spim.h"


uint32_t SystemCoreClock = __HSI;  /*!< System Clock Frequency (Core Clock)*/
uint32_t CyclesPerUs      = (__HSI / 1000000);  /*!< Cycles per micro second */



static uint32_t gau32ClkSrcTbl[] = {__HXT, 0};/*!< System clock source table */

/*----------------------------------------------------------------------------
  Clock functions
 *---------------------------------------------------------------------------*/
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

	//added by az to improve flash speed . MAX flash speed is 80Mhz :
	SPIM_SetBusClock(SPIM,SystemCoreClock/2);


	SPIM_ENABLE_DMM_MODE(SPIM, SPIM_CTL0_CMDCODE_FAST_READ_QUAD_IO , 0);
}





static void get_parent_clock_rate(struct cfg_clk_t *cfg_clk, uint32_t *rate)
{
	struct dev_desc_t * parent_clk_dev;

	parent_clk_dev = cfg_clk->parent_clk;
	if (NULL == parent_clk_dev) CRITICAL_ERROR("bad parent clock\n");
	DEV_IOCTL_1_PARAMS(parent_clk_dev, CLK_IOCTL_GET_FREQ, rate);
}


uint8_t clock_nuc505_xtal_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_SET_FREQ :
		return 1;
		break;
	case CLK_IOCTL_GET_FREQ :
		*(uint32_t*)aIoctl_param1 = cfg_clk->rate;
		break;
	default :
		return 1;
	}
	return 0;
}


uint8_t clock_nuc505_core_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;
	uint32_t rate;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_SET_PARENT :
		//TODO:
		CRITICAL_ERROR("bad parent clock \n");
		cfg_clk->parent_clk = aIoctl_param1;
	case CLK_IOCTL_SET_FREQ :
		rate = *(uint32_t*)aIoctl_param1;
		CLK->PWRCTL |= CLK_PWRCTL_HXTEN_Msk;

		CLK_SetCoreClock(rate);
		/* Set PCLK divider */
		CLK_SetModuleClock(PCLK_MODULE, 0 , 1);

		/* Update System Core Clock */
		SystemCoreClockUpdate();
		break;
	case CLK_IOCTL_GET_FREQ :
		*(uint32_t*)aIoctl_param1 = SystemCoreClock;//cfg_clk->rate;
		//get_parent_clock_rate(cfg_clk, aIoctl_param1);
		break;
	default :
		return 1;
	}
	return 0;
}


uint8_t nuc505_systick_clk_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_SET_PARENT :
		//TODO:
		CRITICAL_ERROR("bad parent clock \n");
		cfg_clk->parent_clk = aIoctl_param1;
	case CLK_IOCTL_SET_FREQ :
		//TODO:
		break;
	case CLK_IOCTL_GET_FREQ :
		get_parent_clock_rate(cfg_clk, aIoctl_param1);
		break;
	default :
		return 1;
	}
	return 0;
}

/**
 * clock_control_nuc505_ioctl()
 *
 * return:
 */
uint8_t clock_control_nuc505_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct clk_cntl_nuc505_cfg_t *cfg_hndl;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);

	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		DEV_IOCTL_1_PARAMS(nuc505_core_clk_dev,
				CLK_IOCTL_SET_FREQ, &cfg_hndl->core_rate);

		break;


	default :
		return 1;
	}
	return 0;
}
