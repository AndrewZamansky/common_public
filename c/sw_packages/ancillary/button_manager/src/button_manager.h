/*
 * file : button_manager.h
 *
 *
 */

#ifndef _BUTTON_MANAGER_H
#define _BUTTON_MANAGER_H

#include "dev_management_api.h"
#include "os_wrapper.h"

/***************   typedefs    *******************/


struct button_manager_runtime_t {
	os_queue_t  xButtonQueue;
	struct gpio_api_read_t *curr_gpio_state_arr;
	uint8_t     manager_state;
	uint32_t    hold_count;
	uint32_t    queue_wait_delay;
	uint8_t     state;
	uint32_t    curr_event_indx;
};

struct button_manager_config_t {
	struct dev_desc_t     *client_dev;
	struct dev_desc_t     **gpio_devs_arr;
	struct btn_event_t   *btn_events_arr;
	uint32_t              num_of_events;
	uint8_t               num_of_gpio_devs;
};


#endif /* */
