/*
 *
 * uart_stm8.h
 *
 *
 *
 *
 *
 */

#ifndef _UART_STM8_H_
#define _UART_STM8_H_

/********  includes *********************/

#include "uart_stm8_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_uart_stm8_prerequirements_check.h" // should be after {uart_stm8_config.h,dev_managment_api.h}

/********  types  *********************/

typedef struct {
	pdev_descriptor   callback_dev;
	uint32_t baud_rate;
} UART_STM8_Instance_t;

#endif
