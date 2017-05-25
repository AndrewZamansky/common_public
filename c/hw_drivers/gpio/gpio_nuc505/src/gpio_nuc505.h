/*
 *
 * gpio_nuc505.h
 *
 *
 */

#ifndef _GPIO_NUC505_H_
#define _GPIO_NUC505_H_

/********  includes *********************/


/********  types  *********************/

struct gpio_nuc505_cfg_t {
	uint32_t 	port_num;
	uint16_t 	pin_num_mask;
	uint8_t mode;
};

#endif
