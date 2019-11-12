/*
 *
 * file :   shell_presets.c
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"
#include "errors_api.h"

#include "dev_management_api.h"
#include "string.h"

#include "shell_presets_api.h"
#include "shell_presets.h"
#include "management_api.h"

#include "os_wrapper.h"


#define MAGIC_NUMBER            0xA5B6C7D8
#define MAGIC_NUMBER_POS        (0)
#define MAGIC_NUMBER_SIZE       (4)
#define PRESET_SIZE_WORD_POS    (MAGIC_NUMBER_POS + MAGIC_NUMBER_SIZE)
#define PRESET_SIZE_WORD_SIZE   (2)
#define DATA_POSITION           (PRESET_SIZE_WORD_SIZE + MAGIC_NUMBER_SIZE)


enum state_t {
	SHELL_PRESET_STATE_IDLE,
	SHELL_PRESET_STATE_RECORDING,
	SHELL_PRESET_STATE_RECORDING_OVERFLOW,
	SHELL_PRESET_STATE_LOADING
};


static uint32_t magic_number = MAGIC_NUMBER;

/**
 * shell_presets_pwrite()
 *
 * return:
 */
static size_t shell_presets_pwrite(struct dev_desc_t *adev,
			const uint8_t *apData, size_t aLength, size_t aOffset)
{
	struct shell_presets_runtime_t *runtime_handle;
	struct shell_presets_cfg_t *config_handle;
	uint8_t *curr_preset_buf;
	uint16_t preset_actual_size;
	uint16_t preset_size;

	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(shell_presets, adev);
	if (SHELL_PRESET_STATE_RECORDING != runtime_handle->state)
	{
		return 0;
	}

	preset_actual_size = runtime_handle->preset_actual_size;
	config_handle = DEV_GET_CONFIG_DATA_POINTER(shell_presets, adev);
	preset_size = config_handle->preset_size;
	if (preset_size < (preset_actual_size + aLength))
	{
		runtime_handle->state = SHELL_PRESET_STATE_RECORDING_OVERFLOW;
		return 0;
	}

	curr_preset_buf = runtime_handle->curr_preset_buf;
	memcpy(&curr_preset_buf[preset_actual_size], apData, aLength);

	preset_actual_size += aLength;
	runtime_handle->preset_actual_size = preset_actual_size;

	return aLength;
}



static uint8_t save_preset( struct shell_presets_cfg_t *config_handle,
		struct shell_presets_runtime_t *runtime_handle,
		size_t num_of_preset)
{
	uint16_t preset_size;
	struct dev_desc_t *   storage_dev ;
	uint8_t *curr_preset_buf;

	if (SHELL_PRESET_STATE_RECORDING != runtime_handle->state)
	{
		return 1;
	}

	storage_dev = config_handle->storage_dev;
	preset_size = config_handle->preset_size;
	curr_preset_buf = runtime_handle->curr_preset_buf;

	if (CONFIG_SHELL_PRESETS_NUMBER_OF_PRESET < num_of_preset)
	{
		return 1;
	}

	memcpy(&curr_preset_buf[MAGIC_NUMBER_POS],
					&magic_number, MAGIC_NUMBER_SIZE);
	memcpy(&curr_preset_buf[PRESET_SIZE_WORD_POS],
			&runtime_handle->preset_actual_size, PRESET_SIZE_WORD_SIZE);
	DEV_PWRITE(storage_dev,
			curr_preset_buf, preset_size, preset_size * num_of_preset);

	runtime_handle->state = SHELL_PRESET_STATE_IDLE;

	return 0;
}


static uint8_t get_preset(struct shell_presets_cfg_t *config_handle,
		struct shell_presets_runtime_t *runtime_handle,
		size_t num_of_preset)
{
	uint16_t actual_preset_size;
	struct dev_desc_t *   storage_dev ;
	uint16_t preset_size;
	uint8_t *curr_preset_buf;

	if (SHELL_PRESET_STATE_RECORDING == runtime_handle->state)
	{
		return 1;
	}

	if (CONFIG_SHELL_PRESETS_NUMBER_OF_PRESET < num_of_preset)
	{
		return 1;
	}

	curr_preset_buf = runtime_handle->curr_preset_buf;
	storage_dev = config_handle->storage_dev;
	preset_size = config_handle->preset_size;
	DEV_PREAD(storage_dev, curr_preset_buf,
			preset_size, preset_size * num_of_preset);

	if (0 != memcmp(&curr_preset_buf[MAGIC_NUMBER_POS],
							&magic_number, MAGIC_NUMBER_SIZE))
	{
		return 1;
	}

	memcpy(&actual_preset_size,
			&curr_preset_buf[PRESET_SIZE_WORD_POS], PRESET_SIZE_WORD_SIZE);

	if (actual_preset_size > preset_size)
	{
		return 1;
	}

	runtime_handle->preset_actual_size = actual_preset_size;

	return 0;
}

