/*
 *
 * uart_linux.h

 *
 */

#ifndef _UART_LINUX_H_
#define _UART_LINUX_H_

/********  includes *********************/

#include "dev_management_api.h"
#include <pthread.h>

/********  types  *********************/

struct uart_linux_cfg_t {
	struct dev_desc_t *   callback_tx_dev;
	struct dev_desc_t *   callback_rx_dev;
	uint32_t baud_rate;
	char  tty_dev_path[64];
} ;


#define UART_LINUX_RCV_DATA_SIZE_BUFFER   128
struct uart_linux_runtime_t {
	int tty_fd;
	pthread_t  thread_id;
	uint8_t	rcv_data[UART_LINUX_RCV_DATA_SIZE_BUFFER];
	uint8_t initialized;
};

#endif
