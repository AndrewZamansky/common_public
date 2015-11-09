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

#include "button_manager_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_button_manager_prerequirements_check.h" // should be after {button_manager_config.h,dev_managment_api.h}


/***************   typedefs    *******************/



typedef struct
{
	pdev_descriptor   this_dev;
	os_queue_t xQueue ;
	pdev_descriptor   client_dev;

	uint8_t num_of_buttons;
	pdev_descriptor   *server_dev;
	uint8_t		*idle_state;


	uint8_t num_of_actons;
	btn_action_t *btn_actions;

	uint8_t *curr_state;

} buttons_group_t;




#endif /* */
