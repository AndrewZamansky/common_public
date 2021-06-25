/*
 * cmd_board_name.c
 */

#include "_project.h"

#include "dev_management_api.h" // for device manager defines and typedefs

#include <u-boot/include/command.h>
#include "u_boot_shell_api.h"


/*
 * Subroutine:  do_brd_name
 *
 * Description:
 *
 * Inputs:
 *
 * Return:      None
 *
 */
static int do_brd_name (
		cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	//PRINT_STR_NOTE("board 001 \n");
	SHELL_REPLY_STR("board_001 \n");
	return 0;
}

U_BOOT_CMD(
	brd_name, 255, 0, do_brd_name,
	"board info",
	"info   - \n"
);

