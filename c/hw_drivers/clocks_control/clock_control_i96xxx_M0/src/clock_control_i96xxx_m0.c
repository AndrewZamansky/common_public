/*
 *
 * file :   clocks_control_i96xxx_m0.c
 *
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "clock_control_api.h"
#include "clock_control_common_api.h"
#include "clock_control_i96xxx_m0_api.h"
#include "clock_control_i96xxx_m0.h"

#include "I96100.h"

#include "clk.h"


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
/********** i96xxx_xtal_clk_dev ********/
static void clock_i96xxx_xtal_enable()
{
	CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);
	CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);
}

static void clock_i96xxx_xtal_set_freq(uint32_t freq, uint32_t parent_freq)
{
	__HXT = freq;
	gau32ClkSrcTbl[0] = freq;
}

#define DT_DEV_NAME                i96xxx_xtal_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_ENABLE_CLK_FUNC    clock_i96xxx_xtal_enable
#define CLK_DT_SET_FREQ_FUNC      clock_i96xxx_xtal_set_freq

#include "clk_cntl_add_device.h"



/****************************************/
/********** i96xxx_lxtal_clk_dev ********/
static void clock_i96xxx_lxtal_enable()
{
	CLK_EnableXtalRC(CLK_PWRCTL_LXTEN_Msk);
	CLK_WaitClockReady(CLK_STATUS_LXTSTB_Msk);
}

static void clock_i96xxx_lxtal_set_freq(uint32_t freq, uint32_t parent_freq)
{
	__LXT = freq;
	gau32ClkSrcTbl[1] = freq;
}

#define DT_DEV_NAME                i96xxx_lxtal_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_ENABLE_CLK_FUNC    clock_i96xxx_lxtal_enable
#define CLK_DT_SET_FREQ_FUNC      clock_i96xxx_lxtal_set_freq

#include "clk_cntl_add_device.h"



/***************************************/
/********** i96xxx_hirc_clk_dev ********/
static void clock_i96xxx_hirc_enable()
{
	CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
	CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
}

static void clock_i96xxx_hirc_set_freq(uint32_t freq, uint32_t parent_freq)
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
#ifndef I96XXX_FPGA_SIMULATION
		CRITICAL_ERROR("bad clock rate \n");
#endif
	}
	__HIRC = freq;
}

#define DT_DEV_NAME               i96xxx_hirc_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_ENABLE_CLK_FUNC    clock_i96xxx_hirc_enable
#define CLK_DT_SET_FREQ_FUNC      clock_i96xxx_hirc_set_freq

#include "clk_cntl_add_device.h"


/***************************************/
/********** i96xxx_pll_clk_dev ********/
static void clock_i96xxx_pll_set_freq(uint32_t freq, uint32_t parent_freq)
{
	uint32_t PLL_Src;

	if (freq > 500000000)
	{
		CRITICAL_ERROR("bad PLL rate");
	}

	if (CLK->PLLCTL & CLK_PLLCTL_PLLSRC_Msk)
	{
		PLL_Src = CLK_PLLCTL_PLLSRC_HIRC;
	}
	else
	{
		PLL_Src = CLK_PLLCTL_PLLSRC_HXT;
	}
	if (0 == CLK_EnablePLL(PLL_Src, freq ))
	{
		CRITICAL_ERROR("cannot set PLL clock");
	}
}

static void clock_i96xxx_pll_get_freq(uint32_t *freq, uint32_t parent_freq)
{
	*freq = CLK_GetPLLClockFreq();
}


static void clock_i96xxx_pll_set_parent_clk(struct dev_desc_t *parent_clk)
{
	CLK->PLLCTL &= ~CLK_PLLCTL_PLLSRC_Msk;
	if (i96xxx_xtal_clk_dev == parent_clk)
	{
		// remain 0
	}
	else if (i96xxx_hirc_clk_dev == parent_clk)
	{
		CLK->PLLCTL |= (1 << CLK_PLLCTL_PLLSRC_Pos);
	}
	else
	{
		CRITICAL_ERROR("bad parent clock \n");
	}
	CLK_WaitClockReady(CLK_STATUS_PLLSTB_Msk);
}

#define DT_DEV_NAME               i96xxx_pll_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_DEFAULT_PARENT       i96xxx_xtal_clk_dev
#define CLK_DT_SET_FREQ_FUNC        clock_i96xxx_pll_set_freq
#define CLK_DT_GET_FREQ_FUNC        clock_i96xxx_pll_get_freq
#define CLK_DT_SET_PARENT_CLK_FUNC  clock_i96xxx_pll_set_parent_clk

#include "clk_cntl_add_device.h"



