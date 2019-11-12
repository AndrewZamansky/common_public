
#ifndef _Shell_frontend_API_H_
#define _Shell_frontend_API_H_


#define SHELL_FRONTEND_API_SERVER_DEVICE_STR  "server_device"

#include "dev_management_api.h"

struct shell_frontend_cfg_t {
	struct dev_desc_t * server_tx_dev;
	struct dev_desc_t * server_rx_dev;
	struct dev_desc_t *   shell_backend_dev;
	struct dev_desc_t *   cmd_save_dev;
};

SET_CONFIG_TYPE(shell_frontend, struct shell_frontend_cfg_t);

#endif
