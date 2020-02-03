
#ifndef _GPIO_STM32F10X_API_H_
#define _GPIO_STM32F10X_API_H_

#include "gpio_api.h"


#define GPIO_STM32F10X_API_PORT_STR   "port"
#define GPIO_STM32F10X_API_PIN_STR    "pin"
#define GPIO_STM32F10X_API_MODE_STR   "mode"



enum GPIO_STM32F10X_API_ioctl_t
{
	IOCTL_GPIO_STM32F10X_SET_PORT_PARAM = IOCTL_LAST_COMMON_GPIO_IOCTL + 1,
	IOCTL_GPIO_STM32F10X_SET_PIN_PARAM,
	IOCTL_GPIO_STM32F10X_SET_MODE_PARAM
};



#define  GPIO_STM32F10X_API_PORT_A  0
#define  GPIO_STM32F10X_API_PORT_B  1
#define  GPIO_STM32F10X_API_PORT_C  2
#define  GPIO_STM32F10X_API_PORT_D  3


#define  GPIO_STM32F10X_API_MODE_OUT_PP  0x10

struct gpio_stm32f10x_config_t {
	struct dev_desc_t *client_dev;
	uint32_t    port_num;
	uint8_t     pin_arr_size;
	uint8_t     *pin_arr;
	uint8_t     *pin_arr_idle_state;
	uint8_t     mode;
};
SET_CONFIG_TYPE(gpio_stm32f10x, struct gpio_stm32f10x_config_t);

#endif
