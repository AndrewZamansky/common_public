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
 * Comments:
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
 *  cmd_get_serial.c
 */
#include "_project.h"

#include "dev_management_api.h" // for device manager defines and typedefs

#include <u-boot/include/command.h>
#include "u_boot_shell_api.h"
#include "ff.h"

#define MAX_RET_BUFF_SIZE 32

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
int do_cat (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	FIL fp;
	uint32_t file_size ;
	uint8_t ret_buff[MAX_RET_BUFF_SIZE];
	uint32_t bytes_was_read;

	if((argc < 2) || (FR_OK!=f_open(&fp , (char*)argv[1] , FA_READ)) )
	{
		SHELL_REPLY_STR("unknown file");
	}
	else
	{
		file_size=fp.fsize;
		while(file_size)
		{
			f_read(&fp,ret_buff, MAX_RET_BUFF_SIZE , (UINT*)&bytes_was_read);
			SHELL_REPLY_DATA(ret_buff,bytes_was_read);
			file_size -= bytes_was_read;
		}
	}
	f_close(&fp);

	return 0;
}

U_BOOT_CMD(
	cat,     255,	0,	do_cat,
	"",
	"info   - \n"
);

