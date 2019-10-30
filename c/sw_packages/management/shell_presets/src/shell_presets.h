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
	uint8_t state;
	uint16_t preset_actual_size;
	uint8_t *curr_preset_buf;
};
SET_RUNTIME_DATA_TYPE(shell_presets, struct shell_presets_runtime_t);

#endif
