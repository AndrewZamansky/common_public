/*
 * uboot.c
 *
 *  Created on: Sep 21, 2015
 *      Author: azamansk
 */


#include "config.h"
#include "command.h"

#ifdef  _BOOTSTAGE_H
	typedef enclosure_unnedded _code_by_DONT_USE_STD_IO_in_common_h_in_uboor_include_dir  dummy_type;
#endif

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
