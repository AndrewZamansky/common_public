/*
 *
 * uart_stm8.h
 *
 *
 *
 *
 *
 */

#ifndef _UART_STM8_H_
#define _UART_STM8_H_

/********  includes *********************/

#include "src/_uart_stm8_prerequirements_check.h"

/********  types  *********************/

typedef struct {
	pdev_descriptor_t   callback_tx_dev;
	pdev_descriptor_t   callback_rx_dev;
	uint32_t baud_rate;
} uart_stm8_instance_t;

#endif
