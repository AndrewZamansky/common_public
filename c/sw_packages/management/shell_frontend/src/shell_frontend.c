/*
 *
 * file :   shell_frontend.c
 *
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_tasks_defines.h"

#include "dev_management_api.h"

#include "os_wrapper.h"
#include "shell_frontend_api.h"
#include "shell_frontend.h"
#include "management_api.h"

#ifdef CONFIG_INCLUDE_SHELL_PRESETS
	#include "shell_presets_api.h"
#endif

enum msg_type_e {
	MSG_TYPE_FROM_RX_DEVICE,
	MSG_TYPE_LOAD_PRESET,
	MSG_TYPE_EXECUTE_BATCH
};

struct msg_from_rx_device_t {
	uint8_t dummy;
};

struct msg_load_preset_t {
	struct dev_desc_t *shell_preset_pdev;
	uint8_t num_of_preset;
};

struct msg_batch_t {
	uint8_t *batch_buffer;
	size_t batch_buffer_size;
};


struct queue_msg_t {
	enum msg_type_e msg_type;
	struct dev_desc_t *shell_frontend_target_pdev;
	union {
		struct msg_from_rx_device_t msg_from_rx_device;
		struct msg_load_preset_t msg_load_preset;
		struct msg_batch_t msg_batch;
	};
};

#define BIN_FLAGS_EXTENDED_CMD            0x01
#define BIN_FLAGS_SUPPRESS_REPLY_HEADER   0x02
#define BIN_FLAGS_INTEGRITY_STAMP_ADDED   0x04

extern struct shell_bin_cmd_struct_t *bin_cmd_table[];
extern const int bin_cmd_table_size;

static uint8_t task_is_running = 0;
static size_t  remain_bin_reply_size;
static uint8_t  use_stamp_in_bin_msg;
static size_t  reply_bin_msg_data_size;
static size_t  reply_bin_msg_full_size;
static uint8_t suppress_bin_reply_header;

#define PREAMBLE_SIZE                 4
uint8_t bin_preamble_patern[PREAMBLE_SIZE] = {'b', 'i', 'n', '\n'};

static const char erase_seq[] = "\b \b";   /* erase sequence */
static char EOF_MARKER_STR[] = "\r\n~2@5\r\n";

#define HEADER_CHAR_ON    '+'
#define HEADER_CHAR_OFF   '-'

#define BIN_CMD_TYPE_0x00   0x00
#define BIN_NON_EXTENDED_HEADER_SIZE  4

#define BIN_CMD_REPLY_STATUS_NO_ERROR        0
#define BIN_CMD_REPLY_STATUS_CMD_NOT_FOUND   1
#define BIN_CMD_REPLY_BAD_INTEGRITY_STAMP    2
#define BIN_CMD_REPLY_MSG_TOO_LONG           3
#define BIN_CMD_REPLY_MSG_TOO_SHORT          4
#define BIN_CMD_REPLY_NOT_SUPPORTED_FLAG     5

enum Header_positions_t {
	HEADER_SUPPRESS_ECHO_POS, // must be on first position
	HEADER_ADD_EOF_MARK_POS,
	START_OF_CMD_POS
};

static size_t process_buffer(struct shell_frontend_cfg_t *config_handle,
		uint8_t *input_buffer, size_t total_length);

static struct dev_desc_t *curr_tx_dev;
static struct shell_frontend_runtime_instance_t  *curr_runtime_hndl;

static os_queue_t xQueue = NULL;


/**
 * shell_frontend_callback()
 *
 * return:
 */
static uint8_t shell_frontend_callback(struct dev_desc_t *adev,
		const uint8_t aCallback_num,
		void * aCallback_param1, void * aCallback_param2)
{
	struct queue_msg_t  msg;

	if (NULL == xQueue) return 1;

	msg.msg_type = MSG_TYPE_FROM_RX_DEVICE;
	msg.shell_frontend_target_pdev = adev;
	os_queue_send_without_wait( xQueue, ( void * ) &msg);
	return 0;
}


static void shell_frontend_reply_data(const uint8_t *data, size_t len)
{
	size_t written_len;

	if (NULL == curr_tx_dev) return ;
	while (len)
	{
		written_len = DEV_WRITE(curr_tx_dev, data, len);
		len -= written_len;
		data += written_len;
	}
}


