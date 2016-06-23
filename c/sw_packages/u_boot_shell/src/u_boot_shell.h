/*
 * file : u_boot_shell.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _U_BOOT_SHELL_H
#define _U_BOOT_SHELL_H

#include "src/_u_boot_shell_prerequirements_check.h"


/***************   typedefs    *******************/

typedef struct {
	pdev_descriptor_const server_dev;
	pdev_descriptor_const this_dev;
} u_boot_shell_instance_t;

#endif /* */