#ifndef _add_static_device_step1
#define _add_static_device_step1

#include  "u_boot_shell_api.h"

#undef _add_static_device_step2

#undef U_BOOT_SHELL_DT_DEV_NAME
#undef U_BOOT_SHELL_DT_SERVER_PDEV


#elif !defined(_add_static_device_step2)
#define _add_static_device_step2

#undef _add_static_device_step1

#ifndef U_BOOT_SHELL_DT_DEV_NAME
#error "U_BOOT_SHELL_DT_DEV_NAME should be defined"
#endif

#ifndef U_BOOT_SHELL_DT_SERVER_PDEV
#error "U_BOOT_SHELL_DT_SERVER_PDEV should be defined"
#endif

extern uint8_t u_boot_shell_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);
extern uint8_t u_boot_shell_callback(void * const aHandle ,
		const uint8_t aCallback_num , void * aCallback_param1, void * aCallback_param2);

#include "src/u_boot_shell.h"

EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(U_BOOT_SHELL_DT_DEV_NAME) ;
EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(U_BOOT_SHELL_DT_SERVER_PDEV) ;
u_boot_shell_instance_t STATIC_DEVICE_INNER_INST(U_BOOT_SHELL_DT_DEV_NAME) =
	{
			P_TO_STATIC_DEVICE_INST(U_BOOT_SHELL_DT_SERVER_PDEV) ,
			P_TO_STATIC_DEVICE_INST(U_BOOT_SHELL_DT_DEV_NAME)
	};
CREATE_STATIC_DEVICE(U_BOOT_SHELL_DT_DEV_NAME ,
	&STATIC_DEVICE_INNER_INST(U_BOOT_SHELL_DT_DEV_NAME)  ,
	u_boot_shell_ioctl , 		DEV_API_dummy_pwrite_func ,
	DEV_API_dummy_pread_func , u_boot_shell_callback);


#undef CURRENT_DEV

#endif
