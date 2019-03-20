#include "_project.h"
#include "sys/socket.h"
//#include <sys/type.h>

#include "dev_management_api.h" // for device manager defines and typedefs
#include "ESP8266_api.h"
#include "file_descriptor_manager_api.h"
#include "std_net_functions_api.h"
//#define DEBUG
#include "PRINTF_api.h"




static int  esp8266_closesocket(void* socketfd)
{
	struct dev_desc_t * socket_dev;
	uint8_t retVal;

	socket_dev = (struct dev_desc_t *)socketfd;
	retVal = DEV_IOCTL_0_PARAMS(socket_dev , IOCTL_ESP8266_SOCKET_CLOSE);

	if ( (0 != retVal) && (ESP8266_ERR_SOCKET_NOT_AVAILABLE != retVal))
	{
		CRITICAL_ERROR("unknown return");
	}
	return 0;
}


static int esp8266_connect(void* socketfd,
						const struct sockaddr *addr, unsigned int addrlen)
{
	struct dev_desc_t * socket_dev;
	struct ESP8266_ioctl_socket_connect_t  ioctl_socket_connect;
	struct sockaddr_in   *lp_sockaddr;
	struct  in_addr *sin_addr;
	char addr_str[32] = {0};
	char port_str[8] = {0};
	uint8_t retVal;

	socket_dev = (struct dev_desc_t *)socketfd;
	lp_sockaddr = (struct sockaddr_in   *)addr;
	sin_addr = &(lp_sockaddr->sin_addr);

	snprintf((char*)addr_str, 32 ,"%d.%d.%d.%d",
			sin_addr->S_un.S_un_b.s_b1,
			sin_addr->S_un.S_un_b.s_b2,
			sin_addr->S_un.S_un_b.s_b3,
			sin_addr->S_un.S_un_b.s_b4 );
	snprintf((char*)port_str, 8 ,"%d",htons(lp_sockaddr->sin_port));

	ioctl_socket_connect.strHostName = addr_str;
	ioctl_socket_connect.strPort = port_str;
	retVal = DEV_IOCTL_1_PARAMS(socket_dev,
			IOCTL_ESP8266_SOCKET_CONNECT, &ioctl_socket_connect);

	if(0 != retVal)
	{
		CRITICAL_ERROR("socket cannot connect");
	}

	return 0;
}


static uint8_t esp8266_is_data_available(void* socketfd,
											uint8_t *is_data_available)
{
	struct dev_desc_t * socket_dev;

	socket_dev = (struct dev_desc_t *)socketfd;
	*is_data_available = 0;
	return DEV_IOCTL_1_PARAMS(socket_dev,
			IOCTL_ESP8266_SOCKET_IS_DATA_RECEIVED, is_data_available);

}


static ssize_t esp8266_recv(void* socketfd,
								void *buf, size_t len, int flags)
{
	struct dev_desc_t * socket_dev;
	size_t size_received;
	struct ESP8266_ioctl_data_received_t ESP8266_ioctl_data_received;
	uint8_t retVal;

	if (0 != flags)
	{
		CRITICAL_ERROR("flags != 0 not implemented yet");
	}

	socket_dev = (struct dev_desc_t *)socketfd;
	ESP8266_ioctl_data_received.buffer = buf;
	ESP8266_ioctl_data_received.max_size = len;
	ESP8266_ioctl_data_received.size_received = &size_received;
	retVal = DEV_IOCTL_1_PARAMS(socket_dev,
			IOCTL_ESP8266_SOCKET_GET_RECEIVED_DATA ,
			&ESP8266_ioctl_data_received);

	if (0 != retVal)
	{
		return -1;
	}

	return size_received;
}


