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
#include "auto_init_api.h"

/********  defines *********************/

#define AP_CONNECT_TIMEOUT  15000
#define ESP8266_TIMEOUT     10000//3000
#define ESP8266_MAX_QUEUE_LEN   ( 3 )
#define MAX_RCVD_BUFFER_SIZE   8000

#define SEND_TO_ESP8266(dev, data, len)    DEV_WRITE(dev , data , len)

/********  types  *********************/
/********  externals *********************/

/********  local defs *********************/

/********  types  *********************/


/**********   external variables    **************/



/***********   local variables    **************/
#define DBG_ARR_SIZE  300
uint8_t dbg_trigger = 0;
int dbg_cnt = 0;
uint8_t dbg_arr[DBG_ARR_SIZE];

void update_dbg(uint8_t dbg_param)
{
	if((1 == dbg_trigger) && ( dbg_cnt < DBG_ARR_SIZE))
	{
		dbg_arr[dbg_cnt++] = dbg_param;
	}
}


static void set_AP(
		struct dev_desc_t* tx_dev, char* ssid_name, char* ssid_pswrd);

static char unknown_state[] = "unknown state XX";
char* ESP8266_get_state_name(uint16_t state)
{
	switch (state)
	{
	case ESP8266_State_StartReset : return "StartReset";
	case ESP8266_State_Resetting : return "Resetting";
	case ESP8266_State_Setting_Echo_Off : return "Setting_Echo_Off";
	case ESP8266_State_Setting_Mode : return "Setting_Mode";
	case ESP8266_State_Setting_AP : return "Setting_AP";
	case ESP8266_State_Setting_Redundent_Ap : return "Setting_Redundent_Ap";
	case ESP8266_State_Setting_Connection_Type :
		return "Setting_Connection_Type";
	case ESP8266_State_Creating_Server : return "Creating_Server";
	case ESP8266_State_Setting_Timeout : return "Setting_Timeout";
	case ESP8266_State_Receiving_Data : return "Receiving_Data";
	case ESP8266_State_Wait_For_Receiving_Data_Complete :
		return "Wait_For_Receiving_Data_Complete";
	case ESP8266_State_Wait_For_Send_Ready : return "Wait_For_Send_Ready";
	case ESP8266_State_Wait_Send_Complete : return "Wait_Send_Complete";
	case ESP8266_State_Wait_For_IP : return "Wait_For_IP";
	case ESP8266_State_Wait_For_IP_Complete : return "Wait_For_IP_Complete";
	case ESP8266_State_Connecting_Socket : return "Connecting_Socket";
	case ESP8266_State_Closing_Socket : return "Closing_Socket";
	case ESP8266_State_Wait_For_Socket_Status : return "Wait_For_Socket_Status";
	case ESP8266_State_Wait_For_Socket_Status_Complete :
		return "Wait_For_Socket_Status_Complete";
	case ESP8266_State_Idle : return "Idle";
	default :
		unknown_state[14] = '0' + state / 10;
		unknown_state[15] = '0' + state % 10;
		return unknown_state;
	}
}



/*
 * send_str_to_chip()
 *
 * return:
 */
static void send_str_to_chip (struct dev_desc_t *  tx_dev, char *data)
{
	SEND_TO_ESP8266(tx_dev , (uint8_t*)data,strlen(data));
}


/*
 * ESP8266_callback()
 *
 * return:
 */
uint8_t ESP8266_callback(struct dev_desc_t *adev, const uint8_t aCallback_num,
		void * aCallback_param1, void * aCallback_param2)
{
	struct esp8266_runtime_t *esp8266_dev_state_hndl;
	os_queue_t xQueue;
	struct esp8266_message_t  queueMsg;

	esp8266_dev_state_hndl = DEV_GET_RUNTIME_DATA_POINTER(adev);

	xQueue = esp8266_dev_state_hndl->xQueue;
	if (NULL == xQueue) return 1;

	queueMsg.type = DATA_FROM_UART;
	os_queue_send_immediate( xQueue, ( void * ) &queueMsg);
	return 0;
}


/*
 * send_message_and_wait()
 *
 * return:
 */
static uint8_t  send_message_and_wait(
		struct esp8266_runtime_t *esp8266_dev_state_hndl,
		const struct esp8266_message_t  *queueMsg)
{
	uint8_t error;
	os_queue_t xQueue;
	os_mutex_t sendDataMutex;

	xQueue = esp8266_dev_state_hndl->xQueue;
	if(NULL == xQueue) return 1;

	sendDataMutex = esp8266_dev_state_hndl->sendDataMutex;
	if(NULL == sendDataMutex) return 1;

	os_mutex_take_infinite_wait(sendDataMutex);
	esp8266_dev_state_hndl->lRequest_done = 0;
	esp8266_dev_state_hndl->lCurrError = 0;

	os_queue_send_infinite_wait(xQueue , queueMsg);
	while (0 == esp8266_dev_state_hndl->lRequest_done)
	{
		os_delay_ms( 2 );
	}
	error = esp8266_dev_state_hndl->lCurrError;

	os_mutex_give(sendDataMutex);
	return error;
}


/*
 * ESP8266_socket_pwrite()
 *
 * return:
 */
static size_t ESP8266_socket_pwrite(struct dev_desc_t *adev,
		const uint8_t *apData, size_t aLength, size_t aOffset)
{
	struct esp8266_socket_t *socket_cfg_handle;
	struct esp8266_runtime_t *esp8266_dev_state_hndl;
	struct esp8266_message_t  queueMsg;

	socket_cfg_handle = DEV_GET_CONFIG_DATA_POINTER(adev);
	esp8266_dev_state_hndl = DEV_GET_RUNTIME_DATA_POINTER(
			socket_cfg_handle->esp8266_dev);

	if(0 == aLength) return 0;

	queueMsg.type = SEND_DATA;
	queueMsg.msg_data.msg_send_data_to_socket.socket_pdev = adev ;
	queueMsg.msg_data.msg_send_data_to_socket.data = apData;
	queueMsg.msg_data.msg_send_data_to_socket.data_length = aLength;

//	PRINTF_DBG( (uint8_t*)"ESP8266_socket_write1 \r\n");

	return (0 == send_message_and_wait(esp8266_dev_state_hndl, &queueMsg)) ?
			aLength : 0;

}


static uint8_t cmpBuff2Str(const uint8_t *buf, size_t buf_len, const char *str)
{/*maximal length of string should be 16*/
	size_t str_len;
	str_len = strnlen(str,17);
	if (17 == str_len) return 1;
	if (buf_len < str_len) return 1;
	return memcmp(buf, str, str_len );
}


/*
 * receiving_incoming_net_data()
 *
 * return:
 */
