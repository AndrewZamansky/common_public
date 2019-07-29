/*
 *
 * I2S_onSPI_i94xxx.c
 *
 */

/********  includes *********************/
#include "dev_management_api.h" // for device manager defines and typedefs

#include "I2S_onSPI_i94xxx_api.h"
#include "I2S_onSPI_i94xxx.h"
#include "irq_api.h"

#include "timer_wrapper_api.h"


#include "I94100.h"

#include "clock_control_i94xxx_api.h"
#include "pin_control_api.h"
#ifdef CONFIG_INCLUDE_INTERNAL_PWM
	#include "dpwm_i94xxx_api.h"
#endif

#include "I2S_onSPI_i94xxx_add_component.h"

/********  defines *********************/

//#define USE_SPI_I2S_INTERRUPT

#ifdef  USE_SPI_I2S_INTERRUPT
	#if !defined(INTERRUPT_PRIORITY_FOR_I2S)
		#error "INTERRUPT_PRIORITY_FOR_I2S should be defined"
	#endif

	#if CHECK_INTERRUPT_PRIO_FOR_OS_SYSCALLS(INTERRUPT_PRIORITY_FOR_I2S)
		#error "priority should be lower then maximal priority for os syscalls"
	#endif
#endif



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
#if defined(USE_SPI_I2S_INTERRUPT)
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
#endif

	return 1;
}


/*
 * configure_i2s_spi_pinout :
 *
 */
static void configure_i2s_spi_pinout(struct I2S_onSPI_i94xxx_cfg_t *cfg_hndl)
{
	pin_control_api_set_pin_function(cfg_hndl->BCLK_pin);
	pin_control_api_enable_input_schmitt_trigger(cfg_hndl->BCLK_pin);
	pin_control_api_set_pin_function(cfg_hndl->LRCLK_pin);
	pin_control_api_enable_input_schmitt_trigger(cfg_hndl->LRCLK_pin);
	pin_control_api_set_pin_function(cfg_hndl->DI_pin);
	pin_control_api_enable_input_schmitt_trigger(cfg_hndl->DI_pin);
	pin_control_api_set_pin_function(cfg_hndl->DO_pin);
	pin_control_api_enable_input_schmitt_trigger(cfg_hndl->DO_pin);
	if (0xffffffff != cfg_hndl->MCLK_pin)
	{
		pin_control_api_set_pin_function(cfg_hndl->MCLK_pin);
		pin_control_api_enable_input_schmitt_trigger(cfg_hndl->MCLK_pin);
	}
}


static void set_clocks(struct I2S_onSPI_i94xxx_cfg_t *cfg_hndl,
		SPI_T  *I2S_SPI_module, struct dev_desc_t *spi_i2s_clk_dev)
{
	uint32_t  src_clk_freq;
	uint32_t  mclk_freq;
	uint32_t  res;
	uint16_t  Mclock_factor_based_on_FSclock;

	DEV_IOCTL_1_PARAMS(spi_i2s_clk_dev, CLK_IOCTL_GET_FREQ, &src_clk_freq);
	Mclock_factor_based_on_FSclock = cfg_hndl->Mclock_factor_based_on_FSclock;
	mclk_freq = cfg_hndl->sample_rate * Mclock_factor_based_on_FSclock;

	res = (src_clk_freq / 2) % mclk_freq;
	if ((src_clk_freq < mclk_freq) || ((src_clk_freq != mclk_freq) && (res)))
	{
		CRITICAL_ERROR("cannot create desired M clock");
	}

	SPI_I2SEnableMCLK(I2S_SPI_module, mclk_freq);
}


static void i94xxx_I2S_onSPI_init(struct dev_desc_t *adev,
								struct I2S_onSPI_i94xxx_cfg_t *cfg_hndl)
{
	SPI_T  *I2S_SPI_module;
	struct dev_desc_t  *clk_dev;
	struct dev_desc_t  *src_clock;
	uint32_t clk_mode;
	uint32_t sample_rate;
	uint32_t data_width;
	uint32_t audio_format;
	uint32_t txrx_format;

	I2S_SPI_module = (SPI_T*)cfg_hndl->base_address;
	src_clock = cfg_hndl->src_clock;

	clk_mode = cfg_hndl->clk_mode;
	sample_rate = cfg_hndl->sample_rate;
	data_width = cfg_hndl->data_width;
	audio_format = cfg_hndl->audio_format;
	txrx_format = cfg_hndl->txrx_format;

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

	configure_i2s_spi_pinout(cfg_hndl);

	DEV_IOCTL_1_PARAMS(clk_dev, CLK_IOCTL_SET_PARENT, src_clock);
	DEV_IOCTL_0_PARAMS(clk_dev, CLK_IOCTL_ENABLE);

	SPI_I2SOpen(I2S_SPI_module, clk_mode, sample_rate, data_width,
										audio_format, txrx_format);

	set_clocks(cfg_hndl, I2S_SPI_module, clk_dev);


	SPI_I2S_SET_RXTH(I2S_SPI_module, SPI_I2S_FIFO_RX_LEVEL_4);
	SPI_I2S_SET_TXTH(I2S_SPI_module, SPI_I2S_FIFO_TX_LEVEL_4);

	SPI_I2S_RST_TX_FIFO(I2S_SPI_module);
	SPI_I2S_RST_RX_FIFO(I2S_SPI_module);
	I2S_SPI_module->PDMACTL =  SPI_PDMACTL_PDMARST_Msk;


