#include "_project.h"
#include "sys/socket.h"
//#include <sys/type.h>

#include "dev_management_api.h" // for device manager defines and typedefs
#include "std_net_functions_api.h"
#include "errno.h"
#include "os_wrapper.h"
//#define DEBUG
#include "PRINTF_api.h"


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

struct hostent*  gethostbyname_uCprojects(const char *name)
{
	struct ioctl_net_device_get_host_addr_t ioctl_net_device_get_host_addr;
	uint8_t  i;
	char ipAddr[MAX_ADDR_LEN] = {0};
	char *ipAddrStr;

	strncpy(curr_host_name , name , MAX_HOST_NAME - 1);

	ioctl_net_device_get_host_addr.name = name;
	ioctl_net_device_get_host_addr.addr_str = ipAddr;
	ioctl_net_device_get_host_addr.addr_str_len = MAX_ADDR_LEN - 1;
	for (i = 0; i < MAX_NUM_OF_NET_DEVS; i++)
	{
		struct dev_desc_t * net_dev;

		net_dev = net_dev_arr[i];
		if (NULL == net_dev)
		{
			continue;
		}

		if (0 == DEV_IOCTL_1_PARAMS(net_dev,
			IOCTL_NET_DEVICE_GET_HOST_ADDR , &ioctl_net_device_get_host_addr))
		{
			break;
		}

	}
	if (MAX_NUM_OF_NET_DEVS == i)
	{
		return NULL;
	}

	ipAddrStr = ipAddr;
	curr_addr.S_un.S_un_b.s_b1 = atoi(ipAddrStr);
	ipAddrStr = strchr(ipAddrStr, '.');
	if(NULL == ipAddrStr) return NULL;
	ipAddrStr++;
	curr_addr.S_un.S_un_b.s_b2 = atoi(ipAddrStr);
	ipAddrStr = strchr(ipAddrStr + 1, '.');
	if(NULL == ipAddrStr) return NULL;
	ipAddrStr++;
	curr_addr.S_un.S_un_b.s_b3 = atoi(ipAddrStr);
	ipAddrStr = strchr(ipAddrStr + 1, '.');
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



