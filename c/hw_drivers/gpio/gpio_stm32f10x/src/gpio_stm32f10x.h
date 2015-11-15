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

#include "gpio_stm32f10x_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_gpio_stm32f10x_prerequirements_check.h" // should be after {gpio_stm32f10x_config.h,dev_managment_api.h}

/********  types  *********************/

typedef struct {
	uint8_t port_num;
	uint8_t pin_num_mask;
	uint8_t mode;
} GPIO_STM32F10X_Instance_t;

#endif
