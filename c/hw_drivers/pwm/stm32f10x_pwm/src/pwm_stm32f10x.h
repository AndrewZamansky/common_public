/*
 *
 * pwm_stm32f10x.h
 *
 */

#ifndef _PWM_STM32F10X_H_
#define _PWM_STM32F10X_H_

#include "stm32f10x_tim.h"

struct pwm_stm32f10x_runtime_t {
	TIM_TypeDef* TIMx;
	uint8_t channel;
	uint8_t  init_done;
};
SET_RUNTIME_DATA_TYPE(pwm_stm32f10x, struct pwm_stm32f10x_runtime_t);


#endif