	/*
	 * Commented lines activate the interrupt which follows the callback
	 *   function. Currently using the DMA so interrupt is not needed.
	 *   Please refer to device tree for current set up to comment or
	 *   uncomment these lines.
	 */
#if defined(USE_SPI_I2S_INTERRUPT)
	{
		int i2s_spi_irq;

		if (SPI1_BASE == (uint32_t)I2S_SPI_module)
		{
			i2s_spi_irq = SPI1_IRQn;
		}
		else if (SPI2_BASE == (uint32_t)I2S_SPI_module)
		{
			i2s_spi_irq = SPI2_IRQn;
		}
		else
		{
			return ;
		}
		SPI_I2SEnableInt(I2S_SPI_module, SPI_I2S_RXTH_INT_MASK);
		irq_register_device_on_interrupt(i2s_spi_irq , adev);
		irq_set_priority(i2s_spi_irq , INTERRUPT_PRIORITY_FOR_I2S );
		irq_enable_interrupt(i2s_spi_irq);
	}
#endif
	if (cfg_hndl->do_reordering_for_16or8bit_channels)
	{
		SPI_I2S_SET_STEREOORDER(I2S_SPI_module, SPI_I2SORDER_LOW);
	}
	else
	{
		SPI_I2S_SET_STEREOORDER(I2S_SPI_module, SPI_I2SORDER_HIGH);
	}

	SPI_I2SEnableControl(I2S_SPI_module);
}


static void i94xxx_sync_to_dpwm_fs_rate(struct I2S_onSPI_i94xxx_cfg_t *cfg_hndl,
		struct dev_desc_t *dpwm_dev)
{
#ifdef CONFIG_INCLUDE_INTERNAL_PWM
	struct dev_desc_t *dpwm_root_clk_dev;
	struct dev_desc_t *i2s_root_clk_dev;
	SPI_T  *I2S_SPI_module;
	uint32_t dpwm_sample_rate_hz;
	uint32_t i2s_sample_rate_hz;

	I2S_SPI_module = (SPI_T*)cfg_hndl->base_address;
	DEV_IOCTL_1_PARAMS(dpwm_dev,
			DPWM_I94XXX_GET_ROOT_CLK_DEV, &dpwm_root_clk_dev);
	if ((SPI_T*)SPI1_BASE == I2S_SPI_module)
	{
		DEV_IOCTL_1_PARAMS(i94xxx_spi1clk_clk_dev,
				CLK_IOCTL_GET_ROOT_CLK, &i2s_root_clk_dev);
		DEV_IOCTL_1_PARAMS(i94xxx_I2S_onSPI1_FSCLK_clk_dev,
				CLK_IOCTL_GET_FREQ, &i2s_sample_rate_hz);
	}
	else if ((SPI_T*)SPI2_BASE == I2S_SPI_module)
	{
		DEV_IOCTL_1_PARAMS(i94xxx_spi2clk_clk_dev,
				CLK_IOCTL_GET_ROOT_CLK, &i2s_root_clk_dev);
		DEV_IOCTL_1_PARAMS(i94xxx_I2S_onSPI2_FSCLK_clk_dev,
				CLK_IOCTL_GET_FREQ, &i2s_sample_rate_hz);
	}

	if (dpwm_root_clk_dev != i2s_root_clk_dev)
	{
		CRITICAL_ERROR("synchronized dpwm and i2s should be from same root clock");
	}

	DEV_IOCTL_1_PARAMS(dpwm_dev,
			DPWM_I94XXX_GET_SAMPLE_RATE_HZ, &dpwm_sample_rate_hz);

	if ( dpwm_sample_rate_hz != i2s_sample_rate_hz)
	{
		CRITICAL_ERROR("DPWM and I2S sample rates are not synchronized");
	}
#else
	CRITICAL_ERROR("DPWM not included in firmware");
#endif
}


/**
 * I2S_onSPI_i94xxx_ioctl()
 *
 * return:
 */
uint8_t I2S_onSPI_i94xxx_ioctl( struct dev_desc_t *adev,
			const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct I2S_onSPI_i94xxx_cfg_t *cfg_hndl;
	struct I2S_onSPI_i94xxx_runtime_t *runtime_hndl;
	SPI_T	*I2S_SPI_module;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(adev);

	if ((0 == runtime_hndl->init_done) && (IOCTL_DEVICE_START != aIoctl_num))
	{
		CRITICAL_ERROR("not initialized yet");
	}

	I2S_SPI_module = (SPI_T*)cfg_hndl->base_address;

	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		i94xxx_I2S_onSPI_init(adev, cfg_hndl);
		runtime_hndl->init_done = 1;
		break;

	case SPI_I2S_ENABLE_INPUT_IOCTL:
		SPI_I2S_ENABLE_RX(I2S_SPI_module);
#if !defined(USE_SPI_I2S_INTERRUPT)
	    SPI_I2S_ENABLE_RXDMA(I2S_SPI_module);
#endif
		break;

	case SPI_I2S_DISABLE_INPUT_IOCTL:
		SPI_I2S_DISABLE_RX(I2S_SPI_module);
		SPI_I2S_DISABLE_RXDMA(I2S_SPI_module);
		break;

	case SPI_I2S_ENABLE_OUTPUT_IOCTL:
		SPI_I2S_ENABLE_TX(I2S_SPI_module);
#if !defined(USE_SPI_I2S_INTERRUPT)
	    SPI_I2S_ENABLE_TXDMA(I2S_SPI_module);
#endif
		break;

	case SPI_I2S_DISABLE_OUTPUT_IOCTL:
		SPI_I2S_DISABLE_TX(I2S_SPI_module);
		SPI_I2S_DISABLE_TXDMA(I2S_SPI_module);
		break;

	case SPI_I2S_I94XXX_SYNC_FS_TO_DPWM_FS_RATE:
		i94xxx_sync_to_dpwm_fs_rate(cfg_hndl, aIoctl_param1);
		break;


	default :
		return 1;
	}
	return 0;
}
