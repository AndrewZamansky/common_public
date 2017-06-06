/*
 * file : shell_presets.h
 *
 */

#ifndef _SHELL_PRESETS_H
#define _SHELL_PRESETS_H

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"


/***************   typedefs    *******************/

struct shell_presets_cfg_t{
	struct dev_desc_t * storage_dev;
	struct dev_desc_t *   shell_callback_dev;
	uint16_t preset_size;
	uint16_t max_number_of_presets;
};

struct shell_presets_runtime_t {
	uint8_t state;
	uint16_t preset_actual_size;
	uint8_t *curr_preset_buf;
};

#endif
