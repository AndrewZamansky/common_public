/*
 *  cmd_bin.c
 */

#include "_project.h"

#include <u-boot/include/command.h>
#include "u_boot_shell_api.h"
#include "shell_frontend_api.h"


/*
 * Subroutine:  do_heap_test
 * Description:
 * Inputs:
 * Return:      None
 *
 */
static int do_bin(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	shell_frontend_set_mode(SHELL_FRONTEND_MODE_BINARY);
	return 0;
}

U_BOOT_CMD(
	bin, 255, 0, do_bin,
	"",
	"info   - \n"
);

