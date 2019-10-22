
#ifndef _timer_wrapper_API_H_
#define _timer_wrapper_API_H_

#include "dev_management_api.h"

enum TIMER_WRAPPER_API_ioctl_e {
	IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_uSEC_AND_RESET
										= IOCTL_LAST_COMMON_IOCTL + 1,
	IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_mSEC_AND_RESET,
	IOCTL_TIMER_WRAPPER_API_CHECK_IF_COUNTDOWN_ELAPSED,
	IOCTL_TIMER_WRAPPER_API_GET_RATE_HZ,
	IOCTL_TIMER_WRAPPER_API_RESTART_COUNTER,
	IOCTL_TIMER_WRAPPER_API_GET_COUNTER,
	IOCTL_TIMER_WRAPPER_API_BUSY_WAIT
};

struct timer_wrapper_cfg_t {
	struct dev_desc_t *hw_timer_wrapper;
};

SET_CONFIG_TYPE(timer_wrapper, struct timer_wrapper_cfg_t);

#endif
