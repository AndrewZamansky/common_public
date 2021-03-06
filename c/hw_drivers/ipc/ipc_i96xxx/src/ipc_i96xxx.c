/*
 *
 * file :   ipc_i96xxx.c
 *
 */

#include "_project.h"
#include "errors_api.h"

#include "dev_management_api.h"
#define DEBUG
#include "PRINTF_api.h"
#include "ipc_i96xxx_api.h"
#include "timer_wrapper_api.h"
#include "os_wrapper.h"
#include "auto_init_api.h"
#include "ipc_i96xxx.h"
#include "I96100.h"
#include "irq_api.h"


#ifndef  IPC_I96XXX_TASK_PRIORITY
	#error  "IPC_I96XXX_TASK_PRIORITY should be define in project"
#endif
#ifndef  IPC_I96XXX_TASK_STACK_SIZE
	#error  "IPC_I96XXX_TASK_STACK_SIZE should be define in project"
#endif

#ifdef CONFIG_I96XXX_M0
	#define NOTIFY_REG   (SYS->P2PNTFY0)
	#define CONFIRM_REG  (SYS->P2PSTS1)
#else
	#define NOTIFY_REG   (SYS->P2PNTFY1)
	#define CONFIRM_REG  (SYS->P2PSTS0)
#endif

#define IPC_I96XXX_TIMEOUT     100//30000//10000//3000
#define IPC_I96XXX_MAX_QUEUE_LEN   ( 3 )
#define MAX_RCVD_BUFFER_SIZE   256

#define MAX_ALLOWED_MSG_ID 0xfe
#define RSRVD_REPLY_ID     0xff

#define MAX_NUMBER_OF_PORTS  32
static struct ipc_i96xxx_socket_t *ports[MAX_NUMBER_OF_PORTS];

static uint32_t dummy_msg;
static struct ipc_i96xxx_runtime_t *l_ipc_i96xxx_runtime_hndl;

static struct ipc_i96xxx_remote_message_t l_remote_msg;

static char unknown_state[] = "unknown state XX";
char* ipc_i96xxx_get_state_name(uint16_t state)
{
	switch (state)
	{
	case IPC_I96XXX_State_InitialHandShake : return "HandShake";
	case IPC_I96XXX_State_Connecting_Socket :
		return "IPC_I96XXX_State_Connecting_Socket";
	case IPC_I96XXX_State_Sending_Data : return "IPC_I96XXX_State_Sending_Data";
	case IPC_I96XXX_State_Accept_Sent : return "IPC_I96XXX_State_Accept_Sent";
	case IPC_I96XXX_State_Closing_Socket :
		return "IPC_I96XXX_State_Closing_Socket";
	case IPC_I96XXX_State_Idle : return "Idle";
	default :
		unknown_state[14] = '0' + state / 10;
		unknown_state[15] = '0' + state % 10;
		return unknown_state;
	}
}


static void ipc_isr_callback()
{
	os_queue_t main_queue;
	struct ipc_i96xxx_message_t  queueMsg;
	uint32_t reg_val;

	main_queue = l_ipc_i96xxx_runtime_hndl->main_queue;
	reg_val = CONFIRM_REG;
	if (NULL == main_queue)
	{
		CONFIRM_REG = reg_val;
		return ;
	}

	queueMsg.type = DATA_FROM_REMOTE;
	memcpy(&queueMsg.msg_data_from_peer.msg_from_remote,
				(void*)reg_val, sizeof(struct ipc_i96xxx_remote_message_t));

	if (OS_QUEUE_SEND_SUCCESS !=
			os_queue_send_without_wait(main_queue, (void*)&queueMsg))
	{
		//PRINTF_DBG("ipc not queue sent\n");
	}

	CONFIRM_REG = reg_val;
	return;
}


#define BUSY_LOOP_TRIES  100
static void send_to_peer(struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl,
							uint8_t msg_idx,
							struct ipc_i96xxx_remote_message_t *remote_msg)
{
	uint16_t tries = BUSY_LOOP_TRIES;

	while (0 != NOTIFY_REG)
	{
		if (0 == (tries--)) break;
	}

	while (0 != NOTIFY_REG)
	{
		os_delay_ms(10);
	}
	memcpy(&l_remote_msg, remote_msg, sizeof(l_remote_msg));
	l_remote_msg.msg_id = msg_idx;

	NOTIFY_REG = (uint32_t)&l_remote_msg;
}


static void send_msg_to_peer(
		struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl,
		struct ipc_i96xxx_remote_message_t *remote_msg)
{
	uint8_t curr_sent_msg_id;
	curr_sent_msg_id = ipc_i96xxx_runtime_hndl->curr_sent_msg_id;
	curr_sent_msg_id++;
	if (MAX_ALLOWED_MSG_ID < curr_sent_msg_id) curr_sent_msg_id = 0;
	ipc_i96xxx_runtime_hndl->curr_sent_msg_id = curr_sent_msg_id;
	send_to_peer(ipc_i96xxx_runtime_hndl, curr_sent_msg_id, remote_msg);
}


