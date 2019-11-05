/*
 *
 * file :   clocks_control_i94xxx.c
 *
 *
 */

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

/***************************************/
/********** i94xxx_xtal_clk_dev ********/
static void clock_i94xxx_xtal_enable()
{
	CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);
	CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);
}

static void clock_i94xxx_xtal_set_freq(uint32_t freq, uint32_t parent_freq)
{
	__HXT = freq;
	gau32ClkSrcTbl[0] = freq;
}

#define DT_DEV_NAME                i94xxx_xtal_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_ENABLE_CLK_FUNC    clock_i94xxx_xtal_enable
#define CLK_DT_SET_FREQ_FUNC      clock_i94xxx_xtal_set_freq

#include "clk_cntl_add_device.h"



/****************************************/
/********** i94xxx_lxtal_clk_dev ********/
static void clock_i94xxx_lxtal_enable()
{
	CLK_EnableXtalRC(CLK_PWRCTL_LXTEN_Msk);
	CLK_WaitClockReady(CLK_STATUS_LXTSTB_Msk);
}

static void clock_i94xxx_lxtal_set_freq(uint32_t freq, uint32_t parent_freq)
{
	__LXT = freq;
	gau32ClkSrcTbl[1] = freq;
}

#define DT_DEV_NAME                i94xxx_lxtal_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_ENABLE_CLK_FUNC    clock_i94xxx_lxtal_enable
#define CLK_DT_SET_FREQ_FUNC      clock_i94xxx_lxtal_set_freq

#include "clk_cntl_add_device.h"



/***************************************/
/********** i94xxx_hirc_clk_dev ********/
static void clock_i94xxx_hirc_enable()
{
	CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
	CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
}

static void clock_i94xxx_hirc_set_freq(uint32_t freq, uint32_t parent_freq)
{
	if (48000000 == freq)
	{
		CLK_SELECT_TRIM_HIRC(CLK_CLKSEL0_HIRCFSEL_48M);
	}
	else if (49152000 == freq)
	{
		CLK_SELECT_TRIM_HIRC(CLK_CLKSEL0_HIRCFSEL_49M);
	}
	else
	{
		CRITICAL_ERROR("bad clock rate \n");
	}
	__HIRC = freq;
}

#define DT_DEV_NAME               i94xxx_hirc_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_ENABLE_CLK_FUNC    clock_i94xxx_hirc_enable
#define CLK_DT_SET_FREQ_FUNC      clock_i94xxx_hirc_set_freq

#include "clk_cntl_add_device.h"


/***************************************/
/********** i94xxx_pll_clk_dev ********/
static void clock_i94xxx_pll_set_freq(uint32_t freq, uint32_t parent_freq)
{
	if (CLK->PLLCTL & CLK_PLLCTL_PLLSRC_Msk)
	{
		CLK_EnablePLL(CLK_PLLCTL_PLLSRC_HIRC, freq );
	}
	else
	{
		CLK_EnablePLL(CLK_PLLCTL_PLLSRC_HXT, freq);
	}
}

static void clock_i94xxx_pll_get_freq(uint32_t *freq, uint32_t parent_freq)
{
	*freq = CLK_GetPLLClockFreq();
}


static void clock_i94xxx_pll_set_parent_clk(struct dev_desc_t *parent_clk)
{
	if (i94xxx_xtal_clk_dev == parent_clk)
	{
		CLK->PLLCTL &= ~CLK_PLLCTL_PLLSRC_Msk;
	}
	else if (i94xxx_hirc_clk_dev == parent_clk)
	{
		CLK->PLLCTL |= CLK_PLLCTL_PLLSRC_Msk;
	}
	else
	{
		CRITICAL_ERROR("bad parent clock \n");
	}
}

#define DT_DEV_NAME               i94xxx_pll_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_SET_FREQ_FUNC        clock_i94xxx_pll_set_freq
#define CLK_DT_GET_FREQ_FUNC        clock_i94xxx_pll_get_freq
#define CLK_DT_SET_PARENT_CLK_FUNC  clock_i94xxx_pll_set_parent_clk

#include "clk_cntl_add_device.h"



