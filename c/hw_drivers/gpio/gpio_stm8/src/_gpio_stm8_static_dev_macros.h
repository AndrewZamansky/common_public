
#ifndef _gpio_stm8_static_dev_macros_h_
#define _gpio_stm8_static_dev_macros_h_

#include "src/gpio_stm8.h"
uint8_t gpio_stm8_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);


#define __GPIO_STM8_API_CREATE_STATIC_DEV(pdev ,port , pin , mode)		\
		GPIO_STM8_Instance_t handle_of_##pdev =	 {port , pin , mode};	\
		STATIC_DEVICE(pdev , &handle_of_##pdev , gpio_stm8_ioctl , 		\
				DEV_API_dummy_pwrite_func , DEV_API_dummy_pread_func , DEV_API_dummy_callback_func)

#endif
