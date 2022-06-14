/*
 *  cmd_dsp_setkcs_store.c
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

/*
 * Subroutine:  dsp_setkcs_store
 *
 * Description: Writes 4 bytes to the buffer of the kcs_i2c device and
 *              increments the counter by 4 to proceed to the next memory
 *              address.
 *
 * Inputs:
 *
 * Return:      None
 *
 */
int do_dsp_setkcs_store(
		cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct kcs_add_data_for_send_ioctl_t add_data_ioctl;
	long int   device_addr;
	uint32_t   data_val;
	char *pEnd;
	int rc = NAU83GXX_RC_OK;
	struct dev_desc_t *kcs_dev = NULL;
	uint8_t data[4];

	/*
	 * Arguments:
	 *     [0] Command
	 *     [1] Device Address
	 *     [2] Data being stored (4 bytes)
	 */
	if(argc != 3)
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

	//Get data in hex
	data_val = strtoul(argv[2], &pEnd, 16);

	if(0xFFFFFFFF == data_val)
	{
		rc = NAU83GXX_RC_BUFFER_OVERFLOW_ERR;
		goto end;
	}

	data[0] = (uint8_t)(data_val & 0xFF);
	data[1] = (uint8_t)((data_val >> 8) & 0xFF);
	data[2] = (uint8_t)((data_val >> 16) & 0xFF);
	data[3] = (uint8_t)((data_val >> 24) & 0xFF);

	add_data_ioctl.size = 4;
	add_data_ioctl.data = data;

	rc = DEV_IOCTL(kcs_dev, IOCTL_KCS_ADD_DATA_FOR_SEND, &add_data_ioctl);

end:
	os_delay_ms(1);
	SHELL_REPLY_PRINTF("rc%02X\n\r", rc);

	// needed for Klippel GUI:
	SHELL_REPLY_PRINTF("%c", 0x04UL);
	return 0;
}

U_BOOT_CMD(
    dsp_setkcs_store ,     255,	0,	do_dsp_setkcs_store,
	"[-device_address] [-four_bytes_to_store_in_buffer]\n\tStores and increments buffer by four bytes",
	"info   - NAU83G10 Specific, Reads a specific amount of words \
	by an offset in the DSP.\n\r"
);
