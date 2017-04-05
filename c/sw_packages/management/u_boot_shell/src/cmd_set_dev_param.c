/*
 * (C) Copyright 2002-2007
 * Detlev Zundel, DENX Software Engineering, dzu@denx.de.
 *
 * Code used from linux/kernel/printk.c
 * Copyright (C) 1991, 1992  Linus Torvalds
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * SHELLents:
 *
 * After relocating the code, the environment variable "loglevel" is
 * copied to console_loglevel.  The functionality is similar to the
 * handling in the Linux kernel, i.e. messages logged with a priority
 * less than console_loglevel are also output to stdout.
 *
 * If you want messages with the default level (e.g. POST messages) to
 * appear on stdout also, make sure the environment variable
 * "loglevel" is set at boot time to a number higher than
 * default_message_loglevel below.
 */

/*
 * cmd_set_dev_param.c
 */
#include "_project.h"


#include <u-boot/include/command.h>
#include "shell_api.h"

/*
 * Subroutine:  force_output
 *
 * Description:
 *
 * Inputs:
 *
 * Return:      None
 *
 */
int do_set_dev_param (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct dev_desc_t * dev_descriptor;
	uint32_t retVal;
	const dev_param_t *pIoctlParam;

	if(3 > argc)
	{
		SHELL_REPLY_STR("cmd error");
		return 0;
	}

#if 0 // TODO

	retVal= get_dev_ioctl((uint8_t*)argv[1] , &dev_descriptor ,(uint8_t*)argv[2] ,  &pIoctlParam);

	if(1 == retVal )
	{
		SHELL_REPLY_STR("unknown dev");
	}
	else if(2 == retVal )
	{
		SHELL_REPLY_STR("unknown param");
	}
	else
	{
		DEV_IOCTL(dev_descriptor, pIoctlParam->paramSetIoctl , (4 > argc) ? 0: argv[3]);
		SHELL_REPLY_STR("Done");
	}
#endif
	return 0;
}

U_BOOT_CMD(
	set_dev_param,     255,	0,	do_set_dev_param,
	"set dev param",
	"info   - \n"
);

