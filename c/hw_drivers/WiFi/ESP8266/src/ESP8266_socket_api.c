#include "_project.h"
#include "sys/socket.h"
#include "sys/time.h"

#include "dev_management_api.h" // for device manager defines and typedefs
#include "ESP8266_api.h" // for device manager defines and typedefs
#include "errno.h"
#include "os_wrapper.h"

static pdev_descriptor_t esp8266_dev = NULL;

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

void set_esp8266_pdev_for_socket_api(pdev_descriptor_t a_esp8266_dev)
{
	esp8266_dev = a_esp8266_dev;
}

#define MAX_NUM_OF_SOCKETS	4
pdev_descriptor_t  allocated_socket_dev[MAX_NUM_OF_SOCKETS]={0};

int socket(int socket_family, int socket_type, int protocol)
{
	uint8_t i;
	uint8_t retVal;

	switch (socket_family)
	{
		case AF_INET:
			break;
		default:
			while(1);
	}

	switch (socket_type)
	{
		case SOCK_STREAM:
			break;
		default:
			while(1);
	}

	switch (protocol)
	{
		case IPPROTO_IP:
		case IPPROTO_TCP:
			break;
		default:
			while(1);
	}

	for (i = 0 ; i< MAX_NUM_OF_SOCKETS ; i++)
	{
		if (NULL == allocated_socket_dev[i])
		{
			ESP8266_ioctl_socket_open_t ioctl_socket_open;
			ioctl_socket_open.new_socket_descriptor = &allocated_socket_dev[i];
			retVal = DEV_IOCTL_1_PARAMS(esp8266_dev ,
					IOCTL_ESP8266_SOCKET_OPEN , &ioctl_socket_open);
			if (0 == retVal)
			{
				return i;
			}
			else
			{
				return -1;
			}
		}
	}
	return -1;
}


int _close(int file)
{
	pdev_descriptor_t  socket_dev;
	uint8_t retVal;

	socket_dev = allocated_socket_dev[file];
	retVal = DEV_IOCTL_0_PARAMS(socket_dev , IOCTL_ESP8266_SOCKET_CLOSE);

	if ( (0 != retVal) &&
			(ESP8266_ERR_SOCKET_NOT_AVAILABLE != retVal)) while(1);
	return 0;
}


int connect(int sockfd, const struct sockaddr *addr, unsigned int addrlen)
{
	pdev_descriptor_t  socket_dev;
	ESP8266_ioctl_socket_connect_t	ioctl_socket_connect;
	struct sockaddr_in   *lp_sockaddr;
	struct  in_addr *sin_addr;
	char addr_str[32]={0};
	char port_str[8]={0};
	uint8_t retVal;

	lp_sockaddr = (struct sockaddr_in   *)addr;
	sin_addr = &(lp_sockaddr->sin_addr);

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

	if(0 != retVal) while(1);

	return 0;
}


size_t recv(int sockfd, void *buf, size_t len, int flags)
{
	pdev_descriptor_t  socket_dev;
	size_t size_received;
	ESP8266_ioctl_data_received_t ESP8266_ioctl_data_received;
	uint8_t retVal;

	if (0 != flags) while(1);

	ESP8266_ioctl_data_received.buffer = buf;
	ESP8266_ioctl_data_received.max_size = len;
	ESP8266_ioctl_data_received.size_received = &size_received;
	socket_dev = allocated_socket_dev[sockfd];
	retVal = DEV_IOCTL_1_PARAMS(socket_dev ,
			IOCTL_ESP8266_SOCKET_GET_RECEIVED_DATA ,
			&ESP8266_ioctl_data_received);

	if (0 != retVal)
	{
		errno = ENOTCONN;
	}
	static volatile int xxx=0;
	xxx++;
	if(2 == xxx)
	{
		xxx++;
	}
	return size_received;
}

int getsockopt(int sockfd, int level, int optname,
                      void *optval, socklen_t *optlen)
{
	if(SOL_SOCKET != level) while(1);

	switch(optname)
	{
		case SO_ERROR:
			if(NULL != optval)
			{
				*(int*)optval = 0;
				*optlen = sizeof(int);
			}
			return 0;

		default:
			while(1);
	}
}

void setsockopt()
{
	while(1);//temporary debug trap
}

void bind()
{
	while(1);//temporary debug trap
}

int getsockname(int sockfd, struct sockaddr *local_addr, socklen_t *addrlen)
{
	struct sockaddr_in   *lp_sockaddr;
	struct  in_addr *sin_addr;
	char *ipAddrStr;
	char ipAddr[20] = {0};
	uint8_t retVal;
	ESP8266_ioctl_socket_get_ip_t ESP8266_ioctl_socket_get_ip;

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
	pdev_descriptor_t  socket_dev;
	struct sockaddr_in   *lp_sockaddr;
	struct  in_addr *sin_addr;
	char *ipAddrStr;
	char ipAddr[20] = {0};
	uint8_t retVal;
	ESP8266_ioctl_get_conn_status_t	ioctl_socket_get_open_connection;
	uint16_t port;

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

struct hostent*  gethostbyname( const char *name)
{
	pdev_descriptor_t  socket_dev;
	ESP8266_ioctl_socket_open_t ioctl_socket_open;
	ESP8266_ioctl_socket_connect_t	ioctl_socket_connect;
	ESP8266_ioctl_get_conn_status_t	ioctl_socket_get_open_connection;
	uint16_t port;
	char *ipAddrStr;
	char ipAddr[20] = {0};
	uint8_t retVal;

	strncpy(curr_host_name , name , MAX_HOST_NAME-1);

	if (NULL == esp8266_dev) CRITICAL_ERROR("");
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

	curr_hostent.h_name = curr_host_name;
	curr_hostent.h_aliases = NULL;
	curr_hostent.h_addrtype = AF_INET;
	curr_hostent.h_length = sizeof(struct in_addr);
	curr_hostent.h_addr_list = h_addr_list;


	return &curr_hostent;

}



int select(int nfds, fd_set *readfds, fd_set *writefds,
                  fd_set *exceptfds, struct timeval *timeout)
{
	pdev_descriptor_t  socket_dev;
	uint8_t i;
	uint8_t read_ready;
	uint8_t retVal;

	for(i=0; i<MAX_NUM_OF_SOCKETS ; i++)
	{
		if ( 0 != FD_ISSET(i , readfds))
		{
			socket_dev = allocated_socket_dev[i];
			retVal = DEV_IOCTL_1_PARAMS(socket_dev ,
					IOCTL_ESP8266_SOCKET_IS_DATA_RECEIVED , &read_ready);
			if(0 != retVal)
			{
				FD_SET(i , exceptfds);
			}
			if (0 == read_ready)
			{
				FD_CLR(i , readfds);
				os_delay_ms( 1 );

			}
					//os_delay_ms( 1000 );

		}
	}

	FD_ZERO(exceptfds);
	return 1;// STILL CAN BE BLOCKED IN ESP8266 TASK
	//while(1);//temporary debug trap
}

ssize_t send(int socket, const void *buffer, size_t length, int flags)
{
	pdev_descriptor_t  socket_dev;
	 size_t ret_length;

	if(0 != flags) while(1);

	socket_dev = allocated_socket_dev[socket];
	ret_length = DEV_WRITE(socket_dev , buffer , length) ;
	if(0==ret_length) while(1);
	return ret_length;
}

void _stat()
{
	while(1);//temporary debug trap
}

