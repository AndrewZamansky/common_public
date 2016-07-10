
#ifndef _sw_timer_wrapper_static_dev_macros_h_
#define _sw_timer_wrapper_static_dev_macros_h_


extern uint8_t sw_timer_wrapper_ioctl(void * const aHandle ,const uint8_t aIoctl_num ,void * aIoctl_param1 , void * aIoctl_param2);

#include "src/sw_timer_wrapper.h"


#define __SW_TIMER_WRAPPER_API_CREATE_STATIC_DEV(dev,dev_name, server_dev )	\
		extern const dev_descriptor_t dev ;						\
		extern const dev_descriptor_t server_dev ;				\
SW_TIMER_WRAPPER_Instance_t handle_of_##dev =	 {				\
			&server_dev ,	/*server_dev*/			\
			0,										\
			0										\
		};											\
	const dev_descriptor_t dev =					\
		{											\
			dev_name,								\
			&handle_of_##dev,						\
			sw_timer_wrapper_ioctl,					\
			DEV_API_dummy_pwrite_func,				\
			DEV_API_dummy_pread_func,				\
			DEV_API_dummy_callback_func				\
		}





#endif
