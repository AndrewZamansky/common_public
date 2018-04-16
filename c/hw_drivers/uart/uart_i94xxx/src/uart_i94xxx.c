/*
 *
 * uart_i94xxx.c

 *
 */

/********  includes *********************/

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "uart_i94xxx.h"
#include "I94100.h"

#include "irq_api.h"
#include "uart_api.h"
#include "clock_control_i94xxx_api.h"

#include "_uart_i94xxx_prerequirements_check.h"

/*following line add module to available module list for dynamic device tree*/
#include "uart_i94xxx_add_component.h"

/********  defines *********************/


/********  types  *********************/


/* ------------ External variables ---------------------------------*/

/* ------------------------ External functions ------------*/

/* ------------------------ Exported variables --------*/



uint8_t uart_i94xxx_callback(struct dev_desc_t *adev ,
		uint8_t aCallback_num , void * aCallback_param1,
		void * aCallback_param2)
{
	struct uart_i94xxx_cfg_t *cfg_hndl;

    uint8_t u8InChar;
    uint32_t u32IntSts ;
	UART_T *uart_regs;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	uart_regs =(UART_T *)cfg_hndl->base_address;

	u32IntSts = uart_regs->INTSTS;

	if(u32IntSts & UART_INTSTS_RDAINT_Msk)
	{
		struct dev_desc_t * callback_rx_dev ;
		callback_rx_dev = cfg_hndl->callback_rx_dev;
		u8InChar = UART_READ(uart_regs);
		if (NULL != callback_rx_dev)
		{
			DEV_CALLBACK_2_PARAMS(callback_rx_dev,
					CALLBACK_DATA_RECEIVED,  &u8InChar,  1);
		}
	}

	if(u32IntSts & UART_INTSTS_THREINT_Msk)
	{
		struct dev_desc_t * callback_tx_dev ;
		callback_tx_dev = cfg_hndl->callback_tx_dev;
		if (NULL != callback_tx_dev)
		{
			DEV_CALLBACK_1_PARAMS(callback_tx_dev , CALLBACK_TX_DONE, (void*)1);
		}
   }

    return 0;
}


/**
 * uart_i94xxx_pwrite()
 *
 * return:
 */
size_t uart_i94xxx_pwrite(struct dev_desc_t *adev,
			const uint8_t *apData, size_t aLength, size_t aOffset)
{
	UART_T *uart_regs;
	struct uart_i94xxx_cfg_t *cfg_hndl;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	uart_regs =(UART_T *)cfg_hndl->base_address;

	UART_WRITE(uart_regs, *apData);
    UART_EnableInt(uart_regs,  UART_INTEN_THREIEN_Msk );

	return 1;

}


/**
 * uart_i94xxx_ioctl()
 *
 * return:
 */
uint8_t uart_i94xxx_ioctl( struct dev_desc_t *adev, uint8_t aIoctl_num,
		void * aIoctl_param1, void * aIoctl_param2)
{
	struct uart_i94xxx_cfg_t *cfg_hndl;
	UART_T *uart_regs;
	int uart_irq;
	uint32_t uart_module_rst;
	struct dev_desc_t	*src_clock;
	struct dev_desc_t	*uart_clk_dev;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	uart_regs = (UART_T *)cfg_hndl->base_address;
	src_clock = cfg_hndl->src_clock;
	switch(aIoctl_num)
	{
	case IOCTL_UART_SET_BAUD_RATE :
		cfg_hndl->baud_rate = *(uint32_t*)aIoctl_param1;
		break;
	case IOCTL_DEVICE_START :
		if(UART0 == uart_regs)
		{
			uart_irq = UART0_IRQn;
			uart_module_rst = UART0_MODULE;
			uart_clk_dev = i94xxx_uart0clk_clk_dev;
			//uart_clk_src = CLK_UART0_SRC_EXT;
			/*
			* Init I/O Multi-function
			* Set PA multi-function pins for UART0 RXD(PA.8) and TXD(PA.7)
			*/
			switch (cfg_hndl->pinout)
			{
			case UART_I94XXX_UART0_TX_RX_PINS_PORT_B_PINS_8_9:
			    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB8MFP_Msk);
			    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB9MFP_Msk);
			    SYS->GPB_MFPH |=  (SYS_GPB_MFPH_PB8MFP_UART0_TXD);
			    SYS->GPB_MFPH |=  (SYS_GPB_MFPH_PB9MFP_UART0_RXD);
				break;
			case UART_I94XXX_UART0_TX_RX_PINS_PORT_D_PINS_14_15:
				  SYS->GPD_MFPH = (SYS->GPD_MFPH & (~0xFF000000)) | 0x44000000;
				break;
			default :
				return 1;

			}
		}
		else
		{
			return 1;
		}

		/*set in clock dev*/
//				/* Select IP clock source */
//				CLK_SetModuleClock(uart_module_num, uart_clk_src, 0);

		DEV_IOCTL_1_PARAMS(uart_clk_dev, CLK_IOCTL_SET_PARENT, src_clock);
		DEV_IOCTL_0_PARAMS(uart_clk_dev, CLK_IOCTL_ENABLE);

		SYS_ResetModule(uart_module_rst);

		/* Configure UART and set UART baud rate */
		UART_Open(uart_regs, cfg_hndl->baud_rate);

		/* Enable UART RDA/RLS/Time-out interrupt */
		UART_EnableInt(uart_regs,
				(UART_INTEN_RDAIEN_Msk  | UART_INTEN_RXTOIEN_Msk));

		irq_register_device_on_interrupt(uart_irq, adev);
		irq_set_priority(uart_irq, INTERRUPT_LOWEST_PRIORITY - 1 );
		irq_enable_interrupt(uart_irq);
		break;

	case IOCTL_UART_DISABLE_TX :
		UART_DISABLE_INT(uart_regs,  UART_INTEN_THREIEN_Msk);
		break;

	case IOCTL_UART_ENABLE_TX :
		break;

	case IOCTL_UART_SET_ISR_CALLBACK_TX_DEV:
		cfg_hndl->callback_tx_dev =(struct dev_desc_t *) aIoctl_param1;
		break;

	case IOCTL_UART_SET_ISR_CALLBACK_RX_DEV:
		cfg_hndl->callback_rx_dev =(struct dev_desc_t *) aIoctl_param1;
		break;

	default :
		return 1;
	}
	return 0;
}
