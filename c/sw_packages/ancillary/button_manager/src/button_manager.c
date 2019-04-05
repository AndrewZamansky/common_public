/*
 *
 * file :   button_manager.c
 *
 *
 *
 *
 */



/********  includes *********************/
#include "_project_tasks_defines.h"

#include "dev_management_api.h"
#include "os_wrapper.h"

#include "button_manager_api.h"
#include "button_manager.h"

#include "gpio_api.h"


#ifndef  BUTTON_MANAGER_CONFIG_TASK_PRIORITY
	#error  "BUTTON_MANAGER_CONFIG_TASK_PRIORITY should be define in project"
#endif
#ifndef  BUTTON_MANAGER_CONFIG_TASK_STACK_SIZE
	#error  "BUTTON_MANAGER_CONFIG_TASK_STACK_SIZE should be define in project"
#endif

/********  defines *********************/


/********  types  *********************/

struct button_manager_requested_gpio_values_t {
	uint8_t   values_arr_size;
	uint8_t   *pin_bitwise_requested_values_arr;
};

struct btn_action_t {
	uint16_t  report_config;
	void     *callback_private_data;
	struct   button_manager_requested_gpio_values_t *req_gpio_values_arr;
};
/********  externals *********************/


/********  local defs *********************/

#define  DEBOUNCE_DELAY  10
#define  VERY_LONG_WAIT  100000


/**********   external variables    **************/



/***********   local variables    **************/

typedef enum
{
	BTN_STATE_IDLE,
	BTN_STATE_DEBOUNCING_ON_PUSH,
	BTN_STATE_HOLD,
	BTN_STATE_DEBOUNCING_ON_RELEASE
}btn_state_t;


typedef struct
{
	uint8_t dummy;
} xMessage_t;



/*
 * Function:        button_manager_callback
 *
 * Parameters:
 *
 * Returns:
 * Side effects:
 * Description:
*/
uint8_t button_manager_callback(
		struct dev_desc_t *adev ,const uint8_t aCallback_num ,
		void * aCallback_param1, void * aCallback_param2)
{
	struct button_manager_runtime_t *runtime_handle;
	os_queue_t xButtonQueue ;
	xMessage_t xTxMessage;

	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);

	xButtonQueue = runtime_handle->xButtonQueue;

	if (NULL == xButtonQueue)
	{
		return 1;
	}

	os_queue_send_without_wait( xButtonQueue, ( void * ) &xTxMessage);

	return 0;
}


static uint8_t compare_gpio_state(struct gpio_api_read_t *curr_gpio_state_arr,
		uint8_t num_of_gpio_devs, struct btn_action_t *btn_action)
{
	struct gpio_api_read_t *curr_gpio_state;
	uint8_t values_arr_size;
	uint8_t i;
	struct   button_manager_requested_gpio_values_t *req_gpio_values_arr;
	struct   button_manager_requested_gpio_values_t *req_gpio_values;

	req_gpio_values_arr = btn_action->req_gpio_values_arr;
	if (NULL == req_gpio_values_arr)
	{
		return 0;
	}
	for(i = 0; i < num_of_gpio_devs; i++)
	{
		curr_gpio_state = &curr_gpio_state_arr[i];
		values_arr_size = curr_gpio_state->values_arr_size;
		req_gpio_values = &req_gpio_values_arr[i];
		if (memcmp(curr_gpio_state->pin_bitwise_curr_values,
				req_gpio_values->pin_bitwise_requested_values_arr,
				values_arr_size))
		{
			return 0;
		}
	}
	return 1;
}


