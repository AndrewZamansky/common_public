#include "_project.h"
#include "sys/socket.h"
//#include <sys/type.h>

#include "dev_management_api.h"
#include "ESP8266_api.h"
#include "file_descriptor_manager_api.h"
#include "std_net_functions_api.h"
//#define DEBUG
#include "PRINTF_api.h"




static int  esp8266_closesocket(void* socketfd)
{
	struct esp8266_socket_t  *socket_handle;
	struct esp8266_message_t  queueMsg;
	struct esp8266_runtime_t *esp8266_runtime_hndl;
	uint8_t retVal;

	socket_handle = (struct esp8266_socket_t  *)socketfd;
	esp8266_runtime_hndl =
			DEV_GET_RUNTIME_DATA_POINTER(ESP8266, socket_handle->esp8266_dev);

	queueMsg.type = CLOSE_SOCKET;
	queueMsg.msg_data.msg_close_socket.socket_handle = socket_handle;
	retVal = send_message_and_wait(esp8266_runtime_hndl, &queueMsg);

	if ( (0 != retVal) && (ESP8266_ERR_SOCKET_NOT_AVAILABLE != retVal))
	{
		CRITICAL_ERROR("unknown return");
	}
	return 0;
}


static int esp8266_connect(void* socketfd,
						const struct sockaddr *addr, unsigned int addrlen)
{
	struct esp8266_socket_t  *socket_handle;
	struct esp8266_message_t  queueMsg;
	struct esp8266_runtime_t *esp8266_runtime_hndl;
	struct sockaddr_in   *lp_sockaddr;
	struct  in_addr *sin_addr;
	char addr_str[32] = {0};
	char port_str[8] = {0};
	uint8_t retVal;

	socket_handle = (struct esp8266_socket_t  *)socketfd;
	esp8266_runtime_hndl =
			DEV_GET_RUNTIME_DATA_POINTER(ESP8266, socket_handle->esp8266_dev);

	lp_sockaddr = (struct sockaddr_in   *)addr;
	sin_addr = &(lp_sockaddr->sin_addr);

	snprintf((char*)addr_str, 32 ,"%d.%d.%d.%d",
			sin_addr->S_un.S_un_b.s_b1,
			sin_addr->S_un.S_un_b.s_b2,
			sin_addr->S_un.S_un_b.s_b3,
			sin_addr->S_un.S_un_b.s_b4 );
	snprintf((char*)port_str, 8 ,"%d",htons(lp_sockaddr->sin_port));

	queueMsg.type = CONNECT_SOCKET;
	queueMsg.msg_data.msg_connect_socket.socket_handle = socket_handle;
	queueMsg.msg_data.msg_connect_socket.strHostName = addr_str;
	queueMsg.msg_data.msg_connect_socket.port = port_str;
	retVal = send_message_and_wait(esp8266_runtime_hndl, &queueMsg);

	if(0 != retVal)
	{
		CRITICAL_ERROR("socket cannot connect");
	}

	return 0;
}


static uint8_t esp8266_is_data_available(void* socketfd,
											uint8_t *is_data_available)
{
	struct esp8266_socket_t  *socket_handle;
	struct esp8266_message_t  queueMsg;
	struct esp8266_runtime_t *esp8266_runtime_hndl;

	socket_handle = (struct esp8266_socket_t  *)socketfd;
	esp8266_runtime_hndl =
			DEV_GET_RUNTIME_DATA_POINTER(ESP8266, socket_handle->esp8266_dev);

	*is_data_available = 0;
	queueMsg.type = CHECK_IF_RECEIVED_DATA;
	queueMsg.msg_data.msg_check_if_new_data_received.socket_handle =
															socket_handle;
	queueMsg.msg_data.msg_check_if_new_data_received.newDataExists =
														is_data_available;
	return send_message_and_wait(esp8266_runtime_hndl, &queueMsg);

}


static ssize_t esp8266_recv(void* socketfd,
								void *buf, size_t len, int flags)
{
	struct esp8266_socket_t  *socket_handle;
	struct esp8266_message_t  queueMsg;
	struct esp8266_runtime_t *esp8266_runtime_hndl;
	uint8_t retVal;
	size_t size_received;
	uint32_t dummy_msg;

	if (0 != flags)
	{
		CRITICAL_ERROR("flags != 0 not implemented yet");
	}

	socket_handle = (struct esp8266_socket_t  *)socketfd;
	esp8266_runtime_hndl =
			DEV_GET_RUNTIME_DATA_POINTER(ESP8266, socket_handle->esp8266_dev);


	queueMsg.type = GET_RECEIVED_DATA;
	queueMsg.msg_data.msg_get_data_received.socket_handle = socket_handle;
	queueMsg.msg_data.msg_get_data_received.buffer = buf;
	queueMsg.msg_data.msg_get_data_received.max_size = len;
	queueMsg.msg_data.msg_get_data_received.size_received = &size_received;

	size_received = 0;
	while (0 == size_received)
	{
		retVal = send_message_and_wait(esp8266_runtime_hndl, &queueMsg);
		if (0 != retVal)
		{
			return -1;
		}
		if (0 == size_received) // blocking
		{

			os_queue_receive_infinite_wait(
						socket_handle->wake_queue, &dummy_msg);
		}
	}
	return size_received;
}


