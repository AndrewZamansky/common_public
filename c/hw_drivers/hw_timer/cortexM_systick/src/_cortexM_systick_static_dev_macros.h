
#ifndef _cortexM_systick_static_dev_macros_h_
#define _cortexM_systick_static_dev_macros_h_

#include "src/cortexM_systick.h"
uint8_t cortexM_systick_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);


#define __CORTEXM_SYSTICK_API_CREATE_STATIC_DEV(dev,dev_name ,input_clock , rate ,mode , callback)\
		extern const dev_descriptor_t dev ;						\
		CORTEXM_SYSTICK_Instance_t handle_of_##dev =	 { input_clock , rate , callback , mode };	\
		const dev_descriptor_t dev =								\
			{											\
				dev_name,								\
				&handle_of_##dev,						\
				cortexM_systick_ioctl,					\
				DEV_API_dummy_pwrite_func,				\
				DEV_API_dummy_pread_func,				\
				DEV_API_dummy_callback_func				\
			}

#endif
