/*
 *
 * file :   ESP8266.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

#include "dev_managment_api.h" // for device manager defines and typedefs
#define DEBUG
#include "PRINTF_api.h"

#include "ESP8266_api.h"
#include "ESP8266.h"
#include "timer_api.h"

/********  defines *********************/

//#define ESP8266_MAX_NUM	 2

#define AP_CONNECT_TIMEOUT 30000


#define ESP8266_TIMEOUT  10000//3000

#define ESP8266_TASK_PRIORITY				TASK_NORMAL_PRIORITY

/* The check task uses the sprintf function so requires a little more stack. */
#define ESP8266_TASK_STACK_SIZE			DEFINE_STACK_SIZE( 200 )


#define ESP8266_MAX_QUEUE_LEN			( 3 )


#define   	ESP8266_RESET_STR   	"AT+RST\r\n"

//#define   	ESP8266_SET_MODE_STR   	"AT+CWMODE=3\r\n" // both AP and client
#define   	ESP8266_SET_MODE_STR   	"AT+CWMODE=1\r\n" // only client

//#define   	ESP8266_CONNECT_TO_AP_STR   	"AT+CWJAP=\"FreeWIFI1(DONT_OVERUSE)\",\"\"\r\n"
#define   	ESP8266_CONNECT_TO_AP_STR   	"AT+CWJAP=\""

#define   	ESP8266_GET_IP_STR   	"AT+CIFSR\r\n"

#define   	ESP8266_SET_MULTIPLE_CONNECTIONS_STR   	"AT+CIPMUX=1\r\n"

#define   	ESP8266_SET_SINGLE_CONNECTIONS_STR   	"AT+CIPMUX=0\r\n"

#define   	ESP8266_SET_HTTP_SERVER_STR   	"AT+CIPSERVER=1,80\r\n"

#define   	ESP8266_SET_SERVER_TIMEOUT_STR   	"AT+CIPSTO=200\r\n"

#define   	ESP8266_START_TCP_CONNECTION_STR   	"AT+CIPSTART=3,\"TCP\",\""

#define   	ESP8266_SEND_CMD   	"AT+CIPSEND="

#define   	ESP8266_ECHO_OFF   	"ATE0\r\n"


#define MAX_SSID_NAME_LEN  31
#define MAX_SSID_PSWRD_LEN  31
/********  types  *********************/
/********  externals *********************/


/********  local defs *********************/


/********  types  *********************/
typedef enum { DATA_FROM_UART,
	CLOSE_SOCKET, SEND_DATA,
	OPEN_SOCKET, GET_IP } Message_Type_t;

typedef struct
{
	uint8_t dummy;
} Msg_data_from_uart_t;

typedef struct
{
	const void *socket;
} Msg_close_socket_t;

typedef struct
{
	uint8_t *IPstr;
} Msg_getIP_t;

typedef struct
{
	const void *socket;
	const uint8_t *data;
	uint16_t data_length;
} Msg_send_data_to_socket_t;

typedef struct
{
	pdev_descriptor *socket;
	uint8_t *ip_strs;
	uint8_t *port;
} Msg_open_socket_t;

typedef struct
{
	Message_Type_t type;
	union
	{
		Msg_data_from_uart_t Msg_data_from_uart;
		Msg_close_socket_t  Msg_close_socket;
		Msg_send_data_to_socket_t Msg_send_data_to_socket;
		Msg_open_socket_t  Msg_open_socket;
		Msg_getIP_t   Msg_getIP;
	} msg_data;
}xMessage_t;



typedef enum
{
	ESP8266_State_StartReset =0,
	ESP8266_State_Resetting,
	ESP8266_State_Setting_Mode,
	ESP8266_State_Setting_AP,
	ESP8266_State_Setting_Redundent_Ap,
	ESP8266_State_Setting_Connection_Type,
	ESP8266_State_Setting_Timeout,
	ESP8266_State_Receiving_Data,
	ESP8266_State_Wait_For_Receiving_Data_Complete,
	ESP8266_State_Wait_For_Send_Ready,
	ESP8266_State_Wait_Send_Complete,
	ESP8266_State_Wait_For_IP,
	ESP8266_State_Wait_For_IP_Complete,
	ESP8266_State_Openning_Socket,
	ESP8266_State_Closing_Socket,
	ESP8266_State_Wait_For_Setting_Timeout_Complete,
	ESP8266_State_Idle,
} ESP8266_State_t;

/**********   external variables    **************/



/***********   local variables    **************/

