/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "gpio_nuc505_api.h"


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


#define STATIC_DEV_DATA_STRUCT_TYPE	GPIO_NUC505_Instance_t
#define STATIC_DEV_DATA_STRUCT	\
	{							\
		GPIO_NUC505_DT_PORT ,	\
		GPIO_NUC505_DT_PIN ,	\
		GPIO_NUC505_DT_MODE		\
	}

#define	STATIC_DEV_IOCTL_FUNCTION	gpio_nuc505_ioctl
#include "add_static_dev.h"

#undef GPIO_NUC505_DT_PORT
#undef GPIO_NUC505_DT_PIN
#undef GPIO_NUC505_DT_MODE
