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
#include "clock_control_common_api.h"
#include "clock_control_i94xxx.h"

#include "I94100.h"

#include "clk.h"
#include "i2s.h"



/********  defines *********************/

/********  types  *********************/


/********  externals *********************/


/********  local variables *********************/

/*-----
  SystemCoreClock,__HXT, __LXT, __HIRC are BSP required  variables.
  add weak attribute for case when we compile library so this variable
  declared outside of library
 *------------------*/
uint32_t SystemCoreClock __attribute__((weak));
uint32_t __HXT __attribute__((weak));
uint32_t __LXT __attribute__((weak));
uint32_t __HIRC __attribute__((weak));
static uint32_t gau32ClkSrcTbl[] = {0, 0, 0, __LIRC, 0, 0, 0, 0};

/*-----
  SystemCoreClockUpdate() BSP required  function.
  add weak attribute for case when we compile library so this function can
  be implemented outside of library
 *------------------*/
__attribute__((weak)) void SystemCoreClockUpdate(void)
{
	uint32_t  PllClock;

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
	//	__BKPT(0);

 //   CyclesPerUs = (SystemCoreClock + 500000) / 1000000;
}


uint8_t clock_i94xxx_xtal_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;
	uint32_t rate;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_ENABLE :
		CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);
		CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);
		break;
	case CLK_IOCTL_SET_FREQ :
		rate = *(uint32_t*)aIoctl_param1;
		cfg_clk->rate = rate;
		__HXT = rate;
		gau32ClkSrcTbl[0] = rate;
		break;
	case CLK_IOCTL_GET_FREQ :
		*(uint32_t*)aIoctl_param1 = cfg_clk->rate;
		break;
	default :
		return 1;
	}
	return 0;
}


uint8_t clock_i94xxx_lxtal_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;
	uint32_t rate;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_ENABLE :
		CLK_EnableXtalRC(CLK_PWRCTL_LXTEN_Msk);
		CLK_WaitClockReady(CLK_STATUS_LXTSTB_Msk);
		break;
	case CLK_IOCTL_SET_FREQ :
		rate = *(uint32_t*)aIoctl_param1;
		cfg_clk->rate = rate;
		__LXT = rate;
		gau32ClkSrcTbl[1] = rate;
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
	struct cfg_clk_t *cfg_clk;
	uint32_t rate;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);

	switch(aIoctl_num)
	{
	case CLK_IOCTL_ENABLE :
		CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
		CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
		break;
	case CLK_IOCTL_SET_FREQ :
		rate = *(uint32_t*)aIoctl_param1;
		if (48000000 == rate)
		{
			CLK_SELECT_TRIM_HIRC(CLK_CLKSEL0_HIRCFSEL_48M);
		}
		else if (49152000 == rate)
		{
			CLK_SELECT_TRIM_HIRC(CLK_CLKSEL0_HIRCFSEL_49M);
		}
		else
		{
			CRITICAL_ERROR("bad clock rate \n");
		}
		__HIRC = rate;
		cfg_clk->rate = rate;
		break;
	case CLK_IOCTL_GET_FREQ :
		*(uint32_t*)aIoctl_param1 = cfg_clk->rate;
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
			CLK->PLLCTL &= ~CLK_PLLCTL_PLLSRC_Msk;
		}
		else if (i94xxx_hirc_clk_dev == aIoctl_param1)
		{
			CLK->PLLCTL |= CLK_PLLCTL_PLLSRC_Msk;
		}
		else
		{
			CRITICAL_ERROR("bad parent clock \n");
		}
		cfg_clk->parent_clk = aIoctl_param1;
		break;
	case CLK_IOCTL_SET_FREQ :
		rate = *(uint32_t*)aIoctl_param1;
		if (CLK->PLLCTL & CLK_PLLCTL_PLLSRC_Msk)
		{
			CLK_EnablePLL(CLK_PLLCTL_PLLSRC_HIRC, rate );
		}
		else
		{
			CLK_EnablePLL(CLK_PLLCTL_PLLSRC_HXT, rate);
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


uint8_t clock_i94xxx_usb_ioctl( struct dev_desc_t *adev,
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
	case CLK_IOCTL_SET_PARENT :
		if (i94xxx_hirc_clk_dev == aIoctl_param1)
		{
			//CLK->CLKSEL4 &= ~(0x1ul << 24); //CLKSEL4 not exist yet in BSP
			*((uint32_t *)0x40000224) &= ~(0x1ul << 24);
		}
		else if (i94xxx_pll_clk_dev == aIoctl_param1)
		{
			//CLK->CLKSEL4 |= (0x1ul << 24); //CLKSEL4 not exist yet in BSP
			*((uint32_t *)0x40000224) |= (0x1UL<<24);
		}
		else
		{
			CRITICAL_ERROR("bad parent clock \n");
		}
		cfg_clk->parent_clk = aIoctl_param1;
		break;
	case CLK_IOCTL_ENABLE :
		CLK_EnableModuleClock(USBD_MODULE);
		break;
	case CLK_IOCTL_SET_FREQ :
		req_rate = *(uint32_t*)aIoctl_param1;
		DEV_IOCTL_1_PARAMS(cfg_clk->parent_clk,
						CLK_IOCTL_GET_FREQ, &parent_rate);

		div = (parent_rate / req_rate);
		if (div)
		{
			div--;
		}

		CLK->CLKDIV0 = (CLK->CLKDIV0 & (~CLK_CLKDIV0_USBDIV_Msk))
							| (div << CLK_CLKDIV0_USBDIV_Pos);
		cfg_clk->rate = req_rate;
		break;
	case CLK_IOCTL_GET_FREQ :
		DEV_IOCTL_1_PARAMS(cfg_clk->parent_clk,
						CLK_IOCTL_GET_FREQ, &parent_rate);
		div = ((CLK->CLKDIV0 & CLK_CLKDIV0_USBDIV_Msk) >>
				CLK_CLKDIV0_USBDIV_Pos) + 1;
		*(uint32_t*)aIoctl_param1 = parent_rate / div;
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
		break;

	case CLK_IOCTL_SET_FREQ :
		//TODO:
		break;
	default :
		return common_clock_api_standard_clk_ioctls(
				adev, aIoctl_num, aIoctl_param1, aIoctl_param2);
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
	default :
		return common_clock_api_standard_clk_ioctls(
				adev, aIoctl_num, aIoctl_param1, aIoctl_param2);
	}
	return 0;
}

