#include "_project.h"
#include "sys/socket.h"
//#include <sys/type.h>

#include "dev_management_api.h"
#include "ipc_i96xxx_api.h"
#include "file_descriptor_manager_api.h"
#include "std_net_functions_api.h"
//#define DEBUG
#include "PRINTF_api.h"

#include "ipc_i96xxx.h"
#include "ipc_i96xxx_msg.h"



static int  ipc_i96xxx_closesocket(void* socketfd)
{
	struct ipc_i96xxx_socket_t  *socket_cfg_handle;
	struct ipc_i96xxx_message_t  queueMsg;
	struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl;
	uint8_t retVal;

	socket_cfg_handle = (struct ipc_i96xxx_socket_t  *)socketfd;
	ipc_i96xxx_runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(
						ipc_i96xxx, socket_cfg_handle->ipc_i96xxx_dev);

	queueMsg.type = CLOSE_SOCKET;
	queueMsg.msg_data.msg_close_socket.socket_handle = socket_cfg_handle;
	retVal = send_message_and_wait(ipc_i96xxx_runtime_hndl, &queueMsg);

	if ( (0 != retVal) && (IPC_I96XXX_ERR_SOCKET_NOT_AVAILABLE != retVal))
	{
		CRITICAL_ERROR("unknown return");
	}
	return 0;
}


static int ipc_i96xxx_connect(void* socketfd,
						const struct sockaddr *addr, unsigned int addrlen)
{
	struct ipc_i96xxx_socket_t  *socket_cfg_handle;
	struct ipc_i96xxx_message_t  queueMsg;
	struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl;
	uint32_t dummy_msg;
	uint8_t retVal;

	socket_cfg_handle = (struct ipc_i96xxx_socket_t  *)socketfd;
	ipc_i96xxx_runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(
						ipc_i96xxx, socket_cfg_handle->ipc_i96xxx_dev);

	queueMsg.type = CONNECT_SOCKET;
	queueMsg.msg_data.msg_connect_socket.socket_handle = socket_cfg_handle;
	queueMsg.msg_data.msg_connect_socket.port = addr->sa_data[0];
	retVal = send_message_and_wait(ipc_i96xxx_runtime_hndl, &queueMsg);

	if(0 != retVal)
	{
		return -1;
	}

	// waiting for accept()
	os_queue_receive_infinite_wait(socket_cfg_handle->wake_queue, &dummy_msg);

	return 0;
}


static uint8_t ipc_i96xxx_is_data_available(void* socketfd,
										uint8_t *is_data_available)
{
	struct ipc_i96xxx_socket_t  *socket_cfg_handle;
	struct ipc_i96xxx_message_t  queueMsg;
	struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl;

	socket_cfg_handle = (struct ipc_i96xxx_socket_t  *)socketfd;
	ipc_i96xxx_runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(
						ipc_i96xxx, socket_cfg_handle->ipc_i96xxx_dev);

	*is_data_available = 0;

	queueMsg.type = CHECK_IF_RECEIVED_DATA;
	queueMsg.msg_data.msg_check_if_new_data_received.socket_handle =
															socket_cfg_handle;
	queueMsg.msg_data.msg_check_if_new_data_received.newDataExists =
															is_data_available;
	return send_message_and_wait(ipc_i96xxx_runtime_hndl, &queueMsg);

}


static ssize_t ipc_i96xxx_recv(void* socketfd,
								void *buf, size_t len, int flags)
{
	struct ipc_i96xxx_socket_t  *socket_cfg_handle;
	struct ipc_i96xxx_message_t  queueMsg;
	struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl;
	size_t size_received;
	uint32_t dummy_msg;
	uint8_t retVal;

	if (0 != flags)
	{
		CRITICAL_ERROR("flags != 0 not implemented yet");
	}
	socket_cfg_handle = (struct ipc_i96xxx_socket_t  *)socketfd;
	ipc_i96xxx_runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(
						ipc_i96xxx, socket_cfg_handle->ipc_i96xxx_dev);


	queueMsg.type = GET_RECEIVED_DATA;
	queueMsg.msg_data.msg_get_data_received.socket_handle = socket_cfg_handle;
	queueMsg.msg_data.msg_get_data_received.buffer = buf;
	queueMsg.msg_data.msg_get_data_received.max_size = len;
	queueMsg.msg_data.msg_get_data_received.size_received = &size_received;

	size_received = 0;
	while (size_received)
	{
		retVal = send_message_and_wait(ipc_i96xxx_runtime_hndl, &queueMsg);
		if (0 != retVal)
		{
			return -1;
		}
		if (0 == size_received) // blocking
		{
			os_queue_receive_infinite_wait(
						socket_cfg_handle->wake_queue, &dummy_msg);
		}
	}
	return size_received;
}



