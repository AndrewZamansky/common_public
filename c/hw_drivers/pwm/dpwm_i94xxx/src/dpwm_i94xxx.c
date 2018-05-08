/*
 *
 * dpwm_i94xxx.c
 *
 */

/********  includes *********************/

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "dpwm_i94xxx_api.h"
#include "dpwm_i94xxx.h"
#include "irq_api.h"
#include "timer_wrapper_api.h"


#include "I94100.h"


#include "clock_control_i94xxx_api.h"
#include "dpwm.h"

#include "_dpwm_i94xxx_prerequirements_check.h"

/*following line add module to available module list for dynamic device tree*/
#include "dpwm_i94xxx_add_component.h"

/********  defines *********************/


/********  types  *********************/

/* ------------- External variables --------------------------*/

/* ------------------------ External functions --------------*/

/* ------------------------ Exported variables ---------------*/

//#define 	 DEBUG_USE_INTERRUPT

#ifdef 	DEBUG_USE_INTERRUPT
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


//extern uint32_t actualSamplingRate;
#define SYS_GPA_MFPL_PA4MFP_DPWM_MINUS_L     (0x03UL<<SYS_GPA_MFPL_PA4MFP_Pos)
#define SYS_GPA_MFPL_PA5MFP_DPWM_PLUS_L      (0x03UL<<SYS_GPA_MFPL_PA5MFP_Pos)
#define SYS_GPA_MFPH_PA10MFP_DPWM_MINUS_R     (0x03UL<<SYS_GPA_MFPH_PA10MFP_Pos)
#define SYS_GPA_MFPH_PA11MFP_DPWM_PLUS_R      (0x03UL<<SYS_GPA_MFPH_PA11MFP_Pos)

// on some sources appear PA9 PA10
#define SYS_GPA_MFPL_PA10MFP_DPWM_MINUS_R    (0x03UL<<SYS_GPA_MFPL_PA10MFP_Pos)
#define SYS_GPA_MFPL_PA11MFP_DPWM_PLUS_R     (0x03UL<<SYS_GPA_MFPL_PA11MFP_Pos)

void DPWM_MuxPins(struct dpwm_i94xxx_cfg_t *cfg_hndl)
{
	uint8_t ioPort;

	ioPort = cfg_hndl->left_ch_io_bank;
	if (ioPort == DPWM_IO_ON_GPA)
	{
		SYS->GPA_MFPL &= ~(SYS_GPA_MFPL_PA4MFP_Msk | SYS_GPA_MFPL_PA5MFP_Msk);
		SYS->GPA_MFPL |= SYS_GPA_MFPL_PA4MFP_DPWM_MINUS_L;
		SYS->GPA_MFPL |= SYS_GPA_MFPL_PA5MFP_DPWM_PLUS_L ;
	}
	else if (ioPort == DPWM_IO_ON_GPC)
	{
		SYS->GPC_MFPH &= ~(SYS_GPC_MFPH_PC12MFP_Pos | SYS_GPC_MFPH_PC13MFP_Pos);
		SYS->GPC_MFPH |= SYS_GPC_MFPH_PC12MFP_DPWM_LN;
		SYS->GPC_MFPH |= SYS_GPC_MFPH_PC13MFP_DPWM_LP ;
	}
	else if (ioPort == DPWM_IO_ON_GPD)
	{
		SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD5MFP_Pos | SYS_GPD_MFPL_PD5MFP_Pos);
		SYS->GPD_MFPL |= SYS_GPD_MFPL_PD5MFP_DPWM0_N;
		SYS->GPD_MFPL |= SYS_GPD_MFPL_PD6MFP_DPWM0_P ;
	}

	ioPort = cfg_hndl->right_ch_io_bank;
	if (ioPort == DPWM_IO_ON_GPA)
	{
		SYS->GPA_MFPH &= ~(SYS_GPA_MFPH_PA10MFP_Msk | SYS_GPA_MFPH_PA11MFP_Msk);
		SYS->GPA_MFPH |= SYS_GPA_MFPH_PA10MFP_DPWM_MINUS_R;
		SYS->GPA_MFPH |= SYS_GPA_MFPH_PA11MFP_DPWM_PLUS_R;
	}
	else if (ioPort == DPWM_IO_ON_GPC)
	{
		SYS->GPC_MFPH &= ~(SYS_GPC_MFPH_PC10MFP_Pos | SYS_GPC_MFPH_PC11MFP_Pos);
		SYS->GPC_MFPH |= SYS_GPC_MFPH_PC10MFP_DPWM_RN;
		SYS->GPC_MFPH |= SYS_GPC_MFPH_PC11MFP_DPWM_RP ;
	}
	else if (ioPort == DPWM_IO_ON_GPD)
	{
		SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD0MFP_Pos | SYS_GPD_MFPL_PD1MFP_Pos);
		SYS->GPD_MFPL |= SYS_GPD_MFPL_PD0MFP_DPWM1_N;
		SYS->GPD_MFPL |= SYS_GPD_MFPL_PD1MFP_DPWM1_P ;
	}

	ioPort = cfg_hndl->sub_ch_io_bank;
	if (ioPort == DPWM_IO_ON_GPA)
	{
//				_SYS_GPA_MFPH_PA14(SYS_GPA_MFPH_PA14MFP_DPWM_PLUS_S);
//				_SYS_GPA_MFPH_PA13(SYS_GPA_MFPH_PA13MFP_DPWM_MINUS_S);
	}
	else if (ioPort == DPWM_IO_ON_GPC)
	{
//		_SYS_GPC_MFPH_PC15(SYS_GPC_MFPH_PC15MFP_DPWM_PLUS_S);
//		_SYS_GPC_MFPH_PC14(SYS_GPC_MFPH_PC14MFP_DPWM_MINUS_S);
	}
	else if (ioPort == DPWM_IO_ON_GPD)
	{
//		_SYS_GPD_MFPH_PD9(SYS_GPD_MFPH_PD9MFP_DPWM_PLUS_S);
//		_SYS_GPD_MFPH_PD8(SYS_GPD_MFPH_PD8MFP_DPWM_MINUS_S);
	}

}



