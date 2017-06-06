/*
 *
 * file :   clocks_control_i94xxx.c
 *
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "clock_control_api.h"
#include "clock_control_i94xxx_api.h"
#include "clock_control_i94xxx.h"

#include "ISD94XXXSeries.h"

#include "clk.h"

#include "_clock_control_i94xxx_prerequirements_check.h"


/********  defines *********************/

/********  types  *********************/


/********  externals *********************/


/********  local variables *********************/

uint32_t SystemCoreClock, PllClock;
uint32_t gau32ClkSrcTbl[] = {__HXT, __LXT, 0, __LIRC, 0, 0, 0, __HIRC};
/*-----
  BSP required  function
 *------------------*/
void SystemCoreClockUpdate(void)
{
#if 1
    uint32_t u32Freq, u32ClkSrc;
    uint32_t u32HclkDiv;

    /* Update PLL Clock */
    PllClock = CLK_GetPLLClockFreq();

    u32ClkSrc = CLK->CLKSEL0 & CLK_CLKSEL0_HCLKSEL_Msk;

    if(u32ClkSrc == CLK_CLKSEL0_HCLKSEL_PLL)
    {
        /* Use PLL clock */
        u32Freq = PllClock;
    }
    else
    {
        /* Use the clock sources directly */
        u32Freq = gau32ClkSrcTbl[u32ClkSrc];
    }

    u32HclkDiv = (CLK->CLKDIV0 & CLK_CLKDIV0_HCLKDIV_Msk) + 1;

    /* Update System Core Clock */
    SystemCoreClock = u32Freq / u32HclkDiv;


    //if(SystemCoreClock == 0)
    //    __BKPT(0);

 //   CyclesPerUs = (SystemCoreClock + 500000) / 1000000;
#endif
}


static void get_parent_clock_rate(struct cfg_clk_t *cfg_clk, uint32_t *rate)
{
	struct dev_desc_t * parent_clk_dev;

	parent_clk_dev = cfg_clk->parent_clk;
	if (NULL == parent_clk_dev) CRITICAL_ERROR("bad parent clock\n");
	DEV_IOCTL_1_PARAMS(parent_clk_dev, CLK_IOCTL_GET_FREQ, rate);
}


uint8_t clock_i94xxx_xtal_ioctl( struct dev_desc_t *adev,
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


uint8_t clock_i94xxx_hirc_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	switch(aIoctl_num)
	{
	case CLK_IOCTL_SET_FREQ :
		return 1;
		break;
	case CLK_IOCTL_GET_FREQ :
		*(uint32_t*)aIoctl_param1 = 50000000;
		break;
	default :
		return 1;
	}
	return 0;
}


uint8_t clock_i94xxx_pll_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;
	uint32_t rate;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_SET_PARENT :
		if (i94xxx_xtal_clk_dev == aIoctl_param1)
		{
			CLK->PWRCTL |= CLK_PWRCTL_HXTEN_Msk;
		}
		else if (i94xxx_hirc_clk_dev == aIoctl_param1)
		{
	        CLK->PWRCTL |= CLK_PWRCTL_HIRCEN_Msk;
		}
		else
		{
			CRITICAL_ERROR("bad parent clock \n");
		}
		break;
	case CLK_IOCTL_SET_FREQ :
		rate = *(uint32_t*)aIoctl_param1;
		if (CLK->PWRCTL & CLK_PWRCTL_HXTEN_Msk)
		{
	        CLK_EnablePLL(CLK_PLLCTL_PLLSRC_HXT, rate);
		}
	    else
	    {
	        CLK_EnablePLL(CLK_PLLCTL_PLLSRC_HIRC, rate );
	    }
		cfg_clk->rate = rate;
	    break;
	case CLK_IOCTL_GET_FREQ :
		*(uint32_t*)aIoctl_param1 = CLK_GetPLLClockFreq();
		break;
	default :
		return 1;
	}
	return 0;
}


uint8_t clock_i94xxx_hclk_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;
	uint32_t rate;
	uint32_t u32HclkDiv;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_SET_PARENT :
		if (i94xxx_xtal_clk_dev == aIoctl_param1)
		{
		    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));
		}
		else if (i94xxx_hirc_clk_dev == aIoctl_param1)
		{
		    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));
		}
		else if (i94xxx_pll_clk_dev == aIoctl_param1)
		{
		    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_PLL, CLK_CLKDIV0_HCLK(1));
		}
		else
		{
			CRITICAL_ERROR("bad parent clock \n");
		}
		cfg_clk->parent_clk = aIoctl_param1;
		break;
	case CLK_IOCTL_SET_FREQ :
		rate = *(uint32_t*)aIoctl_param1;
		//TODO :
		cfg_clk->rate = rate;
	    break;
	case CLK_IOCTL_GET_FREQ :
		DEV_IOCTL_1_PARAMS(cfg_clk->parent_clk,	CLK_IOCTL_GET_FREQ, &rate);
	    u32HclkDiv = (CLK->CLKDIV0 & CLK_CLKDIV0_HCLKDIV_Msk) + 1;
		*(uint32_t*)aIoctl_param1 = rate / u32HclkDiv;
		break;
	default :
		return 1;
	}
	return 0;
}