uint8_t end_of_debounce_on_push(
		struct dev_desc_t *adev, uint32_t  *curr_action_indx)
{
	struct button_manager_runtime_t *runtime_handle;
	struct button_manager_config_t  *config_handle;
	struct dev_desc_t *   *gpio_devs_arr;
	struct dev_desc_t     *client_dev;
	uint8_t i;
	uint8_t action_idx;
	uint8_t num_of_gpio_devs;
	uint8_t num_of_actions;
	struct gpio_api_read_t *curr_gpio_state_arr;
	uint8_t action_found;
	struct btn_action_t *btn_actions_arr;

	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);
	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);

	curr_gpio_state_arr = runtime_handle->curr_gpio_state_arr;

	gpio_devs_arr = config_handle->gpio_devs_arr;
	client_dev = config_handle->client_dev;
	num_of_gpio_devs = config_handle->num_of_gpio_devs;
	num_of_actions = config_handle->num_of_actions;
	btn_actions_arr = config_handle->btn_actions_arr;

	for(i = 0; i < num_of_gpio_devs; i++)
	{
		DEV_IOCTL_1_PARAMS(
				gpio_devs_arr[i], IOCTL_GPIO_PIN_READ, &curr_gpio_state_arr[i]);
	}

	action_found = 0;
	for (action_idx = 0; action_idx < num_of_actions ; action_idx++)
	{
		action_found = compare_gpio_state(curr_gpio_state_arr,
				num_of_gpio_devs, &btn_actions_arr[action_idx]);

		if (action_found)
		{
			*curr_action_indx = action_idx;
			if ((NULL != client_dev) &&
				(BTN_REPORT_PUSH & btn_actions_arr[action_idx].report_config))
			{
				DEV_CALLBACK_2_PARAMS(client_dev, CALLBACK_BTN_STATE_CHANGED,
					(void *)BTN_REPORT_PUSH,
					btn_actions_arr[action_idx].callback_private_data);
			}
			return 1;
		}
	}

	return 0;
}


uint8_t button_state_hold(struct dev_desc_t *adev, uint8_t action_idx)
{
	struct button_manager_runtime_t *runtime_handle;
	struct button_manager_config_t  *config_handle;
	struct dev_desc_t     *client_dev;
	struct btn_action_t *btn_actions_arr;
	uint32_t   hold_count;
	void *callback_private_data;

	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);
	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);

	client_dev = config_handle->client_dev;
	if (NULL == client_dev)
	{
		return 0;
	}
	hold_count = runtime_handle->hold_count;
	btn_actions_arr = config_handle->btn_actions_arr;
	callback_private_data = btn_actions_arr[action_idx].callback_private_data;
	hold_count += 128;
	if(BTN_REPORT_HOLD_EVERY_0s1 & btn_actions_arr[action_idx].report_config)
	{
		DEV_CALLBACK_2_PARAMS( client_dev, CALLBACK_BTN_STATE_CHANGED,
			(void *)BTN_REPORT_HOLD_EVERY_0s1, callback_private_data);
	}
	if( 0x0 == (hold_count & 0x3ff) ) // enter each 1024
	{
		if(BTN_REPORT_HOLD_EVERY_1s & btn_actions_arr[action_idx].report_config)
		{
			DEV_CALLBACK_2_PARAMS( client_dev ,CALLBACK_BTN_STATE_CHANGED ,
				(void *)BTN_REPORT_HOLD_EVERY_1s, callback_private_data);
		}
	}
	if( 4992 == hold_count  ) // enter once ~5s
	{
		if (BTN_REPORT_HOLD_ONCE_5s & btn_actions_arr[action_idx].report_config)
		{
			DEV_CALLBACK_2_PARAMS( client_dev ,CALLBACK_BTN_STATE_CHANGED ,
				(void *)BTN_REPORT_HOLD_ONCE_5s, callback_private_data);
		}
	}

	runtime_handle->hold_count = hold_count;
	return 0;
}


uint8_t end_of_debounce_on_release(struct dev_desc_t *adev, uint8_t action_idx)
{
	struct button_manager_runtime_t *runtime_handle;
	struct button_manager_config_t  *config_handle;
	uint8_t idle_state_detected;
	struct dev_desc_t **gpio_devs_arr;
	uint8_t i;
	uint8_t num_of_gpio_devs;
	struct gpio_api_read_t *curr_gpio_state_arr;

	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);
	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);

	curr_gpio_state_arr = runtime_handle->curr_gpio_state_arr;
	gpio_devs_arr = config_handle->gpio_devs_arr;
	num_of_gpio_devs = config_handle->num_of_gpio_devs;

	for (i = 0; i < num_of_gpio_devs; i++)
	{
		DEV_IOCTL_1_PARAMS(
				gpio_devs_arr[i], IOCTL_GPIO_PIN_READ, &curr_gpio_state_arr[i]);
	}

	idle_state_detected = 1;
	for (i = 0; i < num_of_gpio_devs; i++)
	{
		struct gpio_api_read_t *curr_gpio_state;
		uint8_t values_arr_size;

		curr_gpio_state = &curr_gpio_state_arr[i];
		values_arr_size = curr_gpio_state->values_arr_size;
		if (memcmp(curr_gpio_state->pin_bitwise_curr_values,
				curr_gpio_state->pin_bitwise_idle_values, values_arr_size))
		{
			idle_state_detected = 0;
			break;
		}
	}

	if (idle_state_detected)
	{
		struct btn_action_t *btn_actions_arr;
		struct dev_desc_t     *client_dev;

		client_dev = config_handle->client_dev;
		btn_actions_arr = config_handle->btn_actions_arr;
		if ( (NULL != client_dev) &&
			(BTN_REPORT_RELEASE & btn_actions_arr[action_idx].report_config))
		{
			DEV_CALLBACK_2_PARAMS( client_dev, CALLBACK_BTN_STATE_CHANGED,
				(void *)BTN_REPORT_RELEASE,
				btn_actions_arr[action_idx].callback_private_data);
		}
		return 1;
	}
	return 0;
}

