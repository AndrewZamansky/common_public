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
	uint8_t     pin_arr_size;
	uint8_t     *pin_arr;
	uint8_t     *pin_arr_idle_state;
	uint8_t     mode;
};

struct gpio_i94xxx_runtime_t
{
	uint16_t  pin_mask;
	uint8_t  init_done;
	uint8_t  pin_bitwise_curr_values[2];
	uint8_t  pin_bitwise_idle_values[2];
};

#endif
