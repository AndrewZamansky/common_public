/*
 * file : os_wrapper_FreeRTOS.c
 *
 *
 *
 */


#include "_project.h"
#include "errors_api.h"

#ifdef CONFIG_TEST_TASK_STACK
	#define DEBUG
	#include "PRINTF_api.h"
#endif

#include "os_wrapper.h"
#include "hw_timer_api.h"
#ifdef CONFIG_INCLUDE_HEARTBEAT
#include "heartbeat_api.h"
#endif
#include "freertos_internal.h"

extern void os_start_arch_related_components(void);
extern void os_sleep_arch_related_components(void);
extern void xPortSysTickHandler(void);

static struct dev_desc_t * l_timer_dev = NULL;
static struct dev_desc_t * l_heartbeat_dev = NULL;

const uint8_t OS_API_VER_VARIABLE(OS_API_VERSION);
static uint8_t os_started = 0;

void *os_safe_malloc(size_t xSize)
{
	return os_started ? pvPortMalloc(xSize) : malloc(xSize);
}

void os_safe_free(void *pv)
{
	return os_started ? vPortFree(pv) : free(pv);
}

void *os_safe_realloc(void *p, size_t xWantedSize)
{
	void *pvReturn;

	pvReturn = os_safe_malloc(xWantedSize);
	errors_api_check_if_malloc_succeed(pvReturn);
	if (NULL != p)
	{
		memcpy(pvReturn, p, xWantedSize);
		os_safe_free(p);
	}

	#if( configUSE_MALLOC_FAILED_HOOK == 1 )
	{
		if ((os_started) && ( pvReturn == NULL ))
		{
			extern void vApplicationMallocFailedHook( void );
			vApplicationMallocFailedHook();
		}
	}
	#endif

	return pvReturn;
}

os_task_t os_create_task_FreeRTOS(
		char *taskName, void (*taskFunction)(void *apParam),
		void *taskFunctionParam, size_t stack_size_bytes, uint8_t priority)
{
	TaskHandle_t xHandle = NULL;
	xTaskCreate( (TaskFunction_t)taskFunction, taskName,
			(stack_size_bytes / sizeof(StackType_t)),
			(void*) taskFunctionParam, priority, &xHandle);
	return xHandle;
}


void *os_create_detached_task(
		char *taskName, void (*taskFunction)(void *apParam),
		void *taskFunctionParam, uint16_t stack_size_bytes, uint8_t priority)
{
	CRITICAL_ERROR("not implemented yet");
}


#ifndef portEND_SWITCHING_ISR
	#define portEND_SWITCHING_ISR(...)   portYIELD_FROM_ISR()
#endif

uint8_t os_queue_send_without_wait(os_queue_t queue, void * pData)
{
	uint8_t retVal;
	BaseType_t xHigherPriorityTaskWoken ;

	if (0 == os_started) CRITICAL_ERROR("os not started yet");

	xHigherPriorityTaskWoken = pdFALSE ;
	retVal = xQueueSendFromISR(queue, (void*)pData, &xHigherPriorityTaskWoken);
	if ((pdTRUE == retVal) && (pdTRUE == xHigherPriorityTaskWoken))
	{
		portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
	}
	return retVal;
}


void os_start(void)
{
	if (NULL == l_timer_dev)
	{
		return ;
	}

	os_start_arch_related_components();
	os_started = 1;// on success vTaskStartScheduler() will not return
	vTaskStartScheduler();
}

void system_tick_callback(void)
{
#ifdef CONFIG_INCLUDE_HEARTBEAT
	if(NULL != l_heartbeat_dev)
	{
		DEV_IOCTL(l_heartbeat_dev, HEARTBEAT_API_EACH_1mS_CALL );
	}
#endif
	xPortSysTickHandler();
}

void vApplicationIdleHook()
{
#ifdef CONFIG_INCLUDE_HEARTBEAT
	if(NULL != l_heartbeat_dev)
	{
		DEV_IOCTL(l_heartbeat_dev, HEARTBEAT_API_CALL_FROM_IDLE_TASK);
	}
#endif
	os_sleep_arch_related_components();
}


/* implement  vPortSetupTimerInterrupt() function
 * to disable weak copy of this function in port.c if local systick
 * code are used to start systick
 */
void vPortSetupTimerInterrupt( void )
{
	DEV_IOCTL(l_timer_dev, IOCTL_TIMER_CALLBACK_SET, system_tick_callback);
	DEV_IOCTL(l_timer_dev, IOCTL_DEVICE_START );
}


void  os_set_tick_timer_dev(struct dev_desc_t *a_timer_dev)
{
	l_timer_dev = a_timer_dev;
}

void  os_set_heartbeat_dev(struct dev_desc_t *a_heartbeat_dev)
{
	l_heartbeat_dev = a_heartbeat_dev;
}

void os_init(void)
{
#ifdef CONFIG_INCLUDE_HEARTBEAT
	if(NULL != l_heartbeat_dev)
	{
		DEV_IOCTL(l_heartbeat_dev, IOCTL_DEVICE_START);
	}
#endif
}

#ifdef CONFIG_TEST_TASK_STACK

void os_stack_test_free_rtos(uint32_t *p_lowest_stack, const char *task_name)
{
	uint32_t stackLeft;

	stackLeft = uxTaskGetStackHighWaterMark( NULL );
	stackLeft *= sizeof(StackType_t);
	if (stackLeft < 100)
	{
		CRITICAL_ERROR("stack too low");
	}
	if(*p_lowest_stack > stackLeft)
	{
		*p_lowest_stack = stackLeft;
		PRINTF_DBG("%s stack left = %d\r\n", task_name, *p_lowest_stack);
	}
}

#endif
