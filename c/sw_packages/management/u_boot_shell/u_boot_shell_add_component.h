/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "u_boot_shell_api.h"
#include "src/u_boot_shell.h"

extern uint8_t u_boot_shell_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);
extern uint8_t u_boot_shell_callback(void * const aHandle ,
		const uint8_t aCallback_num , void * aCallback_param1, void * aCallback_param2);

#define	MODULE_NAME					u_boot_shell
#define	MODULE_IOCTL_FUNCTION		u_boot_shell_ioctl
#define	MODULE_CALLBACK_FUNCTION	u_boot_shell_callback
#define MODULE_DATA_STRUCT_TYPE		u_boot_shell_instance_t

#ifdef DT_DEV_MODULE

	#ifndef U_BOOT_SHELL_DT_SERVER_PDEV
		#error "U_BOOT_SHELL_DT_SERVER_PDEV should be defined"
	#endif

	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(DT_DEV_NAME) ;
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(U_BOOT_SHELL_DT_SERVER_PDEV) ;
	#define STATIC_DEV_DATA_STRUCT										\
		{																\
				P_TO_STATIC_DEVICE_INST(U_BOOT_SHELL_DT_SERVER_PDEV) ,	\
				P_TO_STATIC_DEVICE_INST(DT_DEV_NAME)					\
		};

#endif

#define MODULE_CONFIGURABLE_PARAMS_ARRAY	{	\
		{IOCTL_SET_SERVER_DEVICE_BY_NAME , IOCTL_VOID , U_BOOT_SHELL_API_SERVER_DEVICE_STR, NOT_FOR_SAVE},\
	}

#include "add_component.h"


#undef U_BOOT_SHELL_DT_SERVER_PDEV
