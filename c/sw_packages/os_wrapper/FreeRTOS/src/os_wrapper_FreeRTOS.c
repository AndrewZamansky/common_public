/*
 * file : os_wrapper_FreeRTOS.c
 *
 *
 *
 *
 *
 *
 */


#include "_project.h"

#define DEBUG
#include "PRINTF_api.h"
#include "os_wrapper.h"
#include "hw_timer_api.h"


extern void os_start_arch_related_components(void) ;
extern void xPortSysTickHandler(void);

BaseType_t xDummyHigherPriorityTaskWoken ;
pdev_descriptor_t timer_dev = NULL;
app_tick_callback_func_t app_tick_callback_func = NULL;
app_idle_entrance_callback_func_t app_idle_entrance_callback_func = NULL;



void *os_create_task_FreeRTOS(char *taskName , void (*taskFunction)(void *apParam),
		void *taskFunctionParam , uint16_t stackSize , uint8_t priority)
{
	TaskHandle_t xHandle=NULL;
	xTaskCreate( (TaskFunction_t)taskFunction, taskName,
			 stackSize,(void*) taskFunctionParam,
			 priority , &xHandle  );
	return (void*)xHandle;
}


uint8_t os_queue_send_immediate(os_queue_t queue ,  void * pData  )
{
	uint8_t retVal;
	BaseType_t xHigherPriorityTaskWoken ;

	retVal = xQueueSendFromISR( queue, ( void * ) pData,  &xHigherPriorityTaskWoken );
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
	return retVal;

}


void os_start(void)
{
	if (NULL == timer_dev)
	{
		return ;
	}

	os_start_arch_related_components();
	vTaskStartScheduler();
}

void system_tick_callback(void)
{
	if (NULL != app_tick_callback_func)
	{
		app_tick_callback_func();
	}
	xPortSysTickHandler();
}

void vApplicationIdleHook()
{
	if (NULL != app_idle_entrance_callback_func)
	{
		app_idle_entrance_callback_func();
	}
}

/* implement  vPortSetupTimerInterrupt() function
 * to disable weak copy of this function in port.c if local systick
 * code are used to start systick
 */
void vPortSetupTimerInterrupt( void )
{
	DEV_IOCTL_1_PARAMS(timer_dev , IOCTL_TIMER_CALLBACK_SET , (void*) system_tick_callback);
	DEV_IOCTL_0_PARAMS(timer_dev , IOCTL_DEVICE_START );
}


void  os_set_tick_timer_dev(pdev_descriptor_t a_timer_dev)
{
	timer_dev = a_timer_dev;
}

void  os_set_tick_callback(app_tick_callback_func_t a_app_tick_callback_func)
{
	app_tick_callback_func = a_app_tick_callback_func;
}

void  os_set_idle_entrance_callback(app_idle_entrance_callback_func_t a_app_idle_entrance_callback_func)
{
	app_idle_entrance_callback_func = a_app_idle_entrance_callback_func;
}

#ifdef CONFIG_TEST_TASK_STACK

void os_stack_test_free_rtos(uint32_t *p_lowest_stack ,const char *task_name )
{
	uint32_t stackLeft;

	stackLeft = uxTaskGetStackHighWaterMark( NULL );
	if(*p_lowest_stack > stackLeft)
	{
		*p_lowest_stack = stackLeft;
		PRINTF_DBG("%s stack left = %d\r\n" , task_name ,*p_lowest_stack);
	}
}

#endif
