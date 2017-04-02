/*
 *
 * I2S_onSPI_i94xxx.c
 *
 */

/********  includes *********************/

#include "src/_I2S_onSPI_i94xxx_prerequirements_check.h"

#include "I2S_onSPI_i94xxx_api.h"
#include "I2S_onSPI_i94xxx.h"
#include "irq_api.h"
#include "timer_wrapper_api.h"


#include "ISD94XXXSeries.h"

#include "I2S_onSPI_i94xxx_add_component.h"

#include "clock_control_i94xxx_api.h"

/********  defines *********************/


/********  types  *********************/


/* ------------- External variables --------------------------*/

/* ------------------------ External functions --------------*/

/* ------------------------ Exported variables ---------------*/

volatile int g_u32DataCount = 0;
volatile int status = 0;
#define TEST_COUNT	100
int data[TEST_COUNT+1] = {0};
volatile int pos = 0;
void SPI_IRQHandler()
{
    /* Write 2 TX values to TX FIFO */
  //  I2S_WRITE_TX_FIFO(SPI1, g_u32TxValue);
 //   I2S_WRITE_TX_FIFO(SPI1, g_u32TxValue);
    if((SPI2->I2SSTS & SPI_I2SSTS_RXEMPTY_Msk) == 0)
    {
		status = ((SPI_T *)SPI2_BASE)->I2SSTS;
		if (pos < TEST_COUNT)
		{
			pos++;
		}
		data[pos] = ((SPI_T *)SPI2_BASE)->RX;
    }
    g_u32DataCount += 2;
}

/**
 * I2S_onSPI_i94xxx_ioctl()
 *
 * return:
 */
uint8_t I2S_onSPI_i94xxx_ioctl( pdev_descriptor_t apdev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	struct I2S_onSPI_i94xxx_cfg_t *cfg_hndl;
	SPI_T	*I2S_module;
//	uint8_t   	clock_mode;
	uint8_t		num_of_bytes_in_word;
	int spi_irq;
	struct dev_desc_t	*clk_dev;
	struct dev_desc_t	*src_clock;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(apdev);
	I2S_module = (SPI_T*)cfg_hndl->base_address;
	src_clock = cfg_hndl->src_clock;


	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		if (SPI1_BASE == (size_t)I2S_module)
		{
			/* Configure SPI1 related multi-function pins.
			 * GPD[2:6] : SPI1_CLK (I2S1_BCLK), SPI1_MISO (I2S1_DI), SPI1_MOSI
			 *  (I2S1_DO), SPI1_SS (I2S1_LRCLK).
			 */
			while(1); //TODO
			SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD2MFP_Msk |
					SYS_GPD_MFPL_PD3MFP_Msk | SYS_GPD_MFPL_PD4MFP_Msk |
					SYS_GPD_MFPL_PD5MFP_Msk | SYS_GPD_MFPL_PD6MFP_Msk);
			SYS->GPD_MFPL |= (SYS_GPD_MFPL_PD2MFP_SPI1_MOSI |
					SYS_GPD_MFPL_PD3MFP_SPI1_MISO |
					SYS_GPD_MFPL_PD4MFP_SPI1_CLK |
					SYS_GPD_MFPL_PD5MFP_SPI1_SS |
					SYS_GPD_MFPL_PD6MFP_SPI1_I2SMCLK );
			spi_irq = SPI1_INT;
		}
		else if (SPI2_BASE == (size_t)I2S_module)
		{
#if 0
			/* Configure SPI2 related multi-function pins.
			 * GPC[5:9] : SPI2_CLK (I2S2_BCLK), SPI2_MISO (I2S2_DI), SPI2_MOSI
			 *  (I2S2_DO), SPI2_SS (I2S2_LRCLK).
			 */
			SYS->GPC_MFPL &= ~(SYS_GPC_MFPL_PC5MFP_Msk |
					SYS_GPC_MFPL_PC6MFP_Msk | SYS_GPC_MFPL_PC7MFP_Msk |
					SYS_GPC_MFPH_PC8MFP_Msk | SYS_GPC_MFPH_PC9MFP_Msk);
			SYS->GPC_MFPH |= (SYS_GPC_MFPL_PC5MFP_SPI2_MOSI |
					SYS_GPC_MFPL_PC6MFP_SPI2_MISO |
					SYS_GPC_MFPL_PC7MFP_SPI2_CLK |
					SYS_GPC_MFPH_PC8MFP_SPI2_SS |
					SYS_GPC_MFPH_PC9MFP_SPI2_I2SMCLK );
#else
		    SYS->IPRST1 |= 0x00007000;
		    SYS->IPRST1 &= (~0x00007000);
			/* GPA[7] : SPI2_MISO */
			/* GPA[12] : SPI2_I2SMCLK,GPA[10:8] : SPI2_BCLK,SPI2_SS, SPI2_MOSI*/
		      SYS->GPA_MFPL = (SYS->GPA_MFPL & (~0xF0000000)) | 0x40000000;
					SYS->GPA_MFPH = (SYS->GPA_MFPH & (~0xF0FFF)) | 0x40444;
#endif
			spi_irq = SPI2_INT;
			clk_dev = i94xxx_spi2clk_clk_dev;
		}
		else
		{
			return 1;
		}
		DEV_IOCTL_1_PARAMS(clk_dev,	CLK_IOCTL_SET_PARENT, src_clock);
		DEV_IOCTL_0_PARAMS(clk_dev, CLK_IOCTL_ENABLE);


	    /*
	     *  Master mode, 16-bit word width, stereo mode, I2S format.
	     *  Set TX and RX FIFO threshold to middle value.
	     */

	    num_of_bytes_in_word = cfg_hndl->num_of_bytes_in_word;
