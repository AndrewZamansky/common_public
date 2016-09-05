/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "gpio_nuc505_api.h"
#include "src/gpio_nuc505.h"

uint8_t gpio_nuc505_ioctl( pdev_descriptor_t apdev ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

#define	MODULE_NAME				gpio_nuc505
#define	MODULE_IOCTL_FUNCTION	gpio_nuc505_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE	gpio_nuc505_instance_t

#ifdef DT_DEV_MODULE

	#ifndef GPIO_NUC505_DT_PORT
		#error "GPIO_NUC505_DT_PORT should be defined"
	#endif

	#ifndef GPIO_NUC505_DT_PIN
		#error "GPIO_NUC505_DT_PIN should be defined"
	#endif

	#ifndef GPIO_NUC505_DT_MODE
		#error "GPIO_NUC505_DT_MODE should be defined"
	#endif



	#define STATIC_DEV_DATA_STRUCT	\
		{							\
			GPIO_NUC505_DT_PORT ,	\
			GPIO_NUC505_DT_PIN ,	\
			GPIO_NUC505_DT_MODE		\
		}

#endif

#define MODULE_CONFIGURABLE_PARAMS_ARRAY	{	\
		{IOCTL_GPIO_NUC505_SET_PORT_PARAM , IOCTL_VOID ,GPIO_NUC505_API_PORT_STR, NOT_FOR_SAVE},\
		{IOCTL_GPIO_NUC505_SET_PIN_PARAM , IOCTL_VOID , GPIO_NUC505_API_PIN_STR, NOT_FOR_SAVE},	\
		{IOCTL_GPIO_NUC505_SET_MODE_PARAM , IOCTL_VOID ,GPIO_NUC505_API_MODE_STR, NOT_FOR_SAVE},\
	}

#include "add_component.h"

/* device specific defines should be undefined after calling #include "add_component.h" */
#undef GPIO_NUC505_DT_PORT
#undef GPIO_NUC505_DT_PIN
#undef GPIO_NUC505_DT_MODE