static void send_simple_reply_to_peer(
		struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl, uint32_t reply)
{
	struct ipc_i96xxx_remote_message_t remote_msg;

	remote_msg.type = REMOTE_MSG_TYPE_REPLY;
	remote_msg.reply_msg.reply = reply;
	remote_msg.reply_msg.reply_to_msg_id =
			ipc_i96xxx_runtime_hndl->curr_received_msg_id;
	send_to_peer(ipc_i96xxx_runtime_hndl, RSRVD_REPLY_ID, &remote_msg);
}


static void close_socket(struct ipc_i96xxx_socket_t  *curr_socket)
{
	curr_socket->socket_in_use = 0;
	curr_socket->local_port = 0xffff;
	curr_socket->socket_state = SOCKET_STATE_IDLE;
	os_safe_free(curr_socket->recvedData);
	curr_socket->recvedData = NULL ;
	curr_socket->curr_data_size = 0;

	// release all waiting tasks
	os_queue_send_without_wait(curr_socket->wake_queue, ( void *)&dummy_msg);
}


static void process_reply_for_connect(
		struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl,
		struct ipc_i96xxx_socket_t  *curr_socket,
		struct ipc_i96xxx_remote_msg_reply_t *reply_msg)
{
	if (REPLY_OK != reply_msg->reply)
	{
		ports[curr_socket->local_port] = NULL;
		curr_socket->local_port = 0xffff;
		ipc_i96xxx_runtime_hndl->last_error = IPC_I96XXX_ERR_CANNOT_CONNECT;
	}
	else
	{
		curr_socket->socket_state = SOCKET_STATE_WAITING_FOR_ACCEPT;
	}
	ipc_i96xxx_runtime_hndl->request_done = 1;
}


static void process_reply_for_accept(
		struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl,
		struct ipc_i96xxx_socket_t  *curr_socket,
		struct ipc_i96xxx_remote_msg_reply_t *reply_msg)
{
	if (REPLY_OK != reply_msg->reply)
	{
		ports[curr_socket->local_port] = NULL;
		close_socket(curr_socket);
		curr_socket->local_port = 0xffff;
		ipc_i96xxx_runtime_hndl->last_error = IPC_I96XXX_ERR_CANNOT_CONNECT;
	}
	else
	{
		curr_socket->socket_state = SOCKET_STATE_CONNECTED;
	}
	ipc_i96xxx_runtime_hndl->request_done = 1;
}


static void process_reply_for_send(
		struct ipc_i96xxx_runtime_t *ipc_runtime_hndl,
		struct ipc_i96xxx_remote_msg_reply_t *reply_msg)
{
	struct ipc_i96xxx_msg_send_data_to_socket_t *send_msg;
	send_msg = &ipc_runtime_hndl->pending_local_msg.msg_send_data_to_socket;
	if (REPLY_OK != reply_msg->reply)
	{
		ipc_runtime_hndl->last_error = IPC_I96XXX_ERR_CANNOT_SEND;
		*send_msg->data_length_sent = 0;
	}
	else
	{
		*send_msg->data_length_sent = reply_msg->send_reply.accepted_data_size;
	}
	ipc_runtime_hndl->request_done = 1;
}


static void process_reply_from_remote(
		struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl,
		struct ipc_i96xxx_remote_msg_reply_t *reply_msg)
{
	enum ipc_i96xxx_state_e current_state;
	enum ipc_i96xxx_state_e newState;
	struct ipc_i96xxx_socket_t  *curr_socket;

	current_state = ipc_i96xxx_runtime_hndl->current_state;
	if (IPC_I96XXX_State_Idle == current_state)
	{
		PRINTF_DBG("IPC err: reply for timed-out message\n");
		return;
	}
	if (ipc_i96xxx_runtime_hndl->curr_sent_msg_id != reply_msg->reply_to_msg_id)
	{
		PRINTF_DBG(
			"IPC err: reply with wrong message ID. sent_id=%d, rcvd_id=%d\n",
			ipc_i96xxx_runtime_hndl->curr_sent_msg_id,
			reply_msg->reply_to_msg_id);
		return;
	}

	curr_socket = ipc_i96xxx_runtime_hndl->curr_socket;
	newState = IPC_I96XXX_State_Idle;// by default go to Idle
	switch (current_state)
	{
	case IPC_I96XXX_State_InitialHandShake:
		if (REPLY_OK != reply_msg->reply)
		{
			PRINTF_DBG("IPC: unexpected NOT OK reply on handshake\n");
		}
		break;
	case IPC_I96XXX_State_Connecting_Socket:
		process_reply_for_connect(
				ipc_i96xxx_runtime_hndl, curr_socket, reply_msg);
		break;
	case IPC_I96XXX_State_Accept_Sent:
		process_reply_for_accept(
				ipc_i96xxx_runtime_hndl, curr_socket, reply_msg);
		break;
	case IPC_I96XXX_State_Sending_Data:
		process_reply_for_send(ipc_i96xxx_runtime_hndl, reply_msg);
		break;
	case IPC_I96XXX_State_Closing_Socket:
		close_socket(curr_socket);
		ipc_i96xxx_runtime_hndl->request_done = 1;
		break;
	default :
		PRINTF_DBG("IPC: reply during unexpected state(%d)\n", current_state);
	}
	ipc_i96xxx_runtime_hndl->current_state = newState;
}


static void process_connect_from_remote(
		struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl,
		struct ipc_i96xxx_remote_msg_connect_t *connect_msg)
{
	struct ipc_i96xxx_socket_t *socket_handle;

