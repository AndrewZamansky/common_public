/*
 *
 * I2S_onSPI_i94xxx.c
 *
 */

#include "_project_defines.h"
#include "_project_typedefs.h"

#include "dev_management_api.h"
#include "errors_api.h"

#include "I2S_onSPI_i94xxx_api.h"
#include "I2S_onSPI_i94xxx.h"
#include "irq_api.h"

#include "timer_wrapper_api.h"
#include "os_wrapper.h"


#include "I94100.h"

#include "clock_control_i94xxx_api.h"
#include "pin_control_api.h"
#ifdef CONFIG_INCLUDE_INTERNAL_PWM
	#include "dpwm_i9xxxx_api.h"
#endif


#if !defined(INTERRUPT_PRIORITY_FOR_I2S)
	#error "INTERRUPT_PRIORITY_FOR_I2S should be defined"
#endif

#if CHECK_INTERRUPT_PRIO_FOR_OS_SYSCALLS(INTERRUPT_PRIORITY_FOR_I2S)
	#error "priority should be lower then maximal priority for os syscalls"
#endif

#define MAX_NUM_OF_U32_WORDS_IN_INPUT_BUFFER 16



/*
 * Currently I2S_onSPI_i94xxx_callback() is a loopback function, TX = RX.
 * TODO: Make a data transfer interface.
 */

