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

#ifdef CONFIG_INCLUDE_INTERNAL_PWM
	#include "dpwm_i94xxx_api.h"
#endif

#include "clock_control_i94xxx_api.h"
#include "pin_control_api.h"



//#define  DEBUG_USE_INTERRUPT

#ifdef  DEBUG_USE_INTERRUPT
	#if !defined(INTERRUPT_PRIORITY_FOR_I2S)
		#error "INTERRUPT_PRIORITY_FOR_I2S should be defined"
	#endif

	#if CHECK_INTERRUPT_PRIO_FOR_OS_SYSCALLS(INTERRUPT_PRIORITY_FOR_I2S)
		#error "priority should be lower then maximal priority for os syscalls"
	#endif
#endif

/********  defines *********************/

/********  types  *********************/



/* ------------- External variables --------------------------*/

/* ------------------------ External functions --------------*/

/* ------------------------ Exported variables ---------------*/

//#define  DEBUG_USE_INTERRUPT

#ifdef  DEBUG_USE_INTERRUPT
volatile int g_u32DataCount = 0;
volatile int status = 0;
#define TEST_COUNT   100
uint32_t i2s_data[TEST_COUNT+1] = {0};
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
		its_data[pos] = I2S0->RXFIFO;
    }
    g_u32DataCount += 2;
}
#endif


static void disable_pinout(struct I2S_i94xxx_cfg_t *cfg_hndl)
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


static void configure_pinout(struct I2S_i94xxx_cfg_t *cfg_hndl)
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


static void set_clocks(struct I2S_i94xxx_cfg_t *cfg_hndl)
{
	uint32_t  src_clk_freq;
	uint32_t  mclk_freq;
	uint32_t  res;
	uint16_t  Mclock_factor_based_on_FSclock;

	DEV_IOCTL_1_PARAMS(i94xxx_i2s_clk_dev, CLK_IOCTL_GET_FREQ, &src_clk_freq);
	Mclock_factor_based_on_FSclock = cfg_hndl->Mclock_factor_based_on_FSclock;
	mclk_freq = cfg_hndl->sample_rate * Mclock_factor_based_on_FSclock;

	res = (src_clk_freq / 2) % mclk_freq;
	if ((src_clk_freq < mclk_freq) || ((src_clk_freq != mclk_freq) && (res)))
	{
		CRITICAL_ERROR("cannot create desired M clock");
	}

	I2S_EnableMCLK(I2S0, mclk_freq);
}


static void i94xxx_I2S_init(struct I2S_i94xxx_cfg_t *cfg_hndl,
		struct I2S_i94xxx_runtime_t *runtime_handle)
{
	uint8_t   num_of_bytes_in_word;
	uint32_t  i2s_format;
	struct dev_desc_t  *src_clock;

	if (runtime_handle->init_done)
	{
		return;
	}

	i2s_format = cfg_hndl->i2s_format;
	num_of_bytes_in_word = cfg_hndl->num_of_bytes_in_word;
	src_clock = cfg_hndl->src_clock;

	DEV_IOCTL_0_PARAMS(i94xxx_i2s_clk_dev, IOCTL_DEVICE_START);
	DEV_IOCTL_1_PARAMS(i94xxx_i2s_clk_dev, CLK_IOCTL_SET_PARENT, src_clock);
	DEV_IOCTL_0_PARAMS(i94xxx_i2s_clk_dev, CLK_IOCTL_ENABLE);
	DEV_IOCTL_0_PARAMS(i94xxx_I2S_FSCLK_clk_dev, IOCTL_DEVICE_START);
	DEV_IOCTL_0_PARAMS(i94xxx_I2S_BCLK_clk_dev, IOCTL_DEVICE_START);
	DEV_IOCTL_0_PARAMS(i94xxx_I2S_MCLK_clk_dev, IOCTL_DEVICE_START);

	configure_pinout(cfg_hndl);

	runtime_handle->actual_sample_rate = I2S_Open(
					I2S0,
					cfg_hndl->clock_mode,
					cfg_hndl->sample_rate,
					(num_of_bytes_in_word - 1) << I2S_CTL0_DATWIDTH_Pos,
					((cfg_hndl->tdm_ch_num / 2) - 1) << I2S_CTL0_TDMCHNUM_Pos,
					cfg_hndl->mono_or_stereo,
					i2s_format);

	set_clocks(cfg_hndl);

	I2S_SET_TXTH(I2S0, I2S_FIFO_TX_LEVEL_WORD_4);
	I2S_SET_RXTH(I2S0, I2S_FIFO_RX_LEVEL_WORD_4);
	I2S_CLR_TX_FIFO(I2S0);
	I2S_CLR_RX_FIFO(I2S0);

