/*
 * cmd_echo.c
 */

#include "_project.h"


#include <u-boot/include/command.h>
#include "u_boot_shell_api.h"


/*
 * Subroutine:  do_echo
 *
 * Description:
 *
 * Inputs:
 *
 * Return:      None
 *
 */
static int do_echo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	uint8_t i;

	for (i = 1; i < (argc - 1); i++)
	{
		SHELL_REPLY_STR(argv[i]);
		SHELL_REPLY_STR(" ");
	}
	if (1 < argc) SHELL_REPLY_STR(argv[argc - 1]);

	return 0;
}

U_BOOT_CMD(
	echo, 255, 0, do_echo,
	"",
	"info   - \n"
);

