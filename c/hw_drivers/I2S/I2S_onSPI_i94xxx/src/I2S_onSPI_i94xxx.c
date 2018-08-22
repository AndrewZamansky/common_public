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


#include "I94100.h"

#include "I2S_onSPI_i94xxx_add_component.h"

#include "clock_control_i94xxx_api.h"

/********  defines *********************/

//Pin definitions not included in the I94xxx BSP v3.03
#define SYS_GPC_MFPL_PC0MFP_SPI1_MOSI           (0x03UL<<SYS_GPC_MFPL_PC0MFP_Pos) /* SPI1 MOSI (Master Out, Slave In) pin; or I2S1 data output pin. */
#define SYS_GPC_MFPL_PC1MFP_SPI1_MISO           (0x03UL<<SYS_GPC_MFPL_PC1MFP_Pos) /* SPI1 MISO (Master In, Slave Out) pin; or I2S1 data input pin. */
#define SYS_GPC_MFPL_PC2MFP_SPI1_CLK            (0x03UL<<SYS_GPC_MFPL_PC2MFP_Pos) /* SPI1 Serial Clock pin; or I2S1 bit clock pin. */
#define SYS_GPC_MFPL_PC3MFP_SPI1_SS             (0x03UL<<SYS_GPC_MFPL_PC3MFP_Pos) /* SPI1 slave select pin; or I2S1 left right channel clock pin. */
#define SYS_GPC_MFPL_PC4MFP_SPI1_I2SMCLK        (0x03UL<<SYS_GPC_MFPL_PC4MFP_Pos) /* SPI1 I2S master clock output pin. */


/********  types  *********************/


/* ---------------- External variables --------------------------*/

/* ---------------- External functions --------------------------*/

/* ---------------- Exported variables --------------------------*/

/* ---------------- Main variables ------------------------------*/


/* ---------------- Interrupt Routine ---------------------------*/

/*
 * Currently I2S_onSPI_i94xxx_callback() is a loopback function, TX = RX.
 * TODO: Make a data transfer interface.
 */

