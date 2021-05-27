/*
 * file : WIRELESS_UART.h
 *
 *
 */

#ifndef _WIRELESS_UART_H
#define _WIRELESS_UART_H

#include "wireless_uart_config.h"
#include "dev_management_api.h"


struct WIRELESS_UART_Instance_t {
	struct dev_desc_t * server_dev;
	struct dev_desc_t *   callback_dev;
	struct dev_desc_t *   this_dev;
};


#endif
