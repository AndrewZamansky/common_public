/*
 *
 * gpio_stm32f10x.h
 *
 */

#ifndef _GPIO_STM32F10X_H_
#define _GPIO_STM32F10X_H_

struct gpio_stm32f10x_runtime_t {
	uint16_t  pin_mask;
	uint8_t  init_done;
	uint8_t  pin_bitwise_curr_values[2];
	uint8_t  pin_bitwise_idle_values[2];
};
SET_RUNTIME_DATA_TYPE(gpio_stm32f10x, struct gpio_stm32f10x_runtime_t);


#endif
