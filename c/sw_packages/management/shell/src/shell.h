/*
 * file : shell.h
 *
 *
 */

#ifndef _SHELL_H
#define _SHELL_H

#include "dev_management_api.h"


/***************   typedefs    *******************/

struct shell_cfg_t {
	struct dev_desc_t * server_tx_dev;
	struct dev_desc_t * server_rx_dev;
	struct dev_desc_t *   callback_dev;
	struct dev_desc_t *   cmd_save_dev;
};

struct shell_runtime_instance_t {
	uint32_t lastTestedBytePos;
	uint8_t lastEOLchar;
};

#endif /* */
