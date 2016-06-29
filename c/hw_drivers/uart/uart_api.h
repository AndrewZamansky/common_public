
#ifndef _UART_API_H_
#define _UART_API_H_

#include "dev_managment_api.h" // for device manager defines and typedefs

/*****************  defines  **************/


/**********  define API  types ************/


/**********  define API  functions  ************/
typedef enum
{
	IOCTL_UART_TURN_OFF = IOCTL_LAST_COMMON_IOCTL+1,
	IOCTL_UART_SET_BAUD_RATE,
	IOCTL_UART_DISABLE_TX,
	IOCTL_UART_ENABLE_TX,
//	IOCTL_UART_FILTER_NON_ASCII_NOISE,
	IOCTL_LAST_COMMON_UART_IOCTL
}UART_COMMON_API_ioctl_t;


#define UART_API_BAUD_RATE_STR "baud_rate"

#endif