	socket_handle = ports[connect_msg->to_port];

	if ((NULL == socket_handle) ||
			(SOCKET_STATE_LISTENING != socket_handle->socket_state))
	{
		send_simple_reply_to_peer(
				ipc_i96xxx_runtime_hndl, REPLY_PORT_NOT_LISTENING);
		return;
	}
	socket_handle->socket_state = SOCKET_STATE_CONNECTION_PENDING;
	socket_handle->remote_port = connect_msg->from_port;
	os_queue_send_without_wait(socket_handle->wake_queue, ( void *)&dummy_msg);
	send_simple_reply_to_peer(ipc_i96xxx_runtime_hndl, REPLY_OK);
}


static void process_accept_from_remote(
		struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl,
		struct ipc_i96xxx_remote_msg_accept_t *accept_msg)
{
	struct ipc_i96xxx_socket_t *socket_handle;

	socket_handle = ports[accept_msg->to_port];

	if ((NULL == socket_handle) ||
			(SOCKET_STATE_WAITING_FOR_ACCEPT != socket_handle->socket_state))
	{
		send_simple_reply_to_peer(
				ipc_i96xxx_runtime_hndl, REPLY_PORT_NOT_WAITING_FOR_ACCEPT);
		return;
	}
	socket_handle->socket_state = SOCKET_STATE_CONNECTED;
	socket_handle->remote_port = accept_msg->from_port;
	os_queue_send_without_wait(socket_handle->wake_queue, ( void *)&dummy_msg);
	send_simple_reply_to_peer(ipc_i96xxx_runtime_hndl, REPLY_OK);
}


static void process_send_data_from_remote(
		struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl,
		struct ipc_i96xxx_remote_msg_send_data_t *send_msg)
{
	struct ipc_i96xxx_socket_t *socket_handle;
	size_t size_to_copy;
	size_t curr_data_size;
	size_t size_left_in_buffer;
	struct ipc_i96xxx_remote_message_t remote_msg;

	socket_handle = ports[send_msg->to_port];

	if ((NULL == socket_handle) ||
			(SOCKET_STATE_CONNECTED != socket_handle->socket_state))
	{
		send_simple_reply_to_peer(
				ipc_i96xxx_runtime_hndl, REPLY_PORT_NOT_CONNECTED);
		return;
	}

	size_to_copy = send_msg->data_size;
	curr_data_size = socket_handle->curr_data_size;
	size_left_in_buffer = MAX_RCVD_BUFFER_SIZE - curr_data_size;
	if (size_left_in_buffer < size_to_copy)
	{
		size_to_copy = size_left_in_buffer;
	}

	memcpy(&socket_handle->recvedData[curr_data_size],
							send_msg->data , size_to_copy);
	curr_data_size += size_to_copy;
	socket_handle->curr_data_size = curr_data_size;

	if (0 != curr_data_size)
	{
		os_queue_send_without_wait(
				socket_handle->wake_queue, ( void *)&dummy_msg);
	}
	remote_msg.type = REMOTE_MSG_TYPE_REPLY;
	remote_msg.reply_msg.reply = REPLY_OK;
	remote_msg.reply_msg.reply_to_msg_id =
				ipc_i96xxx_runtime_hndl->curr_received_msg_id;
	remote_msg.reply_msg.send_reply.accepted_data_size = size_to_copy;
	send_to_peer(ipc_i96xxx_runtime_hndl, RSRVD_REPLY_ID, &remote_msg);
}


static void process_close_connection_from_remote(
		struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl,
		struct ipc_i96xxx_remote_msg_close_connection_t *close_msg)
{
	struct ipc_i96xxx_socket_t *socket_handle;

	socket_handle = ports[close_msg->to_port];

	if ((NULL == socket_handle) ||
			(SOCKET_STATE_CONNECTED != socket_handle->socket_state))
	{
		send_simple_reply_to_peer(
				ipc_i96xxx_runtime_hndl, REPLY_PORT_NOT_CONNECTED);
		return;
	}

	close_socket(socket_handle);
	send_simple_reply_to_peer(ipc_i96xxx_runtime_hndl, REPLY_OK);
	return;
}


/*
 * process_message_from_remote()
 *
 * return:
 */
static void process_message_from_remote(struct ipc_i96xxx_cfg_t *config_handle,
		struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl,
		struct ipc_i96xxx_msg_data_from_peer_t  *msg_data_from_peer)
{
	struct ipc_i96xxx_remote_message_t *msg_from_remote;
	uint8_t msg_id;
	uint8_t expected_msg_id;

	msg_from_remote = &msg_data_from_peer->msg_from_remote;
	expected_msg_id = ipc_i96xxx_runtime_hndl->curr_received_msg_id + 1;
	if (MAX_ALLOWED_MSG_ID < expected_msg_id) expected_msg_id = 0;
	msg_id =  msg_from_remote->msg_id;
	if (RSRVD_REPLY_ID != msg_id)
	{
		if (expected_msg_id != msg_id)
		{
			PRINTF_DBG(
					"IPC wrn: possible loss of msg."
					" rcv_id = %d, expected_id = %d",
					msg_id, expected_msg_id);
		}
		ipc_i96xxx_runtime_hndl->curr_received_msg_id = msg_id;
	}

