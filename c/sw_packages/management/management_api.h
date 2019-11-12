
#ifndef _management_api_h_
#define _management_api_h_

#include "dev_management_api.h"

enum SHELL_API_IOCTL_E
{
	IOCTL_SHELL_NEW_FRAME_RECEIVED = IOCTL_LAST_COMMON_IOCTL + 1,
};

struct rcvd_cmd_t {
	struct dev_desc_t * reply_dev;
	uint8_t  *cmd_buf;
	size_t	cmd_len;
};

#endif
