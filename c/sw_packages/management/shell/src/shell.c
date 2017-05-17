/*
 *
 * file :   shell.c
 *
 *
 */



/********  includes *********************/
#include "src/_shell_prerequirements_check.h"

#include "shell_api.h"

#include "shell.h"

#include "shell_add_component.h"

#if SHELL_CONFIG_MAX_RX_BUFFER_SIZE <= (1<<8)
	typedef uint8_t shel_rx_int_size_t;
#else
	#if SHELL_CONFIG_MAX_RX_BUFFER_SIZE <= (1<<16)
		typedef uint16_t shel_rx_int_size_t;
	#else
		typedef uint32_t shel_rx_int_size_t;
	#endif
#endif


/********  defines *********************/



/********  types  *********************/
typedef struct
{

	struct dev_desc_t * pdev;
} xMessage_t;



/********  externals *********************/

extern int run_command(const char *cmd, int flag);


/********  local variables *********************/
static uint8_t task_is_running=0;

static const char erase_seq[] = "\b \b";		/* erase sequence	*/
static const char eol_seq[] = "\r\n";		/* end of line sequence	*/
static uint8_t EOF_MARKER_STR[]="\r\n~2@5\r\n";


#define HEADER_CHAR_ON		'+'
#define HEADER_CHAR_OFF		'-'

typedef enum
{
	HEADER_SUPPRESS_ECHO_POS, // must be on first position
	HEADER_ADD_EOF_MARK_POS,
	START_OF_CMD_POS
}Header_positions_t;


static os_queue_t xQueue = NULL;
struct dev_desc_t * gCurrReplyDev;

#define SHELL_PRINTF_BUF_LENGTH	 64
static uint8_t shell_printf_buffer[SHELL_PRINTF_BUF_LENGTH];
#include <stdarg.h>

int printf(const char *Format, ...)
{
	va_list args;
	int retVal;

	//return ;
	va_start(args,Format);
	retVal = vsnprintf((char*)shell_printf_buffer,
			SHELL_PRINTF_BUF_LENGTH,(char*)Format, args);
	va_end(args);

	SHELL_REPLY_DATA( shell_printf_buffer, retVal);

	return 0;
}

/**
 * shell_callback()
 *
 * return:
 */
uint8_t shell_callback(struct dev_desc_t *adev ,const uint8_t aCallback_num
		, void * aCallback_param1, void * aCallback_param2)
{
	xMessage_t  queueMsg;

	if (NULL == xQueue)
	{
		return 1;
	}


	queueMsg.pdev = adev;

	//xQueueSendFromISR( xQueue, ( void *)&queueMsg, &xHigherPriorityTaskWoken);
	os_queue_send_immediate( xQueue, ( void * ) &queueMsg);

	return 0;
}

#define SUMS_OF_EOLS	('\r'+'\n')

