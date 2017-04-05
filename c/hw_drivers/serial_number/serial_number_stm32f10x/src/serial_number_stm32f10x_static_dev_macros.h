
#ifndef _serial_number_stm32f10x_static_dev_macros_h_
#define _serial_number_stm32f10x_static_dev_macros_h_

uint8_t serial_stm32f10x_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num ,
		void * aIoctl_param1 , void * aIoctl_param2);



#define __SERIAL_NUMBER_ATMEGA_API_CREATE_STATIC_DEV(dev,dev_name ,port , pin , mode)\
		const struct dev_desc_t dev =								\
			{											\
				dev_name,								\
				NULL,									\
				serial_stm32f10x_ioctl,					\
				DEV_API_dummy_pwrite_func,				\
				DEV_API_dummy_pread_func,				\
				DEV_API_dummy_callback_func				\
			}

#endif