static size_t receiving_incoming_net_data(struct esp8266_cfg_t *config_handle,
		struct esp8266_runtime_t *esp8266_dev_state_hndl,
		uint8_t *pBufferStart, size_t total_length)
{
	uint8_t *recvedData;
	uint64_t timeout ;
	struct esp8266_socket_t  *curr_rcv_data_socket;
	size_t  curr_data_size, bytes_to_copy;
	size_t leftDataToReceive;
	struct esp8266_socket_t  *sockets;

	leftDataToReceive = esp8266_dev_state_hndl->leftDataToReceive;
	sockets = esp8266_dev_state_hndl->sockets;
	curr_rcv_data_socket =
			&sockets[esp8266_dev_state_hndl->currentSocketNumber];
	curr_data_size = curr_rcv_data_socket->curr_data_size;
	recvedData = curr_rcv_data_socket->recvedData;
	bytes_to_copy = MAX_RCVD_BUFFER_SIZE - curr_data_size;

	//PRINTF_DBG("--rcv_data=%d\r\n" , total_length );
	if(total_length < leftDataToReceive)
	{
		if (total_length < bytes_to_copy)
		{
			bytes_to_copy = total_length;
		}
		else
		{
			CRITICAL_ERROR("esp8266 input buffer overflow");
		}
		timeout=ESP8266_TIMEOUT;
		DEV_IOCTL(config_handle->timer_dev,
			IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_VALUE_AND_RESET, &timeout);
	}
	else
	{
		PRINTF_DBG("--end rcv_data\r\n" );
		if (leftDataToReceive < bytes_to_copy)
		{
			bytes_to_copy = leftDataToReceive;
		}
		else
		{
			CRITICAL_ERROR("esp8266 input buffer overflow");
		}
		esp8266_dev_state_hndl->currentState = ESP8266_State_Idle;
	}

	memcpy(&recvedData[curr_data_size],pBufferStart , bytes_to_copy);
	curr_data_size += bytes_to_copy;
	curr_rcv_data_socket->curr_data_size = curr_data_size;
	leftDataToReceive -= bytes_to_copy;
	esp8266_dev_state_hndl->leftDataToReceive = leftDataToReceive;
	return (0 < bytes_to_copy) ? bytes_to_copy : 1;
}


static size_t process_new_incoming_net_data(
		struct esp8266_cfg_t *config_handle,
		struct esp8266_runtime_t *esp8266_dev_state_hndl,
		uint8_t *pBufferStart, size_t total_length)
{
	char *start_of_requested_str;
	char *end_of_requested_str;
	char *start_of_data_str;
	size_t header_size;
	uint8_t receivedSocketNumber;
	uint64_t timeout ;

	if (7 > total_length) return 0;

	// check if buffer start "+IPD,nn,nnn:" or (TODO "+IPD,n,nnn")
	if (0 != memcmp("+IPD,",pBufferStart,5)) return 1;

	pBufferStart += 5;
	total_length -= 5;
	start_of_data_str = memchr(pBufferStart,':',total_length);
	if (NULL == start_of_data_str)
	{
		if (10 > total_length)
			return 0;
		else
			return 1;
	}

	header_size = start_of_data_str - (char*)pBufferStart;
	pBufferStart[header_size] = 0;// create valid string

	receivedSocketNumber = (uint8_t)strtoul((char*)pBufferStart,
			&start_of_requested_str,10);
	if ((receivedSocketNumber >= ESP8266_MAX_NUM_OF_SOCKETS)
			|| (start_of_requested_str == (char*)pBufferStart))
	{
		pBufferStart[header_size] = ':';
		return 1;
	}
	esp8266_dev_state_hndl->currentSocketNumber = receivedSocketNumber;

	if (',' != *start_of_requested_str)
	{
		pBufferStart[header_size] = ':';
		return 1;
	}

	start_of_requested_str++;
	esp8266_dev_state_hndl->leftDataToReceive = strtoul(
			(char*)start_of_requested_str, &end_of_requested_str, 10);

	if (start_of_requested_str == end_of_requested_str)
	{
		pBufferStart[header_size] = ':';
		return 1;
	}

//	PRINTF_DBG("\r\nnew in=%d-\r\n" ,
//			esp8266_dev_state_hndl->leftDataToReceive);

	esp8266_dev_state_hndl->currentState = ESP8266_State_Receiving_Data;
	timeout = ESP8266_TIMEOUT;
	DEV_IOCTL(config_handle->timer_dev ,
			IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_VALUE_AND_RESET, &timeout);
	return header_size + 6;/* 5 for "+IPD,"   , 1 for ":"  */
}


static size_t process_receiving_socket_close(
		struct esp8266_runtime_t *esp8266_dev_state_hndl,
		uint8_t *pBufferStart, size_t total_length)
{
	char *start_of_requested_str;
	size_t line_length;
	uint8_t receivedSocketNumber;
	struct esp8266_socket_t  *curr_closing_socket;

	line_length = 1;
	while (line_length < total_length)
	{
		uint8_t currChar;
		currChar = pBufferStart[line_length];
		if (('\r' == currChar) || ('\n' == currChar))
		{
			break;
		}
		line_length++;
	}

	if (line_length == total_length) return 0;


	receivedSocketNumber = (uint8_t)strtoul((char*)pBufferStart,
			&start_of_requested_str,10);
	if ((receivedSocketNumber >= ESP8266_MAX_NUM_OF_SOCKETS) ||
					(start_of_requested_str == (char*)pBufferStart))
	{
		return 1;
	}

	if (',' != *start_of_requested_str)  return 1;

	start_of_requested_str++;
	total_length -= (start_of_requested_str - (char*)pBufferStart);

	if (0 != cmpBuff2Str((uint8_t*)start_of_requested_str,
			total_length, "CLOSED")) return 1;

	curr_closing_socket =
			&esp8266_dev_state_hndl->sockets[receivedSocketNumber];
	curr_closing_socket->socket_in_use = 0;
	os_safe_free(curr_closing_socket->recvedData);
	curr_closing_socket->recvedData = NULL ;
	curr_closing_socket->curr_data_size = 0;
	return line_length + 1 ;
}


static size_t process_data_from_esp8266_on_idle(
		struct esp8266_cfg_t *config_handle,
		struct esp8266_runtime_t *esp8266_dev_state_hndl,
		uint8_t *pBufferStart, size_t total_length)
{
	size_t bytes_consumed = 0;

	/*
	* if no new data arrived then no new input arrive after reasonable
	* wait time . so we can assume that something bad happen to esp8266
	* and data can be discarded
	*/
//	PRINTF_DBG("idle in");
//	PRINT_DATA_DBG(pBufferStart ,total_length>5 ? 8 : total_length);

	if (esp8266_dev_state_hndl->last_tested_length == total_length)
	{
		PRINTF_DBG("idle no new data");
		return 1;
	}

	if ('+' == *pBufferStart)
	{
		bytes_consumed = process_new_incoming_net_data(config_handle,
				esp8266_dev_state_hndl,
				pBufferStart, total_length);
	}
	else if (isdigit(*pBufferStart))
	{
		bytes_consumed = process_receiving_socket_close(
				esp8266_dev_state_hndl,
				pBufferStart, total_length);
	}
	else/* feed all non-relevant chars*/
	{
		while ((total_length) && ('+' != *pBufferStart) &&
				(!isdigit(*pBufferStart)) )
		{
			total_length--;
			pBufferStart++ ;
			bytes_consumed++;
		}
	}
	//PRINT_DATA_DBG(pBufferStart , bytes_consumed);

	return bytes_consumed;
}


static size_t process_data_from_esp8266_on_wait_for_send_ready(
		struct esp8266_cfg_t *config_handle,
		struct esp8266_runtime_t *esp8266_dev_state_hndl,
		uint8_t *pBufferStart, size_t total_length)
{
	struct esp8266_msg_send_data_to_socket_t*  pmsg_send_data_to_socket;
	size_t  bytes_consumed;
	uint64_t  timeout ;

	pmsg_send_data_to_socket =
			&esp8266_dev_state_hndl->pendingMessage.
			msg_data.msg_send_data_to_socket;

	bytes_consumed = 0;
	while ( total_length && ('>' != *pBufferStart))
	{
		total_length--;
		pBufferStart++ ;
		bytes_consumed++;
	}

	if (0 == total_length) return bytes_consumed;

	SEND_TO_ESP8266( config_handle->uart_tx_dev ,
			pmsg_send_data_to_socket->data ,
			pmsg_send_data_to_socket->data_length);

	esp8266_dev_state_hndl->currentState = ESP8266_State_Wait_Send_Complete;
	timeout = ESP8266_TIMEOUT;
	DEV_IOCTL(config_handle->timer_dev,
		IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_VALUE_AND_RESET, &timeout);
	return bytes_consumed + 1;
}


