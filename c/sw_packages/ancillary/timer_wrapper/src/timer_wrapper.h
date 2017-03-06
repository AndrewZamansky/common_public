/*
 * file : timer_wrapper.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _timer_wrapper_H
#define _timer_wrapper_H

#include "src/_timer_wrapper_prerequirements_check.h"


/***************   typedefs    *******************/

typedef struct
{
	pdev_descriptor_t   hw_timer_wrapper;
} timer_wrapper_instance_t;

typedef struct
{
	uint64_t	timer_wrapper_value_on_start;
	uint64_t	countdown_value;
} timer_wrapper_runtime_instance_t;

#endif /* */
