
#ifndef _UART_I9XXXX_API_H_
#define _UART_I9XXXX_API_H_


#ifdef CONFIG_I96XXX_M0
	#include "I96100.h"
	#define UART_I96XXX_API_BASE_ADDRESS_UART0    (size_t)UART0_BASE
	#define UART_I96XXX_API_BASE_ADDRESS_UART1    (size_t)UART1_BASE
#endif

struct uart_i9xxxx_cfg_t {
	struct dev_desc_t *   callback_tx_dev;
	struct dev_desc_t *   callback_rx_dev;
	struct dev_desc_t *   src_clock;
	size_t     base_address;
	uint32_t baud_rate;
	uint32_t tx_pin;
	uint32_t rx_pin;
};

SET_CONFIG_TYPE(uart_i9xxxx, struct uart_i9xxxx_cfg_t);

#endif
