/*
 * file :   clocks_control_stm32f10x.c
 *
 */

#include "_project.h"

#include "auto_init_api.h"

#include "clock_control_common_api.h"
#include "clock_control_stm32f10x_api.h"
#include "clock_control_stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"


/***************************************/
/********** stm32f10x_xtal_clk_dev ********/
static void clock_stm32f10x_xtal_enable()
{
	/* Enable HSE (high speed external clock). */
	RCC_HSEConfig( RCC_HSE_ON );

	/* Wait till HSE is ready. */
	while( RCC_GetFlagStatus( RCC_FLAG_HSERDY ) == RESET );
}

#define DT_DEV_NAME                stm32f10x_xtal_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_ENABLE_CLK_FUNC    clock_stm32f10x_xtal_enable

#include "clk_cntl_add_device.h"


/***************************************/
/********** stm32f10x_xtal_div128_clk_dev ********/
static void stm32f10x_xtal_div128_set_freq(uint32_t freq, uint32_t parent_freq)
{
	if ((parent_freq / 128) != freq)
	{
		CRITICAL_ERROR("bad clock rate \n");
	}
}

#define DT_DEV_NAME                stm32f10x_xtal_div128_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_DEFAULT_PARENT     stm32f10x_xtal_clk_dev
#define CLK_DT_SET_FREQ_FUNC      stm32f10x_xtal_div128_set_freq

#include "clk_cntl_add_device.h"


/***************************************/
/********** stm32f10x_hsirc_clk_dev ********/
static void clock_stm32f10x_hsirc_enable()
{
	/* Set HSION bit */
	RCC->CR |= (uint32_t)0x00000001;

	// wait for hsirc ready
	while (0 == (0x00000002 & RCC->CR));
}

static void clock_stm32f10x_hsirc_set_freq(uint32_t freq, uint32_t parent_freq)
{
	if (8000000 != freq)
	{
		CRITICAL_ERROR("bad clock rate \n");
	}
}

#define DT_DEV_NAME               stm32f10x_hsirc_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_ENABLE_CLK_FUNC    clock_stm32f10x_hsirc_enable
#define CLK_DT_SET_FREQ_FUNC      clock_stm32f10x_hsirc_set_freq

#include "clk_cntl_add_device.h"


/***************************************/
/********** stm32f10x_lsirc_clk_dev ********/
static void clock_stm32f10x_lsirc_enable()
{
	/* Set LSION bit */
	RCC->CSR |= (uint32_t)0x00000001;

	// wait for hsirc ready
	while (0 == (0x00000002 & RCC->CSR));
}

static void clock_stm32f10x_lsirc_set_freq(uint32_t freq, uint32_t parent_freq)
{
	if (40000 != freq)
	{
		CRITICAL_ERROR("bad clock rate \n");
	}
}

#define DT_DEV_NAME               stm32f10x_lsirc_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_ENABLE_CLK_FUNC    clock_stm32f10x_lsirc_enable
#define CLK_DT_SET_FREQ_FUNC      clock_stm32f10x_lsirc_set_freq

#include "clk_cntl_add_device.h"

/***************************************/
/********** stm32f10x_sysclk_dev ********/
static void stm32f10x_sysclk_set_parent_clk(struct dev_desc_t *parent_clk)
{
	uint32_t wait_val;

	wait_val = 0;
	if (stm32f10x_xtal_clk_dev == parent_clk)
	{
		RCC_SYSCLKConfig( RCC_SYSCLKSource_HSE );
		wait_val = 0x4;
	}
	else if (stm32f10x_hsirc_clk_dev == parent_clk)
	{
		RCC_SYSCLKConfig( RCC_SYSCLKSource_HSI );
		wait_val = 0x0;
	}
//		else if (stm32f10x_pll_clk_dev == parent_clk)
//		{
//			RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK );
//		wait_val = 0x8;
//		}
	else
	{
		CRITICAL_ERROR("bad parent clock \n");
	}
	/* Wait till selected clock is used as system clock source. */
	while( RCC_GetSYSCLKSource() != wait_val );
}

#define DT_DEV_NAME               stm32f10x_sysclk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_DEFAULT_PARENT       stm32f10x_hsirc_clk_dev
#define CLK_DT_SET_PARENT_CLK_FUNC  stm32f10x_sysclk_set_parent_clk

#include "clk_cntl_add_device.h"


/***************************************/
/********** stm32f10x_hclk_clk_dev ********/
static void clock_stm32f10x_hclk_set_freq(uint32_t freq, uint32_t parent_freq)
{
	uint32_t div;
	uint32_t div_reg_val;

	div = 1;
	div_reg_val = 0x7;
	while (((div * freq) < parent_freq) && (div < 512))
	{
		div *= 2;
		if (32 == div)// skip 32
		{
			div = 64;
		}
		div_reg_val++;
	}
	div = parent_freq / freq;
	if ((512 < div) || (parent_freq != (div * freq)) )
	{
		CRITICAL_ERROR("cannot create requested hclk \n");
	}
	RCC_HCLKConfig( div_reg_val << 4);
}

