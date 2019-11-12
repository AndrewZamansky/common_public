/*
 * file : u_boot_shell.h
 *
 *
 */

#ifndef _U_BOOT_SHELL_H
#define _U_BOOT_SHELL_H

#include "os_wrapper.h"

#define _UBOOT_SHELL_API_VER_VARIABLE(ver)  uboot_shell_API_ver_##ver
#define UBOOT_SHELL_API_VER_VARIABLE(ver)  _UBOOT_SHELL_API_VER_VARIABLE(ver)

struct u_boot_shell_runtime_t {
	os_mutex_t  mutex;
};
SET_RUNTIME_DATA_TYPE(u_boot_shell, struct u_boot_shell_runtime_t);

#endif
