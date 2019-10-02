/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "shell_api.h"
#include  "src/shell.h"

#define MODULE_CONFIG_DATA_STRUCT_TYPE   struct shell_cfg_t
#define MODULE_RUNTIME_DATA_STRUCT_TYPE  struct shell_runtime_instance_t


#ifndef SHELL_DT_TX_SERVER_PDEV
	#error "SHELL_DT_TX_SERVER_PDEV should be defined"
#endif

#ifndef SHELL_DT_RX_SERVER_PDEV
	#error "SHELL_DT_RX_SERVER_PDEV should be defined"
#endif

#ifndef SHELL_DT_CALLBACK_PDEV
	#define _CALLBACK_PDEV  NULL
#else
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SHELL_DT_CALLBACK_PDEV) ;
	#define _CALLBACK_PDEV  P_TO_STATIC_DEVICE_INST(SHELL_DT_CALLBACK_PDEV)
#endif


#ifndef SHELL_DT_CMD_SAVE_PDEV
	#define _CMD_SAVE_PDEV	NULL
#else
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SHELL_DT_CMD_SAVE_PDEV) ;
	#define _CMD_SAVE_PDEV  P_TO_STATIC_DEVICE_INST(SHELL_DT_CMD_SAVE_PDEV)
#endif

EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SHELL_DT_TX_SERVER_PDEV);
EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SHELL_DT_RX_SERVER_PDEV);
#define STATIC_DEV_DATA_STRUCT      \
	{                               \
		P_TO_STATIC_DEVICE_INST(SHELL_DT_TX_SERVER_PDEV) ,  \
		P_TO_STATIC_DEVICE_INST(SHELL_DT_RX_SERVER_PDEV) ,  \
		_CALLBACK_PDEV,         \
		_CMD_SAVE_PDEV          \
	}

#include "add_static_dev.h"

/*
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef SHELL_DT_TX_SERVER_PDEV
#undef SHELL_DT_RX_SERVER_PDEV
#undef SHELL_DT_CALLBACK_PDEV
#undef SHELL_DT_CMD_SAVE_PDEV
#undef _CALLBACK_PDEV
#undef _CMD_SAVE_PDEV
