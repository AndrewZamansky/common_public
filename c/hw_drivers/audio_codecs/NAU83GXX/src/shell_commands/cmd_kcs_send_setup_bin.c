/*
 *  cmd_kcs_send_setup_bin.c
 */

#include "_project.h"
#include "shell_frontend_api.h"
#include "NAU83GXX_api.h"


extern struct dev_desc_t *G10_0x10;
extern struct dev_desc_t *G10_0x11;


static uint8_t perform_send_setup( struct dev_desc_t *NAU83GXX_dev,
		struct kcs_start_collect_data_for_send_ioctl_t *p_collect_data_ioctl,
		struct kcs_add_data_for_send_ioctl_t *p_add_data_ioctl)
{
	uint8_t reply;

	reply = DEV_IOCTL(NAU83GXX_dev,
		IOCTL_KCS_START_COLLECT_DATA_FOR_SEND, p_collect_data_ioctl);
	if (reply) return reply;

	reply = DEV_IOCTL(NAU83GXX_dev,
			IOCTL_KCS_ADD_DATA_FOR_SEND, p_add_data_ioctl);
	if (reply) return reply;

	reply = DEV_IOCTL(NAU83GXX_dev, IOCTL_KCS_SEND_COLLECTED_SETUP_DATA);
	return reply;
}


/*
 * Subroutine:  do_send_setup_bin
 * Description:
 *  buffer description:
 *  [G10_id, low offset, high offset, .... data...]
 * Inputs:
 * Return:      None
 *
 */
static void do_send_setup_bin(uint8_t *buff, size_t length)
{
	uint8_t reply;
	struct kcs_start_collect_data_for_send_ioctl_t collect_data_ioctl;
	struct kcs_add_data_for_send_ioctl_t add_data_ioctl;

	reply = NAU83GXX_RC_DEVICE_DOES_NOT_EXIST;
	shell_frontend_set_reply_bin_msg_data_size(1);

	collect_data_ioctl.offset = buff[1] + (buff[2] << 8);
	collect_data_ioctl.size_to_collect = length - 3;

	add_data_ioctl.size = length - 3;
	add_data_ioctl.data = &buff[3];

	if (0x1 & buff[0])
	{
		reply =
			perform_send_setup(G10_0x10, &collect_data_ioctl, &add_data_ioctl);
		if (reply) goto end;
	}
	if (0x2 & buff[0])
	{
		reply =
			perform_send_setup(G10_0x11, &collect_data_ioctl, &add_data_ioctl);
		if (reply) goto end;
	}

end:
	shell_frontend_reply_bin_msg_data(&reply, 1);
}

SHELL_FRONTEND_API_ADD_BINARY_CMD(0x00E1, do_send_setup_bin);