	switch(msg_from_remote->type)
	{
	case REMOTE_MSG_TYPE_REPLY:
		process_reply_from_remote(
				ipc_i96xxx_runtime_hndl, &msg_from_remote->reply_msg);
		break;
	case REMOTE_MSG_TYPE_HANDSHAKE:
		send_simple_reply_to_peer(ipc_i96xxx_runtime_hndl, REPLY_OK);
		break;
	case REMOTE_MSG_TYPE_CONNECT:
		process_connect_from_remote(ipc_i96xxx_runtime_hndl,
				&msg_from_remote->connect_msg);
		break;
	case REMOTE_MSG_TYPE_ACCEPT:
		process_accept_from_remote(ipc_i96xxx_runtime_hndl,
				&msg_from_remote->accept_msg);
		break;
	case REMOTE_MSG_TYPE_SEND_DATA:
		process_send_data_from_remote(ipc_i96xxx_runtime_hndl,
				&msg_from_remote->send_msg);
		break;
	case REMOTE_MSG_TYPE_CLOSE_CONNECTION:
		process_close_connection_from_remote(ipc_i96xxx_runtime_hndl,
						&msg_from_remote->close_msg);
		break;
	default:
		CRITICAL_ERROR("unknown message");
	}

}




static enum ipc_i96xxx_state_e process_close_socket_message(
		struct ipc_i96xxx_msg_close_socket_t  *pmsg_close_socket,
		struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl)
{
	struct ipc_i96xxx_socket_t *socket_handle;
	struct ipc_i96xxx_remote_message_t remote_msg;

	socket_handle = pmsg_close_socket->socket_handle;

	if (0 == socket_handle->socket_in_use)
	{
		ipc_i96xxx_runtime_hndl->last_error =
								IPC_I96XXX_ERR_SOCKET_NOT_AVAILABLE;
		ipc_i96xxx_runtime_hndl->request_done = 1;
		return IPC_I96XXX_State_Idle;
	}

	if (SOCKET_STATE_CONNECTED != socket_handle->socket_state)
	{
		close_socket(socket_handle);
		ipc_i96xxx_runtime_hndl->request_done = 1;
		return IPC_I96XXX_State_Idle;
	}

	remote_msg.type = REMOTE_MSG_TYPE_CLOSE_CONNECTION;
	remote_msg.close_msg.to_port = socket_handle->remote_port;
	send_msg_to_peer(ipc_i96xxx_runtime_hndl, &remote_msg);
	ipc_i96xxx_runtime_hndl->curr_socket = socket_handle;
	return IPC_I96XXX_State_Closing_Socket;
}


static enum ipc_i96xxx_state_e process_send_data_message(
		struct ipc_i96xxx_msg_send_data_to_socket_t *pmsg_send_data_to_socket,
		struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl)
{
	struct ipc_i96xxx_socket_t *socket_handle;
	struct ipc_i96xxx_remote_message_t remote_msg;

	socket_handle = pmsg_send_data_to_socket->socket_handle;

	if (SOCKET_STATE_CONNECTED != socket_handle->socket_state)
	{
		ipc_i96xxx_runtime_hndl->last_error =
				IPC_I96XXX_ERR_SOCKET_NOT_CONNECTED;
		ipc_i96xxx_runtime_hndl->request_done = 1;
		return IPC_I96XXX_State_Idle;
	}

	remote_msg.type = REMOTE_MSG_TYPE_SEND_DATA;
	remote_msg.send_msg.to_port = socket_handle->remote_port;
	remote_msg.send_msg.data = pmsg_send_data_to_socket->data;
	remote_msg.send_msg.data_size =
			pmsg_send_data_to_socket->data_length;
	send_msg_to_peer(ipc_i96xxx_runtime_hndl, &remote_msg);
	ipc_i96xxx_runtime_hndl->curr_socket = socket_handle;
	return IPC_I96XXX_State_Sending_Data;
}


static struct ipc_i96xxx_socket_t *allocate_new_socket(
					struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl)
{
	struct ipc_i96xxx_socket_t *allocated_socket;
	size_t socket_num;
	allocated_socket = ipc_i96xxx_runtime_hndl->sockets;
	for (socket_num = 0;
				socket_num < IPC_I96XXX_MAX_NUM_OF_SOCKETS; socket_num++)
	{
		if (0 == allocated_socket->socket_in_use)
		{

			allocated_socket->socket_in_use = 1;
			allocated_socket->socket_state = SOCKET_STATE_IDLE;
			allocated_socket->local_port = 0xffff;
			allocated_socket->recvedData =
					(uint8_t*)os_safe_malloc(MAX_RCVD_BUFFER_SIZE);
			errors_api_check_if_malloc_succeed(allocated_socket->recvedData);

			allocated_socket->curr_data_size = 0;
			//cleanup message queue
			os_queue_receive_with_timeout(
					allocated_socket->wake_queue, &dummy_msg, 0);

			return allocated_socket;
		}
		allocated_socket++;
	}
	return NULL;
}


