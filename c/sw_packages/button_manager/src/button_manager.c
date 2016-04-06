/*
 *
 * file :   button_manager.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "button_manager_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "_button_manager_prerequirements_check.h" // should be after {button_manager_config.h,dev_managment_api.h}

#include "button_manager_api.h" //place first to test that header file is self-contained
#include "button_manager.h"

#include "gpio_api.h"

/********  defines *********************/


/********  types  *********************/


/********  externals *********************/


/********  local defs *********************/


#define INSTANCE(hndl)	((buttons_group_t*)hndl)


/**********   external variables    **************/



/***********   local variables    **************/
static uint16_t usedBtnGroups =0 ;
#if BUTTON_MANAGER_CONFIG_NUM_OF_DYNAMIC_BUTTONS_GROUPS>0

	static buttons_group_t BtnGroupParams[BUTTON_MANAGER_CONFIG_NUM_OF_DYNAMIC_BUTTONS_GROUPS] = { {0} };

#if 0
	static const dev_param_t Button_Manager_Dev_Params[]=
	{
	};
#endif

#endif // for BUTTON_MANAGER_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0

typedef struct
{
	uint8_t dummy;
} xMessage_t;


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        button_manager_callback                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t button_manager_callback(void * const aHandle ,const uint8_t aCallback_num ,
		void * aCallback_param1, void * aCallback_param2)
{
	os_queue_t xQueue ;
	xMessage_t xTxMessage;

	xQueue = INSTANCE(aHandle)->xQueue;

	if (NULL == xQueue)
	{
		return 1;
	}



	os_queue_send_immediate( xQueue, ( void * ) &xTxMessage);

	return 0;
}


typedef enum
{
	BTN_STATE_IDLE,
	BTN_STATE_DEBOUNCING_ON_PUSH,
	BTN_STATE_HOLD,
	BTN_STATE_DEBOUNCING_ON_RELEASE
}btn_state_t;