static uint8_t get_valid_line(struct shell_runtime_instance_t *runtime_handle,
		uint8_t **pBuffer, shel_rx_int_size_t *p_total_length,
		shel_rx_int_size_t *p_bytesConsumed)
{
	uint8_t validCommadFound;
	uint8_t *pBufferStart;
	shel_rx_int_size_t total_length;
	shel_rx_int_size_t bytesConsumed;
	shel_rx_int_size_t curr_buff_pos;
	shel_rx_int_size_t lastTestedBytePos;
	shel_rx_int_size_t endOfLastPrintfPos;
	uint8_t curr_char;
	uint8_t eol_char;

	total_length = *p_total_length;
	pBufferStart = *pBuffer;
	bytesConsumed = *p_bytesConsumed;

	lastTestedBytePos = runtime_handle->lastTestedBytePos;
	endOfLastPrintfPos = lastTestedBytePos;
	curr_buff_pos = lastTestedBytePos;

	eol_char = runtime_handle->lastEOLchar;

	validCommadFound = 0;
	while ((curr_buff_pos < total_length) && (0 == validCommadFound))
	{
		curr_char = pBufferStart[curr_buff_pos];
		switch(curr_char)
		{
		case '\r':
		case '\n':
			if ( SUMS_OF_EOLS  == (curr_char + eol_char))
			{
				pBufferStart++;
				total_length--;
				bytesConsumed++;
			}
			else
			{
				validCommadFound = 1;
			}
			break;
		case 0x08:
		case 0x7f:
			if(curr_buff_pos)
			{
				if (HEADER_CHAR_ON != pBufferStart[HEADER_SUPPRESS_ECHO_POS])
				{
					SHELL_REPLY_DATA(&pBufferStart[endOfLastPrintfPos],
										curr_buff_pos - endOfLastPrintfPos);
					SHELL_REPLY_DATA(erase_seq,sizeof(erase_seq)-1);
				}
				//shift buffer right before DEL
				memmove(&pBufferStart[2],pBufferStart, curr_buff_pos-1);

				curr_buff_pos -= 1;
				endOfLastPrintfPos = curr_buff_pos;

				//step 1 :
				pBufferStart++;
				bytesConsumed++;
				total_length--;
			}

			/* if 'del' char is first , just remove it, also remove
			 *  1 additional char from buffer as second step from
			 *  step 1 of buffer that 'del' char is not first
			 */
			pBufferStart++;
			bytesConsumed++;
			total_length--;
			break;
		default:
			curr_buff_pos++;
			break;
		}

		//lastEOLchar only valid for first loop
		eol_char = 0;
	}


	if (HEADER_CHAR_ON != pBufferStart[HEADER_SUPPRESS_ECHO_POS])
	{
		SHELL_REPLY_DATA( &pBufferStart[endOfLastPrintfPos],
								curr_buff_pos - endOfLastPrintfPos);
	}

	if (validCommadFound)
	{
		eol_char = curr_char;
		lastTestedBytePos = 0;
	}
	else
	{
		lastTestedBytePos = curr_buff_pos;
	}
	runtime_handle->lastEOLchar = eol_char;
	runtime_handle->lastTestedBytePos = lastTestedBytePos;

	*p_total_length = total_length ;
	*pBuffer = pBufferStart;
	*p_bytesConsumed = bytesConsumed;

	return curr_buff_pos;
}


static uint8_t *extract_command_from_line(
		uint8_t *pBufferStart, shel_rx_int_size_t EOL_pos)
{
	if ((HEADER_CHAR_ON == pBufferStart[HEADER_SUPPRESS_ECHO_POS]) ||
			(HEADER_CHAR_OFF == pBufferStart[HEADER_SUPPRESS_ECHO_POS]) )
	{
		if (START_OF_CMD_POS > EOL_pos)
		{
			return NULL;
		}

		if ((HEADER_CHAR_ON == pBufferStart[START_OF_CMD_POS - 1]) ||
				(HEADER_CHAR_OFF == pBufferStart[START_OF_CMD_POS - 1]) )
		{
			return &pBufferStart[START_OF_CMD_POS];
		}
		else
		{
			SHELL_REPLY_DATA("invalid header\r\n",
							sizeof("invalid header\r\n")-1);
			return NULL;
		}
	}
	else
	{
		return pBufferStart;
	}
	return NULL;
}


static void consume_line(
		struct shell_instance_t *config_handle, uint8_t *pBufferStart,
		shel_rx_int_size_t total_length, shel_rx_int_size_t EOL_pos)
{
	struct dev_desc_t *   callback_dev;
	uint8_t *pCmd;

	pCmd = extract_command_from_line(pBufferStart, EOL_pos);

	if (NULL != pCmd)
	{
		if (pBufferStart == pCmd )
		{
			SHELL_REPLY_DATA( (const uint8_t*)"\r\n",2);
		}
		callback_dev = config_handle->callback_dev;
		if (callback_dev )
		{
			DEV_CALLBACK_2_PARAMS(callback_dev , CALLBACK_DATA_RECEIVED,
					pCmd, (void*)(EOL_pos + 1));
		}
	}


	if (pBufferStart != pCmd )
	{
		if (HEADER_CHAR_ON == pBufferStart[HEADER_ADD_EOF_MARK_POS])
		{
			SHELL_REPLY_DATA( EOF_MARKER_STR,sizeof(EOF_MARKER_STR) - 1);
		}
	}
	else
	{
		SHELL_REPLY_DATA( (const uint8_t*)"\r\n>",3);
	}
}