static void set_tcp_level_option(struct dev_desc_t * socket_dev,
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


static void set_socket_level_option(struct dev_desc_t * socket_dev,
		int optname, const void *optval, socklen_t optlen)
{
	uint32_t socket_options;
	uint32_t val;

	DEV_IOCTL_1_PARAMS(socket_dev ,
			IOCTL_ESP8266_SOCKET_GET_OPTIONS, &socket_options);

	switch(optname)
	{
		case SO_ERROR:
			break;
		case SO_KEEPALIVE:
			// TODO : ESP32 has keep alive parameter for port, ESP8266 doesn't
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
	DEV_IOCTL_1_PARAMS(socket_dev,
			IOCTL_ESP8266_SOCKET_SET_OPTIONS, &socket_options);
}

static int esp8266_setsockopt(void* socketfd,
		int level, int optname, const void *optval, socklen_t optlen)
{
	struct dev_desc_t * socket_dev;

	socket_dev = (struct dev_desc_t *)socketfd;
	switch(level)
	{
	case SOL_SOCKET:
		set_socket_level_option(socket_dev, optname, optval, optlen);
		break;
	case IPPROTO_TCP:
		set_tcp_level_option(socket_dev, optname, optval, optlen);
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
	struct dev_desc_t * socket_dev;
	struct sockaddr_in   *lp_sockaddr;
	struct  in_addr *sin_addr;
	char *ipAddrStr;
	char ipAddr[20] = {0};
	uint8_t retVal;
	struct ESP8266_ioctl_get_conn_status_t	ioctl_socket_get_open_connection;
	uint16_t port;

	socket_dev = (struct dev_desc_t *)socketfd;
	lp_sockaddr = (struct sockaddr_in   *)addr;
	sin_addr = &(lp_sockaddr->sin_addr);

	ioctl_socket_get_open_connection.strIP = ipAddr;
	ioctl_socket_get_open_connection.strIPLen = sizeof(ipAddr) - 1 ;
	ioctl_socket_get_open_connection.pPort = &port;
	retVal = DEV_IOCTL_1_PARAMS(socket_dev ,
			IOCTL_ESP8266_SOCKET_GET_OPEN_CONNECTION_STATUS ,
			&ioctl_socket_get_open_connection);
	if (0 != retVal) return 1 ;

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
	struct dev_desc_t * socket_dev;
	size_t ret_length;

	if(0 != flags)
	{
		CRITICAL_ERROR("flags != 0 not implemented yet");
	}

	socket_dev = (struct dev_desc_t *)socketfd;
	ret_length = DEV_WRITE(socket_dev , buffer , length) ;
	if(0 == ret_length)
	{
		CRITICAL_ERROR(" return length on send is 0");
	}
	return ret_length;
}


static uint8_t open_socket(struct dev_desc_t *adev,
		struct file_descriptor_manager_ioctl_socket_open_t * socket_open_s)
{
	uint8_t retVal;
	struct ESP8266_ioctl_socket_open_t ioctl_socket_open;
	struct file_desc_t*  new_socket_descriptor;
	struct file_desc_ops_t* p_file_desc_ops;

	switch (socket_open_s->socket_family)
	{
		case AF_INET:
			break;
		default:
			PRINTF_DBG("socket_family = %d\n", socket_open_s->socket_family);
			CRITICAL_ERROR("this socket family not implemented yet");
	}

	switch (socket_open_s->socket_type)
	{
		case SOCK_STREAM:
			break;
		default:
			CRITICAL_ERROR("this socket type not implemented yet");
	}

	switch (socket_open_s->protocol)
	{
		case IPPROTO_IP:
		case IPPROTO_TCP:
			break;
		default:
			CRITICAL_ERROR("this protocol not implemented yet");
	}

	new_socket_descriptor = socket_open_s->new_socket_descriptor;
	ioctl_socket_open.new_socket_descriptor =
					(struct dev_desc_t **)&new_socket_descriptor->internal_desc;
	retVal = DEV_IOCTL_1_PARAMS(
			adev, IOCTL_ESP8266_SOCKET_OPEN , &ioctl_socket_open);
	if (0 != retVal)
	{
		return 1;
	}
	p_file_desc_ops = &new_socket_descriptor->file_desc_ops;
	p_file_desc_ops->closesocket_func = esp8266_closesocket;
	p_file_desc_ops->connect_func = esp8266_connect;
	p_file_desc_ops->is_data_available_func = esp8266_is_data_available;
	p_file_desc_ops->recv_func = esp8266_recv;
	p_file_desc_ops->setsockopt_func = esp8266_setsockopt;
	p_file_desc_ops->bind_func = esp8266_bind;
	p_file_desc_ops->getpeername_func = esp8266_getpeername;
	p_file_desc_ops->send_func = esp8266_send;
	return 0;
}

