/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "adc_stm32f10x_api.h"
#include  "src/adc_stm32f10x.h"

#ifndef ADC_STM32F10X_DT_CLOCK_RATE
	#error "ADC_STM32F10X_DT_CHANNEL should be defined"
#endif

#ifndef ADC_STM32F10X_DT_CHANNEL
	#error "ADC_STM32F10X_DT_CHANNEL should be defined"
#endif


SET_STATIC_DEV_CONFIG(adc_stm32f10x) =
{
	ADC_STM32F10X_DT_CLOCK_RATE,
	ADC_STM32F10X_DT_CHANNEL,
};


#include "add_static_dev.h"

/* device specific defines should be undefined
 * after calling #include "add_static_dev.h"
 */
#undef ADC_STM32F10X_DT_CLOCK_RATE
#undef ADC_STM32F10X_DT_CHANNEL
