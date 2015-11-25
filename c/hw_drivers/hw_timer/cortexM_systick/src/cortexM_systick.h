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

#include "cortexM_systick_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_cortexM_systick_prerequirements_check.h" // should be after {cortexM_systick_config.h,dev_managment_api.h}
#include "hw_timer_api.h"
/********  types  *********************/

typedef struct {
	uint32_t input_clock;
	uint32_t rate;
	timer_callback_func_t timer_callback;
	uint8_t mode;
} CORTEXM_SYSTICK_Instance_t;

#endif
