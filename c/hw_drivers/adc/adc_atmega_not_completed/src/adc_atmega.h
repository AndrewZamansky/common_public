/*
 * file : adc_atmega.h
 *
 */

#ifndef _ADC_H
#define _ADC_H

struct ADC_Instance_t {
	uint8_t channel;
};


uint8_t ADC_STM32F10x_Init(struct ADC_Instance_t *pInstance);

#endif 
