/*
 * file : adc_stm8.h
 *
 */

#ifndef _ADC_STM8_H
#define _ADC_STM8_H


// should be after {uart_stm8_config.h,dev_management_api.h}
#include "src/_adc_stm8_prerequirements_check.h"

struct adc_stm8_instance_t {
	uint8_t channel;
};


#endif
