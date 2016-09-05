
#ifndef _Timer_API_H_
#define _Timer_API_H_


/**********  define API  types ************/


/**********  define API  functions  ************/
typedef void (*timer_callback_func_t)(void)  ;

#define TIMER_API_PERIODIC_MODE	0
#define TIMER_API_ONE_SHOT_MODE	1

typedef enum
{
	IOCTL_TIMER_INPUT_CLOCK_ID_SET = IOCTL_LAST_COMMON_IOCTL+1,
	IOCTL_TIMER_RATE_HZ_SET,
	IOCTL_TIMER_MODE_SET,
	IOCTL_TIMER_CALLBACK_SET,
	IOCTL_TIMER_STOP,
	IOCTL_GET_CURRENT_TIMER_VALUE,
	IOCTL_LAST_COMMON_TIMER_IOCTL
}TIMER_COMMON_API_ioctl_t;




#endif
