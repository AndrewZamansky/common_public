/*
 *
 * uart_i94xxx.h

 *
 */

#ifndef _UART_I94XXX_H_
#define _UART_I94XXX_H_

/********  includes *********************/

#include "src/_uart_i94xxx_prerequirements_check.h"

/********  types  *********************/

struct uart_i94xxx_cfg_t {
	pdev_descriptor_t   callback_tx_dev;
	pdev_descriptor_t   callback_rx_dev;
	pdev_descriptor_t   src_clock;
	size_t   base_address;
	uint32_t baud_rate;
} ;


#endif
