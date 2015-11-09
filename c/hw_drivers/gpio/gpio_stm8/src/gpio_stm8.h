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

#include "gpio_stm8_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_gpio_stm8_prerequirements_check.h" // should be after {gpio_stm8_config.h,dev_managment_api.h}

/********  types  *********************/

typedef struct {
	GPIO_TypeDef* GPIOx;
	uint8_t pin_num;
	uint8_t mode;
} GPIO_STM8_Instance_t;

#endif
