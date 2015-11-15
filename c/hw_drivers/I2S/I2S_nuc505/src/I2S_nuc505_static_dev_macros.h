
#ifndef _I2S_nuc505_static_dev_macros_h_
#define _I2S_nuc505_static_dev_macros_h_

#include "src/I2S_nuc505.h"
uint8_t I2S_nuc505_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);


#define __I2S_NUC505_API_CREATE_STATIC_DEV(dev,dev_name,callback_dev)\
		extern const dev_descriptor_t dev ;						\
		extern const dev_descriptor_t callback_dev ;			\
		I2S_NUC505_Instance_t handle_of_##dev =	 {&callback_dev};	\
		const dev_descriptor_t dev =								\
			{											\
				dev_name,								\
				&handle_of_##dev,						\
				I2S_nuc505_ioctl,						\
				DEV_API_dummy_pwrite_func,						\
				DEV_API_dummy_pread_func,				\
				DEV_API_dummy_callback_func				\
			}

#endif
