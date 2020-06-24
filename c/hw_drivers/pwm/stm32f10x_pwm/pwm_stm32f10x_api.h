
#ifndef _PWM_STM32F10X_API_H_
#define _PWM_STM32F10X_API_H_

#include "pwm_api.h"

struct pwm_stm32f10x_instance_t {
	uint32_t  output_pin;
};

SET_CONFIG_TYPE(pwm_stm32f10x, struct pwm_stm32f10x_instance_t);

#endif
