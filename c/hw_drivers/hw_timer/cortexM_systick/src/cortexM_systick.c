/*
 *
 *   file  :  cortexM_systick.c
 *
 */
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "cortexM_systick_api.h"
#include "cortexM_systick.h"

#include "irq_api.h"
#include "clock_control_api.h"
#include "hw_timer_api.h"

enum IRQn_local {
    /******  Cortex-M4 Processor Exceptions Numbers ****************/
    NonMaskableInt_IRQn_local   = -14,   /*!<  2 Non Maskable Interrupt  */
    MemoryManagement_IRQn_local = -12,   /*!<  4 Memory Management Interrupt */
    BusFault_IRQn_local         = -11,   /*!<  5 Bus Fault Interrupt    */
    UsageFault_IRQn_local       = -10,   /*!<  6 Usage Fault Interrupt  */
    SVCall_IRQn_local           = -5,    /*!< 11 SV Call Interrupt      */
    DebugMonitor_IRQn_local     = -4,    /*!< 12 Debug Monitor Interrupt */
    PendSV_IRQn_local           = -2,    /*!< 14 Pend SV Interrupt       */
    SysTick_IRQn_local          = -1,    /*!< 15 System Tick Interrupt    */
};

#if 1 == CONFIG_CORTEX_M0
 #include "core_cm0.h"
#elif 1 == CONFIG_CORTEX_M4
 #include "core_cm4.h"
#elif 1 == CONFIG_CORTEX_M3
 #include "core_cm3.h"
#else
 #error unknown cortex-m type
#endif


static struct cortexM_systick_instance_t *pcortexM_systick_instanceParams;

static volatile uint64_t currentTick=0;
//volatile uint64_t currentTick = 0; // global variable is used for debug

/*
 * SysTick_IRQHandler()
 *
 * return:
 */
static void __attribute__((interrupt("IRQ"))) SysTick_IRQHandler(void)
{
	timer_callback_func_t timer_callback;

	currentTick++;
	timer_callback = pcortexM_systick_instanceParams->timer_callback;
	if (NULL != timer_callback)
	{
		timer_callback();
	}
}



/*
 * cortexM_systick_ioctl()
 *
 * return:
 */
static uint8_t cortexM_systick_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct cortexM_systick_instance_t *config_handle;
	uint32_t core_clock_rate;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(cortexM_systick, adev);
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

	case IOCTL_TIMER_MODE_SET :
		config_handle->mode = ((size_t)aIoctl_param1);
		break;

	case IOCTL_DEVICE_START :
		DEV_IOCTL(config_handle->clock_dev, IOCTL_DEVICE_START);
		DEV_IOCTL(config_handle->clock_dev,
				CLK_IOCTL_GET_FREQ, &core_clock_rate );
		pcortexM_systick_instanceParams = config_handle;
		/* Configure SysTick to interrupt at the requested rate. */

		 /* set reload register */
		SysTick->LOAD  = ((core_clock_rate/config_handle->rate) &
				SysTick_LOAD_RELOAD_Msk) - 1;
		 /* Load the SysTick Counter Value */
		SysTick->VAL   = 0;
		 /* Enable SysTick IRQ and SysTick Timer */
		SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
						SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

		irq_register_interrupt(SysTick_IRQn_local, SysTick_IRQHandler);
		//	irq_enable_interrupt(SysTick_IRQn);
		// no need to enable systick interrupt
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

#define MODULE_NAME                  cortexM_systick
#define MODULE_IOCTL_FUNCTION        cortexM_systick_ioctl
#define MODULE_HAS_NO_RUNTIME_DATA
#include "add_module.h"