static void set_socket_level_option(
		struct ipc_i96xxx_socket_t  *socket_cfg_handle,
		int optname, const void *optval, socklen_t optlen)
{
	uint32_t socket_options;
	uint32_t val;

	socket_options = socket_cfg_handle->socket_options;

	switch(optname)
	{
		case SO_ERROR:
			break;
		default:
			CRITICAL_ERROR("this socket option is not implemented yet");
	}

	if (sizeof(long) == optlen)
	{
		val = (*(long *)optval) ? 1 : 0 ;
	}
	else
	{
		val = (*(int *)optval) ? 1 : 0 ;
	}

	socket_options &= ~(1 << optname);
	socket_options |= (val << optname);
	socket_cfg_handle->socket_options = socket_options;
}

static int ipc_i96xxx_setsockopt(void* socketfd,
		int level, int optname, const void *optval, socklen_t optlen)
{
	struct ipc_i96xxx_socket_t  *socket_cfg_handle;

	socket_cfg_handle = (struct ipc_i96xxx_socket_t  *)socketfd;
	switch(level)
	{
	case SOL_SOCKET:
		set_socket_level_option(socket_cfg_handle, optname, optval, optlen);
		break;
	default:
		CRITICAL_ERROR("unknown option level");
	}

	return 0;
}


static int ipc_i96xxx_bind(void* socketfd,
				const struct sockaddr *addr, socklen_t addrlen)
{
	struct ipc_i96xxx_socket_t  *socket_cfg_handle;
	struct ipc_i96xxx_message_t  queueMsg;
	struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl;
	uint8_t retVal;

	socket_cfg_handle = (struct ipc_i96xxx_socket_t  *)socketfd;
	ipc_i96xxx_runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(
						ipc_i96xxx, socket_cfg_handle->ipc_i96xxx_dev);

	queueMsg.type = BIND_SOCKET;
	queueMsg.msg_data.msg_bind_socket.socket_handle = socket_cfg_handle;
	queueMsg.msg_data.msg_connect_socket.port = addr->sa_data[0];
	retVal = send_message_and_wait(ipc_i96xxx_runtime_hndl, &queueMsg);

	if (0 != retVal)
	{
		return -1;
	}

	return 0;
}


static int ipc_i96xxx_getpeername(void* socketfd,
				struct sockaddr *addr, socklen_t *addrlen)
{
	char *sa_addr;
	sa_addr = addr->sa_data;
#ifdef CONFIG_I96XXX_M0
	memcpy(sa_addr, "HIFI3", sizeof("HIFI3"));
#else
	memcpy(sa_addr, "M0", sizeof("M0"));
#endif
	return 0;
}


static ssize_t ipc_i96xxx_send(void* socketfd,
						const void *buffer, size_t length, int flags)
{
	struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl;
	struct ipc_i96xxx_message_t  queueMsg;
	struct ipc_i96xxx_socket_t  *socket_cfg_handle;
	size_t ret_val;
	uint16_t length_sent;

	if (0 == length)
	{
		return 0;
	}
	if (0xffff < length)
	{
		length = 0xffff;
	}
	if(0 != flags)
	{
		CRITICAL_ERROR("flags != 0 not implemented yet");
	}

	socket_cfg_handle = (struct ipc_i96xxx_socket_t  *)socketfd;
	ipc_i96xxx_runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(
						ipc_i96xxx, socket_cfg_handle->ipc_i96xxx_dev);

	queueMsg.type = SEND_DATA;
	queueMsg.msg_data.msg_send_data_to_socket.socket_handle = socket_cfg_handle;
	queueMsg.msg_data.msg_send_data_to_socket.data = buffer;
	queueMsg.msg_data.msg_send_data_to_socket.data_length = length;
	queueMsg.msg_data.msg_send_data_to_socket.data_length_sent = &length_sent;

	ret_val = send_message_and_wait(ipc_i96xxx_runtime_hndl, &queueMsg);

	if(0 != ret_val)
	{
		return -1;
	}

	return length_sent;
}


