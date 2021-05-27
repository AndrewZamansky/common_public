
#ifndef _REMOTE_DEVICES_API_H_
#define _REMOTE_DEVICES_API_H_

#include "gpio_api.h"


#define REMOTE_DEVICES_API_SERVER_DEVICE_STR  	"server_device"


enum REMOTE_DEVICES_API_ioctl_e {
	IOCTL_REMOTE_SEND_CMD = IOCTL_LAST_COMMON_IOCTL + 1,
};


struct remote_cmd_t {
	uint8_t *addr_num;
	uint8_t *comand_str;
	uint8_t comand_str_len;
	uint8_t result[4];
};


uint8_t  remote_devices_api_init_dev_descriptor(struct dev_desc_t *aDevDescriptor);

#endif
