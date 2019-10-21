/*
 *
 * file :   heartbeat.c
 *
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"


#include "heartbeat_api.h"

#include "heartbeat.h"

#include "hw_timer_api.h"

#include "irq_api.h"

//#include "dev_management_api.h"
#include "os_wrapper.h"

#define MODULE_NAME             heartbeat

static volatile uint32_t cpuUsageCounter;
static volatile uint32_t cpu1secUsageCounter = 0;
static volatile uint32_t restart_counter = 1;
static volatile uint32_t callibrationDone = 0;
//uint32_t idleCpuUsageCounter;
static uint32_t cpu_usage_measure_mPercents = 0;
static int16_t one_sec_countdown = 1000;


#define SKIP_MEASURES	3

/* Function:        heartbeat_timer_callback
 *
 *
 */
static void heartbeat_timer_callback()
{
	restart_counter++;
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
	static size_t ticks_per_mSec = 1;

	handle = DEV_GET_CONFIG_DATA_POINTER(MODULE_NAME, adev);
	switch (aIoctl_num)
	{
	case IOCTL_DEVICE_START :

		DEV_IOCTL_1_PARAMS(handle->callibration_timer ,
				IOCTL_TIMER_CALLBACK_SET, heartbeat_timer_callback);
		DEV_IOCTL_0_PARAMS(handle->callibration_timer, IOCTL_DEVICE_START );

		irq_unblock_all();
		while (1)
		{
			if ( restart_counter )
			{
				if(SKIP_MEASURES == callibrationDone)
				{
					break;
				}
				callibrationDone++;
				cpuUsageCounter = 0;
				restart_counter = 0;
			}
			cpuUsageCounter++;
		}
		irq_block_all();

		ticks_per_mSec = cpuUsageCounter;
		//idleCpuUsageCounter = cpuUsageCounter * 1000;
		cpuUsageCounter=0;

		callibrationDone++;


		DEV_IOCTL_0_PARAMS(handle->callibration_timer , IOCTL_TIMER_STOP );

		break;


	case HEARTBEAT_API_EACH_1mS_CALL:
		restart_counter++;
		break;

	case HEARTBEAT_API_CALL_FROM_IDLE_TASK :
		while (1)
		{
			if (restart_counter )
			{
				uint16_t tmp_restart_counter = restart_counter;

				one_sec_countdown -= tmp_restart_counter;
				cpu1secUsageCounter += cpuUsageCounter;
				if (0 >= one_sec_countdown)
				{
					struct dev_desc_t * heartbeat_callback_dev;
					cpu_usage_measure_mPercents = 100000 -
							( (cpu1secUsageCounter * 100) / ticks_per_mSec );
					heartbeat_callback_dev = handle->heartbeat_callback_dev;
					if (NULL != heartbeat_callback_dev)
					{
						DEV_CALLBACK_0_PARAMS( heartbeat_callback_dev,
												HEARTBEAT_API_HEARTBEAT_TICK );
					}
					cpu1secUsageCounter = 0;
					one_sec_countdown = 1000;

					os_low_stack_trap(32);
				}
				cpuUsageCounter = cpuUsageCounter / restart_counter;
				if ( cpuUsageCounter > ticks_per_mSec)
				{
					ticks_per_mSec = cpuUsageCounter;
					one_sec_countdown = 1000;
					cpu1secUsageCounter = 0;
				}
				restart_counter = 0;
				cpuUsageCounter = 0;
			}
			cpuUsageCounter++;

		}
		break;

	case HEARTBEAT_API_GET_CPU_USAGE:
		*(uint32_t *)aIoctl_param1 = cpu_usage_measure_mPercents;
		break;

	case HEARTBEAT_API_BUSY_WAIT_mS:
		{
			size_t ticks_to_wait = (size_t)aIoctl_param1;
			ticks_to_wait = ticks_to_wait * ticks_per_mSec;
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
