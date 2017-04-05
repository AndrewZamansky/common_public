/*
 * file : WIRELESS_UART.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _WIRELESS_UART_H
#define _WIRELESS_UART_H

#include "wireless_uart_config.h"
#include "dev_management_api.h" // for device manager defines and typedefs
#include "src/_wireless_uart_prerequirements_check.h" // should be after {wireless_uart_config.h,dev_management_api.h}


/***************   typedefs    *******************/


typedef struct {
	struct dev_desc_t * server_dev;
	struct dev_desc_t *   callback_dev;
	struct dev_desc_t *   this_dev;
} WIRELESS_UART_Instance_t;


#endif /* */
