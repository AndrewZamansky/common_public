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

//#define DEBUG
#include "PRINTF_api.h"

/*following line add module to available module list for dynamic device tree*/
#include "timer_wrapper_add_component.h"

/********  defines *********************/



/********  types  *********************/
enum sec_type_t {
	SEC,
	mSEC,
	uSEC
};


/********  externals *********************/



/********  local variables *********************/



/* calculating additional_ticks = (usec * rate_hz) / div
 * for example (usec and rate_hz > 1000) :
 *   additional_ticks = (usec * rate_hz) / 1000000
 */
static uint64_t calc_additional_ticks(
					uint32_t  rate_hz, uint64_t time, uint32_t div)
{
	uint64_t  numirator;

	numirator = time * rate_hz;
	// rate_hz is uint32, so after 'time' reduced bellow uint32
	// loop will stop for sure
	while (numirator / rate_hz != time) // overflow handling
	{
		if (1 == div)
		{
			CRITICAL_ERROR("countdown overflow");
		}
		time = time >> 1;
		div = div >> 1;
		numirator = time * rate_hz;
	}
	return numirator / div;
}


static void set_countdown(struct timer_wrapper_runtime_t *runtime_handle,
		struct dev_desc_t *hw_timer_wrapper, uint64_t time, uint8_t time_type)
{
	uint64_t curr_timer_wrapper_val;
	uint64_t additional_ticks;
	uint8_t was_set;
	uint32_t  rate_hz;

	DEV_IOCTL(hw_timer_wrapper,	IOCTL_TIMER_GET_RATE_HZ, &rate_hz);
	was_set = 0;
	while (0 == was_set)
	{
		switch (time_type)
		{
		case  uSEC :
			if (rate_hz <= 1000)
			{
				time /= 1000;
				time_type = mSEC;
			}
			else
			{
				additional_ticks =
						calc_additional_ticks(rate_hz, time, 1000000);
				was_set = 1;
			}
			break;
		case  mSEC :
			additional_ticks = calc_additional_ticks(rate_hz, time, 1000);
			was_set = 1;
			break;
		}
	}

	DEV_IOCTL(hw_timer_wrapper,
			IOCTL_GET_CURRENT_TIMER_VALUE, (void*)&curr_timer_wrapper_val);
	runtime_handle->timer_wrapper_value_on_start = curr_timer_wrapper_val;
	runtime_handle->countdown_value =
			curr_timer_wrapper_val + additional_ticks;
	PRINTF_DBG("timer set %llu\n", runtime_handle->countdown_value);
}


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
	struct dev_desc_t *hw_timer_wrapper;

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

	case IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_uSEC_AND_RESET :
		set_countdown(runtime_handle,
				hw_timer_wrapper, *(uint64_t*)aIoctl_param1, uSEC);
		break;
	case IOCTL_TIMER_WRAPPER_API_SET_COUNTDOWN_mSEC_AND_RESET :
		set_countdown(runtime_handle,
				hw_timer_wrapper, *(uint64_t*)aIoctl_param1, mSEC);
		break;

	case IOCTL_TIMER_WRAPPER_API_CHECK_IF_COUNTDOWN_ELAPSED :
		DEV_IOCTL(hw_timer_wrapper,
				IOCTL_GET_CURRENT_TIMER_VALUE, (void*)&curr_timer_wrapper_val);
		PRINTF_DBG("timer check %llu\n", curr_timer_wrapper_val);
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