void shell_frontend_set_mode(uint8_t mode)
{
	curr_runtime_hndl->mode = mode;
}


static void send_bin_reply_head(size_t msg_data_size, uint8_t reply_status)
{
	uint8_t reply_header[4];

	if (0 != suppress_bin_reply_header) return;

	if (use_stamp_in_bin_msg && (0 != msg_data_size))
	{
		reply_header[2] = 4;
		reply_bin_msg_full_size = msg_data_size + 8; // add header and tail
	}
	else
	{
		reply_header[2] = 0;
		reply_bin_msg_full_size = msg_data_size + 4; // add only header
	}
	reply_header[0] = reply_bin_msg_full_size & 0xff;
	reply_header[1] = reply_bin_msg_full_size >> 8;
	reply_header[3] = reply_status;

	{
		shell_frontend_reply_data(reply_header, 4);
	}
}

void shell_frontend_set_reply_bin_msg_data_size(size_t msg_data_size)
{
	reply_bin_msg_data_size = msg_data_size;
	send_bin_reply_head(msg_data_size, BIN_CMD_REPLY_STATUS_NO_ERROR);
	remain_bin_reply_size = msg_data_size;
}


void shell_frontend_reply_bin_msg_data(const uint8_t *data, size_t len)
{
	uint8_t reply_data[4];

	if (0 == remain_bin_reply_size) return;

	if (0 == suppress_bin_reply_header)
	{
		if (len > remain_bin_reply_size)
		{
			len = remain_bin_reply_size;
		}
		remain_bin_reply_size -= len;
	}
	shell_frontend_reply_data(data, len);

	if (0 != remain_bin_reply_size) return;

	if (use_stamp_in_bin_msg)
	{
		reply_data[0] = (uint8_t)(reply_bin_msg_full_size & 0xff);
		reply_data[1] = (uint8_t)((reply_bin_msg_full_size >> 8) & 0xff);
		reply_data[2] = 0;
		reply_data[3] = 0;
		shell_frontend_reply_data(reply_data, 4);
	}
}


static void pad_remaining_data()
{
	size_t i;
	uint8_t data = 0xB0;

	if (suppress_bin_reply_header) return;

	i = remain_bin_reply_size;
	while (i--)
	{
		shell_frontend_reply_bin_msg_data(&data, 1);
	}
}

static struct msg_load_preset_t  curr_msg_load_preset;

void shell_frontend_load_preset_from_shell_cmd(
		struct shell_frontend_load_preset_t *preset_params)
{
	if (NULL == curr_runtime_hndl) return;
	curr_msg_load_preset.shell_preset_pdev =
								preset_params->shell_preset_pdev;
	curr_msg_load_preset.num_of_preset =
								preset_params->num_of_preset;
	curr_runtime_hndl->load_preset_flag = 1;
}


static void send_echo(
		uint8_t *buff, size_t curr_buff_pos, size_t last_printed_out_pos)
{
	if (0 == curr_buff_pos) return;

	if (HEADER_CHAR_ON != buff[HEADER_SUPPRESS_ECHO_POS])
	{
		shell_frontend_reply_data(&buff[last_printed_out_pos],
							curr_buff_pos - last_printed_out_pos);
	}
}


static uint8_t process_backspace(uint8_t *buff, size_t curr_buff_pos)
{
	size_t num_of_removed_chars;
	num_of_removed_chars = 0;
	if (curr_buff_pos)
	{
		if (HEADER_CHAR_ON != buff[HEADER_SUPPRESS_ECHO_POS])
		{
			shell_frontend_reply_data(
					(uint8_t*)erase_seq, sizeof(erase_seq) - 1);
		}
		//shift buffer right before DEL
		memmove(&buff[2], buff, curr_buff_pos - 1);
		curr_runtime_hndl->last_tested_pos = curr_buff_pos - 1;
		num_of_removed_chars++;
	}
	else
	{
		curr_runtime_hndl->last_tested_pos = 0;
	}

	/* if 'del' char is first , just remove it, also remove
	 *  1 additional char from buffer as second step from
	 *  step 1 of buffer that 'del' char is not first
	 */
	num_of_removed_chars++;
	return num_of_removed_chars;
}


