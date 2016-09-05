/*
 * file : timer.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _TIMER_H
#define _TIMER_H

#include "src/_timer_prerequirements_check.h"


/***************   typedefs    *******************/

typedef struct
{
	pdev_descriptor_t   hw_timer;
} timer_instance_t;

typedef struct
{
	uint64_t	timer_value_on_start;
	uint64_t	countdown_value;
} timer_runtime_instance_t;

#endif /* */