/***************************************/
/********** i94xxx_hclk_clk_dev ********/
static void clock_i94xxx_hclk_set_parent_clk(struct dev_desc_t *parent_clk)
{
	if (i94xxx_xtal_clk_dev == parent_clk)
	{
		CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));
	}
	else if (i94xxx_hirc_clk_dev == parent_clk)
	{
		CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));
	}
	else if (i94xxx_pll_clk_dev == parent_clk)
	{
		CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_PLL, CLK_CLKDIV0_HCLK(1));
	}
	else
	{
		CRITICAL_ERROR("bad parent clock \n");
	}
}

#define DT_DEV_NAME               i94xxx_hclk_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_SET_PARENT_CLK_FUNC      clock_i94xxx_hclk_set_parent_clk

#include "clk_cntl_add_device.h"



/****************************************/
/********** i94xxx_pclk0_clk_dev ********/
static void clock_i94xxx_pclk0_set_freq(uint32_t freq, uint32_t parent_freq)
{
	uint32_t div;

	div = parent_freq / freq;
	if (div)
	{
		div--;
	}
	CLK->PCLKDIV = (CLK->PCLKDIV & (~CLK_PCLKDIV_APB0DIV_Msk)) |
									(div << CLK_PCLKDIV_APB0DIV_Pos);
}

static void clock_i94xxx_pclk0_get_freq(uint32_t *freq, uint32_t parent_freq)
{
	*freq = CLK_GetPCLK0Freq();
}

#define DT_DEV_NAME               i94xxx_pclk0_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_DEFAULT_PARENT     i94xxx_hclk_clk_dev
#define CLK_DT_SET_FREQ_FUNC      clock_i94xxx_pclk0_set_freq
#define CLK_DT_GET_FREQ_FUNC      clock_i94xxx_pclk0_get_freq

#include "clk_cntl_add_device.h"



/****************************************/
/********** i94xxx_pclk1_clk_dev ********/
static void clock_i94xxx_pclk1_set_freq(uint32_t freq, uint32_t parent_freq)
{
	uint32_t div;

	div = parent_freq / freq;
	if (div)
	{
		div--;
	}
	CLK->PCLKDIV = (CLK->PCLKDIV & (~CLK_PCLKDIV_APB1DIV_Msk)) |
									(div << CLK_PCLKDIV_APB1DIV_Pos);
}

static void clock_i94xxx_pclk1_get_freq(uint32_t *freq, uint32_t parent_freq)
{
	*freq = CLK_GetPCLK1Freq();
}

#define DT_DEV_NAME               i94xxx_pclk1_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_DEFAULT_PARENT     i94xxx_hclk_clk_dev
#define CLK_DT_SET_FREQ_FUNC      clock_i94xxx_pclk1_set_freq
#define CLK_DT_GET_FREQ_FUNC      clock_i94xxx_pclk1_get_freq

#include "clk_cntl_add_device.h"



/***************************************/
/********** i94xxx_usb_clk_dev ********/
static void clock_i94xxx_usb_enable()
{
	CLK_EnableModuleClock(USBD_MODULE);
}

static void clock_i94xxx_usb_set_freq(uint32_t freq, uint32_t parent_freq)
{
	uint32_t div;

	div = (parent_freq / freq);
	if (div)
	{
		div--;
	}

	CLK->CLKDIV0 = (CLK->CLKDIV0 & (~CLK_CLKDIV0_USBDIV_Msk))
						| (div << CLK_CLKDIV0_USBDIV_Pos);
}

static void clock_i94xxx_usb_get_freq(uint32_t *freq, uint32_t parent_freq)
{
	uint32_t div;

	div =
		((CLK->CLKDIV0 & CLK_CLKDIV0_USBDIV_Msk) >> CLK_CLKDIV0_USBDIV_Pos) + 1;
	*freq = parent_freq / div;
}

