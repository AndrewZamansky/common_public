/*
 *
 *   file  :  STM8_TIM1.c
 *
 */



/***************   includes    *******************/
#include "src/_stm8_tim1_prerequirements_check.h"

#include "stm8_tim1_api.h"
#include "stm8_tim1.h"


#include "stm8_tim1_add_component.h"



/***************   defines    *******************/



/***************   typedefs    *******************/

/**********   external variables    **************/



/***********   local variables    **************/

static stm8_tim1_instance_t *pstm8_tim1_instanceParams;

#if defined(CONFIG_STM8_TIM1_USE_INTERNAL_COUNTER)
static volatile uint32_t currentTick=0;
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* ISR to config_handle systick                                                        */
/*---------------------------------------------------------------------------------------------------------*/
INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_BRK_IRQHandler, 11)
{
	timer_callback_func_t timer_callback;

	TIM1->SR1 = (uint8_t)(~(uint8_t)TIM1_IT_UPDATE);
#if defined(CONFIG_STM8_TIM1_USE_INTERNAL_COUNTER)
	currentTick++;
#endif
	timer_callback = pstm8_tim1_instanceParams->timer_callback;
	if(NULL != 	timer_callback)
	{
		timer_callback();
	}
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        stm8_tim1_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t stm8_tim1_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	stm8_tim1_instance_t *config_handle;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
#ifdef CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION
		case IOCTL_TIMER_RATE_HZ_SET :
			{
				config_handle->rate = ((uint32_t)aIoctl_param1);
			}
			break;

		case IOCTL_TIMER_CALLBACK_SET :
			{
				config_handle->timer_callback = ((timer_callback_func_t)aIoctl_param1);
			}
			break;

		case IOCTL_TIMER_MODE_SET :
			{
				config_handle->mode = ((size_t)aIoctl_param1);
			}
			break;
#endif
		case IOCTL_DEVICE_START :
			{
				uint32_t calculated_period_rate;

				calculated_period_rate = config_handle->calculated_period_rate;

				pstm8_tim1_instanceParams = config_handle;
			    /* Reset TIM1 */
			    TIM1_DeInit();

			    TIM1_TimeBaseInit(15, TIM1_COUNTERMODE_UP, calculated_period_rate, 0);

			    /* Generate an interrupt on timer count overflow */
			    TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE);

			    /* Enable TIM1 */
			    TIM1_Cmd(ENABLE);
			}
			break;

#if defined(CONFIG_STM8_TIM1_USE_INTERNAL_COUNTER)
		case IOCTL_GET_CURRENT_TIMER_VALUE :
			*(uint32_t *)aIoctl_param1 = currentTick ;
			break;
#endif

		default :
			return 1;
	}
	return 0;
}
