/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include "u_boot_shell_api.h"
#include "src/u_boot_shell.h"


#define MODULE_CONFIG_DATA_STRUCT_TYPE  struct u_boot_shell_instance_t


#ifdef U_BOOT_SHELL_DT_SERVER_PDEV
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(U_BOOT_SHELL_DT_SERVER_PDEV);
	#define POINTER_TO_SERVER_PDEV  \
					P_TO_STATIC_DEVICE_INST(U_BOOT_SHELL_DT_SERVER_PDEV)
#else
	#define POINTER_TO_SERVER_PDEV   NULL
#endif

#define STATIC_DEV_DATA_STRUCT    \
	{                             \
		POINTER_TO_SERVER_PDEV ,  \
	};



#include "add_static_dev.h"

/**
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef U_BOOT_SHELL_DT_SERVER_PDEV
#undef POINTER_TO_SERVER_PDEV
