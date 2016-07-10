
#ifndef _SW_GPIO_WRAPPER_API_H_
#define _SW_GPIO_WRAPPER_API_H_

#include "gpio_api.h"
/**********  define API  types ************/



typedef enum
{
	IOCTL_SW_GPIO_WRAPPER_SET_BEHAVIOR_PARAM = IOCTL_LAST_COMMON_GPIO_IOCTL+1,
	IOCTL_SW_GPIO_WRAPPER_PIN_FORCE_SET,
	IOCTL_SW_GPIO_WRAPPER_PIN_FORCE_GET,
	IOCTL_SW_GPIO_WRAPPER_PIN_SET,
	IOCTL_SW_GPIO_WRAPPER_PIN_CLEAR,
	IOCTL_SW_GPIO_WRAPPER_PIN_READ
}SW_GPIO_WRAPPER_API_ioctl_t;


#define SW_GPIO_WRAPPER_API_BEHAVIOR_STR  		"behavior"
#define SW_GPIO_WRAPPER_API_FORCE_STR  			"force_output"
#define SW_GPIO_WRAPPER_API_SERVER_DEVICE_STR  	"server_device"


#define GPIO_API_FORCE_NONE_STR  			"none"
#define GPIO_API_FORCE_SET_STR  			"1"
#define GPIO_API_FORCE_CLEAR_STR   			"0"

/**********  define API  functions  ************/



uint8_t sw_gpio_wrapper_api_init(void);

uint8_t  sw_gpio_wrapper_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor);


#else
#pragma message( "warning : this header file had already been included" )
#endif
