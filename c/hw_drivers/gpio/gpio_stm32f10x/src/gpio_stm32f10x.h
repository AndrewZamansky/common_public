/*
 *
 * gpio_stm32f10x.h
 *
 *
 *
 *
 *
 */

#ifndef _GPIO_STM32F10X_H_
#define _GPIO_STM32F10X_H_

/********  includes *********************/

#include "src/_gpio_stm32f10x_prerequirements_check.h"

/********  types  *********************/

typedef struct {
	uint8_t port_num;
	uint16_t pin_num_mask;
	uint8_t mode;
} GPIO_STM32F10X_Instance_t;

#endif
