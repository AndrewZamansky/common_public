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


struct xMessage_t {
	struct dev_desc_t * pdev;
};



static uint8_t task_is_running=0;

static const char erase_seq[] = "\b \b";   /* erase sequence */
static char EOF_MARKER_STR[] = "\r\n~2@5\r\n";


#define HEADER_CHAR_ON    '+'
#define HEADER_CHAR_OFF   '-'

enum Header_positions_t {
	HEADER_SUPPRESS_ECHO_POS, // must be on first position
	HEADER_ADD_EOF_MARK_POS,
	START_OF_CMD_POS
};

static struct dev_desc_t *   curr_tx_dev;

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
	struct xMessage_t  queueMsg;

	if (NULL == xQueue)
	{
		return 1;
	}


	queueMsg.pdev = adev;

	//xQueueSendFromISR( xQueue, ( void *)&queueMsg, &xHigherPriorityTaskWoken);
	os_queue_send_without_wait( xQueue, ( void * ) &queueMsg);

	return 0;
}


static void reply_data(const char *data, size_t len)
{
	if (NULL == curr_tx_dev) return ;
	DEV_WRITE(curr_tx_dev, (uint8_t*)data, len);
}


static void send_echo(
		uint8_t *buff, size_t curr_buff_pos, size_t last_printed_out_pos)
{
	if (0 == curr_buff_pos) return;

	if (HEADER_CHAR_ON != buff[HEADER_SUPPRESS_ECHO_POS])
	{
		reply_data((char*)&buff[last_printed_out_pos],
							curr_buff_pos - last_printed_out_pos);
	}
}


static uint8_t process_backspace(
		struct shell_frontend_runtime_instance_t *runtime_handle,
		uint8_t *buff, size_t curr_buff_pos)
{
	size_t num_of_removed_chars;
	num_of_removed_chars = 0;
	if (curr_buff_pos)
	{
		if (HEADER_CHAR_ON != buff[HEADER_SUPPRESS_ECHO_POS])
		{
			reply_data(erase_seq, sizeof(erase_seq) - 1);
		}
		//shift buffer right before DEL
		memmove(&buff[2], buff, curr_buff_pos - 1);
		runtime_handle->last_tested_pos = curr_buff_pos - 1;
		num_of_removed_chars++;
	}
	else
	{
		runtime_handle->last_tested_pos = 0;
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
		reply_data("invalid header\r\n", sizeof("invalid header\r\n")-1);
		return NULL;
	}

	*p_EOL_pos = (*p_EOL_pos) - START_OF_CMD_POS;
	return &buff[START_OF_CMD_POS];
}


#define SUMS_OF_EOLS	('\r' + '\n')

static void consume_line(struct shell_frontend_cfg_t *config_handle,
					struct shell_frontend_runtime_instance_t *runtime_handle,
					uint8_t *buff, size_t EOL_pos)
{
	struct dev_desc_t *shell_backend_dev;
	struct dev_desc_t *cmd_save_dev;
	struct rcvd_cmd_t  rcvd_cmd;
	uint8_t  cmd_header_found;
	uint8_t *pCmd;

	runtime_handle->last_tested_pos = 0;
	if ((0 == EOL_pos) &&
			( SUMS_OF_EOLS  == (buff[0] + runtime_handle->last_EOL_char)))
	{
		runtime_handle->last_EOL_char = 0;
		return;
	}

	runtime_handle->last_EOL_char = buff[EOL_pos];
	pCmd = extract_command_from_line(buff, &EOL_pos, &cmd_header_found);

	if (NULL != pCmd)
	{
		if ((0 == cmd_header_found) ||
				(HEADER_CHAR_OFF == buff[HEADER_SUPPRESS_ECHO_POS]))
		{
			reply_data("\r\n", 2);
		}
		shell_backend_dev = config_handle->shell_backend_dev;
		if (shell_backend_dev )
		{
			rcvd_cmd.reply_dev = config_handle->server_tx_dev;
			rcvd_cmd.cmd_buf = pCmd;
			rcvd_cmd.cmd_len = EOL_pos + 1;
			DEV_IOCTL_1_PARAMS(
				shell_backend_dev, IOCTL_SHELL_NEW_FRAME_RECEIVED, &rcvd_cmd);
		}
		cmd_save_dev = config_handle->cmd_save_dev;
		if (cmd_save_dev )
		{
			DEV_WRITE(cmd_save_dev, pCmd, EOL_pos + 1);
		}
	}

	if (cmd_header_found)
	{
		if (HEADER_CHAR_ON == buff[HEADER_ADD_EOF_MARK_POS])
		{
			reply_data( EOF_MARKER_STR, sizeof(EOF_MARKER_STR) - 1);
		}
	}
	else
	{
		reply_data("\r\n>", 3);
	}
}


