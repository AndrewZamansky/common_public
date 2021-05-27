/*
 *
 * uart_atmega.c
 *
 */

/********  includes *********************/

#include "uart_atmega_config.h"
#include "dev_management_api.h" // for device manager defines and typedefs
#include "uart_api.h"

#include <avr/io.h>

#include "uart_atmega.h"


//#include "atom.h"
//#include "atommutex.h"
//#include "uart.h"
#include "uart_atmega_api.h"

#include "sw_uart_wrapper_api.h"

/********  defines *********************/

#ifdef UCSRA
/* Device with single UART */
#define REG_UCSRA   UCSRA
#define REG_UCSRB   UCSRB
#define REG_UBRRL   UBRRL
#define REG_UDR     UDR
#define BIT_TXEN    TXEN
#define BIT_RXEN    RXEN
#define BIT_UDRE    UDRE
#else
/* Device with multiple UARTs */
#define REG_UCSRA   UCSR0A
#define REG_UCSRB   UCSR0B
#define REG_UBRRL   UBRR0L
#define REG_UDR     UDR0
#define BIT_TXEN    TXEN0
#define BIT_RXEN    RXEN0
#define BIT_UDRE    UDRE0
#endif

#define AVR_CPU_HZ   8000000

#define INSTANCE(hndl)  ((struct UART_ATMEGA_Instance_t*)hndl)


#if UART_ATMEGA_CONFIG_NUM_OF_DYNAMIC_INSTANCES > 0
	struct UART_ATMEGA_Instance_t UART_ATMEGA_Instance;
#endif

static struct UART_ATMEGA_Instance_t *pHw_uart_pointer_to_instance = NULL;

void tx_function(void)
{
	/* In order to detect unexpected events during development,
   	 * it is recommended to set a breakpoint on the following instruction.
	*/

	if (REG_UCSRA & _BV(TXC0) )
	{
		if(pHw_uart_pointer_to_instance->callback_dev)
		{
			DEV_CALLBACK_1_PARAMS(pHw_uart_pointer_to_instance->callback_dev,
					CALLBACK_TX_DONE,1);
		}
	}

}

//struct dev_desc_t *   atmega_callback_dev;

/**
  * @brief   TX Interrupt routine.
  * @param  None
  * @retval None
  */
ISR (USART_TX_vect)
 {
    /* In order to detect unexpected events during development,
	  it is recommended to set a breakpoint on the following instruction.
    */

	 atomIntEnter();

//		_asm("ldw Y,SP\n ldw X,#0x3ff\n  ldw SP,X\n PUSHW Y\n");

		tx_function();

//		_asm(" POPW Y \n ldw SP,Y\n ");

		atomIntExit(FALSE);
 }



void rx_function(void)
{
	/* In order to detect unexpected events during development,
   	   it is recommended to set a breakpoint on the following instruction.
	 */
	static uint8_t cChar;

	if (REG_UCSRA & _BV(RXC0) )
	{
		cChar = REG_UDR;
		if (NULL ==pHw_uart_pointer_to_instance->callback_dev )  return ;


		DEV_CALLBACK(pHw_uart_pointer_to_instance->callback_dev ,
				CALLBACK_DATA_RECEIVED, &cChar, 1);

	}
}


/**
  * @brief   RX Interrupt routine.
  * @param  None
  * @retval None
  */
ISR (USART_RX_vect)
 {
	 atomIntEnter();

//	 _asm("ldw Y,SP\n ldw X,#0x3ff\n  ldw SP,X\n PUSHW Y\n");
	 rx_function();
//	_asm(" POPW Y \n ldw SP,Y\n ");

	atomIntExit(FALSE);

 }


/*--------------------------------------------------------------------------
 * Function:        uart_atmega_pwrite
 * Parameters:
 * Returns:
 *--------------------------------------------------------------------------
 */
size_t uart_atmega_pwrite(const void *aHandle,
		const uint8_t *apData, size_t aLength, size_t aOffset)
{

	loop_until_bit_is_set(REG_UCSRA, BIT_UDRE);
	REG_UDR = *apData;

	REG_UCSRB |= _BV(BIT_TXEN) ;

	return 1;
}


/*-------------------------------------------------------------------------
 * Function:        uart_atmega_ioctl
 * Parameters:
 * Returns:
 *-------------------------------------------------------------------------
 */
uint8_t uart_atmega_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	switch(aIoctl_num)
	{
		case IOCTL_GET_PARAMS_ARRAY_FUNC :
			*(uint8_t*)aIoctl_param2 =  0; //size
			break;


		case IOCTL_UART_SET_BAUD_RATE :
			INSTANCE(aHandle)->baud_rate = (uint32_t)aIoctl_param1;
			break;
		case IOCTL_DEVICE_START :
			#if AVR_CPU_HZ < 2000000UL && defined(U2X)
			  REG_UCSRA = _BV(U2X);/* improve baud rate error by using 2x clk */
			  REG_UBRRL =
					  (AVR_CPU_HZ / (8UL * INSTANCE(aHandle)->baud_rate)) - 1;
			#else
			  REG_UBRRL =
					  (AVR_CPU_HZ / (16UL * INSTANCE(aHandle)->baud_rate)) - 1;
			#endif

			  pHw_uart_pointer_to_instance = INSTANCE(aHandle);
			  REG_UCSRB =  _BV(BIT_RXEN); /* rx enable */
			break;
		case IOCTL_UART_DISABLE_TX :
			REG_UCSRB &= (~ (_BV(BIT_TXEN)));
			break;
		case IOCTL_UART_ENABLE_TX :
		    //UART1_ITConfig( UART1_IT_TXE, ENABLE );
		    break;
#if UART_ATMEGA_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0
		case IOCTL_SET_ISR_CALLBACK_DEV:
			atmega_callback_dev =(struct dev_desc_t *) aIoctl_param1;
			break;
#endif
		default :
			return 1;
	}
	return 0;
}
