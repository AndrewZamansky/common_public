/*
 *
 * uart_i9xxxx.h
 *
 */

#ifndef _UART_I9XXXX_H_
#define _UART_I9XXXX_H_

#include "dev_management_api.h"

#define   UART_I9XXXX_RCV_DATA_SIZE_BUFFER  20
struct i9xxx_uart_runtime_t {
	uint8_t  rcv_data[UART_I9XXXX_RCV_DATA_SIZE_BUFFER];
	size_t   send_data_len;
	uint8_t init_done;
	uint32_t rx_fifo_overflow_cnt;// for debug
};
SET_RUNTIME_DATA_TYPE(uart_i9xxxx, struct i9xxx_uart_runtime_t);

#endif
