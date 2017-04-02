/*
 *
 * cortexM_systick.h
 *
 *
 *
 *
 *
 */

#ifndef _CORTEXM_SYSTICK_H_
#define _CORTEXM_SYSTICK_H_

/********  includes *********************/

#include "src/_cortexM_systick_prerequirements_check.h"

/********  types  *********************/

typedef struct {
	uint32_t rate;
	pdev_descriptor_t   clock_dev;
	timer_callback_func_t timer_callback;
	uint8_t mode;
} cortexM_systick_instance_t;

#endif