static void clock_i94xxx_usb_set_parent_clk(struct dev_desc_t *parent_clk)
{
	if (i94xxx_hirc_clk_dev == parent_clk)
	{
		//CLK->CLKSEL4 &= ~(0x1ul << 24); //CLKSEL4 not exist yet in BSP
		*((uint32_t *)0x40000224) &= ~(0x1ul << 24);
	}
	else if (i94xxx_pll_clk_dev == parent_clk)
	{
		//CLK->CLKSEL4 |= (0x1ul << 24); //CLKSEL4 not exist yet in BSP
		*((uint32_t *)0x40000224) |= (0x1UL<<24);
	}
	else
	{
		CRITICAL_ERROR("bad parent clock \n");
	}
}

#define DT_DEV_NAME               i94xxx_usb_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_ENABLE_CLK_FUNC      clock_i94xxx_usb_enable
#define CLK_DT_SET_FREQ_FUNC        clock_i94xxx_usb_set_freq
#define CLK_DT_GET_FREQ_FUNC        clock_i94xxx_usb_get_freq
#define CLK_DT_SET_PARENT_CLK_FUNC  clock_i94xxx_usb_set_parent_clk

#include "clk_cntl_add_device.h"




/******************************************/
/********** i94xxx_systick_clk_dev ********/
#define DT_DEV_NAME                i94xxx_systick_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_DEFAULT_PARENT       i94xxx_hclk_clk_dev

#include "clk_cntl_add_device.h"


/***************************************/
/********** i94xxx_spi2_clk_dev ********/
static void clock_i94xxx_spi2_enable()
{
	CLK_EnableModuleClock(SPI2_MODULE);
}

static void clock_i94xxx_spi2_set_parent_clk(struct dev_desc_t *parent_clk)
{
	if (i94xxx_xtal_clk_dev == parent_clk)
	{
		CLK_SetModuleClock(SPI2_MODULE,
				CLK_CLKSEL2_SPI2SEL_HXT, MODULE_NoMsk);
	}
	else if (i94xxx_hirc_clk_dev == parent_clk)
	{
		CLK_SetModuleClock(SPI2_MODULE,
				CLK_CLKSEL2_SPI2SEL_HIRC, MODULE_NoMsk);
	}
	else if (i94xxx_pll_clk_dev == parent_clk)
	{
		CLK_SetModuleClock(SPI2_MODULE,
				CLK_CLKSEL2_SPI2SEL_PLL, MODULE_NoMsk);
	}
	else if (i94xxx_pclk1_clk_dev == parent_clk)
	{
		CLK_SetModuleClock(SPI2_MODULE,
				CLK_CLKSEL2_SPI2SEL_PCLK0, MODULE_NoMsk);
	}
	else
	{
		CRITICAL_ERROR("bad parent clock \n");
	}
}

#define DT_DEV_NAME               i94xxx_spi2_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_ENABLE_CLK_FUNC       clock_i94xxx_spi2_enable
#define CLK_DT_SET_PARENT_CLK_FUNC   clock_i94xxx_spi2_set_parent_clk

#include "clk_cntl_add_device.h"



/***************************************/
/********** i94xxx_spi1_clk_dev ********/
static void clock_i94xxx_spi1_enable()
{
	CLK_EnableModuleClock(SPI1_MODULE);
}

static void clock_i94xxx_spi1_set_parent_clk(struct dev_desc_t *parent_clk)
{
	if (i94xxx_xtal_clk_dev == parent_clk)
	{
		CLK_SetModuleClock(SPI1_MODULE,
				CLK_CLKSEL2_SPI1SEL_HXT, MODULE_NoMsk);
	}
	else if (i94xxx_hirc_clk_dev == parent_clk)
	{
		CLK_SetModuleClock(SPI1_MODULE,
				CLK_CLKSEL2_SPI1SEL_HIRC, MODULE_NoMsk);
	}
	else if (i94xxx_pll_clk_dev == parent_clk)
	{
		CLK_SetModuleClock(SPI1_MODULE,
				CLK_CLKSEL2_SPI1SEL_PLL, MODULE_NoMsk);
	}
	else if (i94xxx_pclk1_clk_dev == parent_clk)
	{
		CLK_SetModuleClock(SPI1_MODULE,
				CLK_CLKSEL2_SPI1SEL_PCLK1, MODULE_NoMsk);
	}
	else
	{
		CRITICAL_ERROR("bad parent clock \n");
	}
}