static enum ipc_i96xxx_state_e process_open_socket_message(
		struct ipc_i96xxx_msg_open_socket_t *pmsg_open_socket,
		struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl)
{
	struct ipc_i96xxx_socket_t *allocated_socket;

	allocated_socket = allocate_new_socket(ipc_i96xxx_runtime_hndl);
	*pmsg_open_socket->new_socket_handle = allocated_socket;
	if (NULL == allocated_socket)
	{
		ipc_i96xxx_runtime_hndl->last_error =
				IPC_I96XXX_ERR_NO_FREE_SOCKET_LEFT;
	}
	ipc_i96xxx_runtime_hndl->request_done = 1;
	return IPC_I96XXX_State_Idle;
}


static enum ipc_i96xxx_state_e process_bind_socket_message(
		struct ipc_i96xxx_msg_bind_socket_t *pmsg_bind_socket,
		struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl)
{
	struct ipc_i96xxx_socket_t *socket_handle;
	uint16_t local_port;
	uint16_t new_port;

	ipc_i96xxx_runtime_hndl->request_done = 1;
	new_port = pmsg_bind_socket->port;
	if (MAX_NUMBER_OF_PORTS <= new_port)
	{
		ipc_i96xxx_runtime_hndl->last_error = IPC_I96XXX_ERR_WRONG_PORT_NUMBER;
		return IPC_I96XXX_State_Idle;
	}

	socket_handle = pmsg_bind_socket->socket_handle;
	local_port = socket_handle->local_port;
	if (0xffff != local_port)
	{
		ipc_i96xxx_runtime_hndl->last_error =
				IPC_I96XXX_ERR_SOCKET_ALREADY_BIND;
		return IPC_I96XXX_State_Idle;
	}

	if (NULL != ports[new_port])
	{
		ipc_i96xxx_runtime_hndl->last_error =
				IPC_I96XXX_ERR_WRONG_PORT_IS_USED;
		return IPC_I96XXX_State_Idle;
	}

	socket_handle->local_port = new_port;
	ports[new_port] = socket_handle;
	return IPC_I96XXX_State_Idle;
}


static enum ipc_i96xxx_state_e process_listen_socket_message(
		struct ipc_i96xxx_msg_listen_socket_t *pmsg_listen_socket,
		struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl)
{
	struct ipc_i96xxx_socket_t *socket_handle;

	ipc_i96xxx_runtime_hndl->request_done = 1;
	socket_handle = pmsg_listen_socket->socket_handle;
	if (0xffff == socket_handle->local_port)
	{
		ipc_i96xxx_runtime_hndl->last_error =
				IPC_I96XXX_ERR_LISTEN_PORT_NOT_SET;
		return IPC_I96XXX_State_Idle;
	}

	socket_handle->socket_state = SOCKET_STATE_LISTENING;
	return IPC_I96XXX_State_Idle;
}


static enum ipc_i96xxx_state_e process_accept_socket_message(
		struct ipc_i96xxx_msg_accept_socket_t *pmsg_accept_socket,
		struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl)
{
	struct ipc_i96xxx_socket_t *allocated_socket;
	struct ipc_i96xxx_socket_t *socket_handle;
	struct ipc_i96xxx_remote_message_t remote_msg;
	int i;

	socket_handle = pmsg_accept_socket->socket_handle;
	if (SOCKET_STATE_CONNECTION_PENDING != socket_handle->socket_state)
	{
		ipc_i96xxx_runtime_hndl->last_error =
				IPC_I96XXX_ERR_NO_CONNECTION_PENDING;
		ipc_i96xxx_runtime_hndl->request_done = 1;
		return IPC_I96XXX_State_Idle;
	}

	if (0 == socket_handle->socket_in_use)
	{
		ipc_i96xxx_runtime_hndl->last_error =
							IPC_I96XXX_ERR_SOCKET_NOT_AVAILABLE;
		ipc_i96xxx_runtime_hndl->request_done = 1;
		return IPC_I96XXX_State_Idle;
	}
	allocated_socket = allocate_new_socket(ipc_i96xxx_runtime_hndl);
	*pmsg_accept_socket->new_socket_handle = allocated_socket;
	if (NULL == allocated_socket)
	{
		ipc_i96xxx_runtime_hndl->last_error =
				IPC_I96XXX_ERR_NO_FREE_SOCKET_LEFT;
		ipc_i96xxx_runtime_hndl->request_done = 1;
		return IPC_I96XXX_State_Idle;
	}

	remote_msg.type = REMOTE_MSG_TYPE_ACCEPT;
	remote_msg.connect_msg.to_port = socket_handle->remote_port;
	for (i = (MAX_NUMBER_OF_PORTS - 1); 0 <= i; i--)
	{
		if (NULL == ports[i])
		{
			ports[i] = allocated_socket;
			allocated_socket->local_port = i;
			allocated_socket->remote_port = socket_handle->remote_port;
			remote_msg.connect_msg.from_port = i;
			send_msg_to_peer(ipc_i96xxx_runtime_hndl, &remote_msg);
			ipc_i96xxx_runtime_hndl->curr_socket = allocated_socket;
			return IPC_I96XXX_State_Accept_Sent;
		}
	}
	close_socket(allocated_socket);
	ipc_i96xxx_runtime_hndl->last_error = IPC_I96XXX_ERR_NO_FREE_PORT_LEFT;
	ipc_i96xxx_runtime_hndl->request_done = 1;
	socket_handle->socket_state = SOCKET_STATE_IDLE;
	return IPC_I96XXX_State_Idle;
}


