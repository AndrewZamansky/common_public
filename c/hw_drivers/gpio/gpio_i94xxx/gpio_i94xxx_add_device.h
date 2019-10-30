/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "gpio_i94xxx_api.h"
#include "src/gpio_i94xxx.h"

#ifndef GPIO_I94XXX_DT_PORT
	#error "GPIO_I94XXX_DT_PORT should be defined"
#endif

#ifndef GPIO_I94XXX_DT_PINS
	#error "GPIO_I94XXX_DT_PINS should be defined"
#else
	DECLARE_DT_UINT8_ARRAY(pins_arr, GPIO_I94XXX_DT_PINS);
#endif

#ifndef GPIO_I94XXX_DT_PINS_IDLE_STATE
	#error "GPIO_I94XXX_DT_PINS_IDLE_STATE should be defined"
#else
	DECLARE_DT_UINT8_ARRAY(pins_idle_state_arr, GPIO_I94XXX_DT_PINS_IDLE_STATE);
#endif

#ifndef GPIO_I94XXX_DT_MODE
	#error "GPIO_I94XXX_DT_MODE should be defined"
#endif

#if (GPIO_I94XXX_DT_MODE == GPIO_I94XXX_API_MODE_IN)
	#ifndef GPIO_I94XXX_DT_CLIENT_DEV
		#warning "GPIO_I94XXX_DT_CLIENT_DEV not defined"
		#define P_GPIO_CLIENT_DEV  NULL
	#else
		#define P_GPIO_CLIENT_DEV           \
				P_TO_STATIC_DEVICE_INST(GPIO_I94XXX_DT_CLIENT_DEV)
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(GPIO_I94XXX_DT_CLIENT_DEV);
	#endif
#else
	#define P_GPIO_CLIENT_DEV NULL
#endif


SET_STATIC_DEV_CONFIG(gpio_i94xxx) =
{
	P_GPIO_CLIENT_DEV,
	GPIO_I94XXX_DT_PORT,
	DT_UINT8_ARRAY_SIZE(pins_arr),
	DT_ARRAY_NAME(pins_arr),
	DT_ARRAY_NAME(pins_idle_state_arr),
	GPIO_I94XXX_DT_MODE
};


#include "add_static_dev.h"

/* device specific defines should be undefined
 * after calling #include "add_static_dev.h"
 */
#undef GPIO_I94XXX_DT_PORT
#undef GPIO_I94XXX_DT_PINS
#undef GPIO_I94XXX_DT_MODE
#undef GPIO_I94XXX_DT_CLIENT_DEV
#undef P_GPIO_CLIENT_DEV
#undef GPIO_I94XXX_DT_PINS_IDLE_STATE