#define 	NO_VALID_STATE_FOUND	0xff
#define 	DEBOUNCE_DELAY	10

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        button_manager_task                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static void button_manager_task( void *aHandle )
{

	xMessage_t xRxMessage;
	uint8_t i;
	uint8_t		*curr_state;
	uint8_t curr_num_of_btn_in_group;
	uint32_t hold_count,queue_wait_delay=100000;
	uint8_t currState = BTN_STATE_IDLE;
	uint8_t queueRetVal;
	pdev_descriptor   *server_dev;
	pdev_descriptor   this_dev;
	uint8_t		*idle_state;
	btn_action_t	*pFound_action_state;

	curr_state = INSTANCE(aHandle)->curr_state;
	server_dev = INSTANCE(aHandle)->server_dev;
	idle_state = INSTANCE(aHandle)->idle_state;
	this_dev = INSTANCE(aHandle)->this_dev;

	os_queue_t xQueue ;

	xQueue =  os_create_queue( 1 , sizeof(xMessage_t ) );

	INSTANCE(aHandle)->xQueue = xQueue ;


	if( 0 == xQueue  ) return ;

	curr_num_of_btn_in_group = INSTANCE(aHandle)->num_of_buttons;

	for( ;; )
	{
		queueRetVal = os_queue_receive_with_timeout( xQueue , &( xRxMessage ),queue_wait_delay) ;

		switch(currState)
		{
			default :
			case BTN_STATE_IDLE:
				if( OS_QUEUE_RECEIVE_SUCCESS == queueRetVal)
				{
					queue_wait_delay = DEBOUNCE_DELAY;
					currState = BTN_STATE_DEBOUNCING_ON_PUSH;
				}
				break;
			case BTN_STATE_DEBOUNCING_ON_PUSH:
				if( OS_QUEUE_RECEIVE_SUCCESS != queueRetVal)
				{

					for(i = 0; i < curr_num_of_btn_in_group ; i++)
					{
						DEV_IOCTL_1_PARAMS(server_dev[i],IOCTL_GPIO_PIN_READ, &curr_state[i]);
					}

					pFound_action_state=NULL;
					for(i = 0; i < INSTANCE(aHandle)->num_of_actons ; i++)
					{
						if(0 == memcmp(INSTANCE(aHandle)->btn_actions[i].action_state,
								curr_state,curr_num_of_btn_in_group))
						{
							pFound_action_state = &(INSTANCE(aHandle)->btn_actions[i]);
							if(BTN_REPORT_PUSH & INSTANCE(aHandle)->btn_actions[i].report_config)
							{
								DEV_CALLBACK_2_PARAMS( this_dev , CALLBACK_BTN_STATE_CHANGED ,
										(void *)BTN_REPORT_PUSH ,
										&(INSTANCE(aHandle)->btn_actions[i])) ;
							}
							break;
						}
					}

					if(NULL == pFound_action_state)
					{
						queue_wait_delay = DEBOUNCE_DELAY;
						currState = BTN_STATE_DEBOUNCING_ON_RELEASE;

					}
					else
					{
						queue_wait_delay = 128;
						hold_count=0;
						currState = BTN_STATE_HOLD;
					}
				}
				break;
			case BTN_STATE_HOLD:

				if( OS_QUEUE_RECEIVE_SUCCESS != queueRetVal)
				{
					hold_count+=128;
					if(BTN_REPORT_HOLD_EVERY_0s1 & INSTANCE(aHandle)->btn_actions[i].report_config)
					{
						DEV_CALLBACK_2_PARAMS( this_dev ,CALLBACK_BTN_STATE_CHANGED ,
								(void *)BTN_REPORT_HOLD_EVERY_0s1 ,
								&(INSTANCE(aHandle)->btn_actions[i])) ;
					}
					if( 0x0 == (hold_count & 0x3ff) ) // enter each 1024
					{
						if(BTN_REPORT_HOLD_EVERY_1s & INSTANCE(aHandle)->btn_actions[i].report_config)
						{
							DEV_CALLBACK_2_PARAMS( this_dev ,CALLBACK_BTN_STATE_CHANGED ,
									(void *)BTN_REPORT_HOLD_EVERY_1s ,
									&(INSTANCE(aHandle)->btn_actions[i])) ;
						}
					}
					if( 4992 == hold_count  ) // enter once ~5s
					{
						if(BTN_REPORT_HOLD_ONCE_5s & INSTANCE(aHandle)->btn_actions[i].report_config)
						{
							DEV_CALLBACK_2_PARAMS( this_dev ,CALLBACK_BTN_STATE_CHANGED ,
									(void *)BTN_REPORT_HOLD_ONCE_5s ,
									&(INSTANCE(aHandle)->btn_actions[i])) ;
						}
					}
				}
				else
				{
					queue_wait_delay = DEBOUNCE_DELAY;
					currState = BTN_STATE_DEBOUNCING_ON_RELEASE;

				}
				break;
			case BTN_STATE_DEBOUNCING_ON_RELEASE:
				if( OS_QUEUE_RECEIVE_SUCCESS != queueRetVal)
				{
					for(i = 0; i < curr_num_of_btn_in_group ; i++)
					{
						DEV_IOCTL_1_PARAMS(server_dev[i],IOCTL_GPIO_PIN_READ, &curr_state[i]);
					}
					if(0 == memcmp(idle_state ,
							curr_state,curr_num_of_btn_in_group))
					{
						queue_wait_delay=100000;
						currState = BTN_STATE_IDLE;
					}

				}
				break;
		}




#if ((1==INCLUDE_uxTaskGetStackHighWaterMark ) && (1==CONFIG_FREE_RTOS))
		{
			static  uint32_t stackLeft,minStackLeft=0xffffffff;

			stackLeft = uxTaskGetStackHighWaterMark( NULL );
			if(minStackLeft > stackLeft)
			{
				minStackLeft = stackLeft;
				PRINTF_DBG("%s   stack left = %d  \r\n" ,
						__FUNCTION__   ,minStackLeft);
			}
		}
#endif
	}

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        button_manager_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t button_manager_ioctl(void * const aHandle ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	uint8_t i,num;
	pdev_descriptor   server_dev;

	switch(aIoctl_num)
	{
#if BUTTON_MANAGER_CONFIG_NUM_OF_DYNAMIC_BUTTONS_GROUPS > 0
		case IOCTL_GET_PARAMS_ARRAY_FUNC :
//			*(const dev_param_t**)aIoctl_param1  = BUTTON_MANAGER_Dev_Params;
//			*(uint8_t*)aIoctl_param2 =  sizeof(BUTTON_MANAGER_Dev_Params)/sizeof(dev_param_t); //size
			*(uint8_t*)aIoctl_param2 =  0; //size
			break;
		case IOCTL_BUTTON_MANAGER_SET_GROUP_SERVER_DEVS :
			num = ((buttons_manager_set_group_servers_devs_t *)aIoctl_param1)->num_of_buttons;
			INSTANCE(aHandle)->num_of_buttons = num ;
			for(i=0; i<num ;i++)
			{
				server_dev = ((buttons_manager_set_group_servers_devs_t *)aIoctl_param1)->server_dev[i];
				INSTANCE(aHandle)->server_dev[i] = server_dev;
				DEV_IOCTL(server_dev,IOCTL_ADD_ISR_CALLBACK_DEV,INSTANCE(aHandle)->this_dev);
			}
			break;
		case IOCTL_BUTTON_MANAGER_SET_GROUP_IDLE_STATE :
			memcpy(INSTANCE(aHandle)->idle_state,(uint8_t *)aIoctl_param1, INSTANCE(aHandle)->num_of_buttons);
			break;
		case IOCTL_BUTTON_MANAGER_SET_GROUP_ACTIONS :
			num = ((buttons_manager_set_group_actions_t *)aIoctl_param1)->num_of_actons;
			INSTANCE(aHandle)->num_of_actons = num ;
			memcpy(INSTANCE(aHandle)->btn_actions,(buttons_manager_set_group_actions_t *)aIoctl_param1,
					num*(INSTANCE(aHandle)->num_of_buttons + sizeof(void*)+1));
			break;
#endif // for BUTTON_MANAGER_CONFIG_NUM_OF_DYNAMIC_INSTANCES > 0

		case IOCTL_DEVICE_START :
			os_create_task("button_manager_task", button_manager_task ,
					aHandle , BUTTON_MANAGER_CONFIG_TASK_STACK_SIZE , BUTTON_MANAGER_CONFIG_TASK_PRIORITY);

			break;

		default :
			return 1;
	}
	return 0;
}





#if BUTTON_MANAGER_CONFIG_NUM_OF_DYNAMIC_BUTTONS_GROUPS>0

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BUTTON_MANAGER_API_Init_Dev_Descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  button_manager_api_init_dev_descriptor(pdev_descriptor aDevDescriptor)
{
	buttons_group_t *pInstance;
	if(NULL == aDevDescriptor) return 1;
	if ( usedBtnGroups >= BUTTON_MANAGER_CONFIG_NUM_OF_DYNAMIC_BUTTONS_GROUPS ) return 1;

	pInstance = &BtnGroupParams[usedBtnGroups ];
	pInstance->this_dev = aDevDescriptor;


	aDevDescriptor->handle = pInstance;
	aDevDescriptor->ioctl = button_manager_ioctl;
	aDevDescriptor->callback = button_manager_callback;
	usedBtnGroups++;

	return 0 ;

}
#endif  // for BUTTON_MANAGER_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0


