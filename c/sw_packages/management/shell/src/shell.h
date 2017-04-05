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
	struct dev_desc_t * server_tx_dev;
	struct dev_desc_t * server_rx_dev;
	struct dev_desc_t *   callback_dev;
} shell_instance_t;

typedef struct {
	uint32_t lastTestedBytePos;
	uint8_t lastEOLchar;
} shell_runtime_instance_t;

#endif /* */
