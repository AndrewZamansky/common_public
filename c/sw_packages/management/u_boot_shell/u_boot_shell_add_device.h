/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include "u_boot_shell_api.h"
#include "src/u_boot_shell.h"


#ifdef U_BOOT_SHELL_DT_SHELL_FRONTEND_PDEV
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(
				U_BOOT_SHELL_DT_SHELL_FRONTEND_PDEV);
	#define POINTER_TO_SHELL_FRONTEND_PDEV  \
					P_TO_STATIC_DEVICE_INST(U_BOOT_SHELL_DT_SHELL_FRONTEND_PDEV)
#else
	#define POINTER_TO_SHELL_FRONTEND_PDEV   NULL
#endif

SET_STATIC_DEV_CONFIG(u_boot_shell) =
{
	POINTER_TO_SHELL_FRONTEND_PDEV,
};

#include "add_static_dev.h"

/**
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef U_BOOT_SHELL_DT_SHELL_FRONTEND_PDEV
#undef POINTER_TO_SHELL_FRONTEND_PDEV