#define DT_DEV_NAME               i94xxx_spi1_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_ENABLE_CLK_FUNC       clock_i94xxx_spi1_enable
#define CLK_DT_SET_PARENT_CLK_FUNC   clock_i94xxx_spi1_set_parent_clk

#include "clk_cntl_add_device.h"



/***************************************/
/********** i94xxx_i2s_clk_dev ********/
static void clock_i94xxx_i2s_enable()
{
	CLK->APBCLK0 |= CLK_APBCLK0_I2S0CKEN_Msk;
}

static void clock_i94xxx_i2s_set_parent_clk(struct dev_desc_t *parent_clk)
{
	uint32_t curr_val;

	curr_val = (CLK->CLKSEL3 & ~ CLK_CLKSEL3_I2S0SEL_Msk);
	if (i94xxx_xtal_clk_dev == parent_clk)
	{
		CLK->CLKSEL3 = curr_val | CLK_CLKSEL3_I2S0SEL_HXT;
	}
	else if (i94xxx_hirc_clk_dev == parent_clk)
	{
		CLK->CLKSEL3 = curr_val | CLK_CLKSEL3_I2S0SEL_HIRC;
	}
	else if (i94xxx_pll_clk_dev == parent_clk)
	{
		CLK->CLKSEL3 = curr_val | CLK_CLKSEL3_I2S0SEL_PLL;
	}
	else if (i94xxx_pclk0_clk_dev == parent_clk)
	{
		CLK->CLKSEL3 = curr_val | CLK_CLKSEL3_I2S0SEL_PCLK0;
	}
	else
	{
		CRITICAL_ERROR("bad parent clock \n");
	}
}

#define DT_DEV_NAME               i94xxx_i2s_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_ENABLE_CLK_FUNC      clock_i94xxx_i2s_enable
#define CLK_DT_SET_PARENT_CLK_FUNC  clock_i94xxx_i2s_set_parent_clk

#include "clk_cntl_add_device.h"



/***************************************/
/********** i94xxx_dmic_clk_dev ********/
static void clock_i94xxx_dmic_enable()
{
	CLK->APBCLK0 |= CLK_APBCLK0_DMICCKEN_Msk;
}

static void clock_i94xxx_dmic_set_parent_clk(struct dev_desc_t *parent_clk)
{
	uint32_t curr_val;

	curr_val = (CLK->CLKSEL2 & (~CLK_CLKSEL2_DMICSEL_Msk));
	if (i94xxx_xtal_clk_dev == parent_clk)
	{
		CLK->CLKSEL2 = curr_val | CLK_CLKSEL2_DMICSEL_HXT;
	}
	else if (i94xxx_hirc_clk_dev == parent_clk)
	{
		CLK->CLKSEL2 = curr_val | CLK_CLKSEL2_DMICSEL_HIRC;
	}
	else if (i94xxx_pll_clk_dev == parent_clk)
	{
		CLK->CLKSEL2 = curr_val | CLK_CLKSEL2_DMICSEL_PLL;
	}
	else if (i94xxx_pclk0_clk_dev == parent_clk)
	{
		CLK->CLKSEL2 = curr_val | CLK_CLKSEL2_DMICSEL_PCLK0;
	}
	else
	{
		CRITICAL_ERROR("bad parent clock \n");
	}
}

#define DT_DEV_NAME                i94xxx_dmic_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_ENABLE_CLK_FUNC      clock_i94xxx_dmic_enable
#define CLK_DT_SET_PARENT_CLK_FUNC  clock_i94xxx_dmic_set_parent_clk

#include "clk_cntl_add_device.h"



/*******************************************/
/********** i94xxx_I2S_MCLK_clk_dev ********/
static void clock_i94xxx_I2S_MCLK_get_freq(uint32_t *freq, uint32_t parent_freq)
{
	uint32_t mclkdiv;
	mclkdiv = I2S0->CLKDIV & I2S_CLKDIV_MCLKDIV_Msk;
	if (0 == mclkdiv)
	{
		*freq = parent_freq;
	}
	else
	{
		*freq = parent_freq / 2;
	}
}

#define DT_DEV_NAME                i94xxx_I2S_MCLK_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_GET_FREQ_FUNC        clock_i94xxx_I2S_MCLK_get_freq
#define CLK_DT_DEFAULT_PARENT      i94xxx_i2s_clk_dev

