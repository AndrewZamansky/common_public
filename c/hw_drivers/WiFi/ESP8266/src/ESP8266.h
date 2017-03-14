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

#define ESP8266_MAX_NUM_OF_SOCKETS	4
#define ESP8266_SEND_BUFFER_LEN 128
#define ESP8266_MAX_SSID_NAME_LEN  32
#define ESP8266_MAX_SSID_PSWRD_LEN  32

/***************   typedefs    *******************/


typedef struct {
	pdev_descriptor_t   timer_dev;
	pdev_descriptor_t   uart_rx_dev;
	pdev_descriptor_t	uart_tx_dev;
} esp8266_instance_t;

typedef enum
{
	ESP8266_State_StartReset =0,
	ESP8266_State_Resetting,
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

typedef struct
{
	pdev_descriptor_t  client_device;
	uint8_t socket_number;
	uint8_t socket_in_use;
	uint8_t	*recvedData;
	size_t	curr_data_size;
	size_t	size_of_data_left_to_receive;
	pdev_descriptor_t	esp8266_dev;
} esp8266_socket_t;

typedef enum { DATA_FROM_UART,
	CLOSE_SOCKET, SEND_DATA,
	OPEN_SOCKET , CONNECT_SOCKET, GET_IP ,
	GET_OPEN_CONNECTION_STATUS , CHECK_IF_RECEIVED_DATA ,
	GET_RECEIVED_DATA
} esp8266_message_type_t;

typedef struct
{
	uint8_t dummy;
} esp8266_msg_data_from_uart_t;

typedef struct
{
	pdev_descriptor_t socket_pdev;
} esp8266_msg_close_socket_t;

typedef struct
{
	char *IPstr;
	uint8_t strIPLen;
} esp8266_msg_getIP_t;

typedef struct
{
	pdev_descriptor_t socket_pdev;
	const uint8_t *data;
	uint16_t data_length;
} esp8266_msg_send_data_to_socket_t;

typedef struct
{
	pdev_descriptor_t socket_pdev;
	const char *strHostName;
	char *port;
} esp8266_msg_connect_socket_t;

typedef struct
{
	pdev_descriptor_t *new_socket_pdev;
} esp8266_msg_open_socket_t;

typedef struct
{
	pdev_descriptor_t  socket_pdev;
	char *strIP;
	uint8_t strIPLen;
	uint16_t *pPort;
} esp8266_msg_get_open_connection_t;

typedef struct
{
	pdev_descriptor_t  socket_pdev;
	uint8_t *newDataExists;
} esp8266_msg_check_if_new_data_rcvd_t;

typedef struct
{
	pdev_descriptor_t  socket_pdev;
	uint8_t *buffer;
	size_t max_size;
	size_t *size_received;
} esp8266_msg_get_data_received_t;

typedef struct
{
	esp8266_message_type_t type;
	union
	{
		esp8266_msg_data_from_uart_t msg_data_from_uart;
		esp8266_msg_close_socket_t  msg_close_socket;
		esp8266_msg_send_data_to_socket_t msg_send_data_to_socket;
		esp8266_msg_connect_socket_t  msg_connect_socket;
		esp8266_msg_open_socket_t  msg_open_socket;
		esp8266_msg_getIP_t   msg_getIP;
		esp8266_msg_get_open_connection_t   msg_get_open_connection;
		esp8266_msg_check_if_new_data_rcvd_t	msg_check_if_new_data_received;
		esp8266_msg_get_data_received_t	msg_get_data_received;
	} msg_data;
}esp8266_message_t;

typedef struct {
	os_queue_t xQueue;
	os_mutex_t sendDataMutex;
	dev_descriptor_t  sockets_descriptors[ESP8266_MAX_NUM_OF_SOCKETS];
	esp8266_socket_t  sockets[ESP8266_MAX_NUM_OF_SOCKETS];
	ESP8266_State_t returnFromDataReceiveState;
	ESP8266_State_t currentState ;
	size_t leftDataToReceive;
	esp8266_message_t pendingMessage;
	size_t last_tested_length;
	char sendBuffer[ESP8266_SEND_BUFFER_LEN+1];// +1 for '\0'
	char ssid_name[ESP8266_MAX_SSID_NAME_LEN];
	char ssid_pswrd[ESP8266_MAX_SSID_PSWRD_LEN];
	char ssid_name_redandency[ESP8266_MAX_SSID_NAME_LEN];
	char ssid_pswrd_redandency[ESP8266_MAX_SSID_PSWRD_LEN];
	uint8_t currentSocketNumber;
	uint8_t isMessagePending;
	uint8_t lCurrError;
	uint8_t lRequest_done;
} esp8266_state_instance_t;

#endif /* */
