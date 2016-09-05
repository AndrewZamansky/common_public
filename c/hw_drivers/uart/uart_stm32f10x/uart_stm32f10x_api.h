
#ifndef _UART_STM32F10x_API_H_
#define _UART_STM32F10x_API_H_

#include "uart_api.h"
#include "src/_uart_stm32f10x_prerequirements_check.h"

/*****************  defines  **************/


/**********  define API  types ************/


#define UART_STM32F10X_API_UART_NUM_STR "uart_num"
#define UART_STM32F10X_API_UART_DISABLE_STR "disable"

/**********  define API  functions  ************/
typedef enum
{
	IOCTL_UART_STM32F10X_SET_UART_NUM = IOCTL_LAST_COMMON_UART_IOCTL+1,
	IOCTL_UART_STM32F10X_DISABLE
}UART_STM32F10X_API_ioctl_t;


#endif