uint8_t clock_i94xxx_spi1clk_ioctl( struct dev_desc_t *adev,
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
			CLK_SetModuleClock(SPI1_MODULE,
					CLK_CLKSEL2_SPI1SEL_HXT, MODULE_NoMsk);
		}
		else if (i94xxx_hirc_clk_dev == aIoctl_param1)
		{
			CLK_SetModuleClock(SPI1_MODULE,
					CLK_CLKSEL2_SPI1SEL_HIRC, MODULE_NoMsk);
		}
		else if (i94xxx_pll_clk_dev == aIoctl_param1)
		{
			CLK_SetModuleClock(SPI1_MODULE,
					CLK_CLKSEL2_SPI1SEL_PLL, MODULE_NoMsk);
		}
		else if (i94xxx_pclk1_clk_dev == aIoctl_param1)
		{
			CLK_SetModuleClock(SPI1_MODULE,
					CLK_CLKSEL2_SPI1SEL_PCLK1, MODULE_NoMsk);
		}
		else
		{
			CRITICAL_ERROR("bad parent clock \n");
		}
		cfg_clk->parent_clk = aIoctl_param1;
		break;
	case CLK_IOCTL_ENABLE :
		CLK_EnableModuleClock(SPI1_MODULE);
		break;
	default :
		return common_clock_api_standard_clk_ioctls(
				adev, aIoctl_num, aIoctl_param1, aIoctl_param2);
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
			CLK->CLKSEL3 = curr_val | CLK_CLKSEL3_I2S0SEL_HXT;
		}
		else if (i94xxx_hirc_clk_dev == aIoctl_param1)
		{
			CLK->CLKSEL3 = curr_val | CLK_CLKSEL3_I2S0SEL_HIRC;
		}
		else if (i94xxx_pll_clk_dev == aIoctl_param1)
		{
			CLK->CLKSEL3 = curr_val | CLK_CLKSEL3_I2S0SEL_PLL;
		}
		else if (i94xxx_pclk0_clk_dev == aIoctl_param1)
		{
			CLK->CLKSEL3 = curr_val | CLK_CLKSEL3_I2S0SEL_PCLK0;
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
	default :
		return common_clock_api_standard_clk_ioctls(
				adev, aIoctl_num, aIoctl_param1, aIoctl_param2);
	}
	return 0;
}


