
#ifndef _sw_uart_wrapper_API_H_
#define _sw_uart_wrapper_API_H_

#include "src/_sw_uart_wrapper_prerequirements_check.h"

/*****************  defines  **************/


/**********  define API  types ************/


#define SW_UART_WRAPPER_API_RX_BUFF_SIZE_STR   "rx_buff_size"
#define SW_UART_WRAPPER_API_SERVER_DEVICE_STR  	"server_device"

typedef enum
{
	IOCTL_SW_UART_WRAPPER_SET_BUFF_SIZE = IOCTL_LAST_COMMON_IOCTL+1,
	IOCTL_SW_UART_WRAPPER_RESET,
	IOCTL_SW_UART_WRAPPER_DISABLE_TX,
	IOCTL_SW_UART_WRAPPER_ENABLE_TX,
	IOCTL_SW_UART_WRAPPER_USE_TASK
}SW_UART_WRAPPER_API_ioctl_t;

/**********  define API  functions  ************/



#endif
