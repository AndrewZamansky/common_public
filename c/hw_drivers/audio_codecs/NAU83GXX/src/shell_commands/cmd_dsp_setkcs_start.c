/*
 *  cmd_dsp_setkcs_start.c
 */

#include "audio_path_api.h"
#include "_project.h"

#include "u-boot/include/command.h"
#include "u_boot_shell_api.h"

#include "os_wrapper.h"
#include "NAU83GXX_api.h"
#include "i2c_api.h"


extern struct dev_desc_t *kcs_left_dev;
extern struct dev_desc_t *kcs_right_dev;
extern struct dev_desc_t *kcs_right_core_1_dev;

int do_dsp_setkcs_start(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	long int   device_addr;
	uint16_t   offset, size;
	char *pEnd;
	int rc = NAU83GXX_RC_OK;
	struct dev_desc_t *kcs_dev = NULL;
	struct kcs_start_collect_data_for_send_ioctl_t collect_data_ioctl;

	/*
	 * Arguments:
	 *     [0] Command
	 *     [1] Device Address
	 *     [2] Address Offset
	 *     [3] Size of data being received
	 */
	if(argc != 4)
	{
		rc = NAU83GXX_RC_SYNTAX_ERROR;
		goto end;
	}

	//Get I2C device address, same was as c programs in linux off gcc
	//I2C device address have to be either 0x10 or 0x11
	device_addr = strtol(argv[1], &pEnd, 16);

	if ((0x10 == device_addr) || (0x100 == device_addr))
	{
		kcs_dev = kcs_left_dev;
	}
	else if (0x11 == device_addr)
	{
		kcs_dev = kcs_right_dev;
	}
	else if (0x101 == device_addr)
	{
		kcs_dev = kcs_right_core_1_dev;
	}
	else
	{
		rc = NAU83GXX_RC_DEVICE_DOES_NOT_EXIST;
		goto end;
	}

	//Get offset in hex and size in decimal
	offset = strtol(argv[2], &pEnd, 16);
	size = strtol(argv[3], &pEnd, 10);

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
	os_delay_ms(1);
	SHELL_REPLY_PRINTF("rc%02X\n\r", rc);


	// needed for Klippel GUI:
	SHELL_REPLY_PRINTF("%c", 0x04UL);

	return 0;
}

U_BOOT_CMD(
    dsp_setkcs_start,     255,	0,	do_dsp_setkcs_start,
	"[-device_address(hex)] [-offset(hex)] [-size(dec)]\n\tStarts the process for KCS setup",
	"info   - NAU83G10 Specific, Reads a specific amount of words \
	by an offset in the DSP.\n\r"
);
