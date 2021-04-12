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
	SHELL_PRESET_STATE_LOADING,
	SHELL_PRESET_STATE_SAVING_DEFAULT_PRESET
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
	if ((0 == runtime_handle->init_done) || (runtime_handle->overflow) ||
			(SHELL_PRESET_STATE_RECORDING != runtime_handle->state))
	{
		return 0;
	}

	preset_actual_size = runtime_handle->preset_actual_size;
	config_handle = DEV_GET_CONFIG_DATA_POINTER(shell_presets, adev);
	preset_size = config_handle->preset_size;
	if (preset_size < (preset_actual_size + aLength))
	{
		runtime_handle->overflow = 1;
		return 0;
	}

	curr_preset_buf = runtime_handle->curr_preset_buf;
	memcpy(&curr_preset_buf[preset_actual_size], apData, aLength);

	preset_actual_size += aLength;
	runtime_handle->preset_actual_size = preset_actual_size;

	return aLength;
}



static uint8_t save_preset(
		struct shell_presets_cfg_t *config_handle,
		struct shell_presets_runtime_t *runtime_handle,
		size_t num_of_preset)
{
	uint16_t preset_size;
	struct dev_desc_t *   storage_dev ;
	uint8_t *curr_preset_buf;
	uint16_t preset_actual_size;
	size_t write_addr;
	uint8_t ret_val;

	ret_val = 0;
	if (SHELL_PRESET_STATE_LOADING == runtime_handle->state)
	{
		/* if one command of preset is some 'save_preset' command,
		 * then on loading preset this command should be skipped.
		 */
		return 0;
	}

	if (SHELL_PRESET_STATE_RECORDING == runtime_handle->state)
	{
		if (runtime_handle->overflow)
		{
			ret_val = 2;
			runtime_handle->overflow = 0;
			goto exit;
		}
	}
	else if (SHELL_PRESET_STATE_SAVING_DEFAULT_PRESET == runtime_handle->state)
	{
		// do nothing yet
	}
	else
	{
		ret_val = 1;
		goto exit;
	}

	storage_dev = config_handle->storage_dev;
	preset_size = config_handle->preset_size;
	curr_preset_buf = runtime_handle->curr_preset_buf;
	preset_actual_size = runtime_handle->preset_actual_size;

	memcpy(&curr_preset_buf[MAGIC_NUMBER_POS],
					&magic_number, MAGIC_NUMBER_SIZE);
	memcpy(&curr_preset_buf[PRESET_SIZE_WORD_POS],
				&preset_actual_size, PRESET_SIZE_WORD_SIZE);
	// just for easier debugging set all unused space to 0's
	memset(&curr_preset_buf[preset_actual_size], 0,
					preset_size - preset_actual_size);
	write_addr = config_handle->start_address_on_storage +
									(preset_size * num_of_preset);
	DEV_PWRITE(storage_dev, curr_preset_buf, preset_size, write_addr);

exit:
	runtime_handle->state = SHELL_PRESET_STATE_IDLE;
	return ret_val;
}


static uint8_t get_preset(struct shell_presets_cfg_t *config_handle,
		struct shell_presets_runtime_t *runtime_handle, size_t num_of_preset)
{
	uint16_t actual_preset_size;
	struct dev_desc_t *   storage_dev ;
	uint16_t preset_size;
	uint8_t *curr_preset_buf;
	size_t read_addr;

	if (SHELL_PRESET_STATE_RECORDING == runtime_handle->state)
	{
		return 1;
	}

	if (config_handle->max_number_of_presets <= num_of_preset)
	{
		return 1;
	}

	curr_preset_buf = runtime_handle->curr_preset_buf;
	storage_dev = config_handle->storage_dev;
	preset_size = config_handle->preset_size;
	read_addr = config_handle->start_address_on_storage +
								(preset_size * num_of_preset);
	DEV_PREAD(storage_dev, curr_preset_buf, preset_size, read_addr);

	if (0 != memcmp(&curr_preset_buf[MAGIC_NUMBER_POS],
							&magic_number, MAGIC_NUMBER_SIZE))
	{
		return 2;
	}

	memcpy(&actual_preset_size,
			&curr_preset_buf[PRESET_SIZE_WORD_POS], PRESET_SIZE_WORD_SIZE);

