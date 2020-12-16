/*
 *  cmd_bin_0x00.c
 */

#include "_project.h"
#include "shell_frontend_api.h"


/*
 * Subroutine:  do_bin_0x00
 * Description:
 * Inputs:
 * Return:      None
 *
 */
static void do_bin_0x00(uint8_t *buff, size_t length)
{
	shell_frontend_set_reply_bin_msg_data_size(length);
	shell_frontend_reply_bin_msg_data(buff, length);
}

SHELL_FRONTEND_API_ADD_BINARY_CMD(0x0000, do_bin_0x00);
