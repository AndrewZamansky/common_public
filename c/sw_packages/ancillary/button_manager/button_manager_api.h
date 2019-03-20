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
#define	BTN_REPORT_SHORT_RELEASE        0x02
#define	BTN_REPORT_HOLD_EVERY_0s1       0x04
#define	BTN_REPORT_HOLD_EVERY_1s        0x08
#define	BTN_REPORT_HOLD_ONCE_5s         0x10
#define	BTN_REPORT_RELEASE_AFTER_5s     0x20

/*
 * Button Action Structure (needs to be made for each action case):
 *
 *  Description - Used to check the hold time and state of which will return
 *                  a request for action to the client device that should
 *                  be specified in the device tree.
 *
 *      report_config - Configuration of button group, refers to the button
 *                        reports as shown above. This needs to be set according
 *                        to what you want the button manager to report.
 *
 *      callback_private_data - (Optional) Data to be sent to host device.
 *                                This can be additional data like I2C byte
 *                                or some sort of msg indicating something
 *                                other than the #defines listed above.
 *
 */






typedef size_t button_manager_action_handle_t;

struct button_manager_requested_gpio_values_t {
	uint8_t   values_arr_size;
	uint8_t   *pin_bitwise_requested_values_arr;
};

struct btn_action_t
{

	uint8_t  report_config;
	void     *callback_private_data;
	struct   button_manager_requested_gpio_values_t *req_gpio_values_arr;

};


#define BTN_MANAGER_ADD_GPIO_STATE(gpio_dev, pins_array, size_of_pinns_array) \
			P_TO_STATIC_DEVICE_INST(loopback_button_dev), \
			pins_array, size_of_pinns_array


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


struct set_gpio_states_for_action_t
{
	button_manager_action_handle_t  action_handle;
	struct  gpio_state_for_action_t   *gpio_state_arr;
	uint8_t  gpio_state_arr_size;
};


struct btn_add_action_t
{
	uint8_t  report_config;
	void     *callback_private_data;
};

struct buttons_manager_group_t
{
	struct dev_desc_t **gpio_dev_arr;
	uint8_t  gpio_dev_arr_size;
};


enum BUTTON_MANAGER_API_IOCTL_E
{
	IOCTL_BUTTON_MANAGER_CREATE_BUTTON_GROUP = IOCTL_LAST_COMMON_IOCTL + 1,
	IOCTL_BUTTON_MANAGER_ADD_ACTION,
	IOCTL_BUTTON_MANAGER_SET_GPIO_STATES_FOR_ACTION
};

/**********  define API  functions  ************/



#endif
