/*
 * file : _ADC_STM32F10X.h
 *
 */

#ifndef _ADC_STM32F10X_H
#define _ADC_STM32F10X_H

struct adc_stm32f10x_runtime_t {
	uint8_t  init_done;
	uint8_t channel;
};
SET_RUNTIME_DATA_TYPE(adc_stm32f10x, struct adc_stm32f10x_runtime_t);

#endif
