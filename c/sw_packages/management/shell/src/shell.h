/*
 * file : SW_UART_WRAPPER.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _SHELL_H
#define _SHELL_H

#include "src/_shell_prerequirements_check.h"


/***************   typedefs    *******************/

typedef struct {
	pdev_descriptor_t server_tx_dev;
	pdev_descriptor_t server_rx_dev;
	pdev_descriptor_t   callback_dev;
} shell_instance_t;

typedef struct {
	uint32_t lastTestedBytePos;
	uint8_t lastEOLchar;
} shell_runtime_instance_t;

#endif /* */