/***************************************/
/********** i96xxx_hclk_clk_dev ********/
static void clock_i96xxx_hclk_set_parent_clk(struct dev_desc_t *parent_clk)
{
	uint32_t prev_div;

	prev_div = ((CLK->CLKDIV0 & CLK_CLKDIV0_HCLKDIV_Msk) >>
												CLK_CLKDIV0_HCLKDIV_Pos) + 1;
	if (i96xxx_xtal_clk_dev == parent_clk)
	{
		CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HXT, CLK_CLKDIV0_HCLK(prev_div));
	}
	else if (i96xxx_hirc_clk_dev == parent_clk)
	{
		CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(prev_div));
	}
	else if (i96xxx_pll_clk_dev == parent_clk)
	{
		// to be sure that hclk is less then 150Mh set divider to 4,
		// it will be set correctly in set_rate function later
		CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_PLL, CLK_CLKDIV0_HCLK(4));
	}
	else
	{
		CRITICAL_ERROR("bad parent clock \n");
	}
}

/********************************************/
/******** clock_i96xxx_hclk_set_freq ********/
static void clock_i96xxx_hclk_set_freq(uint32_t freq, uint32_t parent_freq)
{
	uint32_t div;
	uint32_t hclk_src;

	div = parent_freq / freq;
	if ((div * freq) != parent_freq)
	{
		CRITICAL_ERROR("cannot create hclk");
	}

	hclk_src = CLK->CLKSEL0 & CLK_CLKSEL0_HCLKSEL_Msk;
	if (CLK_CLKSEL0_HCLKSEL_HXT == hclk_src)
	{
		CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HXT, CLK_CLKDIV0_HCLK(div));
	}
	else if (CLK_CLKSEL0_HCLKSEL_HIRC == hclk_src)
	{
		CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(div));
	}
	else if (CLK_CLKSEL0_HCLKSEL_PLL == hclk_src)
	{
		// to be sure that pclk is less then 75Mh set divider to 2,
		// it will be set correctly in set_rate of PCLK0/1 function later
		if (freq > 75000000)
		{
			CLK->PCLKDIV = (CLK->PCLKDIV & (~CLK_PCLKDIV_APB0DIV_Msk)) |
												(1 << CLK_PCLKDIV_APB0DIV_Pos);
			CLK->PCLKDIV = (CLK->PCLKDIV & (~CLK_PCLKDIV_APB1DIV_Msk)) |
												(1 << CLK_PCLKDIV_APB1DIV_Pos);
		}
		CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_PLL, CLK_CLKDIV0_HCLK(div));
	}
	else
	{
		CRITICAL_ERROR("bad parent clock \n");
	}

}


#define DT_DEV_NAME               i96xxx_hclk_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_DEFAULT_PARENT         i96xxx_hirc_clk_dev
#define CLK_DT_SET_PARENT_CLK_FUNC    clock_i96xxx_hclk_set_parent_clk
#define CLK_DT_SET_FREQ_FUNC          clock_i96xxx_hclk_set_freq

#include "clk_cntl_add_device.h"


/******************************************/
/********** i96xxx_systick_clk_dev ********/
#define DT_DEV_NAME                i96xxx_systick_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_DEFAULT_PARENT       i96xxx_hclk_clk_dev

#include "clk_cntl_add_device.h"



/****************************************/
/********** i96xxx_pclk0_clk_dev ********/
static void clock_i96xxx_pclk0_set_freq(uint32_t freq, uint32_t parent_freq)
{
	uint32_t div;

	div = 1;
	while (freq < parent_freq)
	{
		div++;
		if (5 < div) break;
		freq *= 2;
	}
	if (freq != parent_freq)
	{
		CRITICAL_ERROR("cannot create requested pclk0 \n");
	}

	CLK->PCLKDIV = (CLK->PCLKDIV & (~CLK_PCLKDIV_APB0DIV_Msk)) |
									((div - 1) << CLK_PCLKDIV_APB0DIV_Pos);
}

static void clock_i96xxx_pclk0_get_freq(uint32_t *freq, uint32_t parent_freq)
{
	*freq = CLK_GetPCLK0Freq();
}

#define DT_DEV_NAME               i96xxx_pclk0_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_DEFAULT_PARENT     i96xxx_hclk_clk_dev
#define CLK_DT_SET_FREQ_FUNC      clock_i96xxx_pclk0_set_freq
#define CLK_DT_GET_FREQ_FUNC      clock_i96xxx_pclk0_get_freq

#include "clk_cntl_add_device.h"



