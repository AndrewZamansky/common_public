
#ifndef _GPIO_API_H_
#define _GPIO_API_H_

#include "dev_management_api.h"

enum GPIO_API_IOCTL_E
{
	IOCTL_GPIO_PIN_SET = IOCTL_LAST_COMMON_IOCTL + 1,
	IOCTL_GPIO_PIN_CLEAR,
	IOCTL_GPIO_PIN_READ,
	IOCTL_GPIO_TEST_IF_PINS_NOT_IN_IDLE,
	IOCTL_LAST_COMMON_GPIO_IOCTL
};


struct gpio_api_read_t {
	uint8_t   values_arr_size;
	uint8_t   *pin_bitwise_curr_values;
	uint8_t const *pin_bitwise_idle_values;
};

#endif
