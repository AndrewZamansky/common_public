#include "_project.h"
#include "sys/socket.h"
//#include <sys/type.h>

#include "dev_management_api.h" // for device manager defines and typedefs
#include "ESP8266_api.h" // for device manager defines and typedefs
#include "errno.h"
#include "os_wrapper.h"
//#define DEBUG
#include "PRINTF_api.h"

static struct dev_desc_t * esp8266_dev = NULL;

#define MAX_NUM_OF_SOCKETS	4

#define  SOCKET_OFFSET   0//0x100
#define  MAX_SOCKET_FD   (SOCKET_OFFSET + MAX_NUM_OF_SOCKETS)
/**
 * Convert an uint16_t from host- to network byte order.
 *
 * @param n uint16_t in host byte order
 * @return n in network byte order
 */
uint16_t htons(uint16_t n)
{
  return ((n & 0xff) << 8) | ((n & 0xff00) >> 8);
}

/**
 * Convert an uint16_t from network- to host byte order.
 *
 * @param n uint16_t in network byte order
 * @return n in host byte order
 */
uint16_t ntohs(uint16_t n)
{
  return htons(n);
}

/**
 * Convert an uint32_t from host- to network byte order.
 *
 * @param n uint32_t in host byte order
 * @return n in network byte order
 */
uint32_t htonl(uint32_t n)
{
  return ((n & 0xff) << 24) |
    ((n & 0xff00) << 8) |
    ((n & 0xff0000UL) >> 8) |
    ((n & 0xff000000UL) >> 24);
}

/**
 * Convert an uint32_t from network- to host byte order.
 *
 * @param n uint32_t in network byte order
 * @return n in host byte order
 */
uint32_t ntohl(uint32_t n)
{
  return htonl(n);
}

void set_esp8266_pdev_for_socket_api(struct dev_desc_t *a_esp8266_dev)
{
	esp8266_dev = a_esp8266_dev;
}

static int calculate_socket_fd(int fd)
{
	if ((SOCKET_OFFSET > fd) || (MAX_SOCKET_FD <= fd))
	{
		PRINTF_DBG("%s :  fd = %d\n",__FUNCTION__, fd);
		CRITICAL_ERROR("bad socket fd");
	}
	return fd - SOCKET_OFFSET;
}


struct dev_desc_t *  allocated_socket_dev[MAX_NUM_OF_SOCKETS]={0};

int socket_uCprojects(int socket_family, int socket_type, int protocol)
{
	uint8_t i;
	uint8_t retVal;

	switch (socket_family)
	{
		case AF_INET:
			break;
		default:
			PRINTF_DBG("socket_family = %d\n", socket_family);
			CRITICAL_ERROR("this socket family not implemented yet");
	}

	switch (socket_type)
	{
		case SOCK_STREAM:
			break;
		default:
			CRITICAL_ERROR("this socket type not implemented yet");
	}

	switch (protocol)
	{
		case IPPROTO_IP:
		case IPPROTO_TCP:
			break;
		default:
			CRITICAL_ERROR("this protocol not implemented yet");
	}

	for (i = 0; i < MAX_NUM_OF_SOCKETS; i++)
	{
		if (NULL == allocated_socket_dev[i])
		{
			struct ESP8266_ioctl_socket_open_t ioctl_socket_open;
			ioctl_socket_open.new_socket_descriptor = &allocated_socket_dev[i];
			retVal = DEV_IOCTL_1_PARAMS(esp8266_dev ,
					IOCTL_ESP8266_SOCKET_OPEN , &ioctl_socket_open);
			if (0 == retVal)
			{
				PRINTF_DBG("%s : open socket fd = %d\n",__FUNCTION__, i);
				return i + SOCKET_OFFSET;
			}
			else
			{
				return -1;
			}
		}
	}
	return -1;
}


int closesocket_uCprojects(int sockfd)
{
	struct dev_desc_t *  socket_dev;
	uint8_t retVal;

	sockfd = calculate_socket_fd(sockfd);

	socket_dev = allocated_socket_dev[sockfd];
	retVal = DEV_IOCTL_0_PARAMS(socket_dev , IOCTL_ESP8266_SOCKET_CLOSE);

	if ( (0 != retVal) &&
			(ESP8266_ERR_SOCKET_NOT_AVAILABLE != retVal))
	{
		CRITICAL_ERROR("unknown return");
	}
	return 0;
}




