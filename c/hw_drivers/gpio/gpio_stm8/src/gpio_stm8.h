/*
 *
 * gpio_stm8.h
 *
 *
 */

#ifndef _GPIO_STM8_H_
#define _GPIO_STM8_H_


struct gpio_stm8_instance_t {
	GPIO_TypeDef* GPIOx;
	uint8_t pin_num;
	uint8_t mode;
};

#endif
