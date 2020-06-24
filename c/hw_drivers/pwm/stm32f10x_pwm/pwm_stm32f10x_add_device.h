/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include "pwm_stm32f10x_api.h"
#include "src/pwm_stm32f10x.h"

#ifndef PWM_STM32F10X_DT_OUTPUT_PIN
	#error "PWM_STM32F10X_DT_OUTPUT_PIN should be defined"
#endif


SET_STATIC_DEV_CONFIG(pwm_stm32f10x) =
{
	PWM_STM32F10X_DT_OUTPUT_PIN
};

/**
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#include "add_static_dev.h"

#undef PWM_STM32F10X_DT_OUTPUT_PIN
