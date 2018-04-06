/*
 *
 * I2S_i94xxx.c
 *
 */

/********  includes *********************/

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "I2S_i94xxx_api.h"
#include "I2S_i94xxx.h"
#include "irq_api.h"
#include "timer_wrapper_api.h"

#include "os_wrapper.h"

#include "I94100.h"

#include "i2s.h"
#include "dpwm.h"

#include "clock_control_i94xxx_api.h"

#include "_I2S_i94xxx_prerequirements_check.h"

/*following line add module to available module list for dynamic device tree*/
#include "I2S_i94xxx_add_component.h"

/********  defines *********************/


/********  types  *********************/
#define SYS_GPD_MFPL_PD0MFP_I2S0_BCLK   (0x04UL<<SYS_GPD_MFPL_PD0MFP_Pos) /*  */
#define SYS_GPD_MFPL_PD1MFP_I2S0_LRCLK  (0x04UL<<SYS_GPD_MFPL_PD1MFP_Pos) /*  */



/* ------------- External variables --------------------------*/

/* ------------------------ External functions --------------*/

/* ------------------------ Exported variables ---------------*/

//#define 	 DEBUG_USE_INTERRUPT

#ifdef 	DEBUG_USE_INTERRUPT
volatile int g_u32DataCount = 0;
volatile int status = 0;
#define TEST_COUNT	100
uint32_t data[TEST_COUNT+1] = {0};
volatile int pos = 0;
void I2S_IRQHandler()
{
    /* Write 2 TX values to TX FIFO */
  //  I2S_WRITE_TX_FIFO(SPI1, g_u32TxValue);
 //   I2S_WRITE_TX_FIFO(SPI1, g_u32TxValue);
    if((I2S0->STATUS0 & I2S_STATUS0_RXEMPTY_Msk) == 0)
    {
		status = I2S0->STATUS0;
		if (pos < TEST_COUNT)
		{
			pos++;
		}
		data[pos] = I2S0->RXFIFO;
    }
    g_u32DataCount += 2;
}
#endif


/**
 * I2S_i94xxx_ioctl()
 *
 * return:
 */
uint8_t I2S_i94xxx_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	struct I2S_i94xxx_cfg_t *cfg_hndl;
	struct I2S_i94xxx_runtime_t *runtime_handle;
//	uint8_t   	clock_mode;
	uint8_t		num_of_bytes_in_word;
	struct dev_desc_t	*clk_dev;
	struct dev_desc_t	*src_clock;
	uint32_t	sample_rate;
	uint32_t	i2s_format;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);
	src_clock = cfg_hndl->src_clock;
	sample_rate = cfg_hndl->sample_rate;
	i2s_format = cfg_hndl->i2s_format;
	num_of_bytes_in_word = cfg_hndl->num_of_bytes_in_word;
	clk_dev = i94xxx_i2s_clk_dev;

	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		if (I2S_I94XXX_API_DI_PIN_B13 == cfg_hndl->DI_pin)
		{
			SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB13MFP_Msk);
			SYS->GPB_MFPH |= SYS_GPB_MFPH_PB13MFP_I2S0_DI;
		}
		else if (I2S_I94XXX_API_DI_PIN_D4 == cfg_hndl->DI_pin)
		{
			SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD4MFP_Msk);
			SYS->GPD_MFPL |= SYS_GPD_MFPL_PD4MFP_I2S0_DI;
		}

		if (I2S_I94XXX_API_DO_PIN_B14 == cfg_hndl->DO_pin)
		{
			SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB14MFP_Msk);
			SYS->GPB_MFPH |= SYS_GPB_MFPH_PB14MFP_I2S0_DO;
		}
		else if (I2S_I94XXX_API_DO_PIN_D5 == cfg_hndl->DO_pin)
		{
			SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD5MFP_Msk);
			SYS->GPD_MFPL |= SYS_GPD_MFPL_PD5MFP_I2S0_DO;
		}

		if (I2S_I94XXX_API_MCLK_PIN_B15 == cfg_hndl->MCLK_pin)
		{
			SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB15MFP_Msk);
			SYS->GPB_MFPH |= SYS_GPB_MFPH_PB15MFP_I2S0_MCLK;
		}
		else if (I2S_I94XXX_API_MCLK_PIN_D2 == cfg_hndl->MCLK_pin)
		{
			SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD2MFP_Msk);
			SYS->GPD_MFPL |= SYS_GPD_MFPL_PD2MFP_I2S0_MCLK;
		}

		if (I2S_I94XXX_API_LRCLK_PIN_D1 == cfg_hndl->LRCLK_pin)
		{
			SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD1MFP_Msk);
			SYS->GPD_MFPL |= SYS_GPD_MFPL_PD1MFP_I2S0_LRCLK;
		}
		else if (I2S_I94XXX_API_LRCLK_PIN_D3 == cfg_hndl->LRCLK_pin)
		{
			SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD3MFP_Msk);
			SYS->GPD_MFPL |= SYS_GPD_MFPL_PD3MFP_I2S0_LRCK;
		}

		if (I2S_I94XXX_API_BCLK_PIN_D0 == cfg_hndl->BCLK_pin)
		{
			SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD0MFP_Msk);
			SYS->GPD_MFPL |= SYS_GPD_MFPL_PD0MFP_I2S0_BCLK;
		}
		else if (I2S_I94XXX_API_BCLK_PIN_D6 == cfg_hndl->BCLK_pin)
		{
			SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD6MFP_Msk);
			SYS->GPD_MFPL |= SYS_GPD_MFPL_PD6MFP_I2S0_BCLK;
		}



		DEV_IOCTL_1_PARAMS(clk_dev,	CLK_IOCTL_SET_PARENT, src_clock);
		DEV_IOCTL_0_PARAMS(clk_dev, CLK_IOCTL_ENABLE);

		runtime_handle->actual_sample_rate = I2S_Open(
				I2S0, cfg_hndl->clock_mode, sample_rate,
				(num_of_bytes_in_word-1) << SPI_I2SCTL_WDWIDTH_Pos,
				I2S_TDMCHNUM_4CH, I2S_STEREO, i2s_format);

		I2S_SET_TXTH(I2S0, I2S_FIFO_TX_LEVEL_WORD_4);
		I2S_SET_RXTH(I2S0, I2S_FIFO_RX_LEVEL_WORD_4);

		I2S_EnableMCLK(I2S0, sample_rate * 256);

		if ( I2S_FORMAT_PCMMSB == i2s_format )
		{
			/* I2S Configuration. */
			I2S_SET_PCMSYNC(I2S0, I2S_PCMSYNC_BCLK);
			I2S_SET_MONO_RX_CHANNEL(I2S0, I2S_MONO_RX_RIGHT);
			I2S_SET_STEREOORDER(I2S0, I2S_ORDER_EVENLOW);

			/* Set channel width. */
			I2S_SET_CHWIDTH(I2S0, I2S_CHWIDTH_32);
		}
		I2S_ENABLE_RX(I2S0);
		I2S_ENABLE(I2S0);

