
#ifndef _heartbeat_static_dev_macros_h_
#define _heartbeat_static_dev_macros_h_

#include "src/_heartbeat_prerequirements_check.h"

extern uint8_t heartbeat_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

#include "src/heartbeat.h"

#define __HEARTBEAT_API_CREATE_STATIC_DEV(dev,dev_name,heartbeat_callback,callibration_timer )		\
		extern const dev_descriptor_t dev ;							\
		extern const dev_descriptor_t callibration_timer ;							\
		heartbeat_instance_t handle_of_##dev =	 {heartbeat_callback,&callibration_timer}; \
														\
		const dev_descriptor_t dev =					\
			{											\
				dev_name,								\
				&handle_of_##dev,						\
				heartbeat_ioctl,							\
				DEV_API_dummy_pwrite_func,				\
				DEV_API_dummy_pread_func,				\
				DEV_API_dummy_callback_func				\
			}


#endif
