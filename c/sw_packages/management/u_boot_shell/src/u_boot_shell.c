/*
 *
 * file :   u_boot_shell.c
 *
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "u_boot_shell_api.h"

#include "u_boot_shell.h"
#include "config.h"
#include "management_api.h"

#ifdef  _BOOTSTAGE_H
	typedef enclosure_unneeded _code_by_DONT_USE_STD_IO_in_common_h_in_uboor_include_dir  dummy_type;
#endif

#include <stdio.h>
#include <stdarg.h>
#include "os_wrapper.h"

static 	os_mutex_t  u_boot_shell_mutex = NULL;
struct dev_desc_t * gCurrReplyDev = NULL;

extern int run_command(const char *cmd, int flag);


const uint8_t UBOOT_SHELL_API_VER_VARIABLE(UBOOT_SHELL_API_VERSION);

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


/**
 * SHELL_REPLY_DATA()
 *
 * return:
 */
void SHELL_REPLY_DATA(uint8_t *data, size_t len)
{
	size_t written_len;
	if (NULL != gCurrReplyDev)
	{
		while (len)
		{
			written_len = DEV_WRITE(gCurrReplyDev, data, len);
			len -= written_len;
			data += written_len;
		}
	}
}


/**
 * SHELL_REPLY_STR()
 *
 * return:
 */
void SHELL_REPLY_STR(const char* str)
{
	SHELL_REPLY_DATA((uint8_t*)str , strlen(str));
}


#define  MAX_STR_LEN   64
static  char  printf_str[MAX_STR_LEN];

/**
 * SHELL_REPLY_PRINTF()
 *
 * return:
 */
void SHELL_REPLY_PRINTF(const char* Format, ...)
{
	va_list args;
	int retVal;

	va_start(args, Format);
	retVal = vsnprintf(
			(char*)printf_str, MAX_STR_LEN, Format, args);
	va_end(args);

	if (0 >= retVal)  return ;

	SHELL_REPLY_DATA((uint8_t*)printf_str , strlen(printf_str));
}


/**
 * new_frame_received()
 *
 * return:
 */
static void new_frame_received(struct rcvd_cmd_t *rcvd_cmd)
{
	char prev_eol;
	size_t eol_pos;
	char *pCmdStart;

	gCurrReplyDev = rcvd_cmd->reply_dev;
	pCmdStart =( char *)rcvd_cmd->cmd_buf;

	eol_pos = rcvd_cmd->cmd_len - 1;
	prev_eol = pCmdStart[eol_pos];
	pCmdStart[eol_pos] = '\0';
	run_command((const char *)pCmdStart, 0);
	gCurrReplyDev = NULL;
	pCmdStart[eol_pos] = prev_eol;
}


/**
 * u_boot_shell_ioctl()
 *
 * return:
 */
static uint8_t u_boot_shell_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		if (NULL != u_boot_shell_mutex)
		{
			CRITICAL_ERROR("u_boot_shell can be only 1 instance");
		}
		u_boot_shell_mutex = os_create_mutex();
		break;
	case IOCTL_SHELL_NEW_FRAME_RECEIVED:
		if (NULL == u_boot_shell_mutex)
		{
			CRITICAL_ERROR("uboot shell not initialized yet");
		}
		os_mutex_take_infinite_wait(u_boot_shell_mutex);
		new_frame_received(aIoctl_param1);
		os_mutex_give(u_boot_shell_mutex);
		break;
	default :
		return 1;
	}
	return 0;
}

#define MODULE_NAME                     u_boot_shell
#define MODULE_IOCTL_FUNCTION           u_boot_shell_ioctl
#define MODULE_HAS_NO_RUNTIME_DATA
#define MODULE_HAS_NO_CONFIG_DATA
#include "add_module.h"
