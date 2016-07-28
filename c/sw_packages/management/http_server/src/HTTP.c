/*
 *
 *   file  :  HTTP.cpp
 *
 */



/***************   includes    *******************/
#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

#include "dev_management_api.h" // for device manager defines and typedefs
#define DEBUG
#include "PRINTF_api.h"

#include "http_server_api.h"
#include "HTTP.h"
#include "timer_api.h"
#include "serial_number_stm32f10x_api.h"
#include "ff.h"
#include "ESP8266_api.h"

/***************   defines    *******************/

#define HTTP_TASK_PRIORITY				TASK_NORMAL_PRIORITY

/* The check task uses the sprintf function so requires a little more stack. */
#define HTTP_TASK_STACK_SIZE			DEFINE_STACK_SIZE( 250 )

#define HTTP_MAX_QUEUE_LEN			( 3 )

#define POLLING_PERIOD 10000

#define ERR_STR (uint8_t*)"err: file not found"


/***************   typedefs    *******************/

typedef enum
{
	HTTP_State_Connecting_To_Main_Manager=0,
	HTTP_State_Connecting_To_Aux_Manager ,
	HTTP_State_Connected,
} HTTP_State_t;


typedef struct
{
	pdev_descriptor_t low_level_socket;
	uint32_t socket_state;
} HTTP_socket_info_t;


typedef struct
{
	uint32_t len;
	uint8_t *pData;
	pdev_descriptor_t low_level_socket;
}xMessage_t;


/**********   external variables    **************/



/***********   local variables    **************/
static const dev_param_t HTTP_Dev_Params[]=
{
		{IOCTL_HTTP_SET_MNG_SERVER_IP , IOCTL_VOID , (uint8_t*)HTTP_API_MNG_SERVER_IP_STR, NOT_FOR_SAVE},
		{IOCTL_HTTP_SET_MNG_SERVER_PORT , IOCTL_VOID , (uint8_t*)HTTP_API_MNG_SERVER_PORT_STR, NOT_FOR_SAVE},
		{IOCTL_HTTP_SET_MNG_SERVER_IP_REDANDENCY , IOCTL_VOID , (uint8_t*)HTTP_API_MNG_SERVER_IP_REDANDENCY_STR, NOT_FOR_SAVE},
		{IOCTL_HTTP_SET_MNG_SERVER_PORT_REDANDENCY , IOCTL_VOID , (uint8_t*)HTTP_API_MNG_SERVER_PORT_REDANDENCY_STR, NOT_FOR_SAVE},
		{IOCTL_HTTP_SET_SERVER_DEVICE , IOCTL_VOID , (uint8_t*)HTTP_API_SERVER_DEVICE_STR, NOT_FOR_SAVE},
};

TIMER_API_CREATE_STATIC_DEV(http_timer_inst,"httpt" ,systick_dev_inst );
static pdev_descriptor_const  timer_device = &http_timer_inst;


static uint32_t initDone=0;
static QueueHandle_t xQueue = NULL;

#define IP_STR_LEN  15
#define PORT_STR_LEN 5

static uint8_t ManagmentServerIP[IP_STR_LEN+1];
static uint8_t ManagmentPort[PORT_STR_LEN+1];
static uint8_t ManagmentServerIPRedandency[IP_STR_LEN+1];
static uint8_t ManagmentPortRedandency[PORT_STR_LEN+1];

static pdev_descriptor_t  server_device;

static pdev_descriptor_const  timer_device = &http_timer_inst;
//static pdev_descriptor_const semihosting_dev = &sh_dev_inst;

#define HTTP_BUFFER_LEN  256
static uint8_t http_buffer[HTTP_BUFFER_LEN];


static pdev_descriptor_t HTTP_menagmentSocketDesc;
static uint16_t unused_bytes_left;

static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
static uint32_t comm_reply_state;
static HTTP_State_t currentState ;

static pdev_descriptor_t this_dev;
static pdev_descriptor_t client_dev;

