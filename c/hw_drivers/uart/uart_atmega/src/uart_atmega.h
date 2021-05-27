/*
 *
 * uart_atmega.h
 *
 *
 */

#ifndef _UART_ATMEGA_H_
#define _UART_ATMEGA_H_

#include "uart_atmega_config.h"
#include "dev_management_api.h"

struct UART_ATMEGA_Instance_t {
	struct dev_desc_t *   callback_dev;
	uint32_t baud_rate;
};

#endif
