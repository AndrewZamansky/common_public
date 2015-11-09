/*
 * file : ADC.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _ADC_H
#define _ADC_H


/***************   typedefs    *******************/


typedef struct ADC_Instance_t_{
	uint8_t channel;
} ADC_Instance_t;



uint8_t ADC_STM32F10x_Init(ADC_Instance_t *pInstance);

#else
#pragma message( "warning : this header file had already been included" )
#endif /* */
