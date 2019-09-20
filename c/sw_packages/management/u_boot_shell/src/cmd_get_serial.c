/*
 *  cmd_get_serial.c
 */
#include "_project.h"
#include "errors_api.h"

#include "dev_management_api.h" // for device manager defines and typedefs

#include <u-boot/include/command.h>
#include "u_boot_shell_api.h"
#include "serial_number_stm32f10x_api.h"


extern uint8_t *pSerNum;

/*
 * Subroutine:  do_get_serial
 *
 * Description:
 *
 * Inputs:
 *
 * Return:      None
 *
 */
int do_get_serial (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	uint8_t *ret_buff;

	ret_buff = (uint8_t*)malloc(2*SERIAL_NUMBER_LEN * sizeof(uint8_t));
	errors_api_check_if_malloc_succeed(ret_buff);

	if(NULL != ret_buff)
	{
		uint8_t i;

		for(i = 0; i < SERIAL_NUMBER_LEN; i++)
		{
			snprintf((char*)&ret_buff[i*2], 3, "%02X", pSerNum[i]);
		}

		SHELL_REPLY_DATA(ret_buff, 2*SERIAL_NUMBER_LEN);
		free(ret_buff);
	}
	return 0;
}

U_BOOT_CMD(
	get_serial, 255, 0, do_get_serial,
	"",
	"info   - \n"
);