uint8_t I2S_onSPI_i94xxx_callback(struct dev_desc_t *adev ,
		uint8_t aCallback_num , void * aCallback_param1,
		void * aCallback_param2)
{
	struct I2S_onSPI_i94xxx_cfg_t *cfg_hndl;
	SPI_T	*I2S_SPI_module;

	static uint32_t u32RxValue[2];
	uint32_t audio_format, data_width;

	uint8_t i;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	I2S_SPI_module = (SPI_T*)cfg_hndl->base_address;
	audio_format = cfg_hndl->audio_format;
	data_width = cfg_hndl->data_width;


	SPI_I2S_CLR_INT_FLAG(I2S_SPI_module, SPI_I2S_TXTH_INT_MASK);
	if (SPI_I2SMONO == audio_format)
	{
		switch (data_width)
		{
		case SPI_I2SDATABIT_8 :
			/*  TODO: Not working.
			 * FIFO Data Seperation : MONO 8-bit
			 * |7  N+3  0|7  N+2  0|7  N+1  0|7   N   0|
			 */
			if(!SPI_I2S_IS_RX_EMPTY(I2S_SPI_module))
			{
				u32RxValue[0] = (uint8_t)SPI_I2S_READ_RX_FIFO(I2S_SPI_module);
			}
			SPI_I2S_WRITE_TX_FIFO(I2S_SPI_module, (uint8_t)u32RxValue[0]);

			break;

		case SPI_I2SDATABIT_16 :
			/*
			 * FIFO Data Seperation : MONO 16-bit
			 * |15       N+1       0|15       N       0|
			 */
			if(!SPI_I2S_IS_RX_EMPTY(I2S_SPI_module))
			{
				u32RxValue[0] = (uint16_t)SPI_I2S_READ_RX_FIFO(I2S_SPI_module);
			}
			SPI_I2S_WRITE_TX_FIFO(I2S_SPI_module, (uint16_t)u32RxValue[0]);

			break;

		case SPI_I2SDATABIT_24 | SPI_I2SDATABIT_32:
			/*
			 * FIFO Data Seperation : MONO 24-bit
			 * |              |23          N          0|
			 * FIFO Data Seperation : MONO 32-bit
			 * |31                 N                  0|
			 */
			if(!SPI_I2S_IS_RX_EMPTY(I2S_SPI_module))
			{
				u32RxValue[0] = SPI_I2S_READ_RX_FIFO(I2S_SPI_module);
			}
			SPI_I2S_WRITE_TX_FIFO(I2S_SPI_module, u32RxValue[0]);

			break;

		}
	}
	else if (SPI_I2SSTEREO == audio_format)
	{
		switch (data_width)
		{
		case SPI_I2SDATABIT_8 :
			/*
			 * FIFO Data Seperation : STEREO 8-bit
			 * |7  L+1  0|7  R+1  0|7   L   0|7   R   0|
			 */
			if(!SPI_I2S_IS_RX_EMPTY(I2S_SPI_module))
			{
				u32RxValue[0] = (uint16_t)SPI_I2S_READ_RX_FIFO(I2S_SPI_module);
			}
			SPI_I2S_WRITE_TX_FIFO(I2S_SPI_module, (uint16_t)u32RxValue[0]);

			break;

		case SPI_I2SDATABIT_16 :
			/*
			 * FIFO Data Seperation : STEREO 16-bit
			 * |15       L        0|15       R        0|
			 */
			if(!SPI_I2S_IS_RX_EMPTY(I2S_SPI_module))
			{
				u32RxValue[0] = SPI_I2S_READ_RX_FIFO(I2S_SPI_module);
			}
			SPI_I2S_WRITE_TX_FIFO(I2S_SPI_module, u32RxValue[0]);
			break;

		case SPI_I2SDATABIT_24 | SPI_I2SDATABIT_32 :
			/*
			 * FIFO Data Seperation : STEREO 24-bit
			 * |              |23          L          0|
			 * |              |23          R          0|
			 * FIFO Data Seperation : STEREO 32-bit
			 * |31                 L                  0|
			 * |31                 R                  0|
			 */
			for(i = 0; i < 2; i++)
			{
				if(!SPI_I2S_IS_RX_EMPTY(I2S_SPI_module))
				{
					u32RxValue[i] = SPI_I2S_READ_RX_FIFO(I2S_SPI_module);
				}
				SPI_I2S_WRITE_TX_FIFO(I2S_SPI_module, u32RxValue[i]);
			}

			break;

		}
	}
	else {
		CRITICAL_ERROR("Audio Format not set in Device Tree.")
	}

	return 1;
}

/*
 * configure_i2s_spi_pinout(uint8_t spi_bus) :
 *
 * Set Pinouts for I2S over SPI bus. Refer to defines for fuller mask/pos
 * 		Input = 1~4 numbered through defines in I2S_onSPI_i94XXX_api.h
 * 				and listed through SPI1_PD/PC and SPI2PA/PC
 * 		Output = None
 */

