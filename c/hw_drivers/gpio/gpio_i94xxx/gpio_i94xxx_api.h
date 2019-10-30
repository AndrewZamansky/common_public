
#ifndef _GPIO_I94XXX_API_H_
#define _GPIO_I94XXX_API_H_


#include "dev_management_api.h"
#include "gpio_api.h"


#define GPIO_I94XXX_API_PORT_STR   			"port"
#define GPIO_I94XXX_API_SINGLE_PIN_STR   	"single_pin"
#define GPIO_I94XXX_API_MODE_STR   			"mode"


#include "I94100.h"
#include "gpio.h"

#define GPIO_I94XXX_API_PORT_A		GPIOA_BASE
#define GPIO_I94XXX_API_PORT_B		GPIOB_BASE
#define GPIO_I94XXX_API_PORT_C		GPIOC_BASE
#define GPIO_I94XXX_API_PORT_D		GPIOD_BASE

enum  GPIO_I94XXX_API_IOCTLE_E
{
	IOCTL_GPIO_I94XXX_SET_PORT_PARAM = IOCTL_LAST_COMMON_GPIO_IOCTL + 1,
	IOCTL_GPIO_I94XXX_SET_SINGLE_PIN_NUMBER_PARAM,
	IOCTL_GPIO_I94XXX_SET_MODE_PARAM
};


#define GPIO_I94XXX_API_MODE_OUT_PP 	GPIO_MODE_OUTPUT
#define GPIO_I94XXX_API_MODE_IN		 	GPIO_MODE_INPUT


struct gpio_i94xxx_config_t {
	struct dev_desc_t *client_dev;
	uint32_t    port_num;
	uint8_t     pin_arr_size;
	uint8_t     *pin_arr;
	uint8_t     *pin_arr_idle_state;
	uint8_t     mode;
};
SET_CONFIG_TYPE(gpio_i94xxx, struct gpio_i94xxx_config_t);

#endif
