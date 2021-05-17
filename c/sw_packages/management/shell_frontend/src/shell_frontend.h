/*
 * file : shell_frontend.h
 *
 *
 */

#ifndef _SHELL_FRONTEND_H
#define _SHELL_FRONTEND_H

#include "dev_management_api.h"

struct shell_frontend_runtime_instance_t {
	uint32_t last_tested_pos;
	uint8_t mode;
	uint8_t last_EOL_char;
	uint8_t load_preset_flag;
};

SET_RUNTIME_DATA_TYPE(shell_frontend, struct shell_frontend_runtime_instance_t);

#endif
