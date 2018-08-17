/*
 * file : BUTTON_MANAGER.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _BUTTON_MANAGER_H
#define _BUTTON_MANAGER_H

#include "dev_management_api.h"
#include "os_wrapper.h"

/***************   typedefs    *******************/


struct button_manager_runtime_t {
	os_queue_t            xButtonQueue;
	uint8_t               *curr_button_state;
	uint8_t               manager_state;
	uint32_t              hold_count;
	uint32_t              queue_wait_delay;
};

struct button_manager_config_t {
	struct dev_desc_t     *client_dev;
	struct dev_desc_t *   *server_dev;
	struct btn_action_states_t   *btn_actions;
	uint8_t		          *idle_state;
	uint8_t               num_buttons;
	uint8_t               num_actions;
};


#endif /* */
