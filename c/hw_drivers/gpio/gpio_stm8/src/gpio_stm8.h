/*
 *
 * gpio_stm8.h
 *
 *
 *
 *
 *
 */

#ifndef _GPIO_STM8_H_
#define _GPIO_STM8_H_

/********  includes *********************/

#include "src/_gpio_stm8_prerequirements_check.h"

/********  types  *********************/

typedef struct {
	GPIO_TypeDef* GPIOx;
	uint8_t pin_num;
	uint8_t mode;
} GPIO_STM8_Instance_t;

#endif
