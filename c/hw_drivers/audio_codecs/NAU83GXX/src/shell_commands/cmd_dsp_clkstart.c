/*
 *  cmd_dsp_clkstart.c
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


int do_dsp_clkstart(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct kcs_simple_cmd_ioctl_t  kcs_simple_cmd_ioctl;
	struct dev_desc_t *kcs_dev = NULL;
	long int device_addr;
	char *pEnd;
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

	kcs_simple_cmd_ioctl.cmd = DSP_CMD_CLK_RESTART;
	kcs_simple_cmd_ioctl.p_recieved_U32_reply = &status;
	rc = DEV_IOCTL(kcs_dev, IOCTL_KCS_SIMPLE_CMD, &kcs_simple_cmd_ioctl);

end:
	SHELL_REPLY_PRINTF("rc%02X\r\n", rc);

	if(NAU83GXX_RC_OK == rc)
	{
		SHELL_REPLY_PRINTF("%08X\r\n", status);
	}

	// needed for Klippel GUI:
	SHELL_REPLY_PRINTF("%c", 0x04UL);
	return 0;
}

U_BOOT_CMD(
    dsp_clkstart ,     255,	0,	do_dsp_clkstart,
	"[-device address]\n\tReads the counter from DSP that increments at every read.",
	"info   - NAU83G10 Specific, Reads a register that returns the \
	current status of the DSP.\r\n"
);
