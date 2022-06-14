/*
 *  cmd_dsp_setkcs_send.c
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

int do_dsp_setkcs_send(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	long int   device_addr;
	char *pEnd;
	struct dev_desc_t *kcs_dev = NULL;

	int rc = NAU83GXX_RC_OK;

	/*
	 * Arguments:
	 *     [0] Command
	 *     [1] Device Address
	 *     [2] Address Offset
	 *     [3] Size of data being received
	 */
	if(argc != 2)
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

	rc = DEV_IOCTL(kcs_dev, IOCTL_KCS_SEND_COLLECTED_SETUP_DATA);

	end:
	os_delay_ms(1);
	SHELL_REPLY_PRINTF("rc%02X\n\r", rc);

	// needed for Klippel GUI:
	SHELL_REPLY_PRINTF("%c", 0x04UL);

	return 0;
}

U_BOOT_CMD(
    dsp_setkcs_send ,     255,	0,	do_dsp_setkcs_send,
	"[-device_address]\n\tSends buffer to DSP",
	"info   - NAU83G10 Specific, Reads a specific amount of words \
	by an offset in the DSP.\n\r"
);
