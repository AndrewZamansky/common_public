
#ifndef _async_tx_wrapper_API_H_
#define _async_tx_wrapper_API_H_

#include "dev_management_api.h"

/*****************  defines  **************/


/**********  define API  types ************/


#define ASYNC_TX_WRAPPER_API_RX_BUFF_SIZE_STR   "rx_buff_size"
#define ASYNC_TX_WRAPPER_API_SERVER_DEVICE_STR   "server_device"

enum ASYNC_TX_WRAPPER_API_ioctl_e {
	IOCTL_ASYNC_TX_WRAPPER_SET_BUFF_SIZE = IOCTL_LAST_COMMON_IOCTL + 1,
	IOCTL_ASYNC_TX_WRAPPER_RESET,
	IOCTL_ASYNC_TX_WRAPPER_DISABLE_TX,
	IOCTL_ASYNC_TX_WRAPPER_ENABLE_TX,
};

/**********  define API  functions  ************/


struct async_tx_wrapper_cfg_t {
	struct dev_desc_t *server_dev;
	uint32_t           char_timeout; // max time to transmit one char
};


#endif