static uint8_t test_validity_of_header(uint8_t *buff, size_t EOL_pos)
{
	if (START_OF_CMD_POS > EOL_pos) return 0;

	if ((HEADER_CHAR_ON != buff[START_OF_CMD_POS - 1]) &&
			(HEADER_CHAR_OFF != buff[START_OF_CMD_POS - 1]) )
	{
		return 0;
	}
	return 1;
}


static uint8_t *extract_command_from_line(
		uint8_t *buff, size_t *p_EOL_pos, uint8_t *cmd_header_found)
{
	*cmd_header_found = 0;
	if (1 > *p_EOL_pos) return NULL;

	if ((HEADER_CHAR_ON != buff[HEADER_SUPPRESS_ECHO_POS]) &&
			(HEADER_CHAR_OFF != buff[HEADER_SUPPRESS_ECHO_POS]) )
	{
		return buff; // if no header
	}

	*cmd_header_found = 1;
	if (0 == test_validity_of_header(buff, *p_EOL_pos))
	{
		shell_frontend_reply_data(
			(uint8_t*)"invalid header\r\n", sizeof("invalid header\r\n") - 1);
		return NULL;
	}

	*p_EOL_pos = (*p_EOL_pos) - START_OF_CMD_POS;
	return &buff[START_OF_CMD_POS];
}


static void execute_batch(struct shell_frontend_cfg_t *config_handle,
		uint8_t *input_buffer, size_t total_length)
{
	uint32_t saved_last_tested_pos;
	saved_last_tested_pos = curr_runtime_hndl->last_tested_pos;
	curr_runtime_hndl->last_tested_pos = 0;
	process_buffer(config_handle, input_buffer, total_length);
	curr_runtime_hndl->last_tested_pos = saved_last_tested_pos;
}


static void load_preset(
		struct shell_frontend_cfg_t *config_handle,
		struct msg_load_preset_t *p_preset)
{
#ifdef CONFIG_INCLUDE_SHELL_PRESETS
	uint8_t retries;
	uint8_t ret_val;
	struct shell_presets_api_get_buffer_t  shell_presets_api_get_buffer;

	ret_val = 1;
	retries = 50;
	shell_presets_api_get_buffer.num_of_preset = p_preset->num_of_preset;
	while (ret_val && retries--)
	{
		ret_val = DEV_IOCTL( p_preset->shell_preset_pdev,
				IOCTL_SHELL_PRESETS_GET_PRESET_BUFFER,
				&shell_presets_api_get_buffer);
		if (SHELL_PRESET_VALID_PRESET_DOES_NOT_EXIST == ret_val)
		{
			break;
		}
		os_delay_ms(10);
	}

	if (SHELL_PRESET_VALID_PRESET_DOES_NOT_EXIST == ret_val)
	{
		#define prst_err1 "error: preset no found\n"
		shell_frontend_reply_data((uint8_t*)prst_err1, sizeof(prst_err1) - 1);
		return;
	}
	else if (0 != ret_val)
	{
		#define prst_err2 "error: cannot load preset\n"
		shell_frontend_reply_data((uint8_t*)prst_err2, sizeof(prst_err2) - 1);
		return;
	}
	execute_batch(config_handle, shell_presets_api_get_buffer.preset_buffer,
			shell_presets_api_get_buffer.preset_size);
	DEV_IOCTL(p_preset->shell_preset_pdev,
			IOCTL_SHELL_PRESETS_RELEASE_PRESET_BUFFER);
#endif
}


static void save_to_preset_if_needed(
	struct shell_frontend_cfg_t *config_handle, uint8_t *buff, size_t buff_size)
{
	struct dev_desc_t *cmd_save_dev;
	cmd_save_dev = config_handle->cmd_save_dev;
	if (cmd_save_dev )
	{
		DEV_WRITE(cmd_save_dev, buff, buff_size);
	}
}



#define SUMS_OF_EOLS	('\r' + '\n')

