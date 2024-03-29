
#ifndef _Shell_frontend_API_H_
#define _Shell_frontend_API_H_


#define SHELL_FRONTEND_API_SERVER_DEVICE_STR  "server_device"

#include "dev_management_api.h"

/* cmd_id should be in 16 bit hex format. examles: 0x0001, 0x004a ,....
 * if, by mistake, 2 commands with same ID will be declared then
 * 2 variables shell_frontend_bin_cmd_ID will exist, so linkage will fail
 */
#define SHELL_FRONTEND_API_ADD_BINARY_CMD(cmd_id, func)   \
	struct shell_bin_cmd_struct_t \
	shell_frontend_bin_cmd_##cmd_id = { cmd_id, func}

enum shell_frontend_mode_e {
	SHELL_FRONTEND_MODE_ASCII = 0,
	SHELL_FRONTEND_MODE_BINARY
};

typedef void (*shell_bin_cmd_func_t)(uint8_t *buff, size_t length);

struct shell_bin_cmd_struct_t {
	uint16_t cmd_id;
	shell_bin_cmd_func_t  bin_cmd_func;
};


enum shell_frontend_api_ioctl_e {
	// need to load shell preset from shell task because it may
	// require a lot of stack due to commands parsing (for example u-boot)
	IOCTL_SHELL_FRONTEND_LOAD_PRESET = IOCTL_LAST_COMMON_IOCTL + 1,
	IOCTL_SHELL_FRONTEND_RUN_BATCH,
};


struct shell_frontend_load_preset_t {
	struct dev_desc_t *shell_preset_pdev; // - pointer to preset manager module
	uint8_t num_of_preset;                // - number of preset to load
};


struct shell_frontend_batch_t {
	uint8_t *batch_buffer;     // - buffer of shell command
	size_t batch_buffer_size;  // - size of buffer
};


struct shell_frontend_cfg_t {
	struct dev_desc_t *server_tx_dev;
	struct dev_desc_t *server_rx_dev;
	struct dev_desc_t *shell_backend_dev;
	struct dev_desc_t *cmd_save_dev;
};
SET_CONFIG_TYPE(shell_frontend, struct shell_frontend_cfg_t);

void shell_frontend_set_mode(uint8_t mode);

void shell_frontend_reply_bin_msg_data(const uint8_t *data, size_t len);
void shell_frontend_set_reply_bin_msg_data_size(size_t msg_size);
void shell_frontend_load_preset_from_shell_cmd(
		struct shell_frontend_load_preset_t *preset_params);

#endif