	if (actual_preset_size > preset_size)
	{
		return 2;
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

	if (SHELL_PRESET_STATE_IDLE != runtime_handle->state)
	{
		retVal = 1;
		goto exit;
	}
	runtime_handle->state = SHELL_PRESET_STATE_LOADING;

	retVal = get_preset(config_handle, runtime_handle, num_of_preset);

	if (0 != retVal)
	{
		goto exit;
	}

	shell_backend_dev = config_handle->shell_backend_dev;
	if (NULL == shell_backend_dev)
	{
		retVal = 0;
		goto exit;
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

exit:
	runtime_handle->state = SHELL_PRESET_STATE_IDLE;
	return retVal;
}


static uint8_t set_preset_to_default(
		struct shell_presets_cfg_t *config_handle,
		struct shell_presets_runtime_t *runtime_handle,
		size_t num_of_preset)
{
	uint8_t retVal;

	if (SHELL_PRESET_STATE_IDLE != runtime_handle->state)
	{
		return 1;
	}

	runtime_handle->state = SHELL_PRESET_STATE_SAVING_DEFAULT_PRESET;
	retVal = get_preset(config_handle, runtime_handle, num_of_preset);

	if (0 != retVal)
	{
		return 1;
	}

	save_preset(config_handle, runtime_handle, 0);

	return 0;
}


static uint8_t start_recording_preset(struct shell_presets_cfg_t *config_handle,
		struct shell_presets_runtime_t *runtime_handle, size_t num_of_preset )
{
	uint8_t *curr_preset_buf;
	uint16_t curr_preset_num;
	size_t write_addr;

	if (SHELL_PRESET_STATE_LOADING == runtime_handle->state)
	{
		/* if one command of preset is some 'start recording' command,
		 * then on loading preset this command should be skipped.
		 */
		return 0;
	}
	if (SHELL_PRESET_STATE_IDLE != runtime_handle->state)
	{
		return 1;
	}

	curr_preset_num = (uint16_t)num_of_preset;
	if (config_handle->max_number_of_presets <= curr_preset_num)
	{
		return 2;
	}

	runtime_handle->state = SHELL_PRESET_STATE_RECORDING;
	runtime_handle->overflow = 0;
	runtime_handle->preset_actual_size = DATA_POSITION;
	curr_preset_buf = runtime_handle->curr_preset_buf;
	runtime_handle->curr_preset_num = curr_preset_num;

	/* first invalidate saved preset. otherwise if preset was not saved
	 * correctly, the old preset will remain and maybe confused with new one
	 */
	memset(&curr_preset_buf[MAGIC_NUMBER_POS], 0, MAGIC_NUMBER_SIZE);
	memset(&curr_preset_buf[PRESET_SIZE_WORD_POS], 0, PRESET_SIZE_WORD_SIZE);
	write_addr = config_handle->start_address_on_storage +
							(config_handle->preset_size * curr_preset_num);
	DEV_PWRITE(config_handle->storage_dev, curr_preset_buf,
			PRESET_SIZE_WORD_SIZE + MAGIC_NUMBER_SIZE, write_addr);

	return 0;
}


static uint8_t abort_recording(
		struct shell_presets_cfg_t *config_handle,
		struct shell_presets_runtime_t *runtime_handle)
{
	runtime_handle->state = SHELL_PRESET_STATE_IDLE;
	runtime_handle->overflow = 0;
}


static uint8_t device_start(struct shell_presets_cfg_t *config_handle,
		struct shell_presets_runtime_t *runtime_handle)
{
	struct dev_desc_t *   storage_dev ;
	uint16_t preset_size;
	uint8_t *curr_preset_buf;

	preset_size = config_handle->preset_size;
	storage_dev = config_handle->storage_dev;
	if (DATA_POSITION >= preset_size)
	{
		CRITICAL_ERROR("too small preset size");
	}
	curr_preset_buf = (uint8_t *)os_safe_malloc(preset_size);
	errors_api_check_if_malloc_succeed(curr_preset_buf);
	runtime_handle->curr_preset_buf = curr_preset_buf;
	DEV_IOCTL_0_PARAMS(storage_dev, IOCTL_DEVICE_START);
	runtime_handle->state = SHELL_PRESET_STATE_IDLE;
	runtime_handle->init_done = 1;
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
	struct shell_presets_cfg_t *config_handle;
	struct shell_presets_runtime_t *runtime_handle;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(shell_presets, adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(shell_presets, adev);

	if ((0 == runtime_handle->init_done) && (IOCTL_DEVICE_START != aIoctl_num))
	{
		return 1;
	}

	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		return device_start(config_handle, runtime_handle);
		break;
	case IOCTL_SHELL_PRESETS_START_RECORDING :
		return start_recording_preset(
				config_handle, runtime_handle, (size_t) aIoctl_param1);
		break;
	case IOCTL_SHELL_PRESETS_STOP_RECORDING :
		return save_preset(config_handle,
				runtime_handle, runtime_handle->curr_preset_num);
		break;
	case IOCTL_SHELL_PRESETS_LOAD_PRESET :
		return load_preset(
					config_handle, runtime_handle, (size_t) aIoctl_param1);
		break;
	case IOCTL_SHELL_PRESETS_SET_PRESET_TO_DEFAULT :
		return set_preset_to_default(
				config_handle, runtime_handle, (size_t) aIoctl_param1);
		break;
	case IOCTL_SHELL_PRESETS_ABORT_RECORDING:
		return abort_recording(config_handle, runtime_handle);
		break;
		break;
	default :
		return 1;
	}
	return 0;
}


#define MODULE_NAME                      shell_presets
#define MODULE_IOCTL_FUNCTION            shell_presets_ioctl
#define MODULE_PWRITE_FUNCTION           shell_presets_pwrite
#include "add_module.h"
