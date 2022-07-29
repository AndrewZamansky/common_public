/*
 *  cmd_kcs_write_i2c_reg.c
 */

#include "audio_path_api.h"
#include "_project.h"

#include "u-boot/include/command.h"
#include "u_boot_shell_api.h"

#include "os_wrapper.h"
#include "NAU83GXX_api.h"


int do_kcs_write_i2c_reg(
		cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct kcs_cmd_register_read_ioctl_t register_read_ioctl;
	struct dev_desc_t *kcs_dev;
	int rc = NAU83GXX_RC_OK;
	char *pEnd;
	uint32_t data;

	/*
	 * Arguments:
	 *     [0] Command
	 *     [1] Device name
	 *     [2] reg address
	 *     [3] data
	 */
	if(argc != 4)
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

	register_read_ioctl.reg_addr = strtol(argv[2], &pEnd, 0);
	register_read_ioctl.data = strtol(argv[3], &pEnd, 0);

	rc = DEV_IOCTL(kcs_dev, IOCTL_KCS_REGISTER_WRITE, &register_read_ioctl);

end:

	SHELL_REPLY_PRINTF("rc%02X\r\n", rc);

	return 0;
}

U_BOOT_CMD(
    kcs_write_i2c_reg , 255, 0, do_kcs_write_i2c_reg,
	"",
	""
);
