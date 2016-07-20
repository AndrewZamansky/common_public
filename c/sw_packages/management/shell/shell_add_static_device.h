/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "shell_api.h"


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

EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(DT_DEV_NAME) ;
EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SHELL_DT_SERVER_PDEV) ;
EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SHELL_DT_CALLBACK_PDEV) ;
#define STATIC_DEV_DATA_STRUCT_TYPE	shell_instance_t
#define STATIC_DEV_DATA_STRUCT									\
	{															\
		P_TO_STATIC_DEVICE_INST(SHELL_DT_SERVER_PDEV) , \
		P_TO_STATIC_DEVICE_INST(DT_DEV_NAME) ,		\
		P_TO_STATIC_DEVICE_INST(SHELL_DT_CALLBACK_PDEV) , 0 , 0	\
	}


#define	STATIC_DEV_IOCTL_FUNCTION		shell_ioctl
#define	STATIC_DEV_CALLBACK_FUNCTION	shell_callback
#include "add_static_dev.h"

#undef SHELL_DT_SERVER_PDEV
#undef SHELL_DT_CALLBACK_PDEV

