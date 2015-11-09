
#ifndef _gpio_NUC505_static_dev_macros_h_
#define _gpio_NUC505_static_dev_macros_h_

#include "src/gpio_nuc505.h"
uint8_t gpio_nuc505_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);


#define __GPIO_NUC505_API_CREATE_STATIC_DEV(dev,dev_name ,port , pin , mode)\
		extern const dev_descriptor_t dev ;						\
		GPIO_NUC505_Instance_t handle_of_##dev =	 {port , pin , mode};	\
		const dev_descriptor_t dev =								\
			{											\
				dev_name,								\
				&handle_of_##dev,						\
				gpio_nuc505_ioctl,						\
				DEV_API_dummy_pwrite_func,				\
				DEV_API_dummy_pread_func,				\
				DEV_API_dummy_callback_func				\
			}

#endif
