
#ifndef _timer_API_H_
#define _timer_API_H_

#include "src/_timer_prerequirements_check.h"

/*****************  defines  **************/


/**********  define API  types ************/


typedef enum
{
	TIMER_API_SET_COUNTDOWN_VALUE_AND_REST = IOCTL_LAST_COMMON_IOCTL+1,
	TIMER_API_CHECK_IF_COUNTDOWN_ELAPSED,
	TIMER_API_BUSY_WAIT,
}TIMER_API_ioctl_t;


/**********  define API  functions  ************/
uint8_t  timer_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor);

#include "src/_timer_static_dev_macros.h"

#define TIMER_API_CREATE_STATIC_DEV(pdev ,hw_timer_pdev ) \
			__TIMER_API_CREATE_STATIC_DEV(pdev  ,hw_timer_pdev)

#endif
