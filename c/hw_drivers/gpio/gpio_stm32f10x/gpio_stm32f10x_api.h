
#ifndef _GPIO_STM32F10X_API_H_
#define _GPIO_STM32F10X_API_H_

#include "gpio_stm32f10x_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "gpio_api.h"
#include "src/_gpio_stm32f10x_prerequirements_check.h" // should be after {uart_stm32f10x_config.h,dev_managment_api.h}

/**********  define API  types ************/


#define GPIO_STM32F10X_API_PORT_STR   	"port"
#define GPIO_STM32F10X_API_PIN_STR   	"pin"
#define GPIO_STM32F10X_API_MODE_STR   	"mode"



typedef enum
{
	IOCTL_GPIO_STM32F10X_SET_PORT_PARAM = IOCTL_LAST_COMMON_GPIO_IOCTL+1,
	IOCTL_GPIO_STM32F10X_SET_PIN_PARAM,
	IOCTL_GPIO_STM32F10X_SET_MODE_PARAM
}GPIO_STM32F10X_API_ioctl_t;



#define	GPIO_STM32F10X_API_PORT_A	0
#define	GPIO_STM32F10X_API_PORT_B	1
#define	GPIO_STM32F10X_API_PORT_C	2
#define	GPIO_STM32F10X_API_PORT_D	3


#define GPIO_STM32F10X_API_MODE_OUT_PP 0x10

/**********  define API  functions  ************/




uint8_t  gpio_stm32f10x_api_init_dev_descriptor(pdev_descriptor aDevDescriptor);


#include "src/_gpio_stm32f10x_static_dev_macros.h"

#define GPIO_STM32F10X_API_CREATE_STATIC_DEV(dev,dev_name ,port , pin , mode) \
		__GPIO_STM32F10X_API_CREATE_STATIC_DEV(dev,dev_name ,port , pin , mode)



#endif
