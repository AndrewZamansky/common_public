/*
 * file : ADC.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _ADC_STM8_H
#define _ADC_STM8_H


/***************   typedefs    *******************/

#include "adc_stm8_config.h"
#include "dev_management_api.h" // for device manager defines and typedefs
#include "src/_adc_stm8_prerequirements_check.h" // should be after {uart_stm8_config.h,dev_management_api.h}

typedef struct ADC_STM8_Instance_t_{
	uint8_t channel;

} ADC_STM8_Instance_t;




#endif /* */