uint8_t clock_i94xxx_dmic_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;
	uint32_t curr_val;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_SET_PARENT :
		curr_val = (CLK->CLKSEL2 & (~CLK_CLKSEL2_DMICSEL_Msk));
		if (i94xxx_xtal_clk_dev == aIoctl_param1)
		{
			CLK->CLKSEL2 = curr_val | CLK_CLKSEL2_DMICSEL_HXT;
		}
		else if (i94xxx_hirc_clk_dev == aIoctl_param1)
		{
			CLK->CLKSEL2 = curr_val | CLK_CLKSEL2_DMICSEL_HIRC;
		}
		else if (i94xxx_pll_clk_dev == aIoctl_param1)
		{
			CLK->CLKSEL2 = curr_val | CLK_CLKSEL2_DMICSEL_PLL;
		}
		else if (i94xxx_pclk0_clk_dev == aIoctl_param1)
		{
			CLK->CLKSEL2 = curr_val | CLK_CLKSEL2_DMICSEL_PCLK0;
		}
		else
		{
			CRITICAL_ERROR("bad parent clock \n");
		}
		cfg_clk->parent_clk = aIoctl_param1;
		break;
	case CLK_IOCTL_ENABLE :
		CLK->APBCLK0 |= CLK_APBCLK0_DMICCKEN_Msk;
		break;
	default :
		return common_clock_api_standard_clk_ioctls(
				adev, aIoctl_num, aIoctl_param1, aIoctl_param2);
	}
	return 0;
}


uint8_t i94xxx_I2S_MCLK_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;
	uint32_t freq;
	uint32_t mclkdiv;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_GET_FREQ :
		clock_control_common_api_get_parent_clock_rate(cfg_clk, &freq);
		mclkdiv = I2S0->CLKDIV & I2S_CLKDIV_MCLKDIV_Msk;
		if (0 == mclkdiv)
		{
			*(uint32_t *)aIoctl_param1 = freq;
		}
		else
		{
			*(uint32_t *)aIoctl_param1 = freq / 2;
		}
		break;
	default :
		return 1;
	}
	return 0;
}


uint8_t i94xxx_I2S_BCLK_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;
	uint32_t freq;
	uint32_t bclkdiv;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_GET_FREQ :
		clock_control_common_api_get_parent_clock_rate(cfg_clk, &freq);
		bclkdiv = (I2S0->CLKDIV & I2S_CLKDIV_BCLKDIV_Msk) >>
												I2S_CLKDIV_BCLKDIV_Pos;
		*(uint32_t *)aIoctl_param1 = freq / (2 * (bclkdiv + 1));
		break;
	default :
		return 1;
	}
	return 0;
}


uint8_t i94xxx_I2S_FSCLK_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;
	uint32_t freq;
	uint16_t num_of_channels;
	uint16_t channel_width;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_GET_FREQ :
		clock_control_common_api_get_parent_clock_rate(cfg_clk, &freq);
		num_of_channels =
				(I2S0->CTL0 & I2S_CTL0_TDMCHNUM_Msk) >> I2S_CTL0_TDMCHNUM_Pos;
		num_of_channels = 2 * (1 + num_of_channels);
		channel_width =
				(I2S0->CTL0 & I2S_CTL0_CHWIDTH_Msk) >> I2S_CTL0_CHWIDTH_Pos;
		channel_width = 8 * (1 + channel_width);
		*(uint32_t *)aIoctl_param1 = freq / (num_of_channels * channel_width);
		break;
	default :
		return 1;
	}
	return 0;
}


