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

struct shell_instance_t {
	struct dev_desc_t * server_tx_dev;
	struct dev_desc_t * server_rx_dev;
	struct dev_desc_t *   callback_dev;
};

struct shell_runtime_instance_t {
	uint32_t lastTestedBytePos;
	uint8_t lastEOLchar;
};

#endif /* */
