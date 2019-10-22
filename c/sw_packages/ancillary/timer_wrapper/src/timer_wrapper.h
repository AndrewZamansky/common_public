/*
 * file : timer_wrapper.h
 *
 *
 */

#ifndef _timer_wrapper_H
#define _timer_wrapper_H

#include "dev_management_api.h"


/***************   typedefs    *******************/

struct timer_wrapper_runtime_t {
	uint64_t  timer_wrapper_value_on_start;
	uint64_t  countdown_value;
};

SET_RUNTIME_DATA_TYPE(timer_wrapper, struct timer_wrapper_runtime_t);

#endif /* */
