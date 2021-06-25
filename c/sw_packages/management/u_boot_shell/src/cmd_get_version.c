/*
 *  cmd_get_dev_param.c
 */
#include "_project.h"

#include "u-boot/include/command.h"
#include "u_boot_shell_api.h"
#include "version_management_api.h"
#include "dev_management_api.h"

#define MAX_RET_BUFF_SIZE 16

#if !defined(CONFIG_USE_DEVICE_NAME_STRINGS)
	extern struct dev_desc_t * ver_dev;
#endif

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
static int do_get_version(
		cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct dev_desc_t * dev;
	uint8_t ret_buff[MAX_RET_BUFF_SIZE + 1];
	size_t read_count;
	size_t last_read_pos;

#if !defined(CONFIG_USE_DEVICE_NAME_STRINGS)
	dev = ver_dev;
#else
	dev = DEV_OPEN("ver_dev");
#endif

	last_read_pos = 0;
	read_count = 1;
	while (read_count)
	{
		read_count = DEV_PREAD(dev, ret_buff, MAX_RET_BUFF_SIZE, last_read_pos);
		last_read_pos += read_count;
		SHELL_REPLY_DATA(ret_buff, read_count);
	}
	SHELL_REPLY_STR("\r\n");
	return 0;
}

U_BOOT_CMD(
	get_version, 255, 0, do_get_version,
	"",
	"info   - \n"
);

