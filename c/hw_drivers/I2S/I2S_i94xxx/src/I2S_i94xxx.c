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

#include "ISD94XXXSeries.h"

#include "i2s.h"
#include "clock_control_i94xxx_api.h"

#include "_I2S_i94xxx_prerequirements_check.h"

/*following line add module to available module list for dynamic device tree*/
#include "I2S_i94xxx_add_component.h"

/********  defines *********************/


/********  types  *********************/
#define SYS_GPD_MFPL_PD0MFP_I2S0_BCLK   (0x04UL<<SYS_GPD_MFPL_PD0MFP_Pos) /*  */
#define SYS_GPD_MFPL_PD1MFP_I2S0_LRCLK  (0x04UL<<SYS_GPD_MFPL_PD1MFP_Pos) /*  */
#define SYS_GPD_MFPL_PD2MFP_I2S0_MCLK   (0x03UL<<SYS_GPD_MFPL_PD2MFP_Pos) /* */
#define SYS_GPD_MFPL_PD4MFP_I2S0_DI     (0x03UL<<SYS_GPD_MFPL_PD4MFP_Pos) /*  */
#define SYS_GPD_MFPL_PD5MFP_I2S0_DO     (0x03UL<<SYS_GPD_MFPL_PD5MFP_Pos) /*  */

#define SYS_GPB_MFPH_PB15MFP_I2S0_MCLK   (0x02UL<<SYS_GPB_MFPH_PB15MFP_Msk)/* */
#define SYS_GPB_MFPH_PB13MFP_I2S0_DI     (0x02UL<<SYS_GPB_MFPH_PB13MFP_Msk)/* */
#define SYS_GPB_MFPH_PB14MFP_I2S0_DO     (0x02UL<<SYS_GPB_MFPH_PB14MFP_Msk)/* */


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
    if((I2S->STATUS0 & I2S_STATUS0_RXEMPTY_Msk) == 0)
    {
		status = I2S->STATUS0;
		if (pos < TEST_COUNT)
		{
			pos++;
		}
		data[pos] = I2S->RXFIFO;
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

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);
	src_clock = cfg_hndl->src_clock;


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


        /*    BCLK  GPD0, FS(LR) GPD1  */
        SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD0MFP_Msk | SYS_GPD_MFPL_PD1MFP_Msk);
        SYS->GPD_MFPL |= (SYS_GPD_MFPL_PD0MFP_I2S0_BCLK |
        		SYS_GPD_MFPL_PD1MFP_I2S0_LRCLK);

		clk_dev = i94xxx_i2s_clk_dev;

		DEV_IOCTL_1_PARAMS(clk_dev,	CLK_IOCTL_SET_PARENT, src_clock);
		DEV_IOCTL_0_PARAMS(clk_dev, CLK_IOCTL_ENABLE);


	    /*
	     *  Master mode, 16-bit word width, stereo mode, I2S format.
	     *  Set TX and RX FIFO threshold to middle value.
	     */

	    num_of_bytes_in_word = cfg_hndl->num_of_bytes_in_word;
//	    num_of_bytes_in_word=1;

	    runtime_handle->actual_sample_rate = I2S_Open(
	    		I2S, cfg_hndl->clock_mode, cfg_hndl->sample_rate,
	    		(num_of_bytes_in_word-1)<<SPI_I2SCTL_WDWIDTH_Pos,
				I2S_STEREO, I2S_FORMAT_I2S_STD);

		I2S_DISABLE_TX(I2S);

#ifdef DEBUG_USE_INTERRUPT
		irq_register_interrupt(I2S0_IRQn , I2S_IRQHandler);
		irq_set_priority(I2S0_IRQn , OS_MAX_INTERRUPT_PRIORITY_FOR_API_CALLS );
		irq_enable_interrupt(I2S0_IRQn);
		I2S_EnableInt(I2S, I2S_IEN_RXTHIEN_Msk);

#else
		I2S->CTL0 |= I2S_CTL0_RXPDMAEN_Msk;
#endif
	   // I2S_ENABLE_RX(I2S_module);
//		I2S_module->FIFOCTL |= I2S_FIFO_RX_LEVEL_WORD_4;
//	    I2S_module->FIFOCTL |= SPI_FIFOCTL_RXRST_Msk;
		break;

	case I2S_I94XXX_ENABLE_OUTPUT_IOCTL:
		I2S_ENABLE_TX(I2S);
		I2S->CTL0 |= I2S_CTL0_TXPDMAEN_Msk;
		break;

	case I2S_I94XXX_GET_MEASURED_SAMPLE_RATE:
		*((uint32_t*)aIoctl_param1) = runtime_handle->actual_sample_rate;
		break;

	default :
		return 1;
	}
	return 0;
}
