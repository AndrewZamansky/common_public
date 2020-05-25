
#ifndef _RTC_API_H_
#define _RTC_API_H_

#include "dev_management_api.h"

enum RTC_API_IOCTL_E
{
	IOCTL_RTC_SET_WAKEUP_mSec = IOCTL_LAST_COMMON_IOCTL + 1,
	IOCTL_RTC_CALIBRATE,
	IOCTL_LAST_COMMON_RTC_IOCTL
};


#endif
