/*
 *
 * file :   clocks_control_stm32f10x.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/
#include "_project.h"

#include "auto_init_api.h"

#include "clock_control_common_api.h"
#include "clock_control_stm32f10x_api.h"
#include "clock_control_stm32f10x.h"
#include "stm32f10x_rcc.h"

/********  defines *********************/

/********  types  *********************/


/********  externals *********************/


/********  local defs *********************/




uint8_t clock_stm32f10x_xtal_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;
	uint32_t rate;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_ENABLE :
		/* Enable HSE (high speed external clock). */
		RCC_HSEConfig( RCC_HSE_ON );

		/* Wait till HSE is ready. */
		while( RCC_GetFlagStatus( RCC_FLAG_HSERDY ) == RESET )
		{
		}
		break;
	case CLK_IOCTL_SET_FREQ :
		rate = *(uint32_t*)aIoctl_param1;
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


uint8_t clock_stm32f10x_hsirc_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;
	uint32_t rate;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);

	switch(aIoctl_num)
	{
	case CLK_IOCTL_ENABLE :
		/* Set HSION bit */
		RCC->CR |= (uint32_t)0x00000001;

		// wait for hsirc ready
		while (0 == (0x00000002 & RCC->CR));
		break;
	case CLK_IOCTL_SET_FREQ :
		rate = *(uint32_t*)aIoctl_param1;
		if (8000000 != rate)
		{
			CRITICAL_ERROR("bad clock rate \n");
		}
		cfg_clk->rate = rate;
		break;
	case CLK_IOCTL_GET_FREQ :
		*(uint32_t*)aIoctl_param1 = 8000000;
		break;
	default :
		return 1;
	}
	return 0;
}


uint8_t clock_stm32f10x_apb2_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;
	uint32_t rate;
	uint32_t sysclk_rate;
	uint32_t prescaler;
	uint32_t RCC_HCLK_Div_val;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);

	switch(aIoctl_num)
	{
	case CLK_IOCTL_ENABLE :
		break;
	case CLK_IOCTL_SET_FREQ :
		clock_control_common_api_get_parent_clock_rate(cfg_clk, &sysclk_rate);
		rate = *(uint32_t*)aIoctl_param1;
		prescaler = 1;
		RCC_HCLK_Div_val = RCC_HCLK_Div1;
		while (( (sysclk_rate / prescaler) != rate) && (16 >= prescaler))
		{
			prescaler *= 2;
			RCC_HCLK_Div_val += 0x100;
		}
		if (16 < prescaler)
		{
			CRITICAL_ERROR("cannot create requested APB2 clock \n");
		}
		RCC_PCLK2Config( RCC_HCLK_Div_val );
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



uint8_t clock_stm32f10x_sysclk_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;
	uint32_t wait_val;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_SET_PARENT :
		if (stm32f10x_xtal_clk_dev == aIoctl_param1)
		{
			RCC_SYSCLKConfig( RCC_SYSCLKSource_HSE );
			wait_val = 0x4;
		}
		else if (stm32f10x_hsirc_clk_dev == aIoctl_param1)
		{
			RCC_SYSCLKConfig( RCC_SYSCLKSource_HSI );
			wait_val = 0x0;
		}
//		else if (stm32f10x_pll_clk_dev == aIoctl_param1)
//		{
//			RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK );
//		wait_val = 0x8;
//		}
		else
		{
			CRITICAL_ERROR("bad parent clock \n");
		}
		/* Wait till selected clock is used as system clock source. */
		while( RCC_GetSYSCLKSource() != wait_val )
		{
		}
		cfg_clk->parent_clk = aIoctl_param1;
		break;
	//case CLK_IOCTL_SET_FREQ :
		// cannot be set as totaly derived from parent, so return 1
	//	break;
	case CLK_IOCTL_GET_FREQ :
		DEV_IOCTL_1_PARAMS(
				cfg_clk->parent_clk, CLK_IOCTL_GET_FREQ, aIoctl_param1);
		break;
	default :
		return 1;
	}
	return 0;
}