/**
 * Shell_Task()
 *
 * return:
 */
static void Shell_Task( void *pvParameters )
{
	xMessage_t pxRxedMessage;
	shel_rx_int_size_t total_length;
	shel_rx_int_size_t bytesConsumed;
	shel_rx_int_size_t EOL_pos;
	uint8_t *pBufferStart;
	struct ioctl_get_data_buffer_t data_buffer_info;
	struct shell_runtime_instance_t  *runtime_handle;
	struct shell_instance_t *config_handle;
	struct dev_desc_t *   curr_rx_dev;
	struct dev_desc_t *   curr_dev;

	xQueue = os_create_queue( CONFIG_SHELL_MAX_QUEUE_LEN, sizeof(xMessage_t) );

    if( 0 == xQueue  ) return ;

	for( ;; )
	{
		if( OS_QUEUE_RECEIVE_SUCCESS ==
				os_queue_receive_infinite_wait( xQueue, &( pxRxedMessage )) )
		{
			curr_dev = pxRxedMessage.pdev;
			config_handle = DEV_GET_CONFIG_DATA_POINTER(curr_dev);
			runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(curr_dev);
			gCurrReplyDev = config_handle->server_tx_dev;
			curr_rx_dev = config_handle->server_rx_dev;

			DEV_IOCTL(curr_rx_dev,
					IOCTL_GET_AND_LOCK_DATA_BUFFER, &data_buffer_info);

			total_length = data_buffer_info.TotalLength ;
			pBufferStart = data_buffer_info.pBufferStart ;

			if(0 != data_buffer_info.bufferWasOverflowed)
			{
				runtime_handle->lastTestedBytePos = 0;
				runtime_handle->lastEOLchar = 0;
			}

			while (total_length)
			{
				bytesConsumed=0;

				// will return total_length if EOL not found
				EOL_pos = get_valid_line(runtime_handle,
						&pBufferStart, &total_length, &bytesConsumed);

				if (EOL_pos < total_length)
				{
					consume_line(config_handle, pBufferStart,
							total_length, EOL_pos);

					EOL_pos++;// add first EOL char
					bytesConsumed += EOL_pos;
				}
				pBufferStart = &pBufferStart[EOL_pos];
				total_length -= EOL_pos;

				DEV_IOCTL(curr_rx_dev,IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER,
						(void *)((uint32_t)bytesConsumed));
			}
			DEV_IOCTL(curr_rx_dev, IOCTL_SET_UNLOCK_DATA_BUFFER ,(void *) 0);

		}
		os_stack_test();
	}
}


/**
 * shell_ioctl()
 *
 * return:
 */
uint8_t shell_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct shell_instance_t *config_handle;
	struct dev_desc_t *   server_dev ;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);

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

			config_handle->server_tx_dev=server_dev;
			config_handle->server_rx_dev=server_dev;
		}
		break;
	case IOCTL_SET_CALLBACK_DEV:
		config_handle->callback_dev =(struct dev_desc_t *) aIoctl_param1;
		break;
#endif
	case IOCTL_DEVICE_START :
		if(0==task_is_running)
		{
			task_is_running=1;
			os_create_task("shell_task",Shell_Task,
					NULL , SHELL_TASK_STACK_SIZE , SHELL_TASK_PRIORITY);
			server_dev = config_handle->server_tx_dev;
			DEV_IOCTL_0_PARAMS(server_dev , IOCTL_DEVICE_START );
			server_dev = config_handle->server_rx_dev;
			DEV_IOCTL_0_PARAMS(server_dev , IOCTL_DEVICE_START );

		}
		break;

	default :
		return 1;
	}
	return 0;
}
