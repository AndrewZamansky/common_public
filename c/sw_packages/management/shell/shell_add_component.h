/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "shell_api.h"
#include "src/shell.h"


#define	MODULE_NAME							shell
#define	MODULE_IOCTL_FUNCTION				shell_ioctl
#define	MODULE_CALLBACK_FUNCTION			shell_callback
#define MODULE_CONFIG_DATA_STRUCT_TYPE		struct shell_instance_t
#define MODULE_RUNTIME_DATA_STRUCT_TYPE		struct shell_runtime_instance_t

#ifdef DT_DEV_MODULE

	#ifndef SHELL_DT_TX_SERVER_PDEV
		#error "SHELL_DT_TX_SERVER_PDEV should be defined"
	#endif

	#ifndef SHELL_DT_RX_SERVER_PDEV
		#error "SHELL_DT_RX_SERVER_PDEV should be defined"
	#endif



	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SHELL_DT_TX_SERVER_PDEV) ;
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SHELL_DT_RX_SERVER_PDEV) ;
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SHELL_DT_CALLBACK_PDEV) ;
	#define STATIC_DEV_DATA_STRUCT									\
		{															\
			P_TO_STATIC_DEVICE_INST(SHELL_DT_TX_SERVER_PDEV) , 		\
			P_TO_STATIC_DEVICE_INST(SHELL_DT_RX_SERVER_PDEV) , 		\
			P_TO_STATIC_DEVICE_INST(SHELL_DT_CALLBACK_PDEV)			\
		}

#endif

#define MODULE_CONFIGURABLE_PARAMS_ARRAY	{	\
			{"shell_server" 	, IOCTL_SET_SERVER_DEVICE 	, IOCTL_VOID , \
				DEV_PARAM_TYPE_PDEVICE , MAPPED_SET_DUMMY_PARAM() }		   \
		}



#include "add_component.h"

/*
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef SHELL_DT_SERVER_PDEV
#undef SHELL_DT_CALLBACK_PDEV
