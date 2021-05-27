/*
 *
 *   file  :  GPIO.cpp
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

#include "dev_management_api.h"

#include "gpio_api.h"
#include "sw_gpio_wrapper_api.h"
#include "sw_gpio_wrapper.h"


#define GPIO_HAL_MAX_NUM_OF_GPIOS 5
#define GPIO_HAL_MAX_NUM_OF_OUTPUTS_GPIOS 5

#define GPIO_TASK_PRIORITY				TASK_NORMAL_PRIORITY

#define GPIO_TASK_STACK_SIZE			DEFINE_STACK_SIZE( 160 )

#define INSTANCE(hndl)	((GPIO_Instance_t*)hndl)


struct GPIO_OutputPin_t {
	GPIO_Instance_t *pGPIO_Instance;
	uint8_t currCount;
};



static const struct dev_param_t GPIO_PIN_Dev_Params[]=
{
	{IOCTL_SET_SERVER_DEVICE_BY_NAME , IOCTL_VOID ,
		(uint8_t*)SW_GPIO_WRAPPER_API_SERVER_DEVICE_STR, NOT_FOR_SAVE},
	{IOCTL_SW_GPIO_WRAPPER_SET_BEHAVIOR_PARAM , IOCTL_VOID ,
		(uint8_t*)SW_GPIO_WRAPPER_API_BEHAVIOR_STR, NOT_FOR_SAVE},
	{IOCTL_SW_GPIO_WRAPPER_PIN_FORCE_SET, IOCTL_SW_GPIO_WRAPPER_PIN_FORCE_GET,
		(uint8_t*)SW_GPIO_WRAPPER_API_FORCE_STR, FOR_SAVE}
};


static GPIO_Instance_t GPIO_InstanceParams[GPIO_HAL_MAX_NUM_OF_GPIOS] = { {0} };
static uint16_t usedInstances =0 ;
static struct GPIO_OutputPin_t GPIO_OutputPins[
					GPIO_HAL_MAX_NUM_OF_OUTPUTS_GPIOS]= { {0} };
static uint16_t usedOutputPins =0 ;



/*
 * function : GPIO_API_InitPin()
 *
 *
 */
uint8_t GPIO_Start(GPIO_Instance_t *pInstance)
{
	if( 	GPIO_API_OUT_CONTROLED_EXTERNALLY != pInstance->outputBehavior )
	{
		GPIO_OutputPins[usedOutputPins++].pGPIO_Instance = pInstance;
	}
	
	return 0;
}



#define GPIO_OUT_STATE_SET			0
#define GPIO_OUT_STATE_CLEAR		1
#define GPIO_OUT_STATE_DONT_CHANGE	2


static uint8_t change_output_state( struct GPIO_OutputPin_t *pCurrOutputPin , uint8_t cycles)
{
	pCurrOutputPin->currCount = (pCurrOutputPin->currCount + 1) % cycles;
	if((cycles>>1) == pCurrOutputPin->currCount)
	{
		return GPIO_OUT_STATE_SET;
	}
	else if(0 == pCurrOutputPin->currCount)
	{
		return GPIO_OUT_STATE_CLEAR;
	}
	return GPIO_OUT_STATE_DONT_CHANGE;
}


static void GPIO_output_pins_Task( void *pvParameters )
{
	GPIO_Instance_t *pCurrGPIO_Instance;
	uint32_t i;
	uint8_t next_state;
	struct dev_desc_t *   server_dev  ;
	struct GPIO_OutputPin_t *pCurrOutputPin ;
	uint8_t cycles;

	for( ;; )
	{
		for(i=0 ; i<GPIO_HAL_MAX_NUM_OF_OUTPUTS_GPIOS ; i++)
		{
			pCurrOutputPin = &GPIO_OutputPins[i];
			pCurrGPIO_Instance= pCurrOutputPin->pGPIO_Instance;

			if(NULL == pCurrGPIO_Instance) continue ;

			switch(pCurrGPIO_Instance->outputBehavior)
			{
				case GPIO_API_OUT_BLINK_LOW_SPEED:
					cycles = 12 ;
					break;
				case GPIO_API_OUT_BLINK_MEDIUM_SPEED:
					cycles = 4 ;
					break;
				case GPIO_API_OUT_BLINK_HIGH_SPEED:
					cycles = 2 ;
					break;
				default :
					cycles = 0;
					break;
			}

			if( 0!= cycles)
			{
				next_state = change_output_state(pCurrOutputPin , cycles);

				server_dev = pCurrGPIO_Instance->server_dev ;

				if(GPIO_OUT_STATE_SET == next_state)
				{
					DEV_IOCTL_0_PARAMS(server_dev,IOCTL_GPIO_PIN_SET);
				}
				else if(GPIO_OUT_STATE_CLEAR ==next_state)
				{
					DEV_IOCTL_0_PARAMS(server_dev,IOCTL_GPIO_PIN_CLEAR);
				}
			}


		}
		vTaskDelay( 500 );

		os_stack_test();

	}
}



