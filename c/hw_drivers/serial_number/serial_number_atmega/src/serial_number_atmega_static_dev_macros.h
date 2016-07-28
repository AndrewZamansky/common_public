
#ifndef _serial_number_atmega_static_dev_macros_h_
#define _serial_number_atmega_static_dev_macros_h_

uint8_t serial_atmega_ioctl( pdev_descriptor_t apdev ,const uint8_t aIoctl_num , uint32_t aIoctl_param);


#define __SERIAL_NUMBER_ATMEGA_API_CREATE_STATIC_DEV(dev,dev_name ,port , pin , mode)\
		const dev_descriptor_t dev =								\
			{											\
				dev_name,								\
				NULL,									\
				serial_atmega_ioctl,					\
				DEV_API_dummy_pwrite_func,				\
				DEV_API_dummy_pread_func,				\
				DEV_API_dummy_callback_func				\
			}


#endif