static enum ipc_i96xxx_state_e process_connect_socket_message(
		struct ipc_i96xxx_msg_connect_socket_t *pmsg_connect_socket,
		struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl)
{
	struct ipc_i96xxx_socket_t *socket_handle;
	struct ipc_i96xxx_remote_message_t remote_msg;
	int i;

	socket_handle = pmsg_connect_socket->socket_handle;
	if (0 == socket_handle->socket_in_use)
	{
		ipc_i96xxx_runtime_hndl->last_error =
							IPC_I96XXX_ERR_SOCKET_NOT_AVAILABLE;
		ipc_i96xxx_runtime_hndl->request_done = 1;
		return IPC_I96XXX_State_Idle;
	}

	remote_msg.type = REMOTE_MSG_TYPE_CONNECT;
	remote_msg.connect_msg.to_port = pmsg_connect_socket->port;
	for (i = (MAX_NUMBER_OF_PORTS - 1); 0 <= i; i--)
	{
		if (NULL == ports[i])
		{
			ports[i] = socket_handle;
			socket_handle->local_port = i;
			socket_handle->remote_port = pmsg_connect_socket->port;
			remote_msg.connect_msg.from_port = i;
			send_msg_to_peer(ipc_i96xxx_runtime_hndl, &remote_msg);
			ipc_i96xxx_runtime_hndl->curr_socket = socket_handle;
			return IPC_I96XXX_State_Connecting_Socket;
		}
	}
	ipc_i96xxx_runtime_hndl->last_error = IPC_I96XXX_ERR_NO_FREE_PORT_LEFT;
	ipc_i96xxx_runtime_hndl->request_done = 1;
	return IPC_I96XXX_State_Idle;
}


static enum ipc_i96xxx_state_e process_check_if_data_rcvd_message(
	struct ipc_i96xxx_msg_check_if_new_data_rcvd_t *pmsg_check_if_new_data_rcvd,
	struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl)
{
	struct ipc_i96xxx_socket_t *socket_handle;

	socket_handle = pmsg_check_if_new_data_rcvd->socket_handle;
	ipc_i96xxx_runtime_hndl->request_done = 1;
	if (SOCKET_STATE_CONNECTED != socket_handle->socket_state)
	{
		ipc_i96xxx_runtime_hndl->last_error =
				IPC_I96XXX_ERR_SOCKET_NOT_CONNECTED;
		return IPC_I96XXX_State_Idle;
	}

	*pmsg_check_if_new_data_rcvd->newDataExists =
					socket_handle->curr_data_size ? 1 : 0;
	return IPC_I96XXX_State_Idle;
}


static enum ipc_i96xxx_state_e process_get_rcvd_data_message(
		struct ipc_i96xxx_msg_get_data_received_t * pmsg_get_data_received,
		struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl)
{
	struct ipc_i96xxx_socket_t *socket_handle;
	size_t bytes_to_copy;
	size_t curr_data_size;

	socket_handle = pmsg_get_data_received->socket_handle;

	ipc_i96xxx_runtime_hndl->request_done = 1;
	if (SOCKET_STATE_CONNECTED != socket_handle->socket_state)
	{
		ipc_i96xxx_runtime_hndl->last_error =
				IPC_I96XXX_ERR_SOCKET_NOT_CONNECTED;
		return IPC_I96XXX_State_Idle;
	}

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
	if (bytes_to_copy)
	{
		os_queue_send_without_wait(
				socket_handle->wake_queue, ( void *)&dummy_msg);
	}
	return IPC_I96XXX_State_Idle;
}


/*
 * process_local_message()
 *
 * return:
 */
static void process_local_message(struct ipc_i96xxx_cfg_t *config_handle,
		struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl)
{
	struct ipc_i96xxx_message_t  *pending_local_msg;
	struct dev_desc_t *   timer_dev;
	uint64_t timeout;
	enum ipc_i96xxx_state_e current_state ;


//	PRINTF_DBG("---ESP process_message=%d \r\n" ,pending_local_msg->type);
	pending_local_msg = &ipc_i96xxx_runtime_hndl->pending_local_msg;

	timeout = IPC_I96XXX_TIMEOUT;
	switch(pending_local_msg->type)
	{
	case CLOSE_SOCKET :
		current_state = process_close_socket_message(
				&pending_local_msg->msg_close_socket,
				ipc_i96xxx_runtime_hndl);
		break;
	case SEND_DATA :
		current_state = process_send_data_message(
				&pending_local_msg->msg_send_data_to_socket,
				ipc_i96xxx_runtime_hndl);
		break;
	case OPEN_SOCKET :
		current_state = process_open_socket_message(
				&pending_local_msg->msg_open_socket,
				ipc_i96xxx_runtime_hndl);
		break;
	case CONNECT_SOCKET :
		current_state = process_connect_socket_message(
				&pending_local_msg->msg_connect_socket,
				ipc_i96xxx_runtime_hndl);
		break;
	case BIND_SOCKET :
		current_state = process_bind_socket_message(
				&pending_local_msg->msg_bind_socket,
				ipc_i96xxx_runtime_hndl);
		break;
	case LISTEN_SOCKET:
		current_state = process_listen_socket_message(
				&pending_local_msg->msg_listen_socket,
				ipc_i96xxx_runtime_hndl);
		break;
	case ACCEPT_SOCKET:
		current_state = process_accept_socket_message(
				&pending_local_msg->msg_accept_socket,
				ipc_i96xxx_runtime_hndl);
		break;
	case CHECK_IF_RECEIVED_DATA:
		current_state = process_check_if_data_rcvd_message(
				&pending_local_msg->msg_check_if_new_data_received,
				ipc_i96xxx_runtime_hndl);
		break;
	case GET_RECEIVED_DATA:
		current_state = process_get_rcvd_data_message(
				&pending_local_msg->msg_get_data_received,
				ipc_i96xxx_runtime_hndl);
		break;
	default:
		current_state = IPC_I96XXX_State_Idle;
		break;
	}
	ipc_i96xxx_runtime_hndl->current_state = current_state;
	timer_dev = config_handle->timer_dev;
	DEV_IOCTL(timer_dev,
			IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_mSEC_AND_RESET, &timeout);
}


