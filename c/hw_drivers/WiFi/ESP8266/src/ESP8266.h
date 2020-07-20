/*
 * file : ESP8266.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _ESP8266_H
#define _ESP8266_H

#include "os_wrapper.h"

#define ESP8266_MAX_NUM_OF_SOCKETS   4
#define ESP8266_SEND_BUFFER_LEN     128
#define ESP8266_MAX_SSID_NAME_LEN    32
#define ESP8266_MAX_SSID_PSWRD_LEN  32



enum ESP8266_State_e {
	ESP8266_State_InitialHandShake,
	ESP8266_State_StartResetting,
	ESP8266_State_Resetting,
	ESP8266_State_Setting_Echo_Off,

	/* following state needed because otherwise there are problems some times
	 * after uart baud rate change. like not receiving handshakes
	 */
	ESP8266_State_DisconnectWIFI_before_baud_rate_change,

	ESP8266_State_ChangingBaudRate,
	ESP8266_State_BaudChangeHandShake,
	ESP8266_State_Setting_Mode,
	ESP8266_State_Setting_AP,
	ESP8266_State_Setting_Redundent_Ap,
	ESP8266_State_Setting_Connection_Type,
	ESP8266_State_Creating_Server,
	ESP8266_State_Setting_Timeout,
	ESP8266_State_Receiving_Data,
	ESP8266_State_Wait_For_Receiving_Data_Complete,
	ESP8266_State_Wait_For_Send_Ready,
	ESP8266_State_Wait_Send_Complete,
	ESP8266_State_Wait_For_IP,
	ESP8266_State_Wait_For_IP_Complete,
	ESP8266_State_Connecting_Socket,
	ESP8266_State_Closing_Socket,
	ESP8266_State_Wait_For_Socket_Status,
	ESP8266_State_Wait_For_Socket_Status_Complete,
	ESP8266_State_Idle,
};


struct esp8266_socket_t {
	os_queue_t  wake_queue;
	uint8_t   socket_number;
	uint8_t   socket_in_use;
	uint8_t*  recvedData;
	size_t   curr_data_size;
	size_t  size_of_data_left_to_receive;
	struct dev_desc_t*  esp8266_dev;
	uint32_t  socket_options;
};


enum esp8266_message_type_e {
	DATA_FROM_UART,
	CLOSE_SOCKET, SEND_DATA,
	OPEN_SOCKET , CONNECT_SOCKET, GET_IP ,
	GET_OPEN_CONNECTION_STATUS , CHECK_IF_RECEIVED_DATA ,
	GET_RECEIVED_DATA
};


struct esp8266_msg_data_from_uart_t {
	uint8_t dummy;
};


struct esp8266_msg_close_socket_t {
	struct esp8266_socket_t *socket_handle;
};


struct esp8266_msg_getIP_t {
	char *IPstr;
	uint8_t strIPLen;
};


struct esp8266_msg_send_data_to_socket_t
{
	struct esp8266_socket_t *socket_handle;
	const uint8_t *data;
	uint16_t data_length;
};


struct esp8266_msg_connect_socket_t {
	struct esp8266_socket_t *socket_handle;
	const char *strHostName;
	char *port;
};


struct esp8266_msg_open_socket_t {
	struct esp8266_socket_t ** new_socket_handle;
};


struct esp8266_msg_get_open_connection_t {
	struct esp8266_socket_t *socket_handle;
	char *strIP;
	uint8_t strIPLen;
	uint16_t *pPort;
};


struct esp8266_msg_check_if_new_data_rcvd_t {
	struct esp8266_socket_t *socket_handle;
	uint8_t *newDataExists;
};


struct esp8266_msg_get_data_received_t {
	struct esp8266_socket_t *socket_handle;
	uint8_t *buffer;
	size_t max_size;
	size_t *size_received;
};


struct esp8266_message_t {
	enum esp8266_message_type_e type;
	union
	{
		struct esp8266_msg_data_from_uart_t  msg_data_from_uart;
		struct esp8266_msg_close_socket_t  msg_close_socket;
		struct esp8266_msg_send_data_to_socket_t msg_send_data_to_socket;
		struct esp8266_msg_connect_socket_t  msg_connect_socket;
		struct esp8266_msg_open_socket_t  msg_open_socket;
		struct esp8266_msg_getIP_t   msg_getIP;
		struct esp8266_msg_get_open_connection_t   msg_get_open_connection;
		struct esp8266_msg_check_if_new_data_rcvd_t  msg_check_if_new_data_received;
		struct esp8266_msg_get_data_received_t  msg_get_data_received;
	} msg_data;
};


struct esp8266_runtime_t {
	os_queue_t  main_queue;
	os_queue_t  end_of_msg_queue;
	os_mutex_t  sendDataMutex;
	struct esp8266_socket_t  sockets[ESP8266_MAX_NUM_OF_SOCKETS];
	enum ESP8266_State_e  returnFromDataReceiveState;
	enum ESP8266_State_e  currentState ;
	size_t  leftDataToReceive;
	struct esp8266_message_t  pendingMessage;
	size_t  last_tested_length;
	size_t  last_bytes_consumed;
	char  sendBuffer[ESP8266_SEND_BUFFER_LEN+1];// +1 for '\0'
	char  ssid_name[ESP8266_MAX_SSID_NAME_LEN];
	char  ssid_pswrd[ESP8266_MAX_SSID_PSWRD_LEN];
	char  ssid_name_redandency[ESP8266_MAX_SSID_NAME_LEN];
	char  ssid_pswrd_redandency[ESP8266_MAX_SSID_PSWRD_LEN];
	uint8_t  returnFromDataReceiveStateSocketNumber;
	uint8_t  currentSocketNumber;
	uint8_t  isMessagePending;
	uint8_t  lCurrError;
	uint8_t  lRequest_done;
	uint8_t  handshake_counter;
	uint8_t  need_to_reset;
	uint32_t  interface_device_speed;
	uint8_t init_done;
	uint8_t wifi_connected;
};
SET_RUNTIME_DATA_TYPE(ESP8266, struct esp8266_runtime_t);

#endif
