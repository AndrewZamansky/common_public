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

#include "timer_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_timer_prerequirements_check.h" // should be after {timer_config.h,dev_managment_api.h}


/***************   typedefs    *******************/

typedef struct
{
	pdev_descriptor_const   hw_timer;
	uint64_t	timer_value_on_start;
	uint64_t	countdown_value;
} timer_instance_t;

#endif /* */
