
#ifndef _hw_Timer_API_H_
#define _hw_Timer_API_H_

#include "dev_management_api.h"

typedef void (*timer_callback_func_t)(void)  ;

#define TIMER_API_PERIODIC_MODE	0
#define TIMER_API_ONE_SHOT_MODE	1

struct set_pwm_params {
	uint32_t freq;
	uint32_t duty_cycle_mPercent;
};

enum PWM_COMMON_API_IOCTL_e {
	IOCTL_PWM_SET_PARAMS = IOCTL_LAST_COMMON_IOCTL + 1,
	IOCTL_PWM_ENABLE_OUTPUT,
	IOCTL_PWM_DISABLE_OUTPUT,
};




#endif
