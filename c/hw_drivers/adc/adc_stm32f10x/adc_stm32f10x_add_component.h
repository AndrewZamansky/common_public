/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "adc_stm32f10x_api.h"
#include  "src/adc_stm32f10x.h"


#define MODULE_NAME                     adc_stm32f10x
#define MODULE_IOCTL_FUNCTION           adc_stm32f10x_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE  struct adc_stm32f10x_config_t
#define MODULE_RUNTIME_DATA_STRUCT_TYPE struct adc_stm32f10x_runtime_t

#ifdef DT_DEV_MODULE

	#ifndef ADC_STM32F10X_DT_CLOCK_RATE
		#error "ADC_STM32F10X_DT_CHANNEL should be defined"
	#endif

	#ifndef ADC_STM32F10X_DT_CHANNEL
		#error "ADC_STM32F10X_DT_CHANNEL should be defined"
	#endif



	#define STATIC_DEV_DATA_STRUCT  \
		{                           \
			ADC_STM32F10X_DT_CLOCK_RATE, \
			ADC_STM32F10X_DT_CHANNEL,    \
		}


#endif

#define MODULE_CONFIGURABLE_PARAMS_ARRAY  {     \
		{IOCTL_ADC_STM32F10X_SET_CHANNEL_PARAM , IOCTL_VOID ,    \
					ADC_STM32F10X_API_CHANNEL_STR, NOT_FOR_SAVE} \
	}

#include "add_static_dev.h"

/* device specific defines should be undefined
 * after calling #include "add_static_dev.h"
 */
#undef ADC_STM32F10X_DT_CLOCK_RATE
#undef ADC_STM32F10X_DT_CHANNEL
