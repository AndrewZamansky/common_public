/*
 *
 *   file  :  CORTEXM_SYSTICK.c
 *
 */



/***************   includes    *******************/
#include "src/_cortexM_systick_prerequirements_check.h"

#include "cortexM_systick_api.h"
#include "cortexM_systick.h"
#include "irq_api.h"
#include "clock_control_api.h"

#include "cortexM_systick_add_component.h"

//locally disable IRQn_Type defined in soc
#define IRQn_Type IRQn_Type_local

typedef enum IRQn_local {
    /******  Cortex-M4 Processor Exceptions Numbers ***************************************************/
    NonMaskableInt_IRQn_local           = -14,      /*!<  2 Non Maskable Interrupt                        */
    MemoryManagement_IRQn_local         = -12,      /*!<  4 Memory Management Interrupt                   */
    BusFault_IRQn_local                 = -11,      /*!<  5 Bus Fault Interrupt                           */
    UsageFault_IRQn_local               = -10,      /*!<  6 Usage Fault Interrupt                         */
    SVCall_IRQn_local                   = -5,       /*!< 11 SV Call Interrupt                             */
    DebugMonitor_IRQn_local             = -4,       /*!< 12 Debug Monitor Interrupt                       */
    PendSV_IRQn_local                   = -2,       /*!< 14 Pend SV Interrupt                             */
    SysTick_IRQn_local                  = -1,       /*!< 15 System Tick Interrupt                         */

} IRQn_Type_local;

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

static CORTEXM_SYSTICK_Instance_t *pCORTEXM_SYSTICK_InstanceParams;

static volatile uint64_t currentTick=0;

/*---------------------------------------------------------------------------------------------------------*/
/* ISR to handle systick                                                        */
/*---------------------------------------------------------------------------------------------------------*/
void __attribute__((interrupt("IRQ"))) SysTick_IRQHandler(void)
{
	timer_callback_func_t timer_callback;
	currentTick++;
	timer_callback = pCORTEXM_SYSTICK_InstanceParams->timer_callback;
	if(NULL != 	timer_callback)
	{
		timer_callback();
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
uint8_t cortexM_systick_ioctl( pdev_descriptor_t apdev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	CORTEXM_SYSTICK_Instance_t *handle;

	handle = apdev->handle;
	switch(aIoctl_num)
	{
		case IOCTL_TIMER_RATE_HZ_SET :
			{
				handle->rate = ((uint32_t)aIoctl_param1);
			}
			break;

		case IOCTL_TIMER_CALLBACK_SET :
			{
				handle->timer_callback = ((timer_callback_func_t)aIoctl_param1);
			}
			break;

		case IOCTL_TIMER_MODE_SET :
			{
				handle->mode = ((size_t)aIoctl_param1);
			}
			break;

		case IOCTL_DEVICE_START :
			{
				uint32_t core_clock_rate;
				DEV_IOCTL_2_PARAMS(handle->clock_dev , IOCTL_CLOCK_CONTROL_GET_RATE , handle->clock_index , &core_clock_rate );
				pCORTEXM_SYSTICK_InstanceParams = handle;
				/* Configure SysTick to interrupt at the requested rate. */
				SysTick->LOAD  = ((core_clock_rate/handle->rate) & SysTick_LOAD_RELOAD_Msk) - 1;      /* set reload register */
				SysTick->VAL   = 0;                                          /* Load the SysTick Counter Value */
				SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
			                   SysTick_CTRL_TICKINT_Msk   |
			                   SysTick_CTRL_ENABLE_Msk;                    /* Enable SysTick IRQ and SysTick Timer */

				irq_register_interrupt(SysTick_IRQn_local , SysTick_IRQHandler);
				//	irq_enable_interrupt(SysTick_IRQn);// no need to enable systick interrupt
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
