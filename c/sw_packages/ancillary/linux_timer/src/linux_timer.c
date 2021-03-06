/*
 *
 *   file  :  linux_timer.c
 *
 */
#include "linux_timer.h"


#include "_project_typedefs.h"
#include "_project_defines.h"
#include <time.h>

#include "dev_management_api.h"


//static volatile uint64_t currentTick=0;
volatile uint64_t currentTick=0;


/*
 * linux_timer_ioctl()
 *
 * return:
 */
uint8_t linux_timer_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct timespec abs_timeout;

	switch(aIoctl_num)
	{
	case IOCTL_TIMER_RATE_HZ_SET :
		break;
	case IOCTL_TIMER_GET_RATE_HZ :
		*(uint32_t *)aIoctl_param1 = 1000;
		break;

	case IOCTL_TIMER_CALLBACK_SET :
		break;

	case IOCTL_TIMER_MODE_SET :
		break;

	case IOCTL_DEVICE_START :

		break;

	case IOCTL_TIMER_STOP :
		break;

	case IOCTL_GET_CURRENT_TIMER_VALUE :
		clock_gettime(CLOCK_REALTIME, &abs_timeout);
		*(uint64_t *)aIoctl_param1 =
				(abs_timeout.tv_sec * 1000) + (abs_timeout.tv_nsec / 1000000);
		break;

	default :
		return 1;
	}
	return 0;
}

#define MODULE_NAME                  linux_timer
#define MODULE_IOCTL_FUNCTION        linux_timer_ioctl
#define MODULE_HAS_NO_CONFIG_DATA
#define MODULE_HAS_NO_RUNTIME_DATA
#include "add_module.h"
