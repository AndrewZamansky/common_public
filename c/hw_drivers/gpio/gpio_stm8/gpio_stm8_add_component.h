/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "gpio_stm8_api.h"

#include "src/gpio_stm8.h"
uint8_t gpio_stm8_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

#define	MODULE_NAME				gpio_stm8
#define	MODULE_IOCTL_FUNCTION	gpio_stm8_ioctl
#define MODULE_DATA_STRUCT_TYPE	GPIO_STM8_Instance_t

#ifdef DT_DEV_MODULE

	#ifndef GPIO_STM8_DT_PORT
		#error "GPIO_STM8_DT_PORT should be defined"
	#endif

	#ifndef GPIO_STM8_DT_PIN
		#error "GPIO_STM8_DT_PIN should be defined"
	#endif

	#ifndef GPIO_STM8_DT_MODE
		#error "GPIO_STM8_DT_MODE should be defined"
	#endif



	#define STATIC_DEV_DATA_STRUCT	\
		{							\
			GPIO_STM8_DT_PORT ,		\
			GPIO_STM8_DT_PIN ,		\
			GPIO_STM8_DT_MODE		\
		}

#endif


#include "add_component.h"

#undef GPIO_STM8_DT_PORT
#undef GPIO_STM8_DT_PIN
#undef GPIO_STM8_DT_MODE