#include "clk_cntl_add_device.h"



/*******************************************/
/********** i94xxx_I2S_BCLK_clk_dev ********/
static void clock_i94xxx_I2S_BCLK_get_freq(uint32_t *freq, uint32_t parent_freq)
{
	uint32_t bclkdiv;

	bclkdiv = (I2S0->CLKDIV & I2S_CLKDIV_BCLKDIV_Msk) >>
											I2S_CLKDIV_BCLKDIV_Pos;
	*freq = parent_freq / (2 * (bclkdiv + 1));
}

#define DT_DEV_NAME                i94xxx_I2S_BCLK_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_GET_FREQ_FUNC       clock_i94xxx_I2S_BCLK_get_freq
#define CLK_DT_DEFAULT_PARENT      i94xxx_i2s_clk_dev

#include "clk_cntl_add_device.h"


/********************************************/
/********** i94xxx_I2S_FSCLK_clk_dev ********/
static void clock_i94xxx_I2S_FSCLK_get_freq(
						uint32_t *freq, uint32_t parent_freq)
{
	uint16_t num_of_channels;
	uint16_t channel_width;

	num_of_channels =
			(I2S0->CTL0 & I2S_CTL0_TDMCHNUM_Msk) >> I2S_CTL0_TDMCHNUM_Pos;
	num_of_channels = 2 * (1 + num_of_channels);
	channel_width =
			(I2S0->CTL0 & I2S_CTL0_CHWIDTH_Msk) >> I2S_CTL0_CHWIDTH_Pos;
	channel_width = 8 * (1 + channel_width);
	*freq = parent_freq / (num_of_channels * channel_width);
}

#define DT_DEV_NAME                i94xxx_I2S_FSCLK_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_GET_FREQ_FUNC       clock_i94xxx_I2S_FSCLK_get_freq
#define CLK_DT_DEFAULT_PARENT      i94xxx_I2S_BCLK_clk_dev

#include "clk_cntl_add_device.h"



/**************************************************/
/********** i94xxx_I2S_onSPI1_MCLK_clk_dev ********/
static void clock_i94xxx_I2S_onSPI1_MCLK_get_freq(
							uint32_t *freq, uint32_t parent_freq)
{
	uint32_t mclkdiv;

	mclkdiv = ((SPI_T*)SPI1_BASE)->I2SCLK & SPI_I2SCLK_MCLKDIV_Msk;
	if (0 == mclkdiv)
	{
		*freq = parent_freq;
	}
	else
	{
		*freq = parent_freq / 2;
	}
}

#define DT_DEV_NAME                i94xxx_I2S_onSPI1_MCLK_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_GET_FREQ_FUNC       clock_i94xxx_I2S_onSPI1_MCLK_get_freq
#define CLK_DT_DEFAULT_PARENT      i94xxx_spi1_clk_dev

#include "clk_cntl_add_device.h"



/**************************************************/
/********** i94xxx_I2S_onSPI1_BCLK_clk_dev ********/
static void clock_i94xxx_I2S_onSPI1_BCLK_get_freq(
									uint32_t *freq, uint32_t parent_freq)
{
	uint32_t bclkdiv;
	bclkdiv = (((SPI_T*)SPI1_BASE)->I2SCLK & SPI_I2SCLK_BCLKDIV_Msk) >>
													SPI_I2SCLK_BCLKDIV_Pos;
	*freq = parent_freq / (2 * (bclkdiv + 1));
}

#define DT_DEV_NAME                i94xxx_I2S_onSPI1_BCLK_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_GET_FREQ_FUNC       clock_i94xxx_I2S_onSPI1_BCLK_get_freq
#define CLK_DT_DEFAULT_PARENT      i94xxx_spi1_clk_dev

#include "clk_cntl_add_device.h"