static void set_tcp_level_option(struct esp8266_socket_t * socket_handle,
		int optname, const void *optval, socklen_t optlen)
{
	switch(optname)
	{
		case TCP_NODELAY:
			break;
		default:
			CRITICAL_ERROR("this tcp option is not implemented yet");
	}
}


static void set_socket_level_option(
		struct esp8266_socket_t  *socket_handle,
		int optname, const void *optval, socklen_t optlen)
{
	uint32_t socket_options;
	uint32_t val;

	socket_options = socket_handle->socket_options;

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
	socket_handle->socket_options = socket_options;
}


static int esp8266_setsockopt(void* socketfd,
		int level, int optname, const void *optval, socklen_t optlen)
{
	struct esp8266_socket_t  *socket_handle;

	socket_handle = (struct esp8266_socket_t  *)socketfd;
	switch(level)
	{
	case SOL_SOCKET:
		set_socket_level_option(socket_handle, optname, optval, optlen);
		break;
	case IPPROTO_TCP:
		set_tcp_level_option(socket_handle, optname, optval, optlen);
		break;

	default:
		CRITICAL_ERROR("unknown option level");
	}

	return 0;
}


static int esp8266_bind(void* socketfd,
				const struct sockaddr *addr, socklen_t addrlen)
{
	CRITICAL_ERROR("bind_uCprojects not implemented yet");
	return 0;
}




static int esp8266_getpeername(void* socketfd,
				struct sockaddr *addr, socklen_t *addrlen)
{
	struct esp8266_socket_t  *socket_handle;
	struct esp8266_message_t  queueMsg;
	struct esp8266_runtime_t *esp8266_runtime_hndl;
	struct sockaddr_in   *lp_sockaddr;
	struct  in_addr *sin_addr;
	char *ipAddrStr;
	char ipAddr[20] = {0};
	uint8_t retVal;
	uint16_t port;

	socket_handle = (struct esp8266_socket_t  *)socketfd;
	esp8266_runtime_hndl =
			DEV_GET_RUNTIME_DATA_POINTER(ESP8266, socket_handle->esp8266_dev);

	queueMsg.type = GET_OPEN_CONNECTION_STATUS;
	queueMsg.msg_data.msg_get_open_connection.socket_handle = socket_handle;
	queueMsg.msg_data.msg_get_open_connection.strIP = ipAddr;
	queueMsg.msg_data.msg_get_open_connection.strIPLen = sizeof(ipAddr) - 1;
	queueMsg.msg_data.msg_get_open_connection.pPort = &port;
	retVal = send_message_and_wait(esp8266_runtime_hndl, &queueMsg);

	if (0 != retVal) return 1 ;

	lp_sockaddr = (struct sockaddr_in *)addr;
	sin_addr = &(lp_sockaddr->sin_addr);

	ipAddrStr = ipAddr;
	sin_addr->S_un.S_un_b.s_b1 = atoi(ipAddrStr);
	ipAddrStr = strchr(ipAddrStr,'.');
	if(NULL == ipAddrStr) return ENOBUFS;
	ipAddrStr++;
	sin_addr->S_un.S_un_b.s_b2 = atoi(ipAddrStr);
	ipAddrStr = strchr(ipAddrStr+1,'.');
	if(NULL == ipAddrStr) return ENOBUFS;
	ipAddrStr++;
	sin_addr->S_un.S_un_b.s_b3 = atoi(ipAddrStr);
	ipAddrStr = strchr(ipAddrStr+1,'.');
	if(NULL == ipAddrStr) return ENOBUFS;
	ipAddrStr++;
	sin_addr->S_un.S_un_b.s_b4 = atoi(ipAddrStr);

	lp_sockaddr->sin_port = htons(port);
	lp_sockaddr->sin_family = AF_INET;

	return 0;
}


static ssize_t esp8266_send(void* socketfd,
						const void *buffer, size_t length, int flags)
{
	struct esp8266_socket_t  *socket_handle;
	struct esp8266_message_t  queueMsg;
	struct esp8266_runtime_t *esp8266_runtime_hndl;
	uint8_t ret_val;

	if (0 == length)
	{
		return 0;
	}
	if(0 != flags)
	{
		CRITICAL_ERROR("flags != 0 not implemented yet");
	}

	socket_handle = (struct esp8266_socket_t  *)socketfd;
	esp8266_runtime_hndl =
			DEV_GET_RUNTIME_DATA_POINTER(ESP8266, socket_handle->esp8266_dev);

	queueMsg.type = SEND_DATA;
	queueMsg.msg_data.msg_send_data_to_socket.socket_handle = socket_handle;
	queueMsg.msg_data.msg_send_data_to_socket.data = buffer;
	queueMsg.msg_data.msg_send_data_to_socket.data_length = length;
	ret_val = send_message_and_wait(esp8266_runtime_hndl, &queueMsg);

	if(0 != ret_val)
	{
		return -1;
	}

	return length;
}


