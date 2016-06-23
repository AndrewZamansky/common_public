
#ifndef _cortexM_systick_static_dev_macros_h_
#define _cortexM_systick_static_dev_macros_h_

#include "src/cortexM_systick.h"
uint8_t cortexM_systick_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);


#define __CORTEXM_SYSTICK_API_CREATE_STATIC_DEV(pdev ,  rate , mode , callback)			\
		extern pdev_descriptor_t pdev ;													\
		CORTEXM_SYSTICK_Instance_t handle_of_##pdev =	 {  rate , callback , mode  };	\
		STATIC_DEVICE(pdev , &handle_of_##pdev , cortexM_systick_ioctl , 				\
				DEV_API_dummy_pwrite_func , DEV_API_dummy_pread_func , DEV_API_dummy_callback_func)


#endif
