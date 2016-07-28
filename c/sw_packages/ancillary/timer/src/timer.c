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



/********  types  *********************/


/********  externals *********************/



/********  local variables *********************/


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        timer_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t timer_ioctl( pdev_descriptor_t apdev ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	timer_instance_t *handle;
	uint64_t curr_timer_val;

	handle = apdev->handle;

	switch(aIoctl_num)
	{
		case IOCTL_SET_CALLBACK_DEV :
			handle->hw_timer =  (pdev_descriptor_t)aIoctl_param1;
			break;

		case TIMER_API_SET_COUNTDOWN_VALUE_AND_REST :
			DEV_IOCTL(handle->hw_timer,
					IOCTL_GET_CURRENT_TIMER_VALUE, (void*)&curr_timer_val);
			handle->timer_value_on_start = curr_timer_val;
			handle->countdown_value = curr_timer_val + *(uint64_t*)aIoctl_param1 ;
			break;

		case TIMER_API_CHECK_IF_COUNTDOWN_ELAPSED :
			DEV_IOCTL(handle->hw_timer,
					IOCTL_GET_CURRENT_TIMER_VALUE, (void*)&curr_timer_val);
			if(curr_timer_val > handle->countdown_value)
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