static void send_handshake(
						struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl)
{
	struct ipc_i96xxx_remote_message_t remote_msg;

	ipc_i96xxx_runtime_hndl->current_state = IPC_I96XXX_State_InitialHandShake;
	remote_msg.type = REMOTE_MSG_TYPE_HANDSHAKE;
	send_msg_to_peer(ipc_i96xxx_runtime_hndl, &remote_msg);
}


#define SKIP_PRINTS  20
/*
 * check_timeouts()
 *
 * return:
 */
static void check_timeouts(struct ipc_i96xxx_cfg_t *config_handle,
		struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl)
{
	enum ipc_i96xxx_state_e current_state ;
	struct dev_desc_t *   timer_dev;
	uint64_t timeout;
	uint8_t is_timer_elapsed;
	uint8_t do_print;
	static uint16_t skip_print_count = 0;

	current_state = ipc_i96xxx_runtime_hndl->current_state;
	timer_dev = config_handle->timer_dev;
	DEV_IOCTL(timer_dev, IOCTL_TIMER_WRAPPER_API_CHECK_IF_COUNTDOWN_ELAPSED,
			&is_timer_elapsed );
	if (0 == is_timer_elapsed) return;

	do_print = 1;
	if (IPC_I96XXX_State_InitialHandShake == current_state)
	{
		if (0 == skip_print_count)
		{
			skip_print_count = SKIP_PRINTS;
		}
		else
		{
			do_print = 0;
		}
		skip_print_count--;
	}
	if (do_print)
	{
		PRINTF_DBG( "--icp timeout crSt=%s\r\n",
						ipc_i96xxx_get_state_name(current_state));
	}
	timeout = IPC_I96XXX_TIMEOUT;
	switch(current_state)
	{
	case IPC_I96XXX_State_InitialHandShake :
		send_handshake(ipc_i96xxx_runtime_hndl);
		break;
	case IPC_I96XXX_State_Connecting_Socket:
	case IPC_I96XXX_State_Sending_Data:
	case IPC_I96XXX_State_Accept_Sent:
	case IPC_I96XXX_State_Closing_Socket:
		ipc_i96xxx_runtime_hndl->last_error = IPC_I96XXX_ERR_REQUEST_TIMED_OUT;
		ipc_i96xxx_runtime_hndl->request_done=1;
		current_state = IPC_I96XXX_State_Idle;
		break ;
	default:
		PRINTF_DBG("IPC err: timeout during state =%d", current_state);
		break ;
	}

	DEV_IOCTL(timer_dev,IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_mSEC_AND_RESET,
			&timeout);
	ipc_i96xxx_runtime_hndl->current_state = current_state;
}


static void init_icp(struct dev_desc_t *adev,
		struct ipc_i96xxx_cfg_t *config_handle,
		struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl)
{
	struct ipc_i96xxx_socket_t  *sockets;
	size_t i;

	l_ipc_i96xxx_runtime_hndl = ipc_i96xxx_runtime_hndl;

	ipc_i96xxx_runtime_hndl->config_handle = config_handle;
	ipc_i96xxx_runtime_hndl->end_of_msg_queue =
						os_create_queue( 1, sizeof(dummy_msg));
	if (NULL == ipc_i96xxx_runtime_hndl->end_of_msg_queue)
	{
		CRITICAL_ERROR("cannot create queue");
	}
	ipc_i96xxx_runtime_hndl->send_data_mutex = os_create_mutex();
	if (NULL == ipc_i96xxx_runtime_hndl->send_data_mutex)
	{
		CRITICAL_ERROR("cannot create mutex");
	}

	sockets = ipc_i96xxx_runtime_hndl->sockets;

	for(i = 0; i < IPC_I96XXX_MAX_NUM_OF_SOCKETS; i++)
	{
		sockets[i].socket_in_use = 0;
		sockets[i].socket_state = SOCKET_STATE_IDLE;
		sockets[i].recvedData = NULL ;
		sockets[i].curr_data_size = 0 ;
		sockets[i].ipc_i96xxx_dev = adev;
		sockets[i].socket_options = 0;
		sockets[i].local_port = 0xffff;
		sockets[i].wake_queue = os_create_queue( 1, sizeof(uint32_t));
	}

#ifdef CONFIG_I96XXX_M0
	irq_register_interrupt(P2P1_IRQn, ipc_isr_callback);
	irq_set_priority(P2P1_IRQn , INTERRUPT_PRIORITY_FOR_IPC_I96XXX );
	irq_enable_interrupt(P2P1_IRQn);
#else
	#define P2P0_IRQn_HIFI3  8
	irq_register_interrupt(P2P0_IRQn_HIFI3, ipc_isr_callback);
	irq_enable_interrupt(P2P0_IRQn_HIFI3);
#endif

	// should be the last as it is used for checking if module was initialized
	ipc_i96xxx_runtime_hndl->main_queue = os_create_queue(
			IPC_I96XXX_MAX_QUEUE_LEN, sizeof(struct ipc_i96xxx_message_t ));
	if (NULL == ipc_i96xxx_runtime_hndl->main_queue)
	{
		CRITICAL_ERROR("cannot create main queue");
	}
}


