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

#include "dev_management_api.h"
#include "hw_timer_api.h"

/********  types  *********************/

typedef struct {
	uint32_t rate;
	struct dev_desc_t *   clock_dev;
	timer_callback_func_t timer_callback;
	uint8_t mode;
} cortexM_systick_instance_t;

#endif
