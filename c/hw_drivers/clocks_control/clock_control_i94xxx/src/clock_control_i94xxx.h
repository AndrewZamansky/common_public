/*
 *
 *clock_control_i94xxx.h
 *
 *
 *
 *
 *
 */

#ifndef _CLOCK_CONTROL_I94XXX_H_
#define _CLOCK_CONTROL_I94XXX_H_

/********  includes *********************/

#include "src/_cortexM_systick_prerequirements_check.h"

/********  types  *********************/

typedef struct {
	uint32_t initial_clock_rates[I94XXX_TOTAL_NUM_OF_CLOCKS];
} clock_control_i94xxx_instance_t;

#endif