static int ipc_i96xxx_listen(void* socketfd, int backlog)
{
	struct ipc_i96xxx_socket_t  *socket_cfg_handle;
	struct ipc_i96xxx_message_t  queueMsg;
	struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl;
	uint32_t dummy_msg;
	uint8_t retVal;

	if (1 != backlog)
	{
		CRITICAL_ERROR("unsupported backlog");
	}
	socket_cfg_handle = (struct ipc_i96xxx_socket_t  *)socketfd;
	ipc_i96xxx_runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(
						ipc_i96xxx, socket_cfg_handle->ipc_i96xxx_dev);

	queueMsg.type = LISTEN_SOCKET;
	queueMsg.msg_data.msg_bind_socket.socket_handle = socket_cfg_handle;
	retVal = send_message_and_wait(ipc_i96xxx_runtime_hndl, &queueMsg);

	if (0 != retVal)
	{
		return -1;
	}

	os_queue_receive_infinite_wait(socket_cfg_handle->wake_queue, &dummy_msg);

	return 0;
}


static int ipc_i96xxx_accept(
		void* socketfd, struct sockaddr *addr, socklen_t *addrlen)
{
	struct ipc_i96xxx_socket_t  *new_socket_cfg_handle;
	struct ipc_i96xxx_socket_t  *socket_cfg_handle;
	struct ipc_i96xxx_message_t  queueMsg;
	struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl;
	uint8_t retVal;

	socket_cfg_handle = (struct ipc_i96xxx_socket_t  *)socketfd;
	ipc_i96xxx_runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(
						ipc_i96xxx, socket_cfg_handle->ipc_i96xxx_dev);

	queueMsg.type = ACCEPT_SOCKET;
	queueMsg.msg_data.msg_accept_socket.socket_handle = socket_cfg_handle;
	queueMsg.msg_data.msg_accept_socket.new_socket_handle =
												&new_socket_cfg_handle;
	retVal = send_message_and_wait(ipc_i96xxx_runtime_hndl, &queueMsg);

	if (0 != retVal)
	{
		return -1;
	}
	return (int)new_socket_cfg_handle;
}


static int ipc_i96xxx_getsockname(
		void* socketfd, struct sockaddr *local_addr, socklen_t *addrlen)
{
	local_addr->sa_data[0] = 0;
	return 0;
}


static void __fill_socket_functions(struct file_desc_ops_t* p_file_desc_ops)
{
	p_file_desc_ops->closesocket_func = ipc_i96xxx_closesocket;
	p_file_desc_ops->connect_func = ipc_i96xxx_connect;
	p_file_desc_ops->is_data_available_func = ipc_i96xxx_is_data_available;
	p_file_desc_ops->recv_func = ipc_i96xxx_recv;
	p_file_desc_ops->setsockopt_func = ipc_i96xxx_setsockopt;
	p_file_desc_ops->bind_func = ipc_i96xxx_bind;
	p_file_desc_ops->getpeername_func = ipc_i96xxx_getpeername;
	p_file_desc_ops->send_func = ipc_i96xxx_send;
	p_file_desc_ops->listen_func = ipc_i96xxx_listen;
	p_file_desc_ops->accept_func = ipc_i96xxx_accept;
	p_file_desc_ops->getsockname_func = ipc_i96xxx_getsockname;
}


static uint8_t open_socket(struct dev_desc_t *adev,
		struct file_descriptor_manager_ioctl_socket_open_t * socket_open_s)
{
	struct ipc_i96xxx_runtime_t *ipc_i96xxx_runtime_hndl;
	struct file_desc_t*  new_socket_descriptor;
	struct ipc_i96xxx_message_t  queueMsg;
	struct ipc_i96xxx_socket_t *new_internal_socket_handle;
	uint8_t retVal;

	ipc_i96xxx_runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(ipc_i96xxx, adev);

	new_socket_descriptor = socket_open_s->new_socket_descriptor;
	switch (new_socket_descriptor->socket_family)
	{
		case AF_IPC:
			break;
		default:
			CRITICAL_ERROR("this socket family not implemented yet");
	}

	switch (new_socket_descriptor->socket_type)
	{
		case 0:
			break;
		default:
			CRITICAL_ERROR("this socket type not implemented yet");
	}

	switch (new_socket_descriptor->protocol)
	{
		case 0:
			break;
		default:
			CRITICAL_ERROR("this protocol not implemented yet");
	}


	queueMsg.type = OPEN_SOCKET;
	queueMsg.msg_data.msg_open_socket.new_socket_handle =
									&new_internal_socket_handle;
	retVal = send_message_and_wait(ipc_i96xxx_runtime_hndl, &queueMsg);

	if (0 != retVal)
	{
		return -1;
	}
	new_socket_descriptor->internal_desc = new_internal_socket_handle;
	__fill_socket_functions(&new_socket_descriptor->file_desc_ops);
	return 0;
}

