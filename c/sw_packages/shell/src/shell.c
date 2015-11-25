/*
 *
 * file :   shell.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/
#include "shell_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_shell_prerequirements_check.h" // should be after {shell_config.h,dev_managment_api.h}

#include "shell_api.h"

#include "shell.h"

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

#define INSTANCE(hndl)	((shell_instance_t*)hndl)


/********  types  *********************/
typedef struct
{

	shell_instance_t *pShell_instance;
} xMessage_t;



/********  externals *********************/



/********  local defs *********************/
#if SHELL_CONFIG_USE_AS_DYNAMIC_INSTANCE > 0


static const dev_param_t Shell_Dev_Params[]=
{
		{IOCTL_SET_SERVER_DEVICE_BY_NAME , IOCTL_VOID , (uint8_t*)SHELL_API_SERVER_DEVICE_STR, NOT_FOR_SAVE},
};

#define MAX_NUM_OF_SHELL_SERVERS	3
shell_instance_t	shell_instances[MAX_NUM_OF_SHELL_SERVERS] = {{0}};
uint8_t usedShellInstances = 0;

#endif

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
pdev_descriptor_const gCurrReplyDev;

#define SHELL_PRINTF_BUF_LENGTH	 64
static uint8_t shell_printf_buffer[SHELL_PRINTF_BUF_LENGTH]; //  define your own buffer�s size
#include <stdarg.h>

