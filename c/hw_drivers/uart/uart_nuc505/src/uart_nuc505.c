/*
 *
 * uart_nuc505.c
 *
 *
 *
 *
 *
 */

/********  includes *********************/

#include "src/_uart_nuc505_prerequirements_check.h"

#include "uart_nuc505.h"


#include "NUC505Series.h"

#include "sw_uart_wrapper_api.h"
#include "irq_api.h"

#include "uart_nuc505_add_component.h"

/********  defines *********************/


/********  types  *********************/


/* ---------------------------- External variables ---------------------------------*/

/* ------------------------ External functions ------------------------------*/

/* ------------------------ Exported variables ------------------------------*/


/********  local defs *********************/



uint8_t uart_nuc505_callback(pdev_descriptor_t apdev ,
		const uint8_t aCallback_num , void * aCallback_param1, void * aCallback_param2)
{
	uart_nuc505_instance_t *config_handle;
	pdev_descriptor_t callback_dev ;

    uint8_t u8InChar;
    uint32_t u32IntSts ;
	UART_T *uart_regs;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(apdev);
	uart_regs =(UART_T *)config_handle->base_address;
	callback_dev = config_handle->callback_dev;

	u32IntSts = uart_regs->INTSTS;
	if (NULL != callback_dev)
	{
		if(u32IntSts & UART_INTSTS_RDAINT_Msk)
		{
				u8InChar = UART_READ(uart_regs);
				DEV_CALLBACK_2_PARAMS(callback_dev , CALLBACK_DATA_RECEIVED,  &u8InChar, (void*)1);
		}

		if(u32IntSts & UART_INTSTS_THREINT_Msk)
		{
			DEV_CALLBACK_1_PARAMS( callback_dev , CALLBACK_TX_DONE,(void*)1);
	   }
	}
    return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        uart_nuc505_pwrite                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
 size_t uart_nuc505_pwrite(pdev_descriptor_t apdev ,const uint8_t *apData , size_t aLength, size_t aOffset)
{
	UART_T *uart_regs;
	uart_nuc505_instance_t *config_handle;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(apdev);
	uart_regs =(UART_T *)config_handle->base_address;

	UART_WRITE(uart_regs, *apData);
    UART_EnableInt(uart_regs,  UART_INTEN_THREIEN_Msk );

	return 1;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        uart_nuc505_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t uart_nuc505_ioctl( pdev_descriptor_t apdev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	uart_nuc505_instance_t *config_handle;
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
				uint32_t uart_clk_src;
				uint32_t uart_module_rst;

				if(UART0 == uart_regs)
				{
					uart_irq = UART0_IRQn;
					uart_module_num = UART0_MODULE;
					uart_clk_src = CLK_UART0_SRC_EXT;
					uart_module_rst = UART0_RST;
					/*---------------------------------------------------------------------------------------------------------*/
					/* Init I/O Multi-function                                                                                 */
					/*---------------------------------------------------------------------------------------------------------*/
					/* Configure multi-function pins for UART0 RXD and TXD */
					SYS->GPB_MFPL  = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB0MFP_Msk) ) | SYS_GPB_MFPL_PB0MFP_UART0_TXD;
					SYS->GPB_MFPL  = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB1MFP_Msk) ) | SYS_GPB_MFPL_PB1MFP_UART0_RXD;

				}
				else
				{
					return 1;
				}

				/* Select IP clock source */
				CLK_SetModuleClock(uart_module_num, uart_clk_src, 0);

				/* Enable IP clock */
				CLK_EnableModuleClock(uart_module_num);

				/*---------------------------------------------------------------------------------------------------------*/
				/* Init UART                                                                                               */
				/*---------------------------------------------------------------------------------------------------------*/
				/* Reset UART module */
				SYS_ResetModule(uart_module_rst);

				/* Configure UART and set UART baud rate */
				UART_Open(uart_regs, config_handle->baud_rate);

				/* Enable UART RDA/RLS/Time-out interrupt */
				UART_EnableInt(uart_regs, (UART_INTEN_RDAIEN_Msk  | UART_INTEN_RXTOIEN_Msk));


				irq_register_device_on_interrupt(uart_irq , apdev);
				irq_set_priority(uart_irq , INTERRUPT_LOWEST_PRIORITY - 1 );
				irq_enable_interrupt(uart_irq);
			}
			break;
		case IOCTL_UART_DISABLE_TX :
			UART_DISABLE_INT(uart_regs,  UART_INTEN_THREIEN_Msk);
			break;
		case IOCTL_UART_ENABLE_TX :
		    break;
		case IOCTL_SET_ISR_CALLBACK_DEV:
			config_handle->callback_dev =(pdev_descriptor_t) aIoctl_param1;
			break;
		default :
			return 1;
	}
	return 0;
}