#define MAX_PERMIITED_SILENCE_TIMEOUT 3
#define SILENCE_AFTER_LAST_RECEIVE_TIMEOUT 2

#define POLL_STR_SERIAL_START  5
#define POLL_STR_SERIAL_LEN  8
static uint8_t poll_str[]="poll 12345678 \n";
static uint8_t EOF_MARKER_STR[]="\r\n~2@5\r\n";

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        HTTP_API_ParseRequest                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        http_callback                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t http_callback(pdev_descriptor_t apdev ,const uint8_t aCallback_num
		, void * aCallback_param1, void * aCallback_param2)
{
	uint8_t *requestStr = ((callback_new_data_from_socket_t*)aCallback_param2)->pData;
	uint32_t str_len =  ((callback_new_data_from_socket_t*)aCallback_param2)->DataLen;
	uint32_t cmdStartPos;
	uint8_t *pSendData;
	xMessage_t xMessage;
//	pdev_descriptor_t socketHandle=(pdev_descriptor_t)aCallback_param1;

	if(NULL == xQueue)
	{
		return 1;
	}


	if(0==memcmp("GET",requestStr,3))
	{
		cmdStartPos=3;
//		PRINT_DATA_DBG( (uint8_t*)"---http2 data\r\n",15);

		while(' ' == requestStr[cmdStartPos])
		{
			cmdStartPos++;
		}


		str_len -= cmdStartPos;
		pSendData=(uint8_t*)malloc(str_len * sizeof(uint8_t));

		memcpy(pSendData,(uint8_t*)&requestStr[cmdStartPos],str_len);

		xMessage.low_level_socket = (pdev_descriptor_t)aCallback_param1;
		xMessage.len=str_len;
		xMessage.pData=pSendData;


		if(pdTRUE !=xQueueSendFromISR( xQueue, ( void * ) &xMessage,  &xHigherPriorityTaskWoken ))
		{
	        free(pSendData);
	        return 1;
		}

		return 0;
	}
	return 1;

}


