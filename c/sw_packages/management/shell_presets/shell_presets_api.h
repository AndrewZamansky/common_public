
#ifndef _shell_presets_API_H_
#define _shell_presets_API_H_


#include "dev_management_api.h"


enum shell_presets_api_ioctl_e {
	IOCTL_SHELL_PRESETS_START_RECORDING = IOCTL_LAST_COMMON_IOCTL + 1,
	IOCTL_SHELL_PRESETS_STOP_RECORDING,
	IOCTL_SHELL_PRESETS_ABORT_RECORDING,
	IOCTL_SHELL_PRESETS_GET_PRESET_BUFFER,
	IOCTL_SHELL_PRESETS_RELEASE_PRESET_BUFFER,
	IOCTL_SHELL_PRESETS_SET_PRESET_TO_DEFAULT
};

struct shell_presets_api_get_buffer_t {
	uint8_t **preset_buffer;
	size_t *preset_size;
	uint8_t num_of_preset;
};

struct shell_presets_cfg_t {
	struct dev_desc_t * storage_dev;
	size_t  start_address_on_storage;
	uint16_t preset_size;
	uint16_t max_number_of_presets;
};
SET_CONFIG_TYPE(shell_presets, struct shell_presets_cfg_t);


#endif
