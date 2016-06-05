/*
 *
 *   file  :  CORTEXM_SYSTICK.c
 *
 */



/***************   includes    *******************/
#include "src/_cortexM_systick_prerequirements_check.h"

#include "cortexM_systick_api.h"
#include "cortexM_systick.h"
#include "NVIC_api.h"
#include "clocks_api.h"



#if 1 == CONFIG_CORTEX_M4
 #include "core_cm4.h"
#elif 1 == CONFIG_CORTEX_M3
 #include "core_cm3.h"
#else
 #error unknown cortex-m type
#endif


/***************   defines    *******************/

#define portNVIC_SYSTICK_CTRL_REG			( * ( ( volatile uint32_t * ) 0xe000e010 ) )
#define portNVIC_SYSTICK_LOAD_REG			( * ( ( volatile uint32_t * ) 0xe000e014 ) )
#define portNVIC_SYSTICK_CURRENT_VALUE_REG	( * ( ( volatile uint32_t * ) 0xe000e018 ) )



/***************   typedefs    *******************/

/**********   external variables    **************/



/***********   local variables    **************/
#if I2S_NUC505_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0
 static CORTEXM_SYSTICK_Instance_t CORTEXM_SYSTICK_InstanceParams;


static const dev_param_t CORTEXM_SYSTICK_Dev_Params[]=
{
		{IOCTL_CORTEXM_SYSTICK_SET_PORT_PARAM , IOCTL_VOID , (uint8_t*)CORTEXM_SYSTICK_API_PORT_STR, NOT_FOR_SAVE},
		{IOCTL_CORTEXM_SYSTICK_SET_PIN_PARAM , IOCTL_VOID , (uint8_t*)CORTEXM_SYSTICK_API_PIN_STR, NOT_FOR_SAVE},
		{IOCTL_CORTEXM_SYSTICK_SET_MODE_PARAM , IOCTL_VOID , (uint8_t*)CORTEXM_SYSTICK_API_MODE_STR, NOT_FOR_SAVE},
};

#endif

#define INSTANCE(hndl)	((CORTEXM_SYSTICK_Instance_t*)hndl)

static CORTEXM_SYSTICK_Instance_t *pCORTEXM_SYSTICK_InstanceParams;

static volatile uint64_t currentTick=0;

/*---------------------------------------------------------------------------------------------------------*/
/* ISR to handle systick                                                        */
/*---------------------------------------------------------------------------------------------------------*/
void __attribute__((interrupt("IRQ"))) SysTick_IRQHandler(void)
{
	currentTick++;
	if(NULL != 	pCORTEXM_SYSTICK_InstanceParams->timer_callback)
	{
		pCORTEXM_SYSTICK_InstanceParams->timer_callback();
	}
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        cortexM_systick_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t cortexM_systick_ioctl( void * const aHandle ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{

	switch(aIoctl_num)
	{
#if I2S_NUC505_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0
		case IOCTL_GET_PARAMS_ARRAY_FUNC :
			*(const dev_param_t**)aIoctl_param1  = CORTEXM_SYSTICK_Dev_Params;
			*(uint8_t*)aIoctl_param2 =  sizeof(CORTEXM_SYSTICK_Dev_Params)/sizeof(dev_param_t); //size
			break;

#endif
		case IOCTL_TIMER_RATE_HZ_SET :
			{
				INSTANCE(aHandle)->rate = ((uint32_t)aIoctl_param1);
			}
			break;

		case IOCTL_TIMER_CALLBACK_SET :
			{
				INSTANCE(aHandle)->timer_callback = ((timer_callback_func_t)aIoctl_param1);
			}
			break;

		case IOCTL_TIMER_MODE_SET :
			{
				INSTANCE(aHandle)->mode = ((size_t)aIoctl_param1);
			}
			break;

		case IOCTL_DEVICE_START :
			{
				uint32_t core_clock_rate = 	clocks_api_get_rate(CONFIG_DT_CORTEX_M_SYSTICK_INPUT_CLOCK);
				pCORTEXM_SYSTICK_InstanceParams = (CORTEXM_SYSTICK_Instance_t*)aHandle;
				/* Configure SysTick to interrupt at the requested rate. */
				SysTick->LOAD  = ((core_clock_rate/INSTANCE(aHandle)->rate) & SysTick_LOAD_RELOAD_Msk) - 1;      /* set reload register */
				SysTick->VAL   = 0;                                          /* Load the SysTick Counter Value */
				SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
			                   SysTick_CTRL_TICKINT_Msk   |
			                   SysTick_CTRL_ENABLE_Msk;                    /* Enable SysTick IRQ and SysTick Timer */

				NVIC_API_RegisterInt(CONFIG_DT_CORTEX_M_SYSTICK_INTERRUPT , SysTick_IRQHandler);
				//	NVIC_API_EnableInt(SysTick_IRQn);// no need to enable systick interrupt
			}
			break;

		case IOCTL_TIMER_STOP :
			  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk;

			break;

		case IOCTL_GET_CURRENT_TIMER_VALUE :
			*(uint64_t *)aIoctl_param1 = currentTick ;
			break;

		default :
			return 1;
	}
	return 0;
}

#if I2S_NUC505_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        cortexM_systick_api_dev_descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  cortexM_systick_api_init_dev_descriptor(pdev_descriptor aDevDescriptor)
{
	if(NULL == aDevDescriptor) return 1;


	aDevDescriptor->handle = &CORTEXM_SYSTICK_InstanceParams ;
	aDevDescriptor->ioctl = cortexM_systick_ioctl;

	return 0 ;

}
#endif

