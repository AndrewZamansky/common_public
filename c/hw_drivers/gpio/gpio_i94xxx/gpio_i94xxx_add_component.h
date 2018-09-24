/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "gpio_i94xxx_api.h"
#include "src/gpio_i94xxx.h"

uint8_t gpio_i94xxx_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

#define MODULE_NAME               gpio_i94xxx
#define MODULE_IOCTL_FUNCTION     gpio_i94xxx_ioctl
#define MODULE_CALLBACK_FUNCTION  gpio_i94xxx_callback
#define MODULE_CONFIG_DATA_STRUCT_TYPE  struct gpio_i94xxx_config_t
//#define MODULE_RUNTIME_DATA_STRUCT_TYPE struct gpio_i94xxx_runtime_t

#ifdef DT_DEV_MODULE

	#ifndef GPIO_I94XXX_DT_PORT
		#error "GPIO_I94XXX_DT_PORT should be defined"
	#endif

	#ifndef GPIO_I94XXX_DT_PIN
		#error "GPIO_I94XXX_DT_PIN should be defined"
	#endif

	#ifndef GPIO_I94XXX_DT_MODE
		#error "GPIO_I94XXX_DT_MODE should be defined"
	#endif

	#if (GPIO_I94XXX_DT_MODE == GPIO_I94XXX_API_MODE_IN)
		#ifndef GPIO_I94XXX_DT_CLIENT_DEV
			#warning "GPIO_I94XXX_DT_CLIENT_DEV should be defined"
			#define P_GPIO_CLIENT_DEV NULL
		#else
			#define P_GPIO_CLIENT_DEV           \
			P_TO_STATIC_DEVICE_INST(GPIO_I94XXX_DT_CLIENT_DEV)
		#endif
	#else
		#define P_GPIO_CLIENT_DEV NULL
	#endif


	#define STATIC_DEV_DATA_STRUCT  \
		{                           \
			P_GPIO_CLIENT_DEV,      \
			GPIO_I94XXX_DT_PORT,    \
			0,                      \
			GPIO_I94XXX_DT_PIN,     \
			GPIO_I94XXX_DT_MODE     \
		}

#else /*for module */

/* add this #ifdef block to avoid unused variable warning*/
#ifdef CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION_BY_PARAMETER_NAMES
	static mapped_set_to_size_param_t  gpio_i94xxx_port_mapped_set[] = {
			{"a" , GPIO_I94XXX_API_PORT_A},
			{"b" , GPIO_I94XXX_API_PORT_B},
			{"c" , GPIO_I94XXX_API_PORT_C},
			{"d" , GPIO_I94XXX_API_PORT_D}
		};

	static mapped_set_to_size_param_t  gpio_i94xxx_mode_mapped_set[] = {
			{"out" , GPIO_I94XXX_API_MODE_OUT_PP},
			{"in"  , GPIO_I94XXX_API_MODE_IN}
		};
#endif

#define MODULE_CONFIGURABLE_PARAMS_ARRAY                          \
	{                                                             \
		{                                                         \
			GPIO_I94XXX_API_PORT_STR,                             \
			IOCTL_GPIO_I94XXX_SET_PORT_PARAM,                     \
			IOCTL_VOID,                                           \
			DEV_PARAM_TYPE_MAPPED_SET_TO_SIZE,                    \
			MAPPED_SET_TO_SIZE_PARAM(gpio_i94xxx_port_mapped_set) \
		},                                                        \
		{                                                         \
			GPIO_I94XXX_API_SINGLE_PIN_STR,                       \
			IOCTL_GPIO_I94XXX_SET_SINGLE_PIN_NUMBER_PARAM,        \
			IOCTL_VOID,                                           \
			DEV_PARAM_TYPE_UINT8,                                 \
			MAPPED_SET_DUMMY_PARAM()                              \
		},                                                        \
		{                                                         \
			GPIO_I94XXX_API_MODE_STR,                             \
			IOCTL_GPIO_I94XXX_SET_MODE_PARAM,                     \
			IOCTL_VOID,                                           \
			DEV_PARAM_TYPE_MAPPED_SET_TO_SIZE,                    \
			MAPPED_SET_TO_SIZE_PARAM(gpio_i94xxx_mode_mapped_set) \
		}                                                         \
	}

#endif

#include "add_component.h"

/* device specific defines should be undefined after calling #include "add_component.h" */
#undef GPIO_I94XXX_DT_PORT
#undef GPIO_I94XXX_DT_PIN
#undef GPIO_I94XXX_DT_MODE
#undef GPIO_I94XXX_DT_CLIENT_DEV
#undef P_GPIO_CLIENT_DEV
