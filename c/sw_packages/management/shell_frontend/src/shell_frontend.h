/*
 * file : shell_frontend.h
 *
 *
 */

#ifndef _SHELL_FRONTEND_H
#define _SHELL_FRONTEND_H

#include "dev_management_api.h"

struct msg_load_preset_t {
	struct dev_desc_t *shell_preset_pdev;
	uint8_t num_of_preset;
};

struct shell_frontend_runtime_instance_t {
	uint32_t last_tested_pos;
	uint8_t mode;
	uint8_t last_EOL_char;
	uint8_t load_preset_flag;
	struct msg_load_preset_t  msg_load_preset;
};

SET_RUNTIME_DATA_TYPE(shell_frontend, struct shell_frontend_runtime_instance_t);

#endif
