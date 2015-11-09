
#ifndef _adc_stm8_static_dev_macros_h_
#define _adc_stm8_static_dev_macros_h_

#include "src/adc_stm8.h"
uint8_t adc_stm8_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);


#define __ADC_STM8_API_CREATE_STATIC_DEV(dev,dev_name ,channel )\
		extern const dev_descriptor_t dev ;						\
		ADC_STM8_Instance_t handle_of_##dev =	 {channel};	\
		const dev_descriptor_t dev =								\
			{											\
				dev_name,								\
				&handle_of_##dev,						\
				adc_stm8_ioctl,							\
				DEV_API_dummy_pwrite_func,				\
				DEV_API_dummy_pread_func,				\
				DEV_API_dummy_callback_func				\
			}

#endif
