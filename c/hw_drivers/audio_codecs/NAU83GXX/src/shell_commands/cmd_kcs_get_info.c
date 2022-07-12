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
	float gain_f;
	float u_limit;
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

	if (gain & 0x8000000)
	{
		gain_f = - ((float)(0 - gain) / 0x80000000);
	}
	else
	{
		gain_f = (float)gain / 0x80000000;
	}

	gain_f = 20 * log10(gain_f / 0.03162);

	if (0 == kcs_ret)
	{
		SHELL_REPLY_PRINTF("gain = %d (%.2f)\r\n", gain, gain_f);
	}
	else
	{
		SHELL_REPLY_PRINTF("read gain error: %d\r\n", kcs_ret);
	}

	kcs_ret = kcs_remote_get_output_limiter(kcs_dev, &u_out_limit );

	val = u_out_limit.value;
	if (val & 0x800)
	{
		u_limit = - ((float)(0 - val) / 0x8000);
	}
	else
	{
		u_limit = (float)val / 0x8000;
	}
	if (0 == kcs_ret)
	{
		val = u_out_limit.value;
		SHELL_REPLY_PRINTF("u limit (value) =  %d\r\n", val);
		val = u_out_limit.shift;
		SHELL_REPLY_PRINTF("u limit (shift) =  %d\r\n", val);
		u_limit = u_limit * pow(2, val);
		SHELL_REPLY_PRINTF("u limit [SI unit] =  %.2f\r\n", u_limit);
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
