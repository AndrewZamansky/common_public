
#ifndef _heartbeat_API_H_
#define _heartbeat_API_H_

#include "src/_heartbeat_prerequirements_check.h"

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

typedef enum
{
	HEARTBEAT_API_HEARTBEAT_TICK = CALLBACK_LAST_COMMON_IOCTL + 1

}HEARTBEAT_API_callback_t;

/**********  define API  functions  ************/
uint8_t  heartbeat_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor);


#endif
