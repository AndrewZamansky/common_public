/*
 *
 * gpio_i94xxx.h
 *
 *
 */

#ifndef _GPIO_I94XXX_H_
#define _GPIO_I94XXX_H_

/********  includes *********************/


/********  types  *********************/

struct gpio_i94xxx_config_t
{
	struct dev_desc_t *client_dev;
	uint32_t    port_num;
	uint32_t    pin_num;
	uint16_t    pin_num_mask;
	uint8_t     mode;
};

//struct gpio_i94xxx_runtime_t
//{
//};

#endif
