/*
 *
 * dpwm_i9xxxx.c
 *
 */

#include "dpwm_i9xxxx_api.h"
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "irq_api.h"
#include "timer_wrapper_api.h"

#ifdef CONFIG_I94XXX
	#include "I94100.h"
	#include "clock_control_i94xxx_api.h"
#elif CONFIG_I96XXX_M0
	#include "I96100.h"
	#include "clock_control_i96xxx_m0_api.h"
#endif

#include "pin_control_api.h"
#include "dpwm.h"
#include "dpwm_i9xxxx.h"



//#define  DEBUG_USE_INTERRUPT

#ifdef  DEBUG_USE_INTERRUPT
	#if !defined(INTERRUPT_PRIORITY_FOR_DPWM)
		#error "INTERRUPT_PRIORITY_FOR_DPWM should be defined"
	#endif

	#if CHECK_INTERRUPT_PRIO_FOR_OS_SYSCALLS(INTERRUPT_PRIORITY_FOR_DPWM)
		#error "priority should be lower then maximal priority for os syscalls"
	#endif
#endif

#ifdef CONFIG_I94XXX
	#define DPWM_CLK_DEV  i94xxx_dpwm_clk_dev
#elif CONFIG_I96XXX_M0
	#define DPWM_CLK_DEV  i96xxx_dpwm_clk_dev
#endif

static uint32_t actualSamplingRate;
static uint8_t init_done = 0;


#ifdef 	DEBUG_USE_INTERRUPT

	#include "os_wrapper.h"
	extern void DPWM_IRQHandler();

	static volatile int g_u32DataCount = 0;
	static volatile int status = 0;
	#define TEST_COUNT	100
	static uint32_t data[TEST_COUNT+1] = {0};
	static volatile int pos = 0;
	void DPWM_IRQHandler()
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


static void DPWM_MuxPins(struct dpwm_i9xxxx_cfg_t *cfg_hndl)
{
	pin_control_api_set_pin_function(cfg_hndl->left_ch_p);
	pin_control_api_set_pin_function(cfg_hndl->left_ch_n);
	pin_control_api_set_pin_function(cfg_hndl->right_ch_p);
	pin_control_api_set_pin_function(cfg_hndl->right_ch_n);
	if ((0xffffffff != cfg_hndl->sub_ch_p) ||
			(0xffffffff != cfg_hndl->sub_ch_n))
	{
		CRITICAL_ERROR("sub channel not implemented yet");
	}
}


static uint32_t try_to_calclulate_clk_div(uint32_t needed_sample_rate,
		uint32_t src_clk_rate, uint16_t work_clock, uint16_t *clkdiv)
{
	uint16_t l_clkdiv;
	uint32_t u32Error1;
	uint32_t u32Error2;

	l_clkdiv = (src_clk_rate / needed_sample_rate) / work_clock;

	/* Adjust Error */
	u32Error1 = ((src_clk_rate / work_clock) / l_clkdiv) - needed_sample_rate;
	u32Error2 =
			needed_sample_rate - ((src_clk_rate / work_clock)/(l_clkdiv + 1));
	if (u32Error1 > u32Error2)
	{
		l_clkdiv = l_clkdiv + 1;
		u32Error1 = u32Error2;
	}

	*clkdiv = (l_clkdiv - 1);
	return u32Error1;
}


static void set_data_mode(struct dpwm_i9xxxx_cfg_t *cfg_hndl)
{
	if (DPWM_I9XXXX_API_DATA_MODE_FLOATING_POINT == cfg_hndl->data_mode)
	{
#ifdef CONFIG_I94XXX
		DPWM_ENABLE_FLOAT(DPWM);
#endif
	}
	else
	{
#ifdef CONFIG_I94XXX
		DPWM_DISABLE_FLOAT(DPWM);
#endif
		DPWM_SET_FIFODATAWIDTH(DPWM, DPWM_FIFO_DATAWIDTH_16BITS);
#if defined(CONFIG_I96XXX_M0)
		// MODESEL by default in stereo mode
#endif
	}

}