static void parse_wait_for_get_ip_response(
		struct esp8266_runtime_t *esp8266_dev_state_hndl,
		uint8_t *pBufferStart, size_t line_length)
{
	struct esp8266_msg_getIP_t   *pmsg_getIP;
	char *start_of_requested_str;
	char *end_of_requested_str;

	if (0 != cmpBuff2Str(pBufferStart, line_length, "+CIFSR:STAIP,")) return;

	start_of_requested_str = strchr((char*)pBufferStart , '\"');
	if (NULL == start_of_requested_str) return;

	start_of_requested_str++;
	end_of_requested_str = strchr(start_of_requested_str , '\"');
	if (NULL == end_of_requested_str) return;

	*end_of_requested_str = 0;
	pmsg_getIP = &esp8266_dev_state_hndl->pendingMessage.msg_data.msg_getIP;
	strncpy(pmsg_getIP->IPstr, start_of_requested_str, pmsg_getIP->strIPLen);
	esp8266_dev_state_hndl->currentState = ESP8266_State_Wait_For_IP_Complete;
}


static void parse_wait_for_send_complete(
		struct esp8266_runtime_t *esp8266_dev_state_hndl,
		uint8_t *pBufferStart, size_t line_length)
{
	if (0 == cmpBuff2Str(pBufferStart, line_length, "SEND OK"))
	{
		esp8266_dev_state_hndl->lRequest_done = 1;
		esp8266_dev_state_hndl->currentState = ESP8266_State_Idle;
	}
	else if (0 == cmpBuff2Str(pBufferStart, line_length, "link is not"))
	{
		esp8266_dev_state_hndl->lCurrError = 1;
		esp8266_dev_state_hndl->lRequest_done = 1;
		esp8266_dev_state_hndl->currentState = ESP8266_State_Idle;
	}
}


static void parse_wait_for_get_socket_status_response(
		struct esp8266_runtime_t *esp8266_dev_state_hndl,
		uint8_t *pBufferStart, size_t line_length)
{
	uint8_t receivedSocketNumber;
	char *start_of_requested_str;

	if (18 > line_length) return;

	if (0 != cmpBuff2Str(pBufferStart, line_length, "+CIPSTATUS")) return;

	receivedSocketNumber = (uint8_t)strtoul((char*)&pBufferStart[11],
			(char**)&start_of_requested_str,10);
	if (esp8266_dev_state_hndl->currentSocketNumber == receivedSocketNumber)
	{
		struct esp8266_msg_get_open_connection_t   *pmsg_get_open_connection;
		char *end_of_requested_str;
		pmsg_get_open_connection = &esp8266_dev_state_hndl->pendingMessage.
				msg_data.msg_get_open_connection;

		start_of_requested_str = strchr(start_of_requested_str , '\"');
		if(NULL == start_of_requested_str) return;
		start_of_requested_str = strchr(start_of_requested_str + 1 , '\"');
		if(NULL == start_of_requested_str) return;
		start_of_requested_str = strchr(start_of_requested_str + 1 , '\"');
		if(NULL == start_of_requested_str) return;
		end_of_requested_str = strchr(start_of_requested_str + 1 , '\"');
		if(NULL == end_of_requested_str) return;
		*end_of_requested_str = 0;
		strncpy(pmsg_get_open_connection->strIP,start_of_requested_str + 1,
				pmsg_get_open_connection->strIPLen);
		*pmsg_get_open_connection->pPort =
					strtoul(end_of_requested_str + 1,NULL,10);
		esp8266_dev_state_hndl->currentState =
				ESP8266_State_Wait_For_Socket_Status_Complete;
	}
}


static void parse_wait_for_connecting_socket_response(
		struct esp8266_runtime_t *esp8266_dev_state_hndl,
		uint8_t *pBufferStart, size_t line_length)
{
	if (0 == cmpBuff2Str(pBufferStart, line_length, "OK"))
	{
		esp8266_dev_state_hndl->lRequest_done = 1;
		esp8266_dev_state_hndl->currentState = ESP8266_State_Idle;
	}
	else if ( (0 == cmpBuff2Str(pBufferStart, line_length, "ERROR")) ||
				(0 == cmpBuff2Str(pBufferStart, line_length, "ALREADY CONN")) )
	{
		esp8266_dev_state_hndl->lCurrError = 1;
		esp8266_dev_state_hndl->lRequest_done = 1;
		esp8266_dev_state_hndl->currentState = ESP8266_State_Idle;
	}
	else if (0 == cmpBuff2Str(pBufferStart, line_length, "Link typ ERROR"))
	{
		esp8266_dev_state_hndl->lCurrError = 1;
		esp8266_dev_state_hndl->lRequest_done = 1;
		esp8266_dev_state_hndl->currentState = ESP8266_State_StartReset;
	}
}


static void parse_wait_for_close_socket_response(
		struct esp8266_runtime_t *esp8266_dev_state_hndl,
		uint8_t *pBufferStart, size_t line_length)
{
	struct esp8266_socket_t  *curr_rcv_data_socket;
	struct esp8266_socket_t  *sockets;

	if (0 == cmpBuff2Str(pBufferStart, line_length, "OK"))
	{
		sockets = esp8266_dev_state_hndl->sockets;
		curr_rcv_data_socket =
				&sockets[esp8266_dev_state_hndl->currentSocketNumber];
		curr_rcv_data_socket->socket_in_use = 0;
		os_safe_free(curr_rcv_data_socket->recvedData);
		curr_rcv_data_socket->recvedData = NULL ;
		curr_rcv_data_socket->curr_data_size = 0;
		esp8266_dev_state_hndl->lRequest_done = 1;
		esp8266_dev_state_hndl->currentState = ESP8266_State_Idle;
	}
}


static void parse_wait_for_wifi_connect_response(
		struct esp8266_cfg_t *config_handle,
		struct esp8266_runtime_t *esp8266_dev_state_hndl,
		uint8_t *pBufferStart, size_t line_length)
{
	struct dev_desc_t *   uart_tx_dev;
	ESP8266_State_t currentState ;

	char *curr_ssid_name;
	char *curr_ssid_pswrd;

	currentState = esp8266_dev_state_hndl->currentState;
	uart_tx_dev = config_handle->uart_tx_dev;
	if (0 == cmpBuff2Str(pBufferStart, line_length,"OK"))
	{
		send_str_to_chip(uart_tx_dev, "AT+CIPMUX=1\r\n");
		esp8266_dev_state_hndl->currentState =
				ESP8266_State_Setting_Connection_Type;
		PRINTF_DBG("wifi connected  \r\n");
	}
	else if (0 == cmpBuff2Str(pBufferStart, line_length,"FAIL"))
	{
		if (ESP8266_State_Setting_AP == currentState)
		{
			curr_ssid_name = esp8266_dev_state_hndl->ssid_name_redandency;
			curr_ssid_pswrd = esp8266_dev_state_hndl->ssid_pswrd_redandency;
			currentState = ESP8266_State_Setting_Redundent_Ap ;
		}
		else if (ESP8266_State_Setting_Redundent_Ap == currentState)
		{
			curr_ssid_name = esp8266_dev_state_hndl->ssid_name;
			curr_ssid_pswrd = esp8266_dev_state_hndl->ssid_pswrd;
			currentState =	ESP8266_State_Setting_AP ;
		}
		else
		{/*just to remove uninitialized variable warning*/
			curr_ssid_name = "";
			curr_ssid_pswrd = "";
		}

		if ((ESP8266_State_Setting_AP == currentState) ||
				(ESP8266_State_Setting_Redundent_Ap == currentState))
		{
			esp8266_dev_state_hndl->currentState = currentState;
			send_str_to_chip(uart_tx_dev, "AT+CWJAP=\"" );
			send_str_to_chip(uart_tx_dev, curr_ssid_name );
			send_str_to_chip(uart_tx_dev, "\",\"" );
			send_str_to_chip(uart_tx_dev, curr_ssid_pswrd );
			send_str_to_chip(uart_tx_dev, "\"\r\n" );
		}
	}
}

