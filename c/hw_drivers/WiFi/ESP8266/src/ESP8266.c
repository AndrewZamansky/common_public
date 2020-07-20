/*
 *
 * file :   ESP8266.c
 *
 */

#include "_project.h"
#include "errors_api.h"

#include "dev_management_api.h"
#define DEBUG
#include "PRINTF_api.h"
#include "ESP8266_api.h"
#include "ESP8266.h"
#include "timer_wrapper_api.h"
#include "os_wrapper.h"
#include "auto_init_api.h"
#include "uart_api.h"
#include <string.h>

#ifndef  ESP8266_TASK_PRIORITY
	#error  "ESP8266_TASK_PRIORITY should be define in project"
#endif
#ifndef  ESP8266_TASK_STACK_SIZE
	#error  "ESP8266_TASK_STACK_SIZE should be define in project"
#endif



#define AP_CONNECT_TIMEOUT  15000
#define ESP8266_TIMEOUT     10000//3000
#define ESP8266_MAX_QUEUE_LEN   ( 3 )
#define MAX_RCVD_BUFFER_SIZE   8000

#define SEND_TO_ESP8266(dev, data, len)    DEV_WRITE(dev , data , len)
#define HANDSHAKE_TRIES  3


extern uint8_t _do_uart_dbg_print;


static uint8_t dummy_msg;

static void set_AP(
		struct esp8266_cfg_t *config_handle, char* ssid_name, char* ssid_pswrd);

