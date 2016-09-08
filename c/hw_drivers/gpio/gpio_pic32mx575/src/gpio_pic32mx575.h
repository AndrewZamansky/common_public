/*
 *
 * gpio_pic32mx575.h
 *
 *
 *
 *
 *
 */

#ifndef _GPIO_PIC32MX575_H_
#define _GPIO_PIC32MX575_H_

/********  includes *********************/

#include "src/_gpio_pic32mx575_prerequirements_check.h"

/********  types  *********************/

typedef struct {
	size_t 	port_base_address;
	uint32_t pin_number_mask;
	uint16_t package_pin_number;
	uint8_t mode;
} gpio_pic32mx575_instance_t;

#endif
