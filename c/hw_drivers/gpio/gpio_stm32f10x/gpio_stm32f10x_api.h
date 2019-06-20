
#ifndef _GPIO_STM32F10X_API_H_
#define _GPIO_STM32F10X_API_H_

#include "gpio_api.h"

/**********  define API  types ************/


#define GPIO_STM32F10X_API_PORT_STR   	"port"
#define GPIO_STM32F10X_API_PIN_STR   	"pin"
#define GPIO_STM32F10X_API_MODE_STR   	"mode"



enum GPIO_STM32F10X_API_ioctl_t
{
	IOCTL_GPIO_STM32F10X_SET_PORT_PARAM = IOCTL_LAST_COMMON_GPIO_IOCTL + 1,
	IOCTL_GPIO_STM32F10X_SET_PIN_PARAM,
	IOCTL_GPIO_STM32F10X_SET_MODE_PARAM
};



#define	GPIO_STM32F10X_API_PORT_A	0
#define	GPIO_STM32F10X_API_PORT_B	1
#define	GPIO_STM32F10X_API_PORT_C	2
#define	GPIO_STM32F10X_API_PORT_D	3


#define GPIO_STM32F10X_API_MODE_OUT_PP 0x10

/**********  define API  functions  ************/

#endif
