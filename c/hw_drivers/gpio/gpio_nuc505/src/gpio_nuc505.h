/*
 *
 * gpio_nuc505.h
 *
 *
 *
 *
 *
 */

#ifndef _GPIO_NUC505_H_
#define _GPIO_NUC505_H_

/********  includes *********************/

#include "src/_gpio_nuc505_prerequirements_check.h"

/********  types  *********************/

typedef struct {
	void *port_num;
	uint8_t pin_num_mask;
	uint8_t mode;
} gpio_nuc505_instance_t;

#endif