static const dev_param_t ESP8266_Dev_Params[]=
{
		{IOCTL_ESP8266_SET_SSID_NAME , IOCTL_VOID , (uint8_t*)ESP8266_API_SSID_NAME_STR, NOT_FOR_SAVE},
		{IOCTL_ESP8266_SET_SSID_PSWRD , IOCTL_VOID , (uint8_t*)ESP8266_API_SSID_PSWRD_STR, NOT_FOR_SAVE},
		{IOCTL_ESP8266_SET_SSID_NAME_REDANDENCY , IOCTL_VOID , (uint8_t*)ESP8266_API_SSID_NAME_REDANDENCY_STR, NOT_FOR_SAVE},
		{IOCTL_ESP8266_SET_SSID_PSWRD_REDANDENCY , IOCTL_VOID , (uint8_t*)ESP8266_API_SSID_PSWRD_REDANDENCY_STR, NOT_FOR_SAVE},
		{IOCTL_ESP8266_SET_SERVER_DEVICE , IOCTL_VOID , (uint8_t*)ESP8266_API_SERVER_DEVICE_STR, NOT_FOR_SAVE},
};

TIMER_API_CREATE_STATIC_DEV(esp8266_timer_inst,"esp8266t" ,systick_dev_inst );
static pdev_descriptor_const  timer_device = &esp8266_timer_inst;

static dev_descriptor_t  sockets[4];

static uint8_t ssid_name[MAX_SSID_NAME_LEN+1];
static uint8_t ssid_pswrd[MAX_SSID_PSWRD_LEN+1];
static uint8_t ssid_name_redandency[MAX_SSID_NAME_LEN+1];
static uint8_t ssid_pswrd_redandency[MAX_SSID_PSWRD_LEN+1];

static pdev_descriptor  server_device;
static pdev_descriptor  client_device;


//ESP8266_Instance_t ESP8266_InstanceParams= {0} ;
//static uint16_t usedInstances =0 ;

