
#ifndef _GPIO_REMOTE_API_H_
#define _GPIO_REMOTE_API_H_

#include "gpio_api.h"

/**********  define API  types ************/


#define GPIO_REMOTE_API_ADDR_STR   	"device_address"
#define GPIO_REMOTE_API_PIN_STR   	"pin"
#define GPIO_REMOTE_API_MODE_STR   	"mode"
#define GPIO_REMOTE_API_SERVER_DEVICE_STR  	"server_device"



typedef enum
{
	IOCTL_GPIO_REMOTE_SET_ADDR_PARAM = IOCTL_LAST_COMMON_GPIO_IOCTL+1,
	IOCTL_GPIO_REMOTE_SET_PIN_PARAM,
	IOCTL_GPIO_REMOTE_SET_MODE_PARAM,
}GPIO_REMOTE_API_ioctl_t;




/**********  define API  functions  ************/




uint8_t  gpio_remote_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor);


#else
#pragma message( "warning : this header file had already been included" )
#endif