static char unknown_state[] = "unknown state XX";
char* ESP8266_get_state_name(uint16_t state)
{
	switch (state)
	{
	case ESP8266_State_InitialHandShake : return "HandShake";
	case ESP8266_State_StartResetting : return "StartResetting";
	case ESP8266_State_Resetting : return "Resetting";
	case ESP8266_State_Setting_Echo_Off : return "Setting_Echo_Off";
	case ESP8266_State_DisconnectWIFI_before_baud_rate_change :
		return "DisconnectWIFI_before_baud_rate_change";
	case ESP8266_State_ChangingBaudRate : return "ChangingBaudRate";
	case ESP8266_State_BaudChangeHandShake : return "BaudChangeHandShake";
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
static void send_str_to_chip (struct esp8266_cfg_t *config_handle, char *data)
{

	SEND_TO_ESP8266(config_handle->uart_tx_dev , (uint8_t*)data,strlen(data));
}


/*
 * ESP8266_callback()
 *
 * return:
 */
static uint8_t ESP8266_callback(
		struct dev_desc_t *adev, const uint8_t aCallback_num,
		void * aCallback_param1, void * aCallback_param2)
{
	struct esp8266_runtime_t *esp8266_runtime_hndl;
	os_queue_t main_queue;
	struct esp8266_message_t  queueMsg;

	esp8266_runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(ESP8266, adev);

	main_queue = esp8266_runtime_hndl->main_queue;
	if (NULL == main_queue) return 1;

	queueMsg.type = DATA_FROM_UART;
	if (OS_QUEUE_SEND_SUCCESS ==
			os_queue_send_without_wait( main_queue, ( void * ) &queueMsg))
	{
		//PRINTF_DBG("esp queue sent\n");
	}
	else
	{
		//PRINTF_DBG("esp not queue sent\n");
	}
	return 0;
}


/*
 * send_message_and_wait()
 *
 * return:
 */
static uint8_t  send_message_and_wait(
		struct esp8266_runtime_t *esp8266_runtime_hndl,
		const struct esp8266_message_t  *queueMsg)
{
	uint8_t error;
	os_mutex_t sendDataMutex;

	if (0 == esp8266_runtime_hndl->init_done) return 1;

	sendDataMutex = esp8266_runtime_hndl->sendDataMutex;

	os_mutex_take_infinite_wait(sendDataMutex);
	esp8266_runtime_hndl->lCurrError = 0;

	os_queue_send_infinite_wait(esp8266_runtime_hndl->main_queue, queueMsg);

	os_queue_receive_infinite_wait(
				esp8266_runtime_hndl->end_of_msg_queue, &dummy_msg);

	error = esp8266_runtime_hndl->lCurrError;
	if (error)
	{
		PRINTF_DBG("esp: err =%d\n", error);
	}

	os_mutex_give(sendDataMutex);
	return error;
}


static uint8_t cmpBuff2Str(const uint8_t *buf, size_t buf_len, const char *str)
{/*maximal length of string should be 16*/
	size_t str_len;
	str_len = strnlen(str, 17);
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
		struct esp8266_runtime_t *esp8266_runtime_hndl,
		uint8_t *pBufferStart, size_t total_length)
{
	uint8_t *recvedData;
	uint64_t timeout ;
	struct esp8266_socket_t  *curr_rcv_data_socket;
	size_t  curr_data_size, bytes_to_copy;
	size_t leftDataToReceive;
	struct esp8266_socket_t  *sockets;

	leftDataToReceive = esp8266_runtime_hndl->leftDataToReceive;
	sockets = esp8266_runtime_hndl->sockets;
	curr_rcv_data_socket =
			&sockets[esp8266_runtime_hndl->currentSocketNumber];
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
		timeout = ESP8266_TIMEOUT;
		DEV_IOCTL(config_handle->timer_dev,
			IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_mSEC_AND_RESET, &timeout);
	}
	else
	{
		//PRINTF_DBG("--end rcv_data\r\n" );
		if (leftDataToReceive < bytes_to_copy)
		{
			bytes_to_copy = leftDataToReceive;
		}
		else
		{
			CRITICAL_ERROR("esp8266 input buffer overflow");
		}
		os_queue_send_without_wait(
				curr_rcv_data_socket->wake_queue, ( void *)&dummy_msg);
		esp8266_runtime_hndl->currentState =
				esp8266_runtime_hndl->returnFromDataReceiveState;
		esp8266_runtime_hndl->currentSocketNumber =
				esp8266_runtime_hndl->returnFromDataReceiveStateSocketNumber;
	}

	if (curr_rcv_data_socket->socket_in_use)
	{
		memcpy(&recvedData[curr_data_size], pBufferStart , bytes_to_copy);
		curr_data_size += bytes_to_copy;
		curr_rcv_data_socket->curr_data_size = curr_data_size;
	}
	leftDataToReceive -= bytes_to_copy;
	esp8266_runtime_hndl->leftDataToReceive = leftDataToReceive;
	return (0 < bytes_to_copy) ? bytes_to_copy : 1;
}


static size_t process_new_incoming_net_data(
		struct esp8266_cfg_t *config_handle,
		struct esp8266_runtime_t *esp8266_runtime_hndl,
		uint8_t *pBufferStart, size_t total_length,
		uint8_t *is_NOT_incoming_data)
{
	char *start_of_requested_str;
	char *end_of_requested_str;
	char *start_of_data_str;
	size_t header_size;
	uint8_t receivedSocketNumber;
	uint64_t timeout ;
	size_t  leftDataToReceive;

	*is_NOT_incoming_data = 0;
	if (7 > total_length) return 0;

	// check if buffer start "+IPD,nn,nnn:" or (TODO "+IPD,n,nnn")
	if (0 != memcmp("+IPD,", pBufferStart, 5))
	{
		*is_NOT_incoming_data = 1;
		return 1;
	}

	pBufferStart += 5;
	total_length -= 5;
	start_of_data_str = memchr(pBufferStart, ':', total_length);
	if (NULL == start_of_data_str)
	{
		if (10 > total_length)
		{
			return 0;
		}
		else
		{
			*is_NOT_incoming_data = 1;
			return 1;
		}
	}

	header_size = start_of_data_str - (char*)pBufferStart;
	pBufferStart[header_size] = 0;// create valid string

	receivedSocketNumber = (uint8_t)strtoul((char*)pBufferStart,
			&start_of_requested_str,10);
	if ((receivedSocketNumber >= ESP8266_MAX_NUM_OF_SOCKETS)
			|| (start_of_requested_str == (char*)pBufferStart))
	{
		pBufferStart[header_size] = ':';
		*is_NOT_incoming_data = 1;
		return 1;
	}

	if (',' != *start_of_requested_str)
	{
		pBufferStart[header_size] = ':';
		*is_NOT_incoming_data = 1;
		return 1;
	}
	start_of_requested_str++;
	leftDataToReceive = strtoul(
			(char*)start_of_requested_str, &end_of_requested_str, 10);

	if (start_of_requested_str == end_of_requested_str)
	{
		pBufferStart[header_size] = ':';
		*is_NOT_incoming_data = 1;
		return 1;
	}


	//PRINTF_DBG("esp new in=%d\r\n" , leftDataToReceive);
	esp8266_runtime_hndl->returnFromDataReceiveState =
						esp8266_runtime_hndl->currentState;
	esp8266_runtime_hndl->returnFromDataReceiveStateSocketNumber =
								esp8266_runtime_hndl->currentSocketNumber;
	esp8266_runtime_hndl->currentSocketNumber = receivedSocketNumber;
	esp8266_runtime_hndl->currentState = ESP8266_State_Receiving_Data;
	esp8266_runtime_hndl->leftDataToReceive = leftDataToReceive;
	timeout = ESP8266_TIMEOUT;
	DEV_IOCTL(config_handle->timer_dev ,
			IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_mSEC_AND_RESET, &timeout);
	return header_size + 6;/* 5 for "+IPD,"   , 1 for ":"  */
}


static size_t process_receiving_socket_close(
		struct esp8266_runtime_t *esp8266_runtime_hndl,
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

	PRINTF_DBG("!esp rec closed socket #%d\n", receivedSocketNumber);
	curr_closing_socket =
			&esp8266_runtime_hndl->sockets[receivedSocketNumber];
	curr_closing_socket->socket_in_use = 0;
	os_safe_free(curr_closing_socket->recvedData);
	curr_closing_socket->recvedData = NULL ;
	curr_closing_socket->curr_data_size = 0;
	return line_length + 1 ;
}


static size_t process_data_from_esp8266_on_idle(
		struct esp8266_cfg_t *config_handle,
		struct esp8266_runtime_t *esp8266_runtime_hndl,
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


	if ('+' == *pBufferStart)
	{
		uint8_t is_NOT_incoming_data;
		bytes_consumed = process_new_incoming_net_data(config_handle,
				esp8266_runtime_hndl,
				pBufferStart, total_length, &is_NOT_incoming_data);
	}
	else if (isdigit(*pBufferStart))
	{
		bytes_consumed = process_receiving_socket_close(
				esp8266_runtime_hndl,
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
		struct esp8266_runtime_t *esp8266_runtime_hndl,
		uint8_t *pBufferStart, size_t total_length)
{
	struct esp8266_msg_send_data_to_socket_t*  pmsg_send_data_to_socket;
	size_t  bytes_consumed;
	uint64_t  timeout ;

	pmsg_send_data_to_socket =
			&esp8266_runtime_hndl->pendingMessage.
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

	esp8266_runtime_hndl->currentState = ESP8266_State_Wait_Send_Complete;
	timeout = ESP8266_TIMEOUT;
	DEV_IOCTL(config_handle->timer_dev,
		IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_mSEC_AND_RESET, &timeout);
	return bytes_consumed + 1;
}


static enum ESP8266_State_e parse_wait_for_get_ip_response(
		struct esp8266_runtime_t *esp8266_runtime_hndl,
		uint8_t *pBufferStart, size_t line_length)
{
	struct esp8266_msg_getIP_t   *pmsg_getIP;
	char *start_of_requested_str;
	char *end_of_requested_str;
	enum ESP8266_State_e currentState ;

	currentState = esp8266_runtime_hndl->currentState;

	if (0 != cmpBuff2Str(pBufferStart, line_length, "+CIFSR:STAIP,"))
	{
		return currentState;
	}

	start_of_requested_str = strchr((char*)pBufferStart , '\"');
	if (NULL == start_of_requested_str) return currentState;

	start_of_requested_str++;
	end_of_requested_str = strchr(start_of_requested_str , '\"');
	if (NULL == end_of_requested_str) return currentState;

	*end_of_requested_str = 0;
	pmsg_getIP = &esp8266_runtime_hndl->pendingMessage.msg_data.msg_getIP;
	strncpy(pmsg_getIP->IPstr, start_of_requested_str, pmsg_getIP->strIPLen);

	return ESP8266_State_Wait_For_IP_Complete;
}



static enum ESP8266_State_e parse_wait_for_send_complete(
		struct esp8266_runtime_t *esp8266_runtime_hndl,
		uint8_t *pBufferStart, size_t line_length)
{
	uint8_t send_complete;

	send_complete = 0;
	if (0 == cmpBuff2Str(pBufferStart, line_length, "SEND OK"))
	{
		send_complete = 1;
	}
	else if (0 == cmpBuff2Str(pBufferStart, line_length, "link is not"))
	{
		esp8266_runtime_hndl->lCurrError = ESP8266_ERR_CANNOT_SEND_BAD_LINK;
		send_complete = 1;
	}

	if (send_complete)
	{
		//_do_uart_dbg_print = 0;
		esp8266_runtime_hndl->lRequest_done = 1;
		return ESP8266_State_Idle;
	}
	return esp8266_runtime_hndl->currentState;
}


static enum ESP8266_State_e parse_wait_for_get_socket_status_response(
		struct esp8266_runtime_t *esp8266_runtime_hndl,
		uint8_t *pBufferStart, size_t line_length)
{
	uint8_t receivedSocketNumber;
	char *start_of_requested_str;
	enum ESP8266_State_e currentState ;

	currentState = esp8266_runtime_hndl->currentState;

	if (18 > line_length) return currentState;

	if (0 != cmpBuff2Str(pBufferStart, line_length, "+CIPSTATUS"))
	{
		return currentState;
	}

	receivedSocketNumber = (uint8_t)strtoul(
			(char*)&pBufferStart[11], (char**)&start_of_requested_str, 10);
	if (esp8266_runtime_hndl->currentSocketNumber == receivedSocketNumber)
	{
		struct esp8266_msg_get_open_connection_t   *pmsg_get_open_connection;
		char *end_of_requested_str;
		pmsg_get_open_connection = &esp8266_runtime_hndl->pendingMessage.
				msg_data.msg_get_open_connection;

		start_of_requested_str = strchr(start_of_requested_str , '\"');
		if(NULL == start_of_requested_str) return currentState;
		start_of_requested_str = strchr(start_of_requested_str + 1 , '\"');
		if(NULL == start_of_requested_str) return currentState;
		start_of_requested_str = strchr(start_of_requested_str + 1 , '\"');
		if(NULL == start_of_requested_str) return currentState;
		end_of_requested_str = strchr(start_of_requested_str + 1 , '\"');
		if(NULL == end_of_requested_str) return currentState;
		*end_of_requested_str = 0;
		strncpy(pmsg_get_open_connection->strIP,start_of_requested_str + 1,
				pmsg_get_open_connection->strIPLen);
		*pmsg_get_open_connection->pPort =
					strtoul(end_of_requested_str + 1,NULL,10);
		currentState = ESP8266_State_Wait_For_Socket_Status_Complete;
	}
	return currentState;
}


static enum ESP8266_State_e parse_wait_for_connecting_socket_response(
		struct esp8266_runtime_t *esp8266_runtime_hndl,
		uint8_t *pBufferStart, size_t line_length)
{
	enum ESP8266_State_e currentState ;

	currentState = esp8266_runtime_hndl->currentState;
	if (0 == cmpBuff2Str(pBufferStart, line_length, "OK"))
	{
		esp8266_runtime_hndl->lRequest_done = 1;
		currentState = ESP8266_State_Idle;
	}
	else if ( (0 == cmpBuff2Str(pBufferStart, line_length, "ERROR")) ||
				(0 == cmpBuff2Str(pBufferStart, line_length, "ALREADY CONN")) )
	{
		esp8266_runtime_hndl->lCurrError = ESP8266_ERR_CANNOT_CONNECT;
		esp8266_runtime_hndl->lRequest_done = 1;
		currentState = ESP8266_State_Idle;
	}
	else if (0 == cmpBuff2Str(pBufferStart, line_length, "Link typ ERROR"))
	{
		esp8266_runtime_hndl->lCurrError = ESP8266_ERR_CANNOT_CONNECT_LINK_ERR;
		esp8266_runtime_hndl->lRequest_done = 1;
		esp8266_runtime_hndl->need_to_reset = 1;
		currentState = ESP8266_State_InitialHandShake;
	}
	return currentState;
}


static void close_socket(struct esp8266_runtime_t *esp8266_runtime_hndl)
{
	struct esp8266_socket_t  *curr_socket;
	struct esp8266_socket_t  *sockets;
	sockets = esp8266_runtime_hndl->sockets;
	curr_socket = &sockets[esp8266_runtime_hndl->currentSocketNumber];
	curr_socket->socket_in_use = 0;
	os_safe_free(curr_socket->recvedData);
	curr_socket->recvedData = NULL ;
	curr_socket->curr_data_size = 0;
	//PRINTF_DBG("close socket #%d\n", esp8266_runtime_hndl->currentSocketNumber);

}


static enum ESP8266_State_e parse_wait_for_close_socket_response(
		struct esp8266_runtime_t *esp8266_runtime_hndl,
		uint8_t *pBufferStart, size_t line_length)
{
	uint8_t command_completed ;

	command_completed = 0;
	if (0 == cmpBuff2Str(pBufferStart, line_length, "OK"))
	{
		command_completed = 1;
	}
	else if  (0 == cmpBuff2Str(pBufferStart, line_length, "ERROR"))
	{
		esp8266_runtime_hndl->lCurrError = ESP8266_ERR_CANNOT_CLOSE;
		command_completed = 1;
	}

	if (command_completed)
	{
		close_socket(esp8266_runtime_hndl);
		esp8266_runtime_hndl->lRequest_done = 1;
		return ESP8266_State_Idle;
	}

	return esp8266_runtime_hndl->currentState;
}


static enum ESP8266_State_e parse_wait_for_wifi_connect_response(
		struct esp8266_cfg_t *config_handle,
		struct esp8266_runtime_t *esp8266_runtime_hndl,
		uint8_t *pBufferStart, size_t line_length)
{
	enum ESP8266_State_e currentState ;

	char *curr_ssid_name;
	char *curr_ssid_pswrd;

	currentState = esp8266_runtime_hndl->currentState;
	if (0 == cmpBuff2Str(pBufferStart, line_length,"OK"))
	{
		send_str_to_chip(config_handle, "AT+CIPMUX=1\r\n");
		currentState = ESP8266_State_Setting_Connection_Type;
		esp8266_runtime_hndl->wifi_connected = 1;
		PRINTF_DBG("wifi connected  \r\n");
	}
	else if (0 == cmpBuff2Str(pBufferStart, line_length,"FAIL"))
	{
		if (ESP8266_State_Setting_AP == currentState)
		{
			curr_ssid_name = esp8266_runtime_hndl->ssid_name_redandency;
			curr_ssid_pswrd = esp8266_runtime_hndl->ssid_pswrd_redandency;
			currentState = ESP8266_State_Setting_Redundent_Ap ;
		}
		else if (ESP8266_State_Setting_Redundent_Ap == currentState)
		{
			curr_ssid_name = esp8266_runtime_hndl->ssid_name;
			curr_ssid_pswrd = esp8266_runtime_hndl->ssid_pswrd;
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
			set_AP(config_handle, curr_ssid_name, curr_ssid_pswrd);
		}
	}
	return currentState;
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
		struct esp8266_runtime_t *esp8266_runtime_hndl,
		uint8_t *pBufferStart, size_t total_length)
{
	size_t  line_length;
	uint64_t timeout ;
	enum ESP8266_State_e  currentState ;

	if ('+' == *pBufferStart)
	{
		size_t bytes_consumed;
		uint8_t is_NOT_incoming_data;

		bytes_consumed = process_new_incoming_net_data(config_handle,
				esp8266_runtime_hndl,
				pBufferStart, total_length, &is_NOT_incoming_data);
		if (0 == is_NOT_incoming_data)
		{
			return bytes_consumed;
		}
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

//	PRINT_DATA_DBG("esp resp:", 9);
//	PRINT_DATA_DBG(pBufferStart, line_length);
//	PRINT_DATA_DBG("\n", 1);

	pBufferStart[line_length] = 0;
	timeout = ESP8266_TIMEOUT;
	currentState = esp8266_runtime_hndl->currentState;
	switch (currentState)
	{
	case ESP8266_State_InitialHandShake:
		if (0 == cmpBuff2Str(pBufferStart, line_length,"OK"))
		{
			if (esp8266_runtime_hndl->need_to_reset)
			{
				send_str_to_chip(config_handle, "AT+RST\r\n");
				currentState = ESP8266_State_StartResetting;
			}
		}
		break;
	case ESP8266_State_StartResetting:
		if (0 == cmpBuff2Str(pBufferStart, line_length,"OK"))
		{
			uint32_t interface_device_speed  = 115200;
			esp8266_runtime_hndl->interface_device_speed
												= interface_device_speed;
			DEV_IOCTL(config_handle->uart_dev ,
					IOCTL_UART_SET_BAUD_RATE, &interface_device_speed);
			currentState = ESP8266_State_Resetting;
		}
		break;
	case ESP8266_State_Resetting:
		if (0 == cmpBuff2Str(pBufferStart, line_length,"ready"))
		{
			send_str_to_chip(config_handle, "ATE0\r\n");// setting echo off
			currentState = ESP8266_State_DisconnectWIFI_before_baud_rate_change;
		}
		break;
	case ESP8266_State_DisconnectWIFI_before_baud_rate_change :
		if (0 == cmpBuff2Str(pBufferStart, line_length,"OK"))
		{
			send_str_to_chip(config_handle, "AT+CWQAP\r\n");
			currentState = ESP8266_State_Setting_Echo_Off ;
		}
		break;
	case ESP8266_State_Setting_Echo_Off :
		if (0 == cmpBuff2Str(pBufferStart, line_length,"OK"))
		{
			send_str_to_chip(config_handle, "AT+UART_CUR=921600,8,1,0,1\r\n");
			currentState = ESP8266_State_ChangingBaudRate;
		}
		break;
	case ESP8266_State_ChangingBaudRate :
		if (0 == cmpBuff2Str(pBufferStart, line_length,"OK"))
		{
			uint32_t interface_device_speed  = 921600;
			esp8266_runtime_hndl->interface_device_speed
												= interface_device_speed;
			DEV_IOCTL(config_handle->uart_dev ,
					IOCTL_UART_SET_BAUD_RATE, &interface_device_speed);
			os_delay_ms(20);// maybe should be increased on handshake problems
			send_str_to_chip(config_handle, "AT\r\n");
			currentState = ESP8266_State_BaudChangeHandShake ;
			esp8266_runtime_hndl->handshake_counter = HANDSHAKE_TRIES;
			timeout = 100;
		}
		break;
	case ESP8266_State_BaudChangeHandShake :
		if (0 == cmpBuff2Str(pBufferStart, line_length,"OK"))
		{
			send_str_to_chip(config_handle, "AT+CWMODE=1\r\n");
			currentState = ESP8266_State_Setting_Mode ;
		}
		break;
	case ESP8266_State_Setting_Mode:
		if (0 == cmpBuff2Str(pBufferStart, line_length,"OK"))
		{
			set_AP(config_handle, esp8266_runtime_hndl->ssid_name,
								esp8266_runtime_hndl->ssid_pswrd);
			currentState = ESP8266_State_Setting_AP ;
			timeout = AP_CONNECT_TIMEOUT;
		}
		break;
	case ESP8266_State_Wait_For_IP:
		currentState = parse_wait_for_get_ip_response(
				esp8266_runtime_hndl, pBufferStart, line_length);
		break;
	case ESP8266_State_Wait_For_IP_Complete:
		if (0 == cmpBuff2Str(pBufferStart, line_length,"OK"))
		{
			esp8266_runtime_hndl->lRequest_done = 1;
			currentState = ESP8266_State_Idle;
		}
		break;
	case ESP8266_State_Wait_For_Socket_Status:
		currentState = parse_wait_for_get_socket_status_response(
					esp8266_runtime_hndl, pBufferStart, line_length);
		break;
	case ESP8266_State_Wait_For_Socket_Status_Complete:
		if (0 == cmpBuff2Str(pBufferStart, line_length,"OK"))
		{
			esp8266_runtime_hndl->lRequest_done = 1;
			currentState = ESP8266_State_Idle;
		}
		break;
	case ESP8266_State_Wait_Send_Complete:
		currentState = parse_wait_for_send_complete(
				esp8266_runtime_hndl, pBufferStart, line_length);
		break;
	case ESP8266_State_Connecting_Socket :
		currentState = parse_wait_for_connecting_socket_response(
					esp8266_runtime_hndl, pBufferStart, line_length);
		break;
	case ESP8266_State_Closing_Socket :
		currentState = parse_wait_for_close_socket_response(
					esp8266_runtime_hndl, pBufferStart, line_length);
		break;
	case ESP8266_State_Setting_AP:
	case ESP8266_State_Setting_Redundent_Ap:
		currentState = parse_wait_for_wifi_connect_response(config_handle,
				esp8266_runtime_hndl, pBufferStart, line_length);
		timeout = AP_CONNECT_TIMEOUT;
		break;
	case ESP8266_State_Setting_Connection_Type:
		if (0 == cmpBuff2Str(pBufferStart, line_length, "OK"))
		{
			send_str_to_chip(config_handle, "AT+CIPSERVER=1,80\r\n");
			currentState = ESP8266_State_Creating_Server;
		}
		break;
	case ESP8266_State_Creating_Server:
		if (0 == cmpBuff2Str(pBufferStart, line_length, "OK"))
		{
			send_str_to_chip(config_handle, "AT+CIPSTO=200\r\n");
			currentState = ESP8266_State_Setting_Timeout;
		}
		break;
	case ESP8266_State_Setting_Timeout:
		if (0 == cmpBuff2Str(pBufferStart, line_length, "OK"))
		{
			currentState = ESP8266_State_Idle;
		}
		break;
	default :
		break;
	}
	esp8266_runtime_hndl->currentState = currentState;

	DEV_IOCTL(config_handle->timer_dev,
			IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_mSEC_AND_RESET, &timeout);

	line_length++;

	line_length += calculate_eol_chars(
			&pBufferStart[line_length], total_length - line_length);

	return line_length ;
}


/*
 * process_message_from_hw()
 *
 * return:
 */
static void process_message_from_hw(struct esp8266_cfg_t *config_handle,
		struct esp8266_runtime_t *esp8266_runtime_hndl)
{
	struct dev_desc_t *   uart_rx_dev;
	struct ioctl_get_data_buffer_t data_buffer_info;
	enum ESP8266_State_e currentState ;
	size_t bytes_consumed;
	uart_rx_dev = config_handle->uart_rx_dev;

	while(1)
	{
		size_t total_length;
		uint8_t *pBufferStart;

		DEV_IOCTL(uart_rx_dev,
				IOCTL_GET_AND_LOCK_DATA_BUFFER, &data_buffer_info);

		total_length = data_buffer_info.TotalLength;
		pBufferStart = data_buffer_info.pBufferStart;

		if(data_buffer_info.bufferWasOverflowed)
		{
			CRITICAL_ERROR("TODO :  deal with overflow");
		}

		if ((0 == total_length) ||
				((0 == esp8266_runtime_hndl->last_bytes_consumed) &&
				(esp8266_runtime_hndl->last_tested_length == total_length)))
		{
			break;
		}
		esp8266_runtime_hndl->last_tested_length = total_length;

		currentState = esp8266_runtime_hndl->currentState;
//		PRINTF_DBG( "\r\n--%s--\r\n",
//						ESP8266_get_state_name(currentState));
//		PRINTF_DBG("[t=%d[", total_length);
//		PRINT_DATA_DBG(pBufferStart , total_length);

		switch(currentState)
		{
		case ESP8266_State_Receiving_Data :
			bytes_consumed = receiving_incoming_net_data(config_handle,
					esp8266_runtime_hndl,	pBufferStart , total_length);
			break;
		case ESP8266_State_Idle :
			bytes_consumed = process_data_from_esp8266_on_idle(config_handle,
				esp8266_runtime_hndl,	pBufferStart , total_length);
			break;
		case ESP8266_State_Wait_For_Send_Ready :
			bytes_consumed = process_data_from_esp8266_on_wait_for_send_ready(
					config_handle, esp8266_runtime_hndl,
					pBufferStart , total_length);
			break;
		default:
			bytes_consumed = process_data_from_esp8266_on_wait_for_response(
					config_handle, esp8266_runtime_hndl,
					pBufferStart , total_length);
			break;
		}
		//PRINTF_DBG( "\r\n--new state : %s--\r\n",
		//		ESP8266_get_state_name(esp8266_runtime_hndl->currentState));

		//PRINTF_DBG("]c=%d]" , bytes_consumed);

		esp8266_runtime_hndl->last_bytes_consumed = bytes_consumed;
		if (0 == bytes_consumed)
		{
			break;
		}

		DEV_IOCTL(uart_rx_dev,
			IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER , (void*)bytes_consumed);
	}

	DEV_IOCTL_0_PARAMS(uart_rx_dev, IOCTL_SET_UNLOCK_DATA_BUFFER);
}


static enum ESP8266_State_e process_close_socket_message(
		struct esp8266_msg_close_socket_t  *pmsg_close_socket,
		struct esp8266_runtime_t *esp8266_runtime_hndl,
		struct esp8266_cfg_t *config_handle, char *sendBuffer)
{
	struct esp8266_socket_t *socket_handle;

	socket_handle = pmsg_close_socket->socket_handle;
	if(0 != socket_handle->socket_in_use)
	{
		PRINTF_DBG("esp:close soc=%d\n", socket_handle->socket_number);
		esp8266_runtime_hndl->currentSocketNumber =
										socket_handle->socket_number;
		snprintf(sendBuffer, ESP8266_SEND_BUFFER_LEN,
				"AT+CIPCLOSE=%d\r\n", socket_handle->socket_number);
		send_str_to_chip(config_handle,  sendBuffer );
		return ESP8266_State_Closing_Socket;
	}
	else
	{
		esp8266_runtime_hndl->lCurrError = ESP8266_ERR_SOCKET_NOT_AVAILABLE;
		esp8266_runtime_hndl->lRequest_done = 1;
		return ESP8266_State_Idle;
	}
}


static enum ESP8266_State_e process_send_data_message(
		struct esp8266_msg_send_data_to_socket_t *pmsg_send_data_to_socket,
		struct esp8266_runtime_t *esp8266_runtime_hndl,
		struct esp8266_cfg_t *config_handle, char *sendBuffer)
{
	struct esp8266_socket_t *socket_handle;

	socket_handle = pmsg_send_data_to_socket->socket_handle;
	if(0 != socket_handle->socket_in_use)
	{
//		PRINTF_DBG("esp:send soc=%d, len=%d\n", socket_handle->socket_number,
//										pmsg_send_data_to_socket->data_length);
//
//		_do_uart_dbg_print = 1;
		snprintf(sendBuffer, ESP8266_SEND_BUFFER_LEN,
				"AT+CIPSEND=%d,%d\r\n", socket_handle->socket_number,
				pmsg_send_data_to_socket->data_length);
		send_str_to_chip(config_handle,  sendBuffer );

		//if(pmsg_send_data_to_socket->data_length == 181) dbg_trigger=1;

		return ESP8266_State_Wait_For_Send_Ready;
	}
	else
	{
		esp8266_runtime_hndl->lCurrError = ESP8266_ERR_SOCKET_NOT_AVAILABLE;
		esp8266_runtime_hndl->lRequest_done = 1;
		return ESP8266_State_Idle;
	}
}


static enum ESP8266_State_e process_open_socket_message(
		struct esp8266_msg_open_socket_t *pmsg_open_socket,
		struct esp8266_runtime_t *esp8266_runtime_hndl)
{
	struct esp8266_socket_t  *socket_handle;
	struct esp8266_socket_t * new_socket_handle;
	size_t socket_num ;

	new_socket_handle = NULL;
	socket_handle = esp8266_runtime_hndl->sockets;
	for(socket_num = 0;
			socket_num < ESP8266_MAX_NUM_OF_SOCKETS; socket_num++)
	{
		if (0 == socket_handle->socket_in_use)
		{

			socket_handle->socket_in_use = 1;
			socket_handle->recvedData =
					(uint8_t*)os_safe_malloc(MAX_RCVD_BUFFER_SIZE);
			errors_api_check_if_malloc_succeed(socket_handle->recvedData);

			socket_handle->curr_data_size = 0;

			new_socket_handle = socket_handle;
			break;
		}
		socket_handle++;
	}
	*pmsg_open_socket->new_socket_handle = new_socket_handle;
	if(ESP8266_MAX_NUM_OF_SOCKETS == socket_num)
	{
		esp8266_runtime_hndl->lCurrError = ESP8266_ERR_NO_FREE_SOCKET_LEFT;
	}
	esp8266_runtime_hndl->lRequest_done = 1;
	return ESP8266_State_Idle;
}


static enum ESP8266_State_e process_connect_socket_message(
		struct esp8266_msg_connect_socket_t *pmsg_connect_socket,
		struct esp8266_runtime_t *esp8266_runtime_hndl,
		struct esp8266_cfg_t *config_handle, char *sendBuffer)
{
	struct esp8266_socket_t *socket_handle;

	socket_handle = pmsg_connect_socket->socket_handle;
	if(0 != socket_handle->socket_in_use)
	{
		PRINTF_DBG("esp:open socket #%d\n", socket_handle->socket_number);
		snprintf(sendBuffer, ESP8266_SEND_BUFFER_LEN,
			"AT+CIPSTART=%d,\"TCP\",\"%s\",%s\r\n",
			socket_handle->socket_number,
			pmsg_connect_socket->strHostName,
			pmsg_connect_socket->port);
		send_str_to_chip(config_handle, sendBuffer);
		return ESP8266_State_Connecting_Socket;
	}
	else
	{
		PRINTF_DBG("ERR open socket #%d\n", socket_handle->socket_number);
		esp8266_runtime_hndl->lCurrError = ESP8266_ERR_SOCKET_NOT_AVAILABLE;
		esp8266_runtime_hndl->lRequest_done = 1;
		return ESP8266_State_Idle;
	}
}


static enum ESP8266_State_e process_get_connection_status_message(
		struct esp8266_msg_get_open_connection_t *pmsg_get_open_connection,
		struct esp8266_runtime_t *esp8266_runtime_hndl,
		struct esp8266_cfg_t *config_handle)
{
	struct esp8266_socket_t *socket_handle;

	socket_handle = pmsg_get_open_connection->socket_handle;
	if(0 != socket_handle->socket_in_use)
	{
		esp8266_runtime_hndl->currentSocketNumber =
				socket_handle->socket_number;
		send_str_to_chip(config_handle, "AT+CIPSTATUS\r\n");
		return ESP8266_State_Wait_For_Socket_Status;
	}
	else
	{
		esp8266_runtime_hndl->lCurrError = ESP8266_ERR_SOCKET_NOT_AVAILABLE;
		esp8266_runtime_hndl->lRequest_done = 1;
		return ESP8266_State_Idle;
	}
}


static enum ESP8266_State_e process_check_if_data_rcvd_message(
	struct esp8266_msg_check_if_new_data_rcvd_t *pmsg_check_if_new_data_rcvd,
	struct esp8266_runtime_t *esp8266_runtime_hndl)
{
	struct esp8266_socket_t *socket_handle;

	socket_handle = pmsg_check_if_new_data_rcvd->socket_handle;
	*pmsg_check_if_new_data_rcvd->newDataExists =
			socket_handle->curr_data_size ? 1 : 0;
	if(0 == socket_handle->socket_in_use)
	{
		esp8266_runtime_hndl->lCurrError = ESP8266_ERR_SOCKET_NOT_AVAILABLE;
	}
	esp8266_runtime_hndl->lRequest_done = 1;
	return ESP8266_State_Idle;
}


static enum ESP8266_State_e process_check_get_rcvd_data_message(
		struct esp8266_msg_get_data_received_t * pmsg_get_data_received,
		struct esp8266_runtime_t *esp8266_runtime_hndl)
{
	struct esp8266_socket_t *socket_handle;

	socket_handle = pmsg_get_data_received->socket_handle;
	if(0 != socket_handle->socket_in_use)
	{
		size_t bytes_to_copy;
		size_t curr_data_size;

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
		esp8266_runtime_hndl->lCurrError = ESP8266_ERR_SOCKET_NOT_AVAILABLE;
	}
	esp8266_runtime_hndl->lRequest_done = 1;
	return ESP8266_State_Idle;
}


/*
 * process_software_message()
 *
 * return:
 */
static void process_software_message(struct esp8266_cfg_t *config_handle,
		struct esp8266_runtime_t *esp8266_runtime_hndl)
{
	struct esp8266_message_t  pendingMessage;
	struct dev_desc_t *   timer_dev;
	uint64_t timeout;
	char *sendBuffer;
	enum ESP8266_State_e currentState ;


	pendingMessage = esp8266_runtime_hndl->pendingMessage;
	//PRINTF_DBG("---ESP process_message=%d \r\n" ,pendingMessage.type);
	sendBuffer = esp8266_runtime_hndl->sendBuffer;

	timeout=ESP8266_TIMEOUT;
	switch(pendingMessage.type)
	{
	case CLOSE_SOCKET :
		currentState = process_close_socket_message(
				&pendingMessage.msg_data.msg_close_socket,
				esp8266_runtime_hndl, config_handle, sendBuffer);
		break;
	case SEND_DATA :
		currentState = process_send_data_message(
				&pendingMessage.msg_data.msg_send_data_to_socket,
				esp8266_runtime_hndl, config_handle, sendBuffer);
		break;
	case OPEN_SOCKET :
		currentState = process_open_socket_message(
				&pendingMessage.msg_data.msg_open_socket,
				esp8266_runtime_hndl);
		break;
	case CONNECT_SOCKET :
		currentState = process_connect_socket_message(
				&pendingMessage.msg_data.msg_connect_socket,
				esp8266_runtime_hndl, config_handle, sendBuffer);
		break;
	case GET_OPEN_CONNECTION_STATUS :
		currentState = process_get_connection_status_message(
				&pendingMessage.msg_data.msg_get_open_connection,
				esp8266_runtime_hndl, config_handle);
		break;
	case GET_IP :
		send_str_to_chip(config_handle, "AT+CIFSR\r\n");
		currentState = ESP8266_State_Wait_For_IP;
		break;
	case CHECK_IF_RECEIVED_DATA:
		currentState = process_check_if_data_rcvd_message(
				&pendingMessage.msg_data.msg_check_if_new_data_received,
				esp8266_runtime_hndl);
		break;
	case GET_RECEIVED_DATA:
		currentState = process_check_get_rcvd_data_message(
				&pendingMessage.msg_data.msg_get_data_received,
				esp8266_runtime_hndl);
		break;
	default:
		currentState = ESP8266_State_Idle;
		break;
	}
	esp8266_runtime_hndl->currentState = currentState;
	timer_dev = config_handle->timer_dev;
	DEV_IOCTL(timer_dev,IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_mSEC_AND_RESET,
			&timeout);
}


static void set_AP(
		struct esp8266_cfg_t *config_handle, char* ssid_name, char* ssid_pswrd)
{
	send_str_to_chip(config_handle, "AT+CWJAP=\"");
	send_str_to_chip(config_handle, ssid_name);
	send_str_to_chip(config_handle, "\",\"");
	send_str_to_chip(config_handle, ssid_pswrd);
	send_str_to_chip( config_handle, "\"\r\n");
}


static void start_from_begining(struct esp8266_cfg_t *config_handle,
							struct esp8266_runtime_t *esp8266_runtime_hndl)
{
	esp8266_runtime_hndl->need_to_reset = 1;
	esp8266_runtime_hndl->currentState = ESP8266_State_InitialHandShake;
	esp8266_runtime_hndl->handshake_counter = HANDSHAKE_TRIES;
	send_str_to_chip(config_handle, "AT\r\n");
}


/*
 * no_new_message_received()
 *
 * return:
 */
static void no_new_message_received(struct esp8266_cfg_t *config_handle,
		struct esp8266_runtime_t *esp8266_runtime_hndl)
{
	enum ESP8266_State_e currentState ;
	struct dev_desc_t *   timer_dev;
	uint64_t timeout;
	uint8_t is_timer_elapsed;
	uint32_t interface_device_speed;

	currentState = esp8266_runtime_hndl->currentState;
	if (ESP8266_State_Idle == currentState) return ;

	timer_dev = config_handle->timer_dev;
	DEV_IOCTL(timer_dev, IOCTL_TIMER_WRAPPER_API_CHECK_IF_COUNTDOWN_ELAPSED,
			&is_timer_elapsed );
	if (0 == is_timer_elapsed) return;

	PRINTF_DBG(
			"--esp timeout crSt=%s\r\n",ESP8266_get_state_name(currentState));
	timeout = ESP8266_TIMEOUT;
	switch(currentState)
	{
	case ESP8266_State_InitialHandShake :
		esp8266_runtime_hndl->handshake_counter--;
		send_str_to_chip(config_handle, "AT\r\n");
		if (0 == esp8266_runtime_hndl->handshake_counter)
		{
			PRINTF_DBG("cannot handshake, switching to next baud rate..\r\n");
			interface_device_speed =
					esp8266_runtime_hndl->interface_device_speed;
			if (115200 == interface_device_speed)
			{
				interface_device_speed = 921600;
			}
			else
			{
				interface_device_speed = 115200;
			}
			esp8266_runtime_hndl->interface_device_speed =
											interface_device_speed;
			DEV_IOCTL(config_handle->uart_dev ,
					IOCTL_UART_SET_BAUD_RATE, &interface_device_speed);
			esp8266_runtime_hndl->handshake_counter = HANDSHAKE_TRIES;
		}
		timeout = 100;
		break;
	case ESP8266_State_BaudChangeHandShake :
		esp8266_runtime_hndl->handshake_counter--;
		send_str_to_chip(config_handle, "AT\r\n");
		if (0 == esp8266_runtime_hndl->handshake_counter)
		{
			CRITICAL_ERROR("cannot handshake on new baud rate");
		}
		timeout = 100;
		break;
	case ESP8266_State_Setting_AP:
		set_AP(config_handle, esp8266_runtime_hndl->ssid_name_redandency,
							esp8266_runtime_hndl->ssid_pswrd_redandency);
		currentState = ESP8266_State_Setting_Redundent_Ap ;
		timeout = AP_CONNECT_TIMEOUT;
		break;
	case ESP8266_State_Setting_Redundent_Ap :
		set_AP(config_handle, esp8266_runtime_hndl->ssid_name,
							esp8266_runtime_hndl->ssid_pswrd);
		currentState = ESP8266_State_Setting_AP ;
		timeout = AP_CONNECT_TIMEOUT;
		break;
	case ESP8266_State_Receiving_Data:
		currentState = esp8266_runtime_hndl->returnFromDataReceiveState;
		esp8266_runtime_hndl->currentSocketNumber =
				esp8266_runtime_hndl->returnFromDataReceiveStateSocketNumber;
		break;
	case ESP8266_State_Resetting:
		start_from_begining(config_handle, esp8266_runtime_hndl);
		timeout = 10;
		break;
	case ESP8266_State_Creating_Server:
	case ESP8266_State_Setting_Connection_Type:
	case ESP8266_State_Setting_Timeout:
	case ESP8266_State_StartResetting :
	case ESP8266_State_Setting_Mode:
	case ESP8266_State_Setting_Echo_Off :
	case ESP8266_State_DisconnectWIFI_before_baud_rate_change:
	case ESP8266_State_ChangingBaudRate :
		CRITICAL_ERROR("response timeout");
		break;
	case ESP8266_State_Wait_For_Send_Ready :
	case ESP8266_State_Wait_Send_Complete :
	case ESP8266_State_Wait_For_IP :
	case ESP8266_State_Connecting_Socket :
	case ESP8266_State_Closing_Socket :
	case ESP8266_State_Wait_For_Socket_Status :
	case ESP8266_State_Wait_For_Socket_Status_Complete :
		{
			os_delay_ms(10000);//wait, maybe more data will arrive to uart
			//_do_uart_dbg_print = 0;
		}
		esp8266_runtime_hndl->lCurrError = ESP8266_ERR_REQUEST_TIMED_OUT;
		esp8266_runtime_hndl->lRequest_done=1;
		currentState = ESP8266_State_Idle;
		break ;
	default:
		break ;
	}

	DEV_IOCTL(timer_dev,IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_mSEC_AND_RESET,
			&timeout);
	esp8266_runtime_hndl->currentState = currentState;
}


static void init_esp8266(struct dev_desc_t *adev,
		struct esp8266_cfg_t *config_handle,
		struct esp8266_runtime_t *esp8266_runtime_hndl)
{
	struct esp8266_socket_t  *sockets;
	size_t i;

	esp8266_runtime_hndl->wifi_connected = 0;
	esp8266_runtime_hndl->end_of_msg_queue =
						os_create_queue( 1, sizeof(dummy_msg));
	if (NULL == esp8266_runtime_hndl->end_of_msg_queue)
	{
		CRITICAL_ERROR("cannot create queue");
	}
	esp8266_runtime_hndl->sendDataMutex = os_create_mutex();
	if (NULL == esp8266_runtime_hndl->sendDataMutex)
	{
		CRITICAL_ERROR("cannot create mutex");
	}

	sockets = esp8266_runtime_hndl->sockets;
	DEV_IOCTL_0_PARAMS(config_handle->uart_tx_dev, IOCTL_DEVICE_START );
	DEV_IOCTL_0_PARAMS(config_handle->uart_rx_dev, IOCTL_DEVICE_START );
	DEV_IOCTL_0_PARAMS(config_handle->timer_dev, IOCTL_DEVICE_START );

	for(i = 0; i < ESP8266_MAX_NUM_OF_SOCKETS; i++)
	{
		sockets[i].socket_in_use = 0;
		sockets[i].socket_number = i;
		sockets[i].recvedData = NULL ;
		sockets[i].curr_data_size = 0 ;
		sockets[i].esp8266_dev = adev;
		sockets[i].socket_options = 0;
		sockets[i].wake_queue = os_create_queue( 1, sizeof(uint32_t));
	}

	esp8266_runtime_hndl->main_queue = os_create_queue(
			ESP8266_MAX_QUEUE_LEN, sizeof(struct esp8266_message_t ));
	if (NULL == esp8266_runtime_hndl->main_queue)
	{
		CRITICAL_ERROR("cannot create main queue");
	}
	// should be the last as it is used for checking if module was initialized
	esp8266_runtime_hndl->init_done = 1;
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
	struct esp8266_cfg_t *config_handle;
	struct esp8266_runtime_t *esp8266_runtime_hndl;
	enum ESP8266_State_e currentState ;
	os_queue_t main_queue;
	struct esp8266_message_t *pendingMessage;
	uint64_t timeout;
	uint32_t interface_device_speed;

	adev = pvParameters;
	config_handle = DEV_GET_CONFIG_DATA_POINTER(ESP8266, adev);
	esp8266_runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(ESP8266, adev);

	init_esp8266(adev, config_handle, esp8266_runtime_hndl);
	main_queue = esp8266_runtime_hndl->main_queue;
	pendingMessage = &esp8266_runtime_hndl->pendingMessage;

	interface_device_speed = 115200;
	esp8266_runtime_hndl->interface_device_speed = interface_device_speed;
	esp8266_runtime_hndl->lRequest_done = 0;
	DEV_IOCTL(config_handle->uart_dev ,
			IOCTL_UART_SET_BAUD_RATE, &interface_device_speed);

	start_from_begining(config_handle, esp8266_runtime_hndl);

	timeout = 10;
	DEV_IOCTL(config_handle->timer_dev ,
			IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_mSEC_AND_RESET, &timeout);

	while (1)
	{
		size_t isMessageRecieved;
		uint8_t isMessagePending;

		isMessageRecieved = os_queue_receive_with_timeout( main_queue,
				&(xRxedMessage), 50/* portMAX_DELAY*/ ) ;
		isMessagePending = esp8266_runtime_hndl->isMessagePending;

		if( OS_QUEUE_RECEIVE_SUCCESS == isMessageRecieved )
		{
			if(DATA_FROM_UART == xRxedMessage.type)
			{
				process_message_from_hw(config_handle, esp8266_runtime_hndl);
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
			no_new_message_received(config_handle, esp8266_runtime_hndl);
		}

		currentState = esp8266_runtime_hndl->currentState;
		if(isMessagePending && (ESP8266_State_Idle == currentState))
		{
			//PRINTF_DBG("+++ process pending message\n");
			process_software_message(config_handle, esp8266_runtime_hndl);
			if (ESP8266_State_Idle != esp8266_runtime_hndl->currentState)
			{
				esp8266_runtime_hndl->lRequest_done = 2;
			}
			isMessagePending = 0;
		}

		if (1 == esp8266_runtime_hndl->lRequest_done)
		{
			esp8266_runtime_hndl->lRequest_done = 0;
			os_queue_send_without_wait(
				esp8266_runtime_hndl->end_of_msg_queue, ( void *)&dummy_msg);
		}
		esp8266_runtime_hndl->isMessagePending = isMessagePending;
		os_stack_test();
	}
}



static uint8_t ESP8266_device_start(struct dev_desc_t *adev)
{
	struct esp8266_runtime_t *esp8266_runtime_hndl;

	esp8266_runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(ESP8266, adev);

	os_create_task("ESP8266_Task", ESP8266_Task, adev,
			ESP8266_TASK_STACK_SIZE, ESP8266_TASK_PRIORITY);
	return 0;
}


static uint8_t send_socket_open_msg(
		struct esp8266_runtime_t *esp8266_runtime_hndl,
		struct ESP8266_ioctl_socket_open_t *ESP8266_ioctl_socket_open)
{
	struct esp8266_message_t  queueMsg;
	struct esp8266_socket_t **new_socket_handle;

	new_socket_handle = (struct esp8266_socket_t **)
							ESP8266_ioctl_socket_open->new_socket_descriptor;
	queueMsg.type = OPEN_SOCKET;
	queueMsg.msg_data.msg_open_socket.new_socket_handle = new_socket_handle;

	return send_message_and_wait(esp8266_runtime_hndl, &queueMsg);
}


static uint8_t send_get_ip_msg(
		struct esp8266_runtime_t *esp8266_runtime_hndl,
		struct ioctl_net_device_get_local_addr_t *p_get_local_addr)
{
	struct esp8266_message_t  queueMsg;

	queueMsg.type = GET_IP;
	queueMsg.msg_data.msg_getIP.IPstr = p_get_local_addr->addr_str;
	queueMsg.msg_data.msg_getIP.strIPLen = p_get_local_addr->addr_str_len;
	return send_message_and_wait(esp8266_runtime_hndl, &queueMsg);
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



static uint8_t get_host_addr(struct esp8266_runtime_t *esp8266_runtime_hndl,
		struct ioctl_net_device_get_host_addr_t* ioctl_net_device_get_host_addr)
{
	struct esp8266_socket_t  *socket_handle;
	struct esp8266_message_t  queueMsg;
	uint16_t port;
	uint8_t retVal;


	queueMsg.type = OPEN_SOCKET;
	queueMsg.msg_data.msg_open_socket.new_socket_handle = &socket_handle;
	retVal = send_message_and_wait(esp8266_runtime_hndl, &queueMsg);
	if (0 != retVal) return 1;

	queueMsg.type = CONNECT_SOCKET;
	queueMsg.msg_data.msg_connect_socket.socket_handle = socket_handle;
	queueMsg.msg_data.msg_connect_socket.strHostName =
							ioctl_net_device_get_host_addr->name;
	queueMsg.msg_data.msg_connect_socket.port = "80";
	retVal = send_message_and_wait(esp8266_runtime_hndl, &queueMsg);

	if (0 != retVal)
	{
		queueMsg.type = CLOSE_SOCKET;
		queueMsg.msg_data.msg_close_socket.socket_handle = socket_handle;
		retVal = send_message_and_wait(esp8266_runtime_hndl, &queueMsg);
		// don't return on error because socket may not be opened at all

		queueMsg.type = OPEN_SOCKET;
		queueMsg.msg_data.msg_open_socket.new_socket_handle = &socket_handle;
		retVal = send_message_and_wait(esp8266_runtime_hndl, &queueMsg);
		if (0 != retVal) return 1;
		queueMsg.type = CONNECT_SOCKET;
		queueMsg.msg_data.msg_connect_socket.socket_handle = socket_handle;
		queueMsg.msg_data.msg_connect_socket.strHostName =
								ioctl_net_device_get_host_addr->name;
		queueMsg.msg_data.msg_connect_socket.port = "443";
		retVal = send_message_and_wait(esp8266_runtime_hndl, &queueMsg);
		if (0 != retVal)
		{
			queueMsg.type = CLOSE_SOCKET;
			queueMsg.msg_data.msg_close_socket.socket_handle = socket_handle;
			retVal = send_message_and_wait(esp8266_runtime_hndl, &queueMsg);
			return 1;
		}
	}

	queueMsg.type = GET_OPEN_CONNECTION_STATUS;
	queueMsg.msg_data.msg_get_open_connection.socket_handle = socket_handle;
	queueMsg.msg_data.msg_get_open_connection.strIP =
							ioctl_net_device_get_host_addr->addr_str;
	queueMsg.msg_data.msg_get_open_connection.strIPLen =
							ioctl_net_device_get_host_addr->addr_str_len;
	queueMsg.msg_data.msg_get_open_connection.pPort = &port;
	retVal = send_message_and_wait(esp8266_runtime_hndl, &queueMsg);

	queueMsg.type = CLOSE_SOCKET;
	queueMsg.msg_data.msg_close_socket.socket_handle = socket_handle;
	retVal = send_message_and_wait(esp8266_runtime_hndl, &queueMsg);
	if (0 != retVal) return 1;

	return 0;
}


#ifdef CONFIG_USE_INTERNAL_SOCKETS_IMPLEMENTATION

#include "ESP8266_sockets_functions.c"

#endif



/*
 * ESP8266_ioctl()
 *
 * return:
 */
static uint8_t ESP8266_ioctl(struct dev_desc_t *adev, const uint8_t aIoctl_num,
								void * aIoctl_param1, void * aIoctl_param2)
{
	uint8_t retVal = 0;
	struct esp8266_runtime_t *esp8266_runtime_hndl;

	esp8266_runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(ESP8266, adev);
	if ((0 == esp8266_runtime_hndl->init_done) &&
		(   (IOCTL_DEVICE_START != aIoctl_num) &&
			(IOCTL_ESP8266_IS_INITIALIZED != aIoctl_num) &&
			(IOCTL_ESP8266_IS_WIFI_CONNECTED != aIoctl_num) &&
			(IOCTL_ESP8266_SET_SSID_NAME != aIoctl_num) &&
			(IOCTL_ESP8266_SET_SSID_PSWRD != aIoctl_num) ))
	{
		CRITICAL_ERROR("esp8266 not initialized yet");
	}

	switch(aIoctl_num)
	{
	case IOCTL_ESP8266_SET_SSID_NAME :
		retVal = set_string(esp8266_runtime_hndl->ssid_name, aIoctl_param1,
				ESP8266_MAX_SSID_NAME_LEN);
		break;
	case IOCTL_ESP8266_SET_SSID_PSWRD :
		retVal = set_string(esp8266_runtime_hndl->ssid_pswrd, aIoctl_param1,
				ESP8266_MAX_SSID_PSWRD_LEN);
		break;
	case IOCTL_ESP8266_SET_SSID_NAME_REDANDENCY :
		retVal = set_string(esp8266_runtime_hndl->ssid_name_redandency,
				aIoctl_param1, ESP8266_MAX_SSID_NAME_LEN);
		break;
	case IOCTL_ESP8266_SET_SSID_PSWRD_REDANDENCY :
		retVal = set_string(esp8266_runtime_hndl->ssid_pswrd_redandency,
				aIoctl_param1, ESP8266_MAX_SSID_PSWRD_LEN);
		break;
	case IOCTL_ESP8266_SOCKET_OPEN:
		retVal = send_socket_open_msg(esp8266_runtime_hndl, aIoctl_param1);
		break;
	case IOCTL_NET_DEVICE_GET_LOCAL_ADDR:
		retVal = send_get_ip_msg(esp8266_runtime_hndl, aIoctl_param1);
		break;
	case IOCTL_DEVICE_START :
		retVal = ESP8266_device_start(adev);
		break;
	case IOCTL_NET_DEVICE_GET_HOST_ADDR:
		retVal = get_host_addr(esp8266_runtime_hndl, aIoctl_param1);
		break;
#ifdef CONFIG_USE_INTERNAL_SOCKETS_IMPLEMENTATION
	case IOCTL_OPEN_SOCKET:
		retVal = open_socket(adev, aIoctl_param1);
		break;
#endif
	case IOCTL_ESP8266_IS_INITIALIZED :
		*(uint8_t*) aIoctl_param1 = esp8266_runtime_hndl->init_done;
		break;
	case IOCTL_ESP8266_IS_WIFI_CONNECTED:
		*(uint8_t*) aIoctl_param1 = esp8266_runtime_hndl->wifi_connected;
		break;
	default :
		return 1;
	}
	return retVal;
}


#define	MODULE_NAME                ESP8266
#define	MODULE_IOCTL_FUNCTION      ESP8266_ioctl
#define	MODULE_CALLBACK_FUNCTION   ESP8266_callback
#include "add_module.h"