/****************************************/
/********** i96xxx_pclk1_clk_dev ********/
static void clock_i96xxx_pclk1_set_freq(uint32_t freq, uint32_t parent_freq)
{
	uint32_t div;

	div = 1;
	while (freq < parent_freq)
	{
		div++;
		if (5 < div) break;
		freq *= 2;
	}
	if (freq != parent_freq)
	{
		CRITICAL_ERROR("cannot create requested pclk1 \n");
	}

	CLK->PCLKDIV = (CLK->PCLKDIV & (~CLK_PCLKDIV_APB1DIV_Msk)) |
									((div - 1) << CLK_PCLKDIV_APB1DIV_Pos);
}

static void clock_i96xxx_pclk1_get_freq(uint32_t *freq, uint32_t parent_freq)
{
	*freq = CLK_GetPCLK1Freq();
}

#define DT_DEV_NAME               i96xxx_pclk1_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_DEFAULT_PARENT     i96xxx_hclk_clk_dev
#define CLK_DT_SET_FREQ_FUNC      clock_i96xxx_pclk1_set_freq
#define CLK_DT_GET_FREQ_FUNC      clock_i96xxx_pclk1_get_freq

#include "clk_cntl_add_device.h"


/***********************************/
/********** i9xxxx_uart0_dev ********/
static void clock_i9xxxx_uart0_enable()
{
	CLK_EnableModuleClock(UART0_MODULE);
}

static void clock_i9xxxx_uart0_set_parent_clk(struct dev_desc_t *parent_clk)
{
	if (i96xxx_xtal_clk_dev == parent_clk)
	{
		CLK_SetModuleClock(UART0_MODULE,
				CLK_CLKSEL1_UART0SEL_HXT, CLK_CLKDIV0_UART0(1));
	}
	else if (i96xxx_hirc_clk_dev == parent_clk)
	{
		CLK_SetModuleClock(UART0_MODULE,
				CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));
	}
	else if (i96xxx_pll_clk_dev == parent_clk)
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

#define CLK_DT_DEFAULT_PARENT       i96xxx_hirc_clk_dev
#define CLK_DT_ENABLE_CLK_FUNC      clock_i9xxxx_uart0_enable
#define CLK_DT_SET_PARENT_CLK_FUNC  clock_i9xxxx_uart0_set_parent_clk

#include "clk_cntl_add_device.h"


/***************************************/
/********** i96xxx_i2s0_clk_dev ********/
static void clock_i96xxx_i2s0_enable()
{
	CLK->APBCLK0 |= CLK_APBCLK0_I2S0CKEN_Msk;
}

static void clock_i96xxx_i2s0_set_parent_clk(struct dev_desc_t *parent_clk)
{
	uint32_t curr_val;

	curr_val = (CLK->CLKSEL3 & ~ CLK_CLKSEL3_I2S0SEL_Msk);
	if (i96xxx_xtal_clk_dev == parent_clk)
	{
		CLK->CLKSEL3 = curr_val | CLK_CLKSEL3_I2S0SEL_HXT;
	}
	else if (i96xxx_hirc_clk_dev == parent_clk)
	{
		CLK->CLKSEL3 = curr_val | CLK_CLKSEL3_I2S0SEL_HIRC;
	}
	else if (i96xxx_pll_clk_dev == parent_clk)
	{
		CLK->CLKSEL3 = curr_val | CLK_CLKSEL3_I2S0SEL_PLL;
	}
	else if (i96xxx_pclk0_clk_dev == parent_clk)
	{
		CLK->CLKSEL3 = curr_val | CLK_CLKSEL3_I2S0SEL_PCLK0;
	}
	else
	{
		CRITICAL_ERROR("bad parent clock \n");
	}
}

#define DT_DEV_NAME               i96xxx_i2s0_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_DEFAULT_PARENT       i96xxx_hirc_clk_dev
#define CLK_DT_ENABLE_CLK_FUNC      clock_i96xxx_i2s0_enable
#define CLK_DT_SET_PARENT_CLK_FUNC  clock_i96xxx_i2s0_set_parent_clk

#include "clk_cntl_add_device.h"


/*******************************************/
/********** i96xxx_I2S0_MCLK_clk_dev ********/
static void clock_i96xxx_I2S0_MCLK_get_freq(
						uint32_t *freq, uint32_t parent_freq)
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

#define DT_DEV_NAME                i96xxx_I2S0_MCLK_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_GET_FREQ_FUNC       clock_i96xxx_I2S0_MCLK_get_freq
#define CLK_DT_DEFAULT_PARENT      i96xxx_i2s0_clk_dev

#include "clk_cntl_add_device.h"



