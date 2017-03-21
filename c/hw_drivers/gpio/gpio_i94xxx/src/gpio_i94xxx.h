/*
 *
 * gpio_i94xxx.h
 *
 *
 *
 *
 *
 */

#ifndef _GPIO_I94XXX_H_
#define _GPIO_I94XXX_H_

/********  includes *********************/

#include "src/_gpio_i94xxx_prerequirements_check.h"

/********  types  *********************/

typedef struct {
	uint32_t 	port_num;
	uint16_t 	pin_num_mask;
	uint8_t mode;
} gpio_i94xxx_instance_t;

#endif
