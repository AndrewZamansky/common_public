/*
 *
 * file :   heartbeat.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/
#include "heartbeat_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_heartbeat_prerequirements_check.h" // should be after {heartbeat_config.h,dev_managment_api.h}

#include "heartbeat_api.h"

#include "heartbeat.h"

#include "hw_timer_api.h"


/********  defines *********************/

#define INSTANCE(hndl)	((heartbeat_instance_t*)hndl)


/********  types  *********************/


/********  externals *********************/



/********  local defs *********************/

volatile uint32_t cpuUsageCounter;
volatile uint32_t restart_counter=1;
volatile uint32_t callibrationDone = 0;
//uint32_t idleCpuUsageCounter;
static uint32_t cpu_usage_measure_mPercents=0;
static int16_t one_sec_countdown=1000;

#if HEARTBEAT_CONFIG_USE_AS_DYNAMIC_INSTANCE > 0
	heartbeat_instance_t heartbeat_instance;
#endif


#define SKIP_MEASURES	3
/*--------------------------------------------------------------------------*/
/* Function:        heartbeat_timer_callback
 *
 *                                                                        */
void heartbeat_timer_callback()
{
	restart_counter++;
}

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
uint8_t heartbeat_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	static size_t ticks_per_mSec=1;

	switch(aIoctl_num)
	{
		case IOCTL_GET_PARAMS_ARRAY_FUNC :
			*(uint8_t*)aIoctl_param2 =  0; //size
			break;

		case IOCTL_DEVICE_START :

			DEV_IOCTL_1_PARAMS(INSTANCE(aHandle)->callibration_timer , IOCTL_TIMER_CALLBACK_SET ,  heartbeat_timer_callback);
			DEV_IOCTL_0_PARAMS(INSTANCE(aHandle)->callibration_timer , IOCTL_DEVICE_START );

			while(1)
			{

				if(restart_counter )
				{
					if(SKIP_MEASURES == callibrationDone)
					{
						break;
					}
					callibrationDone++;
					cpuUsageCounter=0;
					restart_counter=0;
				}
				cpuUsageCounter++;
			}

			ticks_per_mSec = cpuUsageCounter;
			//idleCpuUsageCounter = cpuUsageCounter * 1000;
			cpuUsageCounter=0;



			DEV_IOCTL_0_PARAMS(INSTANCE(aHandle)->callibration_timer , IOCTL_TIMER_STOP );

			break;

#if HEARTBEAT_CONFIG_USE_AS_DYNAMIC_INSTANCE > 0
		case HEARTBEAT_API_SET_CALLBACK_FUNC_IOCTL :
			INSTANCE(aHandle)->heartbeat_callback =  (heartbeat_callback_func_t)aIoctl_param1;
			break;
#endif

		case HEARTBEAT_API_EACH_1mS_CALL:
			restart_counter++;
			break;

		case HEARTBEAT_API_CALL_FROM_IDLE_TASK :
			while(1)
			{
				if(restart_counter )
				{
					uint16_t tmp_restart_counter = restart_counter;
					restart_counter=0;

					one_sec_countdown -= tmp_restart_counter;
					if(0 >= one_sec_countdown)
					{
						cpu_usage_measure_mPercents = 100000 - (cpuUsageCounter*100)/ticks_per_mSec;
						if (NULL != INSTANCE(aHandle)->heartbeat_callback)
						{
							INSTANCE(aHandle)->heartbeat_callback();
						}
						cpuUsageCounter=0;
						one_sec_countdown=1000;

#if (1==INCLUDE_uxTaskGetStackHighWaterMark )
						{
							static  uint32_t stackLeft,minStackLeft=0xffffffff;

							stackLeft = uxTaskGetStackHighWaterMark( NULL );
							if(minStackLeft > stackLeft)
							{
								minStackLeft = stackLeft;
								// !!!! DONT USE PRINTF_DBG . IT CAN PUT IDLE TASK TO WAIT STATE . THIS IS WRONG !!
								// COMMENT THIS LINE AS SOON AS POSSIBLE
								//PRINTF_DBG("%s stack left = %d\r\n" , __FUNCTION__ ,minStackLeft);
							}
						}
#endif
					}


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
				ticks_to_wait =ticks_to_wait * ticks_per_mSec;
				while (ticks_to_wait--);
			}
			break;

		default :
			return 1;
	}
	return 0;
}

#if HEARTBEAT_CONFIG_USE_AS_DYNAMIC_INSTANCE > 0

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        heartbeat_API_Init_Dev_Descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  heartbeat_api_init_dev_descriptor(pdev_descriptor aDevDescriptor)
{

	if(NULL == aDevDescriptor) return 1;


	aDevDescriptor->ioctl = heartbeat_ioctl;

	return 0;
}

#endif

