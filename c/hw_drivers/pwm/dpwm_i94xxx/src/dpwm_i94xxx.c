/*
 *
 * dpwm_i94xxx.c
 *
 */

/********  includes *********************/

#include "src/_dpwm_i94xxx_prerequirements_check.h"

#include "dpwm_i94xxx_api.h"
#include "dpwm_i94xxx.h"
#include "irq_api.h"
#include "timer_wrapper_api.h"


#include "ISD94XXXSeries.h"

#include "dpwm_i94xxx_add_component.h"

#include "clock_control_i94xxx_api.h"
#include "dpwm.h"

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
    	DPWM_MuxPins(DPWM_CHAN_LEFT, DPWM_IO_ON_GPC);
    	DPWM_MuxPins(DPWM_CHAN_RIGHT, DPWM_IO_ON_GPC);
    	DPWM_MuxPins(DPWM_CHAN_SUB, DPWM_IO_ON_GPC);

		clk_dev = i94xxx_dpwm_clk_dev;

		DEV_IOCTL_1_PARAMS(clk_dev,	CLK_IOCTL_SET_PARENT, src_clock);
		DEV_IOCTL_0_PARAMS(clk_dev, CLK_IOCTL_ENABLE);


		/* open module */
		DPWM_Open(cfg_hndl->sample_rate,
				DPWM_FORMAT_FLOATING_PNT, DPWM_WIDTH_24BIT_LSB);


	    DPWM_DISABLE();

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

	case DPWM_ENABLE_OUTPUT_IOCTL:
		DPWM_ENABLE();
		DPWM_DMA_ENABLE();
		break;

	default :
		return 1;
	}
	return 0;
}
