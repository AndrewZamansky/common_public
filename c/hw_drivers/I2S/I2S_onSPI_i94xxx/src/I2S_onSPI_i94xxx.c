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

#define SYS_GPC_MFPL_PC0MFP_SPI1_MOSI           (0x03UL<<SYS_GPC_MFPL_PC0MFP_Pos) /* SPI1 MOSI (Master Out, Slave In) pin; or I2S1 data output pin. */
#define SYS_GPC_MFPL_PC1MFP_SPI1_MISO           (0x03UL<<SYS_GPC_MFPL_PC1MFP_Pos) /* SPI1 MISO (Master In, Slave Out) pin; or I2S1 data input pin. */
#define SYS_GPC_MFPL_PC2MFP_SPI1_CLK            (0x03UL<<SYS_GPC_MFPL_PC2MFP_Pos) /* SPI1 Serial Clock pin; or I2S1 bit clock pin. */
#define SYS_GPC_MFPL_PC3MFP_SPI1_SS             (0x03UL<<SYS_GPC_MFPL_PC3MFP_Pos) /* SPI1 slave select pin; or I2S1 left right channel clock pin. */
#define SYS_GPC_MFPL_PC4MFP_SPI1_I2SMCLK        (0x03UL<<SYS_GPC_MFPL_PC4MFP_Pos) /* SPI1 I2S master clock output pin. */


#define SPI1_PD_MSK     SYS_GPD_MFPL_PD2MFP_Msk  | \
                        SYS_GPD_MFPL_PD3MFP_Msk  | \
                        SYS_GPD_MFPL_PD4MFP_Msk  | \
                        SYS_GPD_MFPL_PD5MFP_Msk  | \
                        SYS_GPD_MFPL_PD6MFP_Msk

#define SPI1_PD_POS     SYS_GPD_MFPL_PD2MFP_SPI1_MOSI     | \
                        SYS_GPD_MFPL_PD3MFP_SPI1_MISO     | \
                        SYS_GPD_MFPL_PD4MFP_SPI1_CLK      | \
                        SYS_GPD_MFPL_PD5MFP_SPI1_SS       | \
                        SYS_GPD_MFPL_PD6MFP_SPI1_I2SMCLK


#define SPI1_PC_MSK     SYS_GPC_MFPL_PC0MFP_Msk  | \
                        SYS_GPC_MFPL_PC1MFP_Msk  | \
                        SYS_GPC_MFPL_PC2MFP_Msk  | \
                        SYS_GPC_MFPL_PC3MFP_Msk  | \
                        SYS_GPC_MFPL_PC4MFP_Msk

#define SPI1_PC_POS     SYS_GPC_MFPL_PC0MFP_SPI1_MOSI    | \
						SYS_GPC_MFPL_PC1MFP_SPI1_MISO    | \
						SYS_GPC_MFPL_PC2MFP_SPI1_CLK     | \
						SYS_GPC_MFPL_PC3MFP_SPI1_SS      | \
						SYS_GPC_MFPL_PC4MFP_SPI1_I2SMCLK

#define SPI2_PA_MSK

#define SPI2_PA_POS

#define SPI2_PC_MSK

#define SPI2_PC_POS

/********  types  *********************/


/* ---------------- External variables --------------------------*/

/* ---------------- External functions --------------------------*/

/* ---------------- Exported variables --------------------------*/

/* ---------------- Main variables ------------------------------*/
#define TEST_COUNT	128

static volatile int g_u32DataCount = 0;

/* ---------------- Interrupt Routine ---------------------------*/

/*
 * Currently I2S_onSPI_i94xxx_callback() is a loopback function, TX = RX.
 * TODO: Make a data transfer interface.
 */

static uint32_t u32RxValue[2];

uint8_t I2S_onSPI_i94xxx_callback(struct dev_desc_t *adev ,
		uint8_t aCallback_num , void * aCallback_param1,
		void * aCallback_param2)
{
	struct I2S_onSPI_i94xxx_cfg_t *cfg_hndl;
	SPI_T	*I2S_SPI_module;

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

static void configure_i2s_spi_pinout(uint8_t spi_bus)
{
	switch(spi_bus)
	{
	case SPI1_PD :
		//Reset and configure pins
		SYS->GPD_MFPL &= ~SPI1_PD_MSK;
		SYS->GPD_MFPL |= SPI1_PD_POS;

		//Enable schmitt triggering for signal integrity when using both
		//  input and output. (Problem with i94124 chip)
		PD->SMTEN = GPIO_SMTEN_SMTEN2_Msk |
					GPIO_SMTEN_SMTEN3_Msk |
					GPIO_SMTEN_SMTEN4_Msk |
					GPIO_SMTEN_SMTEN5_Msk |
					GPIO_SMTEN_SMTEN6_Msk;
		break;

	case SPI1_PC :
		//Reset and configure pins
		SYS->GPC_MFPL &= ~SPI1_PC_MSK;
		SYS->GPC_MFPL |= SPI1_PC_POS;

		//Enable schmitt triggering for signal integrity when using both
		//  input and output. (Problem with i94124 chip)
		PC->SMTEN = GPIO_SMTEN_SMTEN0_Msk |
					GPIO_SMTEN_SMTEN1_Msk |
					GPIO_SMTEN_SMTEN2_Msk |
					GPIO_SMTEN_SMTEN3_Msk |
					GPIO_SMTEN_SMTEN4_Msk;
		break;
	case SPI2_PA :
		break;
	case SPI2_PC :
		break;
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


	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		if ((SPI_T*)SPI1_BASE == I2S_SPI_module) clk_dev = i94xxx_spi1clk_clk_dev;
		else if ((SPI_T*)SPI2_BASE == I2S_SPI_module) clk_dev = i94xxx_spi2clk_clk_dev;

		configure_i2s_spi_pinout(cfg_hndl->spi_bus);

		DEV_IOCTL_1_PARAMS(clk_dev,	CLK_IOCTL_SET_PARENT, src_clock);
		DEV_IOCTL_0_PARAMS(clk_dev, CLK_IOCTL_ENABLE);

		SPI_I2SOpen(I2S_SPI_module,
					cfg_hndl->clk_mode,
					cfg_hndl->sample_rate,
					cfg_hndl->data_width,
					cfg_hndl->audio_format,
					cfg_hndl->txrx_format);

		//Interrupt based on receive buffer.

		SPI_I2S_SET_RXTH(SPI1, SPI_I2S_FIFO_RX_LEVEL_4);
		SPI_I2S_SET_TXTH(SPI1, SPI_I2S_FIFO_TX_LEVEL_1);

		SPI_I2S_RST_TX_FIFO(I2S_SPI_module);
		SPI_I2S_RST_RX_FIFO(I2S_SPI_module);

//		SPI_I2SEnableInt(I2S_SPI_module, SPI_I2S_RXTH_INT_MASK);
//
//		if (SPI1_BASE == I2S_SPI_module) i2s_spi_irq = SPI1_IRQn;
//		else if (SPI2_BASE == I2S_SPI_module) i2s_spi_irq = SPI2_IRQn;
//
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
