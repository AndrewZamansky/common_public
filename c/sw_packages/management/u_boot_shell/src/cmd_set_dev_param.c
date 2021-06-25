/*
 * cmd_set_dev_param.c
 */
#include "_project.h"


#include <u-boot/include/command.h>
#include "u_boot_shell_api.h"

/*
 * Subroutine:  do_set_dev_param
 *
 * Description:
 *
 * Inputs:
 *
 * Return:      None
 *
 */
static int do_set_dev_param(
		cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
#if 0 // TODO
	struct dev_desc_t * dev_descriptor;
	uint32_t retVal;
	const struct dev_param_t *pIoctlParam;

	if(3 > argc)
	{
		SHELL_REPLY_STR("cmd error");
		return 0;
	}


	retVal= get_dev_ioctl((uint8_t*)argv[1] , &dev_descriptor ,(uint8_t*)argv[2] ,  &pIoctlParam);

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
		DEV_IOCTL(dev_descriptor, pIoctlParam->paramSetIoctl , (4 > argc) ? 0: argv[3]);
		SHELL_REPLY_STR("Done");
	}
#endif
	return 0;
}

U_BOOT_CMD(
	set_dev_param, 255, 0, do_set_dev_param,
	"set dev param",
	"info   - \n"
);

