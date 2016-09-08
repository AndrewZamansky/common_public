
#ifndef _LM35_API_H_
#define _LM35_API_H_

#include "src/_LM35_prerequirements_check.h"

/**********  define API  types ************/


/**********  define API  functions  ************/

#define LM35_API_SERVER_DEVICE_STR  			"server_device"

typedef enum
{
	IOCTL_LM35_SET_SERVER_DEVICE = IOCTL_LAST_COMMON_IOCTL+1,
	IOCTL_LM35_GET_CURRENT_TEMPERATURE_mC,
}LM35_API_ioctl_t;


#endif
