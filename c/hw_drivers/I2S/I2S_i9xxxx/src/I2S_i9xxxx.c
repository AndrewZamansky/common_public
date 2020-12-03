/*
 *
 * I2S_i9xxxx.c
 *
 */

#include "I2S_i9xxxx_api.h"
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "I2S_i9xxxx.h"
#include "irq_api.h"
#include "timer_wrapper_api.h"
#include "os_wrapper.h"

#ifdef CONFIG_I94XXX
	#include "I94100.h"
	#include "clock_control_i94xxx_api.h"
#elif CONFIG_I96XXX_M0
	#include "I96100.h"
	#include "clock_control_i96xxx_m0_api.h"
#endif


#include "i2s.h"
#include "dpwm.h"

#ifdef CONFIG_INCLUDE_INTERNAL_PWM
	#include "dpwm_i9xxxx_api.h"
#endif

#include "pin_control_api.h"



//#define  DEBUG_USE_INTERRUPT

#ifdef  DEBUG_USE_INTERRUPT
	#if !defined(INTERRUPT_PRIORITY_FOR_I2S)
		#error "INTERRUPT_PRIORITY_FOR_I2S should be defined"
	#endif

	#if CHECK_INTERRUPT_PRIO_FOR_OS_SYSCALLS(INTERRUPT_PRIORITY_FOR_I2S)
		#error "priority should be lower then maximal priority for os syscalls"
	#endif



#define TEST_COUNT   100
static uint32_t i2s_data[TEST_COUNT + 1] = {0};
static int pos = 0;
#endif

