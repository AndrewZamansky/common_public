/*
 *  cmd_cat.c
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

	if((argc < 2) || (FR_OK!=f_open(&fp , (char*)argv[1] , FA_READ)) )
	{
		SHELL_REPLY_STR("unknown file");
	}
	else
	{
		uint32_t file_size ;
		uint32_t bytes_was_read;
		uint8_t ret_buff[MAX_RET_BUFF_SIZE];

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

