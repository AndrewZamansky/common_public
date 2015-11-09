/*
 *
 * uart_nuc505.h
 *
 *
 *
 *
 *
 */

#ifndef _UART_NUC505_H_
#define _UART_NUC505_H_

/********  includes *********************/

#include "uart_nuc505_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_uart_nuc505_prerequirements_check.h" // should be after {uart_nuc505_config.h,dev_managment_api.h}

/********  types  *********************/

typedef struct {
	pdev_descriptor_const   callback_dev;
	uint8_t   uart_num;
	uint32_t baud_rate;
} UART_NUC505_Instance_t;

#endif
