#ifndef _add_static_device_step1
#define _add_static_device_step1

#include  "gpio_nuc505_api.h"

#undef _add_static_device_step2

#undef GPIO_NUC505_DT_DEV_NAME
#undef GPIO_NUC505_DT_PORT
#undef GPIO_NUC505_DT_PIN
#undef GPIO_NUC505_DT_MODE


#elif !defined(_add_static_device_step2)
#define _add_static_device_step2

#undef _add_static_device_step1

#ifndef GPIO_NUC505_DT_DEV_NAME
#error "GPIO_NUC505_DT_DEV_NAME should be defined"
#endif

#ifndef GPIO_NUC505_DT_PORT
#error "GPIO_NUC505_DT_PORT should be defined"
#endif

#ifndef GPIO_NUC505_DT_PIN
#error "GPIO_NUC505_DT_PIN should be defined"
#endif

#ifndef GPIO_NUC505_DT_MODE
#error "GPIO_NUC505_DT_MODE should be defined"
#endif

#include "src/gpio_nuc505.h"
uint8_t gpio_nuc505_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);


GPIO_NUC505_Instance_t STATIC_DEVICE_INNER_INST(GPIO_NUC505_DT_DEV_NAME)  =
{
	GPIO_NUC505_DT_PORT ,
	GPIO_NUC505_DT_PIN ,
	GPIO_NUC505_DT_MODE
};
CREATE_STATIC_DEVICE( GPIO_NUC505_DT_DEV_NAME ,
	&STATIC_DEVICE_INNER_INST(GPIO_NUC505_DT_DEV_NAME)  ,
	gpio_nuc505_ioctl ,  DEV_API_dummy_pwrite_func ,
	DEV_API_dummy_pread_func , DEV_API_dummy_callback_func);


#undef CURRENT_DEV

#endif
