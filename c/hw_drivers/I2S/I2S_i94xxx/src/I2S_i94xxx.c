/*
 *
 * I2S_i94xxx.c
 *
 */

/********  includes *********************/

#include "src/_I2S_i94xxx_prerequirements_check.h"

#include "I2S_i94xxx_api.h"
#include "I2S_i94xxx.h"
#include "irq_api.h"
#include "timer_wrapper_api.h"


#include "ISD94XXXSeries.h"

#include "I2S_i94xxx_add_component.h"

#include "clock_control_i94xxx_api.h"
#include "i2s.h"

/********  defines *********************/


/********  types  *********************/
#define SYS_GPD_MFPL_PD0MFP_I2S0_BCLK   (0x04UL<<SYS_GPD_MFPL_PD0MFP_Pos) /*  */
#define SYS_GPD_MFPL_PD1MFP_I2S0_LRCLK  (0x04UL<<SYS_GPD_MFPL_PD1MFP_Pos) /*  */
#define SYS_GPD_MFPL_PD2MFP_I2S0_MCLK   (0x03UL<<SYS_GPD_MFPL_PD2MFP_Pos) /* */
#define SYS_GPD_MFPL_PD4MFP_I2S0_DI     (0x03UL<<SYS_GPD_MFPL_PD4MFP_Pos) /*  */
#define SYS_GPD_MFPL_PD5MFP_I2S0_DO     (0x03UL<<SYS_GPD_MFPL_PD5MFP_Pos) /*  */


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
//	uint8_t   	clock_mode;
	uint8_t		num_of_bytes_in_word;
	struct dev_desc_t	*clk_dev;
	struct dev_desc_t	*src_clock;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	src_clock = cfg_hndl->src_clock;


	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
#if 0
		/* BCLK:PD0[12.36], MCLK:PB15[13.61],
		 * LR:PD1[12.38], DO:PB14[23.97], DI:PB13[23.95] */
		SYS->GPB_MFPH = (SYS->GPB_MFPH & (~0xFFF00000)) | 0x22200000;//ok
		SYS->GPD_MFPL = (SYS->GPD_MFPL & (~0x0FF)) | 0x044;//ok
#else
        /*    MCLK   GPD2, BCLK  GPD0, FS GPD1  , DI  GPD4 , DO   GPD5 */
        SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD0MFP_Msk | SYS_GPD_MFPL_PD1MFP_Msk |
        		SYS_GPD_MFPL_PD2MFP_Msk | SYS_GPD_MFPL_PD4MFP_Msk |
        		SYS_GPD_MFPL_PD5MFP_Msk);
        SYS->GPD_MFPL |= (SYS_GPD_MFPL_PD0MFP_I2S0_BCLK |
        		SYS_GPD_MFPL_PD1MFP_I2S0_LRCLK | SYS_GPD_MFPL_PD2MFP_I2S0_MCLK |
        		SYS_GPD_MFPL_PD4MFP_I2S0_DI |  SYS_GPD_MFPL_PD5MFP_I2S0_DO);
#endif
		clk_dev = i94xxx_i2s_clk_dev;

		DEV_IOCTL_1_PARAMS(clk_dev,	CLK_IOCTL_SET_PARENT, src_clock);
		DEV_IOCTL_0_PARAMS(clk_dev, CLK_IOCTL_ENABLE);


	    /*
	     *  Master mode, 16-bit word width, stereo mode, I2S format.
	     *  Set TX and RX FIFO threshold to middle value.
	     */

	    num_of_bytes_in_word = cfg_hndl->num_of_bytes_in_word;
//	    num_of_bytes_in_word=1;

	    I2S_Open(I2S, cfg_hndl->clock_mode, cfg_hndl->sample_rate,
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

	case I2S_ENABLE_OUTPUT_IOCTL:
		I2S_ENABLE_TX(I2S);
		I2S->CTL0 |= I2S_CTL0_TXPDMAEN_Msk;
		break;

	default :
		return 1;
	}
	return 0;
}