#ifdef DEBUG_USE_INTERRUPT
		irq_register_interrupt(I2S0_IRQn , I2S_IRQHandler);
		irq_set_priority(I2S0_IRQn , OS_MAX_INTERRUPT_PRIORITY_FOR_API_CALLS );
		irq_enable_interrupt(I2S0_IRQn);
		I2S_ENABLE_INT(I2S0, I2S_IEN_RXTHIEN_Msk);

#else
		I2S0->CTL0 |= I2S_CTL0_RXPDMAEN_Msk;
#endif
		// I2S_ENABLE_RX(I2S_module);
		// I2S_module->FIFOCTL |= I2S_FIFO_RX_LEVEL_WORD_4;
		// I2S_module->FIFOCTL |= SPI_FIFOCTL_RXRST_Msk;
		break;

	case I2S_I94XXX_ENABLE_OUTPUT_IOCTL:
		I2S_ENABLE_TX(I2S0);
		I2S0->CTL0 |= I2S_CTL0_TXPDMAEN_Msk;
		break;

	case I2S_I94XXX_GET_MEASURED_SAMPLE_RATE:
		*((uint32_t*)aIoctl_param1) = runtime_handle->actual_sample_rate;
		break;

	case I2S_I94XXX_SYNC_FS_TO_DPWM_FS_RATE:
		{
			uint32_t clock_div;
			uint32_t dpwm_zohdiv;
			uint32_t dpwm_clock_div;
			uint32_t dpwm_k;
			uint8_t channel_num = 2;
			uint32_t dpwm_total_div_to_get_FS;
			uint32_t i2s_total_div_to_get_FS;

			if ( 2 != channel_num)
			{
				CRITICAL_ERROR("only 2 channels supported");
			}
			dpwm_clock_div = DPWM_GET_CLOCKDIV(DPWM);
			dpwm_zohdiv = DPWM_GET_ZOHDIV(DPWM);
			dpwm_k = (DPWM->CTL & DPWM_CLKSET_500FS) ? 125 : 128;


			dpwm_total_div_to_get_FS =
					(dpwm_clock_div + 1) * dpwm_zohdiv * dpwm_k;
			clock_div = ( ( dpwm_total_div_to_get_FS ) /
					(2 * channel_num * num_of_bytes_in_word * 8) ) - 1;

			i2s_total_div_to_get_FS = (clock_div + 1) *
					(2 * channel_num * num_of_bytes_in_word * 8);

			if (dpwm_total_div_to_get_FS != i2s_total_div_to_get_FS)
			{
				CRITICAL_ERROR("same FS for I2S and DPWM cannot be achived");
			}
	        I2S0->CLKDIV = (I2S0->CLKDIV & ~I2S_CLKDIV_BCLKDIV_Msk) |
				(clock_div << I2S_CLKDIV_BCLKDIV_Pos);

		}
		break;

	default :
		return 1;
	}
	return 0;
}