static size_t calculate_eol_chars(
		uint8_t *pBufferStart, size_t total_length)
{
	size_t count = 0;

	while (total_length && (('\r' == *pBufferStart) || ('\n' == *pBufferStart)))
	{/*remove end of line chars*/
		pBufferStart++;
		total_length--;
		count++;
	}
	return count;
}

static size_t process_data_from_esp8266_on_wait_for_response(
		struct esp8266_cfg_t *config_handle,
		struct esp8266_runtime_t *esp8266_dev_state_hndl,
		uint8_t *pBufferStart, size_t total_length)
{
	size_t  line_length;
	uint64_t timeout ;
	struct dev_desc_t * uart_tx_dev;

	/*
	* if no new data arrived then no new input arrive after reasonable
	* wait time . so we can assume that something bad happen to esp8266
	* and data can be discarded
	*/
	if (esp8266_dev_state_hndl->last_tested_length == total_length)
	{
		return total_length;
	}

	line_length = 0;
	while (line_length < total_length)
	{
		uint8_t currChar;
		currChar = pBufferStart[line_length];
		if (('\r' == currChar) || ('\n' == currChar))
		{
			break;
		}
		line_length++;
	}

	if (line_length == total_length) return 0;

	if (2 > line_length) /* "OK" is minimal string*/
	{
		return line_length + calculate_eol_chars(
				&pBufferStart[line_length], total_length - line_length);
	}

	pBufferStart[line_length] = 0;
	uart_tx_dev = config_handle->uart_tx_dev;
	timeout = ESP8266_TIMEOUT;
	switch(esp8266_dev_state_hndl->currentState)
	{
	case ESP8266_State_Wait_For_IP:
		parse_wait_for_get_ip_response(esp8266_dev_state_hndl,
				pBufferStart, line_length);
		break;
	case ESP8266_State_Wait_For_IP_Complete:
		if (0 == cmpBuff2Str(pBufferStart, line_length,"OK"))
		{
			esp8266_dev_state_hndl->lRequest_done = 1;
			esp8266_dev_state_hndl->currentState = ESP8266_State_Idle;
		}
		break;
	case ESP8266_State_Wait_For_Socket_Status:
		parse_wait_for_get_socket_status_response(esp8266_dev_state_hndl,
				pBufferStart, line_length);
		break;
	case ESP8266_State_Wait_For_Socket_Status_Complete:
		if (0 == cmpBuff2Str(pBufferStart, line_length,"OK"))
		{
			esp8266_dev_state_hndl->lRequest_done = 1;
			esp8266_dev_state_hndl->currentState = ESP8266_State_Idle;
		}
		break;
	case ESP8266_State_Wait_Send_Complete:
		parse_wait_for_send_complete(esp8266_dev_state_hndl,
				pBufferStart, line_length);
		break;
	case ESP8266_State_Connecting_Socket :
		parse_wait_for_connecting_socket_response(esp8266_dev_state_hndl,
				pBufferStart, line_length);
		break;
	case ESP8266_State_Closing_Socket :
		parse_wait_for_close_socket_response(esp8266_dev_state_hndl,
				pBufferStart, line_length);
		break;
	case ESP8266_State_Setting_AP:
	case ESP8266_State_Setting_Redundent_Ap:
		parse_wait_for_wifi_connect_response(config_handle,
				esp8266_dev_state_hndl, pBufferStart, line_length);
		timeout = AP_CONNECT_TIMEOUT;
		break;
	case ESP8266_State_Setting_Connection_Type:
		if (0 == cmpBuff2Str(pBufferStart, line_length,"OK"))
		{
			send_str_to_chip(uart_tx_dev, "AT+CIPSERVER=1,80\r\n");
			esp8266_dev_state_hndl->currentState =
					ESP8266_State_Creating_Server;
		}
		break;
	case ESP8266_State_Setting_Echo_Off :
		if (0 == cmpBuff2Str(pBufferStart, line_length,"OK"))
		{
			send_str_to_chip(uart_tx_dev, "AT+CWMODE=1\r\n");

			esp8266_dev_state_hndl->currentState = ESP8266_State_Setting_Mode ;
			timeout = 10;
		}
		break;
	case ESP8266_State_Setting_Mode:
		if (0 == cmpBuff2Str(pBufferStart, line_length,"OK"))
		{
			set_AP(uart_tx_dev, esp8266_dev_state_hndl->ssid_name,
								esp8266_dev_state_hndl->ssid_pswrd);
			esp8266_dev_state_hndl->currentState = ESP8266_State_Setting_AP ;
			timeout = AP_CONNECT_TIMEOUT;
		}
		break;
	case ESP8266_State_Creating_Server:
		if (0 == cmpBuff2Str(pBufferStart, line_length,"OK"))
		{
			send_str_to_chip(uart_tx_dev, "AT+CIPSTO=200\r\n");
			esp8266_dev_state_hndl->currentState =
					ESP8266_State_Setting_Timeout;
		}
		break;
	case ESP8266_State_Setting_Timeout:
		if (0 == cmpBuff2Str(pBufferStart, line_length,"OK"))
		{
			esp8266_dev_state_hndl->currentState = ESP8266_State_Idle;
		}
		break;
	default :
		break;
	}

	DEV_IOCTL(config_handle->timer_dev,
			IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_VALUE_AND_RESET, &timeout);

	line_length++;

	line_length += calculate_eol_chars(
			&pBufferStart[line_length], total_length - line_length);

	return line_length ;
}


/*
 * process_input_message()
 *
 * return:
 */
static void process_input_message(struct esp8266_cfg_t *config_handle,
		struct esp8266_runtime_t *esp8266_dev_state_hndl)
{
	struct dev_desc_t *   uart_rx_dev;
	struct ioctl_get_data_buffer_t data_buffer_info;
	size_t total_length;
	ESP8266_State_t currentState ;
	size_t bytes_consumed;
	uint8_t *pBufferStart;

	uart_rx_dev = config_handle->uart_rx_dev;
	update_dbg(0x64);