uint8_t clock_stm32f10x_hclk_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct cfg_clk_t *cfg_clk;
	uint32_t rate;
	uint32_t new_rate;
	uint32_t div;
	uint32_t div_reg_val;

	cfg_clk = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case CLK_IOCTL_SET_FREQ :
		DEV_IOCTL_1_PARAMS(stm32f10x_sysclk_dev, CLK_IOCTL_GET_FREQ, &rate);
		new_rate = *(uint32_t*)aIoctl_param1;
		div = 1;
		div_reg_val = 0x7;
		while (((div * new_rate) < rate) && (div < 512))
		{
			div *= 2;
			if (32 == div)// skip 32
			{
				div = 64;
			}
			div_reg_val++;
		}
		div = rate / new_rate;
		if ((512 < div) || (rate != (div * new_rate)) )
		{
			CRITICAL_ERROR("cannot create requested hclk \n");
		}
		cfg_clk->rate = new_rate;
		RCC_HCLKConfig( div_reg_val << 4);
		break;
	case CLK_IOCTL_GET_FREQ :
		*(uint32_t*)aIoctl_param1 = cfg_clk->rate;
		break;
	default :
		return 1;
	}
	return 0;
}


uint8_t stm32f10x_systick_clk_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	switch(aIoctl_num)
	{
	case CLK_IOCTL_SET_FREQ :
		CRITICAL_ERROR("TODO");
		break;
	default :
		return common_clock_api_standard_clk_ioctls(
				adev, aIoctl_num, aIoctl_param1, aIoctl_param2);
	}
	return 0;
}

static void init_clocks(struct clk_cntl_stm32f10x_cfg_t *cfg_hndl)
{
	//uint32_t rate;


	/* Start with the clocks in their expected state. */
	RCC_DeInit();

	if (0 != cfg_hndl->hsirc_rate)
	{
		DEV_IOCTL_0_PARAMS(stm32f10x_hsirc_clk_dev, CLK_IOCTL_ENABLE);
	}

	if (0 != cfg_hndl->xtal_rate)
	{
		DEV_IOCTL_0_PARAMS(stm32f10x_xtal_clk_dev, CLK_IOCTL_ENABLE);
		DEV_IOCTL_1_PARAMS(stm32f10x_xtal_clk_dev,
				CLK_IOCTL_SET_FREQ, &cfg_hndl->xtal_rate);
	}

	/* 2 wait states required on the flash. */
	*( ( unsigned long * ) 0x40022000 ) = 0x02;

	if (0 != cfg_hndl->pll_rate)
	{
		CRITICAL_ERROR("TODO");
#if 0
	/* PCLK2 = HCLK */
	RCC_PCLK2Config( RCC_HCLK_Div1 );

	/* PCLK1 = HCLK/2 */
	RCC_PCLK1Config( RCC_HCLK_Div2 );

	/* PLLCLK = 8MHz * 9 = 72 MHz. */
	RCC_PLLConfig( RCC_PLLSource_HSE_Div1, RCC_PLLMul_9 );

	/* Enable PLL. */
	RCC_PLLCmd( ENABLE );

	/* Wait till PLL is ready. */
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
	{
	}

	/* Enable GPIOA, GPIOB, GPIOC, GPIOD, GPIOE and AFIO clocks */
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
			RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
			RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE );

	/* SPI2 Periph clock enable */
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2, ENABLE );

//	RCC_GetClocksFreq(&lRCC_Clocks);

#endif
	}

	DEV_IOCTL_1_PARAMS(stm32f10x_sysclk_dev,
					CLK_IOCTL_SET_PARENT, cfg_hndl->sysclk_src_clk_dev);
	DEV_IOCTL_1_PARAMS(stm32f10x_hclk_clk_dev,
					CLK_IOCTL_SET_FREQ, &cfg_hndl->hclk_rate);
	DEV_IOCTL_1_PARAMS(stm32f10x_apb2_clk_dev,
			CLK_IOCTL_SET_FREQ, &cfg_hndl->apb2_rate);
}


/**
 * clock_control_stm32f10x_ioctl()
 *
 * return:
 */
uint8_t clock_control_stm32f10x_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct clk_cntl_stm32f10x_cfg_t *cfg_hndl;

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
