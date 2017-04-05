/*
 *
 * uart_atmega.h
 *
 *
 *
 *
 *
 */

#ifndef _UART_ATMEGA_H_
#define _UART_ATMEGA_H_

/********  includes *********************/

#include "uart_atmega_config.h"
#include "dev_management_api.h" // for device manager defines and typedefs
#include "src/_uart_atmega_prerequirements_check.h" // should be after {uart_atmega_config.h,dev_management_api.h}

/********  types  *********************/

typedef struct {
	struct dev_desc_t *   callback_dev;
	uint32_t baud_rate;
} UART_ATMEGA_Instance_t;

#endif
