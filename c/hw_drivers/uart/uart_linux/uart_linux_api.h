
#ifndef _UART_LINUX_API_H_
#define _UART_LINUX_API_H_


struct uart_linux_cfg_t {
	struct dev_desc_t *   callback_tx_dev;
	struct dev_desc_t *   callback_rx_dev;
	uint32_t baud_rate;
	char  tty_dev_path[64];
} ;
SET_CONFIG_TYPE(uart_linux, struct uart_linux_cfg_t);

#endif