static void configure_i2s_spi_pinout(struct dev_desc_t *adev)
{
	struct I2S_onSPI_i94xxx_cfg_t *cfg_hndl;
	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);

	switch(cfg_hndl->base_address)
	{
	case I2S_onSPI_I94XXX_API_BASE_ADDRESS_SPI1 :
		if(cfg_hndl->BCLK_pin == I2S_onSPI_I94XXX_API_BCLK_PIN_D4)
		{
			SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD4MFP_Msk);
			SYS->GPD_MFPL |= SYS_GPD_MFPL_PD4MFP_SPI1_CLK;

			PD->SMTEN = GPIO_SMTEN_SMTEN6_Msk;
		}

		if(cfg_hndl->LRCLK_pin == I2S_onSPI_I94XXX_API_LRCLK_PIN_D5)
		{
			SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD5MFP_Msk);
			SYS->GPD_MFPL |= SYS_GPD_MFPL_PD5MFP_SPI1_SS;

			PD->SMTEN |= GPIO_SMTEN_SMTEN3_Msk;
		}

		if(cfg_hndl->DI_pin == I2S_onSPI_I94XXX_API_DI_PIN_D3)
		{
			SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD3MFP_Msk);
			SYS->GPD_MFPL |= SYS_GPD_MFPL_PD3MFP_SPI1_MISO;

			PD->SMTEN |= GPIO_SMTEN_SMTEN4_Msk;
		}

		if(cfg_hndl->DO_pin == I2S_onSPI_I94XXX_API_DO_PIN_D2)
		{
			SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD2MFP_Msk);
			SYS->GPD_MFPL |= SYS_GPD_MFPL_PD2MFP_SPI1_MOSI;

			PD->SMTEN |= GPIO_SMTEN_SMTEN5_Msk;
		}

		if(cfg_hndl->MCLK_pin == I2S_onSPI_I94XXX_API_MCLK_PIN_D6)
		{
			SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD6MFP_Msk);
			SYS->GPD_MFPL |= SYS_GPD_MFPL_PD6MFP_SPI1_I2SMCLK;

			PD->SMTEN |= GPIO_SMTEN_SMTEN2_Msk;
		}

		if(cfg_hndl->BCLK_pin == I2S_onSPI_I94XXX_API_BCLK_PIN_C2)
		{
			SYS->GPC_MFPL &= ~(SYS_GPC_MFPL_PC2MFP_Msk);
			SYS->GPC_MFPL |= SYS_GPC_MFPL_PC2MFP_SPI1_CLK;

			PC->SMTEN |= GPIO_SMTEN_SMTEN2_Msk;
		}

		if(cfg_hndl->LRCLK_pin == I2S_onSPI_I94XXX_API_LRCLK_PIN_C3)
		{
			SYS->GPC_MFPL &= ~(SYS_GPC_MFPL_PC3MFP_Msk);
			SYS->GPC_MFPL |= SYS_GPC_MFPL_PC3MFP_SPI1_SS;

			PC->SMTEN = GPIO_SMTEN_SMTEN3_Msk;
		}

		if(cfg_hndl->DI_pin == I2S_onSPI_I94XXX_API_DI_PIN_C1)
		{
			SYS->GPC_MFPL &= ~(SYS_GPC_MFPL_PC1MFP_Msk);
			SYS->GPC_MFPL |= SYS_GPC_MFPL_PC1MFP_SPI1_MISO;

			PC->SMTEN |= GPIO_SMTEN_SMTEN1_Msk;
		}

		if(cfg_hndl->DO_pin == I2S_onSPI_I94XXX_API_DO_PIN_C0)
		{
			SYS->GPC_MFPL &= ~(SYS_GPC_MFPL_PC0MFP_Msk);
			SYS->GPC_MFPL |= SYS_GPC_MFPL_PC0MFP_SPI1_MOSI;

			PC->SMTEN = GPIO_SMTEN_SMTEN0_Msk;
		}

		if(cfg_hndl->MCLK_pin == I2S_onSPI_I94XXX_API_MCLK_PIN_C4)
		{
			SYS->GPC_MFPL &= ~(SYS_GPC_MFPL_PC4MFP_Msk);
			SYS->GPC_MFPL |= SYS_GPC_MFPL_PC4MFP_SPI1_I2SMCLK;

			PC->SMTEN |= GPIO_SMTEN_SMTEN4_Msk;
		}
		break;
	case I2S_onSPI_I94XXX_API_BASE_ADDRESS_SPI2 :
		CRITICAL_ERROR("SPI2 pinouts not defined yet");
		break;

	default:
		CRITICAL_ERROR("SPI base address needs to be defined");
	}
}


/* ---------------- I/O Control (IOCTL) Function ----------------*/

/**
 * I2S_onSPI_i94xxx_ioctl()
 *
 * return:
 */
