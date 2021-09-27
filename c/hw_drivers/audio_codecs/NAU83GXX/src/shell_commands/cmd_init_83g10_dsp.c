/*
 *  cmd_init_83g10_dsp.c
 */

#include "audio_path_api.h"
#include "_project.h"

#include "u-boot/include/command.h"
#include "u_boot_shell_api.h"
#include "NAU83GXX_api.h"

#include "../NAU83GXX.h"
#include "os_wrapper.h"
#include "i2c_api.h"


extern struct dev_desc_t *G10_0x10;
extern struct dev_desc_t *G10_0x11;

/*
 * Subroutine:  do_init_83g10_dsp
 *
 * Description:
 *
 * Inputs:
 *
 * Return:      None
 *
 */
int do_init_83g10_dsp(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	long int   device_addr;
	uint8_t rc = 0;
	char *pEnd;

	if(argc != 2)
	{
		rc = RC_SYNTAX_ERROR;
		goto end;
	}

	device_addr = strtol(argv[1], &pEnd, 16);

	if (0x10 == device_addr)
	{
		DEV_IOCTL_0_PARAMS(G10_0x10, IOCTL_NAU83GXX_REINIT_I2C_REGISTERS);
	}
	else
	{
		DEV_IOCTL_0_PARAMS(G10_0x11, IOCTL_NAU83GXX_REINIT_I2C_REGISTERS);
	}
	//temporary add delay. will be removed after added sync with KCS task
	os_delay_ms(100);

	end:
	SHELL_REPLY_PRINTF("rc%02X\n\r", rc);

	os_delay_ms(1);

	// needed for Klippel GUI:
	SHELL_REPLY_PRINTF("%c", 0x04UL);

	return 0;
}

U_BOOT_CMD(
    init_83g10_dsp ,     255,	0,	do_init_83g10_dsp,
	"[-device_address]\n\tAssumes 0x10 is left ch and 0x11 is right ch.",
	"info   - \n"
);