static void consume_line(struct shell_frontend_cfg_t *config_handle,
					uint8_t *buff, size_t EOL_pos)
{
	struct dev_desc_t *shell_backend_dev;
	struct rcvd_cmd_t  rcvd_cmd;
	uint8_t  cmd_header_found;
	uint8_t *pCmd;

	curr_runtime_hndl->last_tested_pos = 0;
	if ((0 == EOL_pos) &&
			( SUMS_OF_EOLS  == (buff[0] + curr_runtime_hndl->last_EOL_char)))
	{
		curr_runtime_hndl->last_EOL_char = 0;
		return;
	}

	curr_runtime_hndl->last_EOL_char = buff[EOL_pos];
	pCmd = extract_command_from_line(buff, &EOL_pos, &cmd_header_found);

	if (NULL != pCmd)
	{
		if ((0 == cmd_header_found) ||
				(HEADER_CHAR_OFF == buff[HEADER_SUPPRESS_ECHO_POS]))
		{
			shell_frontend_reply_data((uint8_t*)"\r\n", 2);
		}
		shell_backend_dev = config_handle->shell_backend_dev;
		if (shell_backend_dev )
		{
			rcvd_cmd.reply_dev = curr_tx_dev;
			rcvd_cmd.cmd_buf = pCmd;
			rcvd_cmd.cmd_len = EOL_pos + 1;
			DEV_IOCTL(
				shell_backend_dev, IOCTL_SHELL_NEW_FRAME_RECEIVED, &rcvd_cmd);
		}

		// in case of windows it will save /r/n
		if (SUMS_OF_EOLS == (pCmd[EOL_pos] + pCmd[EOL_pos + 1])) EOL_pos++;
		save_to_preset_if_needed(config_handle, pCmd, EOL_pos + 1);
	}

	if (curr_runtime_hndl->load_preset_flag)
	{
		load_preset(config_handle, &curr_msg_load_preset);
	}

	if (cmd_header_found)
	{
		if (HEADER_CHAR_ON == buff[HEADER_ADD_EOF_MARK_POS])
		{
			shell_frontend_reply_data(
					(uint8_t*)EOF_MARKER_STR, sizeof(EOF_MARKER_STR) - 1);
		}
	}
	else
	{
		shell_frontend_reply_data((uint8_t*)"\r\n>", 3);
	}
}


static size_t process_data_ASCII(struct shell_frontend_cfg_t *config_handle,
		uint8_t *buff, size_t total_length)
{
	size_t curr_buff_pos;
	uint8_t curr_char;

	curr_buff_pos = curr_runtime_hndl->last_tested_pos;
	while (curr_buff_pos < total_length)
	{
		curr_char = buff[curr_buff_pos];
		switch(curr_char)
		{
		case '\r':
		case '\n':
			send_echo(buff, curr_buff_pos, curr_runtime_hndl->last_tested_pos);
			consume_line(config_handle, buff, curr_buff_pos);
			return curr_buff_pos + 1; // add first EOL char
		case 0x08:
		case 0x7f:
			send_echo(buff, curr_buff_pos, curr_runtime_hndl->last_tested_pos);
			curr_runtime_hndl->last_EOL_char = 0;
			return process_backspace(buff, curr_buff_pos);
		default:
			curr_buff_pos++;
			break;
		}
	}

	send_echo(buff, curr_buff_pos, curr_runtime_hndl->last_tested_pos);
	curr_runtime_hndl->last_EOL_char = 0; // we are here if no EOL found
	curr_runtime_hndl->last_tested_pos = curr_buff_pos;
	return 0;
}


static uint8_t parse_bin_header(uint8_t *buff, size_t msg_length,
		size_t *p_msg_envelope_length, uint16_t *cmd_id)
{
	uint8_t cmd_flags;
	size_t msg_envelope_length;

	msg_envelope_length = 0;
	cmd_flags = buff[2];
	if (0 == (cmd_flags & BIN_FLAGS_EXTENDED_CMD))
	{
		*cmd_id = buff[3];
		msg_envelope_length += 4;
	}
	else
	{
		#ifdef CONFIG_INCLUDE_UBOOT_SHELL
			curr_runtime_hndl->mode = SHELL_FRONTEND_MODE_ASCII;
		#endif
		send_bin_reply_head(0, BIN_CMD_REPLY_NOT_SUPPORTED_FLAG);
		return 1;
	}

	if (0 == (cmd_flags & BIN_FLAGS_SUPPRESS_REPLY_HEADER))
	{
		suppress_bin_reply_header = 0;
	}
	else
	{
		suppress_bin_reply_header = 1;
	}


	if (0 != (cmd_flags & BIN_FLAGS_INTEGRITY_STAMP_ADDED))
	{
		size_t  i;
		uint32_t stamp;

		msg_envelope_length += 4;
		if (msg_envelope_length > msg_length)
		{
			send_bin_reply_head(0, BIN_CMD_REPLY_MSG_TOO_SHORT);
			return 1;
		}

//		stamp = msg_length;
		stamp = 0;
		for (i = (msg_length - 1) ; i >= (msg_length - 4); i--)
		{
			stamp = stamp << 8;
			stamp += buff[i];
		}
		if (msg_length != stamp)
		{
			send_bin_reply_head(0, BIN_CMD_REPLY_BAD_INTEGRITY_STAMP);
			return 1;
		}
		use_stamp_in_bin_msg = 1;
	}
	else
	{
		use_stamp_in_bin_msg = 0;
	}
	*p_msg_envelope_length = msg_envelope_length;
	return 0;
}


