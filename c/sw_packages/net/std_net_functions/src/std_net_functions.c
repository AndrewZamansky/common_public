#include "_project.h"
#include "sys/socket.h"
#include "sys/netdb.h"
//#include <sys/type.h>

#include "dev_management_api.h"
#include "std_net_functions_api.h"
#include "errno.h"
#include "os_wrapper.h"

#define DEBUG
#include "PRINTF_api.h"
#include "errors_api.h"


#define  MAX_NUM_OF_NET_DEVS  2
static struct dev_desc_t *  net_dev_arr[MAX_NUM_OF_NET_DEVS] = {0};

uint8_t std_net_functions_api_register_net_device(struct dev_desc_t * ap_dev)
{
	int i;

	for (i = 0; i < MAX_NUM_OF_NET_DEVS; i++)
	{
		if (NULL == net_dev_arr[i])
		{
			net_dev_arr[i] = ap_dev;
			return 0;
		}
	}
	return 1;
}


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



struct hostent  curr_hostent = {0};
struct in_addr curr_addr;
char *h_addr_list[2] = {(char*)&curr_addr , NULL};
#define MAX_HOST_NAME  32
char curr_host_name[MAX_HOST_NAME] = {0};
#define MAX_ADDR_LEN  20

static  uint8_t  get_ip_addr_str(
		const char *host_name, char *ip_str, size_t ip_str_len)
{
	uint8_t  i;
	struct dev_desc_t * net_dev;
	struct ioctl_net_device_get_host_addr_t ioctl_net_device_get_host_addr;

	ioctl_net_device_get_host_addr.name = host_name;
	ioctl_net_device_get_host_addr.addr_str = ip_str;
	ioctl_net_device_get_host_addr.addr_str_len = ip_str_len - 1;

	for (i = 0; i < MAX_NUM_OF_NET_DEVS; i++)
	{
		net_dev = net_dev_arr[i];
		if (NULL == net_dev)
		{
			continue;
		}

		if (0 == DEV_IOCTL(net_dev,
			IOCTL_NET_DEVICE_GET_HOST_ADDR , &ioctl_net_device_get_host_addr))
		{
			return 0;
		}

	}
	return 1;
}


static uint8_t fill_in_addr(char *ip_addr_str, struct in_addr *p_in_addr)
{
	p_in_addr->S_un.S_un_b.s_b1 = atoi(ip_addr_str);
	ip_addr_str = strchr(ip_addr_str, '.');
	if(NULL == ip_addr_str) return 1;
	ip_addr_str++;
	p_in_addr->S_un.S_un_b.s_b2 = atoi(ip_addr_str);
	ip_addr_str = strchr(ip_addr_str + 1, '.');
	if(NULL == ip_addr_str) return 1;
	ip_addr_str++;
	p_in_addr->S_un.S_un_b.s_b3 = atoi(ip_addr_str);
	ip_addr_str = strchr(ip_addr_str + 1, '.');
	if(NULL == ip_addr_str) return 1;
	ip_addr_str++;
	p_in_addr->S_un.S_un_b.s_b4 = atoi(ip_addr_str);

	PRINTF_DBG("dbg : resolved address for %s:  %d.%d.%d.%d\n", ip_addr_str,
			p_in_addr->S_un.S_un_b.s_b1, p_in_addr->S_un.S_un_b.s_b2,
			p_in_addr->S_un.S_un_b.s_b3, p_in_addr->S_un.S_un_b.s_b4 );
	return 0;
}


struct hostent*  gethostbyname(const char *name)
{
	char ipAddr[MAX_ADDR_LEN] = {0};

	strncpy(curr_host_name , name , MAX_HOST_NAME - 1);

	if (0 != get_ip_addr_str(name, ipAddr, MAX_ADDR_LEN))
	{
		return NULL;
	}

	if (0 != fill_in_addr(ipAddr, &curr_addr))
	{
		return NULL;
	}

	curr_hostent.h_name = curr_host_name;
	curr_hostent.h_aliases = NULL;
	curr_hostent.h_addrtype = AF_INET;
	curr_hostent.h_length = sizeof(struct in_addr);
	curr_hostent.h_addr_list = h_addr_list;

	return &curr_hostent;
}


struct servent  servent_obj = { 0 };
char  port_no;
struct servent *getservbyname(const char *name, const char *proto)
{
	CRITICAL_ERROR("getservbyname not implemented yet");
	//servent_obj.s_proto = &port_no;
	return NULL;
}


int getaddrinfo(const char *node, const char *service,
					   const struct addrinfo *hints,
					   struct addrinfo **res)
{
	char ipAddr[MAX_ADDR_LEN] = {0};
	struct addrinfo *new_addrinfo;
	struct sockaddr *ai_addr;
	struct sockaddr_in *serv_addr;

	if (0 != get_ip_addr_str(node, ipAddr, MAX_ADDR_LEN))
	{
		return -1;
	}

	new_addrinfo = (struct addrinfo *)os_safe_malloc(sizeof(struct addrinfo));
	errors_api_check_if_malloc_succeed(new_addrinfo);
	ai_addr = (struct sockaddr *)os_safe_malloc(sizeof(struct sockaddr));
	errors_api_check_if_malloc_succeed(ai_addr);

	new_addrinfo->ai_next = NULL;
	new_addrinfo->ai_family = AF_INET;
	new_addrinfo->ai_socktype = hints->ai_socktype;
	new_addrinfo->ai_protocol = hints->ai_protocol;
	new_addrinfo->ai_addr = ai_addr;
	serv_addr = &ai_addr->S_ub_sockaddr_in;
	serv_addr->sin_family = hints->ai_family;
	serv_addr->sin_port = htons(atoi(service));
	if (0 != fill_in_addr(ipAddr, &serv_addr->sin_addr))
	{
		return -1;
	}

	*res = new_addrinfo;
	return 0;
}

void freeaddrinfo(struct addrinfo *res)
{
	struct addrinfo *next_addrinfo;

	while (NULL != res)
	{
		next_addrinfo = res->ai_next;
		os_safe_free(res->ai_addr);
		os_safe_free(res);
		res = next_addrinfo;
	}
}


#include "winsock2.h"
int WSAStartup(WORD wVersionRequired, LPWSADATA lpWSAData)
{
	return 0;
}