static QueueHandle_t xQueue = NULL;
static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//static uint8_t socketHandles[4]={0,1,2,3};
static SemaphoreHandle_t sendDataMutex;
static ESP8266_State_t returnFromDataReceiveState,currentState ;
static uint8_t lRequest_done=1;
static size_t leftDataToReceive;
static xMessage_t pendingMessage;
//static void *currSocket;
static uint8_t isMessagePending=0;
static uint8_t lCurrError=0;
#define  SEND_BUFFER_LEN 25
static uint8_t sendBuffer[SEND_BUFFER_LEN+1];// +1 for '\0'
//size_t lastTotSize,lastStartPos;
static uint32_t initDone=0;
static pdev_descriptor this_dev;

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        send_to_chip                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static void   send_to_chip	(const uint8_t *data, const size_t length)
{
	 DEV_WRITE(server_device , data , length) ;
	//fUartSendData( pUartHandle,data , length);
	//vTaskDelay( 1 );
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        send_str_to_chip                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                    */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static void   send_str_to_chip	(const uint8_t *data)
{
	send_to_chip(data,strlen((char*)data));
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESP8266_callback                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t ESP8266_callback(void * const aHandle ,const uint8_t aCallback_num
		, void * aCallback_param1, void * aCallback_param2)
{
	xMessage_t  queueMsg;

	if (NULL == xQueue)
	{
		return 1;
	}

	queueMsg.type = DATA_FROM_UART;

	xQueueSendFromISR( xQueue, ( void * ) &queueMsg,  &xHigherPriorityTaskWoken );

	return 0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESP8266_API_GenerateReversedHttpRequest                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static uint8_t  send_message_and_wait(const xMessage_t  *queueMsg)
{
	uint8_t error;
//	uint32_t retVal;

	if(NULL == xQueue) return 1;
//	PRINTF_DBG( (uint8_t*)"send_message_and_wait1\r\n");

	xSemaphoreTake(sendDataMutex, ( TickType_t ) portMAX_DELAY ) ;

	lRequest_done=0;

	xQueueSend( xQueue, ( void * ) queueMsg, ( TickType_t ) portMAX_DELAY );

//	PRINTF_DBG( (uint8_t*)"send_message_and_wait2 ret=%d\r\n",retVal);


	while (0 == lRequest_done)
	{
		vTaskDelay( 1 );
	}
	error=lCurrError;

	xSemaphoreGive(sendDataMutex);
//	PRINTF_DBG( (uint8_t*)"send_message_and_wait3\r\n");

	return error;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        uart_write                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static size_t ESP8266_socket_pwrite(const void *aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset)
{
	xMessage_t  queueMsg;

	if(0==aLength) return 0;

	queueMsg.type = SEND_DATA;
	queueMsg.msg_data.Msg_send_data_to_socket.socket = aHandle;
	queueMsg.msg_data.Msg_send_data_to_socket.data = apData;
	queueMsg.msg_data.Msg_send_data_to_socket.data_length = aLength;

//	PRINTF_DBG( (uint8_t*)"ESP8266_socket_write1 \r\n");

	return (0==send_message_and_wait(&queueMsg)) ? aLength : 0 ;

}

int gUartCount,gWritePos;



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        parse_incoming_data                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static  uint8_t parse_incoming_data(/*const uint8_t *str_to_seek,uint16_t str_to_seek_len
		,uint8_t **start_of_found_str ,uint16_t *length_of_found_str*/)
{
	uint8_t validCommadFound;
	size_t curr_buff_pos;
	size_t total_length;
	uint8_t *pBufferStart;
	ioctl_get_data_buffer_t data_buffer_info;
	uint8_t *pStartOfHttpRequest;
	callback_new_data_from_socket_t  newDataFromSocketInfo;
	uint64_t timeout ;
	size_t lastTestedBytePos=0;


//	*start_of_found_str = NULL;
//	*length_of_found_str =0 ;

//	PRINTF_DBG("s0=%d",currentState);

	DEV_IOCTL(server_device, IOCTL_GET_AND_LOCK_DATA_BUFFER , &data_buffer_info);

	total_length = data_buffer_info.TotalLength;
	pBufferStart= data_buffer_info.pBufferStart;

	if(data_buffer_info.bufferWasOverflowed)
	{
		curr_buff_pos = 0;
	}
	else
	{
		curr_buff_pos = lastTestedBytePos;
	}

//	lastTotSize=total_length;
//	lastStartPos=data_buffer_info->NewStartPos;
//	PRINTF_DBG("tot_size=%d ,newStartPos=%d\r\n" , total_length,newStartPos );
	while(curr_buff_pos < total_length)
	{
		validCommadFound = 0;


		while ((curr_buff_pos < total_length) && (0 == validCommadFound))
		{
			switch(pBufferStart[curr_buff_pos])
			{
				case '\r':			/* Enter		*/
				case '\n':
					PRINT_DATA_DBG((uint8_t*)"\r\n" , 2 );
					validCommadFound=1;
					break;
				default :
					PRINT_DATA_DBG(&pBufferStart[curr_buff_pos] , 1 );
					break;
			}
			curr_buff_pos++;

		}


		if(1 == validCommadFound)
		{
			if(ESP8266_State_Setting_Mode < currentState)
			{
				timeout = ESP8266_TIMEOUT;

				if( (ESP8266_State_Setting_Timeout < currentState) && (0==memcmp("+IPD",(uint8_t*)pBufferStart,4)))
				{
					leftDataToReceive = atoi((char*)&pBufferStart[7]);
					pStartOfHttpRequest = ((uint8_t*)memchr((char*)pBufferStart,':',curr_buff_pos)) +1;

					if((NULL != pStartOfHttpRequest) && (curr_buff_pos > 8)
							&& (0 < leftDataToReceive) && (4096 > leftDataToReceive)
							&& (pBufferStart[5] >= '0')&&(pBufferStart[5]<='3'))
					{

	//					currSocket=&socketHandles[startOfString[5]-'0'];
	//					PRINTF_DBG( (uint8_t*)"---ESP2 socket=%d\r\n",startOfString[5]-'0');
						newDataFromSocketInfo.pData = pStartOfHttpRequest;
						newDataFromSocketInfo.DataLen = curr_buff_pos -((int)pStartOfHttpRequest - (int)pBufferStart);
						if(NULL!=client_device)
						{
							DEV_CALLBACK_2_PARAMS( client_device,CALLBACK_NEW_DATA_ARRIVED , &sockets[pBufferStart[5]-'0']
							                                                                          , &newDataFromSocketInfo) ;
						}
						returnFromDataReceiveState = currentState;
						currentState = ESP8266_State_Receiving_Data;
						DEV_IOCTL(timer_device,TIMER_API_SET_COUNTDOWN_VALUE_AND_REST, &timeout);
					}
					DEV_IOCTL(server_device, IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER , (void*)((size_t)pStartOfHttpRequest - (size_t)pBufferStart));
					return 1;
				}

				if(ESP8266_State_Idle != currentState)
				{
	//				*start_of_found_str = (uint8_t*)pBufferStart;
	//				*length_of_found_str = curr_buff_pos ;

	//				PRINTF_DBG("s=%d\r\n",currentState);

					switch(currentState)
					{
						case ESP8266_State_Wait_For_IP:
							if((curr_buff_pos > 5)&&(isdigit(*pBufferStart)))
							{
								memcpy(pendingMessage.msg_data.Msg_getIP.IPstr,pBufferStart , curr_buff_pos);
								pendingMessage.msg_data.Msg_getIP.IPstr[curr_buff_pos]=0;
								currentState = ESP8266_State_Wait_For_IP_Complete;
							}
							break;
						case ESP8266_State_Wait_For_IP_Complete:
							if (0==memcmp("OK",(uint8_t*)pBufferStart,sizeof("OK")-1))
							{
								lRequest_done = 1;
								currentState = ESP8266_State_Idle;
							}
							break;
						case ESP8266_State_Wait_Send_Complete:
							if (0==memcmp("SEND OK",(uint8_t*)pBufferStart,sizeof("SEND OK")-1))
							{
								lRequest_done = 1;
								currentState = ESP8266_State_Idle;
							}
							else if (0==memcmp("link is not",(uint8_t*)pBufferStart,sizeof("link is not")-1))
							{
								lRequest_done = 1;
								lCurrError = 1;
								currentState = ESP8266_State_Idle;
							}

							break;
						case ESP8266_State_Openning_Socket :
							if (0==memcmp("OK",(uint8_t*)pBufferStart,sizeof("OK")-1))
							{
								lRequest_done = 1;
								currentState = ESP8266_State_Idle;
							}
							else if ((0==memcmp("ERROR",(uint8_t*)pBufferStart,sizeof("ERROR")-1)) ||
									(0==memcmp("ALREAY CONN",(uint8_t*)pBufferStart,sizeof("ALREAY CONN")-1)))
							{
								lCurrError = 1;
								lRequest_done = 1;
								currentState = ESP8266_State_Idle;
							}
							else if (0==memcmp("Link typ ERROR",(uint8_t*)pBufferStart,sizeof("Link typ ERROR")-1))
							{
								lCurrError = 1;
								lRequest_done = 1;
								currentState = ESP8266_State_StartReset;
							}
							break;
						case ESP8266_State_Closing_Socket :
							if (0==memcmp("OK",(uint8_t*)pBufferStart,sizeof("OK")-1))
							{
								lRequest_done = 1;
								currentState = ESP8266_State_Idle;
							}
							break;
						case ESP8266_State_Wait_For_Setting_Timeout_Complete :
							if (0==memcmp("OK",(uint8_t*)pBufferStart,sizeof("OK")-1))
							{
								currentState = ESP8266_State_Idle;
							}
							break;
						case ESP8266_State_Setting_AP:
							if (0==memcmp("OK",(uint8_t*)pBufferStart,sizeof("OK")-1))
							{
								send_to_chip( (uint8_t*)ESP8266_SET_MULTIPLE_CONNECTIONS_STR , sizeof(ESP8266_SET_MULTIPLE_CONNECTIONS_STR)-1);
								currentState = ESP8266_State_Setting_Connection_Type;
								PRINTF_DBG("wifi connected  \r\n");
							}
							else if (0==memcmp("FAIL",(uint8_t*)pBufferStart,sizeof("FAIL")-1))
							{
								send_str_to_chip( (uint8_t*)ESP8266_CONNECT_TO_AP_STR );
								send_str_to_chip( ssid_name_redandency );
								send_str_to_chip( (uint8_t*)"\",\"" );
								send_str_to_chip( ssid_pswrd_redandency );
								send_str_to_chip( (uint8_t*)"\"\r\n" );
								currentState = ESP8266_State_Setting_Redundent_Ap ;
							}
							timeout = AP_CONNECT_TIMEOUT;
							break;
						case ESP8266_State_Setting_Redundent_Ap:
							if (0==memcmp("OK",(uint8_t*)pBufferStart,sizeof("OK")-1))
							{
								send_to_chip( (uint8_t*)ESP8266_SET_MULTIPLE_CONNECTIONS_STR , sizeof(ESP8266_SET_MULTIPLE_CONNECTIONS_STR)-1);
								currentState = ESP8266_State_Setting_Connection_Type;
								PRINTF_DBG("wifi connected  \r\n");
							}
							else if (0==memcmp("FAIL",(uint8_t*)pBufferStart,sizeof("FAIL")-1))
							{
								send_str_to_chip( (uint8_t*)ESP8266_CONNECT_TO_AP_STR );
								send_str_to_chip( ssid_name );
								send_str_to_chip( (uint8_t*)"\",\"" );
								send_str_to_chip( ssid_pswrd );
								send_str_to_chip( (uint8_t*)"\"\r\n" );
								currentState = ESP8266_State_Setting_AP ;
							}
							timeout = AP_CONNECT_TIMEOUT;
							break;
						case ESP8266_State_Setting_Connection_Type:
							if (0==memcmp("OK",(uint8_t*)pBufferStart,sizeof("OK")-1))
							{
								send_to_chip( (uint8_t*)ESP8266_SET_HTTP_SERVER_STR , sizeof(ESP8266_SET_HTTP_SERVER_STR)-1);
								currentState = ESP8266_State_Setting_Timeout;
							}
							break;
						case ESP8266_State_Setting_Timeout:
							if (0==memcmp("OK",(uint8_t*)pBufferStart,sizeof("OK")-1))
							{
								send_to_chip( (uint8_t*)ESP8266_SET_SERVER_TIMEOUT_STR , sizeof(ESP8266_SET_SERVER_TIMEOUT_STR)-1);
								currentState = ESP8266_State_Wait_For_Setting_Timeout_Complete;
							}
							break;
						case ESP8266_State_Wait_For_Receiving_Data_Complete:
							if (0==memcmp("OK",(uint8_t*)pBufferStart,sizeof("OK")-1))
							{
								currentState = returnFromDataReceiveState ;
//									PRINTF_DBG("receive_done st=%d\r\n",currentState);
							}
							break;
						default :
							break;
					}

					DEV_IOCTL(timer_device,TIMER_API_SET_COUNTDOWN_VALUE_AND_REST, &timeout);
					DEV_IOCTL(server_device, IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER ,  (void*)curr_buff_pos);
					return 1;

				}
			}

			pBufferStart=&pBufferStart[curr_buff_pos];
//				PRINTF_DBG("tot_size2=%d currPpos=%d\r\n" , total_length ,curr_buff_pos);
			total_length -= curr_buff_pos;
			while (( total_length) && (('\r'==*pBufferStart)||('\n'==*pBufferStart)))
			{
	//			PRINTF_DBG("1 \n");
				pBufferStart++;
				total_length--;
				curr_buff_pos++;
				//PRINT_DATA_DBG( &pBufferStart[curr_buff_pos],1);
			}
			DEV_IOCTL(server_device, IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER , (void*) curr_buff_pos);
			curr_buff_pos=0;

		}
		else
		{
			if(ESP8266_State_Wait_For_Send_Ready == currentState)
			{
				if (0==memcmp(">",(uint8_t*)pBufferStart,sizeof(">")-1))
				{
					send_to_chip( pendingMessage.msg_data.Msg_send_data_to_socket.data ,
							pendingMessage.msg_data.Msg_send_data_to_socket.data_length);
					currentState = ESP8266_State_Wait_Send_Complete;
					timeout=ESP8266_TIMEOUT;
					DEV_IOCTL(timer_device,TIMER_API_SET_COUNTDOWN_VALUE_AND_REST, &timeout);
					DEV_IOCTL(server_device, IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER , (void*)1);
					return 1;
				}
			}
		}
	}

	lastTestedBytePos = curr_buff_pos;
	DEV_IOCTL(server_device, IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER , 0);

	return 0;

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        receiving_data                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static  uint8_t receiving_data(void)
{
	size_t total_length;
//	uint8_t *pBufferStart;
	//size_t newStartPos;
	uint64_t timeout ;

	ioctl_get_data_buffer_t data_buffer_info;

	DEV_IOCTL(server_device, IOCTL_GET_AND_LOCK_DATA_BUFFER ,  &data_buffer_info);
	total_length=data_buffer_info.TotalLength;

//	lastTotSize=total_length;
//	lastStartPos=newStartPos;
//	PRINTF_DBG("-tot_size=%d ,newStartPos=%d\r\n" , total_length,newStartPos );
	if(total_length < leftDataToReceive)
	{
//		PRINT_DATA_DBG(data_buffer_info.pBufferStart , total_length );
		leftDataToReceive -= total_length;
		DEV_IOCTL(server_device, IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER ,  &total_length);
		timeout=ESP8266_TIMEOUT;
		DEV_IOCTL(timer_device,TIMER_API_SET_COUNTDOWN_VALUE_AND_REST, &timeout);
		return 0;
	}
	else
	{
//		PRINT_DATA_DBG(data_buffer_info.pBufferStart , leftDataToReceive );
		DEV_IOCTL(server_device, IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER ,  &leftDataToReceive);
		currentState = ESP8266_State_Wait_For_Receiving_Data_Complete;
		leftDataToReceive = 0;
		return 1;
	}
	return 0;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        process_message                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static void process_message( )
{
	size_t socket_num ;
	uint64_t timeout;

	lCurrError=0;
	PRINTF_DBG("---ESP process_message=%d \r\n" ,pendingMessage.type);

	timeout=ESP8266_TIMEOUT;
	switch(pendingMessage.type)
	{
		case CLOSE_SOCKET :
			socket_num=(size_t)(pendingMessage.msg_data.Msg_close_socket.socket)+'0';
			snprintf((char*)sendBuffer, SEND_BUFFER_LEN ,"AT+CIPCLOSE=%c\r\n",socket_num);
			send_str_to_chip( sendBuffer );
			currentState = ESP8266_State_Closing_Socket;
			break;

		case SEND_DATA :
//			PRINTF_DBG("---ESP11\r\n");
			socket_num=(size_t)(pendingMessage.msg_data.Msg_close_socket.socket)+'0';
			snprintf((char*)sendBuffer, SEND_BUFFER_LEN ,"AT+CIPSEND=%c,%d\r\n",socket_num,
					pendingMessage.msg_data.Msg_send_data_to_socket.data_length);
			send_str_to_chip( sendBuffer );

			currentState = ESP8266_State_Wait_For_Send_Ready;

			break;

		case OPEN_SOCKET :
			send_to_chip((uint8_t*)ESP8266_START_TCP_CONNECTION_STR,sizeof(ESP8266_START_TCP_CONNECTION_STR)-1);
			snprintf((char*)sendBuffer, SEND_BUFFER_LEN ,"%s\",%s\r\n",pendingMessage.msg_data.Msg_open_socket.ip_strs,
					pendingMessage.msg_data.Msg_open_socket.port);
			send_str_to_chip( sendBuffer  );

			*pendingMessage.msg_data.Msg_open_socket.socket = &sockets[3];

			currentState = ESP8266_State_Openning_Socket;

			break;
		case GET_IP :
			send_to_chip((uint8_t*)ESP8266_GET_IP_STR,sizeof(ESP8266_GET_IP_STR)-1);

			currentState = ESP8266_State_Wait_For_IP;

			break;
		default:
			currentState = ESP8266_State_Idle;
			break;
	}

	DEV_IOCTL(timer_device,TIMER_API_SET_COUNTDOWN_VALUE_AND_REST, &timeout);


}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESP8266_Send_Task                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static void ESP8266_Task( void *pvParameters )
{
	xMessage_t xRxedMessage;
//	ESP8266_Instance_t *pInstance=(ESP8266_Instance_t*)pvParameters;
	size_t data_left_in_buffer,isMessageRecieved;
	uint64_t timeout;
	uint8_t is_timer_elapsed;

	currentState = ESP8266_State_StartReset;


//	data_used_in_buffer_info.BytesWasTested=0;
	for( ;; )
	{

		isMessageRecieved = xQueueReceive( xQueue, &( xRxedMessage ), ( TickType_t )1000/* portMAX_DELAY*/ ) ;
		data_left_in_buffer=1;
		while (data_left_in_buffer)
		{
			data_left_in_buffer = 0;
#if 1
			if( isMessageRecieved )
			{
	//			PRINTF_DBG("---ESP msg type= %d , currState=%d\r\n" , xRxedMessage.type,currentState);
				if(DATA_FROM_UART == xRxedMessage.type)
				{
					if(ESP8266_State_Receiving_Data == currentState)
					{
						data_left_in_buffer = receiving_data();
					}
					else
					{
						data_left_in_buffer = parse_incoming_data();
					}
//					PRINTF_DBG("---ESP dataLeft =%d, currState=%d\r\n" , data_left_in_buffer ,currentState);
					DEV_IOCTL(server_device, IOCTL_SET_UNLOCK_DATA_BUFFER , (void*)0);

				}
				else
				{
//					PRINTF_DBG("---ESP msg pend= %d \r\n" , xRxedMessage.type);
					memcpy(&pendingMessage,&xRxedMessage,sizeof(xMessage_t));
					isMessagePending=1;
				}
			}
			else
			{
#endif
				if(ESP8266_State_Idle != currentState)
				{
					DEV_IOCTL(timer_device, TIMER_API_CHECK_IF_COUNTDOWN_ELAPSED ,  &is_timer_elapsed );
					if(is_timer_elapsed)
					{
						PRINTF_DBG( "--esp timeout crSt=%d\r\n",currentState);

						switch(currentState)
						{
							case ESP8266_State_StartReset :
								DEV_IOCTL(server_device,IOCTL_SET_ISR_CALLBACK_DEV, NULL);//skip garbage on uart

								vTaskDelay( 1 );
								send_to_chip((uint8_t*) ESP8266_RESET_STR , sizeof(ESP8266_RESET_STR)-1);
								DEV_IOCTL(server_device,IOCTL_SET_ISR_CALLBACK_DEV, this_dev);

								timeout = 10000;
								DEV_IOCTL(timer_device,TIMER_API_SET_COUNTDOWN_VALUE_AND_REST, &timeout);
								currentState = ESP8266_State_Resetting;
								break;

							case ESP8266_State_Resetting :
								currentState = ESP8266_State_Setting_Mode ;
								send_to_chip( (uint8_t*)ESP8266_ECHO_OFF , sizeof(ESP8266_ECHO_OFF)-1);
								vTaskDelay( 1 );
								send_to_chip( (uint8_t*)ESP8266_SET_MODE_STR , sizeof(ESP8266_SET_MODE_STR)-1);
								timeout = 2;
								DEV_IOCTL(timer_device,TIMER_API_SET_COUNTDOWN_VALUE_AND_REST, &timeout);
								break;
							case ESP8266_State_Setting_AP:
								send_str_to_chip( (uint8_t*)ESP8266_CONNECT_TO_AP_STR );
								send_str_to_chip( ssid_name_redandency );
								send_str_to_chip( (uint8_t*)"\",\"" );
								send_str_to_chip( ssid_pswrd_redandency );
								send_str_to_chip( (uint8_t*)"\"\r\n" );
								currentState = ESP8266_State_Setting_Redundent_Ap ;
								timeout = AP_CONNECT_TIMEOUT;
								DEV_IOCTL(timer_device,TIMER_API_SET_COUNTDOWN_VALUE_AND_REST, &timeout);
								break;
							case ESP8266_State_Setting_Mode:
							case ESP8266_State_Setting_Redundent_Ap :
								send_str_to_chip( (uint8_t*)ESP8266_CONNECT_TO_AP_STR );
								send_str_to_chip( ssid_name );
								send_str_to_chip( (uint8_t*)"\",\"" );
								send_str_to_chip( ssid_pswrd );
								send_str_to_chip( (uint8_t*)"\"\r\n" );
								currentState = ESP8266_State_Setting_AP ;
								timeout = AP_CONNECT_TIMEOUT;
								DEV_IOCTL(timer_device,TIMER_API_SET_COUNTDOWN_VALUE_AND_REST, &timeout);
								break;
							case ESP8266_State_Receiving_Data:
								currentState = returnFromDataReceiveState ;
								timeout = ESP8266_TIMEOUT;
								DEV_IOCTL(timer_device,TIMER_API_SET_COUNTDOWN_VALUE_AND_REST, &timeout);
								break;
							case ESP8266_State_Wait_For_Send_Ready :
							case ESP8266_State_Wait_Send_Complete :
							case ESP8266_State_Wait_For_IP :
							case ESP8266_State_Openning_Socket :
							case ESP8266_State_Closing_Socket :
								lCurrError = 1;
								lRequest_done=1;
								currentState = ESP8266_State_Idle;
								break ;
							default:
								break ;
						}
					}
				}
#if 1
			}

			if(isMessagePending && (ESP8266_State_Idle == currentState))
			{
				process_message();
				isMessagePending=0;
			}
#endif
		}// end of while(data_left_in_buffer)

		os_stack_test();

	}


}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESP8266_Start                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static uint32_t  ESP8266_Start( )
{
	size_t i;

	for(i=0;i<4;i++)
	{
		sockets[i].handle=(void*)i;
		sockets[i].ioctl=DEV_API_dummy_ioctl_func;
		sockets[i].pwrite=ESP8266_socket_pwrite;
	}




	xQueue = xQueueCreate( ESP8266_MAX_QUEUE_LEN , sizeof( xMessage_t ) );
	sendDataMutex = xSemaphoreCreateMutex();


	xTaskCreate( ESP8266_Task, "ESP8266_Task", ESP8266_TASK_STACK_SIZE,(void*) NULL,
			ESP8266_TASK_PRIORITY , NULL );

	initDone=1;

	return 0;


}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESP8266_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static uint8_t ESP8266_ioctl(void * const aHandle ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	xMessage_t  queueMsg;
	uint32_t retVal;
	uint32_t param_len;

	retVal = 0 ;
	switch(aIoctl_num)
	{
		case IOCTL_GET_PARAMS_ARRAY_FUNC :
			*(const dev_param_t**)aIoctl_param1  = ESP8266_Dev_Params;
			*(uint8_t*)aIoctl_param2 =  sizeof(ESP8266_Dev_Params)/sizeof(dev_param_t); //size
			break;
		case IOCTL_SET_ISR_CALLBACK_DEV :
			client_device = (pdev_descriptor)aIoctl_param1;
			break;
		case IOCTL_ESP8266_SET_SERVER_DEVICE :
			server_device = DEV_OPEN((uint8_t*)aIoctl_param1);
			if (NULL!=server_device)
			{
				DEV_IOCTL(server_device,IOCTL_SET_ISR_CALLBACK_DEV, this_dev);
			}
			break;

		case IOCTL_ESP8266_SET_SSID_NAME :
			param_len = strnlen((char*)aIoctl_param1,MAX_SSID_NAME_LEN+1) + 1; // +1 for '\0' termination
			if((MAX_SSID_NAME_LEN+1) < param_len) return 1;
			memcpy(ssid_name,(uint8_t*)aIoctl_param1,param_len);
			break;

		case IOCTL_ESP8266_SET_SSID_PSWRD :
			param_len = strnlen((char*)aIoctl_param1,MAX_SSID_PSWRD_LEN+1) + 1; // +1 for '\0' termination
			if((MAX_SSID_PSWRD_LEN+1) < param_len) return 1;
			memcpy(ssid_pswrd,(uint8_t*)aIoctl_param1,param_len);
			break;

		case IOCTL_ESP8266_SET_SSID_NAME_REDANDENCY :
			param_len = strnlen((char*)aIoctl_param1,MAX_SSID_NAME_LEN+1) + 1; // +1 for '\0' termination
			if((MAX_SSID_NAME_LEN+1) < param_len) return 1;
			memcpy(ssid_name_redandency,(uint8_t*)aIoctl_param1,param_len);
			break;

		case IOCTL_ESP8266_SET_SSID_PSWRD_REDANDENCY :
			param_len = strnlen((char*)aIoctl_param1,MAX_SSID_PSWRD_LEN+1) + 1; // +1 for '\0' termination
			if((MAX_SSID_PSWRD_LEN+1) < param_len) return 1;
			memcpy(ssid_pswrd_redandency,(uint8_t*)aIoctl_param1,param_len);
			break;

		case IOCTL_ESP8266_SOCKET_CLOSE :
			queueMsg.type = CLOSE_SOCKET;
			queueMsg.msg_data.Msg_close_socket.socket = ((pdev_descriptor)aIoctl_param1)->handle;
			retVal = send_message_and_wait(&queueMsg);
			break;

		case IOCTL_ESP8266_SOCKET_OPEN:
			queueMsg.type = OPEN_SOCKET;
			queueMsg.msg_data.Msg_open_socket.socket = ((ioctl_socket_open_t*)aIoctl_param1)->new_socket_descriptor;
			queueMsg.msg_data.Msg_open_socket.ip_strs =  ((ioctl_socket_open_t*)aIoctl_param1)->strIP;
			queueMsg.msg_data.Msg_open_socket.port = ((ioctl_socket_open_t*)aIoctl_param1)->strPort;
			retVal = send_message_and_wait(&queueMsg);
			break;

		case IOCTL_ESP8266_GET_IP:

			queueMsg.type = GET_IP;
			queueMsg.msg_data.Msg_getIP.IPstr = (uint8_t*)aIoctl_param1;


			retVal = send_message_and_wait(&queueMsg);
			break;

		case IOCTL_DEVICE_START :
			ESP8266_Start();
			break;

		default :
			return 1;
	}
	return retVal;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ESP8266_API_Init_Dev_Descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  ESP8266_api_init_dev_descriptor(pdev_descriptor aDevDescriptor)
{

	if(NULL == aDevDescriptor) return 1;
	aDevDescriptor->handle=NULL;
	this_dev=aDevDescriptor;

	aDevDescriptor->ioctl = ESP8266_ioctl;
	aDevDescriptor->callback = ESP8266_callback;

	return 0;
}

