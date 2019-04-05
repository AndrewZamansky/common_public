/*
 * Button Manager API:
 *
 * The button manager is a software api to serve two purpose, debounce and
 *    report when a button has met a condition. This condition can be a specific
 *    set of buttons described as an array of the buttons in the action state or
 *      in the report config which will return as one of the BTN_REPORT defines.
 *
 *
 * This API was made to be extensible to any kind of button, GPIO or even those
 *      located elsewhere like on a separate device using I2C communication.
 *
 *
 * To start, add device in device tree with a client device. This will invoke
 *      the client device's callback function with the callback number
 *      CALLBACK_BTN_STATE_CHANGED along with 2 parameters, the BTN_REPORT and
 *      the btn_action_states_t structure that invoked the command.
 *
 */
#ifndef _button_manager_API_H_
#define _button_manager_API_H_

#include "dev_management_api.h"


/*****************  defines  **************/


/**********  define API  types ************/


/*
 * Button Reports:
 *
 *      Group of #defines that give the current status of button state.
 *      Several states are defined and correlate to the group of buttons
 *      that are pressed, defined in the btn_action_t structure.
 */

#define	BTN_REPORT_PUSH                 0x01
#define	BTN_REPORT_RELEASE              0x02
#define	BTN_REPORT_HOLD_EVERY_0s1       0x04
#define	BTN_REPORT_HOLD_EVERY_1s        0x08
#define	BTN_REPORT_HOLD_ONCE_5s         0x10

//following are not implemented yet
#define	BTN_REPORT_SHORT_RELEASE        0x20
#define	BTN_REPORT_RELEASE_AFTER_5s     0x40



enum BUTTON_MANAGER_API_IOCTL_E
{
	IOCTL_BUTTON_MANAGER_CREATE_BUTTON_GROUP = IOCTL_LAST_COMMON_IOCTL + 1,
	IOCTL_BUTTON_MANAGER_ADD_ACTION,
	IOCTL_BUTTON_MANAGER_SET_GPIO_STATES_FOR_ACTION
};





/****** utilities for creating btn devices array *****/
#define  BTN_MANAGER_DECLARE_GPIO_DEV(gpio_dev)   \
				EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(gpio_dev)

#define  BTN_MANAGER_ADD_GPIO_DEV(gpio_dev)   \
				P_TO_STATIC_DEVICE_INST(gpio_dev)

struct buttons_manager_group_t
{
	struct dev_desc_t **gpio_dev_arr;
	uint8_t  gpio_dev_arr_size;
};

#define  BTN_MANAGER_SET_GPIO_DEV_ARRAY(buttons_manager_group, gpio_dev_arr) \
	{   \
		buttons_manager_group.gpio_dev_arr = gpio_dev_arr;   \
		buttons_manager_group.gpio_dev_arr_size =            \
					sizeof(gpio_dev_arr) / sizeof(struct dev_desc_t *); \
	}
/****************************************************/


typedef size_t button_manager_action_handle_t;

/*
 *  report_config - Configuration of button group, refers to the button
 *                   reports as shown above. This needs to be set according
 *                  to what you want the button manager to report.
 *
 *  callback_private_data - Data pointer to be passed to during callback .
 */
struct btn_add_action_t
{
	uint16_t  report_config;
	void     *callback_private_data;
};



/****** utilities for adding gpio states to action *****/
#define BTN_MANAGER_ADD_GPIO_STATE(gpio_dev, pins_array, size_of_pinns_array) \
			P_TO_STATIC_DEVICE_INST(gpio_dev), pins_array, size_of_pinns_array


#define MAX_NUMBER_OF_ALLOWED_ACTIVE_PINS    4
/* to prevent from user to put bigger pin_numbers_arr_size
 * the pins in pin_numbers_arr should be in ascending order.
 */
struct gpio_state_for_action_t
{
	struct dev_desc_t  *gpio_dev;
	uint8_t  pin_numbers_arr[MAX_NUMBER_OF_ALLOWED_ACTIVE_PINS];
	uint8_t  pin_numbers_arr_size;
};


#define  BTN_MANAGER_SET_GPIO_STATES_ARRAY(set_gpio_states, gpio_state_arr) \
	{   \
		set_gpio_states.gpio_state_arr = gpio_state_arr;   \
		set_gpio_states.gpio_state_arr_size =              \
			sizeof(gpio_state_arr) / sizeof(struct  gpio_state_for_action_t); \
	}

struct set_gpio_states_for_action_t
{
	button_manager_action_handle_t  action_handle;
	struct  gpio_state_for_action_t   *gpio_state_arr;
	uint8_t  gpio_state_arr_size;
};
/****************************************************/




/**********  define API  functions  ************/



#endif
