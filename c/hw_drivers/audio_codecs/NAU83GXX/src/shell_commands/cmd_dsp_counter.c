/*
 *  cmd_dsp_counter.c
 */

#include "audio_path_api.h"
#include "_project.h"

#include "u-boot/include/command.h"
#include "u_boot_shell_api.h"

#include "os_wrapper.h"
#include "NAU83GXX_api.h"
#include "i2c_api.h"


extern struct dev_desc_t *G10_0x10;
extern struct dev_desc_t *G10_0x11;


int do_dsp_counter(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct kcs_simple_cmd_ioctl_t  kcs_simple_cmd_ioctl;
	long int device_addr;
	char *pEnd;
	struct dev_desc_t *kcs_i2c_dev;
	uint32_t status = 0;
	int rc = RC_OK;

	/*
	 * Arguments:
	 *     [0] Command
	 *     [1] Device Address
	 */
	if(argc != 2)
	{
		rc = RC_SYNTAX_ERROR;
		goto end;
	}

	//Get I2C device address, same was as c programs in linux off gcc
	//I2C device address have to be either 0x10 or 0x11
	device_addr = strtol(argv[1], &pEnd, 16);

	if(0x10 == device_addr)
	{
		kcs_i2c_dev = G10_0x10;
	}
	else if(0x11 == device_addr)
	{
		kcs_i2c_dev = G10_0x11;
	}
	else
	{
		rc = RC_DEVICE_DOES_NOT_EXIST;
		goto end;
	}

	kcs_simple_cmd_ioctl.cmd = DSP_CMD_ECHO_CNT;
	kcs_simple_cmd_ioctl.p_recieved_U32_reply = &status;
	rc = DEV_IOCTL_1_PARAMS(
			kcs_i2c_dev, IOCTL_KCS_SIMPLE_CMD, &kcs_simple_cmd_ioctl);

end:
	//Delay by 1 ms to ensure shell doesn't hog resources [redundant]
	os_delay_ms(1);

	SHELL_REPLY_PRINTF("rc%02X\n\r", rc);

	if(RC_OK == rc)
	{
		//Print the data out
		SHELL_REPLY_PRINTF("%08X\n\r", status);
	}

	// needed for Klippel GUI:
	SHELL_REPLY_PRINTF("%c", 0x04UL);
	return 0;
}

U_BOOT_CMD(
    dsp_counter ,     255,	0,	do_dsp_counter,
	"[-device address]\n\tReads a register that \
	increments after reading",

	"info   - NAU83G10 Specific, Reads a register that \
	increments after reading.\n\r"
);
