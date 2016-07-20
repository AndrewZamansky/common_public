/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "u_boot_shell_api.h"


#ifndef U_BOOT_SHELL_DT_SERVER_PDEV
#error "U_BOOT_SHELL_DT_SERVER_PDEV should be defined"
#endif

extern uint8_t u_boot_shell_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);
extern uint8_t u_boot_shell_callback(void * const aHandle ,
		const uint8_t aCallback_num , void * aCallback_param1, void * aCallback_param2);

#include "src/u_boot_shell.h"

EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(DT_DEV_NAME) ;
EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(U_BOOT_SHELL_DT_SERVER_PDEV) ;
#define STATIC_DEV_DATA_STRUCT_TYPE	u_boot_shell_instance_t
#define STATIC_DEV_DATA_STRUCT										\
	{																\
			P_TO_STATIC_DEVICE_INST(U_BOOT_SHELL_DT_SERVER_PDEV) ,	\
			P_TO_STATIC_DEVICE_INST(DT_DEV_NAME)					\
	};

#define	STATIC_DEV_IOCTL_FUNCTION		u_boot_shell_ioctl
#define	STATIC_DEV_CALLBACK_FUNCTION	u_boot_shell_callback
#include "add_static_dev.h"


#undef U_BOOT_SHELL_DT_SERVER_PDEV
