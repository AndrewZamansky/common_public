/*
 *
 * uart_stm8.h
 *
 */

#ifndef _UART_STM8_H_
#define _UART_STM8_H_

/********  includes *********************/


/********  types  *********************/

typedef struct {
	struct dev_desc_t *   callback_tx_dev;
	struct dev_desc_t *   callback_rx_dev;
	uint32_t baud_rate;
} uart_stm8_instance_t;

#endif
