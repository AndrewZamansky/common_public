
#ifndef _timer_wrapper_API_H_
#define _timer_wrapper_API_H_

#include "dev_management_api.h"

/*****************  defines  **************/


/**********  define API  types ************/


typedef enum
{
	IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_uSEC_AND_RESET
										= IOCTL_LAST_COMMON_IOCTL + 1,
	IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_mSEC_AND_RESET,
	IOCTL_TIMER_WRAPPER_API_CHECK_IF_COUNTDOWN_ELAPSED,
	IOCTL_TIMER_WRAPPER_API_GET_RATE_HZ,
	IOCTL_TIMER_WRAPPER_API_RESTART_COUNTER,
	IOCTL_TIMER_WRAPPER_API_GET_COUNTER,
	IOCTL_TIMER_WRAPPER_API_BUSY_WAIT
}TIMER_WRAPPER_API_ioctl_t;


/**********  define API  functions  ************/

#endif