	if (( I2S_FORMAT_PCMMSB == i2s_format ) ||
			(I2S_FORMAT_PCMLSB == i2s_format) || (I2S_FORMAT_PCM == i2s_format))
	{
		I2S_SET_PCMSYNC(I2S0, I2S_PCMSYNC_BCLK);
		if (1 == num_of_bytes_in_word)
		{
			I2S_SET_CHWIDTH(I2S0, I2S_CHWIDTH_8);
		}
		else if (2 == num_of_bytes_in_word)
		{
			I2S_SET_CHWIDTH(I2S0, I2S_CHWIDTH_16);
		}
		else if (3 == num_of_bytes_in_word)
		{
			I2S_SET_CHWIDTH(I2S0, I2S_CHWIDTH_24);
		}
		else if (4 == num_of_bytes_in_word)
		{
			I2S_SET_CHWIDTH(I2S0, I2S_CHWIDTH_32);
		}
	}

	if (cfg_hndl->do_reordering_for_16or8bit_channels)
	{
		I2S_SET_STEREOORDER(I2S0, I2S_ORDER_EVENLOW);
	}
	else
	{
		I2S_SET_STEREOORDER(I2S0, I2S_ORDER_EVENHIGH);
	}

	I2S_ENABLE_RX(I2S0);
	I2S_ENABLE(I2S0);

#ifdef DEBUG_USE_INTERRUPT
	irq_register_interrupt(I2S0_IRQn , I2S_IRQHandler);
	irq_set_priority(I2S0_IRQn , INTERRUPT_PRIORITY_FOR_I2S );
	irq_enable_interrupt(I2S0_IRQn);
	I2S_ENABLE_INT(I2S0, I2S_IEN_RXTHIEN_Msk);

#else
	I2S0->CTL0 |= I2S_CTL0_RXPDMAEN_Msk;
#endif
	runtime_handle->init_done = 1;
}


static void i94xxx_sync_to_dpwm_fs_rate(struct I2S_i94xxx_cfg_t *cfg_hndl,
		struct dev_desc_t *dpwm_dev)
{
#ifdef CONFIG_INCLUDE_INTERNAL_PWM
	struct dev_desc_t *dpwm_root_clk_dev;
	struct dev_desc_t *i2s_root_clk_dev;
	uint32_t dpwm_sample_rate_hz;
	uint32_t i2s_sample_rate_hz;

	DEV_IOCTL_1_PARAMS(dpwm_dev,
			DPWM_I94XXX_GET_ROOT_CLK_DEV, &dpwm_root_clk_dev);
	DEV_IOCTL_1_PARAMS(i94xxx_i2s_clk_dev,
			CLK_IOCTL_GET_ROOT_CLK, &i2s_root_clk_dev);

	if (dpwm_root_clk_dev != i2s_root_clk_dev)
	{
		CRITICAL_ERROR("synchronized dpwm and i2s should be from same root clock");
	}

	DEV_IOCTL_1_PARAMS(i94xxx_I2S_FSCLK_clk_dev,
						CLK_IOCTL_GET_FREQ, &i2s_sample_rate_hz);
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
 * I2S_i94xxx_ioctl()
 *
 * return:
 */
static uint8_t I2S_i94xxx_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1 , void * aIoctl_param2)
{
	struct I2S_i94xxx_cfg_t *cfg_hndl;
	struct I2S_i94xxx_runtime_t *runtime_handle;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);

	if ((0 == runtime_handle->init_done) && (IOCTL_DEVICE_START != aIoctl_num))
	{
		CRITICAL_ERROR("not initialized yet");
	}


	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		i94xxx_I2S_init(cfg_hndl, runtime_handle);
		break;

	case I2S_I94XXX_ENABLE_OUTPUT_IOCTL:
		I2S_ENABLE_TX(I2S0);
		I2S0->CTL0 |= I2S_CTL0_TXPDMAEN_Msk;
		break;

	case I2S_I94XXX_STOP_IOCTL:
		I2S_DISABLE_TX(I2S0);
		I2S_DISABLE_RX(I2S0);
		I2S_DISABLE(I2S0);
		I2S_Close(I2S0);
		I2S_DisableMCLK(I2S0);
		disable_pinout(cfg_hndl);
		break;

	case I2S_I94XXX_GET_MEASURED_SAMPLE_RATE:
		*((uint32_t*)aIoctl_param1) = runtime_handle->actual_sample_rate;
		break;

	case I2S_I94XXX_SYNC_FS_TO_DPWM_FS_RATE:
		i94xxx_sync_to_dpwm_fs_rate(cfg_hndl, aIoctl_param1);
		break;

	default :
		return 1;
	}
	return 0;
}

#define	MODULE_NAME                       I2S_i94xxx
#define	MODULE_IOCTL_FUNCTION             I2S_i94xxx_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE    struct I2S_i94xxx_cfg_t
#define MODULE_RUNTIME_DATA_STRUCT_TYPE   struct I2S_i94xxx_runtime_t
#include "add_module.h"
