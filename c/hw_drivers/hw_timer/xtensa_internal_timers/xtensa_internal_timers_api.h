
#ifndef _XTENSA_INTERNAL_TIMERS_API_H_
#define _XTENSA_INTERNAL_TIMERS_API_H_

#include "dev_management_api.h"
#include "hw_timer_api.h"


struct xtensa_internal_timers_instance_t {
	uint32_t rate;
	struct dev_desc_t *   clock_dev;
	timer_callback_func_t timer_callback;
	uint8_t timer_index;
};

SET_CONFIG_TYPE(
		xtensa_internal_timers, struct xtensa_internal_timers_instance_t);

#endif