/*******************************************/
/********** i96xxx_I2S0_BCLK_clk_dev ********/
static void clock_i96xxx_I2S0_BCLK_get_freq(
					uint32_t *freq, uint32_t parent_freq)
{
	uint32_t bclkdiv;

	bclkdiv = (I2S0->CLKDIV & I2S_CLKDIV_BCLKDIV_Msk) >>
											I2S_CLKDIV_BCLKDIV_Pos;
	*freq = parent_freq / (2 * (bclkdiv + 1));
}

#define DT_DEV_NAME                i96xxx_I2S0_BCLK_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_GET_FREQ_FUNC       clock_i96xxx_I2S0_BCLK_get_freq
#define CLK_DT_DEFAULT_PARENT      i96xxx_i2s0_clk_dev

#include "clk_cntl_add_device.h"


/********************************************/
/********** i96xxx_I2S0_FSCLK_clk_dev ********/
static void clock_i96xxx_I2S0_FSCLK_get_freq(
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

#define DT_DEV_NAME                i96xxx_I2S0_FSCLK_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_GET_FREQ_FUNC       clock_i96xxx_I2S0_FSCLK_get_freq
#define CLK_DT_DEFAULT_PARENT      i96xxx_I2S0_BCLK_clk_dev

#include "clk_cntl_add_device.h"



/***************************************/
/********** i96xxx_dsp_clk_dev ********/
static void clock_i96xxx_dsp_enable()
{
    CLK_EnableModuleClock(DSP_MODULE);
    __NOP();
    __NOP();
    __NOP();
    __NOP();
}

static void clock_i96xxx_dsp_set_freq(uint32_t freq, uint32_t parent_freq)
{
	uint32_t div;

	div = parent_freq / freq;

	if ((freq * div) != parent_freq)
	{
		CRITICAL_ERROR("bad clock rate\n");
	}

	CLK_SetModuleClock(DSP_MODULE, 0, CLK_CLKDIV0_DSPCLK(div));
}

#define DT_DEV_NAME               i96xxx_dsp_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_DEFAULT_PARENT     i96xxx_pll_clk_dev
#define CLK_DT_ENABLE_CLK_FUNC    clock_i96xxx_dsp_enable
#define CLK_DT_SET_FREQ_FUNC      clock_i96xxx_dsp_set_freq

#include "clk_cntl_add_device.h"


/***************************************/
/********** i96xxx_usb_clk_dev ********/
static void clock_i96xxx_usb_enable()
{
	CLK_EnableModuleClock(USBD_MODULE);
}

static void clock_i96xxx_usb_set_freq(uint32_t freq, uint32_t parent_freq)
{
	uint32_t div;

	div = (parent_freq / freq);
	if ((freq * div) != parent_freq)
	{
		CRITICAL_ERROR("bad clock rate\n");
	}

	if (div)
	{
		div--;
	}

	CLK->CLKDIV0 = (CLK->CLKDIV0 & (~CLK_CLKDIV0_USBDIV_Msk))
						| (div << CLK_CLKDIV0_USBDIV_Pos);
}

static void clock_i96xxx_usb_get_freq(uint32_t *freq, uint32_t parent_freq)
{
	uint32_t div;

	div =
		((CLK->CLKDIV0 & CLK_CLKDIV0_USBDIV_Msk) >> CLK_CLKDIV0_USBDIV_Pos) + 1;
	*freq = parent_freq / div;
}

static void clock_i96xxx_usb_set_parent_clk(struct dev_desc_t *parent_clk)
{
	if (i96xxx_hirc_clk_dev == parent_clk)
	{
		CLK->CLKSEL4 &= ~(0x1ul << 24); //CLKSEL4 not exist yet in BSP
		//*((uint32_t *)0x40000224) &= ~(0x1ul << 24);
	}
	else if (i96xxx_pll_clk_dev == parent_clk)
	{
		CLK->CLKSEL4 |= (0x1ul << 24); //CLKSEL4 not exist yet in BSP
		//*((uint32_t *)0x40000224) |= (0x1UL<<24);
	}
	else
	{
		CRITICAL_ERROR("bad parent clock \n");
	}
}

#define DT_DEV_NAME               i96xxx_usb_clk_dev
#define DT_DEV_MODULE             clk_cntl

#define CLK_DT_ENABLE_CLK_FUNC      clock_i96xxx_usb_enable
#define CLK_DT_SET_FREQ_FUNC        clock_i96xxx_usb_set_freq
#define CLK_DT_GET_FREQ_FUNC        clock_i96xxx_usb_get_freq
#define CLK_DT_SET_PARENT_CLK_FUNC  clock_i96xxx_usb_set_parent_clk

#include "clk_cntl_add_device.h"



/***************************************/
/********** i96xxx_i2c0_clk_dev ********/
static void clock_i96xxx_i2c0_enable()
{
	CLK_EnableModuleClock(I2C0_MODULE);
}

#define DT_DEV_NAME                i96xxx_i2c0_clk_dev
#define DT_DEV_MODULE              clk_cntl

#define CLK_DT_ENABLE_CLK_FUNC      clock_i96xxx_i2c0_enable
#define CLK_DT_DEFAULT_PARENT       i96xxx_pclk0_clk_dev

#include "clk_cntl_add_device.h"


static void init_clocks(struct clk_cntl_i96xxx_m0_cfg_t *cfg_hndl)
{
	if (0 != cfg_hndl->xtal_rate)
	{
		DEV_IOCTL_0_PARAMS(i96xxx_xtal_clk_dev, IOCTL_DEVICE_START);
		DEV_IOCTL_0_PARAMS(i96xxx_xtal_clk_dev, CLK_IOCTL_ENABLE);
		DEV_IOCTL_1_PARAMS(i96xxx_xtal_clk_dev,
				CLK_IOCTL_SET_FREQ, &cfg_hndl->xtal_rate);
	}

	if (0 != cfg_hndl->hirc_rate)
	{
		DEV_IOCTL_0_PARAMS(i96xxx_hirc_clk_dev, IOCTL_DEVICE_START);
		DEV_IOCTL_0_PARAMS(i96xxx_hirc_clk_dev, CLK_IOCTL_ENABLE);
		DEV_IOCTL_1_PARAMS(i96xxx_hirc_clk_dev,
				CLK_IOCTL_SET_FREQ, &cfg_hndl->hirc_rate);
	}

	if (0 != cfg_hndl->pll_rate)
	{
		DEV_IOCTL_0_PARAMS(i96xxx_pll_clk_dev, IOCTL_DEVICE_START);
		DEV_IOCTL_1_PARAMS(i96xxx_pll_clk_dev,
				CLK_IOCTL_SET_PARENT, cfg_hndl->pll_src_clk_dev);
		DEV_IOCTL_1_PARAMS(i96xxx_pll_clk_dev,
				CLK_IOCTL_SET_FREQ, &cfg_hndl->pll_rate);
	}

	DEV_IOCTL_0_PARAMS(i96xxx_hclk_clk_dev, IOCTL_DEVICE_START);
	DEV_IOCTL_1_PARAMS(i96xxx_hclk_clk_dev,
			CLK_IOCTL_SET_PARENT, cfg_hndl->hclk_src_clk_dev);
	DEV_IOCTL_1_PARAMS(i96xxx_hclk_clk_dev,
			CLK_IOCTL_SET_FREQ, &cfg_hndl->hclk_rate);

	if (0 != cfg_hndl->pclk0_rate)
	{
		DEV_IOCTL_0_PARAMS(i96xxx_pclk0_clk_dev, IOCTL_DEVICE_START);
		DEV_IOCTL_1_PARAMS(i96xxx_pclk0_clk_dev,
				CLK_IOCTL_SET_FREQ, &cfg_hndl->pclk0_rate);
	}

	if (0 != cfg_hndl->pclk1_rate)
	{
		DEV_IOCTL_0_PARAMS(i96xxx_pclk1_clk_dev, IOCTL_DEVICE_START);
		DEV_IOCTL_1_PARAMS(i96xxx_pclk1_clk_dev,
				CLK_IOCTL_SET_FREQ, &cfg_hndl->pclk1_rate);
	}

	if (0 != cfg_hndl->dsp_rate)
	{
		DEV_IOCTL_0_PARAMS(i96xxx_dsp_clk_dev, IOCTL_DEVICE_START);
		DEV_IOCTL_0_PARAMS(i96xxx_dsp_clk_dev, CLK_IOCTL_ENABLE);
		DEV_IOCTL_1_PARAMS(i96xxx_dsp_clk_dev,
				CLK_IOCTL_SET_FREQ, &cfg_hndl->dsp_rate);
	}
}


/**
 * clock_control_i96xxx_m0_ioctl()
 *
 * return:
 */
static uint8_t clock_control_i96xxx_m0_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1,
		void * aIoctl_param2)
{
	struct clk_cntl_i96xxx_m0_cfg_t *cfg_hndl;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(clock_control_i96xxx_m0, adev);

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

#define MODULE_NAME                clock_control_i96xxx_m0
#define MODULE_IOCTL_FUNCTION      clock_control_i96xxx_m0_ioctl
#define MODULE_HAS_NO_RUNTIME_DATA
#include "add_module.h"