	while(1)
	{
		DEV_IOCTL(uart_rx_dev,
				IOCTL_GET_AND_LOCK_DATA_BUFFER, &data_buffer_info);

		total_length = data_buffer_info.TotalLength;
		pBufferStart = data_buffer_info.pBufferStart;


		if(data_buffer_info.bufferWasOverflowed)
		{
			CRITICAL_ERROR("TODO :  deal with overflow");
		}

		if(0 == total_length) break;

		currentState = esp8266_dev_state_hndl->currentState;
//		PRINTF_DBG("\r\n--%d--\r\n" , esp8266_dev_state_hndl->currentState);
//		PRINT_DATA_DBG(pBufferStart , total_length);

		switch(currentState)
		{
		case ESP8266_State_StartReset :
		case ESP8266_State_Resetting :
			bytes_consumed = total_length;
			break;
		case ESP8266_State_Receiving_Data :
			bytes_consumed = receiving_incoming_net_data(config_handle,
					esp8266_dev_state_hndl,	pBufferStart , total_length);
			break;
		case ESP8266_State_Idle :
			bytes_consumed = process_data_from_esp8266_on_idle(config_handle,
				esp8266_dev_state_hndl,	pBufferStart , total_length);
			break;
		case ESP8266_State_Wait_For_Send_Ready :
			bytes_consumed = process_data_from_esp8266_on_wait_for_send_ready(
					config_handle, esp8266_dev_state_hndl,
					pBufferStart , total_length);
			break;
		default:
			bytes_consumed = process_data_from_esp8266_on_wait_for_response(
					config_handle, esp8266_dev_state_hndl,
					pBufferStart , total_length);
			break;
		}

		esp8266_dev_state_hndl->last_tested_length = total_length;
		if (0 == bytes_consumed)
		{
			os_delay_ms(10);
		}

		DEV_IOCTL(uart_rx_dev,
			IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER , (void*)bytes_consumed);
	}

	esp8266_dev_state_hndl->last_tested_length = 0;
	DEV_IOCTL_0_PARAMS(uart_rx_dev, IOCTL_SET_UNLOCK_DATA_BUFFER);
}


static ESP8266_State_t process_close_socket_message(
		struct esp8266_msg_close_socket_t  *pmsg_close_socket,
		struct esp8266_runtime_t *esp8266_dev_state_hndl,
		struct dev_desc_t *	tx_dev, char *sendBuffer)
{
	struct dev_desc_t * socket_pdev;
	struct esp8266_socket_t *socket_handle;

	socket_pdev = pmsg_close_socket->socket_pdev;
	socket_handle = DEV_GET_CONFIG_DATA_POINTER(socket_pdev);
	if(0 != socket_handle->socket_in_use)
	{
		esp8266_dev_state_hndl->currentSocketNumber =
										socket_handle->socket_number;
		snprintf(sendBuffer, ESP8266_SEND_BUFFER_LEN,
				"AT+CIPCLOSE=%d\r\n", socket_handle->socket_number);
		send_str_to_chip(tx_dev,  sendBuffer );
		return ESP8266_State_Closing_Socket;
	}
	else
	{
		esp8266_dev_state_hndl->lCurrError = ESP8266_ERR_SOCKET_NOT_AVAILABLE;
		esp8266_dev_state_hndl->lRequest_done = 1;
		return ESP8266_State_Idle;
	}
}


static ESP8266_State_t process_send_data_message(
		struct esp8266_msg_send_data_to_socket_t *pmsg_send_data_to_socket,
		struct esp8266_runtime_t *esp8266_dev_state_hndl,
		struct dev_desc_t *	tx_dev, char *sendBuffer)
{
	struct dev_desc_t * socket_pdev;
	struct esp8266_socket_t *socket_handle;

	socket_pdev = pmsg_send_data_to_socket->socket_pdev;
	socket_handle = DEV_GET_CONFIG_DATA_POINTER(socket_pdev);
	if(0 != socket_handle->socket_in_use)
	{
		snprintf(sendBuffer, ESP8266_SEND_BUFFER_LEN,
				"AT+CIPSEND=%d,%d\r\n", socket_handle->socket_number,
				pmsg_send_data_to_socket->data_length);
		send_str_to_chip(tx_dev,  sendBuffer );

		//if(pmsg_send_data_to_socket->data_length == 181) dbg_trigger=1;

		return ESP8266_State_Wait_For_Send_Ready;
	}
	else
	{
		esp8266_dev_state_hndl->lCurrError = ESP8266_ERR_SOCKET_NOT_AVAILABLE;
		esp8266_dev_state_hndl->lRequest_done = 1;
		return ESP8266_State_Idle;
	}
}


static ESP8266_State_t process_open_socket_message(
		struct esp8266_msg_open_socket_t *pmsg_open_socket,
		struct esp8266_runtime_t *esp8266_dev_state_hndl)
{
	struct dev_desc_t * socket_pdev;
	struct esp8266_socket_t *socket_handle;
	struct dev_desc_t * new_socket_pdev;
	size_t socket_num ;

	new_socket_pdev = NULL;
	socket_pdev = esp8266_dev_state_hndl->sockets_descriptors;
	for(socket_num = 0;
			socket_num < ESP8266_MAX_NUM_OF_SOCKETS; socket_num++)
	{
		socket_handle = DEV_GET_CONFIG_DATA_POINTER(socket_pdev);
		if (0 == socket_handle->socket_in_use)
		{
			socket_handle->socket_in_use = 1;
			socket_handle->recvedData =
					(uint8_t*)os_safe_malloc(MAX_RCVD_BUFFER_SIZE);
			socket_handle->curr_data_size = 0;

			new_socket_pdev = socket_pdev;
			break;
		}
		socket_pdev++;
	}
	*pmsg_open_socket->new_socket_pdev = new_socket_pdev;
	if(ESP8266_MAX_NUM_OF_SOCKETS == socket_num)
	{
		esp8266_dev_state_hndl->lCurrError = 1;
	}
	esp8266_dev_state_hndl->lRequest_done = 1;
	return ESP8266_State_Idle;
}


static ESP8266_State_t process_connect_socket_message(
		struct esp8266_msg_connect_socket_t *pmsg_connect_socket,
		struct esp8266_runtime_t *esp8266_dev_state_hndl,
		struct dev_desc_t * tx_dev, char *sendBuffer)
{
	struct dev_desc_t * socket_pdev;
	struct esp8266_socket_t *socket_handle;

	socket_pdev = pmsg_connect_socket->socket_pdev;
	socket_handle = DEV_GET_CONFIG_DATA_POINTER(socket_pdev);
	if(0 != socket_handle->socket_in_use)
	{
		snprintf(sendBuffer, ESP8266_SEND_BUFFER_LEN,
			"AT+CIPSTART=%d,\"TCP\",\"%s\",%s\r\n",
			socket_handle->socket_number,
			pmsg_connect_socket->strHostName,
			pmsg_connect_socket->port);
		send_str_to_chip(tx_dev, sendBuffer);
		return ESP8266_State_Connecting_Socket;
	}
	else
	{
		esp8266_dev_state_hndl->lCurrError = ESP8266_ERR_SOCKET_NOT_AVAILABLE;
		esp8266_dev_state_hndl->lRequest_done = 1;
		return ESP8266_State_Idle;
	}
}


static ESP8266_State_t process_get_connection_status_message(
		struct esp8266_msg_get_open_connection_t *pmsg_get_open_connection,
		struct esp8266_runtime_t *esp8266_dev_state_hndl,
		struct dev_desc_t * tx_dev)
{
	struct dev_desc_t * socket_pdev;
	struct esp8266_socket_t *socket_handle;

	socket_pdev = pmsg_get_open_connection->socket_pdev;
	socket_handle = DEV_GET_CONFIG_DATA_POINTER(socket_pdev);
	if(0 != socket_handle->socket_in_use)
	{
		esp8266_dev_state_hndl->currentSocketNumber =
				socket_handle->socket_number;
		send_str_to_chip(tx_dev, "AT+CIPSTATUS\r\n");
		return ESP8266_State_Wait_For_Socket_Status;
	}
	else
	{
		esp8266_dev_state_hndl->lCurrError = ESP8266_ERR_SOCKET_NOT_AVAILABLE;
		esp8266_dev_state_hndl->lRequest_done = 1;
		return ESP8266_State_Idle;
	}
}


