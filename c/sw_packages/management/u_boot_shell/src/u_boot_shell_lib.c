/*
 *
 * file :   u_boot_shell_lib.c
 *
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"


#include "u_boot_shell_api.h"

#include "u_boot_shell.h"
#include "config.h"

// following variable required by uboot source:
char console_buffer[1];

void clear_ctrlc (void)
{

}

int had_ctrlc (void)
{
	return 0;
}

int ctrlc()
{
	return 0;
}

ulong get_timer(ulong base)
{
	return 0;
}

int cli_readline(const char *const prompt)
{
	return 0;
}
