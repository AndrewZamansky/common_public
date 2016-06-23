
#ifndef _shell_static_dev_macros_h_
#define _shell_static_dev_macros_h_

#include "src/_shell_prerequirements_check.h"

extern uint8_t shell_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);
extern uint8_t shell_callback(void * const aHandle ,
		const uint8_t aCallback_num , void * aCallback_param1, void * aCallback_param2);

#include "src/shell.h"

#define __SHELL_API_CREATE_STATIC_DEV(pdev , server_pdev )			\
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(pdev) ;		\
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(server_pdev) ;		\
		shell_instance_t handle_of_##pdev =	 						\
			{P_TO_STATIC_DEVICE_INST(server_pdev) , P_TO_STATIC_DEVICE_INST(pdev) , NULL , 0 , 0}; \
		STATIC_DEVICE(pdev , &handle_of_##pdev , shell_ioctl , 							\
				DEV_API_dummy_pwrite_func , DEV_API_dummy_pread_func , shell_callback)



#endif
