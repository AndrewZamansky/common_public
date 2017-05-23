
#ifndef _flash_wrapper_API_H_
#define _flash_wrapper_API_H_

#include "dev_management_api.h"

/**********      defines      ************/

/**********  define API  types ************/


/**********  define API  functions  ************/


typedef enum
{
	IOCTL_FLASH_WRAPPER_GET_ERASE_SIZE = IOCTL_LAST_COMMON_IOCTL + 1,
	IOCTL_FLASH_WRAPPER_GET_FLASH_SIZE,
	IOCTL_FLASH_WRAPPER_ERASE,
	IOCTL_LAST_FLASH_WRAPPER_IOCTL
}flash_wrapper_ioctl_t;

#endif
