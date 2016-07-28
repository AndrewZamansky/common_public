/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "shell_api.h"
#include "src/shell.h"

extern uint8_t shell_ioctl( pdev_descriptor_t apdev ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);
extern uint8_t shell_callback(pdev_descriptor_t apdev ,
		const uint8_t aCallback_num , void * aCallback_param1, void * aCallback_param2);

#define	MODULE_NAME					shell
#define	MODULE_IOCTL_FUNCTION		shell_ioctl
#define	MODULE_CALLBACK_FUNCTION	shell_callback
#define MODULE_DATA_STRUCT_TYPE		shell_instance_t

#ifdef DT_DEV_MODULE

	#ifndef SHELL_DT_SERVER_PDEV
		#error "SHELL_DT_SERVER_PDEV should be defined"
	#endif

	#ifndef SHELL_DT_CALLBACK_PDEV
		#error "SHELL_DT_CALLBACK_PDEV should be defined"
	#endif


	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SHELL_DT_SERVER_PDEV) ;
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SHELL_DT_CALLBACK_PDEV) ;
	#define STATIC_DEV_DATA_STRUCT									\
		{															\
			P_TO_STATIC_DEVICE_INST(SHELL_DT_SERVER_PDEV) , \
			P_TO_STATIC_DEVICE_INST(SHELL_DT_CALLBACK_PDEV) , 0 , 0	\
		}

#endif

#define MODULE_CONFIGURABLE_PARAMS_ARRAY	{	\
		{IOCTL_SET_SERVER_DEVICE_BY_NAME , IOCTL_VOID , SHELL_API_SERVER_DEVICE_STR, NOT_FOR_SAVE},\
	}

#include "add_component.h"

/* device specific defines should be undefined after calling #include "add_component.h" */
#undef SHELL_DT_SERVER_PDEV
#undef SHELL_DT_CALLBACK_PDEV