uint8_t i94xxx_I2S_onSPI1_MCLK_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;
	uint32_t freq;
	uint32_t mclkdiv;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_GET_FREQ :
		clock_control_common_api_get_parent_clock_rate(cfg_clk, &freq);
		mclkdiv = ((SPI_T*)SPI1_BASE)->I2SCLK & SPI_I2SCLK_MCLKDIV_Msk;
		if (0 == mclkdiv)
		{
			*(uint32_t *)aIoctl_param1 = freq;
		}
		else
		{
			*(uint32_t *)aIoctl_param1 = freq / 2;
		}
		break;
	default :
		return 1;
	}
	return 0;
}


uint8_t i94xxx_I2S_onSPI1_BCLK_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;
	uint32_t freq;
	uint32_t bclkdiv;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_GET_FREQ :
		clock_control_common_api_get_parent_clock_rate(cfg_clk, &freq);
		bclkdiv = (((SPI_T*)SPI1_BASE)->I2SCLK & SPI_I2SCLK_BCLKDIV_Msk) >>
											SPI_I2SCLK_BCLKDIV_Pos;
		*(uint32_t *)aIoctl_param1 = freq / (2 * (bclkdiv + 1));
		break;
	default :
		return 1;
	}
	return 0;
}


uint8_t i94xxx_I2S_onSPI1_FSCLK_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;
	uint32_t freq;
	uint16_t num_of_channels;
	uint16_t channel_width;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_GET_FREQ :
		clock_control_common_api_get_parent_clock_rate(cfg_clk, &freq);
		num_of_channels = 2;
		channel_width =
				(((SPI_T*)SPI1_BASE)->I2SCTL & SPI_I2SCTL_WDWIDTH_Msk) >>
														SPI_I2SCTL_WDWIDTH_Pos;
		channel_width = 8 * (1 + channel_width);
		*(uint32_t *)aIoctl_param1 = freq / (num_of_channels * channel_width);
		break;
	default :
		return 1;
	}
	return 0;
}


uint8_t i94xxx_I2S_onSPI2_MCLK_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;
	uint32_t freq;
	uint32_t mclkdiv;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_GET_FREQ :
		clock_control_common_api_get_parent_clock_rate(cfg_clk, &freq);
		mclkdiv = ((SPI_T*)SPI2_BASE)->I2SCLK & SPI_I2SCLK_MCLKDIV_Msk;
		if (0 == mclkdiv)
		{
			*(uint32_t *)aIoctl_param1 = freq;
		}
		else
		{
			*(uint32_t *)aIoctl_param1 = freq / 2;
		}
		break;
	default :
		return 1;
	}
	return 0;
}


uint8_t i94xxx_I2S_onSPI2_BCLK_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;
	uint32_t freq;
	uint32_t bclkdiv;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_GET_FREQ :
		clock_control_common_api_get_parent_clock_rate(cfg_clk, &freq);
		bclkdiv = (((SPI_T*)SPI2_BASE)->I2SCLK & SPI_I2SCLK_BCLKDIV_Msk) >>
											SPI_I2SCLK_BCLKDIV_Pos;
		*(uint32_t *)aIoctl_param1 = freq / (2 * (bclkdiv + 1));
		break;
	default :
		return 1;
	}
	return 0;
}


uint8_t i94xxx_I2S_onSPI2_FSCLK_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;
	uint32_t freq;
	uint16_t num_of_channels;
	uint16_t channel_width;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_GET_FREQ :
		clock_control_common_api_get_parent_clock_rate(cfg_clk, &freq);
		num_of_channels = 2;
		channel_width =
				(((SPI_T*)SPI2_BASE)->I2SCTL & SPI_I2SCTL_WDWIDTH_Msk) >>
														SPI_I2SCTL_WDWIDTH_Pos;
		channel_width = 8 * (1 + channel_width);
		*(uint32_t *)aIoctl_param1 = freq / (num_of_channels * channel_width);
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
	default :
		return common_clock_api_standard_clk_ioctls(
				adev, aIoctl_num, aIoctl_param1, aIoctl_param2);
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
	default :
		return common_clock_api_standard_clk_ioctls(
				adev, aIoctl_num, aIoctl_param1, aIoctl_param2);
	}
	return 0;
}


