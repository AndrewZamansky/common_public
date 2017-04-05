
#ifndef _timer_wrapper_API_H_
#define _timer_wrapper_API_H_

#include "src/_timer_wrapper_prerequirements_check.h"

/*****************  defines  **************/


/**********  define API  types ************/


typedef enum
{
	IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_VALUE_AND_RESET = IOCTL_LAST_COMMON_IOCTL+1,
	IOCTL_TIMER_WRAPPER_API_CHECK_IF_COUNTDOWN_ELAPSED,
	IOCTL_TIMER_WRAPPER_API_GET_RATE_HZ,
	IOCTL_TIMER_WRAPPER_API_RESTART_COUNTER,
	IOCTL_TIMER_WRAPPER_API_GET_COUNTER,
	IOCTL_TIMER_WRAPPER_API_BUSY_WAIT
}TIMER_WRAPPER_API_ioctl_t;


/**********  define API  functions  ************/
uint8_t  timer_wrapper_api_init_dev_descriptor(struct dev_desc_t *aDevDescriptor);

#endif