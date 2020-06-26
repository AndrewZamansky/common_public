/*
 *
 * uart_linux.h

 *
 */

#ifndef _UART_LINUX_H_
#define _UART_LINUX_H_

#include "dev_management_api.h"
#include <pthread.h>


#define UART_LINUX_RCV_DATA_SIZE_BUFFER   128
struct uart_linux_runtime_t {
	int tty_fd;
	pthread_t  thread_id;
	uint8_t	rcv_data[UART_LINUX_RCV_DATA_SIZE_BUFFER];
	uint8_t initialized;
	uint8_t init_done;
};
SET_RUNTIME_DATA_TYPE(uart_linux, struct uart_linux_runtime_t);

#endif
