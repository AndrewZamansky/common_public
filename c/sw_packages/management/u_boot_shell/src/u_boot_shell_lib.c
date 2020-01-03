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

#include <stdarg.h>

extern void shell_reply_data_collection(uint8_t *data, size_t len);

// following variable required by uboot source:
char console_buffer_lib[1];

void clear_ctrlc_lib (void)
{

}

int had_ctrlc_lib (void)
{
	return 0;
}

int ctrlc_lib()
{
	return 0;
}

ulong get_timer_lib(ulong base)
{
	return 0;
}

int cli_readline_lib(const char *const prompt)
{
	return 0;
}


/**
 * SHELL_REPLY_DATA()
 *
 * return:
 */
void SHELL_REPLY_DATA_LIB(uint8_t *data, size_t len)
{
	shell_reply_data_collection(data, len);
}


/**
 * SHELL_REPLY_STR()
 *
 * return:
 */
void SHELL_REPLY_STR_LIB(const char* str)
{
	SHELL_REPLY_DATA_LIB((uint8_t*)str , strlen(str));
}


#define  MAX_STR_LEN   64
static  char  printf_str[MAX_STR_LEN];

/**
 * SHELL_REPLY_PRINTF()
 *
 * return:
 */
void SHELL_REPLY_PRINTF_LIB(const char* Format, ...)
{
	va_list args;
	int retVal;

	va_start(args, Format);
	retVal = vsnprintf(
			(char*)printf_str, MAX_STR_LEN, Format, args);
	va_end(args);

	if (0 >= retVal)  return ;

	SHELL_REPLY_DATA_LIB((uint8_t*)printf_str, strlen(printf_str));
}

