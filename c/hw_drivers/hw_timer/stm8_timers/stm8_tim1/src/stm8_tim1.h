/*
 *
 * stm8_tim1.h
 *
 */

#ifndef _STM8_TIM1_H_
#define _STM8_TIM1_H_


#include "src/_stm8_tim1_prerequirements_check.h"


struct stm8_tim1_instance_t {
	uint32_t rate;
	uint32_t parent_clock_rate;
	uint32_t calculated_period_rate;
//	struct dev_desc_t *   clock_dev;
//	size_t	clock_index;
	timer_callback_func_t timer_callback;
	uint8_t mode;
};


#endif
