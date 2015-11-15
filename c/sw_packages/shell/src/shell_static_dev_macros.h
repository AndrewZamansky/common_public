
#ifndef _shell_static_dev_macros_h_
#define _shell_static_dev_macros_h_

#include "shell_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_shell_prerequirements_check.h" // should be after {shell_config.h,dev_managment_api.h}

extern uint8_t shell_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);
extern uint8_t shell_callback(void * const aHandle ,
		const uint8_t aCallback_num , void * aCallback_param1, void * aCallback_param2);

#include "src/shell.h"

#define __SHELL_API_CREATE_STATIC_DEV(dev,dev_name,server_dev )		\
		extern const dev_descriptor_t dev ;							\
		extern const dev_descriptor_t server_dev ;					\
		shell_instance_t handle_of_##dev =	 {&server_dev,&dev,0,0}; \
														\
		const dev_descriptor_t dev =					\
			{											\
				dev_name,								\
				&handle_of_##dev,						\
				shell_ioctl,							\
				DEV_API_dummy_pwrite_func,				\
				DEV_API_dummy_pread_func,				\
				shell_callback							\
			}


#endif
