/*
 *
 *   file  :  uboot_shell_api_check.c
 *
 */



/***************   includes    *******************/


#include "_project_typedefs.h"
#include "_project_defines.h"

#include "u_boot_shell_api.h"
#include "u_boot_shell.h"

/* will create pointer to variable uboot_shell_API_ver_XXXXXXX.
 * during the linkage, if uboot_shell_ver_XXXXXXX will not be found then
 * it can mean that API and implementation is not compatible
*/
#define _VERSION_TEST_NAME(proj_name)  uboot_shell_API_test_##proj_name
#define VERSION_TEST_NAME(proj_name)  _VERSION_TEST_NAME(proj_name)

extern const uint8_t UBOOT_SHELL_API_VER_VARIABLE(UBOOT_SHELL_API_VERSION);
const uint8_t * const __attribute__((used))
	__attribute__((section(".const_variables_to_keep")))
	VERSION_TEST_NAME(UBOOT_SHELL_PROJ_NAME) =
				&UBOOT_SHELL_API_VER_VARIABLE(UBOOT_SHELL_API_VERSION);
