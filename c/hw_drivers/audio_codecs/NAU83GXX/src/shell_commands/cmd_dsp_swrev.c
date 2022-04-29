/*
 *  cmd_dsp_swrev.c
 */

#include "audio_path_api.h"
#include "_project.h"

#include "u-boot/include/command.h"
#include "u_boot_shell_api.h"

#include "os_wrapper.h"
#include "NAU83GXX_api.h"
#include "i2c_api.h"


extern struct dev_desc_t *NAU83GXX_left_dev;
extern struct dev_desc_t *NAU83GXX_right_dev;


int do_dsp_swrev(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct kcs_simple_cmd_ioctl_t  kcs_simple_cmd_ioctl;
	long int device_addr;
	char *pEnd;
	struct dev_desc_t *kcs_i2c_dev;
	uint32_t status = 0;
	int rc = NAU83GXX_RC_OK;

	/*
	 * Arguments:
	 *     [0] Command
	 *     [1] Device Address
	 */
	if(argc != 2)
	{
		rc = NAU83GXX_RC_SYNTAX_ERROR;
		goto end;
	}

	device_addr = strtol(argv[1], &pEnd, 16);

	if ((0x10 == device_addr) || (0x100 == device_addr))
	{
		kcs_i2c_dev = NAU83GXX_left_dev;
	}
	else if(0x11 == device_addr)
	{
		kcs_i2c_dev = NAU83GXX_right_dev;
	}
	else
	{
		rc = NAU83GXX_RC_DEVICE_DOES_NOT_EXIST;
		goto end;
	}

	kcs_simple_cmd_ioctl.cmd = DSP_CMD_GET_SW_REVISION;
	kcs_simple_cmd_ioctl.p_recieved_U32_reply = &status;
	rc = DEV_IOCTL(kcs_i2c_dev, IOCTL_KCS_SIMPLE_CMD, &kcs_simple_cmd_ioctl);

end:
	os_delay_ms(1);

	SHELL_REPLY_PRINTF("rc%02X\n\r", rc);
	if(NAU83GXX_RC_OK == rc)
	{
		SHELL_REPLY_PRINTF("%08X\n\r", status);
	}

	// needed for Klippel GUI:
	SHELL_REPLY_PRINTF("%c", 0x04UL);
	return 0;
}

U_BOOT_CMD(
    dsp_swrev ,     255,	0,	do_dsp_swrev,
	"[-device address]\n\tReads the counter from DSP that increments at every read.",
	"info   - NAU83G10 Specific, Reads a register that returns the \
	current status of the DSP.\n\r"
);
