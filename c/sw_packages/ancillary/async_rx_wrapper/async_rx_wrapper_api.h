
#ifndef _async_rx_wrapper_API_H_
#define _async_rx_wrapper_API_H_

#include "src/_async_rx_wrapper_prerequirements_check.h"

/*****************  defines  **************/


/**********  define API  types ************/


#define ASYNC_RX_WRAPPER_API_RX_BUFF_SIZE_STR   "rx_buff_size"
#define ASYNC_RX_WRAPPER_API_SERVER_DEVICE_STR  	"server_device"

typedef enum
{
	IOCTL_ASYNC_RX_WRAPPER_SET_BUFF_SIZE = IOCTL_LAST_COMMON_IOCTL+1,
	IOCTL_ASYNC_RX_WRAPPER_RESET,
	IOCTL_ASYNC_RX_WRAPPER_DONT_USE_TASK
}ASYNC_RX_WRAPPER_API_ioctl_t;

/**********  define API  functions  ************/



#endif