pdev_descriptor_t currLowLevelSocket;

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        HTTP_API_SendData                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
size_t HTTP_pwrite(const void *aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset)
{


	DEV_WRITE(currLowLevelSocket , apData , aLength) ;

//	//  first replays will be the command itself
//	if(2==comm_reply_state)
//	{
////		PRINTF_DBG( (uint8_t*)"--- HTTP_socket_write \r\n");
//
//		memcpy( &http_buffer[HTTP_BUFFER_LEN/2] , apData , aLength);
//		comm_reply_len = aLength;
//	}
//	comm_reply_state++;
	return 0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        HTTP_Send_Task                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static void HTTP_Task( void *pvParameters )
{
	xMessage_t xRxedMessage;
	FIL fp;
	uint32_t i,file_size ;
	size_t bytes_was_read;
	uint8_t *pData;
	size_t dataLen;
	uint8_t *fileName;
	uint32_t silenceCount=0;
	ioctl_socket_open_t socketOpenInfo;
	uint64_t pollPeriod;
	uint8_t is_timer_elapsed;
//	const HTTP_socket_info_t *pSocketInfo;

	socketOpenInfo.new_socket_descriptor = &HTTP_menagmentSocketDesc;
	HTTP_menagmentSocketDesc = NULL;

	pollPeriod = POLLING_PERIOD;
	DEV_IOCTL(timer_device,TIMER_API_SET_COUNTDOWN_VALUE_AND_REST, &pollPeriod);
	currentState = HTTP_State_Connecting_To_Main_Manager;

	for( ;; )
	{

		if(pdTRUE == xQueueReceive( xQueue, &( xRxedMessage ), ( TickType_t ) 2000 ) )
		{

			pData = xRxedMessage.pData;
			dataLen = xRxedMessage.len;

			pollPeriod = POLLING_PERIOD;

//					PRINTF_DBG( (uint8_t*)"---http FILE_REQUEST\r\n");
			currLowLevelSocket = xRxedMessage.low_level_socket;

			if(currLowLevelSocket == HTTP_menagmentSocketDesc)
			{
				DEV_IOCTL(timer_device,TIMER_API_SET_COUNTDOWN_VALUE_AND_REST, &pollPeriod);
				silenceCount=0;
			}

			PRINT_DATA_DBG( pData , dataLen-1);
			PRINT_DATA_DBG((uint8_t*)"\r\n" , 2 );

			if(0!=memcmp("ack",pData,3))
			{
				if(0==memcmp("/cmd/",pData,5))
				{
					for(i=5; i<dataLen ; i++ )
					{
						if ('/'==pData[i]) pData[i] = ' ';
					}
					memcpy(http_buffer,&pData[5],dataLen-5);
					unused_bytes_left = dataLen-5 +1;// +1 for   \n
					http_buffer[dataLen - 5 ]='\n';

					comm_reply_state = 0;

					DEV_CALLBACK_1_PARAMS( client_dev,CALLBACK_NEW_DATA_ARRIVED , this_dev  ) ;

					while(0xffffffff != comm_reply_state)
					{
						vTaskDelay( 10 );
					}

//					PRINT_DATA_DBG( &http_buffer[HTTP_BUFFER_LEN/2] , comm_reply_len);
//					PRINT_DATA_DBG((uint8_t*)"\r\n" , 2 );
//					DEV_WRITE(currLowLevelSocket , &http_buffer[HTTP_BUFFER_LEN/2] , comm_reply_len) ;

				}
				else /* usually used for browser access */
				{
					for(i=0; i<(dataLen-1) ; i++ )
					{
						if (' '==pData[i]) break;
					}
					pData[i]=0;

					i=0;
					if('/'==pData[0]) i=1;

					if(0==pData[i])
					{
						fileName = (uint8_t*)"index.htm" ;
					}
					else
					{
						fileName = &pData[i];
					}
	//				PRINTF_DBG( (uint8_t*)"---http fileName=%s\r\n",fileName);

					if(FR_OK!=f_open(&fp , (char*)fileName , FA_READ))
					{
						DEV_WRITE(currLowLevelSocket , ERR_STR , sizeof(ERR_STR)-1) ;
					}
					else
					{
						file_size=fp.fsize;
						while(file_size)
						{
							f_read(&fp,http_buffer, HTTP_BUFFER_LEN , (UINT*)&bytes_was_read);
							DEV_WRITE(currLowLevelSocket , http_buffer , bytes_was_read) ;
							file_size -= bytes_was_read;
						}
					}
					f_close(&fp);

					if  (HTTP_menagmentSocketDesc == currLowLevelSocket )
					{
						DEV_WRITE(currLowLevelSocket , EOF_MARKER_STR, sizeof(EOF_MARKER_STR)-1);
					}

					/* if we receiving file then it's likely that we are in first
					 * stages of browser connection , then postpone polling (open socket take a lot of time)   */
					DEV_IOCTL(timer_device,TIMER_API_SET_COUNTDOWN_VALUE_AND_REST, &pollPeriod);

				}

				if  (HTTP_menagmentSocketDesc != currLowLevelSocket )
				{
					DEV_IOCTL(server_device, IOCTL_ESP8266_SOCKET_CLOSE ,  currLowLevelSocket );
				}


			}// end of (0==memcmp("ack",pData,3))

	        free(pData);
		}

		DEV_IOCTL(timer_device, TIMER_API_CHECK_IF_COUNTDOWN_ELAPSED ,  &is_timer_elapsed );
		if(is_timer_elapsed)
		{
			pollPeriod=POLLING_PERIOD;
			switch(currentState)
			{
				case HTTP_State_Connecting_To_Main_Manager :
					socketOpenInfo.strIP = ManagmentServerIP;
					socketOpenInfo.strPort = ManagmentPort;
					if(0 == DEV_IOCTL(server_device, IOCTL_ESP8266_SOCKET_OPEN , &socketOpenInfo ))
					{
						currentState = HTTP_State_Connected;
						pollPeriod = 1000;
					}
					else
					{
						currentState = HTTP_State_Connecting_To_Aux_Manager;
					}
					silenceCount=0;
					break;
				case HTTP_State_Connecting_To_Aux_Manager :
					socketOpenInfo.strIP = ManagmentServerIPRedandency;
					socketOpenInfo.strPort = ManagmentPortRedandency;
					if(0==DEV_IOCTL(server_device, IOCTL_ESP8266_SOCKET_OPEN , &socketOpenInfo ))
					{
						currentState = HTTP_State_Connected;
						pollPeriod = 1000;
					}
					else
					{
						currentState = HTTP_State_Connecting_To_Main_Manager;
					}
					silenceCount=0;
					break;
				case HTTP_State_Connected :
					silenceCount++;
					if (MAX_PERMIITED_SILENCE_TIMEOUT < silenceCount )
					{
						DEV_IOCTL(server_device, IOCTL_ESP8266_SOCKET_CLOSE , HTTP_menagmentSocketDesc );
						currentState = HTTP_State_Connecting_To_Main_Manager;
						HTTP_menagmentSocketDesc = NULL;
					}
					else
					{
						if(0 == DEV_WRITE(HTTP_menagmentSocketDesc , poll_str , sizeof(poll_str) - 1 ) )
						{
							DEV_IOCTL(server_device, IOCTL_ESP8266_SOCKET_CLOSE , HTTP_menagmentSocketDesc );
							currentState = HTTP_State_Connecting_To_Main_Manager;
							HTTP_menagmentSocketDesc = NULL;
						}
						else
						{
							DEV_WRITE(HTTP_menagmentSocketDesc , EOF_MARKER_STR, sizeof(EOF_MARKER_STR)-1);
						}
					}

					break;
				default :
					break;
			}
			PRINTF_DBG( (uint8_t*)"---http currentState = %d \r\n" , currentState);
			DEV_IOCTL(timer_device,TIMER_API_SET_COUNTDOWN_VALUE_AND_REST, &pollPeriod);
		}

		os_stack_test();

	}


}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        HTTP_socket_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t HTTP_socket_ioctl(const void *aHandle ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2 )
{
	ioctl_get_data_buffer_t *pDataBuffer;

	switch(aIoctl_num)
	{
		case IOCTL_GET_AND_LOCK_DATA_BUFFER :
//			PRINTF_DBG( (uint8_t*)"---http get buffer\r\n");
			pDataBuffer=(ioctl_get_data_buffer_t *)aIoctl_param1;
			pDataBuffer->pBufferStart = http_buffer;
			pDataBuffer->bufferWasOverflowed = 0;
			pDataBuffer->TotalLength = unused_bytes_left;
			break;
		case IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER :
			unused_bytes_left -= ((size_t)aIoctl_param1);
			break;
		case IOCTL_SET_UNLOCK_DATA_BUFFER :
			unused_bytes_left = 0;
			comm_reply_state = 0xffffffff;
			break;

		default :
			return 1;
	}
	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        HTTP_Start                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t  HTTP_Start( )
{
	pdev_descriptor_t dev_descriptor;

	dev_descriptor = DEV_OPEN((uint8_t*)"serial");
	DEV_READ(dev_descriptor,&poll_str[POLL_STR_SERIAL_START],POLL_STR_SERIAL_LEN);

	xQueue = xQueueCreate( HTTP_MAX_QUEUE_LEN , sizeof( xMessage_t ) );

	xTaskCreate( HTTP_Task, "HTTP_Task", HTTP_TASK_STACK_SIZE,(void*) NULL,
			HTTP_TASK_PRIORITY , NULL );

	initDone=1;

	return 0;


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
uint8_t HTTP_ioctl( pdev_descriptor_t apdev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	ioctl_get_data_buffer_t *pDataBuffer;
	uint32_t retVal;
	uint32_t param_len;

	retVal = 0 ;
	switch(aIoctl_num)
	{
		case IOCTL_GET_PARAMS_ARRAY_FUNC :
			*(const dev_param_t**)aIoctl_param1  = HTTP_Dev_Params;
			*(uint8_t*)aIoctl_param2 =  sizeof(HTTP_Dev_Params)/sizeof(dev_param_t); //size
			break;

		case IOCTL_SET_ISR_CALLBACK_DEV :
			client_dev = (pdev_descriptor_t)aIoctl_param1;

			break;

		case IOCTL_HTTP_SET_SERVER_DEVICE :
			server_device = DEV_OPEN((uint8_t*)aIoctl_param1);
			if(NULL != server_device)
			{
				DEV_IOCTL(server_device,IOCTL_SET_ISR_CALLBACK_DEV, this_dev);
			}
			break;

//		case IOCTL_HTTP_SET_TIMER_DEVICE :
//			timer_device = DEV_OPEN((uint8_t*)aIoctl_param1);
//			if(NULL != timer_device)
//			{
//				DEV_IOCTL(timer_device,IOCTL_SET_ISR_CALLBACK_DEV, this_dev);
//			}
//			break;

		case IOCTL_HTTP_SET_MNG_SERVER_IP :
			param_len = strnlen((char*)aIoctl_param1,IP_STR_LEN+1) + 1; // +1 for '\0' termination
			if((IP_STR_LEN+1) < param_len) return 1;
			memcpy(ManagmentServerIP,(uint8_t*)aIoctl_param1,param_len);
			break;

		case IOCTL_HTTP_SET_MNG_SERVER_PORT :
			param_len = strnlen((char*)aIoctl_param1,PORT_STR_LEN+1) + 1; // +1 for '\0' termination
			if((PORT_STR_LEN+1) < param_len) return 1;
			memcpy(ManagmentPort,(uint8_t*)aIoctl_param1,param_len);
			break;

		case IOCTL_HTTP_SET_MNG_SERVER_IP_REDANDENCY :
			param_len = strnlen((char*)aIoctl_param1,IP_STR_LEN+1) + 1; // +1 for '\0' termination
			if((IP_STR_LEN+1) < param_len) return 1;
			memcpy(ManagmentServerIPRedandency,(uint8_t*)aIoctl_param1,param_len);
			break;

		case IOCTL_HTTP_SET_MNG_SERVER_PORT_REDANDENCY :
			param_len = strnlen((char*)aIoctl_param1,PORT_STR_LEN+1) + 1; // +1 for '\0' termination
			if((PORT_STR_LEN+1) < param_len) return 1;
			memcpy(ManagmentPortRedandency,(uint8_t*)aIoctl_param1,param_len);
			break;

		case IOCTL_GET_AND_LOCK_DATA_BUFFER :
//			PRINTF_DBG( (uint8_t*)"---http get buffer\r\n");
			pDataBuffer=(ioctl_get_data_buffer_t *)aIoctl_param1;
			pDataBuffer->pBufferStart = http_buffer;
			pDataBuffer->bufferWasOverflowed = 0;
			pDataBuffer->TotalLength = unused_bytes_left;
			break;
		case IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER :
			unused_bytes_left -= (size_t)aIoctl_param1;
			break;
		case IOCTL_SET_UNLOCK_DATA_BUFFER :
			comm_reply_state = 0xffffffff;
			break;

		case IOCTL_DEVICE_START :
			HTTP_Start();
			break;

		default :
			return 1;
	}
	return retVal;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        HTTP_API_Init_Dev_Descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  http_server_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor)
{

	if(NULL == aDevDescriptor) return 1;
	this_dev=aDevDescriptor;
	aDevDescriptor->handle=NULL;

	aDevDescriptor->ioctl = HTTP_ioctl;
	aDevDescriptor->pwrite =HTTP_pwrite;
	aDevDescriptor->callback = http_callback;

	return 0;
}
