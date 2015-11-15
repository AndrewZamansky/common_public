/*
 * file : _os_wrapper.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _os_wrapper_H
#define _os_wrapper_H

#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"

extern BaseType_t xDummyHigherPriorityTaskWoken ;


/**	defines :	[OS_QUEUE_SEND_SUCCESS]  **/
#define OS_QUEUE_SEND_SUCCESS		pdTRUE
/**	function :	[OS_QUEUE_RECEIVE_SUCCESS]  **/
#define OS_QUEUE_RECEIVE_SUCCESS	pdTRUE


/*define maximal priority for interrupt that can call OS API */
#define OS_MAX_INTERRUPT_PRIORITY_FOR_API_CALLS 	(configKERNEL_INTERRUPT_PRIORITY>>4) // 4 for priority bits . should be changed to generic model !!


/**	*********  mutex type definition *****    **/
/** os_create_mutex should return os_mutex_t  **/
/**	type :	[os_mutex_t]  **/
typedef SemaphoreHandle_t  os_mutex_t;


/**	*********  queue type definition *****    **/
/** os_create_queue should return os_queue_t  **/
/**	type :	[os_queue_t]  **/
typedef QueueHandle_t  os_queue_t;




/**	*********  init os ********        **/
/** 					    		   **/
/**	function :	[os_init()]  **/
#define os_init()

/**	*********  starting os ********        **/
/** 						    		   **/
/**	function :	[os_start()]  **/
#define os_start()		vTaskStartScheduler()



/**	*********  mutex creation ********        **/
/** os_create_mutex should return os_queue_t  **/

/**	function :	[os_create_queue()]  **/
#define os_create_mutex()	 xSemaphoreCreateMutex(  )

/***********  mutex take **********/
/** on success will return OS_QUEUE_SEND_SUCCESS  **/

/**	function :	[os_mutex_take_with_timeout()]  **/
#define os_mutex_take_with_timeout(mutex , timeout)	 xSemaphoreTake(mutex , timeout )



//infinite wait
/**	function :	[os_mutex_take_infinite_wait()]  **/
#define os_mutex_take_infinite_wait(mutex  )   xSemaphoreTake( mutex, ( TickType_t ) portMAX_DELAY )



/***********  mutex give **********/
/** on success will return OS_QUEUE_RECEIVE_SUCCESS  **/

/**	function :	[os_mutex_give()]  **/
#define os_mutex_give(mutex )	 xSemaphoreGive(mutex  )

//infinite wait
/**	function :	[os_queue_receive_with_timeout()]  **/
#define os_queue_receive_infinite_wait(queue , pData )	\
		xQueueReceive(queue, pData, ( TickType_t ) portMAX_DELAY )







/**	*********  queue creation ********        **/
/** os_create_queue should return os_queue_t  **/

/**	function :	[os_create_queue()]  **/
#define os_create_queue(num_of_elements , size_of_elements)	\
		xQueueCreate( num_of_elements , size_of_elements )




/***********  queue insertion **********/
/** on success will return OS_QUEUE_SEND_SUCCESS  **/

/**	function :	[os_queue_send_with_timeout()]  **/
#define os_queue_send_with_timeout(queue , pData , timeout)	\
		xQueueSend( queue , pData , timeout )

// without wait
/**	function :	[os_queue_send_immediate()]  **/
uint8_t os_queue_send_immediate(os_queue_t queue ,  void * pData  );


//infinite wait
/**	function :	[os_queue_send_infinite_wait()]  **/
#define os_queue_send_infinite_wait(queue , pData  )	\
		xQueueSend( queue, ( void * ) pData, ( TickType_t ) portMAX_DELAY )



/***********  queue retrieval **********/
/** on success will return OS_QUEUE_RECEIVE_SUCCESS  **/

/**	function :	[os_queue_receive_with_timeout()]  **/
#define os_queue_receive_with_timeout(queue , pData , timeout)	\
		xQueueReceive(queue, pData, ( TickType_t ) timeout )

//infinite wait
/**	function :	[os_queue_receive_with_timeout()]  **/
#define os_queue_receive_infinite_wait(queue , pData )	\
		xQueueReceive(queue, pData, ( TickType_t ) portMAX_DELAY )



/***********  task creation **********/
/**	function :	[os_create_task()]  **/


#define os_create_task(name , func, funcParam, stackSize , priority)	\
		os_create_task_FreeRTOS(name , func, funcParam, stackSize , priority)

void *os_create_task_FreeRTOS(char *taskName , void *taskFunction,
		void *taskFunctionParam , uint16_t stackSize , uint8_t priority);


/***********  delay **********/
/**	function :	[os_delay_ms()]  **/
#define os_delay_ms   vTaskDelay



#define OS_WRAPPER_INCLUDED

#endif /* */
