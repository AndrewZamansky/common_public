/*
 *
 * file :   timer_wrapper.c
 *
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "timer_wrapper.h"

#include "timer_wrapper_api.h"
#include "hw_timer_api.h"

#include "_timer_wrapper_prerequirements_check.h"

/*following line add module to available module list for dynamic device tree*/
#include "timer_wrapper_add_component.h"

/********  defines *********************/



/********  types  *********************/


/********  externals *********************/



/********  local variables *********************/


/**
 * timer_wrapper_ioctl()
 *
 * return:
 */
uint8_t timer_wrapper_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct timer_wrapper_cfg_t *config_handle;
	struct timer_wrapper_runtime_t *runtime_handle;
	uint64_t curr_timer_wrapper_val;
	struct dev_desc_t *   hw_timer_wrapper;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);
	hw_timer_wrapper = config_handle->hw_timer_wrapper;

	switch(aIoctl_num)
	{
	case IOCTL_SET_SERVER_DEVICE :
		config_handle->hw_timer_wrapper =  (struct dev_desc_t *)aIoctl_param1;
		break;

	case IOCTL_TIMER_WRAPPER_API_GET_RATE_HZ :
		DEV_IOCTL(hw_timer_wrapper,	IOCTL_TIMER_GET_RATE_HZ, aIoctl_param1);
		break;

	case IOCTL_TIMER_WRAPPER_API_RESTART_COUNTER :
		DEV_IOCTL(hw_timer_wrapper,
				IOCTL_GET_CURRENT_TIMER_VALUE, (void*)&curr_timer_wrapper_val);
		runtime_handle->timer_wrapper_value_on_start = curr_timer_wrapper_val;
		break;

	case IOCTL_TIMER_WRAPPER_API_GET_COUNTER :
		DEV_IOCTL(hw_timer_wrapper,
				IOCTL_GET_CURRENT_TIMER_VALUE, (void*)&curr_timer_wrapper_val);
		*(uint64_t*)aIoctl_param1 = curr_timer_wrapper_val -
							runtime_handle->timer_wrapper_value_on_start ;
		break;

	case IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_VALUE_AND_RESET :
		DEV_IOCTL(hw_timer_wrapper,
				IOCTL_GET_CURRENT_TIMER_VALUE, (void*)&curr_timer_wrapper_val);
		runtime_handle->timer_wrapper_value_on_start = curr_timer_wrapper_val;
		runtime_handle->countdown_value =
				curr_timer_wrapper_val + *(uint64_t*)aIoctl_param1 ;
		break;

	case IOCTL_TIMER_WRAPPER_API_CHECK_IF_COUNTDOWN_ELAPSED :
		DEV_IOCTL(hw_timer_wrapper,
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