#define DT_DEV_NAME               stm32f10x_hclk_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_DEFAULT_PARENT     stm32f10x_sysclk_dev
#define CLK_DT_SET_FREQ_FUNC      clock_stm32f10x_hclk_set_freq

#include "clk_cntl_add_device.h"


/***************************************/
/********** stm32f10x_systick_clk_dev ********/
#define DT_DEV_NAME               stm32f10x_systick_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_DEFAULT_PARENT     stm32f10x_hclk_clk_dev

#include "clk_cntl_add_device.h"



/***************************************/
/********** stm32f10x_apb1_clk_dev ********/
static void clock_stm32f10x_apb1_set_freq(uint32_t freq, uint32_t parent_freq)
{
	uint32_t prescaler;
	uint32_t RCC_HCLK_Div_val;

	prescaler = 1;
	RCC_HCLK_Div_val = RCC_HCLK_Div1;
	while (( (parent_freq / prescaler) != freq) && (16 >= prescaler))
	{
		prescaler *= 2;
		RCC_HCLK_Div_val += 0x100;
	}
	if (16 < prescaler)
	{
		CRITICAL_ERROR("cannot create requested APB1 clock \n");
	}
	RCC_PCLK1Config( RCC_HCLK_Div_val );
}

#define DT_DEV_NAME               stm32f10x_apb1_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_DEFAULT_PARENT     stm32f10x_sysclk_dev
#define CLK_DT_SET_FREQ_FUNC      clock_stm32f10x_apb1_set_freq

#include "clk_cntl_add_device.h"



/***************************************/
/********** stm32f10x_apb2_clk_dev ********/
static void clock_stm32f10x_apb2_set_freq(uint32_t freq, uint32_t parent_freq)
{
	uint32_t prescaler;
	uint32_t RCC_HCLK_Div_val;

	prescaler = 1;
	RCC_HCLK_Div_val = RCC_HCLK_Div1;
	while (( (parent_freq / prescaler) != freq) && (16 >= prescaler))
	{
		prescaler *= 2;
		RCC_HCLK_Div_val += 0x100;
	}
	if (16 < prescaler)
	{
		CRITICAL_ERROR("cannot create requested APB2 clock \n");
	}
	RCC_PCLK2Config( RCC_HCLK_Div_val );
}

#define DT_DEV_NAME               stm32f10x_apb2_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_DEFAULT_PARENT     stm32f10x_sysclk_dev
#define CLK_DT_SET_FREQ_FUNC      clock_stm32f10x_apb2_set_freq

#include "clk_cntl_add_device.h"



/*******************************************/
/********** stm32f10x_adc_clk_dev ********/
static void clock_stm32f10x_adc_enable()
{
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_ADC1 , ENABLE );
}

static void clock_stm32f10x_adc_set_freq(uint32_t freq, uint32_t parent_freq)
{
	uint32_t prescaler;
	uint32_t ADCPRE_val;

	prescaler = 2;
	ADCPRE_val = 0;
	while (( (parent_freq / prescaler) != freq) && (8 >= prescaler))
	{
		prescaler += 2;
		ADCPRE_val += 0x4000;
	}
	if (8 < prescaler)
	{
		CRITICAL_ERROR("cannot create requested ADC clock \n");
	}
	RCC_ADCCLKConfig(ADCPRE_val);
}

#define DT_DEV_NAME               stm32f10x_adc_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_DEFAULT_PARENT     stm32f10x_apb2_clk_dev
#define CLK_DT_SET_FREQ_FUNC      clock_stm32f10x_adc_set_freq
#define CLK_DT_ENABLE_CLK_FUNC    clock_stm32f10x_adc_enable

#include "clk_cntl_add_device.h"



/*******************************************/
/********** stm32f10x_pwr_clk_dev ********/
static void clock_stm32f10x_pwr_enable()
{
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_PWR, ENABLE );
}

#define DT_DEV_NAME               stm32f10x_pwr_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_ENABLE_CLK_FUNC      clock_stm32f10x_pwr_enable

#include "clk_cntl_add_device.h"


/*******************************************/
/********** stm32f10x_rtc_clk_dev ********/
static void clock_stm32f10x_rtc_enable()
{
	RCC_RTCCLKCmd(ENABLE);
	// to access RTC, backup(BKP) plane clock should be enabled too
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
}

static void clock_stm32f10x_rtc_set_parent_clk(struct dev_desc_t *parent_clk)
{
	RCC_BackupResetCmd(ENABLE);
	RCC_BackupResetCmd(DISABLE);
	if (stm32f10x_lsirc_clk_dev == parent_clk)
	{
		RCC_RTCCLKConfig( RCC_RTCCLKSource_LSI );
	}
	else if (stm32f10x_xtal_div128_clk_dev == parent_clk)
	{
		RCC_RTCCLKConfig( RCC_RTCCLKSource_HSE_Div128 );
	}
	else
	{
		CRITICAL_ERROR("'not implemented' or bad parent clock \n");
	}
}

