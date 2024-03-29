/*
 *
 * uart_stm8.c
 *
 *
 */


#include "uart_api.h"

#include "uart_stm8.h"

#include "stm8s.h"
#include "arch.h"

#include "stm8s_uart1.h"
#include "uart_stm8_api.h"


#include "uart_stm8_add_component.h"


static struct uart_stm8_instance_t *pHw_uart_pointer_to_instance=NULL;

void tx_function(void)
{
	/* In order to detect unexpected events during development,
   	   it is recommended to set a breakpoint on the following instruction.
	 */

	if (UART1_GetFlagStatus(UART1_FLAG_TXE) == SET)
	{
		struct dev_desc_t *   callback_tx_dev;
		callback_tx_dev = pHw_uart_pointer_to_instance->callback_tx_dev ;
		if(NULL != callback_tx_dev)
		{
			DEV_CALLBACK(callback_tx_dev , CALLBACK_TX_DONE, 1);
		}
	}
}

//struct dev_desc_t *   stm8_callback_dev;

/**
  * @brief UART1 TX Interrupt routine.
  * @param  None
  * @retval None
  */


INTERRUPT_HANDLER(UART1_TX_IRQHandler, 17)
 {
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */


	 arch_enter_interrupt();

		tx_function();

		arch_exit_interrupt(FALSE);

 }



void rx_function(void)
{
	/* In order to detect unexpected events during development,
	*it is recommended to set a breakpoint on the following instruction.
	 */

	if (UART1_GetFlagStatus(UART1_FLAG_RXNE) == SET)
	{
		struct dev_desc_t *   callback_rx_dev;
		uint8_t cChar;

		cChar = UART1_ReceiveData8();
		callback_rx_dev = pHw_uart_pointer_to_instance->callback_rx_dev;
		if (NULL == callback_rx_dev)  return ;

		DEV_CALLBACK( callback_rx_dev, CALLBACK_DATA_RECEIVED,  &cChar,  1);

	}
}


/**
  * @brief UART1 RX Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(UART1_RX_IRQHandler, 18)
 {
	 arch_enter_interrupt();

	 rx_function();

	 arch_exit_interrupt(FALSE);
 }


/*------------------------------------------------------------------------
 * Function:        uart_stm8_pwrite
 * Parameters:
 * Returns:
 *------------------------------------------------------------------------
 */
 size_t uart_stm8_pwrite(const void *aHandle,
		 const uint8_t *apData , size_t aLength, size_t aOffset)
{
	UART1_SendData8(*apData);
	UART1_ITConfig( UART1_IT_TXE, ENABLE );

	return 1;
}


/*----------------------------------------------------------------------
 * Function:        uart_stm8_ioctl
 * Parameters:
 * Returns:
 *----------------------------------------------------------------------
 */
uint8_t uart_stm8_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	struct uart_stm8_instance_t *config_handle;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
		case IOCTL_UART_SET_BAUD_RATE :
			config_handle->baud_rate = *(uint32_t*)aIoctl_param1;
			break;
		case IOCTL_DEVICE_START :
			UART1_DeInit();
			UART1_Init (config_handle->baud_rate, UART1_WORDLENGTH_8D,
					UART1_STOPBITS_1, UART1_PARITY_NO,
					UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
			pHw_uart_pointer_to_instance = config_handle;
			GPIO_Init(GPIOD,GPIO_PIN_6,GPIO_MODE_IN_PU_NO_IT);
			UART1_ITConfig( UART1_IT_RXNE, ENABLE );
			break;
		case IOCTL_UART_DISABLE_TX :
			UART1_ITConfig( UART1_IT_TXE, DISABLE );
			break;
		case IOCTL_UART_ENABLE_TX :
			//UART1_ITConfig( UART1_IT_TXE, ENABLE );
			break;
#ifdef CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION
		case IOCTL_SET_ISR_CALLBACK_DEV:
			stm8_callback_dev =(struct dev_desc_t *) aIoctl_param1;
			break;
#endif
		default :
			return 1;
	}
	return 0;
}



/* COSMIC: Requires putchar() routine to override stdio */
#if defined(__CSMC__)
/**
 * \b putchar
 *
 * Retarget putchar() to use UART2
 *
 * @param[in] c Character to send
 *
 * @return Character sent
 */
char putchar (char c)
{
	return c; // az
 //   return (uart_putchar(c));
}
#endif /* __CSMC__ */


/* RAISONANCE: Requires putchar() routine to override stdio */
#if defined(__RCSTM8__)
/**
 * \b putchar
 *
 * Retarget putchar() to use UART2
 *
 * @param[in] c Character to send
 *
 * @return 1 on success
 */
int putchar (char c)
{
	uart_putchar(c);
	return (1);
}
#endif /* __RCSTM8__ */


/* IAR: Requires __write() routine to override stdio */
#if defined(__IAR_SYSTEMS_ICC__)
/**
 * \b __write
 *
 * Override for IAR stream output
 *
 * @param[in] handle Stdio handle. -1 to flush.
 * @param[in] buf Pointer to buffer to be written
 * @param[in] bufSize Number of characters to be written
 *
 * @return Number of characters sent
 */
size_t __write(int handle, const unsigned char *buf, size_t bufSize)
{
	size_t chars_written = 0;

	/* Ignore flushes */
	if (handle == -1)
	{
		chars_written = (size_t)0;
	}
	/* Only allow stdout/stderr output */
	else if ((handle != 1) && (handle != 2))
	{
		chars_written = (size_t)-1;
	}
	/* Parameters OK, call the low-level character output routine */
	else
	{
		while (chars_written < bufSize)
		{
			uart_putchar (buf[chars_written]);
			chars_written++;
		}
	}

	return (chars_written);
}
#endif /* __IAR_SYSTEMS_ICC__ */
