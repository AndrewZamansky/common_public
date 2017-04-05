/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "gpio_nuc505_api.h"
#include "src/gpio_nuc505.h"

uint8_t gpio_nuc505_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

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

#else /*for module */

/* add this #ifdef block to avoid unused variable warning*/
#ifdef CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION_BY_PARAMETER_NAMES
	static mapped_set_to_size_param_t  gpio_nuc505_port_mapped_set[] = {
			{"a" , GPIO_NUC505_API_PORT_A},
			{"b" , GPIO_NUC505_API_PORT_B},
			{"c" , GPIO_NUC505_API_PORT_C},
			{"d" , GPIO_NUC505_API_PORT_D}
		};

	static mapped_set_to_size_param_t  gpio_nuc505_mode_mapped_set[] = {
			{"out" , GPIO_NUC505_API_MODE_OUT_PP},
			{"in"  , GPIO_NUC505_API_MODE_IN}
		};
#endif

#define MODULE_CONFIGURABLE_PARAMS_ARRAY	{	\
		{GPIO_NUC505_API_PORT_STR 		 ,IOCTL_GPIO_NUC505_SET_PORT_PARAM , IOCTL_VOID , 	\
			DEV_PARAM_TYPE_MAPPED_SET_TO_SIZE , MAPPED_SET_TO_SIZE_PARAM(gpio_nuc505_port_mapped_set) },	\
																							\
		{GPIO_NUC505_API_SINGLE_PIN_STR  ,IOCTL_GPIO_NUC505_SET_SINGLE_PIN_NUMBER_PARAM  , \
			IOCTL_VOID , DEV_PARAM_TYPE_UINT8,  MAPPED_SET_DUMMY_PARAM() },					\
																							\
		{GPIO_NUC505_API_MODE_STR 		 ,IOCTL_GPIO_NUC505_SET_MODE_PARAM , IOCTL_VOID , 	\
				DEV_PARAM_TYPE_MAPPED_SET_TO_SIZE , MAPPED_SET_TO_SIZE_PARAM(gpio_nuc505_mode_mapped_set)}		\
	}

#endif

#include "add_component.h"

/* device specific defines should be undefined after calling #include "add_component.h" */
#undef GPIO_NUC505_DT_PORT
#undef GPIO_NUC505_DT_PIN
#undef GPIO_NUC505_DT_MODE