/***************************************************/
/********** i94xxx_I2S_FSCLK_onSPI1_clk_dev ********/
static void clock_i94xxx_I2S_onSPI1_FSCLK_get_freq(
										uint32_t *freq, uint32_t parent_freq)
{
	uint16_t num_of_channels;
	uint16_t channel_width;

	num_of_channels = 2;
	channel_width = (((SPI_T*)SPI1_BASE)->I2SCTL & SPI_I2SCTL_WDWIDTH_Msk) >>
														SPI_I2SCTL_WDWIDTH_Pos;
	channel_width = 8 * (1 + channel_width);
	*freq = parent_freq / (num_of_channels * channel_width);
}

#define DT_DEV_NAME                i94xxx_I2S_onSPI1_FSCLK_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_GET_FREQ_FUNC       clock_i94xxx_I2S_onSPI1_FSCLK_get_freq
#define CLK_DT_DEFAULT_PARENT      i94xxx_I2S_onSPI1_BCLK_clk_dev

#include "clk_cntl_add_device.h"


/**************************************************/
/********** i94xxx_I2S_onSPI2_MCLK_clk_dev ********/
static void clock_i94xxx_I2S_onSPI2_MCLK_get_freq(
							uint32_t *freq, uint32_t parent_freq)
{
	uint32_t mclkdiv;

	mclkdiv = ((SPI_T*)SPI2_BASE)->I2SCLK & SPI_I2SCLK_MCLKDIV_Msk;
	if (0 == mclkdiv)
	{
		*freq = parent_freq;
	}
	else
	{
		*freq = parent_freq / 2;
	}
}

#define DT_DEV_NAME                i94xxx_I2S_onSPI2_MCLK_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_GET_FREQ_FUNC       clock_i94xxx_I2S_onSPI2_MCLK_get_freq
#define CLK_DT_DEFAULT_PARENT      i94xxx_spi2_clk_dev

#include "clk_cntl_add_device.h"



/**************************************************/
/********** i94xxx_I2S_onSPI2_BCLK_clk_dev ********/
static void clock_i94xxx_I2S_onSPI2_BCLK_get_freq(
									uint32_t *freq, uint32_t parent_freq)
{
	uint32_t bclkdiv;
	bclkdiv = (((SPI_T*)SPI2_BASE)->I2SCLK & SPI_I2SCLK_BCLKDIV_Msk) >>
													SPI_I2SCLK_BCLKDIV_Pos;
	*freq = parent_freq / (2 * (bclkdiv + 1));
}

#define DT_DEV_NAME                i94xxx_I2S_onSPI2_BCLK_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_GET_FREQ_FUNC       clock_i94xxx_I2S_onSPI2_BCLK_get_freq
#define CLK_DT_DEFAULT_PARENT      i94xxx_spi2_clk_dev

#include "clk_cntl_add_device.h"



/***************************************************/
/********** i94xxx_I2S_FSCLK_onSPI2_clk_dev ********/
static void clock_i94xxx_I2S_onSPI2_FSCLK_get_freq(
										uint32_t *freq, uint32_t parent_freq)
{
	uint16_t num_of_channels;
	uint16_t channel_width;

	num_of_channels = 2;
	channel_width = (((SPI_T*)SPI2_BASE)->I2SCTL & SPI_I2SCTL_WDWIDTH_Msk) >>
														SPI_I2SCTL_WDWIDTH_Pos;
	channel_width = 8 * (1 + channel_width);
	*freq = parent_freq / (num_of_channels * channel_width);
}

#define DT_DEV_NAME                i94xxx_I2S_onSPI2_FSCLK_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_GET_FREQ_FUNC       clock_i94xxx_I2S_onSPI2_FSCLK_get_freq
#define CLK_DT_DEFAULT_PARENT      i94xxx_I2S_onSPI2_BCLK_clk_dev

#include "clk_cntl_add_device.h"



/***************************************/
/********** i94xxx_dpwm_clk_dev ********/
static void clock_i94xxx_dpwm_enable()
{
	CLK->APBCLK1 |= CLK_APBCLK1_DPWMCKEN_Msk;
}