static void find_and_run_bin_cmd(struct shell_frontend_cfg_t *config_handle,
		uint16_t cmd_id, uint8_t *buff, size_t msg_length,
		size_t msg_envelope_length)
{
	size_t i;
	size_t cmd_length;
	struct shell_bin_cmd_struct_t *bin_cmd;

	remain_bin_reply_size = 0;
	for (i = 0; i < bin_cmd_table_size; i++)
	{
		bin_cmd = bin_cmd_table[i];
		if (bin_cmd->cmd_id == cmd_id)
		{
			cmd_length = msg_length - msg_envelope_length;
			bin_cmd->bin_cmd_func(&buff[4], cmd_length);
			save_to_preset_if_needed(config_handle, buff, msg_length);
			break;
		}
	}
	if (bin_cmd_table_size == i)
	{
		send_bin_reply_head(0, BIN_CMD_REPLY_STATUS_CMD_NOT_FOUND);
	}
}


static size_t process_data_binary(struct shell_frontend_cfg_t *config_handle,
		uint8_t *buff, size_t total_length)
{
	size_t msg_envelope_length;
	size_t msg_length;
	uint16_t cmd_id;
	size_t preamble_size = 0;
	size_t consumed_num_of_bytes;

	if (total_length < PREAMBLE_SIZE) return 0;

#if 0//#ifndef CONFIG_INCLUDE_UBOOT_SHELL // will be enabled later
	if (0 != memcmp(buff, preamble_patern, PREAMBLE_SIZE))
	{
		return 1; // dismiss just 1 char, preamble not found
	}
	buff += PREAMBLE_SIZE;
	total_length -= PREAMBLE_SIZE;
	preamble_size = PREAMBLE_SIZE;
#endif

	if (total_length < BIN_NON_EXTENDED_HEADER_SIZE) return 0;

	msg_length = buff[0] + (buff[1] << 8);

	if (CONFIG_SHELL_FRONTEND_MAX_BINARY_MESSAGE_LEN < msg_length)
	{
		consumed_num_of_bytes = 1; // dismiss just 1 char
		goto end_binary_msg_process;
	}

	if (total_length < msg_length) return 0;// still not enough data

	if (0 != parse_bin_header(buff, msg_length, &msg_envelope_length, &cmd_id))
	{
		consumed_num_of_bytes = msg_length;
		goto end_binary_msg_process;
	}

	find_and_run_bin_cmd(
			config_handle, cmd_id, buff, msg_length, msg_envelope_length);
	consumed_num_of_bytes = msg_length;

	pad_remaining_data();

	end_binary_msg_process:
#ifdef CONFIG_INCLUDE_UBOOT_SHELL
	curr_runtime_hndl->mode = SHELL_FRONTEND_MODE_ASCII;
#endif
	consumed_num_of_bytes += preamble_size;
	return consumed_num_of_bytes;
}


static size_t process_buffer(struct shell_frontend_cfg_t *config_handle,
		uint8_t *input_buffer, size_t total_length)
{
	size_t bytes_consumed;
	size_t total_bytes_consumed;

	total_bytes_consumed = 0;
	while (total_length)
	{
		curr_runtime_hndl->load_preset_flag = 0;
		if (SHELL_FRONTEND_MODE_ASCII == curr_runtime_hndl->mode)
		{
			bytes_consumed = process_data_ASCII(
					config_handle, input_buffer, total_length);
		}
		else
		{
			bytes_consumed = process_data_binary(
					config_handle, input_buffer, total_length);
		}

		// if no data consume then no full frame/EOL exists
		if (0 == bytes_consumed) break;

		input_buffer += bytes_consumed;
		total_length -= bytes_consumed;
		total_bytes_consumed += bytes_consumed;
	}
	return total_bytes_consumed;
}


