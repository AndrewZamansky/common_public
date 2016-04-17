/*
 *
 * file :   timer.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/
#include "src/_timer_prerequirements_check.h"

#include "timer.h"

#include "timer_api.h"
#include "hw_timer_api.h"


/********  defines *********************/

#define INSTANCE(hndl)	((timer_instance_t*)hndl)


/********  types  *********************/


/********  externals *********************/



/********  local defs *********************/



#if TIMER_CONFIG_USE_AS_DYNAMIC_INSTANCE > 0
	timer_instance_t timer_instance;
#endif




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        HTTP_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t timer_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	uint64_t curr_timer_val;
	switch(aIoctl_num)
	{
		case IOCTL_GET_PARAMS_ARRAY_FUNC :
			*(uint8_t*)aIoctl_param2 =  0; //size
			break;

		case IOCTL_DEVICE_START :

			break;

#if TIMER_CONFIG_USE_AS_DYNAMIC_INSTANCE > 0
		case TIMER_API_SET_CALLBACK_FUNC_IOCTL :
			INSTANCE(aHandle)->hw_timer =  (pdev_descriptor_const)aIoctl_param1;
			break;
#endif

		case TIMER_API_SET_COUNTDOWN_VALUE_AND_REST :
			DEV_IOCTL(INSTANCE(aHandle)->hw_timer,
					IOCTL_GET_CURRENT_TIMER_VALUE, (void*)&curr_timer_val);
			INSTANCE(aHandle)->timer_value_on_start = curr_timer_val;
			INSTANCE(aHandle)->countdown_value = curr_timer_val + *(uint64_t*)aIoctl_param1 ;
			break;

		case TIMER_API_CHECK_IF_COUNTDOWN_ELAPSED :
			DEV_IOCTL(INSTANCE(aHandle)->hw_timer,
					IOCTL_GET_CURRENT_TIMER_VALUE, (void*)&curr_timer_val);
			if(curr_timer_val > INSTANCE(aHandle)->countdown_value)
			{
				*(uint8_t*)aIoctl_param1 = 1;
			}
			else
			{
				*(uint8_t*)aIoctl_param1 = 0;
			}

			break;

		default :
			return 1;
	}
	return 0;
}

#if TIMER_CONFIG_USE_AS_DYNAMIC_INSTANCE > 0

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        timer_API_Init_Dev_Descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  timer_api_init_dev_descriptor(pdev_descriptor aDevDescriptor)
{

	if(NULL == aDevDescriptor) return 1;


	aDevDescriptor->ioctl = timer_ioctl;

	return 0;
}

#endif

