/*
 *  cmd_dsp_getkcs_setup.c
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


int do_dsp_getkcs_setup(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct kcs_cmd_get_ioctl_t 	kcs_cmd_get_ioctl;
	long int   device_addr;
	uint16_t   offset;
	uint16_t   size;
	uint16_t   recieved_size;

	int   data_print_word_sizing = 0;
	char *pEnd;
	int i;
	struct dev_desc_t *kcs_i2c_dev = NULL;
	uint8_t   *recvBuf;
	uint32_t   status = 0;
	int rc = RC_OK;

	/*
	 * Arguments:
	 *     [0] Command
	 *     [1] Device Address
	 *     [2] Address Offset
	 *     [3] Size of data being received
	 */
	if(argc != 4)
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

	//Get offset in hex and size in decimal
	//same was as c programs in linux off gcc
	offset = strtol(argv[2], &pEnd, 16);
	size = strtol(argv[3], &pEnd, 10);

	if((offset > 0xffff) || ((size > 0xffff)))
	{
		rc = RC_PARAMETERS_OUT_OF_RANGE;
		goto end;
	}

	kcs_cmd_get_ioctl.offset = offset;
	kcs_cmd_get_ioctl.size_to_read = size;
	kcs_cmd_get_ioctl.recieved_size = &recieved_size;
	kcs_cmd_get_ioctl.recieved_data = &recvBuf;
	rc = DEV_IOCTL_1_PARAMS(
			kcs_i2c_dev, IOCTL_KCS_GET_SETUP_CMD, &kcs_cmd_get_ioctl);

end:
	//Delay by 1 ms to ensure shell doesn't hog resources [redundant]
	os_delay_ms(1);
	SHELL_REPLY_PRINTF("rc%02X\n\r", rc);

	if(RC_OK == rc)
	{
		//Print the data out
		data_print_word_sizing = size % 4;
		for(i = 0; i < (size - data_print_word_sizing); i = i + 4)
		{
			status =    (recvBuf[i]) | (recvBuf[i+1] << 8) |
						(recvBuf[i+2] << 16) | (recvBuf[i+3] << 24);
			SHELL_REPLY_PRINTF("%08X\n\r", status);
		}
		if(data_print_word_sizing)
		{
			for(i = 1; i <= data_print_word_sizing; i++)
			{
				SHELL_REPLY_PRINTF("%02X", recvBuf[size - i]);
			}
			SHELL_REPLY_PRINTF("\n\r");
		}
	}
	rc = DEV_IOCTL_0_PARAMS(kcs_i2c_dev, IOCTL_KCS_EXIT_GET_STATE);

#ifdef KCS_RC_DEBUG_COUNTER
	else
	{
		kcs_rc_debug_counter++;
	}
#endif

	// needed for Klippel GUI:
	SHELL_REPLY_PRINTF("%c", 0x04UL);

	return 0;
}

U_BOOT_CMD(
    dsp_getkcs_setup ,     255,	0,	do_dsp_getkcs_setup,
	"[-device_address] [-offset] [-num_of_words]\n\tReads num of words by offset from DSP",
	"info   - NAU83G10 Specific, Reads a specific amount of words \
	by an offset in the DSP.\n\r"
);
