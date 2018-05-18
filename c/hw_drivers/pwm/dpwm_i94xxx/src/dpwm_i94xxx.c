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

//#define   DEBUG_USE_INTERRUPT

#ifdef 	DEBUG_USE_INTERRUPT
#include "os_wrapper.h"
extern void DPWM_IRQHandler();
//static volatile int g_u32DataCount = 0;
//static volatile int status = 0;
//#define TEST_COUNT	100
//static uint32_t data[TEST_COUNT+1] = {0};
//static volatile int pos = 0;
//void DPWM_IRQHandler()
//{
//    /* Write 2 TX values to TX FIFO */
//  //  I2S_WRITE_TX_FIFO(SPI1, g_u32TxValue);
// //   I2S_WRITE_TX_FIFO(SPI1, g_u32TxValue);
//    if((I2S->STATUS0 & I2S_STATUS0_RXEMPTY_Msk) == 0)
//    {
//		status = I2S->STATUS0;
//		if (pos < TEST_COUNT)
//		{
//			pos++;
//		}
//		data[pos] = I2S->RXFIFO;
//    }
//    g_u32DataCount += 2;
//}
#endif


//extern uint32_t dpwm_actualSamplingRate;
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

uint32_t DPWM_GetSourceClk( void )
{
	uint32_t clkSel, retVal;

	clkSel = (CLK->CLKSEL2 & CLK_CLKSEL2_DPWMSEL_Msk);

	switch (clkSel)
	{
		case CLK_CLKSEL2_DPWMSEL_HXT:
			retVal = __HXT;
		break;

		case CLK_CLKSEL2_DPWMSEL_PLL:
			retVal = CLK_GetPLLClockFreq();
		break;

		case CLK_CLKSEL2_DPWMSEL_PCLK0:
			retVal = CLK_GetPCLK0Freq();
		break;

		case CLK_CLKSEL2_DPWMSEL_HIRC:
			retVal = __HIRC;
		break;

		default:
			return 0;
	}

	return retVal;
}

static uint32_t dpwm_actualSamplingRate;

void 	DPWM_calc_rate(uint32_t samplingRateHz)
{
	uint32_t zohDiv, clkDiv, k, actualSamplingRate, moduleClk, targetClk;
	//DPWM_CLK_TYPE_E clkType;


	//CLK_EnableModuleClock(DPWM_MODULE);

	/* Reset Module */
	//SYS_ResetModule(DPWM_RST);


	/* get clock type according to module clock */
	moduleClk = DPWM_GetSourceClk();

	/* calculate dividers */
	if ( (moduleClk&0x3FF) == 0) // reference clock is a mulltiple of 1024
	{
		//clkType = DPWM_CLK_512_TIMES_FS;
		k = 128;
		//k = 64; // fixing dpwm switching freq issue
		targetClk = (samplingRateHz > 48000) ? (48 * 1024 * 1000) : (24 * 1024 * 1000);
		//targetClk = (samplingRateHz > 48000) ? (48 * 1024 * 1000) : (12 * 1024 * 1000);

	}
	else
	{
		//clkType = DPWM_CLK_500_TIMES_FS;
		k = 125;
		targetClk = (samplingRateHz > 48000) ? (48 * 1000 * 1000) : (24 * 1000 * 1000);
	}

	clkDiv = moduleClk / targetClk - 1;

	zohDiv = (moduleClk / (clkDiv + 1)) / (samplingRateHz * k);

	if (zohDiv < 4)
	{
		//DRV_PRINT("WARNING! ZOH_DIV < 4 (%u)\n",zohDiv);
		actualSamplingRate = 0;

	}


	{
		actualSamplingRate = moduleClk / (clkDiv + 1) / zohDiv / k;
	}

	dpwm_actualSamplingRate = actualSamplingRate;
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

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	src_clock = cfg_hndl->src_clock;


	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
    	/* mux pins */
    	DPWM_MuxPins(cfg_hndl);

		clk_dev = i94xxx_dpwm_clk_dev;

		DEV_IOCTL_1_PARAMS(clk_dev,	CLK_IOCTL_SET_PARENT, src_clock);
		DEV_IOCTL_0_PARAMS(clk_dev, CLK_IOCTL_ENABLE);

		SYS_ResetModule(DPWM_RST);
#if 1
		/* open module */
//		DPWM->CTL &= ~(1 << DPWM_CTL_CLKSET_Pos); //force K=128 for now
		DPWM->CTL |= (1 << DPWM_CTL_CLKSET_Pos); //force K=125 for now

		DPWM_SetSampleRate(cfg_hndl->sample_rate);
        DPWM_calc_rate(cfg_hndl->sample_rate);
        DPWM_ENABLE_FLOAT(DPWM);


//		dpwm_actualSamplingRate = (dpwm_actualSamplingRate * 125) / 128;
#else
		/* Set DPWM sampling rate */
		// HIRC=48MHz,Fs=24.576MHz/(128x4)=48kHz.
		DPWM_SET_CLKSET(DPWM, DPWM_CLKSET_500FS);
		DPWM_SetSampleRate(cfg_hndl->sample_rate); //Set sample rate
		/* Set Datawidth */
		DPWM_SET_FIFODATAWIDTH(DPWM, DPWM_FIFO_DATAWIDTH_16BITS);

#endif

		DPWM_DISABLE_DRIVER(DPWM);
		DPWM_STOP_PLAY(DPWM);


#ifdef DEBUG_USE_INTERRUPT
		irq_register_interrupt(DPWM_IRQn , DPWM_IRQHandler);
		irq_set_priority(DPWM_IRQn , OS_MAX_INTERRUPT_PRIORITY_FOR_API_CALLS );
		irq_enable_interrupt(DPWM_IRQn);
		DPWM_ENABLE_FIFOTHRESHOLDINT(DPWM, 16);

#endif
	   // I2S_ENABLE_RX(I2S_module);
//		I2S_module->FIFOCTL |= I2S_FIFO_RX_LEVEL_WORD_4;
//	    I2S_module->FIFOCTL |= SPI_FIFOCTL_RXRST_Msk;
		break;

	case DPWM_I94XXX_ENABLE_OUTPUT_IOCTL:
		DPWM_ENABLE_DRIVER(DPWM);
		DPWM_START_PLAY(DPWM);
#ifndef DEBUG_USE_INTERRUPT
		DPWM_ENABLE_PDMA(DPWM);
#endif
		break;
//	case DPWM_I94XXX_DEVICE_STOP :
//
//		// put module to reset
//	    *(volatile uint32_t *)((uint32_t)&SYS->IPRST0 + (DPWM_RST >> 24)) |=
//	    		1 << (DPWM_RST & 0x00ffffff);
//	    break;

	case DPWM_I94XXX_GET_MEASURED_SAMPLE_RATE:
		//*((uint32_t*)aIoctl_param1) = dpwm_actualSamplingRate;
		break;

	default :
		return 1;
	}
	return 0;
}
