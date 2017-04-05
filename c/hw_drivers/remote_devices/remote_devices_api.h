
#ifndef _REMOTE_DEVICES_API_H_
#define _REMOTE_DEVICES_API_H_

#include "gpio_api.h"

/**********  define API  types ************/


#define REMOTE_DEVICES_API_SERVER_DEVICE_STR  	"server_device"



typedef enum
{
	IOCTL_REMOTE_SEND_CMD = IOCTL_LAST_COMMON_IOCTL+1,
}REMOTE_DEVICES_API_ioctl_t;


typedef struct {
	uint8_t *addr_num;
	uint8_t *comand_str;
	uint8_t comand_str_len;
	uint8_t result[4];
} remote_cmd_t;


/**********  define API  functions  ************/




uint8_t  remote_devices_api_init_dev_descriptor(struct dev_desc_t *aDevDescriptor);


#else
#pragma message( "warning : this header file had already been included" )
#endif
