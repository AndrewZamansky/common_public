/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  	"gpio_stm32f10x_api.h"
#include 	"src/gpio_stm32f10x.h"

uint8_t gpio_stm32f10x_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

#define	MODULE_NAME					gpio_stm32f10x
#define	MODULE_IOCTL_FUNCTION		gpio_stm32f10x_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE		GPIO_STM32F10X_Instance_t

#ifdef DT_DEV_MODULE

	#ifndef GPIO_STM32f10X_DT_PORT
		#error "GPIO_STM32f10X_DT_PORT should be defined"
	#endif

	#ifndef GPIO_STM32f10X_DT_PIN_MASK
		#error "GPIO_STM32f10X_DT_PIN_MASK should be defined"
	#endif

	#ifndef GPIO_STM32f10X_DT_MODE
		#error "GPIO_STM32f10X_DT_MODE should be defined"
	#endif



	#define STATIC_DEV_DATA_STRUCT			\
		{									\
			GPIO_STM32f10X_DT_PORT ,		\
			GPIO_STM32f10X_DT_PIN_MASK ,	\
			GPIO_STM32f10X_DT_MODE			\
		}

#endif

#define MODULE_CONFIGURABLE_PARAMS_ARRAY	{	\
		{IOCTL_GPIO_STM32F10X_SET_PORT_PARAM , IOCTL_VOID ,GPIO_STM32F10X_API_PORT_STR, NOT_FOR_SAVE}, 	\
		{IOCTL_GPIO_STM32F10X_SET_PIN_PARAM , IOCTL_VOID , GPIO_STM32F10X_API_PIN_STR, NOT_FOR_SAVE},	\
		{IOCTL_GPIO_STM32F10X_SET_MODE_PARAM , IOCTL_VOID ,GPIO_STM32F10X_API_MODE_STR, NOT_FOR_SAVE},	\
	}

#include "add_component.h"

/* device specific defines should be undefined after calling #include "add_component.h" */
#undef GPIO_STM32f10X_DT_PORT
#undef GPIO_STM32f10X_DT_PIN
#undef GPIO_STM32f10X_DT_MODE
