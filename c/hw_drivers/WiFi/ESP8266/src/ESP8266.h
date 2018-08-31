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

/***************   typedefs    *******************/


struct esp8266_cfg_t {
	struct dev_desc_t *   timer_dev;
	struct dev_desc_t *   uart_rx_dev;
	struct dev_desc_t *  uart_tx_dev;
};

typedef enum
{
	ESP8266_State_HandShake,
	ESP8266_State_Resetting,
	ESP8266_State_Setting_Echo_Off,
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
} ESP8266_State_t;


struct esp8266_socket_t {
	struct dev_desc_t* client_device;
	uint8_t   socket_number;
	uint8_t   socket_in_use;
	uint8_t*  recvedData;
	size_t   curr_data_size;
	size_t  size_of_data_left_to_receive;
	struct dev_desc_t*  esp8266_dev;
	uint32_t  socket_options;
};


typedef enum { DATA_FROM_UART,
	CLOSE_SOCKET, SEND_DATA,
	OPEN_SOCKET , CONNECT_SOCKET, GET_IP ,
	GET_OPEN_CONNECTION_STATUS , CHECK_IF_RECEIVED_DATA ,
	GET_RECEIVED_DATA
} esp8266_message_type_t;


struct esp8266_msg_data_from_uart_t {
	uint8_t dummy;
};


struct esp8266_msg_close_socket_t {
	struct dev_desc_t * socket_pdev;
};


struct esp8266_msg_getIP_t {
	char *IPstr;
	uint8_t strIPLen;
};


struct esp8266_msg_send_data_to_socket_t
{
	struct dev_desc_t * socket_pdev;
	const uint8_t *data;
	uint16_t data_length;
};


struct esp8266_msg_connect_socket_t {
	struct dev_desc_t * socket_pdev;
	const char *strHostName;
	char *port;
};


struct esp8266_msg_open_socket_t {
	struct dev_desc_t * *new_socket_pdev;
};


struct esp8266_msg_get_open_connection_t {
	struct dev_desc_t *  socket_pdev;
	char *strIP;
	uint8_t strIPLen;
	uint16_t *pPort;
};


struct esp8266_msg_check_if_new_data_rcvd_t {
	struct dev_desc_t *  socket_pdev;
	uint8_t *newDataExists;
};


struct esp8266_msg_get_data_received_t {
	struct dev_desc_t *  socket_pdev;
	uint8_t *buffer;
	size_t max_size;
	size_t *size_received;
};


struct esp8266_message_t {
	esp8266_message_type_t type;
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


struct esp8266_runtime_t{
	os_queue_t  xQueue;
	os_mutex_t  sendDataMutex;
	struct dev_desc_t  sockets_descriptors[ESP8266_MAX_NUM_OF_SOCKETS];
	struct esp8266_socket_t  sockets[ESP8266_MAX_NUM_OF_SOCKETS];
	ESP8266_State_t  returnFromDataReceiveState;
	ESP8266_State_t  currentState ;
	size_t  leftDataToReceive;
	struct esp8266_message_t  pendingMessage;
	size_t  last_tested_length;
	char  sendBuffer[ESP8266_SEND_BUFFER_LEN+1];// +1 for '\0'
	char  ssid_name[ESP8266_MAX_SSID_NAME_LEN];
	char  ssid_pswrd[ESP8266_MAX_SSID_PSWRD_LEN];
	char  ssid_name_redandency[ESP8266_MAX_SSID_NAME_LEN];
	char  ssid_pswrd_redandency[ESP8266_MAX_SSID_PSWRD_LEN];
	uint8_t  currentSocketNumber;
	uint8_t  isMessagePending;
	uint8_t  lCurrError;
	uint8_t  lRequest_done;
	uint8_t  handshake_counter;
	uint8_t  need_to_reset;
};

#endif /* */