static void state_machine(struct dev_desc_t *adev,
		uint8_t event_was_received_from_gpio)
{
	struct button_manager_runtime_t *runtime_handle;
	uint8_t    manager_state;
	uint32_t   queue_wait_delay;
	uint32_t   curr_action_indx;

	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);

	manager_state = runtime_handle->manager_state;
	queue_wait_delay = runtime_handle->queue_wait_delay;
	curr_action_indx = runtime_handle->curr_action_indx;

	switch(manager_state)
	{
		default :
		case BTN_STATE_IDLE:
			if (event_was_received_from_gpio)
			{
				queue_wait_delay = DEBOUNCE_DELAY;
				manager_state = BTN_STATE_DEBOUNCING_ON_PUSH;
			}
			break;

		case BTN_STATE_DEBOUNCING_ON_PUSH:
			if (0 == event_was_received_from_gpio)
			{
				uint8_t  action_found;

				action_found = end_of_debounce_on_push(adev, &curr_action_indx);
				if (0 == action_found)
				{
					queue_wait_delay = DEBOUNCE_DELAY;
					manager_state = BTN_STATE_DEBOUNCING_ON_RELEASE;
				}
				else
				{
					runtime_handle->curr_action_indx = curr_action_indx;
					runtime_handle->hold_count = 0;
					queue_wait_delay = 128;
					manager_state = BTN_STATE_HOLD;
				}
			}
			break;

		case BTN_STATE_HOLD:
			if (0 == event_was_received_from_gpio)
			{
				button_state_hold(adev, curr_action_indx);
			}
			else
			{
				queue_wait_delay = DEBOUNCE_DELAY;
				manager_state = BTN_STATE_DEBOUNCING_ON_RELEASE;
			}
			break;
		case BTN_STATE_DEBOUNCING_ON_RELEASE:
			if (0 == event_was_received_from_gpio)
			{
				uint8_t idle_state_detected;

				idle_state_detected =
						end_of_debounce_on_release(adev, curr_action_indx);
				if (idle_state_detected)
				{
					queue_wait_delay = VERY_LONG_WAIT;// not used during idle
					manager_state = BTN_STATE_IDLE;
				}
			}
			break;
	}
	runtime_handle->manager_state = manager_state;
	runtime_handle->queue_wait_delay = queue_wait_delay;
}


/*
 * Function:        button_manager_task
 *
 * Parameters:
 *
 * Returns:
 * Side effects:
 * Description:
*/
static void button_manager_task( void *adev )
{
	struct button_manager_runtime_t *runtime_handle;
	os_queue_t xButtonQueue;
	xMessage_t xTxMessage;

	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);

	xButtonQueue =  os_create_queue( 1 , sizeof(xMessage_t ) );
	runtime_handle->xButtonQueue = xButtonQueue;

	runtime_handle->manager_state = BTN_STATE_IDLE;
	runtime_handle->queue_wait_delay = VERY_LONG_WAIT;// not used during idle

	if( 0 == xButtonQueue  )
	{
		CRITICAL_ERROR("failed to create OS queue");
	}

	while (1)
	{
		uint8_t    queueRetVal;
		uint8_t    event_was_received_from_gpio;

		queueRetVal = os_queue_receive_with_timeout(
			xButtonQueue , &( xTxMessage ), runtime_handle->queue_wait_delay) ;

		event_was_received_from_gpio =
				(OS_QUEUE_RECEIVE_SUCCESS == queueRetVal) ? 1 : 0;
		state_machine(adev, event_was_received_from_gpio);

		os_stack_test();
	}
}



#define STATE_CREATING_GROUP  0
#define STATE_ADDING_ACTIONS  1

