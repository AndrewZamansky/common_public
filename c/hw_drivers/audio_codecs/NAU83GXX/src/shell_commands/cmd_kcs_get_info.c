/*
 *  cmd_kcs_get_info.c
 */

#include "audio_path_api.h"
#include "_project.h"

#include "u-boot/include/command.h"
#include "u_boot_shell_api.h"

#include "os_wrapper.h"
#include "NAU83GXX_api.h"

#include "CoreModules/KCS_RemoteInterface.h"

int do_kcs_get_info(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct dev_desc_t *kcs_dev;
	int rc = NAU83GXX_RC_OK;
	Frac32 gain;
	int val;
	int kcs_ret;
	PseudoFloatSI u_out_limit;

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

	kcs_ret = kcs_remote_get_audio_gain(kcs_dev, &gain );
	if (0 == kcs_ret)
	{
		SHELL_REPLY_PRINTF("gain = %d\r\n", gain);
	}
	else
	{
		SHELL_REPLY_PRINTF("read gain error: %d\r\n", kcs_ret);
	}

	kcs_ret = kcs_remote_get_output_limiter(kcs_dev, &u_out_limit );
	if (0 == kcs_ret)
	{
		val = u_out_limit.value;
		SHELL_REPLY_PRINTF("u limit (value) =  %d\r\n", val);
		val = u_out_limit.shift;
		SHELL_REPLY_PRINTF("u limit (shift) =  %d\r\n", val);
	}
	else
	{
		SHELL_REPLY_PRINTF("read gain error: %d\r\n", kcs_ret);
	}

end:

	SHELL_REPLY_PRINTF("rc%02X\r\n", rc);

	return 0;
}

U_BOOT_CMD(
    kcs_get_info , 255, 0, do_kcs_get_info,
	"",
	""
);