int connect_uCprojects(
		int sockfd, const struct sockaddr *addr, unsigned int addrlen)
{
	struct dev_desc_t *  socket_dev;
	struct ESP8266_ioctl_socket_connect_t	ioctl_socket_connect;
	struct sockaddr_in   *lp_sockaddr;
	struct  in_addr *sin_addr;
	char addr_str[32]={0};
	char port_str[8]={0};
	uint8_t retVal;

	sockfd = calculate_socket_fd(sockfd);

	lp_sockaddr = (struct sockaddr_in   *)addr;
	sin_addr = &(lp_sockaddr->sin_addr);

	PRINTF_DBG("%s :  fd = %d\n",__FUNCTION__, sockfd);

	snprintf((char*)addr_str, 32 ,"%d.%d.%d.%d",
			sin_addr->S_un.S_un_b.s_b1,
			sin_addr->S_un.S_un_b.s_b2,
			sin_addr->S_un.S_un_b.s_b3,
			sin_addr->S_un.S_un_b.s_b4 );
	snprintf((char*)port_str, 8 ,"%d",htons(lp_sockaddr->sin_port));

	socket_dev = allocated_socket_dev[sockfd];
	ioctl_socket_connect.strHostName = addr_str;
	ioctl_socket_connect.strPort = port_str;
	retVal = DEV_IOCTL_1_PARAMS(socket_dev , IOCTL_ESP8266_SOCKET_CONNECT
			, &ioctl_socket_connect);

	if(0 != retVal)
	{
		CRITICAL_ERROR("socket cannot connect");
	}

	return 0;
}


size_t recv_uCprojects(int sockfd, void *buf, size_t len, int flags)
{
	struct dev_desc_t *  socket_dev;
	size_t size_received;
	struct ESP8266_ioctl_data_received_t ESP8266_ioctl_data_received;
	uint8_t retVal;
	uint32_t socket_options;
	uint8_t read_ready;

	sockfd = calculate_socket_fd(sockfd);
	PRINTF_DBG("%s :  fd = %d\n",__FUNCTION__, sockfd);

	if (0 != flags)
	{
		CRITICAL_ERROR("flags != 0 not implemented yet");
	}

	socket_dev = allocated_socket_dev[sockfd];

	DEV_IOCTL_1_PARAMS(socket_dev ,
				IOCTL_ESP8266_SOCKET_GET_OPTIONS, &socket_options);

	while (1)
	{
		retVal = DEV_IOCTL_1_PARAMS(socket_dev ,
				IOCTL_ESP8266_SOCKET_IS_DATA_RECEIVED , &read_ready);
		if(0 != retVal)
		{
			//printf("--%s : fd = %d connection  lost\n",__FUNCTION__, sockfd);
			errno = ENOTCONN;
			return -1;
		}
		if (read_ready)
		{
			break;
		}
		if (socket_options & (1 << SO_NONBLOCK))
		{
			errno = EAGAIN;
			return -1;
		}
		os_delay_ms( 1 );
	}

	ESP8266_ioctl_data_received.buffer = buf;
	ESP8266_ioctl_data_received.max_size = len;
	ESP8266_ioctl_data_received.size_received = &size_received;
	retVal = DEV_IOCTL_1_PARAMS(socket_dev ,
			IOCTL_ESP8266_SOCKET_GET_RECEIVED_DATA ,
			&ESP8266_ioctl_data_received);


	if (0 != retVal)
	{
		//printf("--%s : fd = %d connection was lost\n",__FUNCTION__, sockfd);
		errno = ENOTCONN;
		return -1;
	}


//	static volatile int xxx=0;
//	xxx++;
//	if(2 == xxx)
//	{
//		xxx++;
//	}
	return size_received;
}

int getsockopt_uCprojects(int sockfd, int level, int optname,
                      void *optval, socklen_t *optlen)
{
	struct dev_desc_t *  socket_dev;
	uint32_t socket_options;
	int  int_val;

	sockfd = calculate_socket_fd(sockfd);
	PRINTF_DBG("%s :  fd = %d\n",__FUNCTION__, sockfd);

	socket_dev = allocated_socket_dev[sockfd];
	DEV_IOCTL_1_PARAMS(socket_dev ,
			IOCTL_ESP8266_SOCKET_GET_OPTIONS, &socket_options);

	if (SOL_SOCKET != level)
	{
		CRITICAL_ERROR("unknown option level");
	}
	if (sizeof(int) > *optlen)
	{
		CRITICAL_ERROR("bad option size");
	}

	switch(optname)
	{
		case SO_ERROR:
			break;
		case SO_NONBLOCK:
			break;

		default:
			CRITICAL_ERROR("this option not implemented yet");
	}

	int_val = (socket_options & ( 1 << optname)) ? 1 : 0;
	if (sizeof(long) == *optlen)
	{
		*(long*)optval = int_val;
	}
	else
	{
		*(int*)optval = int_val;
	}
	return 0;
}


