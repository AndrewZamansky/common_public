
#ifndef _LM35_static_dev_macros_h_
#define _LM35_static_dev_macros_h_

#include "src/LM35.h"
uint8_t LM35_ioctl( pdev_descriptor_t apdev ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);


#define __LM35_API_CREATE_STATIC_DEV(dev,dev_name ,adc_server_dev )\
		extern const dev_descriptor_t dev ;						\
		extern const dev_descriptor_t adc_server_dev ;				\
		LM35_Instance_t handle_of_##dev =	 {&adc_server_dev};	\
		const dev_descriptor_t dev =								\
			{											\
				dev_name,								\
				&handle_of_##dev,						\
				LM35_ioctl,								\
				DEV_API_dummy_pwrite_func,				\
				DEV_API_dummy_pread_func,				\
				DEV_API_dummy_callback_func				\
			}

#endif
