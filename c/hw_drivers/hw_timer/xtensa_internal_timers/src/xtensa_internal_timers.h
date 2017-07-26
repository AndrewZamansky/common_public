/*
 *
 * xtensa_internal_timers.h
 *
 *
 *
 *
 *
 */

#ifndef _XTENSA_INTERNAL_TIMERS_H_
#define _XTENSA_INTERNAL_TIMERS_H_

/********  includes *********************/

#include "dev_management_api.h"
#include "hw_timer_api.h"

/********  types  *********************/

struct xtensa_internal_timers_instance_t {
	uint32_t rate;
	struct dev_desc_t *   clock_dev;
	timer_callback_func_t timer_callback;
	uint8_t timer_index;
};


struct xtensa_internal_timers_runtime_t {
	volatile uint64_t currentTick;
};

#endif