/*
 * ipc_i96xxx_Task()
 *
 * return:
 */
static void ipc_i96xxx_Task( void *task_param )
{
	struct dev_desc_t *adev;
	struct ipc_i96xxx_message_t xRxedMessage;
	struct ipc_i96xxx_cfg_t *config_handle;
	struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl;
	uint8_t is_local_msg_pending;
	size_t is_message_received;
	os_queue_t main_queue;
	uint64_t timeout;

	adev = task_param;
	config_handle = DEV_GET_CONFIG_DATA_POINTER(ipc_i96xxx, adev);
	ipc_i96xxx_runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(ipc_i96xxx, adev);

	init_icp(adev, config_handle, ipc_i96xxx_runtime_hndl);
	main_queue = ipc_i96xxx_runtime_hndl->main_queue;

	ipc_i96xxx_runtime_hndl->request_done = 0;
	timeout = 50;
	DEV_IOCTL(config_handle->timer_dev,
			IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_mSEC_AND_RESET, &timeout);

	send_handshake(ipc_i96xxx_runtime_hndl);

	while (1)
	{
		is_message_received = os_queue_receive_with_timeout( main_queue,
				&(xRxedMessage), 50/* portMAX_DELAY*/ ) ;

		is_local_msg_pending = ipc_i96xxx_runtime_hndl->is_local_msg_pending;
		if( OS_QUEUE_RECEIVE_SUCCESS == is_message_received )
		{
			if(DATA_FROM_REMOTE == xRxedMessage.type)
			{
				process_message_from_remote(
						config_handle, ipc_i96xxx_runtime_hndl,
						&xRxedMessage.msg_data_from_peer);
			}
			else
			{
				memcpy(&ipc_i96xxx_runtime_hndl->pending_local_msg,
						&xRxedMessage, sizeof(struct ipc_i96xxx_message_t));
				is_local_msg_pending = 1;
			}
		}

		if (IPC_I96XXX_State_Idle == ipc_i96xxx_runtime_hndl->current_state)
		{
			if (is_local_msg_pending)
			{
				//PRINTF_DBG("+++ process pending message\n");
				process_local_message(config_handle, ipc_i96xxx_runtime_hndl);
				is_local_msg_pending = 0;
			}
		}
		else
		{
			check_timeouts(config_handle, ipc_i96xxx_runtime_hndl);
		}
		ipc_i96xxx_runtime_hndl->is_local_msg_pending = is_local_msg_pending;

		if (1 == ipc_i96xxx_runtime_hndl->request_done)
		{
			ipc_i96xxx_runtime_hndl->request_done = 0;
			os_queue_send_without_wait(
				ipc_i96xxx_runtime_hndl->end_of_msg_queue, (void*)&dummy_msg);
		}
		os_stack_test();
	}
}


static uint8_t ipc_i96xxx_device_start(struct dev_desc_t *adev,
					struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl)
{
	os_create_task("ipc_i96xxx_Task", ipc_i96xxx_Task, adev,
			IPC_I96XXX_TASK_STACK_SIZE, IPC_I96XXX_TASK_PRIORITY);
	return 0;
}


#ifdef CONFIG_USE_INTERNAL_SOCKETS_IMPLEMENTATION

#include "ipc_i96xxx_sockets_functions.c"

#endif



/*
 * ipc_i96xxx_ioctl()
 *
 * return:
 */
uint8_t ipc_i96xxx_ioctl(struct dev_desc_t *adev, const uint8_t aIoctl_num,
		void * aIoctl_param1, void * aIoctl_param2)
{
	struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl;
	uint8_t retVal = 0;

	ipc_i96xxx_runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(ipc_i96xxx, adev);

	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		retVal = ipc_i96xxx_device_start(adev, ipc_i96xxx_runtime_hndl);
		break;
	case I2S_I96XXX_IS_READY_IOCTL :
		*(uint8_t*)aIoctl_param1 = (IPC_I96XXX_State_InitialHandShake >=
				ipc_i96xxx_runtime_hndl->current_state) ? 0 : 1;
		break;
#ifdef CONFIG_USE_INTERNAL_SOCKETS_IMPLEMENTATION
	case IOCTL_OPEN_SOCKET:
		retVal = open_socket(adev, aIoctl_param1);
		break;
#endif
	default :
		return 1;
	}
	return retVal;
}


#define	MODULE_NAME                      ipc_i96xxx
#define	MODULE_IOCTL_FUNCTION            ipc_i96xxx_ioctl
#include "add_module.h"
