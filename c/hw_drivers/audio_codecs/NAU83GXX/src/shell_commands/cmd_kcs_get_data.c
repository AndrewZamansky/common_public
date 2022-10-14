/*
 *  cmd_kcs_get_data.c
 */

#include "audio_path_api.h"
#include "_project.h"

#include "u-boot/include/command.h"
#include "u_boot_shell_api.h"

#include "os_wrapper.h"
#include "NAU83GXX_api.h"


int do_kcs_get_data(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct kcs_cmd_get_ioctl_t 	kcs_cmd_get_ioctl;
	struct dev_desc_t *kcs_dev = NULL;
	uint16_t   offset;
	uint16_t   size;
	uint16_t   recieved_size;
	char *pEnd;
	int   data_print_word_sizing = 0;
	int i;
	uint8_t   *recvBuf;
	uint32_t   status = 0;
	int rc = NAU83GXX_RC_OK;

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

	kcs_cmd_get_ioctl.offset = offset;
	kcs_cmd_get_ioctl.size_to_read = size;
	kcs_cmd_get_ioctl.recieved_size = &recieved_size;
	kcs_cmd_get_ioctl.recieved_data = &recvBuf;
	rc = DEV_IOCTL(kcs_dev, IOCTL_KCS_GET_RESULTS_CMD, &kcs_cmd_get_ioctl);

end:

	SHELL_REPLY_PRINTF("rc%02X\r\n", rc);

	if(NAU83GXX_RC_OK == rc)
	{
		//Print the data out
		data_print_word_sizing = size % 4;
		for(i = 0; i < (size - data_print_word_sizing); i = i + 4)
		{
			status =    (recvBuf[i]) | (recvBuf[i+1] << 8) |
						(recvBuf[i+2] << 16) | (recvBuf[i+3] << 24);
			SHELL_REPLY_PRINTF("%08X\r\n", status);
		}
		if(data_print_word_sizing)
		{
			for(i = 1; i <= data_print_word_sizing; i++)
			{
				SHELL_REPLY_PRINTF("%02X", recvBuf[size - i]);
			}
			SHELL_REPLY_PRINTF("\r\n");
		}
	}

	rc = DEV_IOCTL(kcs_dev, IOCTL_KCS_EXIT_GET_STATE);

	return 0;
}

U_BOOT_CMD(
    kcs_get_data , 255, 0, do_kcs_get_data,
	"",
	""
);