static void clock_i94xxx_dpwm_set_parent_clk(struct dev_desc_t *parent_clk)
{
	uint32_t curr_val;
	curr_val = (CLK->CLKSEL2 & ~ CLK_CLKSEL2_DPWMSEL_Msk);
	if (i94xxx_xtal_clk_dev == parent_clk)
	{
		CLK->CLKSEL2 = curr_val | CLK_CLKSEL2_DPWMSEL_HXT;
	}
	else if (i94xxx_hirc_clk_dev == parent_clk)
	{
		CLK->CLKSEL2 = curr_val | CLK_CLKSEL2_DPWMSEL_HIRC;
	}
	else if (i94xxx_pll_clk_dev == parent_clk)
	{
		CLK->CLKSEL2 = curr_val | CLK_CLKSEL2_DPWMSEL_PLL;
	}
	else if (i94xxx_pclk0_clk_dev == parent_clk)
	{
		CLK->CLKSEL2 = curr_val | CLK_CLKSEL2_DPWMSEL_PCLK0;
	}
	else
	{
		CRITICAL_ERROR("bad parent clock \n");
	}
}

#define DT_DEV_NAME                i94xxx_dpwm_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_ENABLE_CLK_FUNC      clock_i94xxx_dpwm_enable
#define CLK_DT_SET_PARENT_CLK_FUNC  clock_i94xxx_dpwm_set_parent_clk

#include "clk_cntl_add_device.h"




/***********************************/
/********** i9xxxx_uart0_dev ********/
static void clock_i9xxxx_uart0_enable()
{
	CLK_EnableModuleClock(UART0_MODULE);
}

static void clock_i9xxxx_uart0_set_parent_clk(struct dev_desc_t *parent_clk)
{
	if (i94xxx_xtal_clk_dev == parent_clk)
	{
		CLK_SetModuleClock(UART0_MODULE,
				CLK_CLKSEL1_UART0SEL_HXT, CLK_CLKDIV0_UART0(1));
	}
	else if (i94xxx_hirc_clk_dev == parent_clk)
	{
		CLK_SetModuleClock(UART0_MODULE,
				CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));
	}
	else if (i94xxx_pll_clk_dev == parent_clk)
	{
		CLK_SetModuleClock(UART0_MODULE,
				CLK_CLKSEL1_UART0SEL_PLL, CLK_CLKDIV0_UART0(1));
	}
	else
	{
		CRITICAL_ERROR("bad parent clock \n");
	}
}

#define DT_DEV_NAME               i9xxxx_uart0_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_ENABLE_CLK_FUNC      clock_i9xxxx_uart0_enable
#define CLK_DT_SET_PARENT_CLK_FUNC  clock_i9xxxx_uart0_set_parent_clk

#include "clk_cntl_add_device.h"


/***************************************/
/********** i94xxx_i2c1_clk_dev ********/
static void clock_i94xxx_i2c1_enable()
{
	CLK_EnableModuleClock(I2C1_MODULE);
}

#define DT_DEV_NAME                i94xxx_i2c1_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_ENABLE_CLK_FUNC      clock_i94xxx_i2c1_enable
#define CLK_DT_DEFAULT_PARENT       i94xxx_pclk1_clk_dev

#include "clk_cntl_add_device.h"


/***************************************/
/********** i94xxx_i2c0_clk_dev ********/
static void clock_i94xxx_i2c0_enable()
{
	CLK_EnableModuleClock(I2C0_MODULE);
}

#define DT_DEV_NAME                i94xxx_i2c0_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_ENABLE_CLK_FUNC      clock_i94xxx_i2c0_enable
#define CLK_DT_DEFAULT_PARENT       i94xxx_pclk0_clk_dev

#include "clk_cntl_add_device.h"



/***************************************/
/********** i94xxx_spi0_clk_dev ********/
static void clock_i94xxx_spi0_enable()
{
	CLK_EnableModuleClock(SPI0_MODULE);
}

