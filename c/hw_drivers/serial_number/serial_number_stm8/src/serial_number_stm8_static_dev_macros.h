
#ifndef _serial_number_stm8_static_dev_macros_h_
#define _serial_number_stm8_static_dev_macros_h_

uint8_t serial_stm8_ioctl( void * const aHandle ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2);


#define __SERIAL_NUMBER_STM8_API_CREATE_STATIC_DEV(dev,dev_name ,port , pin , mode)\
		const dev_descriptor_t dev =								\
			{											\
				dev_name,								\
				NULL,									\
				serial_stm8_ioctl,						\
				DEV_API_dummy_pwrite_func,				\
				DEV_API_dummy_pread_func,				\
				DEV_API_dummy_callback_func				\
			}



#endif