static uint8_t load_preset(struct shell_presets_cfg_t *config_handle,
		struct shell_presets_runtime_t *runtime_handle,
		size_t num_of_preset)
{
	uint8_t retVal;
	uint16_t actual_preset_size;
	uint8_t *curr_preset_buf;
	uint8_t *curr_pos;
	uint8_t *last_pos;
	uint8_t *end_of_cmd_pos;
	struct dev_desc_t *   shell_backend_dev;
	struct rcvd_cmd_t	rcvd_cmd;

	retVal = get_preset(config_handle, runtime_handle, num_of_preset);

	if (0 != retVal)
	{
		return 1;
	}

	shell_backend_dev = config_handle->shell_backend_dev;
	if (NULL == shell_backend_dev)
	{
		return 0;
	}

	actual_preset_size = runtime_handle->preset_actual_size;
	curr_preset_buf = runtime_handle->curr_preset_buf;
	curr_pos = curr_preset_buf + DATA_POSITION;
	last_pos = curr_preset_buf + actual_preset_size;
	end_of_cmd_pos = curr_pos;
	curr_preset_buf[actual_preset_size - 1] = '\n';
	while (curr_pos < last_pos)
	{
		if ( ('\n' == *end_of_cmd_pos) || ('\r' == *end_of_cmd_pos))
		{
			rcvd_cmd.reply_dev = NULL;
			rcvd_cmd.cmd_buf = curr_pos;
			rcvd_cmd.cmd_len = (end_of_cmd_pos - curr_pos) + 1;
			DEV_IOCTL_1_PARAMS(
				shell_backend_dev, IOCTL_SHELL_NEW_FRAME_RECEIVED, &rcvd_cmd);
			end_of_cmd_pos++;
			curr_pos = end_of_cmd_pos;
		}
		else
		{
			end_of_cmd_pos++;
		}
	}

	return 0;
}


static uint8_t set_preset_to_default(struct shell_presets_cfg_t *config_handle,
		struct shell_presets_runtime_t *runtime_handle,
		size_t num_of_preset )
{
	uint8_t retVal;

	retVal = get_preset(config_handle, runtime_handle, num_of_preset);

	if (0 != retVal)
	{
		return 1;
	}

	save_preset(config_handle, runtime_handle, 0);

	return 0;
}


/**
 * shell_presets_ioctl()
 *
 * return:
 */
static uint8_t shell_presets_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	uint8_t retVal;
	struct shell_presets_cfg_t *config_handle;
	struct shell_presets_runtime_t *runtime_handle;
	struct dev_desc_t *   storage_dev ;
	uint16_t preset_size;
	uint8_t *curr_preset_buf;
	uint8_t curr_state;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(shell_presets, adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(shell_presets, adev);

	storage_dev = config_handle->storage_dev;
	preset_size = config_handle->preset_size;
	curr_state = runtime_handle->state;

	retVal = 0;

	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		if (DATA_POSITION >= preset_size)
		{
			CRITICAL_ERROR("too small preset size");
		}
		curr_preset_buf = (uint8_t *)os_safe_malloc(preset_size);
		errors_api_check_if_malloc_succeed(curr_preset_buf);
		runtime_handle->curr_preset_buf = curr_preset_buf;
		DEV_IOCTL_0_PARAMS(storage_dev, IOCTL_DEVICE_START);
		DEV_PREAD(storage_dev, curr_preset_buf, preset_size, 0);
		break;

	case IOCTL_SHELL_PRESETS_START_RECORDING :
		if (SHELL_PRESET_STATE_LOADING == curr_state)
		{
			return 0;
		}
		runtime_handle->state = SHELL_PRESET_STATE_RECORDING;
		runtime_handle->preset_actual_size = DATA_POSITION;
		break;

	case IOCTL_SHELL_PRESETS_STOP_RECORDING :
		if (SHELL_PRESET_STATE_LOADING == curr_state)
		{
			return 0;
		}
		retVal = save_preset(
				config_handle, runtime_handle, (size_t) aIoctl_param1);
		break;

	case IOCTL_SHELL_PRESETS_LOAD_PRESET :
		if (SHELL_PRESET_STATE_LOADING == curr_state)
		{
			return 0;
		}
		runtime_handle->state = SHELL_PRESET_STATE_LOADING;
		retVal = load_preset(
				config_handle, runtime_handle, (size_t) aIoctl_param1);
		runtime_handle->state = SHELL_PRESET_STATE_IDLE;
		break;

	case IOCTL_SHELL_PRESETS_SET_PRESET_TO_DEFAULT :
		if (SHELL_PRESET_STATE_LOADING == curr_state)
		{
			return 0;
		}
		retVal = set_preset_to_default(
				config_handle, runtime_handle, (size_t) aIoctl_param1);
		break;

	default :
		return 1;
	}
	return retVal;
}


#define MODULE_NAME                      shell_presets
#define MODULE_IOCTL_FUNCTION            shell_presets_ioctl
#define MODULE_PWRITE_FUNCTION           shell_presets_pwrite
#include "add_module.h"
