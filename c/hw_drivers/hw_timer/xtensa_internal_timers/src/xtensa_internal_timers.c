/*
 *
 *   file  :  xtensa_internal_timers.c
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "xtensa_internal_timers_api.h"
#include "xtensa_internal_timers.h"
#include "irq_api.h"
#include "clock_control_api.h"
#include "hw_timer_api.h"

#include    <xtensa/corebits.h>
#include    <xtensa/config/system.h>
#include    <xtensa/config/core.h>


#define MODULE_NAME                     xtensa_internal_timers


extern void _xtensa_internal_timer_timer_asm_init(void);
extern void _xtensa_internal_timer_timer_asm_rearm(void);



/* cached number of cycles per tick */
unsigned _xtensa_internal_timers_tick_divisor = 1;

//static volatile uint64_t currentTick=0;

/*
 * tick_handler()
 *
 * return:
 */
static void xtensa_internal_timer_tick_handler( struct dev_desc_t *adev)
{
	struct xtensa_internal_timers_instance_t *config_handle;
	struct xtensa_internal_timers_runtime_t *runtime_handle;

	timer_callback_func_t timer_callback;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(MODULE_NAME, adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(MODULE_NAME, adev);

	_xtensa_internal_timer_timer_asm_rearm();

	runtime_handle->currentTick++;
	timer_callback = config_handle->timer_callback;
	if (NULL != timer_callback)
	{
		timer_callback();
	}
}



/*
 * xtensa_internal_timers_ioctl()
 *
 * return:
 */
uint8_t xtensa_internal_timers_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct xtensa_internal_timers_instance_t *config_handle;
	struct xtensa_internal_timers_runtime_t *runtime_handle;
	uint32_t core_clock_rate;
	uint8_t timer_index;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(MODULE_NAME, adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(MODULE_NAME, adev);

	switch(aIoctl_num)
	{
	case IOCTL_TIMER_RATE_HZ_SET :
		config_handle->rate = ((uint32_t)aIoctl_param1);
		break;
	case IOCTL_TIMER_GET_RATE_HZ :
		*(uint32_t*)aIoctl_param1 = config_handle->rate;
		break;

	case IOCTL_TIMER_CALLBACK_SET :
		config_handle->timer_callback =	((timer_callback_func_t)aIoctl_param1);
		break;

	case IOCTL_DEVICE_START :
		DEV_IOCTL_1_PARAMS(config_handle->clock_dev,
				CLK_IOCTL_GET_FREQ, &core_clock_rate );

		 /* set rate */
		_xtensa_internal_timers_tick_divisor  =
								(core_clock_rate / config_handle->rate) ;

		timer_index = config_handle->timer_index;
		switch (timer_index)
		{
		case 0:
			xt_set_interrupt_handler(XCHAL_TIMER_INTERRUPT(0),
					 (xt_handler)xtensa_internal_timer_tick_handler, adev);
			_xtensa_internal_timer_timer_asm_init();
			irq_enable_interrupt(XCHAL_TIMER_INTERRUPT(0));
			break;
		default:
			CRITICAL_ERROR("not implemented yet");
			break;
		}

		runtime_handle->currentTick = 0;


		//	irq_enable_interrupt(SysTick_IRQn);
		// no need to enable systick interrupt
		break;

	case IOCTL_TIMER_STOP :
		 // SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk;
		break;

	case IOCTL_GET_CURRENT_TIMER_VALUE :
		*(uint64_t *)aIoctl_param1 = runtime_handle->currentTick ;
		break;

	default :
		return 1;
	}
	return 0;
}

#define MODULE_IOCTL_FUNCTION           xtensa_internal_timers_ioctl
#include "add_module.h"
