/*
 *  cmd_kcs_init_nau83gxx_chip.c
 */

#include "audio_path_api.h"
#include "_project.h"

#include "u-boot/include/command.h"
#include "u_boot_shell_api.h"
#include "NAU83GXX_api.h"

#include "../NAU83GXX.h"
#include "os_wrapper.h"


/*
 * Subroutine:  do_kcs_init_nau83gxx_chip
 *
 * Description:
 *
 * Inputs:
 *
 * Return:      None
 *
 */
int do_kcs_init_nau83gxx_chip(
		cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct dev_desc_t *kcs_dev;
	uint8_t rc = 0;

	if(argc != 2)
	{
		rc = NAU83GXX_RC_SYNTAX_ERROR;
		goto end;
	}

	kcs_dev = DEV_OPEN(argv[1]);

	if (NULL == kcs_dev)
	{
		rc = NAU83GXX_RC_DEVICE_DOES_NOT_EXIST;
		goto end;
	}

	rc = DEV_IOCTL(kcs_dev, IOCTL_KCS_REINIT_I2C_REGISTERS);

	end:
	SHELL_REPLY_PRINTF("rc%02X\r\n", rc);

	return 0;
}

U_BOOT_CMD(
	kcs_init_nau83gxx_chip, 255, 0, do_kcs_init_nau83gxx_chip,
	"",
	"info   - \n"
);
