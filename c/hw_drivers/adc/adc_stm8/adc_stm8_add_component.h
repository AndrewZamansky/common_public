/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "adc_stm8_api.h"
#include "src/adc_stm8.h"

uint8_t adc_stm8_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

#define	MODULE_NAME					adc_stm8
#define	MODULE_IOCTL_FUNCTION		adc_stm8_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE		ADC_STM8_Instance_t

#ifdef DT_DEV_MODULE

	#ifndef ADC_STM8_CHANNEL
		#error "ADC_STM8_CHANNEL should be defined"
	#endif

	#define STATIC_DEV_DATA_STRUCT	\
		{							\
			ADC_STM8_CHANNEL		\
		}
#endif

#include "add_component.h"

#define MODULE_CONFIGURABLE_PARAMS_ARRAY	{	\
		{IOCTL_adc_stm8_SET_SERVER_DEVICE , IOCTL_VOID , adc_stm8_API_SERVER_DEVICE_STR, NOT_FOR_SAVE},	\
	}

/* device specific defines should be undefined after calling #include "add_component.h" */
#undef adc_stm8_ADC_SERVER_PDEV
