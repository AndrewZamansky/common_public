/*
 *
 * file :   button_manager.c
 *
 *
 *        Last Edited 8/13/18 by Nathan Wong
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


/********  defines *********************/


/********  types  *********************/


/********  externals *********************/


/********  local defs *********************/

#define 	NO_VALID_STATE_FOUND	0xff
#define 	DEBOUNCE_DELAY	10


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


/*----------------------------------------------------------------------------*/
/* Function:        button_manager_callback                                   */
/*                                                                            */
/* Parameters:                                                                */
/*                                                                            */
/*                                                                            */
/* Returns:                                                                   */
/* Side effects:                                                              */
/* Description:                                                               */
/*----------------------------------------------------------------------------*/
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

	os_queue_send_immediate( xButtonQueue, ( void * ) &xTxMessage);

	return 0;
}


uint8_t debounce_on_push(struct dev_desc_t *adev)
{
	struct button_manager_runtime_t *runtime_handle;
	struct button_manager_config_t  *config_handle;

	struct dev_desc_t *   *server_dev;
	struct dev_desc_t     *client_dev;

	uint8_t i = 0;
	uint8_t action_idx = 0;
	uint8_t num_buttons;
	uint8_t num_actions;
	uint8_t *curr_button_state;
	uint8_t manager_state;
	uint32_t   hold_count;
	uint32_t   queue_wait_delay;
	struct btn_action_states_t	*pFound_action_state;
	struct btn_action_states_t *btn_actions;

	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);
	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);

	curr_button_state = runtime_handle->curr_button_state;
	hold_count = runtime_handle->hold_count;
	queue_wait_delay = runtime_handle->queue_wait_delay;

	server_dev = config_handle->server_dev;
	client_dev = config_handle->client_dev;
	num_buttons = config_handle->num_buttons;
	num_actions = config_handle->num_actions;
	btn_actions = config_handle->btn_actions;

	for(i = 0; i < num_buttons ; i++)
	{
		DEV_IOCTL_1_PARAMS(
				server_dev[i], IOCTL_GPIO_PIN_READ, &curr_button_state[i]);
	}

	pFound_action_state = NULL;
	for(action_idx = 0; action_idx < num_actions ; action_idx++)
	{
		if(0 == memcmp(btn_actions[action_idx].action_state,
				curr_button_state,num_buttons))
		{
			pFound_action_state = &btn_actions[action_idx];
			if(BTN_REPORT_PUSH & btn_actions[action_idx].report_config)
			{
				DEV_CALLBACK_2_PARAMS(client_dev, CALLBACK_BTN_STATE_CHANGED,
					(void *)BTN_REPORT_PUSH, &btn_actions[action_idx]);
			}
			break;
		}
	}

	if(NULL == pFound_action_state)
	{
		queue_wait_delay = DEBOUNCE_DELAY;
		manager_state = BTN_STATE_DEBOUNCING_ON_RELEASE;
	}
	else
	{
		queue_wait_delay = 128;
		hold_count = 0;
		manager_state = BTN_STATE_HOLD;
	}


	runtime_handle->manager_state = manager_state;
	runtime_handle->hold_count = hold_count;
	runtime_handle->queue_wait_delay = queue_wait_delay;

	return action_idx;
}


