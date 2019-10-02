
#ifndef _Shell_API_H_
#define _Shell_API_H_


#define SHELL_API_SERVER_DEVICE_STR  "server_device"

#include "dev_management_api.h"

struct shell_cfg_t {
	struct dev_desc_t * server_tx_dev;
	struct dev_desc_t * server_rx_dev;
	struct dev_desc_t *   callback_dev;
	struct dev_desc_t *   cmd_save_dev;
};

#endif
