/*
 * Button Manager API:
 *
 * The button manager is a software api to serve two purpose, debounce and
 *    report when a button has met a condition. This condition can be a specific
 *    set of buttons described as an array of the buttons in the some state or
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
 *      the callback_private_data that invoked the command.
 *
 */
#ifndef _button_manager_API_H_
#define _button_manager_API_H_

#include "dev_management_api.h"


/*
 * Button Reports:
 *
 *
 */

#define BTN_REPORT_PUSH                  0x01
#define BTN_REPORT_RELEASE               0x02
#define BTN_REPORT_HOLD_EVERY_0s1        0x04
#define BTN_REPORT_HOLD_EVERY_1s         0x08
#define BTN_REPORT_HOLD_ONCE_5s          0x10
#define BTN_REPORT_RELEASED_BEFOR_500mS  0x20



enum BUTTON_MANAGER_API_IOCTL_E
{
	IOCTL_BUTTON_MANAGER_CREATE_BUTTON_GROUP = IOCTL_LAST_COMMON_IOCTL + 1,
	IOCTL_BUTTON_MANAGER_ADD_REPORTED_EVENT
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


typedef void* button_manager_reported_event_handle_t;


/****** utility for adding state of specific gpio *****/
#define BTN_MANAGER_ADD_GPIO_STATE(gpio_dev, pins_array, size_of_pinns_array) \
			P_TO_STATIC_DEVICE_INST(gpio_dev), pins_array, size_of_pinns_array


#define MAX_NUMBER_OF_ALLOWED_ACTIVE_PINS    4
/* to prevent from user to put bigger pin_numbers_arr_size
 * the pins in pin_numbers_arr should be in ascending order.
 */
struct gpio_state_for_reported_event_t
{
	struct dev_desc_t  *gpio_dev;
	uint8_t  pin_numbers_arr[MAX_NUMBER_OF_ALLOWED_ACTIVE_PINS];
	uint8_t  pin_numbers_arr_size;
};


/****** utility for adding states of all gpios to reported event *****/
#define  BTN_MANAGER_SET_GPIO_STATES_ARRAY(reported_event, _gpio_state_arr) \
	{   \
		reported_event.gpio_states.gpio_state_arr = _gpio_state_arr;     \
		reported_event.gpio_states.gpio_state_arr_size =                 \
			sizeof(_gpio_state_arr) / sizeof(                 \
					struct  gpio_state_for_reported_event_t); \
	}

struct set_gpio_states_for_reported_event_t
{
	struct  gpio_state_for_reported_event_t   *gpio_state_arr;
	uint8_t  gpio_state_arr_size;
};
/****************************************************/


/*
 *  report_config - Configuration of button group, refers to the button
 *                   reports as shown above. This needs to be set according
 *                  to what you want the button manager to report.
 *
 *  callback_private_data - Data pointer to be passed to during callback .
 */
struct btn_add_reported_event_t
{
	uint16_t  report_config;
	void     *callback_private_data;
	struct  set_gpio_states_for_reported_event_t  gpio_states;
};



struct button_manager_config_t {
	struct dev_desc_t     *client_dev;
	struct dev_desc_t     **gpio_devs_arr;
	struct btn_event_t   *btn_events_arr;
	uint32_t              num_of_events;
	uint8_t               num_of_gpio_devs;
};
SET_CONFIG_TYPE(button_manager, struct button_manager_config_t);

#endif
