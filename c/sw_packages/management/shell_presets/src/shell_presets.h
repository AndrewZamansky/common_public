/*
 * file : shell_presets.h
 *
 */

#ifndef _SHELL_PRESETS_H
#define _SHELL_PRESETS_H

#include "_project_typedefs.h"
#include "_project_defines.h"
#include "dev_management_api.h"

struct shell_presets_runtime_t {
	uint8_t *curr_preset_buf;
	uint16_t preset_actual_size;
	uint16_t curr_preset_num;
	uint8_t state;
	uint8_t overflow;
	uint8_t init_done;
};
SET_RUNTIME_DATA_TYPE(shell_presets, struct shell_presets_runtime_t);

#endif
