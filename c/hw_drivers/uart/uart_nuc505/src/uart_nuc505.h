/*
 *
 * uart_nuc505.h
 *
 *
 *
 *
 *
 */

#ifndef _UART_NUC505_H_
#define _UART_NUC505_H_

/********  includes *********************/

#include "src/_uart_nuc505_prerequirements_check.h"

/********  types  *********************/

typedef struct {
	pdev_descriptor_t   callback_tx_dev;
	pdev_descriptor_t   callback_rx_dev;
	size_t   base_address;
	uint32_t baud_rate;
} uart_nuc505_instance_t;


#endif
