/*
 *
 * file :   heartbeat.c
 *
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "heartbeat_api.h"
#include "heartbeat.h"
#include "hw_timer_api.h"
#include "irq_api.h"
//#include "dev_management_api.h"
#include "os_wrapper.h"

#define MODULE_NAME             heartbeat

#define SKIP_MEASURES	3

static size_t empty_cpu_usage_counter_per_mSec = 1;
static volatile uint32_t cpu_usage_counter;
static volatile uint32_t cpu_usage_counter_1sec = 0;
static volatile uint32_t timer_tick_counter = 1;
static volatile uint32_t callibration_done = 0;
//uint32_t idlecpu_usage_counter;
static uint32_t cpu_usage_measure_mPercents = 0;
static int16_t one_sec_countdown = 1000;


/* Function:        heartbeat_timer_callback
 *
 *
 */
static void heartbeat_timer_callback()
{
	timer_tick_counter++;
}


static void cpu_usage_calibration(struct heartbeat_instance_t *handle)
{
	cpu_usage_counter = 0;
	DEV_IOCTL(handle->callibration_timer,
			IOCTL_TIMER_CALLBACK_SET, heartbeat_timer_callback);
	DEV_IOCTL(handle->callibration_timer, IOCTL_DEVICE_START );

	irq_unblock_all();
	while (1)
	{
		cpu_usage_counter++;
		if (0 == timer_tick_counter)
		{
			// if timer didn't tick, then continue to count CPU usage
			continue;
		}
		if(SKIP_MEASURES == callibration_done)
		{
			break; // exit while(1) loop with current cpu usage counter
		}
		callibration_done++;
		cpu_usage_counter = 0;
		timer_tick_counter = 0;
	}
	irq_block_all();
	DEV_IOCTL(handle->callibration_timer, IOCTL_TIMER_STOP );
	empty_cpu_usage_counter_per_mSec = cpu_usage_counter;
}


static void calculate_cpu_usage_per_sec(struct heartbeat_instance_t *handle)
{
	uint32_t cpu_free_mPercents;
	uint32_t one_tick_cpu_usage;
	struct dev_desc_t * heartbeat_callback_dev;

	heartbeat_callback_dev = handle->heartbeat_callback_dev;

	while (1)
	{
		cpu_usage_counter++;
		if (0 == timer_tick_counter)
		{
			// if timer didn't tick, then continue to count CPU usage
			continue;
		}

		// if for some reason current counter is bigger then counter on
		// empty CPU, then it looks that empty CPU was running slower during
		// calibration. In this case update "empty_cpu_counter"
		one_tick_cpu_usage = cpu_usage_counter / timer_tick_counter;
		if ( one_tick_cpu_usage > empty_cpu_usage_counter_per_mSec)
		{
			empty_cpu_usage_counter_per_mSec = one_tick_cpu_usage;
			one_sec_countdown = 1000;
			cpu_usage_counter_1sec = 0;
		}

		one_sec_countdown -= timer_tick_counter;
		cpu_usage_counter_1sec += cpu_usage_counter;
		if (0 >= one_sec_countdown)
		{
			cpu_free_mPercents = (cpu_usage_counter_1sec * 100) /
										empty_cpu_usage_counter_per_mSec;
			cpu_usage_measure_mPercents = 100000 - cpu_free_mPercents;
			if (NULL != heartbeat_callback_dev)
			{
				DEV_CALLBACK_0_PARAMS(heartbeat_callback_dev,
											CALLBACK_HEARTBEAT_TICK);
			}
			cpu_usage_counter_1sec = 0;
			one_sec_countdown = 1000;

			os_low_stack_trap(32);
		}
		cpu_usage_counter = 0;
		timer_tick_counter = 0;
	}
}


/**
 * heartbeat_ioctl()
 *
 * return:
 */
static uint8_t heartbeat_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct heartbeat_instance_t *handle;

	handle = DEV_GET_CONFIG_DATA_POINTER(MODULE_NAME, adev);
	switch (aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		cpu_usage_calibration(handle);
		break;

	case HEARTBEAT_API_EACH_1mS_CALL:
		timer_tick_counter++;
		break;

	case HEARTBEAT_API_CALL_FROM_IDLE_TASK :
		calculate_cpu_usage_per_sec(handle);
		break;

	case HEARTBEAT_API_GET_CPU_USAGE:
		*(uint32_t *)aIoctl_param1 = cpu_usage_measure_mPercents;
		break;

	case HEARTBEAT_API_BUSY_WAIT_mS:
		{
			size_t ticks_to_wait = (size_t)aIoctl_param1;
			ticks_to_wait = ticks_to_wait * empty_cpu_usage_counter_per_mSec;
			while (ticks_to_wait--);
		}
		break;

	default :
		return 1;
	}
	return 0;
}

#define MODULE_INIT_FUNCTION    heartbeat_init
#define MODULE_IOCTL_FUNCTION   heartbeat_ioctl
#define MODULE_HAS_NO_RUNTIME_DATA
#include "add_module.h"