//	    num_of_bytes_in_word=1;

		I2S_module->FIFOCTL = I2S_FIFO_RX_LEVEL_WORD_3;
#if 1
	    I2S_Open(I2S_module, cfg_hndl->clock_mode, cfg_hndl->sample_rate,
	    		(num_of_bytes_in_word-1)<<SPI_I2SCTL_WDWIDTH_Pos,
				I2S_STEREO, I2S_FORMAT_I2S);
#else

	   //  I2S_module->I2SCLK = 0x1f00;
	     I2S_module->I2SCLK = 0x10006;
//	    I2S_module->I2SCTL = (num_of_bytes_in_word-1)<<SPI_I2SCTL_WDWIDTH_Pos |
	//    		SPI_I2SCTL_I2SEN_Msk;
	    I2S_module->I2SCTL = SPI_I2SCTL_I2SEN_Msk;
#endif
//	I2S_CLR_INT_FLAG(I2S_module, I2S_STATUS_LZCIF_Msk | I2S_STATUS_RZCIF_Msk |
//				I2S_STATUS_TXOVIF_Msk | I2S_STATUS_TXUDIF_Msk |
//				I2S_STATUS_RXOVIF_Msk | I2S_STATUS_RXUDIF_Msk |
//				I2S_STATUS_TDMATIF_Msk | I2S_STATUS_TDMAEIF_Msk |
//				I2S_STATUS_RDMATIF_Msk|I2S_STATUS_RDMAEIF_Msk);

#define 	 USE_INTERRUPT
#ifdef USE_INTERRUPT
		irq_register_interrupt(spi_irq , SPI_IRQHandler);
		irq_set_priority(spi_irq , OS_MAX_INTERRUPT_PRIORITY_FOR_API_CALLS );
		irq_enable_interrupt(spi_irq);
	     I2S_EnableInt(I2S_module, 	I2S_FIFO_RXTH_INT_MASK);

#else
		I2S_module->PDMACTL |= SPI_PDMACTL_PDMARST_Msk;
	    SPI_TRIGGER_RX_PDMA(I2S_module) ;
#endif
	   // I2S_ENABLE_RX(I2S_module);
		I2S_module->FIFOCTL |= I2S_FIFO_RX_LEVEL_WORD_4;
	    I2S_module->FIFOCTL |= SPI_FIFOCTL_RXRST_Msk;
		break;

	case I2S_ENABLE_OUTPUT_IOCTL:
		I2S_ENABLE_TX(I2S_module);
	    SPI_TRIGGER_TX_PDMA(I2S_module) ;
		break;

	default :
		return 1;
	}
	return 0;
}