uint8_t I2S_onSPI_i94xxx_ioctl( struct dev_desc_t *adev,
			const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct I2S_onSPI_i94xxx_cfg_t *cfg_hndl;
	SPI_T	*I2S_SPI_module;
	struct dev_desc_t	*clk_dev;
	struct dev_desc_t	*src_clock;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	I2S_SPI_module = (SPI_T*)cfg_hndl->base_address;
	src_clock = cfg_hndl->src_clock;
	uint32_t clk_mode;
	uint32_t sample_rate;
	uint32_t data_width;
	uint32_t audio_format;
	uint32_t txrx_format;

	clk_mode = cfg_hndl->clk_mode;
	sample_rate = cfg_hndl->sample_rate;
	data_width = cfg_hndl->data_width;
	audio_format = cfg_hndl->audio_format;
	txrx_format = cfg_hndl->txrx_format;

	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		if ((SPI_T*)SPI1_BASE == I2S_SPI_module) 
		{
			clk_dev = i94xxx_spi1clk_clk_dev;
		}
		else if ((SPI_T*)SPI2_BASE == I2S_SPI_module) 
		{
			clk_dev = i94xxx_spi2clk_clk_dev;
		}
		else
		{
			CRITICAL_ERROR("SPI Base Address not defined.");
		}

		configure_i2s_spi_pinout(adev);

		DEV_IOCTL_1_PARAMS(clk_dev,	CLK_IOCTL_SET_PARENT, src_clock);
		DEV_IOCTL_0_PARAMS(clk_dev, CLK_IOCTL_ENABLE);

		SPI_I2SOpen(I2S_SPI_module, clk_mode, sample_rate, data_width,
											audio_format, txrx_format);

		if(0 != cfg_hndl->MCLK_pin)
		{
			SPI_I2SEnableMCLK(I2S_SPI_module, sample_rate * 256 );
		}

		SPI_I2S_SET_RXTH(SPI1, SPI_I2S_FIFO_RX_LEVEL_4);
		SPI_I2S_SET_TXTH(SPI1, SPI_I2S_FIFO_TX_LEVEL_4);

		SPI_I2S_RST_TX_FIFO(I2S_SPI_module);
		SPI_I2S_RST_RX_FIFO(I2S_SPI_module);


		/*
		 * Commented lines activate the interrupt which follows the callback
		 *   function. Currently using the DMA so interrupt is not needed.
		 *   Please refer to device tree for current set up to comment or
		 *   uncomment these lines.
		 */
//		if (SPI1_BASE == (uint32_t)I2S_SPI_module)
//		{
//			i2s_spi_irq = SPI1_IRQn;
//		}
//		else if (SPI2_BASE == (uint32_t)I2S_SPI_module)
//		{
//			i2s_spi_irq = SPI2_IRQn;
//		}
//		else
//		{
//			return 1;
//		}
//		SPI_I2SEnableInt(I2S_SPI_module, SPI_I2S_RXTH_INT_MASK);
//		irq_register_device_on_interrupt(i2s_spi_irq , adev);
//		irq_set_priority(i2s_spi_irq , OS_MAX_INTERRUPT_PRIORITY_FOR_API_CALLS );
//		irq_enable_interrupt(i2s_spi_irq);

		SPI_I2SEnableControl(I2S_SPI_module);

		break;

	case SPI_I2S_ENABLE_INPUT_IOCTL:
		SPI_I2S_ENABLE_RX(I2S_SPI_module);
	    SPI_I2S_ENABLE_RXDMA(I2S_SPI_module);
		break;

	case SPI_I2S_DISABLE_INPUT_IOCTL:
		SPI_I2S_DISABLE_RX(I2S_SPI_module);
		SPI_I2S_DISABLE_RXDMA(I2S_SPI_module);
		break;

	case SPI_I2S_ENABLE_OUTPUT_IOCTL:
		SPI_I2S_ENABLE_TX(I2S_SPI_module);
	    SPI_I2S_ENABLE_TXDMA(I2S_SPI_module);
		break;

	case SPI_I2S_DISABLE_OUTPUT_IOCTL:
		SPI_I2S_DISABLE_TX(I2S_SPI_module);
		SPI_I2S_DISABLE_TXDMA(I2S_SPI_module);
		break;

	default :
		return 1;
	}
	return 0;
}
