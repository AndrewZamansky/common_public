
#ifndef _UART_I94XXX_API_H_
#define _UART_I94XXX_API_H_

#include "I94100.h"

/*****************  defines  **************/

#define UART_I94XXX_API_BASE_ADDRESS_UART0	(size_t)UART0_BASE
#define UART_I94XXX_API_BASE_ADDRESS_UART1	(size_t)UART1_BASE

typedef enum {
	UART_I94XXX_UART0_TX_RX_PINS_PORT_B_PINS_8_9,
	UART_I94XXX_UART0_TX_RX_PINS_PORT_D_PINS_14_15
} uart_i94xxx_pinout;


/**********  define API  types ************/



/**********  define API  functions  ************/

#endif