static uint8_t I2S_onSPI_i94xxx_callback(struct dev_desc_t *adev ,
		uint8_t aCallback_num , void * aCallback_param1,
		void * aCallback_param2)
{
	struct I2S_onSPI_i94xxx_cfg_t *cfg_hndl;
	struct I2S_onSPI_i94xxx_runtime_t *runtime_handle;
	SPI_T	*I2S_SPI_module;
	i2s_on_spi_interrupt_handler_t  int_handler;
	uint32_t *in_buff_u32;
	size_t  buff_pos;
	uint32_t status;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(I2S_onSPI_i94xxx, adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(I2S_onSPI_i94xxx, adev);

	I2S_SPI_module = (SPI_T*)cfg_hndl->base_address;

	in_buff_u32 = (uint32_t *)runtime_handle->in_buff;
	buff_pos = 0;

	status = I2S_SPI_module->I2SSTS;
	while (FALSE == (SPI_I2S_IS_RX_EMPTY(I2S_SPI_module)))
	{
		in_buff_u32[buff_pos] = SPI_I2S_READ_RX_FIFO(I2S_SPI_module);
		buff_pos++;
		if (MAX_NUM_OF_U32_WORDS_IN_INPUT_BUFFER <= buff_pos) break;
	}
	SPI_I2S_CLR_INT_FLAG(I2S_SPI_module, SPI_I2S_RXTH_INT_MASK);

	if (status & SPI_I2S_FLAG_RX_OV)
	{
		runtime_handle->overflow_counter++;
		I2S_SPI_module->I2SSTS |= SPI_I2S_FLAG_RX_OV;
		return 0; // just discard data
	}

	if (buff_pos)
	{
		int_handler = runtime_handle->int_handler;
		if (NULL != int_handler)
		{
			int_handler((uint8_t*)in_buff_u32, buff_pos * 4);
		}
	}

	return 0;
}


/**
 * I2S_i9xxxx_pwrite()
 *
 * return:
 */
static size_t I2S_onSPI_i94xxx_pwrite(struct dev_desc_t *adev,
			const uint8_t *apData, size_t aLength, size_t aOffset)
{
	struct I2S_onSPI_i94xxx_cfg_t *cfg_hndl;
	SPI_T  *I2S_SPI_module;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(I2S_onSPI_i94xxx, adev);
	I2S_SPI_module = (SPI_T*)cfg_hndl->base_address;
	if (0 != (aLength % 4))
	{
		CRITICAL_ERROR("data should be in U32 words")
	}

	while (aLength)
	{
		SPI_I2S_WRITE_TX_FIFO(I2S_SPI_module, *(uint32_t*)apData);
		apData += 4;
		aLength -= 4;
	}
	return aLength;
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


static void set_Mclk(struct I2S_onSPI_i94xxx_cfg_t *cfg_hndl,
		SPI_T  *I2S_SPI_module, struct dev_desc_t *spi_i2s_clk_dev)
{
	uint32_t  src_clk_freq;
	uint32_t  mclk_freq;
	uint32_t  res;
	uint16_t  Mclock_factor_based_on_FSclock;

	if (0xffffffff == cfg_hndl->MCLK_pin)
	{
		return;
	}

	DEV_IOCTL(spi_i2s_clk_dev, CLK_IOCTL_GET_FREQ, &src_clk_freq);
	Mclock_factor_based_on_FSclock = cfg_hndl->Mclock_factor_based_on_FSclock;
	mclk_freq = cfg_hndl->sample_rate * Mclock_factor_based_on_FSclock;

	res = (src_clk_freq / 2) % mclk_freq;
	if ((src_clk_freq < mclk_freq) || ((src_clk_freq != mclk_freq) && (res)))
	{
		CRITICAL_ERROR("cannot create desired M clock");
	}

	SPI_I2SEnableMCLK(I2S_SPI_module, mclk_freq);
}


static struct dev_desc_t* set_clocks(
			struct I2S_onSPI_i94xxx_cfg_t *cfg_hndl,
			SPI_T  *I2S_SPI_module)
{
	struct dev_desc_t  *clk_dev;
	struct dev_desc_t  *fs_clk_dev;
	struct dev_desc_t  *bclk_clk_dev;
	struct dev_desc_t  *mclk_clk_dev;
	struct dev_desc_t  *src_clock;

	src_clock = cfg_hndl->src_clock;

	if ((SPI_T*)SPI1_BASE == I2S_SPI_module)
	{
		clk_dev = i94xxx_spi1_clk_dev;
		fs_clk_dev = i94xxx_I2S_onSPI1_FSCLK_clk_dev;
		bclk_clk_dev = i94xxx_I2S_onSPI1_BCLK_clk_dev;
		mclk_clk_dev = i94xxx_I2S_onSPI1_MCLK_clk_dev;
	}
	else if ((SPI_T*)SPI2_BASE == I2S_SPI_module)
	{
		clk_dev = i94xxx_spi2_clk_dev;
		fs_clk_dev = i94xxx_I2S_onSPI2_FSCLK_clk_dev;
		bclk_clk_dev = i94xxx_I2S_onSPI2_BCLK_clk_dev;
		mclk_clk_dev = i94xxx_I2S_onSPI2_MCLK_clk_dev;
	}
	else
	{
		CRITICAL_ERROR("SPI Base Address not defined.");
	}

	DEV_IOCTL(clk_dev, IOCTL_DEVICE_START);
	DEV_IOCTL(clk_dev, CLK_IOCTL_SET_PARENT, src_clock);
	DEV_IOCTL(clk_dev, CLK_IOCTL_ENABLE);
	DEV_IOCTL(fs_clk_dev, IOCTL_DEVICE_START);
	DEV_IOCTL(bclk_clk_dev, IOCTL_DEVICE_START);
	DEV_IOCTL(mclk_clk_dev, IOCTL_DEVICE_START);
	return clk_dev;
}


static void i94xxx_I2S_onSPI_init(struct dev_desc_t *adev,
								struct I2S_onSPI_i94xxx_cfg_t *cfg_hndl)
{
	SPI_T  *I2S_SPI_module;
	uint32_t clk_mode;
	uint32_t sample_rate;
	uint32_t data_width;
	uint32_t audio_format;
	uint32_t txrx_format;
	struct dev_desc_t  *clk_dev;

	I2S_SPI_module = (SPI_T*)cfg_hndl->base_address;

	clk_mode = cfg_hndl->clk_mode;
	sample_rate = cfg_hndl->sample_rate;
	data_width = cfg_hndl->data_width;
	audio_format = cfg_hndl->audio_format;
	txrx_format = cfg_hndl->txrx_format;

	clk_dev = set_clocks(cfg_hndl, I2S_SPI_module);

	SPI_I2SOpen(I2S_SPI_module, clk_mode, sample_rate, data_width,
										audio_format, txrx_format);

	set_Mclk(cfg_hndl, I2S_SPI_module, clk_dev);

	SPI_I2S_SET_RXTH(I2S_SPI_module, SPI_I2S_FIFO_RX_LEVEL_4);
	SPI_I2S_SET_TXTH(I2S_SPI_module, SPI_I2S_FIFO_TX_LEVEL_4);

	SPI_I2S_RST_TX_FIFO(I2S_SPI_module);
	SPI_I2S_RST_RX_FIFO(I2S_SPI_module);
	I2S_SPI_module->PDMACTL =  SPI_PDMACTL_PDMARST_Msk;


	if (cfg_hndl->do_reordering_for_16or8bit_channels)
	{
		SPI_I2S_SET_STEREOORDER(I2S_SPI_module, SPI_I2SORDER_LOW);
	}
	else
	{
		SPI_I2S_SET_STEREOORDER(I2S_SPI_module, SPI_I2SORDER_HIGH);
	}

	configure_i2s_spi_pinout(cfg_hndl);

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
	DEV_IOCTL(dpwm_dev, DPWM_I9XXXX_GET_ROOT_CLK_DEV, &dpwm_root_clk_dev);
	if ((SPI_T*)SPI1_BASE == I2S_SPI_module)
	{
		DEV_IOCTL(i94xxx_spi1_clk_dev,
				CLK_IOCTL_GET_ROOT_CLK, &i2s_root_clk_dev);
		DEV_IOCTL(i94xxx_I2S_onSPI1_FSCLK_clk_dev,
				CLK_IOCTL_GET_FREQ, &i2s_sample_rate_hz);
	}
	else if ((SPI_T*)SPI2_BASE == I2S_SPI_module)
	{
		DEV_IOCTL(i94xxx_spi2_clk_dev,
				CLK_IOCTL_GET_ROOT_CLK, &i2s_root_clk_dev);
		DEV_IOCTL(i94xxx_I2S_onSPI2_FSCLK_clk_dev,
				CLK_IOCTL_GET_FREQ, &i2s_sample_rate_hz);
	}

	if (dpwm_root_clk_dev != i2s_root_clk_dev)
	{
		CRITICAL_ERROR("synchronized dpwm and i2s should be from same root clock");
	}

	DEV_IOCTL(dpwm_dev, DPWM_I9XXXX_GET_SAMPLE_RATE_HZ, &dpwm_sample_rate_hz);

	if ( dpwm_sample_rate_hz != i2s_sample_rate_hz)
	{
		CRITICAL_ERROR("DPWM and I2S sample rates are not synchronized");
	}
#else
	CRITICAL_ERROR("DPWM not included in firmware");
#endif
}


static void enable_I2S_irq(struct dev_desc_t *adev, SPI_T*  I2S_SPI_module)
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
	irq_register_device_on_interrupt(i2s_spi_irq , adev);
	irq_set_priority(i2s_spi_irq , INTERRUPT_PRIORITY_FOR_I2S );
	irq_enable_interrupt(i2s_spi_irq);
}


static void enable_tx(
		struct dev_desc_t *adev, struct I2S_onSPI_i94xxx_cfg_t *cfg_hndl)
{
	SPI_T	*I2S_SPI_module;

	I2S_SPI_module = (SPI_T*)cfg_hndl->base_address;

	if (I2S_onSPI_I9XXXX_API_DATA_TRANSFER_TYPE_DMA ==
								cfg_hndl->data_transfer_type)
	{
		SPI_I2S_ENABLE_TXDMA(I2S_SPI_module);
	}
	else
	{
		enable_I2S_irq(adev, I2S_SPI_module);
	}
	SPI_I2S_ENABLE_TX(I2S_SPI_module);
}


static void enable_rx(struct dev_desc_t *adev,
		struct I2S_onSPI_i94xxx_cfg_t *cfg_hndl,
		struct I2S_onSPI_i94xxx_runtime_t *runtime_hndl)
{
	SPI_T	*I2S_SPI_module;
	uint16_t buff_size;

	I2S_SPI_module = (SPI_T*)cfg_hndl->base_address;
	if (I2S_onSPI_I9XXXX_API_DATA_TRANSFER_TYPE_DMA ==
								cfg_hndl->data_transfer_type)
	{
		SPI_I2S_ENABLE_RXDMA(I2S_SPI_module);
	}
	else
	{
		buff_size = MAX_NUM_OF_U32_WORDS_IN_INPUT_BUFFER * 4;
		runtime_hndl->in_buff = (uint8_t*)os_safe_malloc(buff_size);
		errors_api_check_if_malloc_succeed(runtime_hndl->in_buff);
		enable_I2S_irq(adev, I2S_SPI_module);
		SPI_I2SEnableInt(I2S_SPI_module, SPI_I2S_RXTH_INT_MASK);
	}
	SPI_I2S_ENABLE_RX(I2S_SPI_module);
}


/**
 * I2S_onSPI_i94xxx_ioctl()
 *
 * return:
 */
static uint8_t I2S_onSPI_i94xxx_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct I2S_onSPI_i94xxx_cfg_t *cfg_hndl;
	struct I2S_onSPI_i94xxx_runtime_t *runtime_hndl;
	SPI_T	*I2S_SPI_module;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(I2S_onSPI_i94xxx, adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(I2S_onSPI_i94xxx, adev);

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
		enable_rx(adev, cfg_hndl, runtime_hndl);
		break;
	case SPI_I2S_DISABLE_INPUT_IOCTL:
		SPI_I2S_DISABLE_RX(I2S_SPI_module);
		SPI_I2S_DISABLE_RXDMA(I2S_SPI_module);
		break;
	case SPI_I2S_ENABLE_OUTPUT_IOCTL:
		enable_tx(adev, cfg_hndl);
		break;
	case SPI_I2S_DISABLE_OUTPUT_IOCTL:
		SPI_I2S_DISABLE_TX(I2S_SPI_module);
		SPI_I2S_DISABLE_TXDMA(I2S_SPI_module);
		break;
	case SPI_I2S_I94XXX_SYNC_FS_TO_DPWM_FS_RATE:
		i94xxx_sync_to_dpwm_fs_rate(cfg_hndl, aIoctl_param1);
		break;
	case SPI_I2S_I9XXXX_SET_INTERRUPT_HANDLER:
		runtime_hndl->int_handler = aIoctl_param1;
		break;
	case SPI_I2S_I9XXXX_GET_OVERFLOW_COUNTER:
		*(uint32_t *)aIoctl_param1 = runtime_hndl->overflow_counter;
		runtime_hndl->overflow_counter = 0;
		break;
	default :
		return 1;
	}
	return 0;
}

#define MODULE_NAME                     I2S_onSPI_i94xxx
#define MODULE_IOCTL_FUNCTION           I2S_onSPI_i94xxx_ioctl
#define MODULE_CALLBACK_FUNCTION        I2S_onSPI_i94xxx_callback
#define MODULE_PWRITE_FUNCTION          I2S_onSPI_i94xxx_pwrite
#include "add_module.h"
