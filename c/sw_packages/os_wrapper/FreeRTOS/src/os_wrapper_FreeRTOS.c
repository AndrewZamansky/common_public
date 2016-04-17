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

BaseType_t xDummyHigherPriorityTaskWoken ;

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
