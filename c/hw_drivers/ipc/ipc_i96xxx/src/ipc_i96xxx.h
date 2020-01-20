/*
 * file : IPC_I96XXX.h
 */

#ifndef _IPC_I96XXX_H
#define _IPC_I96XXX_H

#include "os_wrapper.h"

#define IPC_I96XXX_MAX_NUM_OF_SOCKETS   6

enum ipc_i96xxx_message_type_e
{
	DATA_FROM_REMOTE,
	BIND_SOCKET,
	LISTEN_SOCKET,
	ACCEPT_SOCKET,
	CLOSE_SOCKET, SEND_DATA,
	OPEN_SOCKET , CONNECT_SOCKET ,
	GET_OPEN_CONNECTION_STATUS , CHECK_IF_RECEIVED_DATA ,
	GET_RECEIVED_DATA
};


struct ipc_i96xxx_socket_t {
	size_t   curr_data_size;
	struct dev_desc_t*  ipc_i96xxx_dev;
	uint32_t  socket_options;
	os_queue_t  wake_queue;
	uint16_t  local_port;
	uint16_t  remote_port;
	uint8_t   socket_in_use;
	uint8_t   socket_state;
	uint8_t*  recvedData;
};


enum ipc_i96xxx_state_e {
	IPC_I96XXX_State_InitialHandShake = 1,
	IPC_I96XXX_State_Connecting_Socket,
	IPC_I96XXX_State_Sending_Data,
	IPC_I96XXX_State_Accept_Sent,
	IPC_I96XXX_State_Closing_Socket,
	IPC_I96XXX_State_Idle,
};

enum socket_state_e {
	SOCKET_STATE_IDLE,
	SOCKET_STATE_LISTENING,
	SOCKET_STATE_WAITING_FOR_ACCEPT,
	SOCKET_STATE_CONNECTION_PENDING,
	SOCKET_STATE_CONNECTED,
};


enum __attribute__((packed)) replies_e {
	REPLY_OK,
	REPLY_PORT_NOT_LISTENING,
	REPLY_PORT_NOT_WAITING_FOR_ACCEPT,
	REPLY_PORT_CONNECTED
};

enum __attribute__((packed)) ipc_i96xxx_remote_message_type_e
{
	REMOTE_MSG_TYPE_HANDSHAKE = 1,
	REMOTE_MSG_TYPE_REPLY,
	REMOTE_MSG_TYPE_CONNECT,
	REMOTE_MSG_TYPE_ACCEPT,
	REMOTE_MSG_TYPE_SEND_DATA,
	REMOTE_MSG_TYPE_CLOSE_CONNECTION
};

struct __attribute__((packed)) ipc_i96xxx_remote_msg_reply_t {
	uint8_t reply;
	uint8_t reply_to_msg_id;
	uint32_t reply_data[1];
};


struct __attribute__((packed)) ipc_i96xxx_remote_msg_connect_t {
	uint16_t to_port;
	uint16_t from_port;
};


struct __attribute__((packed)) ipc_i96xxx_remote_msg_accept_t {
	uint16_t to_port;
	uint16_t from_port;
};


struct __attribute__((packed)) ipc_i96xxx_remote_msg_send_data_t {
	const uint8_t *data;
	uint16_t data_size;
	uint16_t to_port;
};


struct __attribute__((packed)) ipc_i96xxx_remote_msg_close_connection_t {
	uint16_t to_port;
};


struct __attribute__((packed)) ipc_i96xxx_remote_message_t {
	uint8_t msg_id;
	uint8_t type;
	union
	{
		struct ipc_i96xxx_remote_msg_reply_t  reply_data;
		struct ipc_i96xxx_remote_msg_connect_t  connect_data;
		struct ipc_i96xxx_remote_msg_accept_t  accept_data;
		struct ipc_i96xxx_remote_msg_send_data_t  send_data;
		struct ipc_i96xxx_remote_msg_close_connection_t  close_data;
	} msg_data;
};


struct ipc_i96xxx_msg_data_from_peer_t {
	struct ipc_i96xxx_remote_message_t msg_from_remote;
};


struct ipc_i96xxx_msg_close_socket_t {
	struct ipc_i96xxx_socket_t *socket_handle;
};



struct ipc_i96xxx_msg_send_data_to_socket_t
{
	struct ipc_i96xxx_socket_t *socket_handle;
	const uint8_t *data;
	uint16_t data_length;
	uint16_t *data_length_sent;
};


struct ipc_i96xxx_msg_connect_socket_t {
	struct ipc_i96xxx_socket_t *socket_handle;
	uint8_t port;
};


struct ipc_i96xxx_msg_bind_socket_t {
	struct ipc_i96xxx_socket_t *socket_handle;
	uint8_t port;
};


struct ipc_i96xxx_msg_listen_socket_t {
	struct ipc_i96xxx_socket_t *socket_handle;
};


struct ipc_i96xxx_msg_accept_socket_t {
	struct ipc_i96xxx_socket_t *socket_handle;
	struct ipc_i96xxx_socket_t * *new_socket_handle;
};


struct ipc_i96xxx_msg_open_socket_t {
	struct ipc_i96xxx_socket_t * *new_socket_handle;
};



struct ipc_i96xxx_msg_check_if_new_data_rcvd_t {
	struct ipc_i96xxx_socket_t *  socket_handle;
	uint8_t *newDataExists;
};


struct ipc_i96xxx_msg_get_data_received_t {
	struct ipc_i96xxx_socket_t *socket_handle;
	uint8_t *buffer;
	size_t max_size;
	size_t *size_received;
};


struct ipc_i96xxx_message_t {
	enum ipc_i96xxx_message_type_e type;
	union
	{
		struct ipc_i96xxx_msg_data_from_peer_t  msg_data_from_peer;
		struct ipc_i96xxx_msg_close_socket_t  msg_close_socket;
		struct ipc_i96xxx_msg_bind_socket_t  msg_bind_socket;
		struct ipc_i96xxx_msg_listen_socket_t  msg_listen_socket;
		struct ipc_i96xxx_msg_accept_socket_t  msg_accept_socket;
		struct ipc_i96xxx_msg_send_data_to_socket_t msg_send_data_to_socket;
		struct ipc_i96xxx_msg_connect_socket_t  msg_connect_socket;
		struct ipc_i96xxx_msg_open_socket_t  msg_open_socket;
		struct ipc_i96xxx_msg_check_if_new_data_rcvd_t
												msg_check_if_new_data_received;
		struct ipc_i96xxx_msg_get_data_received_t  msg_get_data_received;
	} msg_data;
};


struct ipc_i96xxx_runtime_t {
	os_queue_t  main_queue;
	os_queue_t  end_of_msg_queue;
	os_mutex_t  sendDataMutex;
	struct ipc_i96xxx_socket_t  sockets[IPC_I96XXX_MAX_NUM_OF_SOCKETS];
	struct ipc_i96xxx_socket_t  *curr_socket;
	enum ipc_i96xxx_state_e  currentState ;
	struct ipc_i96xxx_message_t  pendingMessage;
	uint8_t  isMessagePending;
	uint8_t  lCurrError;
	uint8_t  lRequest_done;
	uint8_t  handshake_counter;
	uint8_t  curr_sent_msg_id;
	uint8_t  curr_received_msg_id;
};
SET_RUNTIME_DATA_TYPE(ipc_i96xxx, struct ipc_i96xxx_runtime_t);

#endif
