/*
 *
 * uart_STM32F103x.h
 *
 *
 */

#ifndef _UART_STM32F103x_H_
#define _UART_STM32F103x_H_

#include "stm32f10x_usart.h"


struct UART_STM32F103x_Instance_t {
	struct dev_desc_t *   callback_dev;
	uint8_t   uart_num;
	uint32_t baud_rate;
	USART_TypeDef* USARTx_Handle;
} ;

#endif