static void clock_i94xxx_spi0_set_parent_clk(struct dev_desc_t *parent_clk)
{
	if (i94xxx_xtal_clk_dev == parent_clk)
	{
		CLK_SetModuleClock(SPI0_MODULE, CLK_CLKSEL2_SPI0SEL_HXT, MODULE_NoMsk);
	}
	else if (i94xxx_pclk0_clk_dev == parent_clk)
	{
		CLK_SetModuleClock(SPI0_MODULE,
				CLK_CLKSEL2_SPI0SEL_PCLK0, MODULE_NoMsk);
	}
	else if (i94xxx_pll_clk_dev == parent_clk)
	{
		CLK_SetModuleClock(SPI0_MODULE, CLK_CLKSEL2_SPI0SEL_PLL, MODULE_NoMsk);
	}
	else if (i94xxx_hirc_clk_dev == parent_clk)
	{
		CLK_SetModuleClock(SPI0_MODULE, CLK_CLKSEL2_SPI0SEL_HIRC, MODULE_NoMsk);
	}
	else
	{
		CRITICAL_ERROR("bad parent clock \n");
	}
}

#define DT_DEV_NAME                i94xxx_spi0_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_ENABLE_CLK_FUNC      clock_i94xxx_spi0_enable
#define CLK_DT_SET_PARENT_CLK_FUNC  clock_i94xxx_spi0_set_parent_clk
#define CLK_DT_DEFAULT_PARENT       i94xxx_pclk1_clk_dev

#include "clk_cntl_add_device.h"


static void init_clocks(struct clk_cntl_i94xxx_cfg_t *cfg_hndl)
{
	uint32_t rate;

	if (0 != cfg_hndl->xtal_rate)
	{
		DEV_IOCTL_0_PARAMS(i94xxx_xtal_clk_dev, IOCTL_DEVICE_START);
		DEV_IOCTL_0_PARAMS(i94xxx_xtal_clk_dev, CLK_IOCTL_ENABLE);
		DEV_IOCTL_1_PARAMS(i94xxx_xtal_clk_dev,
				CLK_IOCTL_SET_FREQ, &cfg_hndl->xtal_rate);
	}

	if (0 != cfg_hndl->hirc_rate)
	{
		DEV_IOCTL_0_PARAMS(i94xxx_hirc_clk_dev, IOCTL_DEVICE_START);
		DEV_IOCTL_0_PARAMS(i94xxx_hirc_clk_dev, CLK_IOCTL_ENABLE);
		DEV_IOCTL_1_PARAMS(i94xxx_hirc_clk_dev,
				CLK_IOCTL_SET_FREQ, &cfg_hndl->hirc_rate);
	}

	DEV_IOCTL_0_PARAMS(i94xxx_pll_clk_dev, IOCTL_DEVICE_START);
	DEV_IOCTL_1_PARAMS(i94xxx_pll_clk_dev,
			CLK_IOCTL_SET_PARENT, cfg_hndl->pll_src_clk_dev);
	DEV_IOCTL_1_PARAMS(i94xxx_pll_clk_dev,
			CLK_IOCTL_SET_FREQ, &cfg_hndl->pll_rate);

	DEV_IOCTL_0_PARAMS(i94xxx_hclk_clk_dev, IOCTL_DEVICE_START);
	DEV_IOCTL_1_PARAMS(i94xxx_hclk_clk_dev,
			CLK_IOCTL_SET_PARENT, i94xxx_pll_clk_dev);
//	DEV_IOCTL_1_PARAMS(i94xxx_hclk_clk_dev,
//			CLK_IOCTL_SET_FREQ, &cfg_hndl->hclk_rate);


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
	DEV_IOCTL_0_PARAMS(i94xxx_pclk0_clk_dev, IOCTL_DEVICE_START);
	DEV_IOCTL_1_PARAMS(i94xxx_pclk0_clk_dev, CLK_IOCTL_SET_FREQ, &rate);
	DEV_IOCTL_0_PARAMS(i94xxx_pclk1_clk_dev, IOCTL_DEVICE_START);
	DEV_IOCTL_1_PARAMS(i94xxx_pclk1_clk_dev, CLK_IOCTL_SET_FREQ, &rate);

}


/**
 * clock_control_i94xxx_ioctl()
 *
 * return:
 */
static uint8_t clock_control_i94xxx_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct clk_cntl_i94xxx_cfg_t *cfg_hndl;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(clock_control_i94xxx, adev);

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

#define MODULE_NAME                     clock_control_i94xxx
#define MODULE_IOCTL_FUNCTION           clock_control_i94xxx_ioctl
#define MODULE_HAS_NO_RUNTIME_DATA
#include "add_module.h"
