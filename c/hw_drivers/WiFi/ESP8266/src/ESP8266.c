/*
 *
 * file :   ESP8266.c
 *
 */

/********  includes *********************/
#include "_project.h"

#include "dev_management_api.h" // for device manager defines and typedefs
#define DEBUG
#include "PRINTF_api.h"
#include "ESP8266_api.h"
#include "ESP8266.h"
#include "timer_wrapper_api.h"
#include "os_wrapper.h"

/********  defines *********************/

#define AP_CONNECT_TIMEOUT 15000
#define ESP8266_TIMEOUT  10000//3000
#define ESP8266_MAX_QUEUE_LEN			( 3 )
#define MAX_SSID_NAME_LEN  31
#define MAX_SSID_PSWRD_LEN  31
#define MAX_RCVD_BUFFER_SIZE	8000
/********  types  *********************/
/********  externals *********************/

/********  local defs *********************/

/********  types  *********************/
typedef enum { DATA_FROM_UART,
	CLOSE_SOCKET, SEND_DATA,
	OPEN_SOCKET , CONNECT_SOCKET, GET_IP ,
	GET_OPEN_CONNECTION_STATUS , CHECK_IF_RECEIVED_DATA ,
	GET_RECEIVED_DATA
} Message_Type_t;

typedef struct
{
	pdev_descriptor_t  client_device;
	uint8_t socket_number;
	uint8_t socket_in_use;
	uint8_t	*recvedData;
	size_t	curr_data_size;
	size_t	size_of_data_left_to_receive;
} socket_t;

typedef struct
{
	uint8_t dummy;
} Msg_data_from_uart_t;

typedef struct
{
	pdev_descriptor_t socket_pdev;
} Msg_close_socket_t;

typedef struct
{
	uint8_t *IPstr;
	uint8_t strIPLen;
} Msg_getIP_t;

typedef struct
{
	pdev_descriptor_t socket_pdev;
	const uint8_t *data;
	uint16_t data_length;
} Msg_send_data_to_socket_t;

typedef struct
{
	pdev_descriptor_t socket_pdev;
	uint8_t *strHostName;
	uint8_t *port;
} Msg_connect_socket_t;

typedef struct
{
	pdev_descriptor_t *new_socket_pdev;
} Msg_open_socket_t;

typedef struct
{
	pdev_descriptor_t  socket_pdev;
	uint8_t *strIP;
	uint8_t strIPLen;
	uint16_t *pPort;
} Msg_get_open_connection_t;

typedef struct
{
	pdev_descriptor_t  socket_pdev;
	uint8_t *newDataExists;
} Msg_check_if_new_data_received_t;

typedef struct
{
	pdev_descriptor_t  socket_pdev;
	uint8_t *buffer;
	size_t max_size;
	size_t *size_received;
} Msg_get_data_received_t;