uint8_t clock_i94xxx_pclk0_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;
	uint32_t parent_rate;
	uint32_t div;
	uint32_t req_rate;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_SET_FREQ :
		req_rate = *(uint32_t*)aIoctl_param1;
		DEV_IOCTL_1_PARAMS(i94xxx_hclk_clk_dev,
				CLK_IOCTL_GET_FREQ, &parent_rate);
		div = parent_rate / req_rate;
		if (div)
		{
			div--;
		}
		CLK->PCLKDIV = (CLK->PCLKDIV & (~CLK_PCLKDIV_APB0DIV_Msk))
			    			| (div << CLK_PCLKDIV_APB0DIV_Pos);
		cfg_clk->rate = req_rate;
		break;
	case CLK_IOCTL_GET_FREQ :
		*(uint32_t*)aIoctl_param1 = CLK_GetPCLK0Freq();
		break;
	default :
		return 1;
	}
	return 0;
}


uint8_t clock_i94xxx_pclk1_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;
	uint32_t parent_rate;
	uint32_t div;
	uint32_t req_rate;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_SET_FREQ :
		req_rate = *(uint32_t*)aIoctl_param1;
		DEV_IOCTL_1_PARAMS(i94xxx_hclk_clk_dev,
						CLK_IOCTL_GET_FREQ, &parent_rate);
		div = parent_rate / req_rate;
		if (div)
		{
			div--;
		}
		CLK->PCLKDIV = (CLK->PCLKDIV & (~CLK_PCLKDIV_APB1DIV_Msk))
			    			| (div << CLK_PCLKDIV_APB1DIV_Pos);
		cfg_clk->rate = req_rate;
		break;
	case CLK_IOCTL_GET_FREQ :
		*(uint32_t*)aIoctl_param1 = CLK_GetPCLK1Freq();
		break;
	default :
		return 1;
	}
	return 0;
}


uint8_t i94xxx_systick_clk_ioctl( struct dev_desc_t *adev,
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


uint8_t clock_i94xxx_spi2clk_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_SET_PARENT :
		if (i94xxx_xtal_clk_dev == aIoctl_param1)
		{
		    CLK_SetModuleClock(SPI2_MODULE,
		    		CLK_CLKSEL2_SPI2SEL_HXT, MODULE_NoMsk);
		}
		else if (i94xxx_hirc_clk_dev == aIoctl_param1)
		{
		    CLK_SetModuleClock(SPI2_MODULE,
		    		CLK_CLKSEL2_SPI2SEL_HIRC, MODULE_NoMsk);
		}
		else if (i94xxx_pll_clk_dev == aIoctl_param1)
		{
		    CLK_SetModuleClock(SPI2_MODULE,
		    		CLK_CLKSEL2_SPI2SEL_PLL, MODULE_NoMsk);
		}
		else if (i94xxx_pclk1_clk_dev == aIoctl_param1)
		{
		    CLK_SetModuleClock(SPI2_MODULE,
		    		CLK_CLKSEL2_SPI2SEL_PCLK0, MODULE_NoMsk);
		}
		else
		{
			CRITICAL_ERROR("bad parent clock \n");
		}
		cfg_clk->parent_clk = aIoctl_param1;
		break;
	case CLK_IOCTL_ENABLE :
	    CLK_EnableModuleClock(SPI2_MODULE);
		break;
	case CLK_IOCTL_GET_FREQ :
		get_parent_clock_rate(cfg_clk, aIoctl_param1);
		break;
	default :
		return 1;
	}
	return 0;
}


uint8_t clock_i94xxx_i2s_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;
	uint32_t curr_val;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_SET_PARENT :
		curr_val = (CLK->CLKSEL3 & ~ CLK_CLKSEL3_I2S0SEL_Msk);
		if (i94xxx_xtal_clk_dev == aIoctl_param1)
		{
			CLK->CLKSEL3 = curr_val | CLK_CLKSEL3_I2S_SEL_HXT;
		}
		else if (i94xxx_hirc_clk_dev == aIoctl_param1)
		{
			CLK->CLKSEL3 = curr_val | CLK_CLKSEL3_I2S_SEL_HIRC;
		}
		else if (i94xxx_pll_clk_dev == aIoctl_param1)
		{
			CLK->CLKSEL3 = curr_val | CLK_CLKSEL3_I2S_SEL_PLL;
		}
		else if (i94xxx_hclk_clk_dev == aIoctl_param1)
		{
			CLK->CLKSEL3 = curr_val | CLK_CLKSEL3_I2S_SEL_HCLK;
		}
		else
		{
			CRITICAL_ERROR("bad parent clock \n");
		}
		cfg_clk->parent_clk = aIoctl_param1;
		break;
	case CLK_IOCTL_ENABLE :
		CLK->APBCLK0 |= CLK_APBCLK0_I2S0CKEN_Msk;
		break;
	case CLK_IOCTL_GET_FREQ :
		get_parent_clock_rate(cfg_clk, aIoctl_param1);
		break;
	default :
		return 1;
	}
	return 0;
}


