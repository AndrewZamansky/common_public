
#ifndef _sw_timer_wrapper_API_H_
#define _sw_timer_wrapper_API_H_

#include "sw_timer_wrapper_config.h"
#include "dev_management_api.h" // for device manager defines and typedefs
#include "src/_sw_timer_wrapper_prerequirements_check.h" // should be after {sw_timer_wrapper_config.h,dev_management_api.h}

/*****************  defines  **************/


/**********  define API  types ************/


#define SW_TIMER_WRAPPER_API_SERVER_DEVICE_STR  	"server_device"

typedef enum
{
	IOCTL_SW_TIMER_WRAPPER_START_TIMER = IOCTL_LAST_COMMON_IOCTL+1,
	IOCTL_SW_TIMER_WRAPPER_STOP_TIMER,
	IOCTL_SW_TIMER_WRAPPER_SET_TIMEOUT,
	IOCTL_SW_TIMER_WRAPPER_TEST_IF_TIMEOUT,
}SW_TIMER_WRAPPER_API_ioctl_t;

/**********  define API  functions  ************/

uint8_t  sw_timer_wrapper_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor);

#include "src/sw_timer_wrapper_static_dev_macros.h"


#define SW_TIMER_WRAPPER_API_CREATE_STATIC_DEV(dev,dev_name, server_dev ) 	\
			__SW_TIMER_WRAPPER_API_CREATE_STATIC_DEV(dev,dev_name, server_dev )





#endif
