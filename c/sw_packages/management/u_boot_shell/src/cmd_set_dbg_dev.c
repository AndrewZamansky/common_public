/*
 *  cmd_set_dbg_dev.c
 */
#include "_project.h"

#include "dev_management_api.h"

#include <u-boot/include/command.h>
#include "u_boot_shell_api.h"
#include "PRINTF_api.h"

extern struct dev_desc_t * gCurrReplyDev;


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
static int do_debug_dev(
		cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct dev_desc_t * dev;
	if(argc < 2) return CMD_RET_USAGE;

	if(3 == argc)
	{
		dev = DEV_OPEN(argv[2]);
		if(NULL == dev)
		{
			SHELL_REPLY_STR("unknown dev");
			return 0;
		}
	}
	else
	{
		dev = gCurrReplyDev;
	}

	if(0 == memcmp(argv[1], "-r", 2))
	{
		PRINTF_API_RemoveDebugOutput(dev);
	}
	else if(0 == memcmp(argv[1], "-a", 2))
	{
		PRINTF_API_AddDebugOutput(dev);

	}
	else
	{
		SHELL_REPLY_STR("unknown switch");
	}

	return 0;
}

U_BOOT_CMD(
	set_dbg_device, 255, 0, do_debug_dev,
	"",
	"info   - \n"
);

