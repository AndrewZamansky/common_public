/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include "u_boot_shell_api.h"
#include "src/u_boot_shell.h"

#define MODULE_HAS_NO_CONFIG_DATA
#define MODULE_HAS_NO_RUNTIME_DATA

#include "add_static_dev.h"

/**
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef U_BOOT_SHELL_DT_SHELL_FRONTEND_PDEV
#undef POINTER_TO_SHELL_FRONTEND_PDEV
