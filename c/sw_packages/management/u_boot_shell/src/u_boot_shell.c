/*
 *
 * file :   u_boot_shell.c
 *
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "u_boot_shell_api.h"

#include "u_boot_shell.h"
#include "config.h"
#include "management_api.h"

#ifdef  _BOOTSTAGE_H
	typedef enclosure_unnedded _code_by_DONT_USE_STD_IO_in_common_h_in_uboor_include_dir  dummy_type;
#endif

#include <stdio.h>
#include <stdarg.h>

/*following line add module to available module list for dynamic device tree*/
#include "u_boot_shell_add_component.h"


/********  defines *********************/



/********  types  *********************/



/********  externals *********************/

/* ----------- Exported variables ------------------------*/
struct dev_desc_t * gCurrReplyDev = NULL;

extern int run_command(const char *cmd, int flag);

/********  local defs *********************/

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
	if ((NULL != gCurrReplyDev) && (0 != len))
	{
		DEV_WRITE(gCurrReplyDev, data , len);
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
 * u_boot_shell_callback()
 *
 * return:
 */
uint8_t u_boot_shell_callback(
		struct dev_desc_t *adev, const uint8_t aCallback_num,
		void * aCallback_param1, void * aCallback_param2)
{
	if(CALLBACK_DATA_RECEIVED == aCallback_num)
	{
		char prev_eol;
		size_t eol_pos;
		char *pCmdStart;
		struct rcvd_cmd_t	*rcvd_cmd;

		rcvd_cmd = aCallback_param1;
		gCurrReplyDev = rcvd_cmd->reply_dev;
		pCmdStart =( char *)rcvd_cmd->cmd_buf;

		eol_pos = rcvd_cmd->cmd_len - 1;
		prev_eol = pCmdStart[eol_pos];
		pCmdStart[eol_pos] = '\0';
		run_command((const char *)pCmdStart, 0);
		gCurrReplyDev = NULL;
		pCmdStart[eol_pos] = prev_eol;
	}
	return 0;
}


/**
 * u_boot_shell_ioctl()
 *
 * return:
 */
uint8_t u_boot_shell_ioctl( struct dev_desc_t *adev ,
		const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	struct u_boot_shell_instance_t *config_handle;
	struct dev_desc_t *   server_dev ;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);

	switch(aIoctl_num)
	{
#ifdef CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION
	case IOCTL_SET_SERVER_DEVICE :
		{
			server_dev = (struct dev_desc_t *)aIoctl_param1;
			if(NULL != server_dev)
			{
				DEV_IOCTL(server_dev, IOCTL_SET_ISR_CALLBACK_DEV, (void*)adev);
			}

			config_handle->server_dev=server_dev;
		}
		break;
#endif
	case IOCTL_DEVICE_START :
		server_dev = config_handle->server_dev;
		DEV_IOCTL_0_PARAMS(server_dev , IOCTL_DEVICE_START );

		break;

	default :
		return 1;
	}
	return 0;
}