uint8_t GPIO_ioctl( struct dev_desc_t *adev,
			const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	uint32_t retVal;
	struct dev_desc_t *   server_dev = INSTANCE(aHandle)->server_dev ;
	GPIO_Force_t forceState = INSTANCE(aHandle)->forceState ;
	enum params_status_e paramStatus = INSTANCE(aHandle)->paramStatus;

	retVal = 0 ;
	switch(aIoctl_num)
	{
		case IOCTL_GET_PARAMS_ARRAY_FUNC :
			*(const dev_param_t**)aIoctl_param1  = GPIO_PIN_Dev_Params;
			*(uint8_t*)aIoctl_param2 =
				sizeof(GPIO_PIN_Dev_Params)/sizeof(dev_param_t); //size
			break;
		case IOCTL_SET_SERVER_DEVICE_BY_NAME :
			INSTANCE(aHandle)->server_dev = DEV_OPEN((uint8_t*)aIoctl_param1);
			break;

		case IOCTL_SW_GPIO_WRAPPER_SET_BEHAVIOR_PARAM :
			if (0 == memcmp((uint8_t*) aIoctl_param1,
						"blink_medium" , sizeof("blink_medium") ))
			{
				INSTANCE(aHandle)->outputBehavior =
							GPIO_API_OUT_BLINK_MEDIUM_SPEED;
			}
			else if (0 == memcmp((uint8_t*) aIoctl_param1,
							"controlled" , sizeof("controlled") ))
			{
				INSTANCE(aHandle)->outputBehavior =
							GPIO_API_OUT_CONTROLED_EXTERNALLY;
			}
			else
			{
				return 1;
			}
			break;

		case IOCTL_GPIO_PIN_SET :
			if(GPIO_Force_None == forceState)
			{
				DEV_IOCTL_0_PARAMS(server_dev,IOCTL_GPIO_PIN_SET);
			}
			break;

		case IOCTL_GPIO_PIN_CLEAR :
			if(GPIO_Force_None == forceState)
			{
				DEV_IOCTL_0_PARAMS(server_dev,IOCTL_GPIO_PIN_CLEAR);
			}
			break;

		case IOCTL_SW_GPIO_WRAPPER_PIN_FORCE_SET :
			if (0 == memcmp((uint8_t*) aIoctl_param1,
					GPIO_API_FORCE_NONE_STR , sizeof(GPIO_API_FORCE_NONE_STR) ))
			{
				INSTANCE(aHandle)->forceState = GPIO_Force_None;
			}
			else if (0 == memcmp((uint8_t*) aIoctl_param1,
						GPIO_API_FORCE_SET_STR , sizeof(GPIO_API_FORCE_SET_STR) ))
			{
				INSTANCE(aHandle)->forceState = GPIO_Force_Set;
				DEV_IOCTL_0_PARAMS(server_dev,IOCTL_GPIO_PIN_SET);
			}
			else if (0 == memcmp((uint8_t*) aIoctl_param1,
					GPIO_API_FORCE_CLEAR_STR , sizeof(GPIO_API_FORCE_CLEAR_STR) ))
			{
				INSTANCE(aHandle)->forceState = GPIO_Force_Clear;
				DEV_IOCTL_0_PARAMS(server_dev,IOCTL_GPIO_PIN_CLEAR);
			}
			INSTANCE(aHandle)->paramStatus = PARAMS_STATUS_CHANGED_AFTER_LAST_TEST;
			break;

		case IOCTL_SW_GPIO_WRAPPER_PIN_FORCE_GET :
			switch(forceState)
			{
				case GPIO_Force_Clear:
					memcpy((uint8_t*) aIoctl_param1,
						GPIO_API_FORCE_CLEAR_STR , sizeof(GPIO_API_FORCE_CLEAR_STR));
					break;
				case GPIO_Force_Set:
					memcpy((uint8_t*) aIoctl_param1,
						GPIO_API_FORCE_SET_STR , sizeof(GPIO_API_FORCE_SET_STR));
					break;
				case GPIO_Force_None:
				default:
					memcpy((uint8_t*) aIoctl_param1,
						GPIO_API_FORCE_NONE_STR , sizeof(GPIO_API_FORCE_NONE_STR));
					break;
			}
			break;

		case IOCTL_TEST_PARAMS_STATUS :
			*((enum params_status_e*)aIoctl_param1 )= paramStatus;
			if(PARAMS_STATUS_CHANGED_AFTER_LAST_TEST == paramStatus)
			{
				INSTANCE(aHandle)->paramStatus = PARAMS_STATUS_NO_CHANGED_AFTER_LAST_TEST;
			}
			else if(PARAMS_STATUS_NO_CHANGED_AFTER_LAST_TEST == paramStatus)
			{
				INSTANCE(aHandle)->paramStatus = PARAMS_STATUS_IDLE;
			}

			break;

		case IOCTL_GPIO_PIN_READ :
			DEV_IOCTL(server_dev,IOCTL_GPIO_PIN_READ,aIoctl_param1);
			break;

		case IOCTL_DEVICE_START :
			GPIO_Start(INSTANCE(aHandle));
			break;

		default :
			return 1;
	}
	return retVal;
}


uint8_t  sw_gpio_wrapper_api_init_dev_descriptor(struct dev_desc_t *aDevDescriptor)
{
	GPIO_Instance_t *pInstance;
	if (usedInstances >= GPIO_HAL_MAX_NUM_OF_GPIOS) return 1;

	pInstance = &GPIO_InstanceParams[usedInstances++];

	aDevDescriptor->handle=pInstance;

	aDevDescriptor->ioctl = GPIO_ioctl;


	return 0;
}

/*
 * function : GPIO_API_Init()
 *
 *
 */
uint8_t sw_gpio_wrapper_api_init(void)
{
	xTaskCreate( GPIO_output_pins_Task, "GPIO_output_pins", GPIO_TASK_STACK_SIZE, NULL, GPIO_TASK_PRIORITY +1 , NULL );

	return 0;
}
