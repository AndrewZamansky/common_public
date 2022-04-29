/*
 *  cmd_kcs_set_start.c
 */

#include "audio_path_api.h"
#include "_project.h"

#include "u-boot/include/command.h"
#include "u_boot_shell_api.h"

#include "os_wrapper.h"
#include "NAU83GXX_api.h"


int do_kcs_set_start(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	uint16_t   offset, size;
	char *pEnd;
	int rc = NAU83GXX_RC_OK;
	struct dev_desc_t *kcs_dev;
	struct kcs_start_collect_data_for_send_ioctl_t collect_data_ioctl;

	/*
	 * Arguments:
	 *     [0] Command
	 *     [1] Device name
	 *     [2] Address Offset
	 *     [3] Size of data being received
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

	offset = strtol(argv[2], &pEnd, 0);
	size = strtol(argv[3], &pEnd, 0);

	if((offset > 0xffff) || ((size > 0xffff)))
	{
		rc = NAU83GXX_RC_PARAMETERS_OUT_OF_RANGE;
		goto end;
	}

	if(KCS_I2C_MAX_BUFFER_IN_BYTES < size)
	{
		rc = NAU83GXX_RC_INPUT_SIZE_TOO_LARGE_ERR;
		goto end;
	}

	collect_data_ioctl.offset = offset;
	collect_data_ioctl.size_to_collect = size;
	rc = DEV_IOCTL(kcs_dev,
		IOCTL_KCS_START_COLLECT_DATA_FOR_SEND, &collect_data_ioctl);

end:

	SHELL_REPLY_PRINTF("rc%02X\r\n", rc);

	return 0;
}

U_BOOT_CMD(
    kcs_set_start, 255, 0, do_kcs_set_start,
	"[-device_address(hex)] [-offset(hex)] [-size(dec)]\n\tStarts the process for KCS setup",
	""
);
