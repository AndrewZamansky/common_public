
#ifndef _gpio_stm32f10x_static_dev_macros_h_
#define _gpio_stm32f10x_static_dev_macros_h_

#include "src/gpio_stm32f10x.h"
uint8_t gpio_stm32f10x_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

#define __GPIO_STM32F10X_API_CREATE_STATIC_DEV(dev,dev_name ,port , pin_mask , mode)\
		extern const dev_descriptor_t dev ;						\
		GPIO_STM32F10X_Instance_t handle_of_##dev =	 {port , 0,pin_mask , mode};	\
		const dev_descriptor_t dev =								\
			{											\
				dev_name,								\
				&handle_of_##dev,						\
				gpio_stm32f10x_ioctl,					\
				DEV_API_dummy_pwrite_func,				\
				DEV_API_dummy_pread_func,				\
				DEV_API_dummy_callback_func				\
			}


#endif