static ESP8266_State_t process_check_if_data_rcvd_message(
	struct esp8266_msg_check_if_new_data_rcvd_t *pmsg_check_if_new_data_rcvd,
	struct esp8266_runtime_t *esp8266_dev_state_hndl)
{
	struct dev_desc_t * socket_pdev;
	struct esp8266_socket_t *socket_handle;

	if( dbg_cnt >= 0x24 )
	{
		esp8266_dev_state_hndl->lCurrError = 0;
	}
	socket_pdev = pmsg_check_if_new_data_rcvd->socket_pdev;
	socket_handle = DEV_GET_CONFIG_DATA_POINTER(socket_pdev);
	*pmsg_check_if_new_data_rcvd->newDataExists =
			socket_handle->curr_data_size ? 1 : 0;
	if(0 == socket_handle->socket_in_use)
	{
		esp8266_dev_state_hndl->lCurrError = ESP8266_ERR_SOCKET_NOT_AVAILABLE;
	}
	esp8266_dev_state_hndl->lRequest_done = 1;
	return ESP8266_State_Idle;
}


static ESP8266_State_t process_check_get_rcvd_data_message(
		struct esp8266_msg_get_data_received_t * pmsg_get_data_received,
		struct esp8266_runtime_t *esp8266_dev_state_hndl)
{
	struct dev_desc_t * socket_pdev;
	struct esp8266_socket_t *socket_handle;
	size_t bytes_to_copy;
	size_t curr_data_size;

	socket_pdev = pmsg_get_data_received->socket_pdev;
	socket_handle = DEV_GET_CONFIG_DATA_POINTER(socket_pdev);

	if(0 != socket_handle->socket_in_use)
	{
		curr_data_size = socket_handle->curr_data_size;
		bytes_to_copy = pmsg_get_data_received->max_size ;
		if (bytes_to_copy > curr_data_size)
		{
			bytes_to_copy = curr_data_size;
		}
		*pmsg_get_data_received->size_received = bytes_to_copy ;
		memcpy(pmsg_get_data_received->buffer,
				socket_handle->recvedData, bytes_to_copy);
		curr_data_size -= bytes_to_copy;
		socket_handle->curr_data_size = curr_data_size;
		memmove(socket_handle->recvedData,
				&socket_handle->recvedData[bytes_to_copy] , curr_data_size);
	}
	else
	{
		*pmsg_get_data_received->size_received = 0;
		esp8266_dev_state_hndl->lCurrError = ESP8266_ERR_SOCKET_NOT_AVAILABLE;
	}
	esp8266_dev_state_hndl->lRequest_done = 1;
	return ESP8266_State_Idle;
}


/*
 * process_output_message()
 *
 * return:
 */
static void process_output_message(struct esp8266_cfg_t *config_handle,
		struct esp8266_runtime_t *esp8266_dev_state_hndl)
{
	struct esp8266_message_t  pendingMessage;
	struct dev_desc_t *   timer_dev;
	uint64_t timeout;
	char *sendBuffer;
	ESP8266_State_t currentState ;
	struct dev_desc_t*  tx_dev;


//	PRINTF_DBG("---ESP process_message=%d \r\n" ,pendingMessage.type);
	pendingMessage = esp8266_dev_state_hndl->pendingMessage;
	sendBuffer = esp8266_dev_state_hndl->sendBuffer;
	tx_dev = config_handle->uart_tx_dev;

	update_dbg(0x20 + pendingMessage.type);

	timeout=ESP8266_TIMEOUT;
	switch(pendingMessage.type)
	{
	case CLOSE_SOCKET :
		currentState = process_close_socket_message(
				&pendingMessage.msg_data.msg_close_socket,
				esp8266_dev_state_hndl, tx_dev, sendBuffer);
		break;
	case SEND_DATA :
		currentState = process_send_data_message(
				&pendingMessage.msg_data.msg_send_data_to_socket,
				esp8266_dev_state_hndl, tx_dev, sendBuffer);
		break;
	case OPEN_SOCKET :
		currentState = process_open_socket_message(
				&pendingMessage.msg_data.msg_open_socket,
				esp8266_dev_state_hndl);
		break;
	case CONNECT_SOCKET :
		currentState = process_connect_socket_message(
				&pendingMessage.msg_data.msg_connect_socket,
				esp8266_dev_state_hndl, tx_dev, sendBuffer);
		break;
	case GET_OPEN_CONNECTION_STATUS :
		currentState = process_get_connection_status_message(
				&pendingMessage.msg_data.msg_get_open_connection,
				esp8266_dev_state_hndl, tx_dev);
		break;
	case GET_IP :
		send_str_to_chip(tx_dev, "AT+CIFSR\r\n");
		currentState = ESP8266_State_Wait_For_IP;
		break;
	case CHECK_IF_RECEIVED_DATA:
		currentState = process_check_if_data_rcvd_message(
				&pendingMessage.msg_data.msg_check_if_new_data_received,
				esp8266_dev_state_hndl);
		break;
	case GET_RECEIVED_DATA:
		currentState = process_check_get_rcvd_data_message(
				&pendingMessage.msg_data.msg_get_data_received,
				esp8266_dev_state_hndl);
		break;
	default:
		currentState = ESP8266_State_Idle;
		break;
	}
	esp8266_dev_state_hndl->currentState = currentState;
	timer_dev = config_handle->timer_dev;
	DEV_IOCTL(timer_dev,IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_VALUE_AND_RESET,
			&timeout);
}


static void set_AP(struct dev_desc_t* tx_dev, char* ssid_name, char* ssid_pswrd)
{
	send_str_to_chip(tx_dev, "AT+CWJAP=\"");
	send_str_to_chip(tx_dev, ssid_name);
	send_str_to_chip(tx_dev, "\",\"");
	send_str_to_chip(tx_dev, ssid_pswrd);
	send_str_to_chip( tx_dev, "\"\r\n");
}


/*
 * no_new_message_received()
 *
 * return:
 */
static void no_new_message_received(struct esp8266_cfg_t *config_handle,
		struct esp8266_runtime_t *esp8266_dev_state_hndl)
{
	ESP8266_State_t currentState ;
	struct dev_desc_t *   timer_dev;
	uint64_t timeout;
	uint8_t is_timer_elapsed;
	struct dev_desc_t * tx_dev;

	update_dbg(0x63);

	currentState = esp8266_dev_state_hndl->currentState;
	if (ESP8266_State_Idle == currentState) return ;

	timer_dev = config_handle->timer_dev;
	DEV_IOCTL(timer_dev, IOCTL_TIMER_WRAPPER_API_CHECK_IF_COUNTDOWN_ELAPSED,
			&is_timer_elapsed );
	if (0 == is_timer_elapsed) return;

	tx_dev = config_handle->uart_tx_dev;

