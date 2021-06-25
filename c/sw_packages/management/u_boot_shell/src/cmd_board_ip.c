/*
 * cmd_board_ip.c
 */
#include "_project.h"

#include "dev_management_api.h" // for device manager defines and typedefs

#include <u-boot/include/command.h>
#include "u_boot_shell_api.h"
#include "ESP8266_api.h"



/*
 * Subroutine:  do_brd_ip
 *
 * Description:
 *
 * Inputs:
 *
 * Return:      None
 *
 */
static int do_brd_ip (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct dev_desc_t * ESP8266_dev;

	uint8_t ip_str[17];

	ESP8266_dev=DEV_API_open_device((uint8_t*)"wifi1");
	if(0 == DEV_IOCTL(ESP8266_dev,IOCTL_ESP8266_GET_IP , ip_str))
	{
		SHELL_REPLY_STR(ip_str);
		return 0;
	}

	SHELL_REPLY_STR("no_ip");
	return 0;
}

U_BOOT_CMD(
	brd_ip, 255, 0, do_brd_ip,
	"board info",
	"info   - \n"
);

