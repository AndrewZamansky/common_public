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
	struct dev_desc_t *   callback_tx_dev;
	struct dev_desc_t *   callback_rx_dev;
	size_t   base_address;
	uint32_t baud_rate;
} uart_nuc505_instance_t;

#define		UART_NUC505_RCV_DATA_SIZE_BUFFER	16
typedef struct {
	uint8_t	rcv_data[UART_NUC505_RCV_DATA_SIZE_BUFFER];
} uart_nuc505_runtime_instance_t;

#endif
