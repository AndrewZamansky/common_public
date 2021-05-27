/*
 *  cmd_get_dev_param.c
 */
#include "_project.h"
#include "dev_management_config.h"


#include <u-boot/include/command.h>
#include "u_boot_shell_api.h"

#define MAX_RET_BUFF_SIZE 31

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
int do_get_dev_param (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
#if 0 // TODO
	struct dev_desc_t * dev_descriptor;
	uint32_t retVal;
	const struct dev_param_t *pIoctlParam;
	uint8_t ret_buff[MAX_RET_BUFF_SIZE+1];

	uint8_t DEV_SET_PARAM(char *dev_name_str , char *param_name_str , char *param_val_str)

	retVal= get_dev_ioctl((uint8_t*)argv[1] , &dev_descriptor ,(uint8_t*)argv[2] , &pIoctlParam);

	if(1 == retVal )
	{
		SHELL_REPLY_STR("unknown dev");
	}
	else if(2 == retVal )
	{
		SHELL_REPLY_STR("unknown param");
	}
	else
	{
		DEV_IOCTL(dev_descriptor, pIoctlParam->paramGetIoctl ,  ret_buff);

		ret_buff[MAX_RET_BUFF_SIZE] = 0;
		SHELL_REPLY_STR(ret_buff);
	}
#endif
	return 0;
}

U_BOOT_CMD(
	get_dev_param,     255,	0,	do_get_dev_param,
	"set dev param",
	"info   - \n"
);

