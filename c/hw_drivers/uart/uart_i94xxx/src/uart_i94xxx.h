/*
 *
 * uart_i94xxx.h
 *
 */

#ifndef _UART_I94XXX_H_
#define _UART_I94XXX_H_

/********  includes *********************/

#include "dev_management_api.h"

/********  types  *********************/

struct uart_i94xxx_cfg_t {
	struct dev_desc_t *   callback_tx_dev;
	struct dev_desc_t *   callback_rx_dev;
	struct dev_desc_t *   src_clock;
	uint32_t baud_rate;
	uint32_t tx_pin;
	uint32_t rx_pin;
};


#endif