uint8_t clock_i94xxx_dpwm_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;
	uint32_t curr_val;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_SET_PARENT :
		curr_val = (CLK->CLKSEL2 & ~ CLK_CLKSEL2_DPWMSEL_Msk);
		if (i94xxx_xtal_clk_dev == aIoctl_param1)
		{
			CLK->CLKSEL2 = curr_val | CLK_CLKSEL2_DPWMSEL_HXT;
		}
		else if (i94xxx_hirc_clk_dev == aIoctl_param1)
		{
			CLK->CLKSEL2 = curr_val | CLK_CLKSEL2_DPWMSEL_HIRC;
		}
		else if (i94xxx_pll_clk_dev == aIoctl_param1)
		{
			CLK->CLKSEL2 = curr_val | CLK_CLKSEL2_DPWMSEL_PLL;
		}
		else if (i94xxx_pclk0_clk_dev == aIoctl_param1)
		{
			CLK->CLKSEL2 = curr_val | CLK_CLKSEL2_DPWMSEL_PCLK0;
		}
		else
		{
			CRITICAL_ERROR("bad parent clock \n");
		}
		cfg_clk->parent_clk = aIoctl_param1;
		break;
	case CLK_IOCTL_ENABLE :
		CLK->APBCLK1 |= CLK_APBCLK1_DPWMCKEN_Msk;
		break;
	case CLK_IOCTL_GET_FREQ :
		get_parent_clock_rate(cfg_clk, aIoctl_param1);
		break;
	default :
		return 1;
	}
	return 0;
}



uint8_t clock_i94xxx_uart0clk_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_SET_PARENT :
		if (i94xxx_xtal_clk_dev == aIoctl_param1)
		{
		    CLK_SetModuleClock(UART0_MODULE,
		    		CLK_CLKSEL1_UART0SEL_HXT, CLK_CLKDIV0_UART0(1));
		}
		else if (i94xxx_hirc_clk_dev == aIoctl_param1)
		{
		    CLK_SetModuleClock(UART0_MODULE,
		    		CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));
		}
		else if (i94xxx_pll_clk_dev == aIoctl_param1)
		{
		    CLK_SetModuleClock(UART0_MODULE,
		    		CLK_CLKSEL1_UART0SEL_PLL, CLK_CLKDIV0_UART0(1));
		}
		else
		{
			CRITICAL_ERROR("bad parent clock \n");
		}
		cfg_clk->parent_clk = aIoctl_param1;
		break;
	case CLK_IOCTL_ENABLE :
	    CLK_EnableModuleClock(UART0_MODULE);
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
 * clock_control_i94xxx_ioctl()
 *
 * return:
 */
uint8_t clock_control_i94xxx_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct clk_cntl_i94xxx_cfg_t *cfg_hndl;
	uint32_t rate;
	struct dev_desc_t	*parent_clk_dev;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);

	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		/* Enable HIRC, HXT and LXT clock */
	    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
	    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);
	    CLK_EnableXtalRC(CLK_PWRCTL_LXTEN_Msk);

	    /* Wait for HIRC, HXT and LXT clock ready */
	    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
	    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);
	    CLK_WaitClockReady(CLK_STATUS_LXTSTB_Msk);

		DEV_IOCTL_1_PARAMS(i94xxx_xtal_clk_dev,	CLK_IOCTL_GET_FREQ, &rate);

		if (0 != rate)/*using XTAL if possible*/
		{
			parent_clk_dev = i94xxx_xtal_clk_dev;
		}
		else
		{
			parent_clk_dev = i94xxx_hirc_clk_dev;
		}
		DEV_IOCTL_1_PARAMS(i94xxx_pll_clk_dev,
				CLK_IOCTL_SET_PARENT, parent_clk_dev);
		DEV_IOCTL_1_PARAMS(i94xxx_pll_clk_dev,
				CLK_IOCTL_SET_FREQ, &cfg_hndl->pll_rate);

		DEV_IOCTL_1_PARAMS(i94xxx_hclk_clk_dev,
				CLK_IOCTL_SET_PARENT, i94xxx_pll_clk_dev);
		DEV_IOCTL_1_PARAMS(i94xxx_hclk_clk_dev,
				CLK_IOCTL_SET_FREQ, &cfg_hndl->hclk_rate);


		DEV_IOCTL_1_PARAMS(i94xxx_hclk_clk_dev,	CLK_IOCTL_GET_FREQ, &rate);

		/* PCLK cannot be greater than 80mhz*/
	    if (160000000 < rate)
	    {
	    	rate = rate / 4;
	    }
	    else if (8000000 < rate)
	    {
	    	rate = rate / 2;
	    }
		DEV_IOCTL_1_PARAMS(i94xxx_pclk0_clk_dev, CLK_IOCTL_SET_FREQ, &rate);
		DEV_IOCTL_1_PARAMS(i94xxx_pclk1_clk_dev, CLK_IOCTL_SET_FREQ, &rate);

		break;


	default :
		return 1;
	}
	return 0;
}
