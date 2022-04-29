/*
 *  cmd_kcs_set_send.c
 */

#include "audio_path_api.h"
#include "_project.h"

#include "u-boot/include/command.h"
#include "u_boot_shell_api.h"

#include "os_wrapper.h"
#include "NAU83GXX_api.h"

int do_kcs_set_send(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct dev_desc_t *kcs_dev;
	int rc = NAU83GXX_RC_OK;

	/*
	 * Arguments:
	 *     [0] Command
	 *     [1] Device name
	 */
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

	rc = DEV_IOCTL(kcs_dev, IOCTL_KCS_SEND_COLLECTED_SETUP_DATA);

end:
	SHELL_REPLY_PRINTF("rc%02X\r\n", rc);

	return 0;
}

U_BOOT_CMD(
    kcs_set_send , 255, 0, do_kcs_set_send,
	"[-device_address]\n\tSends buffer to DSP",
	"info - "
);
