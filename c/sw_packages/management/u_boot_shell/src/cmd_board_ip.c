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
 * Logbuffer handling routines
 */
#include "_project.h"

#include "dev_management_api.h" // for device manager defines and typedefs

#include <u-boot/include/command.h>
#include "shell_api.h"
#include "ESP8266_api.h"



/*
 * Subroutine:  do_brd_ip
 *
 * Description:
 *
 * Inputs:
 *
 * Return:      None
 *
 */
int do_brd_ip (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	pdev_descriptor_t ESP8266_dev;

	uint8_t ip_str[17];

	ESP8266_dev=DEV_API_open_device((uint8_t*)"wifi1");
	if(0 == DEV_IOCTL(ESP8266_dev,IOCTL_ESP8266_GET_IP , ip_str))
	{
		SHELL_REPLY_STR(ip_str);
		return 0;
	}

	SHELL_REPLY_STR("no_ip");
	return 0;
}

U_BOOT_CMD(
	brd_ip,     255,	0,	do_brd_ip,
	"board info",
	"info   - \n"
);