	PRINTF_DBG(
			"--esp timeout crSt=%s\r\n",ESP8266_get_state_name(currentState));
	timeout = ESP8266_TIMEOUT;
	switch(currentState)
	{
	case ESP8266_State_StartReset :
		//skip garbage on uart
		//DEV_IOCTL(rx_server_device,IOCTL_SET_ISR_CALLBACK_DEV, NULL);
		os_delay_ms( 1 );
		send_str_to_chip(tx_dev, "AT+RST\r\n");// setting echo off
		//DEV_IOCTL(rx_server_device,IOCTL_SET_ISR_CALLBACK_DEV, this_dev);

		/*
		 not using os_delay_ms() just not to stop the task and
		 *  receive data from esp8266 module during wait
		 */
		timeout = 5000;
		currentState = ESP8266_State_Resetting;
		break;
	case ESP8266_State_Resetting :
		send_str_to_chip(tx_dev, "ATE0\r\n");// setting echo off
		currentState = ESP8266_State_Setting_Echo_Off ;
		timeout = 10;
		break;
	case ESP8266_State_Setting_AP:
		set_AP(tx_dev, esp8266_dev_state_hndl->ssid_name_redandency,
							esp8266_dev_state_hndl->ssid_pswrd_redandency);
		currentState = ESP8266_State_Setting_Redundent_Ap ;
		timeout = AP_CONNECT_TIMEOUT;
		break;
	case ESP8266_State_Setting_Redundent_Ap :
		set_AP(tx_dev, esp8266_dev_state_hndl->ssid_name,
							esp8266_dev_state_hndl->ssid_pswrd);
		currentState = ESP8266_State_Setting_AP ;
		timeout = AP_CONNECT_TIMEOUT;
		break;
	case ESP8266_State_Receiving_Data:
		currentState = esp8266_dev_state_hndl->returnFromDataReceiveState ;
		break;
	case ESP8266_State_Creating_Server:
	case ESP8266_State_Setting_Connection_Type:
	case ESP8266_State_Setting_Timeout:
		CRITICAL_ERROR("response timeout");
		break;
	case ESP8266_State_Wait_For_Send_Ready :
	case ESP8266_State_Wait_Send_Complete :
	case ESP8266_State_Wait_For_IP :
	case ESP8266_State_Connecting_Socket :
	case ESP8266_State_Closing_Socket :
	case ESP8266_State_Wait_For_Socket_Status :
	case ESP8266_State_Wait_For_Socket_Status_Complete :
	case ESP8266_State_Setting_Mode:
	case ESP8266_State_Setting_Echo_Off :
		esp8266_dev_state_hndl->lCurrError = 1;
		esp8266_dev_state_hndl->lRequest_done=1;
		currentState = ESP8266_State_Idle;
		break ;
	default:
		break ;
	}

	DEV_IOCTL(timer_dev,IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_VALUE_AND_RESET,
			&timeout);
	esp8266_dev_state_hndl->currentState = currentState;
}

/*
 * ESP8266_Task()
 *
 * return:
 */
static void ESP8266_Task( void *pvParameters )
{
	struct dev_desc_t *adev;
	struct esp8266_message_t xRxedMessage;
	size_t isMessageRecieved;
	struct esp8266_cfg_t *config_handle;
	struct esp8266_runtime_t *esp8266_dev_state_hndl;
	ESP8266_State_t currentState ;
	uint8_t isMessagePending;
	os_queue_t xQueue;
	struct esp8266_message_t *pendingMessage;

	adev = pvParameters;
	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);
	esp8266_dev_state_hndl = DEV_GET_RUNTIME_DATA_POINTER(adev);

	xQueue = esp8266_dev_state_hndl->xQueue;
	pendingMessage = &esp8266_dev_state_hndl->pendingMessage;

	for( ;; )
	{

		isMessageRecieved = os_queue_receive_with_timeout( xQueue,
				&(xRxedMessage), 1000/* portMAX_DELAY*/ ) ;

		isMessagePending = esp8266_dev_state_hndl->isMessagePending;

		if( OS_QUEUE_RECEIVE_SUCCESS == isMessageRecieved )
		{
			if(DATA_FROM_UART == xRxedMessage.type)
			{
				process_input_message(config_handle, esp8266_dev_state_hndl);
			}
			else
			{
				memcpy(pendingMessage,
						&xRxedMessage, sizeof(struct esp8266_message_t));
				isMessagePending = 1;
			}
		}
		else
		{
			no_new_message_received(config_handle, esp8266_dev_state_hndl);
		}

		currentState = esp8266_dev_state_hndl->currentState;
		if(isMessagePending && (ESP8266_State_Idle == currentState))
		{
			process_output_message(config_handle, esp8266_dev_state_hndl);
			isMessagePending=0;
		}

		esp8266_dev_state_hndl->isMessagePending = isMessagePending;
		os_stack_test();
	}
}


static uint8_t send_socket_close_msg(struct dev_desc_t *adev,
		struct esp8266_runtime_t *esp8266_dev_state_hndl)
{
	struct esp8266_message_t  queueMsg;

	queueMsg.type = CLOSE_SOCKET;
	queueMsg.msg_data.msg_close_socket.socket_pdev = adev;
	return send_message_and_wait(esp8266_dev_state_hndl, &queueMsg);
}


static uint8_t send_socket_connect_msg(struct dev_desc_t *adev,
		struct esp8266_runtime_t *esp8266_dev_state_hndl,
		struct ESP8266_ioctl_socket_connect_t *ESP8266_ioctl_socket_connect)
{
	struct esp8266_message_t  queueMsg;

	queueMsg.type = CONNECT_SOCKET;
	queueMsg.msg_data.msg_connect_socket.socket_pdev = adev;
	queueMsg.msg_data.msg_connect_socket.strHostName =
			ESP8266_ioctl_socket_connect->strHostName;
	queueMsg.msg_data.msg_connect_socket.port =
			ESP8266_ioctl_socket_connect->strPort;
	return send_message_and_wait(esp8266_dev_state_hndl, &queueMsg);
}


static uint8_t send_get_connection_status_msg(struct dev_desc_t *adev,
		struct esp8266_runtime_t *esp8266_dev_state_hndl,
		struct ESP8266_ioctl_get_conn_status_t *ESP8266_ioctl_get_conn_status)
{
	struct esp8266_message_t  queueMsg;
	void *ptr;

	queueMsg.type = GET_OPEN_CONNECTION_STATUS;
	queueMsg.msg_data.msg_get_open_connection.socket_pdev = adev;
	ptr = ESP8266_ioctl_get_conn_status->strIP;
	if (NULL == ptr) return 2;
	queueMsg.msg_data.msg_get_open_connection.strIP = ptr;
	queueMsg.msg_data.msg_get_open_connection.strIPLen =
			ESP8266_ioctl_get_conn_status->strIPLen;
	ptr = ESP8266_ioctl_get_conn_status->pPort;
	if (NULL == ptr) return 2;
	queueMsg.msg_data.msg_get_open_connection.pPort = ptr;
	return send_message_and_wait(esp8266_dev_state_hndl, &queueMsg);
}


static uint8_t send_is_data_rcvd_msg(struct dev_desc_t *adev,
		struct esp8266_runtime_t *esp8266_dev_state_hndl,
		uint8_t *newDataExists)
{
	struct esp8266_message_t  queueMsg;

	queueMsg.type = CHECK_IF_RECEIVED_DATA;
	queueMsg.msg_data.msg_check_if_new_data_received.socket_pdev = adev;
	queueMsg.msg_data.msg_check_if_new_data_received.newDataExists =
			newDataExists;
	return send_message_and_wait(esp8266_dev_state_hndl, &queueMsg);
}


static uint8_t send_get_data_rcvd_msg(struct dev_desc_t *adev,
		struct esp8266_runtime_t *esp8266_dev_state_hndl,
		struct ESP8266_ioctl_data_received_t *ESP8266_ioctl_data_received)
{
	struct esp8266_message_t  queueMsg;

	queueMsg.type = GET_RECEIVED_DATA;
	queueMsg.msg_data.msg_get_data_received.socket_pdev = adev;
	queueMsg.msg_data.msg_get_data_received.buffer =
			ESP8266_ioctl_data_received->buffer;
	queueMsg.msg_data.msg_get_data_received.max_size =
			ESP8266_ioctl_data_received->max_size;
	queueMsg.msg_data.msg_get_data_received.size_received =
			ESP8266_ioctl_data_received->size_received;
	return send_message_and_wait(esp8266_dev_state_hndl, &queueMsg);
}