static uint32_t actualSamplingRate;



uint32_t try_to_calclulate_clk_div(uint32_t needed_sample_rate,
		uint32_t src_clk_rate, uint8_t clkset,
		uint8_t *zohdiv, uint16_t *clkdiv)
{
	uint16_t l_clkdiv;
	uint8_t l_zohdiv;
	uint32_t total_div;
	uint32_t u32Error1;
	uint32_t u32Error2;

	total_div = (src_clk_rate / needed_sample_rate) / clkset;

	/* Adjust Error */
	u32Error1 = ((src_clk_rate / clkset) / total_div) - needed_sample_rate;
	u32Error2 = needed_sample_rate - ((src_clk_rate / clkset)/(total_div + 1));
	if (u32Error1 > u32Error2)
	{
		total_div = total_div + 1;
		u32Error1 = u32Error2;
	}

	/*
	 * according to dpem spec, clocks src_clk/clkdiv should be 24 or 24.576
	 * so for 196608000 l_zohdiv=4 and l_clkdiv = 7 . so total_div = 4*(7+1)
	 * l_zohdiv=32  l_clkdiv = 0 should also give the same result BUT measured
	 * THD is very high . maybe because of HW implementation
	 */
	if(needed_sample_rate == 48000 || needed_sample_rate == 96000)
	{
		l_zohdiv = 4;
		l_clkdiv =
				src_clk_rate / ((clkset * l_zohdiv) * needed_sample_rate) - 1;
	}
	else
	{
		if ((total_div >= DPWM_ZOHDIV_MIN))
		{
			if(total_div <= DPWM_ZOHDIV_MAX)
			{
				l_zohdiv = total_div;
				l_clkdiv = 0;
			}
			else
			{
				l_clkdiv = 0;
				do
				{
					l_clkdiv++;
					l_zohdiv = total_div / (l_clkdiv + 1);
				}
				while(l_zohdiv > DPWM_ZOHDIV_MAX);
			}
		}
		else
		{
			l_zohdiv = DPWM_ZOHDIV_MIN;
			l_clkdiv = 0;
		}
	}
	*clkdiv = l_clkdiv;
	*zohdiv = l_zohdiv;

	return u32Error1;
}


/**
 * dpwm_i94xxx_ioctl()
 *
 * return:
 */
uint8_t dpwm_i94xxx_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	struct dpwm_i94xxx_cfg_t *cfg_hndl;
	struct dev_desc_t	*clk_dev;
	struct dev_desc_t	*src_clock;
	uint32_t sample_rate;
	uint32_t src_clk_rate;
	uint32_t err1;
	uint32_t err2;
	uint8_t clkset;
	uint8_t zohdiv;
	uint16_t clkdiv;
	uint8_t zohdiv2;
	uint16_t clkdiv2;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	src_clock = cfg_hndl->src_clock;
	sample_rate = cfg_hndl->sample_rate;

	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
    	/* mux pins */
    	DPWM_MuxPins(cfg_hndl);

		clk_dev = i94xxx_dpwm_clk_dev;

		DEV_IOCTL_1_PARAMS(clk_dev,	CLK_IOCTL_SET_PARENT, src_clock);
		DEV_IOCTL_1_PARAMS(clk_dev,	CLK_IOCTL_GET_FREQ, &src_clk_rate);
		DEV_IOCTL_0_PARAMS(clk_dev, CLK_IOCTL_ENABLE);

		SYS_ResetModule(DPWM_RST);

		err1 = try_to_calclulate_clk_div(
				sample_rate, src_clk_rate, 125, &zohdiv, &clkdiv);

		err2 = try_to_calclulate_clk_div(
				sample_rate, src_clk_rate, 128, &zohdiv2, &clkdiv2);

		if (err1 > err2)
		{
			zohdiv = zohdiv2;
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

		DPWM_ENABLE_FLOAT(DPWM);

		DPWM_DISABLE_DRIVER(DPWM);
		DPWM_STOP_PLAY(DPWM);

#ifdef DEBUG_USE_INTERRUPT
		irq_register_interrupt(DPWM_IRQn , DPWM_IRQHandler);
		irq_set_priority(DPWM_IRQn , OS_MAX_INTERRUPT_PRIORITY_FOR_API_CALLS );
		irq_enable_interrupt(DPWM_IRQn);
		DPWM_InterruptEnable(1);

#endif
	   // I2S_ENABLE_RX(I2S_module);
//		I2S_module->FIFOCTL |= I2S_FIFO_RX_LEVEL_WORD_4;
//	    I2S_module->FIFOCTL |= SPI_FIFOCTL_RXRST_Msk;
		break;

	case DPWM_I94XXX_ENABLE_OUTPUT_IOCTL:
		DPWM_ENABLE_DRIVER(DPWM);
		DPWM_START_PLAY(DPWM);
		DPWM_ENABLE_PDMA(DPWM);
		break;

	case DPWM_I94XXX_GET_MEASURED_SAMPLE_RATE:
		//*((uint32_t*)aIoctl_param1) = actualSamplingRate;
		break;

	default :
		return 1;
	}
	return 0;
}
