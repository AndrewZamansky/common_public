/*
 *
 * gpio_i94xxx.h
 *
 *
 */

#ifndef _GPIO_I94XXX_H_
#define _GPIO_I94XXX_H_

#include "dev_management_api.h"

struct gpio_i94xxx_runtime_t
{
	uint16_t  pin_mask;
	uint8_t  init_done;
	uint8_t  pin_bitwise_curr_values[2];
	uint8_t  pin_bitwise_idle_values[2];
};
SET_RUNTIME_DATA_TYPE(gpio_i94xxx, struct gpio_i94xxx_runtime_t);

#endif