static void process_msg_from_rx_device(
		struct shell_frontend_cfg_t *config_handle)
{
	struct ioctl_get_data_buffer_t data_buffer_info;
	size_t bytes_consumed;
	struct dev_desc_t * curr_rx_dev;

	curr_rx_dev = config_handle->server_rx_dev;
	curr_tx_dev = config_handle->server_tx_dev;

	DEV_IOCTL(curr_rx_dev,
			IOCTL_GET_AND_LOCK_DATA_BUFFER, &data_buffer_info);

	if(0 != data_buffer_info.bufferWasOverflowed)
	{
		curr_runtime_hndl->last_tested_pos = 0;
		curr_runtime_hndl->last_EOL_char = 0;
	}

	bytes_consumed = process_buffer(config_handle,
			data_buffer_info.pBufferStart, data_buffer_info.TotalLength);
	DEV_IOCTL(curr_rx_dev, IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER,
			(void *)((uint32_t)bytes_consumed));
	DEV_IOCTL(curr_rx_dev, IOCTL_SET_UNLOCK_DATA_BUFFER);
}


static void in_task_init(void)
{
	xQueue = os_create_queue(
			CONFIG_SHELL_FRONTEND_MAX_QUEUE_LEN, sizeof(struct queue_msg_t) );

	if ( NULL == xQueue  )
	{
		CRITICAL_ERROR("cannot create shell frontend queue");
	}
}


static void main_task_function(uint8_t wait_for_queue_event)
{
	struct queue_msg_t msg;
	struct shell_frontend_cfg_t *config_handle;
	struct dev_desc_t *pdev;
	uint8_t queue_ret_val;

	if (wait_for_queue_event)
	{
		queue_ret_val = os_queue_receive_infinite_wait( xQueue, &msg);
	}
	else
	{
		queue_ret_val = os_queue_receive_with_timeout( xQueue, &msg, 0);
	}
	if (OS_QUEUE_RECEIVE_SUCCESS != queue_ret_val) return;

	pdev = msg.shell_frontend_target_pdev;
	config_handle = DEV_GET_CONFIG_DATA_POINTER(shell_frontend, pdev);
	curr_runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(shell_frontend, pdev);

	curr_tx_dev = NULL;
	switch (msg.msg_type)
	{
	case MSG_TYPE_FROM_RX_DEVICE:
		process_msg_from_rx_device(config_handle);
		break;
	case MSG_TYPE_LOAD_PRESET:
		load_preset(config_handle, &msg.msg_load_preset);
		break;
	case MSG_TYPE_EXECUTE_BATCH:
		execute_batch(config_handle,
			msg.msg_batch.batch_buffer, msg.msg_batch.batch_buffer_size);
		break;
	default:
		break;
	}
	curr_runtime_hndl = NULL;
	os_stack_test();
}


/**
 * shell_frontend_Task()
 *
 * return:
 */
static void shell_frontend_Task( void *pvParameters )
{
	in_task_init();
	while (1)
	{
		main_task_function(1);
	}
}


static uint8_t send_load_preset(struct dev_desc_t *adev,
		struct shell_frontend_load_preset_t *preset_params)
{
	struct queue_msg_t  msg;

	if (NULL == xQueue) return 1;

	if (NULL != curr_runtime_hndl)
	{
		/* we are in the middle of processing shell command, so this
		 * code may be called from shell command, so we cannot send
		 * message to queue because it can stuck the shell task (even if
		 * we use non-blocking send_queue(full queue will stay full forever)).
		 * so return the error and calling task should try it again.
		 * set preset from shell command should not use this function,
		 * use shell_frontend_load_preset_from_shell_cmd() instead
		 */
		return 1;
	}