static void create_button_group(struct button_manager_config_t *config_handle,
		struct button_manager_runtime_t *runtime_handle,
		struct buttons_manager_group_t *buttons_manager_group)
{
	struct dev_desc_t **gpio_dev_arr;
	uint8_t num_of_gpio_devs;
	struct gpio_api_read_t *curr_gpio_state_arr;

	if (STATE_CREATING_GROUP != runtime_handle->state)
	{
		CRITICAL_ERROR("can be called only when state = STATE_CREATING_GROUP");
	}

	num_of_gpio_devs = buttons_manager_group->gpio_dev_arr_size;
	gpio_dev_arr = (struct dev_desc_t **)malloc(
					num_of_gpio_devs * sizeof(struct dev_desc_t *));
	if (NULL == gpio_dev_arr)
	{
		CRITICAL_ERROR("no memory");
	}
	memcpy(gpio_dev_arr, buttons_manager_group->gpio_dev_arr,
						num_of_gpio_devs * sizeof(struct dev_desc_t *));
	config_handle->gpio_devs_arr = gpio_dev_arr;
	config_handle->num_of_gpio_devs = num_of_gpio_devs;

	curr_gpio_state_arr = (struct gpio_api_read_t *)malloc(
					num_of_gpio_devs * sizeof(struct gpio_api_read_t));
	if (NULL == curr_gpio_state_arr)
	{
		CRITICAL_ERROR("no memory");
	}
	runtime_handle->curr_gpio_state_arr = curr_gpio_state_arr;
}


static button_manager_action_handle_t
	add_new_action(struct button_manager_config_t *config_handle,
					struct button_manager_runtime_t *runtime_handle,
					struct btn_add_action_t *btn_add_action)
{
	struct btn_action_t   *btn_actions_arr;
	struct btn_action_t   *new_btn_action;
	uint32_t               num_of_actions;

	runtime_handle->state = STATE_ADDING_ACTIONS;
	btn_actions_arr = config_handle->btn_actions_arr;
	num_of_actions = config_handle->num_of_actions;

	if (0xffffffff == num_of_actions)
	{
		CRITICAL_ERROR("too many actions");
	}
	num_of_actions++;
	btn_actions_arr = (struct btn_action_t *)realloc(btn_actions_arr,
			 num_of_actions * sizeof(struct btn_action_t));
	if (NULL == btn_actions_arr)
	{
		CRITICAL_ERROR("no memory");
	}
	config_handle->num_of_actions = num_of_actions;
	new_btn_action = &btn_actions_arr[num_of_actions - 1];
	new_btn_action->report_config = btn_add_action->report_config;
	new_btn_action->callback_private_data =
							btn_add_action->callback_private_data;
	new_btn_action->req_gpio_values_arr = NULL;
	config_handle->btn_actions_arr = btn_actions_arr;

	return (button_manager_action_handle_t)new_btn_action;
}


static void fill_gpio_state(
		struct set_gpio_states_for_action_t *set_gpio_states,
		struct dev_desc_t *gpio_dev,
		uint8_t   *pin_bitwise_requested_values_arr)
{
	struct  gpio_state_for_action_t   *gpio_state_arr;
	struct  gpio_state_for_action_t   *gpio_state;
	uint8_t  pin_numbers_arr_size;
	uint16_t  i;
	uint16_t  j;
	int16_t  max_pin;

	gpio_state_arr = set_gpio_states->gpio_state_arr;
	max_pin = -1;
	for (i = 0; i < set_gpio_states->gpio_state_arr_size; i++)
	{
		gpio_state = &gpio_state_arr[i];
		if (gpio_dev == gpio_state->gpio_dev)
		{
			uint8_t  pin_num;

			pin_numbers_arr_size = gpio_state->pin_numbers_arr_size;
			if ( MAX_NUMBER_OF_ALLOWED_ACTIVE_PINS < pin_numbers_arr_size)
			{
				CRITICAL_ERROR("to many active gpio");
			}
			for (j = 0; j < pin_numbers_arr_size; j++)
			{
				uint8_t byte_num;
				uint8_t pin_offset_in_byte;

				pin_num = gpio_state->pin_numbers_arr[j];
				if (max_pin < pin_num)
				{
					max_pin = pin_num;
				}
				else
				{
					/* to prevent from user to put bigger pin_numbers_arr_size
					 * the pins should be in ascending order. then uninitialized
					 * pins will be assigned 0, and then this error will happen
					 */
					CRITICAL_ERROR("pin was already configured");
				}
				byte_num = pin_num / 8;
				pin_offset_in_byte = pin_num % 8;
				// change state of bit 0->1  or 1->0 by XOR
				pin_bitwise_requested_values_arr[byte_num] ^=
												(1 << pin_offset_in_byte);
			}
			for (j = pin_numbers_arr_size;
					j < MAX_NUMBER_OF_ALLOWED_ACTIVE_PINS; j++)
			{
				pin_num = gpio_state->pin_numbers_arr[j];
				if (0 != pin_num)
				{
					/* to prevent from user to put small pin_numbers_arr_size */
					CRITICAL_ERROR("pin_numbers_arr_size > defined pins");
				}
			}
		}
	}
}