int printf(const char *Format, ...)
{
	va_list args;
	int retVal;

	//return ;
	va_start(args,Format);
	retVal=vsnprintf((char*)shell_printf_buffer,SHELL_PRINTF_BUF_LENGTH,(char*)Format,args);
	va_end(args);

	SHELL_REPLY_DATA( shell_printf_buffer,retVal);

	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        sw_uart_wrapper_callback                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t shell_callback(void * const aHandle ,const uint8_t aCallback_num
		, void * aCallback_param1, void * aCallback_param2)
{
	xMessage_t  queueMsg;
	if (NULL == xQueue)
	{
		return 1;
	}


	queueMsg.pShell_instance = aHandle;

	//xQueueSendFromISR( xQueue, ( void * ) &queueMsg,  &xHigherPriorityTaskWoken );
	os_queue_send_immediate( xQueue, ( void * ) &queueMsg);

	return 0;
}

#define SUMS_OF_EOLS	('\r'+'\n')
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        Shell_Send_Task                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static void Shell_Task( void *pvParameters )
{
	xMessage_t pxRxedMessage;
	uint8_t endOfLastPrintfPos;
	uint8_t validCommadFound;
	shel_rx_int_size_t curr_buff_pos,total_length;
	shel_rx_int_size_t bytesConsumed;
	uint8_t *pBufferStart;
	uint8_t *pCmdStart;
	uint8_t curr_char,eol_char;
	ioctl_get_data_buffer_t data_buffer_info;
	shell_instance_t  *curr_shell_instance;

	xQueue = os_create_queue( SHELL_MAX_QUEUE_LEN , sizeof( xMessage_t ) );

    if( 0 == xQueue  ) return ;

	for( ;; )
	{
		if( OS_QUEUE_RECEIVE_SUCCESS == os_queue_receive_infinite_wait( xQueue, &( pxRxedMessage )) )
		{
			curr_shell_instance = pxRxedMessage.pShell_instance;
			gCurrReplyDev = curr_shell_instance->server_dev;

			DEV_IOCTL(gCurrReplyDev, IOCTL_GET_AND_LOCK_DATA_BUFFER , &data_buffer_info);

			total_length = data_buffer_info.TotalLength ;
			pBufferStart = data_buffer_info.pBufferStart ;

			if(0 == data_buffer_info.bufferWasOverflowed)
			{
				curr_buff_pos = curr_shell_instance->lastTestedBytePos;
				eol_char = curr_shell_instance->lastEOLchar;
			}
			else
			{
				curr_buff_pos = 0;
				eol_char = 0;
			}
			endOfLastPrintfPos=curr_buff_pos;

			while (curr_buff_pos < total_length)
			{
				bytesConsumed=0;
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
								validCommadFound=1;
							}
							break;
						case 0x08:
						case 0x7f:
							if(curr_buff_pos)
							{
								if (HEADER_CHAR_ON != pBufferStart[HEADER_SUPPRESS_ECHO_POS])
								{
									SHELL_REPLY_DATA(&pBufferStart[endOfLastPrintfPos] , curr_buff_pos - endOfLastPrintfPos);
									SHELL_REPLY_DATA(erase_seq,sizeof(erase_seq)-1);
								}
								//shift buffer right before DEL
								memmove(&pBufferStart[2],pBufferStart,curr_buff_pos-1);
//								for(i=curr_buff_pos ; i>1 ; i--)
//								{
//									pBufferStart[i]=pBufferStart[i-2];
//								}

								curr_buff_pos-=1;
								endOfLastPrintfPos=curr_buff_pos;

								//step 1 :
								pBufferStart++;
								bytesConsumed++;
								total_length--;
							}

							// if 'del' char is first , just remove it , also remove 1 additional char
							// from buffer as second step from step 1 of buffer that 'del' char is not first
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
					SHELL_REPLY_DATA( &pBufferStart[endOfLastPrintfPos] , curr_buff_pos - endOfLastPrintfPos);
				}


				if(1 == validCommadFound)
				{
					pCmdStart=pBufferStart;
					if ((HEADER_CHAR_ON == pBufferStart[HEADER_SUPPRESS_ECHO_POS]) ||
							(HEADER_CHAR_OFF == pBufferStart[HEADER_SUPPRESS_ECHO_POS]) )
					{
						pCmdStart=&pBufferStart[START_OF_CMD_POS];
						if ((HEADER_CHAR_ON != pBufferStart[START_OF_CMD_POS - 1]) &&
								(HEADER_CHAR_OFF != pBufferStart[START_OF_CMD_POS - 1]) )
						{
							validCommadFound = 0;
							SHELL_REPLY_DATA( "invalid header\r\n" , sizeof("invalid header\r\n")-1);
						}
					}


					if(1==validCommadFound)
					{
						eol_char = curr_char;
						pBufferStart[curr_buff_pos]='\0';
						if (pBufferStart == pCmdStart )
						{
							SHELL_REPLY_DATA( (const uint8_t*)"\r\n",2);
						}
						run_command((const char *)pCmdStart,0);

					}

					if (pBufferStart != pCmdStart )
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

					curr_buff_pos++;// skip first EOL char
					pBufferStart=&pBufferStart[curr_buff_pos];
					total_length -= curr_buff_pos;
					bytesConsumed+=curr_buff_pos;

					curr_buff_pos=0;
					endOfLastPrintfPos=0;
				}


				curr_shell_instance->lastTestedBytePos = curr_buff_pos;
				curr_shell_instance->lastEOLchar  =eol_char;
				DEV_IOCTL(gCurrReplyDev, IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER , (void *)((uint32_t)bytesConsumed));
			}
			DEV_IOCTL(gCurrReplyDev, IOCTL_SET_UNLOCK_DATA_BUFFER ,(void *) 0);


		}


#if (1==INCLUDE_uxTaskGetStackHighWaterMark )
		{
			static  uint32_t stackLeft,minStackLeft=0xffffffff;

			stackLeft = uxTaskGetStackHighWaterMark( NULL );
			if(minStackLeft > stackLeft)
			{
				minStackLeft = stackLeft;
				PRINTF_DBG("%s stack left = %d\r\n" , __FUNCTION__ ,minStackLeft);
			}
		}
#endif

	}

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        HTTP_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t shell_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{

	switch(aIoctl_num)
	{
		case IOCTL_GET_PARAMS_ARRAY_FUNC :
#if SHELL_CONFIG_USE_AS_DYNAMIC_INSTANCE > 0
			*(const dev_param_t**)aIoctl_param1  = Shell_Dev_Params;
			*(uint8_t*)aIoctl_param2 =  sizeof(Shell_Dev_Params)/sizeof(dev_param_t); //size
#else
			*(uint8_t*)aIoctl_param2 =  0; //size
#endif
			break;

#if SHELL_CONFIG_USE_AS_DYNAMIC_INSTANCE > 0
		case IOCTL_SET_SERVER_DEVICE_BY_NAME :
			{
				pdev_descriptor server_device;
				server_device = DEV_OPEN((uint8_t*)aIoctl_param1);
				if(NULL != server_device)
				{
					DEV_IOCTL(server_device, IOCTL_SET_ISR_CALLBACK_DEV ,  (void*)INSTANCE(aHandle)->this_dev);
				}

				INSTANCE(aHandle)->server_dev=server_device;
			}
			break;
#endif
		case IOCTL_DEVICE_START :
			if(0==task_is_running)
			{
				task_is_running=1;
				os_create_task("shell_task",Shell_Task,
						NULL , SHELL_TASK_STACK_SIZE , SHELL_TASK_PRIORITY);
			}
			break;

		default :
			return 1;
	}
	return 0;
}

#if SHELL_CONFIG_USE_AS_DYNAMIC_INSTANCE > 0

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        Shell_API_Init_Dev_Descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  shell_api_init_dev_descriptor(pdev_descriptor aDevDescriptor)
{
	shell_instance_t *pShell_instance;

	if(MAX_NUM_OF_SHELL_SERVERS <= usedShellInstances) return 1;

	if(NULL == aDevDescriptor) return 1;

	pShell_instance = &shell_instances[usedShellInstances];

	aDevDescriptor->handle = pShell_instance;
	pShell_instance->this_dev = aDevDescriptor;

	aDevDescriptor->ioctl = shell_ioctl;
	aDevDescriptor->callback = shell_callback;

	usedShellInstances++;

	return 0;
}

#endif

