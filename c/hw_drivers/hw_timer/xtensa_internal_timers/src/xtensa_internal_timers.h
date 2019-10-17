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


struct xtensa_internal_timers_runtime_t {
	volatile uint64_t currentTick;
};

SET_RUNTIME_DATA_TYPE(
		xtensa_internal_timers, struct xtensa_internal_timers_runtime_t);

#endif