uint8_t button_state_hold(struct dev_desc_t *adev, uint8_t action_idx)
{
	struct button_manager_runtime_t *runtime_handle;
	struct button_manager_config_t  *config_handle;
	struct dev_desc_t     *client_dev;

	struct btn_action_states_t *btn_actions;

	uint32_t   hold_count;

	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);
	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);

	hold_count = runtime_handle->hold_count;

	client_dev = config_handle->client_dev;
	btn_actions = config_handle->btn_actions;

	hold_count+=128;
	if(BTN_REPORT_HOLD_EVERY_0s1 & btn_actions[action_idx].report_config)
	{
		DEV_CALLBACK_2_PARAMS( client_dev ,CALLBACK_BTN_STATE_CHANGED ,
				(void *)BTN_REPORT_HOLD_EVERY_0s1 ,
				&btn_actions[action_idx]) ;
	}
	if( 0x0 == (hold_count & 0x3ff) ) // enter each 1024
	{
		if(BTN_REPORT_HOLD_EVERY_1s & btn_actions[action_idx].report_config)
		{
			DEV_CALLBACK_2_PARAMS( client_dev ,CALLBACK_BTN_STATE_CHANGED ,
					(void *)BTN_REPORT_HOLD_EVERY_1s ,
					&btn_actions[action_idx]) ;
		}
	}
	if( 4992 == hold_count  ) // enter once ~5s
	{
		if(BTN_REPORT_HOLD_ONCE_5s & btn_actions[action_idx].report_config)
		{
			DEV_CALLBACK_2_PARAMS( client_dev ,CALLBACK_BTN_STATE_CHANGED ,
					(void *)BTN_REPORT_HOLD_ONCE_5s ,
					&btn_actions[action_idx]) ;
		}
	}


	runtime_handle->hold_count = hold_count;

	return 0;
}


uint8_t debounce_on_release(struct dev_desc_t *adev)
{
	struct button_manager_runtime_t *runtime_handle;
	struct button_manager_config_t  *config_handle;

	struct dev_desc_t *   *server_dev;

	uint8_t i = 0;
	uint8_t num_buttons;
	uint8_t manager_state;
	uint32_t   queue_wait_delay;
	uint8_t *curr_button_state;
	uint8_t *idle_state;

	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);
	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);

	curr_button_state = runtime_handle->curr_button_state;

	manager_state = runtime_handle->manager_state;
	queue_wait_delay = runtime_handle->queue_wait_delay;

	server_dev = config_handle->server_dev;
	num_buttons = config_handle->num_buttons;
	idle_state = config_handle->idle_state;

	for(i = 0; i < num_buttons ; i++)
	{
		DEV_IOCTL_1_PARAMS(
				server_dev[i],IOCTL_GPIO_PIN_READ, &curr_button_state[i]);
	}
	if(0 == memcmp(idle_state ,
			curr_button_state,num_buttons))
	{
		queue_wait_delay=100000;
		manager_state = BTN_STATE_IDLE;
	}

	runtime_handle->manager_state = manager_state;
	runtime_handle->queue_wait_delay = queue_wait_delay;

	return 0;
}


/*----------------------------------------------------------------------------*/
/* Function:        button_manager_task                                       */
/*                                                                            */
/* Parameters:                                                                */
/*                                                                            */
/*                                                                            */
/* Returns:                                                                   */
/* Side effects:                                                              */
/* Description:                                                               */
/*----------------------------------------------------------------------------*/
static void button_manager_task( void *adev )
{
	struct button_manager_runtime_t *runtime_handle;

	uint8_t action_idx = 0;

	os_queue_t xButtonQueue;
	xMessage_t xTxMessage;
	uint8_t    queueRetVal;

	uint8_t    manager_state = BTN_STATE_IDLE;
	uint32_t   hold_count = 0;
	uint32_t   queue_wait_delay = 100000;

	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);

	xButtonQueue =  os_create_queue( 1 , sizeof(xMessage_t ) );

	runtime_handle->queue_wait_delay = queue_wait_delay;
	runtime_handle->hold_count = hold_count;
	runtime_handle->xButtonQueue = xButtonQueue;


	if( 0 == xButtonQueue  )
	{
		CRITICAL_ERROR("OS queue failed to create");
	}

	for( ;; )
	{
		manager_state = runtime_handle->manager_state;
		hold_count = runtime_handle->hold_count;
		queue_wait_delay = runtime_handle->queue_wait_delay;

		queueRetVal = os_queue_receive_with_timeout(
				xButtonQueue , &( xTxMessage ),queue_wait_delay) ;

		switch(manager_state)
		{
			default :
			case BTN_STATE_IDLE:
				if( OS_QUEUE_RECEIVE_SUCCESS == queueRetVal)
				{
					queue_wait_delay = DEBOUNCE_DELAY;
					manager_state = BTN_STATE_DEBOUNCING_ON_PUSH;

					runtime_handle->manager_state = manager_state;
					runtime_handle->queue_wait_delay = queue_wait_delay;
				}
				break;
			case BTN_STATE_DEBOUNCING_ON_PUSH:
				if( OS_QUEUE_RECEIVE_SUCCESS != queueRetVal)
				{
					action_idx = debounce_on_push(adev);
				}
				break;
			case BTN_STATE_HOLD:

				if( OS_QUEUE_RECEIVE_SUCCESS != queueRetVal)
				{
					button_state_hold(adev, action_idx);
				}
				else
				{
					queue_wait_delay = DEBOUNCE_DELAY;
					manager_state = BTN_STATE_DEBOUNCING_ON_RELEASE;

					runtime_handle->manager_state = manager_state;
					runtime_handle->queue_wait_delay = queue_wait_delay;
				}
				break;
			case BTN_STATE_DEBOUNCING_ON_RELEASE:
				if( OS_QUEUE_RECEIVE_SUCCESS != queueRetVal)
				{
					debounce_on_release(adev);
				}
				break;
		}
		os_stack_test();
	}
}