#define DT_DEV_NAME               stm32f10x_rtc_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_ENABLE_CLK_FUNC      clock_stm32f10x_rtc_enable
#define CLK_DT_SET_PARENT_CLK_FUNC  clock_stm32f10x_rtc_set_parent_clk

#include "clk_cntl_add_device.h"



/*******************************************/
/********** stm32f10x_tim1_clk_dev ********/
static void clock_stm32f10x_tim1_enable()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE );
}

static void clock_stm32f10x_tim1_get_freq(uint32_t *freq, uint32_t parent_freq)
{
	uint32_t tmpreg = 0;

	#define CFGR_PPRE2_Mask     ((uint32_t)0x3800)
	tmpreg = RCC->CFGR & CFGR_PPRE2_Mask;
	if (0 != tmpreg) // if prescaler != 1
	{
		parent_freq *= 2;
	}
	*freq = parent_freq;
}


#define DT_DEV_NAME               stm32f10x_tim1_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_DEFAULT_PARENT       stm32f10x_apb2_clk_dev
#define CLK_DT_ENABLE_CLK_FUNC      clock_stm32f10x_tim1_enable
#define CLK_DT_GET_FREQ_FUNC        clock_stm32f10x_tim1_get_freq

#include "clk_cntl_add_device.h"




static void init_clocks(struct clk_cntl_stm32f10x_cfg_t *cfg_hndl)
{
	//uint32_t rate;


	/* Start with the clocks in their expected state. */
	RCC_DeInit();

	if (0 != cfg_hndl->hsirc_rate)
	{
		DEV_IOCTL_0_PARAMS(stm32f10x_hsirc_clk_dev, IOCTL_DEVICE_START);
		DEV_IOCTL_0_PARAMS(stm32f10x_hsirc_clk_dev, CLK_IOCTL_ENABLE);
		DEV_IOCTL_1_PARAMS(stm32f10x_hsirc_clk_dev,
				CLK_IOCTL_SET_FREQ, &cfg_hndl->hsirc_rate);
	}

	if (0 != cfg_hndl->hsirc_rate)
	{
		DEV_IOCTL_0_PARAMS(stm32f10x_lsirc_clk_dev, IOCTL_DEVICE_START);
		DEV_IOCTL_0_PARAMS(stm32f10x_lsirc_clk_dev, CLK_IOCTL_ENABLE);
		DEV_IOCTL_1_PARAMS(stm32f10x_lsirc_clk_dev,
				CLK_IOCTL_SET_FREQ, &cfg_hndl->lsirc_rate);
	}

	if (0 != cfg_hndl->xtal_rate)
	{
		uint32_t xtal_rate;

		xtal_rate = cfg_hndl->xtal_rate;
		DEV_IOCTL_0_PARAMS(stm32f10x_xtal_clk_dev, IOCTL_DEVICE_START);
		DEV_IOCTL_0_PARAMS(stm32f10x_xtal_clk_dev, CLK_IOCTL_ENABLE);
		DEV_IOCTL_1_PARAMS(
				stm32f10x_xtal_clk_dev, CLK_IOCTL_SET_FREQ, &xtal_rate);
		DEV_IOCTL_0_PARAMS(stm32f10x_xtal_div128_clk_dev, IOCTL_DEVICE_START);
		xtal_rate /= 128;
		DEV_IOCTL_1_PARAMS(
				stm32f10x_xtal_clk_dev, CLK_IOCTL_SET_FREQ, &xtal_rate);
	}

	/* 2 wait states required on the flash. */
	*( ( unsigned long * ) 0x40022000 ) = 0x02;

	if (0 != cfg_hndl->pll_rate)
	{
		CRITICAL_ERROR("TODO");
	}

	DEV_IOCTL_0_PARAMS(stm32f10x_sysclk_dev, IOCTL_DEVICE_START);
	DEV_IOCTL_1_PARAMS(stm32f10x_sysclk_dev,
					CLK_IOCTL_SET_PARENT, cfg_hndl->sysclk_src_clk_dev);
	DEV_IOCTL_0_PARAMS(stm32f10x_hclk_clk_dev, IOCTL_DEVICE_START);
	DEV_IOCTL_1_PARAMS(stm32f10x_hclk_clk_dev,
					CLK_IOCTL_SET_FREQ, &cfg_hndl->hclk_rate);
	DEV_IOCTL_0_PARAMS(stm32f10x_apb1_clk_dev, IOCTL_DEVICE_START);
	DEV_IOCTL_1_PARAMS(stm32f10x_apb1_clk_dev,
			CLK_IOCTL_SET_FREQ, &cfg_hndl->apb1_rate);
	DEV_IOCTL_0_PARAMS(stm32f10x_apb2_clk_dev, IOCTL_DEVICE_START);
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

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(clock_control_stm32f10x, adev);

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

#define  MODULE_NAME                     clock_control_stm32f10x
#define  MODULE_IOCTL_FUNCTION           clock_control_stm32f10x_ioctl
#define MODULE_HAS_NO_RUNTIME_DATA
#include "add_module.h"
