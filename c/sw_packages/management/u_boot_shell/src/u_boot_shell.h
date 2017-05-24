/*
 * file : u_boot_shell.h
 *
 *
 */

#ifndef _U_BOOT_SHELL_H
#define _U_BOOT_SHELL_H

#include "dev_management_api.h"


/***************   typedefs    *******************/

struct u_boot_shell_instance_t{
	struct dev_desc_t * server_dev;
};

#endif
