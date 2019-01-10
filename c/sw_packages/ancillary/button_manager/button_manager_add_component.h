/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "button_manager_api.h"
#include  "src/button_manager.h"


#define MODULE_NAME                     button_manager
#define MODULE_IOCTL_FUNCTION           button_manager_ioctl
#define MODULE_CALLBACK_FUNCTION        button_manager_callback
#define MODULE_CONFIG_DATA_STRUCT_TYPE  struct button_manager_config_t
#define MODULE_RUNTIME_DATA_STRUCT_TYPE struct button_manager_runtime_t


#ifdef DT_DEV_MODULE

	#ifndef BUTTON_MANAGER_CLIENT_DEV
		#error "BUTTON_MANAGER_CLIENT_DEV should be defined"
	#endif
	#define POINTER_TO_BUTTON_MANAGER_CLIENT_DEV     \
			P_TO_STATIC_DEVICE_INST(BUTTON_MANAGER_CLIENT_DEV)


	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(BUTTON_MANAGER_CLIENT_DEV);
	#define STATIC_DEV_DATA_STRUCT                \
		{                                         \
			POINTER_TO_BUTTON_MANAGER_CLIENT_DEV, \
			NULL,                                 \
			NULL,                                 \
			0,                                    \
			0                                     \
		}
#endif

#include "add_component.h"

#undef  BUTTON_MANAGER_CLIENT_DEV
#undef  POINTER_TO_BUTTON_MANAGER_CLIENT_DEV
