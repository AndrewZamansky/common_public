/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "u_boot_shell_api.h"
#include "src/u_boot_shell.h"


#define	MODULE_NAME							u_boot_shell
#define	MODULE_IOCTL_FUNCTION				u_boot_shell_ioctl
#define	MODULE_CALLBACK_FUNCTION			u_boot_shell_callback
#define MODULE_CONFIG_DATA_STRUCT_TYPE		struct u_boot_shell_instance_t

#ifdef DT_DEV_MODULE

	#ifndef U_BOOT_SHELL_DT_SERVER_PDEV
		#error "U_BOOT_SHELL_DT_SERVER_PDEV should be defined"
	#endif

	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(U_BOOT_SHELL_DT_SERVER_PDEV) ;
	#define STATIC_DEV_DATA_STRUCT										\
		{																\
				P_TO_STATIC_DEVICE_INST(U_BOOT_SHELL_DT_SERVER_PDEV) ,	\
		};

#endif

#define MODULE_CONFIGURABLE_PARAMS_ARRAY	{	\
			{"u_boot_server" 	, IOCTL_SET_SERVER_DEVICE 	, IOCTL_VOID , \
				DEV_PARAM_TYPE_PDEVICE , MAPPED_SET_DUMMY_PARAM() },		\
		}


#include "add_component.h"

/**
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef U_BOOT_SHELL_DT_SERVER_PDEV