/*
 * ESP8266_socket_ioctl()
 *
 * return:
 */
static uint8_t ESP8266_socket_ioctl(struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct esp8266_socket_t *socket_cfg_handle;
	struct esp8266_runtime_t *esp8266_dev_state_hndl;
	uint32_t retVal = 0;

	socket_cfg_handle = DEV_GET_CONFIG_DATA_POINTER(adev);
	esp8266_dev_state_hndl = DEV_GET_RUNTIME_DATA_POINTER(
			socket_cfg_handle->esp8266_dev);

	switch(aIoctl_num)
	{
	case IOCTL_ESP8266_SOCKET_CLOSE :
		retVal = send_socket_close_msg(adev, esp8266_dev_state_hndl);
		break;
	case IOCTL_ESP8266_SOCKET_CONNECT:
		retVal = send_socket_connect_msg(adev,
				esp8266_dev_state_hndl, aIoctl_param1);
		break;
	case IOCTL_ESP8266_SOCKET_GET_OPEN_CONNECTION_STATUS:
		retVal = send_get_connection_status_msg(adev, esp8266_dev_state_hndl,
				aIoctl_param1);
		break;

	case IOCTL_ESP8266_SOCKET_IS_DATA_RECEIVED:
		retVal = send_is_data_rcvd_msg(adev,
				esp8266_dev_state_hndl, aIoctl_param1);
		break;
	case IOCTL_ESP8266_SOCKET_GET_RECEIVED_DATA:
		retVal = send_get_data_rcvd_msg(adev,
				esp8266_dev_state_hndl, aIoctl_param1);
		break;
	case IOCTL_ESP8266_SOCKET_SET_OPTIONS:
		socket_cfg_handle->socket_options = *(uint32_t*)aIoctl_param1;
		break;
	case IOCTL_ESP8266_SOCKET_GET_OPTIONS:
		*(uint32_t*)aIoctl_param1 = socket_cfg_handle->socket_options;
		break;
	default :
		return 1;
	}
	return retVal;
}



static uint8_t ESP8266_device_start(struct dev_desc_t *adev)
{
	struct esp8266_cfg_t *config_handle;
	struct esp8266_runtime_t *esp8266_dev_state_hndl;
	struct esp8266_socket_t  *sockets;
	struct dev_desc_t  *sockets_descriptors;
	size_t i;

	esp8266_dev_state_hndl = DEV_GET_RUNTIME_DATA_POINTER(adev);
	esp8266_dev_state_hndl->xQueue = os_create_queue(
			ESP8266_MAX_QUEUE_LEN, sizeof(struct esp8266_message_t ));
	esp8266_dev_state_hndl->sendDataMutex = os_create_mutex();
	esp8266_dev_state_hndl->currentState = ESP8266_State_StartReset;
	sockets = esp8266_dev_state_hndl->sockets;
	sockets_descriptors = esp8266_dev_state_hndl->sockets_descriptors;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);
	DEV_IOCTL_0_PARAMS(config_handle->uart_tx_dev, IOCTL_DEVICE_START );
	DEV_IOCTL_0_PARAMS(config_handle->uart_rx_dev, IOCTL_DEVICE_START );

	for(i = 0; i < ESP8266_MAX_NUM_OF_SOCKETS; i++)
	{
		sockets[i].socket_in_use = 0;
		sockets[i].socket_number = i;
		sockets[i].client_device = NULL ;
		sockets[i].recvedData = NULL ;
		sockets[i].curr_data_size = 0 ;
		sockets[i].esp8266_dev = adev;
		sockets[i].socket_options = 0;
		sockets_descriptors[i].p_config_data = (void*)&sockets[i];
		sockets_descriptors[i].ioctl = ESP8266_socket_ioctl;
		sockets_descriptors[i].pwrite = ESP8266_socket_pwrite;
	}

	os_create_task("ESP8266_Task", ESP8266_Task, adev,
			ESP8266_TASK_STACK_SIZE, ESP8266_TASK_PRIORITY);
	return 0;
}


static uint8_t send_socket_open_msg(
		struct esp8266_runtime_t *esp8266_dev_state_hndl,
		struct ESP8266_ioctl_socket_open_t *ESP8266_ioctl_socket_open)
{
	struct esp8266_message_t  queueMsg;

	queueMsg.type = OPEN_SOCKET;
	queueMsg.msg_data.msg_open_socket.new_socket_pdev =
			ESP8266_ioctl_socket_open->new_socket_descriptor;
	return send_message_and_wait(esp8266_dev_state_hndl, &queueMsg);
}


static uint8_t send_get_ip_msg(
		struct esp8266_runtime_t *esp8266_dev_state_hndl,
		struct ESP8266_ioctl_socket_get_ip_t *ESP8266_ioctl_socket_get_ip)
{
	struct esp8266_message_t  queueMsg;

	queueMsg.type = GET_IP;
	queueMsg.msg_data.msg_getIP.IPstr =
			ESP8266_ioctl_socket_get_ip->strIP;
	queueMsg.msg_data.msg_getIP.strIPLen =
			ESP8266_ioctl_socket_get_ip->strIPLen;
	return send_message_and_wait(esp8266_dev_state_hndl, &queueMsg);
}


static uint8_t set_string(char *dest, const char *src, uint16_t max_length)
{
	uint16_t src_str_length ;

	src_str_length = strnlen((char*)src, max_length);

	/* max_length include trailing '\0' */
	if(src_str_length >= max_length) return 1;

	memcpy(dest, src, src_str_length);
	dest[src_str_length] = 0;
	return 0;
}

/*
 * ESP8266_ioctl()
 *
 * return:
 */
uint8_t ESP8266_ioctl(struct dev_desc_t *adev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	uint8_t retVal = 0;
	struct esp8266_runtime_t *esp8266_dev_state_hndl;

	esp8266_dev_state_hndl = DEV_GET_RUNTIME_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case IOCTL_ESP8266_SET_SSID_NAME :
		retVal = set_string(esp8266_dev_state_hndl->ssid_name, aIoctl_param1,
				ESP8266_MAX_SSID_NAME_LEN);
		break;
	case IOCTL_ESP8266_SET_SSID_PSWRD :
		retVal = set_string(esp8266_dev_state_hndl->ssid_pswrd, aIoctl_param1,
				ESP8266_MAX_SSID_PSWRD_LEN);
		break;
	case IOCTL_ESP8266_SET_SSID_NAME_REDANDENCY :
		retVal = set_string(esp8266_dev_state_hndl->ssid_name_redandency,
				aIoctl_param1, ESP8266_MAX_SSID_NAME_LEN);
		break;
	case IOCTL_ESP8266_SET_SSID_PSWRD_REDANDENCY :
		retVal = set_string(esp8266_dev_state_hndl->ssid_pswrd_redandency,
				aIoctl_param1, ESP8266_MAX_SSID_PSWRD_LEN);
		break;
	case IOCTL_ESP8266_SOCKET_OPEN:
		retVal = send_socket_open_msg(esp8266_dev_state_hndl, aIoctl_param1);
		break;
	case IOCTL_ESP8266_GET_IP:
		retVal = send_get_ip_msg(esp8266_dev_state_hndl, aIoctl_param1);
		break;
	case IOCTL_DEVICE_START :
		retVal = ESP8266_device_start(adev);
		break;
	default :
		return 1;
	}
	return retVal;
}