uint8_t button_manager_config_group(
		struct button_manager_runtime_t *runtime_handle,
		struct button_manager_config_t *config_handle,
		struct buttons_manager_group_t *button_group)
{
	uint8_t i;
	uint8_t num_buttons;
	uint16_t num_actions;

	if(NULL == button_group)
	{
		CRITICAL_ERROR("Need button struct, refer to button_manager_api.h");
	}

	num_buttons = button_group->num_buttons;
	num_actions = button_group->num_actions;

	config_handle->num_buttons = num_buttons;
	config_handle->num_actions = num_actions;

	//Allocate memory for storing button configurations.
	config_handle->server_dev = (struct dev_desc_t **)malloc(
						num_buttons * sizeof(struct dev_desc_t));

	config_handle->idle_state =
				(uint8_t *)malloc(num_buttons * sizeof(uint8_t));

	runtime_handle->curr_button_state =
				(uint8_t *)malloc(num_buttons * sizeof(uint8_t));

	config_handle->btn_actions = (struct btn_action_states_t *)malloc(
						num_actions * sizeof(struct btn_action_states_t));


	for(i = 0; i < num_buttons ; i++)
	{
		//Assign pointers to the runtime handle
		config_handle->server_dev[i] = button_group->server_dev[i];

		// Servers need this IOCTL case:
		/*
		 * IOCTL_ADD_ISR_CALLBACK_DEV
		 *      IOCTL function to add this instance of button manager
		 *      to its ISR so the server can notify the button manager
		 *      when the specific button is pressed.
		 *
		 *  *Updated 8/13/18. Currently use device tree to add
		 *      directly through IOCTL_DEVICE_START.
		 *
		 */
//		DEV_IOCTL_1_PARAMS(server_dev, IOCTL_ADD_ISR_CALLBACK_DEV, (void *)adev);
	}

	// Copy the idle states of buttons into the runtime handle
	memcpy(config_handle->idle_state, button_group->idle_state, num_buttons);

	// Copy the button action array into the runtime handle
	//    without padding, The size is based on report_config and
	//    action_state. If there is a callback_private_data then
	//    this needs to be changed.
	memcpy(config_handle->btn_actions, button_group->btn_actions,
			(num_actions * sizeof(struct btn_action_states_t)));

	return 0;
}


/*---------------------------------------------------------------------------*/
/* Function:        button_manager_ioctl                                     */
/*                                                                           */
/* Parameters:                                                               */
/*                                                                           */
/*                                                                           */
/* Returns:                                                                  */
/* Side effects:                                                             */
/* Description:                                                              */
/*---------------------------------------------------------------------------*/
uint8_t button_manager_ioctl(struct dev_desc_t *adev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	struct button_manager_runtime_t *runtime_handle;
	struct button_manager_config_t *config_handle;


	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);

	switch(aIoctl_num)
	{
		case IOCTL_BUTTON_MANAGER_CONFIG_GROUP :
			button_manager_config_group(runtime_handle, config_handle,
						(struct buttons_manager_group_t *)aIoctl_param1);

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