typedef struct
{
	Message_Type_t type;
	union
	{
		Msg_data_from_uart_t Msg_data_from_uart;
		Msg_close_socket_t  Msg_close_socket;
		Msg_send_data_to_socket_t Msg_send_data_to_socket;
		Msg_connect_socket_t  Msg_connect_socket;
		Msg_open_socket_t  Msg_open_socket;
		Msg_getIP_t   Msg_getIP;
		Msg_get_open_connection_t   Msg_get_open_connection;
		Msg_check_if_new_data_received_t	Msg_check_if_new_data_received;
		Msg_get_data_received_t	Msg_get_data_received;
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
	ESP8266_State_Connecting_Socket,
	ESP8266_State_Closing_Socket,
	ESP8266_State_Wait_For_Setting_Timeout_Complete,
	ESP8266_State_Wait_For_Socket_Status,
	ESP8266_State_Wait_For_Socket_Status_Complete,
	ESP8266_State_Idle,
} ESP8266_State_t;


/**********   external variables    **************/



/***********   local variables    **************/

static pdev_descriptor_t  timer_dev ;

#define MAX_NUM_OF_SOCKETS	4
static dev_descriptor_t  sockets_descriptors[MAX_NUM_OF_SOCKETS];
socket_t  sockets[MAX_NUM_OF_SOCKETS];

static uint8_t ssid_name[MAX_SSID_NAME_LEN+1];
static uint8_t ssid_pswrd[MAX_SSID_PSWRD_LEN+1];
static uint8_t ssid_name_redandency[MAX_SSID_NAME_LEN+1];
static uint8_t ssid_pswrd_redandency[MAX_SSID_PSWRD_LEN+1];

static pdev_descriptor_t  tx_server_device;
static pdev_descriptor_t  rx_server_device;


//ESP8266_Instance_t ESP8266_InstanceParams= {0} ;
//static uint16_t usedInstances =0 ;

static os_queue_t xQueue=NULL ;

//static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//static uint8_t socketHandles[4]={0,1,2,3};
static os_mutex_t sendDataMutex;
static ESP8266_State_t returnFromDataReceiveState,currentState ;
static uint8_t lRequest_done=1;
static size_t leftDataToReceive;
static xMessage_t pendingMessage;
//static void *currSocket;
static uint8_t isMessagePending=0;
static uint8_t lCurrError=0;
#define  SEND_BUFFER_LEN 128
static uint8_t sendBuffer[SEND_BUFFER_LEN+1];// +1 for '\0'
//size_t lastTotSize,lastStartPos;
static uint8_t currentSocketNumber;

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
	 DEV_WRITE(tx_server_device , data , length) ;
	//fUartSendData( pUartHandle,data , length);
	//os_delay_ms( 1 );
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
static void   send_str_to_chip	(uint8_t *data)
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
uint8_t ESP8266_callback(pdev_descriptor_t apdev ,const uint8_t aCallback_num
		, void * aCallback_param1, void * aCallback_param2)
{
	xMessage_t  queueMsg;

	if (NULL == xQueue)
	{
		return 1;
	}

	queueMsg.type = DATA_FROM_UART;

	os_queue_send_immediate( xQueue, ( void * ) &queueMsg);

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

	os_mutex_take_infinite_wait(sendDataMutex);

	lRequest_done=0;

	os_queue_send_infinite_wait(xQueue , queueMsg);

//	PRINTF_DBG( (uint8_t*)"send_message_and_wait2 ret=%d\r\n",retVal);


	while (0 == lRequest_done)
	{
		os_delay_ms( 1 );
	}
	error=lCurrError;

	os_mutex_give(sendDataMutex);
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
static size_t ESP8266_socket_pwrite(pdev_descriptor_t apdev ,const uint8_t *apData , size_t aLength, size_t aOffset)
{
	xMessage_t  queueMsg;

	if(0==aLength) return 0;

	queueMsg.type = SEND_DATA;
	queueMsg.msg_data.Msg_send_data_to_socket.socket_pdev = apdev ;
	queueMsg.msg_data.Msg_send_data_to_socket.data = apData;
	queueMsg.msg_data.Msg_send_data_to_socket.data_length = aLength;

//	PRINTF_DBG( (uint8_t*)"ESP8266_socket_write1 \r\n");

	return (0==send_message_and_wait(&queueMsg)) ? aLength : 0 ;

}

int gUartCount,gWritePos;


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
static  uint8_t receiving_data(uint8_t *pBufferStart , size_t total_length)
{
	uint8_t *recvedData;
	//size_t newStartPos;
	uint64_t timeout ;
	socket_t  *curr_rcv_data_socket;
	size_t	curr_data_size, bytes_to_copy;


	curr_rcv_data_socket = &sockets[currentSocketNumber];
	curr_data_size = curr_rcv_data_socket->curr_data_size;
	recvedData = curr_rcv_data_socket->recvedData;
	bytes_to_copy = MAX_RCVD_BUFFER_SIZE - curr_data_size;

//	lastTotSize=total_length;
//	lastStartPos=newStartPos;
//	PRINTF_DBG("-tot_size=%d ,newStartPos=%d\r\n" , total_length,newStartPos );
	if(total_length < leftDataToReceive)
	{
//		PRINT_DATA_DBG(data_buffer_info.pBufferStart , total_length );
		PRINTF_DBG("--rcv_data=%d\r\n" , total_length );
		leftDataToReceive -= total_length;
		if(total_length < bytes_to_copy)
		{
			bytes_to_copy = total_length;
		}
		memcpy(&recvedData[curr_data_size],pBufferStart , bytes_to_copy);
		curr_data_size += bytes_to_copy;
		curr_rcv_data_socket->curr_data_size = curr_data_size;
		DEV_IOCTL(rx_server_device, IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER ,  total_length);
		timeout=ESP8266_TIMEOUT;
		DEV_IOCTL(timer_dev,IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_VALUE_AND_RESET, &timeout);
		return 0;
	}
	else
	{

//		PRINT_DATA_DBG(data_buffer_info.pBufferStart , leftDataToReceive );
		PRINTF_DBG("--end rcv_data=%d\r\n" , total_length );
		if(leftDataToReceive < bytes_to_copy)
		{
			bytes_to_copy = leftDataToReceive;
		}
		memcpy(&recvedData[curr_data_size],pBufferStart , bytes_to_copy);
		curr_data_size += bytes_to_copy;
		curr_rcv_data_socket->curr_data_size = curr_data_size;

		DEV_IOCTL(rx_server_device, IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER ,  leftDataToReceive);
//		currentState = ESP8266_State_Wait_For_Receiving_Data_Complete;
		currentState = ESP8266_State_Idle;
		leftDataToReceive = 0;
		return 1;
	}
	return 0;
}



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
	size_t total_length;
	uint8_t *pBufferStart;
	ioctl_get_data_buffer_t data_buffer_info;
	//callback_new_data_from_socket_t  newDataFromSocketInfo;
	uint64_t timeout ;
	static int last_tested_length = 0;


//	*start_of_found_str = NULL;
//	*length_of_found_str =0 ;

//	PRINTF_DBG("s0=%d",currentState);

	DEV_IOCTL(rx_server_device, IOCTL_GET_AND_LOCK_DATA_BUFFER , &data_buffer_info);

	total_length = data_buffer_info.TotalLength;
	pBufferStart= data_buffer_info.pBufferStart;

	//PRINT_DATA_DBG(pBufferStart , total_length);

	if(data_buffer_info.bufferWasOverflowed)
	{
		while(1);//debug trap
	}

//	lastTotSize=total_length;
//	lastStartPos=data_buffer_info->NewStartPos;
//	PRINTF_DBG("tot_size=%d ,newStartPos=%d\r\n" , total_length,newStartPos );


	// test for incoming data
	if(ESP8266_State_Setting_Mode >= currentState)
	{
		last_tested_length = 0;
		DEV_IOCTL(rx_server_device, IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER , (void*)total_length);
		return 0;
	}
	else if(ESP8266_State_Receiving_Data == currentState)
	{
		return receiving_data(pBufferStart , total_length);
	}
	else if(ESP8266_State_Idle == currentState)
	{
		size_t	bytes_consumed;

		bytes_consumed = 0;
		while(total_length)
		{
			if('+' == pBufferStart[0])
			{
				uint8_t *start_of_requested_str;
				uint8_t *start_of_data_str;
				size_t header_size;
				// if no new data arrived then return 0 size left in received buffer
				// on new data arival we receive new message and will get here again if state remain IDLE
				if(last_tested_length == total_length)
				{
					return 0;
				}
				// check if buffer start "+IPD,nn" or (TODO "+IPD,n,")
				start_of_data_str = memchr(pBufferStart,':',total_length);
				header_size = start_of_data_str - pBufferStart + 1;
				bytes_consumed += header_size;
				if (NULL != start_of_data_str)
				{
					*start_of_data_str = 0;// create valid string
					last_tested_length = 0;
					total_length -= header_size;
					if((7 < total_length) && (0 == memcmp("+IPD,",(uint8_t*)pBufferStart,5)))
					{
						uint8_t receivedSocketNumber;

						start_of_requested_str = strchr(pBufferStart,',');
						if(NULL == start_of_requested_str)
						{
							break;//of while(total_length)
						}
						start_of_requested_str++;
						receivedSocketNumber = atoi((char*)(start_of_requested_str));
						if(receivedSocketNumber >= MAX_NUM_OF_SOCKETS)
						{
							break;//of while(total_length)
						}
						currentSocketNumber = receivedSocketNumber;
						start_of_requested_str = strchr(start_of_requested_str,',');
						if(NULL == start_of_requested_str)
						{
							break;//of while(total_length)
						}
						start_of_requested_str++;
						leftDataToReceive = atoi((char*)(start_of_requested_str));
						currentState = ESP8266_State_Receiving_Data;
						DEV_IOCTL(timer_dev,IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_VALUE_AND_RESET, &timeout);
						break;//of while(total_length)
					}
					else
					{
						break;//of while(total_length)
					}
				}
				last_tested_length = total_length;
			}
			else if(isdigit(*pBufferStart))// looking for closing connections
			{
				uint8_t *start_of_requested_str;
				size_t line_length;
				// if no new data arrived then return 0 size left in received buffer
				// on new data arival we receive new message and will get here again if state remain IDLE
				line_length = 1;
				while (line_length < total_length)
				{
					uint8_t currChar;
					currChar = pBufferStart[line_length];
					if (('\r' == currChar)||('\n' == currChar))
					{
						break;
					}
					line_length++;
				}
				if(line_length != total_length)
				{
					uint8_t receivedSocketNumber;
					socket_t  *curr_closing_socket;

					line_length++;//add end of line char
					bytes_consumed += line_length;
					total_length -= line_length;
					pBufferStart[line_length] = 0;// create valid string
					last_tested_length = 0;
					receivedSocketNumber = atoi((char*)(start_of_requested_str));
					if(receivedSocketNumber >= MAX_NUM_OF_SOCKETS)
					{
						break;//of while(total_length)
					}
					start_of_requested_str = strchr(start_of_requested_str,',');
					if(NULL == start_of_requested_str)
					{
						break;//of while(total_length)
					}
					start_of_requested_str++;
					if(0 != memcmp("CLOSED",start_of_requested_str,sizeof("CLOSED")-1))
					{
						break;//of while(total_length)
					}
					curr_closing_socket = &sockets[currentSocketNumber];
					curr_closing_socket->socket_in_use = 0;
					os_safe_free(curr_closing_socket->recvedData);
					curr_closing_socket->recvedData = NULL ;
					curr_closing_socket->curr_data_size = 0;
					break;//of while(total_length)
				}
				if(last_tested_length == total_length)
				{
					return 0;
				}
				last_tested_length = total_length;
			}
			else
			{
				last_tested_length = 0;
				total_length--;
				pBufferStart++ ;
				bytes_consumed++;
			}
		}// while(total_length)
		DEV_IOCTL(rx_server_device, IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER , (void*)bytes_consumed);
		return total_length;
	}
	else if(ESP8266_State_Wait_For_Send_Ready == currentState)
	{
		size_t	bytes_consumed;

		last_tested_length = 0;
		timeout=ESP8266_TIMEOUT;
		bytes_consumed = 0;
		while ( total_length && ('>' != *pBufferStart))
		{
			total_length--;
			pBufferStart++ ;
			bytes_consumed++;
		}

		if (0 != total_length)
		{
			bytes_consumed++;
			total_length--;
			send_to_chip( pendingMessage.msg_data.Msg_send_data_to_socket.data ,
					pendingMessage.msg_data.Msg_send_data_to_socket.data_length);
			currentState = ESP8266_State_Wait_Send_Complete;
		}
		DEV_IOCTL(timer_dev,IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_VALUE_AND_RESET, &timeout);
		DEV_IOCTL(rx_server_device, IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER , (void*)bytes_consumed);
		return total_length;
	}
	else
	{
		size_t	line_length;

		last_tested_length = 0;
		validCommadFound = 0;
		line_length = 0;
		while ((line_length < total_length) && (0 == validCommadFound))
		{
			switch(pBufferStart[line_length])
			{
				case '\r':			/* Enter		*/
				case '\n':
					//PRINT_DATA_DBG((uint8_t*)"\r\n" , 2 );
					validCommadFound=1;
					break;
				default :
					//PRINT_DATA_DBG(&pBufferStart[line_length] , 1 );
					break;
			}
			line_length++;

		}


		if(validCommadFound)
		{
			timeout = ESP8266_TIMEOUT;
			switch(currentState)
			{
				case ESP8266_State_Wait_For_IP:
					if(0 == memcmp("+CIFSR:STAIP,",pBufferStart,sizeof("+CIFSR:STAIP,")-1))
					{
						char *start_of_requested_str;
						char *end_of_requested_str;
						pBufferStart[line_length-1] = 0;
						start_of_requested_str = strchr((char*)pBufferStart , '\"');
						if(NULL == start_of_requested_str) break;
						start_of_requested_str++;
						end_of_requested_str = strchr(start_of_requested_str , '\"');
						if(NULL == end_of_requested_str) break;
						*end_of_requested_str = 0;
						strncpy((char*)pendingMessage.msg_data.Msg_getIP.IPstr, start_of_requested_str ,
								pendingMessage.msg_data.Msg_getIP.strIPLen);
						currentState = ESP8266_State_Wait_For_IP_Complete;
					}
					break;
				case ESP8266_State_Wait_For_IP_Complete:
					if (0==memcmp("OK",pBufferStart,sizeof("OK")-1))
					{
						lRequest_done = 1;
						currentState = ESP8266_State_Idle;
					}
					break;
				case ESP8266_State_Wait_For_Socket_Status:
					if(0 == memcmp("+CIPSTATUS:",pBufferStart,sizeof("+CIPSTATUS")-1))
					{
						uint8_t receivedSocketNumber;
						if(line_length > 18)
						{
							receivedSocketNumber = atoi((char*)&pBufferStart[11]);
							if(currentSocketNumber == receivedSocketNumber)
							{
								char *start_of_requested_str;
								char *end_of_requested_str;
								pBufferStart[line_length-1] = 0;
								start_of_requested_str = strchr((char*)pBufferStart , '\"');
								if(NULL == start_of_requested_str) break;
								start_of_requested_str = strchr(start_of_requested_str + 1 , '\"');
								if(NULL == start_of_requested_str) break;
								start_of_requested_str = strchr(start_of_requested_str + 1 , '\"');
								if(NULL == start_of_requested_str) break;
								end_of_requested_str = strchr(start_of_requested_str + 1 , '\"');
								if(NULL == end_of_requested_str) break;
								*end_of_requested_str = 0;
								strncpy((char*)pendingMessage.msg_data.Msg_get_open_connection.strIP,start_of_requested_str + 1 ,
										pendingMessage.msg_data.Msg_get_open_connection.strIPLen);
								*pendingMessage.msg_data.Msg_get_open_connection.pPort=atoi(end_of_requested_str + 1);
								currentState = ESP8266_State_Wait_For_Socket_Status_Complete;

							}
						}
					}
					break;
				case ESP8266_State_Wait_For_Socket_Status_Complete:
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
				case ESP8266_State_Connecting_Socket :
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
						send_str_to_chip(char_to_uint8_t("AT+CIPMUX=1\r\n"));
						currentState = ESP8266_State_Setting_Connection_Type;
						PRINTF_DBG("wifi connected  \r\n");
					}
					else if (0==memcmp("FAIL",(uint8_t*)pBufferStart,sizeof("FAIL")-1))
					{
						send_str_to_chip(char_to_uint8_t("AT+CWJAP=\"") );
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
						send_str_to_chip(char_to_uint8_t("AT+CIPMUX=1\r\n"));
						currentState = ESP8266_State_Setting_Connection_Type;
						PRINTF_DBG("wifi connected  \r\n");
					}
					else if (0==memcmp("FAIL",(uint8_t*)pBufferStart,sizeof("FAIL")-1))
					{
						send_str_to_chip((uint8_t*)"AT+CWJAP=\"" );
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
						send_str_to_chip(char_to_uint8_t("AT+CIPSERVER=1,80\r\n"));
						currentState = ESP8266_State_Setting_Timeout;
					}
					break;
				case ESP8266_State_Setting_Timeout:
					if (0==memcmp("OK",(uint8_t*)pBufferStart,sizeof("OK")-1))
					{
						send_str_to_chip(char_to_uint8_t("AT+CIPSTO=200\r\n"));
						currentState = ESP8266_State_Wait_For_Setting_Timeout_Complete;
					}
					break;
//				case ESP8266_State_Wait_For_Receiving_Data_Complete:
//					if (0==memcmp("OK",(uint8_t*)pBufferStart,sizeof("OK")-1))
//					{
//						currentState = returnFromDataReceiveState ;
////									PRINTF_DBG("receive_done st=%d\r\n",currentState);
//					}
					break;
				default :
					break;
			}

			DEV_IOCTL(timer_dev,IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_VALUE_AND_RESET, &timeout);
			pBufferStart=&pBufferStart[line_length];

			total_length -= line_length;
			while (( total_length) && (('\r'==*pBufferStart)||('\n'==*pBufferStart)))//remove end of line chars
			{
				pBufferStart++;
				total_length--;
				line_length++;
			}
			DEV_IOCTL(rx_server_device, IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER ,  (void*)line_length);
			return total_length ; //return 0 if no data in buffer left

		}

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
	pdev_descriptor_t socket_pdev;
	socket_t *socket_handle;
	pdev_descriptor_t new_socket_pdev;

	lCurrError=0;
//	PRINTF_DBG("---ESP process_message=%d \r\n" ,pendingMessage.type);


	timeout=ESP8266_TIMEOUT;
	switch(pendingMessage.type)
	{
		case CLOSE_SOCKET :
			socket_pdev = pendingMessage.msg_data.Msg_close_socket.socket_pdev;
			socket_handle = DEV_GET_CONFIG_DATA_POINTER(socket_pdev);
			socket_num = socket_handle->socket_number;
			snprintf((char*)sendBuffer, SEND_BUFFER_LEN ,"AT+CIPCLOSE=%d\r\n", socket_num);
			send_str_to_chip( sendBuffer );
			currentState = ESP8266_State_Closing_Socket;
			socket_handle->socket_in_use = 0;
			os_safe_free(socket_handle->recvedData);
			socket_handle->recvedData = NULL ;
			socket_handle->curr_data_size = 0;
			break;

		case SEND_DATA :
//			PRINTF_DBG("---ESP11\r\n");
			socket_pdev = pendingMessage.msg_data.Msg_send_data_to_socket.socket_pdev;
			socket_handle = DEV_GET_CONFIG_DATA_POINTER(socket_pdev);
			socket_num = socket_handle->socket_number;
			if(0 != socket_handle->socket_in_use)
			{
				snprintf((char*)sendBuffer, SEND_BUFFER_LEN ,"AT+CIPSEND=%d,%d\r\n",socket_num,
					pendingMessage.msg_data.Msg_send_data_to_socket.data_length);
				send_str_to_chip( sendBuffer );
				currentState = ESP8266_State_Wait_For_Send_Ready;
			}
			else
			{
				lCurrError = 1;
				lRequest_done = 1;
			}
			break;

		case OPEN_SOCKET :
			new_socket_pdev = NULL;
			for(socket_num = 0; socket_num < MAX_NUM_OF_SOCKETS; socket_num++)
			{
				socket_pdev = &sockets_descriptors[socket_num];
				socket_handle = DEV_GET_CONFIG_DATA_POINTER(socket_pdev);
				if (0 == socket_handle->socket_in_use)
				{
					socket_handle->socket_in_use = 1;
					socket_handle->recvedData = (uint8_t*)os_safe_malloc(MAX_RCVD_BUFFER_SIZE);
					socket_handle->curr_data_size = 0;

					new_socket_pdev = socket_pdev;
					break;
				}
			}
			*pendingMessage.msg_data.Msg_open_socket.new_socket_pdev = new_socket_pdev;
			if(MAX_NUM_OF_SOCKETS == socket_num)
			{
				lCurrError = 1;
			}
			lRequest_done = 1;
			break;

		case CONNECT_SOCKET :
			socket_pdev = pendingMessage.msg_data.Msg_connect_socket.socket_pdev;
			socket_handle = DEV_GET_CONFIG_DATA_POINTER(socket_pdev);
			snprintf((char*)sendBuffer, SEND_BUFFER_LEN ,"AT+CIPSTART=%d,\"TCP\",\"%s\",%s\r\n",
				socket_handle->socket_number,
				pendingMessage.msg_data.Msg_connect_socket.strHostName,
				pendingMessage.msg_data.Msg_connect_socket.port);
			send_str_to_chip( sendBuffer  );
			currentState = ESP8266_State_Connecting_Socket;
			break;

		case GET_OPEN_CONNECTION_STATUS :
			socket_pdev = pendingMessage.msg_data.Msg_connect_socket.socket_pdev;
			socket_handle = DEV_GET_CONFIG_DATA_POINTER(socket_pdev);
			snprintf((char*)sendBuffer, SEND_BUFFER_LEN ,"AT+CIPSTATUS\r\n");
			currentSocketNumber = socket_handle->socket_number ;
			send_str_to_chip( sendBuffer  );
			currentState = ESP8266_State_Wait_For_Socket_Status;
			break;

		case GET_IP :
			send_str_to_chip(char_to_uint8_t("AT+CIFSR\r\n"));
			currentState = ESP8266_State_Wait_For_IP;
			break;

		case CHECK_IF_RECEIVED_DATA:
			socket_pdev = pendingMessage.msg_data.Msg_check_if_new_data_received.socket_pdev;
			socket_handle = DEV_GET_CONFIG_DATA_POINTER(socket_pdev);
			*pendingMessage.msg_data.Msg_check_if_new_data_received.newDataExists = socket_handle->curr_data_size ? 1 : 0;
			lRequest_done = 1;
			if(0 == socket_handle->socket_in_use)
			{
				lCurrError = 1;
			}
			break;

		case GET_RECEIVED_DATA:
			{
				size_t bytes_to_copy , curr_data_size;

				socket_pdev = pendingMessage.msg_data.Msg_get_data_received.socket_pdev;
				socket_handle = DEV_GET_CONFIG_DATA_POINTER(socket_pdev);
				if(0 != socket_handle->socket_in_use)
				{
					curr_data_size = socket_handle->curr_data_size;
					bytes_to_copy = pendingMessage.msg_data.Msg_get_data_received.max_size ;
					if (bytes_to_copy > curr_data_size)
					{
						bytes_to_copy = curr_data_size;
					}
					*pendingMessage.msg_data.Msg_get_data_received.size_received = bytes_to_copy ;
					memcpy(pendingMessage.msg_data.Msg_get_data_received.buffer , socket_handle->recvedData , bytes_to_copy);
					curr_data_size -= bytes_to_copy;
					socket_handle->curr_data_size = curr_data_size;
					memmove(socket_handle->recvedData , &socket_handle->recvedData[bytes_to_copy] , curr_data_size);
				}
				else
				{
					pendingMessage.msg_data.Msg_get_data_received.size_received = 0;
					lCurrError = 1;
				}
				lRequest_done = 1;
			}
			break;

		default:
			currentState = ESP8266_State_Idle;
			break;
	}

	DEV_IOCTL(timer_dev,IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_VALUE_AND_RESET, &timeout);


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

	//pdev_descriptor_t this_dev = (pdev_descriptor_t)pvParameters;

//	data_used_in_buffer_info.BytesWasTested=0;
	for( ;; )
	{

		isMessageRecieved = os_queue_receive_with_timeout( xQueue, &(xRxedMessage),1000/* portMAX_DELAY*/ ) ;
		data_left_in_buffer=1;
		while (data_left_in_buffer)
		{
			data_left_in_buffer = 0;
			if( isMessageRecieved )
			{
	//			PRINTF_DBG("---ESP msg type= %d , currState=%d\r\n" , xRxedMessage.type,currentState);
				if(DATA_FROM_UART == xRxedMessage.type)
				{

					data_left_in_buffer = parse_incoming_data();

//					PRINTF_DBG("---ESP dataLeft =%d, currState=%d\r\n" , data_left_in_buffer ,currentState);
					DEV_IOCTL(rx_server_device, IOCTL_SET_UNLOCK_DATA_BUFFER , (void*)0);

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
				if(ESP8266_State_Idle != currentState)
				{
					DEV_IOCTL(timer_dev, IOCTL_TIMER_WRAPPER_API_CHECK_IF_COUNTDOWN_ELAPSED ,  &is_timer_elapsed );
					if(is_timer_elapsed)
					{
						PRINTF_DBG( "--esp timeout crSt=%d\r\n",currentState);

						switch(currentState)
						{
							case ESP8266_State_StartReset :
								//DEV_IOCTL(rx_server_device,IOCTL_SET_ISR_CALLBACK_DEV, NULL);//skip garbage on uart

								os_delay_ms( 1 );
								send_str_to_chip( char_to_uint8_t("AT+RST\r\n")  );// setting echo off
								//DEV_IOCTL(rx_server_device,IOCTL_SET_ISR_CALLBACK_DEV, this_dev);

								timeout = 5000;// not using os_delay_ms() just not to stop the task and receive data from esp8266 module during wait
								DEV_IOCTL(timer_dev,IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_VALUE_AND_RESET, &timeout);
								currentState = ESP8266_State_Resetting;
								break;

							case ESP8266_State_Resetting :
								currentState = ESP8266_State_Setting_Mode ;
								send_str_to_chip( char_to_uint8_t("ATE0\r\n" ) );// setting echo off
								os_delay_ms( 1 );
								send_str_to_chip( char_to_uint8_t("AT+CWMODE=1\r\n")  );
								timeout = 2;
								DEV_IOCTL(timer_dev,IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_VALUE_AND_RESET, &timeout);
								break;
							case ESP8266_State_Setting_AP:
								send_str_to_chip(char_to_uint8_t("AT+CWJAP=\"") );
								send_str_to_chip( ssid_name_redandency );
								send_str_to_chip(char_to_uint8_t("\",\"") );
								send_str_to_chip( ssid_pswrd_redandency );
								send_str_to_chip( char_to_uint8_t("\"\r\n") );
								currentState = ESP8266_State_Setting_Redundent_Ap ;
								timeout = AP_CONNECT_TIMEOUT;
								DEV_IOCTL(timer_dev,IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_VALUE_AND_RESET, &timeout);
								break;
							case ESP8266_State_Setting_Mode:
							case ESP8266_State_Setting_Redundent_Ap :
								send_str_to_chip(char_to_uint8_t("AT+CWJAP=\"") );
								send_str_to_chip( ssid_name );
								send_str_to_chip( char_to_uint8_t("\",\"") );
								send_str_to_chip( ssid_pswrd );
								send_str_to_chip( char_to_uint8_t("\"\r\n") );
								currentState = ESP8266_State_Setting_AP ;
								timeout = AP_CONNECT_TIMEOUT;
								DEV_IOCTL(timer_dev,IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_VALUE_AND_RESET, &timeout);
								break;
							case ESP8266_State_Receiving_Data:
								currentState = returnFromDataReceiveState ;
								timeout = ESP8266_TIMEOUT;
								DEV_IOCTL(timer_dev,IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_VALUE_AND_RESET, &timeout);
								break;
							case ESP8266_State_Wait_For_Send_Ready :
							case ESP8266_State_Wait_Send_Complete :
							case ESP8266_State_Wait_For_IP :
							case ESP8266_State_Connecting_Socket :
							case ESP8266_State_Closing_Socket :
							case ESP8266_State_Wait_For_Socket_Status :
							case ESP8266_State_Wait_For_Socket_Status_Complete :
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
/* Function:        ESP8266_socket_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static uint8_t ESP8266_socket_ioctl(pdev_descriptor_t apdev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	socket_t *config_handle;
	xMessage_t  queueMsg;
	uint32_t retVal;
	pdev_descriptor_t  client_device;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(apdev);
	retVal = 0 ;
	switch(aIoctl_num)
	{
		case IOCTL_SET_ISR_CALLBACK_DEV :
			client_device = (pdev_descriptor_t)aIoctl_param1;
			config_handle->client_device = client_device;
			break;

		case IOCTL_ESP8266_SOCKET_CLOSE :
			queueMsg.type = CLOSE_SOCKET;
			queueMsg.msg_data.Msg_close_socket.socket_pdev = apdev;
			retVal = send_message_and_wait(&queueMsg);
			break;


		case IOCTL_ESP8266_SOCKET_CONNECT:
			queueMsg.type = CONNECT_SOCKET;
			queueMsg.msg_data.Msg_connect_socket.socket_pdev = apdev;
			queueMsg.msg_data.Msg_connect_socket.strHostName =  ((ESP8266_ioctl_socket_connect_t*)aIoctl_param1)->strHostName;
			queueMsg.msg_data.Msg_connect_socket.port = ((ESP8266_ioctl_socket_connect_t*)aIoctl_param1)->strPort;
			retVal = send_message_and_wait(&queueMsg);
			break;

		case IOCTL_ESP8266_SOCKET_GET_OPEN_CONNECTION_STATUS:
			{
				void *ptr;
				queueMsg.type = GET_OPEN_CONNECTION_STATUS;
				queueMsg.msg_data.Msg_get_open_connection.socket_pdev = apdev;
				ptr = ((ESP8266_ioctl_socket_get_open_connection_t*)aIoctl_param1)->strIP;
				if (NULL == ptr) return 2;
				queueMsg.msg_data.Msg_get_open_connection.strIP = ptr;
				queueMsg.msg_data.Msg_get_open_connection.strIPLen = ((ESP8266_ioctl_socket_get_open_connection_t*)aIoctl_param1)->strIPLen;
				ptr = ((ESP8266_ioctl_socket_get_open_connection_t*)aIoctl_param1)->pPort;
				if (NULL == ptr) return 2;
				queueMsg.msg_data.Msg_get_open_connection.pPort = ptr;
				retVal = send_message_and_wait(&queueMsg);
			}
			break;

		case IOCTL_ESP8266_SOCKET_IS_DATA_RECEIVED:
			queueMsg.type = CHECK_IF_RECEIVED_DATA;
			queueMsg.msg_data.Msg_check_if_new_data_received.socket_pdev = apdev;
			queueMsg.msg_data.Msg_check_if_new_data_received.newDataExists = aIoctl_param1;
			retVal = send_message_and_wait(&queueMsg);
			break;

		case IOCTL_ESP8266_SOCKET_GET_RECEIVED_DATA:
			queueMsg.type = GET_RECEIVED_DATA;
			queueMsg.msg_data.Msg_get_data_received.socket_pdev = apdev;
			queueMsg.msg_data.Msg_get_data_received.buffer = ((ESP8266_ioctl_data_received_t*)aIoctl_param1)->buffer;
			queueMsg.msg_data.Msg_get_data_received.max_size = ((ESP8266_ioctl_data_received_t*)aIoctl_param1)->max_size;
			queueMsg.msg_data.Msg_get_data_received.size_received = ((ESP8266_ioctl_data_received_t*)aIoctl_param1)->size_received;
			retVal = send_message_and_wait(&queueMsg);
			break;

		default :
			return 1;
	}
	return retVal;
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
uint8_t ESP8266_ioctl(pdev_descriptor_t apdev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	esp8266_instance_t *config_handle;
	xMessage_t  queueMsg;
	uint32_t retVal;
	uint32_t param_len;
	size_t i;

	retVal = 0 ;
	config_handle = DEV_GET_CONFIG_DATA_POINTER(apdev);
	switch(aIoctl_num)
	{

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


		case IOCTL_ESP8266_SOCKET_OPEN:
			queueMsg.type = OPEN_SOCKET;
			queueMsg.msg_data.Msg_open_socket.new_socket_pdev = ((ESP8266_ioctl_socket_open_t*)aIoctl_param1)->new_socket_descriptor;
			retVal = send_message_and_wait(&queueMsg);
			break;

		case IOCTL_ESP8266_GET_IP:

			queueMsg.type = GET_IP;
			queueMsg.msg_data.Msg_getIP.IPstr = ((ESP8266_ioctl_socket_get_ip_t*)aIoctl_param1)->strIP;
			queueMsg.msg_data.Msg_getIP.strIPLen = ((ESP8266_ioctl_socket_get_ip_t*)aIoctl_param1)->strIPLen;


			retVal = send_message_and_wait(&queueMsg);
			break;

		case IOCTL_DEVICE_START :
			for(i=0;i<4;i++)
			{
				sockets[i].socket_in_use = 0;
				sockets[i].socket_number = i;
				sockets[i].client_device = NULL ;
				sockets[i].recvedData = NULL ;
				sockets[i].curr_data_size = 0 ;
				sockets_descriptors[i].p_config_data=(void*)&sockets[i];
				sockets_descriptors[i].ioctl=ESP8266_socket_ioctl;
				sockets_descriptors[i].pwrite=ESP8266_socket_pwrite;
			}

			timer_dev  = config_handle->timer_dev;
			tx_server_device = config_handle->uart_tx_dev ;
			rx_server_device = config_handle->uart_rx_dev ;
			xQueue = os_create_queue( ESP8266_MAX_QUEUE_LEN , sizeof(xMessage_t ) );
			sendDataMutex = os_create_mutex();

			DEV_IOCTL_0_PARAMS(config_handle->uart_tx_dev , IOCTL_DEVICE_START );
			DEV_IOCTL_0_PARAMS(config_handle->uart_rx_dev , IOCTL_DEVICE_START );

			os_create_task("ESP8266_Task" , ESP8266_Task, apdev , ESP8266_TASK_STACK_SIZE , ESP8266_TASK_PRIORITY);

			break;

		default :
			return 1;
	}
	return retVal;
}
