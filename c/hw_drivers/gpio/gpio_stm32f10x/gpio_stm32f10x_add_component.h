/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  	"gpio_stm32f10x_api.h"
#include 	"src/gpio_stm32f10x.h"


#define	MODULE_NAME					gpio_stm32f10x
#define	MODULE_IOCTL_FUNCTION		gpio_stm32f10x_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE  struct gpio_stm32f10x_config_t
#define MODULE_RUNTIME_DATA_STRUCT_TYPE struct gpio_stm32f10x_runtime_t

#ifdef DT_DEV_MODULE

	#ifndef GPIO_STM32F10X_DT_PORT
		#error "GPIO_STM32F10X_DT_PORT should be defined"
	#endif

	#ifndef GPIO_STM32F10X_DT_PINS
		#error "GPIO_STM32F10X_DT_PINS should be defined"
	#else
		DECLARE_DT_UINT8_ARRAY(pins_arr, GPIO_STM32F10X_DT_PINS);
	#endif

	#ifndef GPIO_STM32F10X_DT_PINS_IDLE_STATE
		#error "GPIO_STM32F10X_DT_PINS_IDLE_STATE should be defined"
	#else
		DECLARE_DT_UINT8_ARRAY( \
				pins_idle_state_arr, GPIO_STM32F10X_DT_PINS_IDLE_STATE);
	#endif

	#ifndef GPIO_STM32F10X_DT_MODE
		#error "GPIO_STM32F10X_DT_MODE should be defined"
	#endif

	#if (GPIO_STM32F10X_DT_MODE == GPIO_STM32F10X_API_MODE_IN)
		#ifndef GPIO_STM32F10X_DT_CLIENT_DEV
			#warning "GPIO_STM32F10X_DT_CLIENT_DEV not defined"
			#define P_GPIO_CLIENT_DEV  NULL
		#else
			#define P_GPIO_CLIENT_DEV           \
					P_TO_STATIC_DEVICE_INST(GPIO_STM32F10X_DT_CLIENT_DEV)
			EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(\
										GPIO_STM32F10X_DT_CLIENT_DEV);
		#endif
	#else
		#define P_GPIO_CLIENT_DEV NULL
	#endif


	#define STATIC_DEV_DATA_STRUCT  \
		{                           \
			P_GPIO_CLIENT_DEV,      \
			GPIO_STM32F10X_DT_PORT,    \
			DT_UINT8_ARRAY_SIZE(pins_arr),  \
			DT_ARRAY_NAME(pins_arr),\
			DT_ARRAY_NAME(pins_idle_state_arr),\
			GPIO_STM32F10X_DT_MODE     \
		}


#endif

#define MODULE_CONFIGURABLE_PARAMS_ARRAY	{	\
		{IOCTL_GPIO_STM32F10X_SET_PORT_PARAM , IOCTL_VOID ,\
					GPIO_STM32F10X_API_PORT_STR, NOT_FOR_SAVE}, 	\
		{IOCTL_GPIO_STM32F10X_SET_PIN_PARAM , IOCTL_VOID , \
							GPIO_STM32F10X_API_PIN_STR, NOT_FOR_SAVE},	\
		{IOCTL_GPIO_STM32F10X_SET_MODE_PARAM , IOCTL_VOID ,\
								GPIO_STM32F10X_API_MODE_STR, NOT_FOR_SAVE},	\
	}

#include "add_component.h"

/* device specific defines should be undefined
 * after calling #include "add_component.h"
 */
#undef GPIO_STM32F10X_DT_PORT
#undef GPIO_STM32F10X_DT_PINS
#undef GPIO_STM32F10X_DT_MODE
#undef GPIO_STM32F10X_DT_CLIENT_DEV
#undef P_GPIO_CLIENT_DEV
#undef GPIO_STM32F10X_DT_PINS_IDLE_STATE
