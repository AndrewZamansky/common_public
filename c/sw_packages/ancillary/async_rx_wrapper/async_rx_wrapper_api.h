
#ifndef _async_rx_wrapper_API_H_
#define _async_rx_wrapper_API_H_

#include "_project_defines.h"
#include "dev_management_api.h"

/*****************  defines  **************/


/**********  define API  types ************/


#define ASYNC_RX_WRAPPER_API_RX_BUFF_SIZE_STR   "rx_buff_size"
#define ASYNC_RX_WRAPPER_API_SERVER_DEVICE_STR  "server_device"

enum ASYNC_RX_WRAPPER_API_ioctl_e {
	IOCTL_ASYNC_RX_WRAPPER_SET_BUFF_SIZE = IOCTL_LAST_COMMON_IOCTL + 1,
	IOCTL_ASYNC_RX_WRAPPER_RESET,
	IOCTL_ASYNC_RX_WRAPPER_DONT_USE_TASK
};


#define ASYNC_RX_WRAPPER_RX_BUFFER_SIZE  64

#if defined(CONFIG_ASYNC_RX_WRAPPER_USE_MALLOC)
	typedef size_t rx_int_size_t;
#else
	#if ASYNC_RX_WRAPPER_RX_BUFFER_SIZE <= (1<<8)
		typedef uint8_t rx_int_size_t;
	#else
		#if ASYNC_RX_WRAPPER_RX_BUFFER_SIZE <= (1<<16)
			typedef uint16_t rx_int_size_t;
		#else
			typedef size_t rx_int_size_t;
		#endif
	#endif
#endif

struct async_rx_wrapper_cfg_t {

	struct dev_desc_t *   server_dev;
	struct dev_desc_t *   client_dev;
	rx_int_size_t rx_buff_size;
#ifdef CONFIG_ASYNC_RX_WRAPPER_USE_MALLOC
	uint8_t *rx_buff;
#else
	uint8_t rx_buff[ASYNC_RX_WRAPPER_RX_BUFFER_SIZE];
#endif

};

/**********  define API  functions  ************/



#endif
