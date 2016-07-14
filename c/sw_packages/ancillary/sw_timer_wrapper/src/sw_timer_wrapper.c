/*
 *
 * file :   sw_timer_wrapper.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "sw_timer_wrapper_config.h"
#include "dev_management_api.h" // for device manager defines and typedefs
#include "timer_api.h"
#include "_sw_timer_wrapper_prerequirements_check.h" // should be after {sw_timer_wrapper_config.h,dev_management_api.h}

#include "sw_timer_wrapper_api.h" //place first to test that header file is self-contained
#include "sw_timer_wrapper.h"


/********  defines *********************/


/********  types  *********************/



/********  externals *********************/


/********  local defs *********************/


#define INSTANCE(hndl)	((SW_TIMER_WRAPPER_Instance_t*)hndl)


/**********   external variables    **************/



/***********   local variables    **************/

#if CONFIG_SW_TIMER_WRAPPER_MAX_NUM_OF_DYNAMIC_INSTANCES > 0

	static SW_UART_WRAPPER_Instance_t SW_UART_WRAPPER_InstanceParams[CONFIG_SW_TIMER_WRAPPER_MAX_NUM_OF_DYNAMIC_INSTANCES] = { {0} };
	static uint16_t usedInstances =0 ;

	static const dev_param_t SW_UART_WRAPPER_Dev_Params[]=
	{
			{IOCTL_SET_SERVER_DEVICE_BY_NAME , IOCTL_VOID , (uint8_t*)SW_UART_WRAPPER_API_SERVER_DEVICE_STR, NOT_FOR_SAVE},
	};

#endif // for SW_UART_WRAPPER_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        sw_timer_wrapper_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t sw_timer_wrapper_ioctl(void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{

	switch(aIoctl_num)
	{
#if SW_TIMER_WRAPPER_CONFIG_NUM_OF_DYNAMIC_INSTANCES > 0
		case IOCTL_GET_PARAMS_ARRAY_FUNC :
			*(const dev_param_t**)aIoctl_param1  = SW_TIMER_WRAPPER_Dev_Params;
			*(uint8_t*)aIoctl_param2 = sizeof(SW_TIMER_WRAPPER_Dev_Params)/sizeof(dev_param_t); //size
			break;
		case IOCTL_SET_SERVER_DEVICE_BY_NAME :
			INSTANCE(aHandle)->server_dev = DEV_OPEN((uint8_t*)aIoctl_param1);
			if (NULL!=INSTANCE(aHandle)->server_dev)
			{
				DEV_IOCTL(INSTANCE(aHandle)->server_dev,IOCTL_SET_ISR_CALLBACK_DEV, (void*)INSTANCE(aHandle)->this_dev);
			}
			break;
#endif // for SW_TIMER_WRAPPER_CONFIG_NUM_OF_DYNAMIC_INSTANCES > 0


		case IOCTL_SW_TIMER_WRAPPER_START_TIMER :
			/* fill here  */
			break;

		case IOCTL_SW_TIMER_WRAPPER_STOP_TIMER :
			/* fill here  */
			break;

		case IOCTL_SW_TIMER_WRAPPER_SET_TIMEOUT :
			/* fill here  */
			break;

		case IOCTL_SW_TIMER_WRAPPER_TEST_IF_TIMEOUT :
			/* fill here  */
			break;

		default :
			return 1;
	}
	return 0;
}




#if CONFIG_SW_TIMER_WRAPPER_MAX_NUM_OF_DYNAMIC_INSTANCES>0

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SW_TIMER_WRAPPER_API_Init_Dev_Descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  sw_timer_wrapper_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor)
{
	SW_TIMER_WRAPPER_Instance_t *pInstance;
	if(NULL == aDevDescriptor) return 1;
	if (usedInstances >= CONFIG_SW_TIMER_WRAPPER_MAX_NUM_OF_DYNAMIC_INSTANCES) return 1;

	pInstance = &SW_TIMER_WRAPPER_InstanceParams[usedInstances ];
	pInstance->this_dev = aDevDescriptor;


	aDevDescriptor->handle = pInstance;
	aDevDescriptor->ioctl = sw_timer_wrapper_ioctl;

	usedInstances++;

	return 0 ;

}
#endif  // for SW_TIMER_WRAPPER_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0


