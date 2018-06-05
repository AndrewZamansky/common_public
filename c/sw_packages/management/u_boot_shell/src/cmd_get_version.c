/*
 *  cmd_get_dev_param.c
 */
#include "_project.h"

#include "u-boot/include/command.h"
#include "u_boot_shell_api.h"
#include "version_management_api.h"

#define MAX_RET_BUFF_SIZE 64
/*
 * Subroutine:  do_get_version
 *
 * Description:
 *
 * Inputs:
 *
 * Return:      None
 *
 */
int do_get_version (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct dev_desc_t * dev;
	uint8_t ret_buff[MAX_RET_BUFF_SIZE+1];

	dev = DEV_OPEN("ver_dev");
	DEV_READ(dev,ret_buff,MAX_RET_BUFF_SIZE);
	SHELL_REPLY_STR(ret_buff);
	SHELL_REPLY_STR("\r\n");


	return 0;
}

U_BOOT_CMD(
	get_version,     255,	0,	do_get_version,
	"",
	"info   - \n"
);

