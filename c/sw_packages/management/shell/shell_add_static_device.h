#ifndef _add_static_device_step1
#define _add_static_device_step1

#include  "shell_api.h"

#undef _add_static_device_step2

#undef SHELL_DT_DEV_NAME
#undef SHELL_DT_SERVER_PDEV
#undef SHELL_DT_CALLBACK_PDEV


#elif !defined(_add_static_device_step2)
#define _add_static_device_step2

#undef _add_static_device_step1

#ifndef SHELL_DT_DEV_NAME
#error "SHELL_DT_DEV_NAME should be defined"
#endif

#ifndef SHELL_DT_SERVER_PDEV
#error "SHELL_DT_SERVER_PDEV should be defined"
#endif

#ifndef SHELL_DT_CALLBACK_PDEV
#error "SHELL_DT_CALLBACK_PDEV should be defined"
#endif

extern uint8_t shell_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);
extern uint8_t shell_callback(void * const aHandle ,
		const uint8_t aCallback_num , void * aCallback_param1, void * aCallback_param2);

#include "src/shell.h"

EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SHELL_DT_DEV_NAME) ;
EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SHELL_DT_SERVER_PDEV) ;
EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SHELL_DT_CALLBACK_PDEV) ;
shell_instance_t STATIC_DEVICE_INNER_INST(SHELL_DT_DEV_NAME) =
	{P_TO_STATIC_DEVICE_INST(SHELL_DT_SERVER_PDEV) , P_TO_STATIC_DEVICE_INST(SHELL_DT_DEV_NAME) ,
	P_TO_STATIC_DEVICE_INST(SHELL_DT_CALLBACK_PDEV) , 0 , 0};

CREATE_STATIC_DEVICE(SHELL_DT_DEV_NAME ,
	&STATIC_DEVICE_INNER_INST(SHELL_DT_DEV_NAME)  ,
	shell_ioctl , 		DEV_API_dummy_pwrite_func ,
	DEV_API_dummy_pread_func , shell_callback);



#undef CURRENT_DEV

#endif