static size_t process_data(struct shell_frontend_cfg_t *config_handle,
		struct shell_frontend_runtime_instance_t *runtime_handle,
		uint8_t *buff, size_t total_length)
{
	size_t curr_buff_pos;
	uint8_t curr_char;

	curr_buff_pos = runtime_handle->last_tested_pos;
	while (curr_buff_pos < total_length)
	{
		curr_char = buff[curr_buff_pos];
		switch(curr_char)
		{
		case '\r':
		case '\n':
			send_echo(buff, curr_buff_pos, runtime_handle->last_tested_pos);
			consume_line(config_handle, runtime_handle, buff, curr_buff_pos);
			return curr_buff_pos + 1; // add first EOL char
		case 0x08:
		case 0x7f:
			send_echo(buff, curr_buff_pos, runtime_handle->last_tested_pos);
			runtime_handle->last_EOL_char = 0;
			return process_backspace(runtime_handle, buff, curr_buff_pos);
		default:
			curr_buff_pos++;
			break;
		}
	}

	send_echo(buff, curr_buff_pos, runtime_handle->last_tested_pos);
	runtime_handle->last_EOL_char = 0; // we are here if no EOL found
	runtime_handle->last_tested_pos = curr_buff_pos;
	return 0;
}


/**
 * shell_frontend_Task()
 *
 * return:
 */
static void shell_frontend_Task( void *pvParameters )
{
	struct xMessage_t pxRxedMessage;
	size_t total_length;
	size_t bytes_consumed;
	uint8_t *pBufferStart;
	struct ioctl_get_data_buffer_t data_buffer_info;
	struct shell_frontend_runtime_instance_t  *runtime_handle;
	struct shell_frontend_cfg_t *config_handle;
	struct dev_desc_t *   curr_rx_dev;
	struct dev_desc_t *   curr_dev;

	xQueue = os_create_queue(
			CONFIG_SHELL_FRONTEND_MAX_QUEUE_LEN, sizeof(struct xMessage_t) );

	if ( 0 == xQueue  ) return ;

	while (1)
	{
		if ( OS_QUEUE_RECEIVE_SUCCESS !=
				os_queue_receive_infinite_wait( xQueue, &( pxRxedMessage )) )
		{
			continue;
		}

		curr_dev = pxRxedMessage.pdev;
		config_handle = DEV_GET_CONFIG_DATA_POINTER(shell_frontend, curr_dev);
		runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(shell_frontend, curr_dev);
		curr_rx_dev = config_handle->server_rx_dev;
		curr_tx_dev = config_handle->server_tx_dev;

		DEV_IOCTL(curr_rx_dev,
				IOCTL_GET_AND_LOCK_DATA_BUFFER, &data_buffer_info);

		total_length = data_buffer_info.TotalLength ;
		pBufferStart = data_buffer_info.pBufferStart ;

		if(0 != data_buffer_info.bufferWasOverflowed)
		{
			runtime_handle->last_tested_pos = 0;
			runtime_handle->last_EOL_char = 0;
		}

		while (total_length)
		{
			bytes_consumed = process_data(config_handle,
					runtime_handle, pBufferStart, total_length);

			// if no data consume then no full frame/EOL exists
			if (0 == bytes_consumed) break;

			pBufferStart += bytes_consumed;
			total_length -= bytes_consumed;

			DEV_IOCTL(curr_rx_dev, IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER,
					(void *)((uint32_t)bytes_consumed));
		}
		DEV_IOCTL(curr_rx_dev, IOCTL_SET_UNLOCK_DATA_BUFFER ,(void *) 0);

		os_stack_test();
	}
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
	struct dev_desc_t *   server_dev ;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(shell_frontend, adev);

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
		}
		server_dev = config_handle->server_tx_dev;
		DEV_IOCTL_0_PARAMS(server_dev, IOCTL_DEVICE_START );
		server_dev = config_handle->server_rx_dev;
		DEV_IOCTL_0_PARAMS(server_dev, IOCTL_DEVICE_START );

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
