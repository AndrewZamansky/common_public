
#ifndef _timer_static_dev_macros_h_
#define _timer_static_dev_macros_h_

#include "src/_timer_prerequirements_check.h"

extern uint8_t timer_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

#include "src/timer.h"

#define __TIMER_API_CREATE_STATIC_DEV(pdev ,hw_timer_pdev )											\
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(hw_timer_pdev) ;									\
		timer_instance_t handle_of_##pdev =	 {P_TO_STATIC_DEVICE_INST(hw_timer_pdev) , 0 , 0}; 		\
		STATIC_DEVICE(pdev , &handle_of_##pdev , timer_ioctl , 										\
				DEV_API_dummy_pwrite_func , DEV_API_dummy_pread_func , DEV_API_dummy_callback_func)



#endif