uint8_t clock_i94xxx_i2c1_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
//	struct cfg_clk_t *cfg_clk;
//
//	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_ENABLE :
		CLK_EnableModuleClock(I2C1_MODULE);
		break;
	case CLK_IOCTL_GET_FREQ :
		DEV_IOCTL_1_PARAMS(i94xxx_pclk1_clk_dev,
						CLK_IOCTL_GET_FREQ, aIoctl_param1);
		break;
	default :
		return 1;
	}
	return 0;
}

uint8_t clock_i94xxx_i2c0_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	switch(aIoctl_num)
	{
	case CLK_IOCTL_ENABLE :
		CLK_EnableModuleClock(I2C0_MODULE);
		break;
	case CLK_IOCTL_GET_FREQ :
		DEV_IOCTL_1_PARAMS(i94xxx_pclk0_clk_dev,
						CLK_IOCTL_GET_FREQ, aIoctl_param1);
		break;
	default :
		return 1;
	}
	return 0;
}



uint8_t clock_i94xxx_spi0_ioctl( struct dev_desc_t *adev,
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
			CLK_SetModuleClock(SPI0_MODULE,
					CLK_CLKSEL2_SPI0SEL_HXT, MODULE_NoMsk);
		}
		else if (i94xxx_pclk0_clk_dev == aIoctl_param1)
		{
			CLK_SetModuleClock(SPI0_MODULE,
					CLK_CLKSEL2_SPI0SEL_PCLK0, MODULE_NoMsk);
		}
		else if (i94xxx_pll_clk_dev == aIoctl_param1)
		{
			CLK_SetModuleClock(SPI0_MODULE,
					CLK_CLKSEL2_SPI0SEL_PLL, MODULE_NoMsk);
		}
		else if (i94xxx_hirc_clk_dev == aIoctl_param1)
		{
			CLK_SetModuleClock(SPI0_MODULE,
					CLK_CLKSEL2_SPI0SEL_HIRC, MODULE_NoMsk);
		}
		else
		{
			CRITICAL_ERROR("bad parent clock \n");
		}
		cfg_clk->parent_clk = aIoctl_param1;
		break;
	case CLK_IOCTL_ENABLE :
		CLK_EnableModuleClock(SPI0_MODULE);
		break;
	case CLK_IOCTL_GET_FREQ :
		DEV_IOCTL_1_PARAMS(i94xxx_pclk1_clk_dev,
						CLK_IOCTL_GET_FREQ, aIoctl_param1);
		break;
	default :
		return 1;
	}
	return 0;
}


static void init_clocks(struct clk_cntl_i94xxx_cfg_t *cfg_hndl)
{
	uint32_t rate;

	if (0 != cfg_hndl->xtal_rate)
	{
		DEV_IOCTL_0_PARAMS(i94xxx_xtal_clk_dev, CLK_IOCTL_ENABLE);
		DEV_IOCTL_1_PARAMS(i94xxx_xtal_clk_dev,
				CLK_IOCTL_SET_FREQ, &cfg_hndl->xtal_rate);
	}

	if (0 != cfg_hndl->hirc_rate)
	{
		DEV_IOCTL_0_PARAMS(i94xxx_hirc_clk_dev, CLK_IOCTL_ENABLE);
		DEV_IOCTL_1_PARAMS(i94xxx_hirc_clk_dev,
				CLK_IOCTL_SET_FREQ, &cfg_hndl->hirc_rate);
	}

	DEV_IOCTL_1_PARAMS(i94xxx_pll_clk_dev,
			CLK_IOCTL_SET_PARENT, cfg_hndl->pll_src_clk_dev);
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
	else if (80000000 < rate)
	{
		rate = rate / 2;
	}
	DEV_IOCTL_1_PARAMS(i94xxx_pclk0_clk_dev, CLK_IOCTL_SET_FREQ, &rate);
	DEV_IOCTL_1_PARAMS(i94xxx_pclk1_clk_dev, CLK_IOCTL_SET_FREQ, &rate);

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

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);

	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		init_clocks(cfg_hndl);
		break;
	default :
		return 1;
	}
	return 0;
}