static void set_gpio_states_for_action(
		struct button_manager_config_t *config_handle,
		struct button_manager_runtime_t *runtime_handle,
		struct set_gpio_states_for_action_t *set_gpio_states)
{
	struct dev_desc_t **gpio_dev_arr;
	uint8_t num_of_gpio_devs;
	uint16_t  i;
	struct btn_action_t   *btn_action;
	struct   button_manager_requested_gpio_values_t *req_gpio_values;
	struct   button_manager_requested_gpio_values_t *req_gpio_values_arr;
	struct   gpio_api_read_t gpio_read_data;
	uint8_t  values_arr_size;
	uint8_t   *pin_bitwise_requested_values_arr;

	if (STATE_ADDING_ACTIONS != runtime_handle->state)
	{
		CRITICAL_ERROR("can be called only when state = STATE_ADDING_ACTIONS");
	}

	btn_action = (struct btn_action_t *)set_gpio_states->action_handle;
	num_of_gpio_devs = config_handle->num_of_gpio_devs;
	req_gpio_values_arr =
			(struct   button_manager_requested_gpio_values_t *) malloc(
					num_of_gpio_devs *
					sizeof(struct button_manager_requested_gpio_values_t));
	if (NULL == req_gpio_values_arr)
	{
		CRITICAL_ERROR("no memory");
	}
	btn_action->req_gpio_values_arr = req_gpio_values_arr;
	gpio_dev_arr = config_handle->gpio_devs_arr;
	for (i = 0; i < num_of_gpio_devs; i++)
	{
		DEV_IOCTL_1_PARAMS(gpio_dev_arr[i],
							IOCTL_GPIO_PIN_READ, &gpio_read_data);
		values_arr_size = gpio_read_data.values_arr_size;
		pin_bitwise_requested_values_arr = (uint8_t *)malloc(values_arr_size);
		if (NULL == pin_bitwise_requested_values_arr)
		{
			CRITICAL_ERROR("no memory");
		}
		req_gpio_values = &req_gpio_values_arr[i];
		req_gpio_values->values_arr_size = values_arr_size;
		memcpy(pin_bitwise_requested_values_arr,
				gpio_read_data.pin_bitwise_idle_values, values_arr_size);
		fill_gpio_state(set_gpio_states, gpio_dev_arr[i],
								pin_bitwise_requested_values_arr);
		req_gpio_values->pin_bitwise_requested_values_arr =
									pin_bitwise_requested_values_arr;
	}
}


/*
 * Function:        button_manager_ioctl
 *
 * Parameters:
 *
 * Returns:gpio_dev
 * Side effects:
 * Description:
 */
uint8_t button_manager_ioctl(struct dev_desc_t *adev, const uint8_t aIoctl_num,
		void * aIoctl_param1, void * aIoctl_param2)
{
	struct button_manager_runtime_t *runtime_handle;
	struct button_manager_config_t *config_handle;


	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);

	switch(aIoctl_num)
	{
	case IOCTL_BUTTON_MANAGER_CREATE_BUTTON_GROUP:
		create_button_group(config_handle, runtime_handle, aIoctl_param1);
		break;

	case IOCTL_BUTTON_MANAGER_ADD_ACTION:
		*(button_manager_action_handle_t *)aIoctl_param1 =
				add_new_action(config_handle, runtime_handle, aIoctl_param2);
		break;

	case IOCTL_BUTTON_MANAGER_SET_GPIO_STATES_FOR_ACTION:
		set_gpio_states_for_action(
				config_handle, runtime_handle, aIoctl_param1);
		break;

	case IOCTL_DEVICE_START :
		os_create_task("button_manager_task", button_manager_task ,
				(void *)adev , BUTTON_MANAGER_CONFIG_TASK_STACK_SIZE ,
				BUTTON_MANAGER_CONFIG_TASK_PRIORITY);

		break;

		default :
			return 1;
	}
	return 0;
}

