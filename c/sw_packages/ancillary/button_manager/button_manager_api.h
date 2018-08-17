/*
 * Button Manager API:
 *
 * The button manager is a software api to serve two purpose, debounce and
 *      report when a button has met a condition. This condition can be a specific
 *      set of buttons described as an array of the buttons in the action state or
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
 *      action_state -  The button state in which activates the report config.
 *                        This is the pointer to array that indicates the
 *                        button presses active state in which they follow
 *                        the server_dev's array order.
 *
 */

struct btn_action_states_t
{

	uint8_t     report_config;
	void        *callback_private_data;
	uint8_t     *action_state;

};


/*
 * Button Manager Group Structure (needs to be made for each button combo):
 *
 *  Description - structure to be passed back and forth
 *
 *      server_dev - Device to retrieve button state. This should have the
 *                      IOCTL case IOCTL_ADD_ISR_CALLBACK_DEV and
 *                      IOCTL_GPIO_PIN_READ. IOCTL_ADD_ISR_CALLBACK_DEV is to
 *                      add/activate the device callback function to invoke
 *                      the button manager's callback function when the
 *                      button is invoked. IOCTL_GPIO_PIN_READ is to read the
 *                       value of which the button would provide.
 *
 *      btn_actions - Button Action Structure that is defined as a double pointer,
 *                      meaning it can be an array of button actions.
 *                      WARNING: DO NOT ASSIGN DUPLICATES OF BTN_REPORTS.
 *
 *      idle_state -  Natural idle state of the buttons. This can describe what
 *                      would be typically be the button return value from
 *                      DEV_IOCTL_1_PARAMS(button_dev, IOCTL_GPIO_PIN_READ, &state)
 *                      its rested state.
 *
 *      num_actions - Number of actions (1-6) that defines the number of
 *                      array elements described in btn_actions.
 *
 *      num_buttons - Number of buttons in the group. This is the number of
 *                      GPIO Devices you wish to read from.
 *
 *  Example (Initialization):
 *
 *  #include "button_manager_api.h"
 *
 *  extern struct dev_desc_t *button_manager_dev;
 *  extern struct dev_desc_t *gpio_dev;
 *
 *  uint8_t some_thread(void *aHandle)
 *  {
 *      struct buttons_manager_group_t init_button_struct;
 *
 *      uint8_t action_state = 0;   //active low
 *      uint8_t idle_state = 4;
 *
 *      init_button_struct.server_dev = &gpio_dev;
 *      init_button_struct.btn_actions->report_config = (uint8_t)BTN_REPORT_PUSH;
 *      init_button_struct.btn_actions->action_state = &action_state;
 *      init_button_struct.idle_state = &idle_state;
 *      init_button_struct.num_actions = 1;
 *      init_button_struct.num_buttons = 1;
 *
 *      DEV_IOCTL_1_PARAMS(button_manager_dev,
 *                IOCTL_BUTTON_MANAGER_CONFIG_GROUP, &init_button_struct);
 *      DEV_IOCTL_0_PARAMS(button_manager_dev, IOCTL_DEVICE_START);
 *
 *  }
 *
 */

struct buttons_manager_group_t
{
	struct dev_desc_t *   *server_dev;
	struct btn_action_states_t    *btn_actions;
	uint8_t               *idle_state;
	uint8_t               num_actions;
	uint8_t               num_buttons;
};


typedef enum
{
	IOCTL_BUTTON_MANAGER_CONFIG_GROUP = IOCTL_LAST_COMMON_IOCTL + 1,
}BUTTON_MANAGER_API_ioctl_t;

/**********  define API  functions  ************/



#endif
