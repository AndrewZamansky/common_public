/*
 * file : os_wrapper_FreeRTOS.c
 *
 *
 *
 *
 *
 *
 */


#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

#include "os_wrapper.h"

BaseType_t xDummyHigherPriorityTaskWoken ;

void *os_create_task_FreeRTOS(char *taskName , void *taskFunction,
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
