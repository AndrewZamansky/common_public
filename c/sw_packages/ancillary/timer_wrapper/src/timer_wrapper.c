/*
 *
 * file :   timer_wrapper.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/
#include "src/_timer_wrapper_prerequirements_check.h"

#include "timer_wrapper.h"

#include "timer_wrapper_api.h"
#include "hw_timer_api.h"


/********  defines *********************/



/********  types  *********************/


/********  externals *********************/



/********  local variables *********************/


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        timer_wrapper_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t timer_wrapper_ioctl( pdev_descriptor_t apdev ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	timer_wrapper_instance_t *config_handle;
	timer_wrapper_runtime_instance_t *runtime_handle;
	uint64_t curr_timer_wrapper_val;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(apdev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(apdev);

	switch(aIoctl_num)
	{
		case IOCTL_SET_SERVER_DEVICE :
			config_handle->hw_timer_wrapper =  (pdev_descriptor_t)aIoctl_param1;
			break;

		case IOCTL_TIMER_WRAPPER_API_GET_RATE_HZ :
			DEV_IOCTL(config_handle->hw_timer_wrapper, IOCTL_TIMER_GET_RATE_HZ, aIoctl_param1);
			break;

		case IOCTL_TIMER_WRAPPER_API_RESTART_COUNTER :
			DEV_IOCTL(config_handle->hw_timer_wrapper,
					IOCTL_GET_CURRENT_TIMER_VALUE, (void*)&curr_timer_wrapper_val);
			runtime_handle->timer_wrapper_value_on_start = curr_timer_wrapper_val;
			break;

		case IOCTL_TIMER_WRAPPER_API_GET_COUNTER :
			DEV_IOCTL(config_handle->hw_timer_wrapper,
					IOCTL_GET_CURRENT_TIMER_VALUE, (void*)&curr_timer_wrapper_val);
			*(uint64_t*)aIoctl_param1 = curr_timer_wrapper_val - runtime_handle->timer_wrapper_value_on_start ;
			break;

		case IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_VALUE_AND_RESET :
			DEV_IOCTL(config_handle->hw_timer_wrapper,
					IOCTL_GET_CURRENT_TIMER_VALUE, (void*)&curr_timer_wrapper_val);
			runtime_handle->timer_wrapper_value_on_start = curr_timer_wrapper_val;
			runtime_handle->countdown_value = curr_timer_wrapper_val + *(uint64_t*)aIoctl_param1 ;
			break;

		case IOCTL_TIMER_WRAPPER_API_CHECK_IF_COUNTDOWN_ELAPSED :
			DEV_IOCTL(config_handle->hw_timer_wrapper,
					IOCTL_GET_CURRENT_TIMER_VALUE, (void*)&curr_timer_wrapper_val);
			if(curr_timer_wrapper_val > runtime_handle->countdown_value)
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
