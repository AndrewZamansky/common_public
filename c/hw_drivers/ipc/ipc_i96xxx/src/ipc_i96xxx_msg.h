/*
 * file : IPC_I96XXX_MSG.h
 */

#ifndef _IPC_I96XXX_MSG_H
#define _IPC_I96XXX_MSG_H



enum replies_e {
	REPLY_OK,
	REPLY_PORT_NOT_LISTENING,
	REPLY_PORT_NOT_WAITING_FOR_ACCEPT,
	REPLY_PORT_CONNECTED
};

enum ipc_i96xxx_remote_message_type_e
{
	REMOTE_MSG_TYPE_HANDSHAKE = 1,
	REMOTE_MSG_TYPE_REPLY,
	REMOTE_MSG_TYPE_CONNECT,
	REMOTE_MSG_TYPE_ACCEPT,
	REMOTE_MSG_TYPE_SEND_DATA,
	REMOTE_MSG_TYPE_CLOSE_CONNECTION
};

struct ipc_i96xxx_remote_msg_reply_t {
	uint32_t reply;
	uint32_t reply_data[1];
};


struct ipc_i96xxx_remote_msg_connect_t {
	uint16_t to_port;
	uint16_t from_port;
};


struct ipc_i96xxx_remote_msg_accept_t {
	uint16_t to_port;
	uint16_t from_port;
};


struct ipc_i96xxx_remote_msg_send_data_t {
	const uint8_t *data;
	uint16_t data_size;
	uint16_t to_port;
};


struct ipc_i96xxx_remote_msg_close_connection_t {
	uint16_t to_port;
};


struct ipc_i96xxx_remote_message_t {
	enum ipc_i96xxx_remote_message_type_e type;
	union
	{
		struct ipc_i96xxx_remote_msg_reply_t  reply_data;
		struct ipc_i96xxx_remote_msg_connect_t  connect_data;
		struct ipc_i96xxx_remote_msg_accept_t  accept_data;
		struct ipc_i96xxx_remote_msg_send_data_t  send_data;
		struct ipc_i96xxx_remote_msg_close_connection_t  close_data;
	} msg_data;
};
#endif
