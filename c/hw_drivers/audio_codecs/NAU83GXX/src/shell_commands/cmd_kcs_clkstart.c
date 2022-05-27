/*
 *  cmd_kcs_clkstart.c
 */

#include "audio_path_api.h"
#include "_project.h"

#include "u-boot/include/command.h"
#include "u_boot_shell_api.h"

#include "os_wrapper.h"
#include "NAU83GXX_api.h"


int do_kcs_clkstart(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct kcs_simple_cmd_ioctl_t  kcs_simple_cmd_ioctl;
	struct dev_desc_t *kcs_dev;
	uint32_t status = 0;
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

	kcs_simple_cmd_ioctl.cmd = DSP_CMD_CLK_RESTART;
	kcs_simple_cmd_ioctl.p_recieved_U32_reply = &status;
	rc = DEV_IOCTL(kcs_dev, IOCTL_KCS_SIMPLE_CMD, &kcs_simple_cmd_ioctl);

end:
	SHELL_REPLY_PRINTF("rc%02X\r\n", rc);

	if(NAU83GXX_RC_OK == rc)
	{
		SHELL_REPLY_PRINTF("%08X\r\n", status);
	}

	return 0;
}

U_BOOT_CMD(
    kcs_clkstart, 255, 0, do_kcs_clkstart,
	"",
	""
);