static void dpwm_init(struct dpwm_i9xxxx_cfg_t *cfg_hndl)
{
	struct dev_desc_t	*clk_dev;
	struct dev_desc_t	*src_clock;
	uint32_t sample_rate;
	uint32_t src_clk_rate;
	uint32_t err1;
	uint32_t err2;
	uint8_t clkset;
	uint8_t zohdiv;
	uint16_t clkdiv;
	uint16_t clkdiv2;

	if (init_done)
	{
		return;
	}

	DPWM_MuxPins(cfg_hndl);

	clk_dev = DPWM_CLK_DEV;
	src_clock = cfg_hndl->src_clock;
	sample_rate = cfg_hndl->sample_rate_hz;

	DEV_IOCTL(clk_dev, IOCTL_DEVICE_START);
	DEV_IOCTL(clk_dev,	CLK_IOCTL_SET_PARENT, src_clock);
	DEV_IOCTL(clk_dev,	CLK_IOCTL_GET_FREQ, &src_clk_rate);
	DEV_IOCTL(clk_dev, CLK_IOCTL_ENABLE);

	SYS_ResetModule(DPWM_RST);

	/*
	*  DPWM clk must be 512fs or 500fs (125*4 or 128*4)fs so ZOHDIV = 4
	*  otherwise high THD will be received . ( for example for ZOHDIV = 32
	*  THD will be high )
	*/
	zohdiv = 4;
	err1 = try_to_calclulate_clk_div(sample_rate, src_clk_rate, 500,  &clkdiv);
	err2 = try_to_calclulate_clk_div(sample_rate, src_clk_rate, 512, &clkdiv2);

	if (err1 > err2)
	{
		clkdiv = clkdiv2;
		clkset = 128;
		DPWM_SET_CLKSET(DPWM, DPWM_CLKSET_512FS);
	}
	else
	{
		clkset = 125;
		DPWM_SET_CLKSET(DPWM, DPWM_CLKSET_500FS);
	}

	DPWM_SET_CLOCKDIV(DPWM, clkdiv);
	DPWM_SET_ZOHDIV(DPWM, zohdiv);
	actualSamplingRate = src_clk_rate / (clkset * (clkdiv + 1) * zohdiv);

	set_data_mode(cfg_hndl);
	DPWM_DISABLE_DRIVER(DPWM);
	DPWM_STOP_PLAY(DPWM);

#ifdef DEBUG_USE_INTERRUPT
	irq_register_interrupt(DPWM_IRQn , DPWM_IRQHandler);
	irq_set_priority(DPWM_IRQn , INTERRUPT_PRIORITY_FOR_DPWM );
	irq_enable_interrupt(DPWM_IRQn);
	DPWM_ENABLE_FIFOTHRESHOLDINT(DPWM, 16);
#endif
	init_done = 1;
}


/**
 * dpwm_i9xxxx_ioctl()
 *
 * return:
 */
static uint8_t dpwm_i9xxxx_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1 , void * aIoctl_param2)
{
	struct dpwm_i9xxxx_cfg_t *cfg_hndl;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(dpwm_i9xxxx, adev);
	if ((0 == init_done) && (IOCTL_DEVICE_START != aIoctl_num))
	{
		CRITICAL_ERROR("not initialized yet");
	}

	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		dpwm_init(cfg_hndl);
		break;

	case DPWM_I9XXXX_ENABLE_OUTPUT_IOCTL:
		DPWM_ENABLE_DRIVER(DPWM);
		DPWM_START_PLAY(DPWM);

		break;

	case DPWM_I9XXXX_GET_SAMPLE_RATE_HZ:
		*((uint32_t*)aIoctl_param1) = actualSamplingRate;
		break;

	case DPWM_I9XXXX_GET_ROOT_CLK_DEV:
		DEV_IOCTL(DPWM_CLK_DEV, CLK_IOCTL_GET_ROOT_CLK, aIoctl_param1);
		break;

	default :
		return 1;
	}
	return 0;
}

#define	MODULE_NAME                     dpwm_i9xxxx
#define	MODULE_IOCTL_FUNCTION           dpwm_i9xxxx_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE  struct dpwm_i9xxxx_cfg_t
#define MODULE_HAS_NO_RUNTIME_DATA
#include "add_module.h"
