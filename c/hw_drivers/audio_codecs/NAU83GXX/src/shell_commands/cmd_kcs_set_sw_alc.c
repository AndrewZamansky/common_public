/*
 *  cmd_kcs_set_sw_alc.c
 */

#include "audio_path_api.h"
#include "_project.h"

#include "u-boot/include/command.h"
#include "u_boot_shell_api.h"

#include "os_wrapper.h"
#include "NAU83GXX_api.h"


int do_kcs_set_sw_alc(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct kcs_cmd_set_alc_data_ioctl_t  kcs_cmd_set_alc_data_ioctl;
	struct dev_desc_t *kcs_dev;
	uint32_t status = 0;
	int rc = NAU83GXX_RC_OK;

	/*
	 * Arguments:
	 *     [0] Command
	 *     [1] Device name
	 */
	if(argc != 11)
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

	kcs_cmd_set_alc_data_ioctl.alc_enable = (uint8_t)atoi(argv[2]);
	kcs_cmd_set_alc_data_ioctl.alc_segment_num = (uint8_t)atoi(argv[3]);
	kcs_cmd_set_alc_data_ioctl.max_battery_level = (float)atof(argv[4]);
	kcs_cmd_set_alc_data_ioctl.min_battery_level_V = (float)atof(argv[5]);
	kcs_cmd_set_alc_data_ioctl.battery_change_step_V = (float)atof(argv[6]);
	kcs_cmd_set_alc_data_ioctl.max_kcs_gain = (float)atof(argv[7]);
	kcs_cmd_set_alc_data_ioctl.min_kcs_gain = (float)atof(argv[8]);
	kcs_cmd_set_alc_data_ioctl.max_limiter_voltage = (float)atof(argv[9]);
	kcs_cmd_set_alc_data_ioctl.min_limiter_voltage = (float)atof(argv[10]);

	rc = DEV_IOCTL(
			kcs_dev, IOCTL_KCS_SET_ALC_DATA, &kcs_cmd_set_alc_data_ioctl);

end:
	SHELL_REPLY_PRINTF("rc%02X\r\n", rc);

	if(NAU83GXX_RC_OK == rc)
	{
		SHELL_REPLY_PRINTF("%08X\r\n", status);
	}

	return 0;
}

U_BOOT_CMD(
    kcs_set_sw_alc, 255, 0, do_kcs_set_sw_alc,
	"",
	""
);