static int esp8266_listen(void * sockfd, int backlog)
{
	CRITICAL_ERROR("esp8266_listen not implemented yet");
	return 0;
}


static int esp8266_accept(
		void * sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	CRITICAL_ERROR("esp8266_accept not implemented yet");
	return 0;
}


#define MAX_ADDR_LEN  20

static int esp8266_getsockname(
		void * socketfd, struct sockaddr *local_addr, socklen_t *addrlen)
{
	struct esp8266_socket_t  *socket_handle;
	struct esp8266_message_t  queueMsg;
	struct esp8266_runtime_t *esp8266_runtime_hndl;
	struct sockaddr_in   *lp_sockaddr;
	struct  in_addr *sin_addr;
	char *ipAddrStr;
	char ipAddr[MAX_ADDR_LEN] = {0};
	uint8_t retVal;

	socket_handle = (struct esp8266_socket_t  *)socketfd;
	esp8266_runtime_hndl =
			DEV_GET_RUNTIME_DATA_POINTER(ESP8266, socket_handle->esp8266_dev);

	lp_sockaddr = (struct sockaddr_in *)local_addr;
	sin_addr = &(lp_sockaddr->sin_addr);

	queueMsg.type = GET_IP;
	queueMsg.msg_data.msg_getIP.IPstr = ipAddr;
	queueMsg.msg_data.msg_getIP.strIPLen = MAX_ADDR_LEN - 1;
	retVal = send_message_and_wait(esp8266_runtime_hndl, &queueMsg);
	if (0 != retVal) return 1 ;

	ipAddrStr = ipAddr;
	sin_addr->S_un.S_un_b.s_b1 = atoi(ipAddrStr);
	ipAddrStr = strchr(ipAddrStr,'.');
	if(NULL == ipAddrStr) return ENOBUFS;
	ipAddrStr++;
	sin_addr->S_un.S_un_b.s_b2 = atoi(ipAddrStr);
	ipAddrStr = strchr(ipAddrStr + 1,'.');
	if(NULL == ipAddrStr) return ENOBUFS;
	ipAddrStr++;
	sin_addr->S_un.S_un_b.s_b3 = atoi(ipAddrStr);
	ipAddrStr = strchr(ipAddrStr + 1,'.');
	if(NULL == ipAddrStr) return ENOBUFS;
	ipAddrStr++;
	sin_addr->S_un.S_un_b.s_b4 = atoi(ipAddrStr);

	lp_sockaddr->sin_port = htons(0xffff);// TODO : implement read of port
	return 0;
}

static void __fill_socket_functions(struct file_desc_ops_t* p_file_desc_ops)
{
	p_file_desc_ops->closesocket_func = esp8266_closesocket;
	p_file_desc_ops->connect_func = esp8266_connect;
	p_file_desc_ops->is_data_available_func = esp8266_is_data_available;
	p_file_desc_ops->recv_func = esp8266_recv;
	p_file_desc_ops->setsockopt_func = esp8266_setsockopt;
	p_file_desc_ops->bind_func = esp8266_bind;
	p_file_desc_ops->getpeername_func = esp8266_getpeername;
	p_file_desc_ops->send_func = esp8266_send;
	p_file_desc_ops->listen_func = esp8266_listen;
	p_file_desc_ops->accept_func = esp8266_accept;
	p_file_desc_ops->getsockname_func = esp8266_getsockname;
}


static uint8_t open_socket(struct dev_desc_t *adev,
		struct file_descriptor_manager_ioctl_socket_open_t * socket_open_s)
{
	uint8_t retVal;
	struct ESP8266_ioctl_socket_open_t ioctl_socket_open;
	struct file_desc_t*  new_socket_descriptor;

	new_socket_descriptor = socket_open_s->new_socket_descriptor;
	switch (new_socket_descriptor->socket_family)
	{
		case AF_INET:
			break;
		default:
			PRINTF_DBG("socket_family = %d\n",
					new_socket_descriptor->socket_family);
			CRITICAL_ERROR("this socket family not implemented yet");
	}

	switch (new_socket_descriptor->socket_type)
	{
		case SOCK_STREAM:
			break;
		default:
			CRITICAL_ERROR("this socket type not implemented yet");
	}

	switch (new_socket_descriptor->protocol)
	{
		case IPPROTO_IP:
		case IPPROTO_TCP:
			break;
		default:
			CRITICAL_ERROR("this protocol not implemented yet");
	}

	ioctl_socket_open.new_socket_descriptor =
								&new_socket_descriptor->internal_desc;
	retVal = DEV_IOCTL_1_PARAMS(
			adev, IOCTL_ESP8266_SOCKET_OPEN , &ioctl_socket_open);
	if (0 != retVal)
	{
		return -1;
	}
	__fill_socket_functions(&new_socket_descriptor->file_desc_ops);
	return 0;
}