	msg.msg_type = MSG_TYPE_LOAD_PRESET;
	msg.shell_frontend_target_pdev = adev;
	msg.msg_load_preset.shell_preset_pdev = preset_params->shell_preset_pdev;
	msg.msg_load_preset.num_of_preset = preset_params->num_of_preset;
	if (OS_QUEUE_SEND_SUCCESS !=
			os_queue_send_infinite_wait( xQueue, ( void * ) &msg))
	{
		return 1;
	}
	return 0;
}


static uint8_t send_batch(struct dev_desc_t *adev,
		struct shell_frontend_batch_t *batch_params)
{
	struct queue_msg_t  msg;

	if (NULL == xQueue) return 1;

	if (NULL != curr_runtime_hndl)
	{
		/* we are in the middle of processing shell command, so this
		 * code may be called from shell command, so we cannot send
		 * message to queue because it can stuck the shell task (even if
		 * we use non-blocking send_queue(full queue will stay full forever)).
		 * so return the error and calling task should try it again.
		 * set preset from shell command should not use this function,
		 * use shell_frontend_load_preset_from_shell_cmd() instead
		 */
		return 1;
	}

	msg.msg_type = MSG_TYPE_EXECUTE_BATCH;
	msg.shell_frontend_target_pdev = adev;
	msg.msg_batch.batch_buffer = batch_params->batch_buffer;
	msg.msg_batch.batch_buffer_size = batch_params->batch_buffer_size;
	if (OS_QUEUE_SEND_SUCCESS !=
			os_queue_send_infinite_wait( xQueue, ( void * ) &msg))
	{
		return 1;
	}
	return 0;
}


/**
 * shell_frontend_ioctl()
 *
 * return:
 */
static uint8_t shell_frontend_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct shell_frontend_cfg_t *config_handle;
	struct shell_frontend_runtime_instance_t  *runtime_handle;
	struct dev_desc_t *   server_dev ;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(shell_frontend, adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(shell_frontend, adev);

	switch(aIoctl_num)
	{
#ifdef CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION
	case IOCTL_SET_SERVER_DEVICE :
		{
			server_dev = (struct dev_desc_t *)aIoctl_param1;
			if(NULL != server_dev)
			{
				DEV_IOCTL(server_dev, IOCTL_SET_ISR_CALLBACK_DEV, (void*)adev);
			}

			config_handle->server_tx_dev = server_dev;
			config_handle->server_rx_dev = server_dev;
		}
		break;
#endif
	case IOCTL_DEVICE_START :
		if (0 == task_is_running)
		{
			task_is_running = 1;
			os_create_task("shell_frontend_task", shell_frontend_Task, NULL,
				SHELL_FRONTEND_TASK_STACK_SIZE, SHELL_FRONTEND_TASK_PRIORITY);

			#ifdef CONFIG_NO_OS
				in_task_init();
			#endif
		}
		server_dev = config_handle->server_tx_dev;
		DEV_IOCTL(server_dev, IOCTL_DEVICE_START);
		server_dev = config_handle->server_rx_dev;
		DEV_IOCTL(server_dev, IOCTL_DEVICE_START);
		#ifdef CONFIG_INCLUDE_UBOOT_SHELL
				runtime_handle->mode = SHELL_FRONTEND_MODE_ASCII;
		#else
				runtime_handle->mode = SHELL_FRONTEND_MODE_BINARY;
		#endif
		break;
	case IOCTL_SHELL_FRONTEND_LOAD_PRESET:
		return send_load_preset(adev, aIoctl_param1);
		break;
	case IOCTL_SHELL_FRONTEND_RUN_BATCH:
		return send_batch(adev, aIoctl_param1);
		break;
	case IOCTL_CALL_MAIN_THREAD_FUNCTION_OF_DEVICE:
		main_task_function(0);
		break;
	default :
		return 1;
	}
	return 0;
}

#define MODULE_NAME                 shell_frontend
#define MODULE_IOCTL_FUNCTION       shell_frontend_ioctl
#define MODULE_CALLBACK_FUNCTION    shell_frontend_callback

#define MODULE_CONFIGURABLE_PARAMS_ARRAY { \
		{"shell_frontend_server", IOCTL_SET_SERVER_DEVICE, IOCTL_VOID, \
				DEV_PARAM_TYPE_PDEVICE, MAPPED_SET_DUMMY_PARAM() }   \
		}

#include "add_module.h"
