
#ifndef _flash_wrapper_API_H_
#define _flash_wrapper_API_H_

#include "dev_management_api.h"

/**********      defines      ************/

/**********  define API  types ************/


/**********  define API  functions  ************/

enum flash_wrapper_ioctl_e {
	IOCTL_FLASH_WRAPPER_GET_ERASE_SIZE = IOCTL_LAST_COMMON_IOCTL + 1,
	IOCTL_FLASH_WRAPPER_GET_FLASH_SIZE,
	IOCTL_FLASH_WRAPPER_ERASE,
	IOCTL_LAST_FLASH_WRAPPER_IOCTL
};

struct flash_wrapper_cfg_t {
	struct dev_desc_t *   hw_flash_dev;
};

#endif
