
#ifndef _timer_static_dev_macros_h_
#define _timer_static_dev_macros_h_

#include "src/_timer_prerequirements_check.h"

extern uint8_t timer_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

#include "src/timer.h"

#define __TIMER_API_CREATE_STATIC_DEV(dev,dev_name ,hw_timer_dev )		\
		extern const dev_descriptor_t dev ;							\
		extern const dev_descriptor_t hw_timer_dev ;							\
		timer_instance_t handle_of_##dev =	 {&hw_timer_dev,0,0}; \
														\
		const dev_descriptor_t dev =					\
			{											\
				dev_name,								\
				&handle_of_##dev,						\
				timer_ioctl,							\
				DEV_API_dummy_pwrite_func,				\
				DEV_API_dummy_pread_func,				\
				DEV_API_dummy_callback_func				\
			}


#endif
