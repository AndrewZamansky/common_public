/*
 *
 * uart_i94xxx.h
 *
 *
 *
 *
 *
 */

#ifndef _UART_I94XXX_H_
#define _UART_I94XXX_H_

/********  includes *********************/

#include "src/_uart_i94xxx_prerequirements_check.h"

/********  types  *********************/

typedef struct {
	pdev_descriptor_t   callback_tx_dev;
	pdev_descriptor_t   callback_rx_dev;
	size_t   base_address;
	uint32_t baud_rate;
} uart_i94xxx_instance_t;


#endif
