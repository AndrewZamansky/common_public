
#ifndef _POWER_MANAGEMENT_API_H_
#define _POWER_MANAGEMENT_API_H_

#include "dev_management_api.h"

enum POWER_MANAGEMENT_API_IOCTL_E
{
	IOCTL_POWER_MANAGEMENT_ENTER_HIBERNATION = IOCTL_LAST_COMMON_IOCTL + 1,
	IOCTL_LAST_COMMON_POWER_MANAGEMENT_IOCTL
};


#endif