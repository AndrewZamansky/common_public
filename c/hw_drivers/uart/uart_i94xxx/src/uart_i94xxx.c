/*
 *
 * uart_i94xxx.c

 *
 */

/********  includes *********************/

#include "src/_uart_i94xxx_prerequirements_check.h"

#include "uart_i94xxx.h"


#include "ISD94XXXSeries.h"

#include "irq_api.h"

#include "uart_api.h"

#include "uart_i94xxx_add_component.h"

/********  defines *********************/


/********  types  *********************/


/* ------------ External variables ---------------------------------*/

/* ------------------------ External functions ------------*/

/* ------------------------ Exported variables --------*/



uint8_t uart_i94xxx_callback(pdev_descriptor_t apdev ,
		const uint8_t aCallback_num , void * aCallback_param1,
		void * aCallback_param2)
{
	uart_i94xxx_instance_t *config_handle;

    uint8_t u8InChar;
    uint32_t u32IntSts ;
	UART_T *uart_regs;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(apdev);
	uart_regs =(UART_T *)config_handle->base_address;

	u32IntSts = uart_regs->INTSTS;

	if(u32IntSts & UART_INTSTS_RDAINT_Msk)
	{
		pdev_descriptor_t callback_rx_dev ;
		callback_rx_dev = config_handle->callback_rx_dev;
		u8InChar = UART_READ(uart_regs);
		if (NULL != callback_rx_dev)
		{
			DEV_CALLBACK_2_PARAMS(callback_rx_dev,
					CALLBACK_DATA_RECEIVED,  &u8InChar,  1);
		}
	}

	if(u32IntSts & UART_INTSTS_THREINT_Msk)
	{
		pdev_descriptor_t callback_tx_dev ;
		callback_tx_dev = config_handle->callback_tx_dev;
		if (NULL != callback_tx_dev)
		{
			DEV_CALLBACK_1_PARAMS( callback_tx_dev , CALLBACK_TX_DONE, 1);
		}
   }

    return 0;
}


/**
 * uart_i94xxx_pwrite()
 *
 * return:
 */
size_t uart_i94xxx_pwrite(pdev_descriptor_t apdev,
			const uint8_t *apData, size_t aLength, size_t aOffset)
{
	UART_T *uart_regs;
	uart_i94xxx_instance_t *config_handle;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(apdev);
	uart_regs =(UART_T *)config_handle->base_address;

	UART_WRITE(uart_regs, *apData);
    UART_EnableInt(uart_regs,  UART_INTEN_THREIEN_Msk );

	return 1;

}


/**
 * uart_i94xxx_ioctl()
 *
 * return:
 */
uint8_t uart_i94xxx_ioctl( pdev_descriptor_t apdev, const uint8_t aIoctl_num,
		void * aIoctl_param1, void * aIoctl_param2)
{
	uart_i94xxx_instance_t *config_handle;
	UART_T *uart_regs;
	int uart_irq;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(apdev);
	uart_regs =(UART_T *)config_handle->base_address;
	switch(aIoctl_num)
	{
	case IOCTL_UART_SET_BAUD_RATE :
		config_handle->baud_rate = *(uint32_t*)aIoctl_param1;
		break;
	case IOCTL_DEVICE_START :
		{
			uint32_t uart_module_num;
			//uint32_t uart_clk_src;
			uint32_t uart_module_rst;

			if(UART0 == uart_regs)
			{
				uart_irq = UART0_IRQn;
				uart_module_num = UART0_MODULE;
				//uart_clk_src = CLK_UART0_SRC_EXT;
				uart_module_rst = UART0_RST;

				/*
				* Init I/O Multi-function
				* Set PA multi-function pins for UART0 RXD(PA.8) and TXD(PA.7)
				*/
				SYS->GPA_MFPL &= ~(SYS_GPA_MFPL_PA7MFP_Msk);
				SYS->GPA_MFPH &= ~(SYS_GPA_MFPH_PA8MFP_Msk);
				SYS->GPA_MFPL |= (SYS_GPA_MFPL_PA7MFP_UART0_TXD);
				SYS->GPA_MFPH |= (SYS_GPA_MFPH_PA8MFP_UART0_RXD);

			}
			else
			{
				return 1;
			}

			/*set in clock dev*/
//				/* Select IP clock source */
//				CLK_SetModuleClock(uart_module_num, uart_clk_src, 0);

			/* Enable IP clock */
			CLK_EnableModuleClock(uart_module_num);

			/* Reset UART module */
			SYS_ResetModule(uart_module_rst);

			/* Configure UART and set UART baud rate */
			UART_Open(uart_regs, config_handle->baud_rate);

			/* Enable UART RDA/RLS/Time-out interrupt */
			UART_EnableInt(uart_regs,
					(UART_INTEN_RDAIEN_Msk  | UART_INTEN_RXTOIEN_Msk));

			irq_register_device_on_interrupt(uart_irq, apdev);
			irq_set_priority(uart_irq, INTERRUPT_LOWEST_PRIORITY - 1 );
			irq_enable_interrupt(uart_irq);
		}
		break;
	case IOCTL_UART_DISABLE_TX :
		UART_DISABLE_INT(uart_regs,  UART_INTEN_THREIEN_Msk);
		break;
	case IOCTL_UART_ENABLE_TX :
		break;
	case IOCTL_UART_SET_ISR_CALLBACK_TX_DEV:
		config_handle->callback_tx_dev =(pdev_descriptor_t) aIoctl_param1;
		break;
	case IOCTL_UART_SET_ISR_CALLBACK_RX_DEV:
		config_handle->callback_rx_dev =(pdev_descriptor_t) aIoctl_param1;
		break;
	default :
		return 1;
	}
	return 0;
}
