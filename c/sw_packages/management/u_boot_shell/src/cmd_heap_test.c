/*
 *  cmd_heap_test.c
 */

#include "_project.h"

#include <u-boot/include/command.h>
#include "u_boot_shell_api.h"

extern size_t get_heap_left(void);


/*
 * Subroutine:  do_heap_test
 * Description:
 * Inputs:
 * Return:      None
 *
 */
static int do_heap_test(
		cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	size_t  heap_left;

	heap_left = get_heap_left();
	SHELL_REPLY_PRINTF("heap left : 0x%x\n", heap_left);
	return 0;
}

U_BOOT_CMD(
	heap_test, 255, 0, do_heap_test,
	"",
	"info   - \n"
);

