/*
 *
 * uart_nuc505.h
 *
 *
 *
 */

#ifndef _UART_NUC505_H_
#define _UART_NUC505_H_

/********  includes *********************/

#include "_project_defines.h"
#include "dev_management_api.h"

/********  types  *********************/

struct uart_nuc505_cfg_t {
	struct dev_desc_t *   callback_tx_dev;
	struct dev_desc_t *   callback_rx_dev;
	size_t   base_address;
	uint32_t baud_rate;
};

#define		UART_NUC505_RCV_DATA_SIZE_BUFFER	16
struct uart_nuc505_runtime_t {
	uint8_t	rcv_data[UART_NUC505_RCV_DATA_SIZE_BUFFER];
};

#endif
