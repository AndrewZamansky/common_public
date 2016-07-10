
#ifndef _heartbeat_static_dev_macros_h_
#define _heartbeat_static_dev_macros_h_

#include "src/_heartbeat_prerequirements_check.h"

extern uint8_t heartbeat_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

#include "src/heartbeat.h"

#define __HEARTBEAT_API_CREATE_STATIC_DEV(pdev , heartbeat_callback , callibration_timer_pdev )	\
		extern pdev_descriptor_t pdev ;															\
		extern pdev_descriptor_t callibration_timer_pdev ;										\
		heartbeat_instance_t handle_of_##pdev =	 												\
			{heartbeat_callback , P_TO_STATIC_DEVICE_INST(callibration_timer_pdev)}; 			\
		STATIC_DEVICE(pdev , &handle_of_##pdev , heartbeat_ioctl , 								\
				DEV_API_dummy_pwrite_func , DEV_API_dummy_pread_func , DEV_API_dummy_callback_func)


#endif
