
#ifndef _heartbeat_API_H_
#define _heartbeat_API_H_

#include "heartbeat_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_heartbeat_prerequirements_check.h" // should be after {heartbeat_config.h,dev_managment_api.h}

/*****************  defines  **************/


/**********  define API  types ************/


typedef enum
{
	HEARTBEAT_API_SET_CALLBACK_FUNC_IOCTL = IOCTL_LAST_COMMON_IOCTL+1,
	HEARTBEAT_API_CALL_FROM_IDLE_TASK,
	HEARTBEAT_API_EACH_1mS_CALL,
	HEARTBEAT_API_GET_CPU_USAGE,
	HEARTBEAT_API_BUSY_WAIT_mS
}HEARTBEAT_API_ioctl_t;


/**********  define API  functions  ************/
uint8_t  heartbeat_api_init_dev_descriptor(pdev_descriptor aDevDescriptor);

#include "src/_heartbeat_static_dev_macros.h"

#define HEARTBEAT_API_CREATE_STATIC_DEV(dev,dev_name ,heartbeat_callback ,callibration_timer ) \
			__HEARTBEAT_API_CREATE_STATIC_DEV(dev,dev_name ,heartbeat_callback,callibration_timer)

#endif
