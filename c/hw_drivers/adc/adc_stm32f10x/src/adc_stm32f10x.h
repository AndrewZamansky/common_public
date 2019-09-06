/*
 * file : _ADC_STM32F10X.h
 *
 *
 */

#ifndef _ADC_STM32F10X_H
#define _ADC_STM32F10X_H


/***************   typedefs    *******************/


struct adc_stm32f10x_config_t {
	uint32_t adc_clock_rate;
	uint8_t channel;
};

struct adc_stm32f10x_runtime_t
{
	uint8_t  init_done;
};

#endif /* */
