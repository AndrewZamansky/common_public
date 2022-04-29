/*
 *  cmd_kcs_set_add_data.c
 */

#include "audio_path_api.h"
#include "_project.h"

#include "u-boot/include/command.h"
#include "u_boot_shell_api.h"

#include "os_wrapper.h"
#include "NAU83GXX_api.h"


/*
 * Subroutine:  kcs_set_add_data
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
int do_kcs_set_add_data(
		cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct kcs_add_data_for_send_ioctl_t add_data_ioctl;
	uint32_t   data_val;
	char *pEnd;
	int rc = NAU83GXX_RC_OK;
	struct dev_desc_t *kcs_dev;
	uint8_t data[4];

	/*
	 * Arguments:
	 *     [0] Command
	 *     [1] Device name
	 *     [2] Data being stored (4 bytes)
	 */
	if(argc != 3)
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

	data_val = strtoul(argv[2], &pEnd, 0);

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
	SHELL_REPLY_PRINTF("rc%02X\r\n", rc);

	return 0;
}

U_BOOT_CMD(
    kcs_set_add_data , 255, 0, do_kcs_set_add_data,
	"[-device_address] [-four_bytes_to_store_in_buffer]\n\tStores and increments buffer by four bytes",
	"info   - \r\n"
);
