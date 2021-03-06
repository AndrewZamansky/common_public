
#ifndef _heartbeat_API_H_
#define _heartbeat_API_H_

#include "_project_typedefs.h"
#include "_project_defines.h"
#include "dev_management_api.h"
#include "dev_common_ioctl_and_types.h"

enum HEARTBEAT_API_ioctl_t {
	HEARTBEAT_API_SET_CALLBACK_FUNC_IOCTL = IOCTL_LAST_COMMON_IOCTL+1,
	HEARTBEAT_API_CALL_FROM_IDLE_TASK,
	HEARTBEAT_API_EACH_1mS_CALL,
	HEARTBEAT_API_GET_CPU_USAGE,
	HEARTBEAT_API_BUSY_WAIT_mS
};


struct heartbeat_instance_t {
	struct dev_desc_t * heartbeat_callback_dev;
	struct dev_desc_t * callibration_timer;
};

SET_CONFIG_TYPE(heartbeat, struct heartbeat_instance_t);


#endif
