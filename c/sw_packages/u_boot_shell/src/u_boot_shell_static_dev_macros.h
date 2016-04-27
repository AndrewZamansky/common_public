
#ifndef _u_boot_shell_static_dev_macros_h_
#define _u_boot_shell_static_dev_macros_h_

#include "src/_u_boot_shell_prerequirements_check.h"

extern uint8_t u_boot_shell_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);
extern uint8_t u_boot_shell_callback(void * const aHandle ,
		const uint8_t aCallback_num , void * aCallback_param1, void * aCallback_param2);

#include "src/u_boot_shell.h"

#define __U_BOOT_SHELL_API_CREATE_STATIC_DEV(dev,dev_name,server_dev )		\
		extern const dev_descriptor_t dev ;							\
		extern const dev_descriptor_t server_dev ;					\
		u_boot_shell_instance_t handle_of_##dev =	 {&server_dev,&dev,0,0}; \
														\
		const dev_descriptor_t dev =					\
			{											\
				dev_name,								\
				&handle_of_##dev,						\
				u_boot_shell_ioctl,							\
				DEV_API_dummy_pwrite_func,				\
				DEV_API_dummy_pread_func,				\
				u_boot_shell_callback							\
			}


#endif