static uint8_t I2S_i9xxxx_callback(struct dev_desc_t *adev,
		uint8_t aCallback_num , void * aCallback_param1,
		void * aCallback_param2)
{
	struct I2S_i9xxxx_cfg_t *cfg_hndl;
	uint32_t status;
	struct I2S_i9xxxx_runtime_t *runtime_handle;
	I2S_T*  base_address;
	i2s_interrupt_handler_t  int_handler;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(I2S_i9xxxx, adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(I2S_i9xxxx, adev);
	base_address = (I2S_T*)cfg_hndl->base_address;
	status = base_address->STATUS0;
    if ((status & I2S_STATUS0_RXEMPTY_Msk) == 0)
    {
    	int_handler = runtime_handle->int_handler;
    	if (NULL != int_handler)
    	{
    		// TODO:
    		//int_handler();
    	}
#ifdef  DEBUG_USE_INTERRUPT
		if (pos < TEST_COUNT)
		{
			pos++;
		}
		else
		{
			pos = 0;
		}
		i2s_data[pos] = I2S0->RXFIFO;
		I2S0->TXFIFO = 0x22224444;//i2s_data[pos];
#endif
    }
    return 0;
}


static void disable_pinout(struct I2S_i9xxxx_cfg_t *cfg_hndl)
{
	pin_control_api_clear_pin_function(cfg_hndl->BCLK_pin);
	pin_control_api_clear_pin_function(cfg_hndl->LRCLK_pin);
	pin_control_api_clear_pin_function(cfg_hndl->DI_pin);
	pin_control_api_clear_pin_function(cfg_hndl->DO_pin);
	if (0xffffffff != cfg_hndl->MCLK_pin)
	{
		pin_control_api_clear_pin_function(cfg_hndl->MCLK_pin);
	}
}


static void configure_pinout(struct I2S_i9xxxx_cfg_t *cfg_hndl)
{
	pin_control_api_set_pin_function(cfg_hndl->BCLK_pin);
	pin_control_api_set_pin_function(cfg_hndl->LRCLK_pin);
	pin_control_api_set_pin_function(cfg_hndl->DI_pin);
	pin_control_api_set_pin_function(cfg_hndl->DO_pin);
	if (0xffffffff != cfg_hndl->MCLK_pin)
	{
		pin_control_api_set_pin_function(cfg_hndl->MCLK_pin);
	}
}


static void set_clocks(struct I2S_i9xxxx_cfg_t *cfg_hndl,
			struct I2S_i9xxxx_runtime_t *runtime_handle, I2S_T*  base_address)
{
	struct dev_desc_t *src_clock;

	src_clock = cfg_hndl->src_clock;
#ifdef CONFIG_I94XXX
	runtime_handle->i9xxxx_i2s_clk_dev = i94xxx_i2s_clk_dev;
	runtime_handle->i9xxxx_I2S_MCLK_clk_dev = i94xxx_I2S_MCLK_clk_dev;
	runtime_handle->i9xxxx_I2S_BCLK_clk_dev = i94xxx_I2S_BCLK_clk_dev;
	runtime_handle->i9xxxx_I2S_FSCLK_clk_dev = i94xxx_I2S_FSCLK_clk_dev;
#elif CONFIG_I96XXX_M0
	if (base_address == I2S0)
	{
		runtime_handle->i9xxxx_i2s_clk_dev = i96xxx_i2s0_clk_dev;
		runtime_handle->i9xxxx_I2S_MCLK_clk_dev = i96xxx_I2S0_MCLK_clk_dev;
		runtime_handle->i9xxxx_I2S_BCLK_clk_dev = i96xxx_I2S0_BCLK_clk_dev;
		runtime_handle->i9xxxx_I2S_FSCLK_clk_dev = i96xxx_I2S0_FSCLK_clk_dev;
	}
	else
	{
		runtime_handle->i9xxxx_i2s_clk_dev = i96xxx_i2s1_clk_dev;
		runtime_handle->i9xxxx_I2S_MCLK_clk_dev = i96xxx_I2S1_MCLK_clk_dev;
		runtime_handle->i9xxxx_I2S_BCLK_clk_dev = i96xxx_I2S1_BCLK_clk_dev;
		runtime_handle->i9xxxx_I2S_FSCLK_clk_dev = i96xxx_I2S1_FSCLK_clk_dev;
	}
#endif
	DEV_IOCTL_0_PARAMS(runtime_handle->i9xxxx_i2s_clk_dev, IOCTL_DEVICE_START);
	DEV_IOCTL_1_PARAMS(
		runtime_handle->i9xxxx_i2s_clk_dev, CLK_IOCTL_SET_PARENT, src_clock);
	DEV_IOCTL_0_PARAMS(runtime_handle->i9xxxx_i2s_clk_dev, CLK_IOCTL_ENABLE);
	DEV_IOCTL_0_PARAMS(
			runtime_handle->i9xxxx_I2S_FSCLK_clk_dev, IOCTL_DEVICE_START);
	DEV_IOCTL_0_PARAMS(
			runtime_handle->i9xxxx_I2S_BCLK_clk_dev, IOCTL_DEVICE_START);
	DEV_IOCTL_0_PARAMS(
			runtime_handle->i9xxxx_I2S_MCLK_clk_dev, IOCTL_DEVICE_START);
}


static void set_Mclk(struct I2S_i9xxxx_cfg_t *cfg_hndl,
			struct I2S_i9xxxx_runtime_t *runtime_handle, I2S_T*  base_address)
{
	uint32_t  src_clk_freq;
	uint32_t  mclk_freq;
	uint32_t  res;
	uint16_t  Mclock_factor_based_on_FSclock;

	if (I2S_I9XXXX_API_MASTER_MODE != cfg_hndl->clock_mode) return;

	DEV_IOCTL_1_PARAMS(
		runtime_handle->i9xxxx_i2s_clk_dev, CLK_IOCTL_GET_FREQ, &src_clk_freq);
	Mclock_factor_based_on_FSclock = cfg_hndl->Mclock_factor_based_on_FSclock;
	mclk_freq = cfg_hndl->sample_rate * Mclock_factor_based_on_FSclock;

	res = (src_clk_freq / 2) % mclk_freq;
	if ((src_clk_freq < mclk_freq) || ((src_clk_freq != mclk_freq) && (res)))
	{
		CRITICAL_ERROR("cannot create desired M clock");
	}

	I2S_EnableMCLK(base_address, mclk_freq);
}


static void set_i2s_fifo_params(I2S_T*  base_address,
		uint32_t  i2s_format, uint8_t num_of_bytes_in_word)
{
	I2S_SET_TXTH(base_address, I2S_FIFO_TX_LEVEL_WORD_4);
	I2S_SET_RXTH(base_address, I2S_FIFO_RX_LEVEL_WORD_4);
	I2S_CLR_TX_FIFO(base_address);
	I2S_CLR_RX_FIFO(base_address);

	if (( I2S_FORMAT_PCMMSB == i2s_format ) ||
			(I2S_FORMAT_PCMLSB == i2s_format) || (I2S_FORMAT_PCM == i2s_format))
	{
		I2S_SET_PCMSYNC(base_address, I2S_PCMSYNC_BCLK);
		if (1 == num_of_bytes_in_word)
		{
			I2S_SET_CHWIDTH(base_address, I2S_CHWIDTH_8);
		}
		else if (2 == num_of_bytes_in_word)
		{
			I2S_SET_CHWIDTH(base_address, I2S_CHWIDTH_16);
		}
		else if (3 == num_of_bytes_in_word)
		{
			I2S_SET_CHWIDTH(base_address, I2S_CHWIDTH_24);
		}
		else if (4 == num_of_bytes_in_word)
		{
			I2S_SET_CHWIDTH(base_address, I2S_CHWIDTH_32);
		}
	}
}


static void i9xxxx_I2S_init(struct dev_desc_t *adev,
		struct I2S_i9xxxx_cfg_t *cfg_hndl,
		struct I2S_i9xxxx_runtime_t *runtime_handle,
		I2S_T*  base_address)
{
	uint8_t   num_of_bytes_in_word;
	uint32_t  i2s_format;

	if (runtime_handle->init_done) return;

	i2s_format = cfg_hndl->i2s_format;
	num_of_bytes_in_word = cfg_hndl->num_of_bytes_in_word;

	set_clocks(cfg_hndl, runtime_handle, base_address);

	runtime_handle->actual_sample_rate = I2S_Open(
					base_address,
					cfg_hndl->clock_mode,
					cfg_hndl->sample_rate,
					(num_of_bytes_in_word - 1) << I2S_CTL0_DATWIDTH_Pos,
					((cfg_hndl->tdm_ch_num / 2) - 1) << I2S_CTL0_TDMCHNUM_Pos,
					cfg_hndl->mono_or_stereo,
					i2s_format);

	set_Mclk(cfg_hndl, runtime_handle, base_address);
	set_i2s_fifo_params(base_address, i2s_format, num_of_bytes_in_word);

	if (cfg_hndl->do_reordering_for_16or8bit_channels)
	{
		I2S_SET_STEREOORDER(base_address, I2S_ORDER_EVENLOW);
	}
	else
	{
		I2S_SET_STEREOORDER(base_address, I2S_ORDER_EVENHIGH);
	}

	I2S_ENABLE(base_address);
	configure_pinout(cfg_hndl);
	runtime_handle->init_done = 1;
}


static void i9xxxx_sync_to_dpwm_fs_rate(struct I2S_i9xxxx_cfg_t *cfg_hndl,
	struct I2S_i9xxxx_runtime_t *runtime_handle, struct dev_desc_t *dpwm_dev)
{
#ifdef CONFIG_INCLUDE_INTERNAL_PWM
	struct dev_desc_t *dpwm_root_clk_dev;
	struct dev_desc_t *i2s_root_clk_dev;
	uint32_t dpwm_sample_rate_hz;
	uint32_t i2s_sample_rate_hz;

	DEV_IOCTL_1_PARAMS(dpwm_dev,
			DPWM_I9XXXX_GET_ROOT_CLK_DEV, &dpwm_root_clk_dev);
	DEV_IOCTL_1_PARAMS(runtime_handle->i9xxxx_i2s_clk_dev,
			CLK_IOCTL_GET_ROOT_CLK, &i2s_root_clk_dev);

	if (dpwm_root_clk_dev != i2s_root_clk_dev)
	{
		CRITICAL_ERROR("synchronized dpwm and i2s should be from same root clock");
	}

	DEV_IOCTL_1_PARAMS(runtime_handle->i9xxxx_I2S_FSCLK_clk_dev,
						CLK_IOCTL_GET_FREQ, &i2s_sample_rate_hz);
	DEV_IOCTL_1_PARAMS(dpwm_dev,
			DPWM_I9XXXX_GET_SAMPLE_RATE_HZ, &dpwm_sample_rate_hz);

	if ( dpwm_sample_rate_hz != i2s_sample_rate_hz)
	{
		CRITICAL_ERROR("DPWM and I2S sample rates are not synchronized");
	}
#else
	CRITICAL_ERROR("DPWM not included in firmware");
#endif
}


static void enable_I2S_irq(struct dev_desc_t *adev, I2S_T*  base_address)
{
	int i2s_irq;
	#ifdef CONFIG_I94XXX
		i2s_irq = I2S0_IRQn;
	#elif CONFIG_I96XXX_M0
		if (base_address == I2S0)
		{
			i2s_irq = I2S0_IRQn;
		}
		else
		{
			i2s_irq = I2S1_IRQn;
		}
	#endif
	irq_register_device_on_interrupt(i2s_irq, adev);
	irq_set_priority(i2s_irq , INTERRUPT_PRIORITY_FOR_I2S );
	irq_enable_interrupt(i2s_irq);
}


static void enable_tx(
		struct dev_desc_t *adev, struct I2S_i9xxxx_cfg_t *cfg_hndl)
{
	I2S_T*  base_address;
	base_address = (I2S_T*)cfg_hndl->base_address;

	I2S_ENABLE_TX(base_address);
	if (I2S_I9XXXX_API_DATA_TRANSFER_TYPE_DMA == cfg_hndl->data_transfer_type)
	{
		base_address->CTL0 |= I2S_CTL0_TXPDMAEN_Msk;
	}
	else
	{
		enable_I2S_irq(adev, base_address);
	}
}


static void enable_rx(
		struct dev_desc_t *adev, struct I2S_i9xxxx_cfg_t *cfg_hndl)
{
	I2S_T*  base_address;
	base_address = (I2S_T*)cfg_hndl->base_address;

	I2S_ENABLE_RX(base_address);
	if (I2S_I9XXXX_API_DATA_TRANSFER_TYPE_DMA == cfg_hndl->data_transfer_type)
	{
		base_address->CTL0 |= I2S_CTL0_RXPDMAEN_Msk;
	}
	else
	{
		enable_I2S_irq(adev, base_address);
		I2S_ENABLE_INT(base_address, I2S_IEN_RXTHIEN_Msk);
	}
}


/**
 * I2S_i9xxxx_ioctl()
 *
 * return:
 */
static uint8_t I2S_i9xxxx_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1 , void * aIoctl_param2)
{
	struct I2S_i9xxxx_cfg_t *cfg_hndl;
	struct I2S_i9xxxx_runtime_t *runtime_handle;
	I2S_T*  base_address;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(I2S_i9xxxx, adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(I2S_i9xxxx, adev);
	if ((0 == runtime_handle->init_done) && (IOCTL_DEVICE_START != aIoctl_num))
	{
		CRITICAL_ERROR("not initialized yet");
	}

	base_address = (I2S_T*)cfg_hndl->base_address;
	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		i9xxxx_I2S_init(adev, cfg_hndl, runtime_handle, base_address);
		break;
	case I2S_I9XXXX_ENABLE_OUTPUT_IOCTL:
		enable_tx(adev, cfg_hndl);
		break;
	case I2S_I9XXXX_ENABLE_INPUT_IOCTL:
		enable_rx(adev, cfg_hndl);
		break;
	case I2S_I9XXXX_STOP_IOCTL:
		I2S_DISABLE_TX(base_address);
		I2S_DISABLE_RX(base_address);
		I2S_DISABLE(base_address);
		I2S_Close(base_address);
		I2S_DisableMCLK(base_address);
		disable_pinout(cfg_hndl);
		break;

	case I2S_I9XXXX_GET_MEASURED_SAMPLE_RATE:
		*((uint32_t*)aIoctl_param1) = runtime_handle->actual_sample_rate;
		break;

	case I2S_I9XXXX_SYNC_FS_TO_DPWM_FS_RATE:
		i9xxxx_sync_to_dpwm_fs_rate(cfg_hndl, runtime_handle, aIoctl_param1);
		break;

	case I2S_I9XXXX_SET_INTERRUPT_HANDLER:
		runtime_handle->int_handler = aIoctl_param1;
		break;
	default :
		return 1;
	}
	return 0;
}

#define	MODULE_NAME                       I2S_i9xxxx
#define	MODULE_IOCTL_FUNCTION             I2S_i9xxxx_ioctl
#define MODULE_CALLBACK_FUNCTION          I2S_i9xxxx_callback
#include "add_module.h"