static void set_tcp_level_option(int sockfd,
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


static void set_socket_level_option(int sockfd,
		int optname, const void *optval, socklen_t optlen)
{
	struct dev_desc_t *  socket_dev;
	uint32_t socket_options;
	uint32_t val;


	socket_dev = allocated_socket_dev[sockfd];
	DEV_IOCTL_1_PARAMS(socket_dev ,
			IOCTL_ESP8266_SOCKET_GET_OPTIONS, &socket_options);

	switch(optname)
	{
		case SO_ERROR:
			break;
		case SO_NONBLOCK:
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
	DEV_IOCTL_1_PARAMS(socket_dev ,
			IOCTL_ESP8266_SOCKET_SET_OPTIONS, &socket_options);

}

int setsockopt_uCprojects(int sockfd, int level,
		int optname, const void *optval, socklen_t optlen)
{

	sockfd = calculate_socket_fd(sockfd);
	PRINTF_DBG("%s :  fd = %d, optname = %d , *(int*)optval = %d\n",
							__FUNCTION__, sockfd, optname, *(int*)optval);

	switch(level)
	{
	case SOL_SOCKET:
		set_socket_level_option(sockfd, optname, optval, optlen);
		break;
	case IPPROTO_TCP:
		set_tcp_level_option(sockfd, optname, optval, optlen);
		break;

	default:
		CRITICAL_ERROR("unknown option level");
	}

	return 0;
}


int bind_uCprojects(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	sockfd = calculate_socket_fd(sockfd);
	PRINTF_DBG("%s :  fd = %d\n", __FUNCTION__, sockfd);
	CRITICAL_ERROR("bind_uCprojects not implemented yet");
	return 0;
}


int getsockname_uCprojects(
		int sockfd, struct sockaddr *local_addr, socklen_t *addrlen)
{
	struct sockaddr_in   *lp_sockaddr;
	struct  in_addr *sin_addr;
	char *ipAddrStr;
	char ipAddr[20] = {0};
	uint8_t retVal;
	struct ESP8266_ioctl_socket_get_ip_t ESP8266_ioctl_socket_get_ip;

	sockfd = calculate_socket_fd(sockfd);
	lp_sockaddr = (struct sockaddr_in   *)local_addr;
	sin_addr = &(lp_sockaddr->sin_addr);


	ESP8266_ioctl_socket_get_ip.strIP = ipAddr;
	ESP8266_ioctl_socket_get_ip.strIPLen = sizeof(ipAddr) - 1 ;
	retVal = DEV_IOCTL_1_PARAMS(esp8266_dev ,
			IOCTL_ESP8266_GET_IP , &ESP8266_ioctl_socket_get_ip);
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

	lp_sockaddr->sin_port = htons(sockfd);
	lp_sockaddr->sin_family = AF_INET;

	return 0;
}

int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	struct dev_desc_t *  socket_dev;
	struct sockaddr_in   *lp_sockaddr;
	struct  in_addr *sin_addr;
	char *ipAddrStr;
	char ipAddr[20] = {0};
	uint8_t retVal;
	struct ESP8266_ioctl_get_conn_status_t	ioctl_socket_get_open_connection;
	uint16_t port;

	sockfd = calculate_socket_fd(sockfd);
	lp_sockaddr = (struct sockaddr_in   *)addr;
	sin_addr = &(lp_sockaddr->sin_addr);

	socket_dev = allocated_socket_dev[sockfd];
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

struct hostent  curr_hostent = {0};
struct in_addr curr_addr;
char *h_addr_list[2] = {(char*)&curr_addr , NULL};
#define MAX_HOST_NAME	32
char curr_host_name[MAX_HOST_NAME] = {0};

struct hostent*  gethostbyname_uCprojects( const char *name)
{
	struct dev_desc_t *  socket_dev;
	struct ESP8266_ioctl_socket_open_t ioctl_socket_open;
	struct ESP8266_ioctl_socket_connect_t	ioctl_socket_connect;
	struct ESP8266_ioctl_get_conn_status_t	ioctl_socket_get_open_connection;
	uint16_t port;
	char *ipAddrStr;
	char ipAddr[20] = {0};
	uint8_t retVal;

	strncpy(curr_host_name , name , MAX_HOST_NAME-1);

	if (NULL == esp8266_dev) CRITICAL_ERROR("esp8266_dev is NULL");
	ioctl_socket_open.new_socket_descriptor = &socket_dev;

	retVal = DEV_IOCTL_1_PARAMS(esp8266_dev ,
			IOCTL_ESP8266_SOCKET_OPEN , &ioctl_socket_open);

	if (0 != retVal) return NULL;

	ioctl_socket_connect.strHostName = name;
	ioctl_socket_connect.strPort = "80";
	retVal = DEV_IOCTL_1_PARAMS(socket_dev ,
			IOCTL_ESP8266_SOCKET_CONNECT , &ioctl_socket_connect);
	if (0 != retVal)
	{
		DEV_IOCTL_0_PARAMS(socket_dev , IOCTL_ESP8266_SOCKET_CLOSE);
		retVal = DEV_IOCTL_1_PARAMS(esp8266_dev ,
				IOCTL_ESP8266_SOCKET_OPEN , &ioctl_socket_open);
		if (0 != retVal) return NULL;
		ioctl_socket_connect.strPort = "443";
		retVal = DEV_IOCTL_1_PARAMS(socket_dev ,
				IOCTL_ESP8266_SOCKET_CONNECT , &ioctl_socket_connect);
		if (0 != retVal)
		{
			DEV_IOCTL_0_PARAMS(socket_dev , IOCTL_ESP8266_SOCKET_CLOSE);
			return NULL;
		}
	}

	ioctl_socket_get_open_connection.strIP = ipAddr;
	ioctl_socket_get_open_connection.strIPLen = sizeof(ipAddr) - 1 ;
	ioctl_socket_get_open_connection.pPort = &port;
	retVal = DEV_IOCTL_1_PARAMS(socket_dev ,
			IOCTL_ESP8266_SOCKET_GET_OPEN_CONNECTION_STATUS ,
			&ioctl_socket_get_open_connection);
	DEV_IOCTL_0_PARAMS(socket_dev , IOCTL_ESP8266_SOCKET_CLOSE );

	if (0 != retVal) return NULL;

	ipAddrStr = ipAddr;
	curr_addr.S_un.S_un_b.s_b1 = atoi(ipAddrStr);
	ipAddrStr = strchr(ipAddrStr,'.');
	if(NULL == ipAddrStr) return NULL;
	ipAddrStr++;
	curr_addr.S_un.S_un_b.s_b2 = atoi(ipAddrStr);
	ipAddrStr = strchr(ipAddrStr+1,'.');
	if(NULL == ipAddrStr) return NULL;
	ipAddrStr++;
	curr_addr.S_un.S_un_b.s_b3 = atoi(ipAddrStr);
	ipAddrStr = strchr(ipAddrStr+1,'.');
	if(NULL == ipAddrStr) return NULL;
	ipAddrStr++;
	curr_addr.S_un.S_un_b.s_b4 = atoi(ipAddrStr);

	PRINTF_DBG("dbg : resolved address for %s:  %d.%d.%d.%d\n", name,
			curr_addr.S_un.S_un_b.s_b1, curr_addr.S_un.S_un_b.s_b2,
			curr_addr.S_un.S_un_b.s_b3, curr_addr.S_un.S_un_b.s_b4 );

	curr_hostent.h_name = curr_host_name;
	curr_hostent.h_aliases = NULL;
	curr_hostent.h_addrtype = AF_INET;
	curr_hostent.h_length = sizeof(struct in_addr);
	curr_hostent.h_addr_list = h_addr_list;


	return &curr_hostent;

}


struct servent  servent_obj = { 0 };
char  port_no;
struct servent *getservbyname_uCprojects(const char *name, const char *proto)
{
	//servent_obj.s_proto = &port_no;
	return NULL;
}



int select_uCprojects(int nfds, fd_set *readfds, fd_set *writefds,
                  fd_set *exceptfds, struct timeval *timeout)
{
	struct dev_desc_t *  socket_dev;
	uint8_t i;
	uint8_t read_ready;
	uint8_t event_happened;
	uint8_t retVal;
	fd_set orig_readfds;
	fd_set orig_writefds;
	uint32_t timeout_msec;
	uint32_t readfds_for_print;
	uint32_t writefds_for_print;

	if (NULL != readfds)
	{
		readfds_for_print = *(uint32_t*)readfds;
	}
	else
	{
		readfds_for_print = 0;
	}
	if (NULL != writefds)
	{
		writefds_for_print = *(uint32_t*)writefds;
	}
	else
	{
		writefds_for_print = 0;
	}
	PRINTF_DBG("%s : in *(uint32_t*)readfds= %u, *(uint32_t*)writefds = %u\n",
			__FUNCTION__, readfds_for_print, writefds_for_print);

	if (NULL != timeout)
	{
		timeout_msec = timeout->tv_sec * 1000;
		timeout_msec += timeout->tv_usec / 1000;
	}
	else
	{
		timeout_msec = 0;
	}

	if ((0 == nfds) &&
			(NULL == readfds) && (NULL == writefds) && (NULL == exceptfds))
	{
		if (0 != timeout_msec)
		{
			os_delay_ms( timeout_msec );
		}
		return 0;
	}

	if (NULL != readfds)
	{
		orig_readfds = *readfds;
		FD_ZERO(readfds);
	}
	if (NULL != writefds)
	{
		orig_writefds = *writefds;
		FD_ZERO(writefds);
	}
	if (NULL != exceptfds)
	{
		FD_ZERO(exceptfds);
	}

	event_happened = 0;
	while (1)
	{
		for(i = 0; i < MAX_NUM_OF_SOCKETS; i++)
		{
			if ((NULL != readfds) &&
					( 0 != FD_ISSET(i + SOCKET_OFFSET, &orig_readfds)))
			{
				socket_dev = allocated_socket_dev[i];
				retVal = DEV_IOCTL_1_PARAMS(socket_dev ,
						IOCTL_ESP8266_SOCKET_IS_DATA_RECEIVED , &read_ready);
				if(0 != retVal)
				{
					printf(
						"--%s : fd = %d connection was lost\n",__FUNCTION__, i);

					FD_SET(i + SOCKET_OFFSET, exceptfds);
				}
				if (read_ready)
				{
					PRINTF_DBG("%s : readfds\n", __FUNCTION__);
					FD_SET(i + SOCKET_OFFSET, readfds);
					event_happened++;
				}
			}
			if ((NULL != writefds) &&
					( 0 != FD_ISSET(i + SOCKET_OFFSET, &orig_writefds)))
			{
				PRINTF_DBG("%s : writefds\n", __FUNCTION__);
				FD_SET(i + SOCKET_OFFSET, writefds);
				event_happened++;
			}
		}
		if (event_happened || (0 == timeout_msec))
		{
			break;
		}
		if (NULL != timeout)
		{
			timeout_msec--;
		}
		os_delay_ms( 1 );
	}

	PRINTF_DBG("%s : out  event_happened = %d\n", __FUNCTION__, event_happened);

	return event_happened;// STILL CAN BE BLOCKED IN ESP8266 TASK
	//while(1);//temporary debug trap
}

ssize_t send_uCprojects(
		int sockfd, const void *buffer, size_t length, int flags)
{
	struct dev_desc_t *  socket_dev;
	 size_t ret_length;

	sockfd = calculate_socket_fd(sockfd);
	PRINTF_DBG("%s :  fd = %d\n",__FUNCTION__, sockfd);

	if(0 != flags)
	{
		CRITICAL_ERROR("flags != 0 not implemented yet");
	}

	socket_dev = allocated_socket_dev[sockfd];
	ret_length = DEV_WRITE(socket_dev , buffer , length) ;
	if(0 == ret_length)
	{
		CRITICAL_ERROR(" return length on send is 0");
	}
	return ret_length;
}


int listen_uCprojects(int sockfd, int backlog)
{
	CRITICAL_ERROR("listen_uCprojects not implemented yet");
	return 0;
}


int accept_uCprojects(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	CRITICAL_ERROR("accept_uCprojects not implemented yet");
	return 0;
}


int shutdown_uCprojects(int socket, int how)
{
	CRITICAL_ERROR("accept_uCprojects not implemented yet");
	return 0;
}


void _stat()
{
	CRITICAL_ERROR("_stat not implemented yet");
}


int WSAGetLastError(void)
{
	return errno;
}

void WSASetLastError(int iError)
{
	errno = iError;
}

int ioctlsocket(int socket, long   cmd, unsigned long *argp)
{
	CRITICAL_ERROR("ioctlsocket not implemented yet");
}

