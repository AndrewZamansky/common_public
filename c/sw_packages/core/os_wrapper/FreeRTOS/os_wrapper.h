/*
 * file : _os_wrapper.h
 *
 *
 */

#ifndef _os_wrapper_H
#define _os_wrapper_H

#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

#define OS_API_VERSION  20191007


#ifndef CONFIG_USE_ONLY_OS_API
	#include "dev_management_api.h"
#endif

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"



#define OS_MUTEX_TAKE_SUCCESS    pdTRUE
#define OS_QUEUE_SEND_SUCCESS    pdTRUE
#define OS_QUEUE_RECEIVE_SUCCESS pdTRUE
#define OS_QUEUE_RECEIVE_FAILED  pdFALSE


/*define maximal priority for interrupt that can call OS API */
 //TODO : 4 for priority bits . should be changed to generic model !!
#define OS_MAX_INTERRUPT_PRIORITY_FOR_API_CALLS  \
						(configMAX_SYSCALL_INTERRUPT_PRIORITY >> 4)


/***********  mutex type definition *******/
/** os_create_mutex should return os_mutex_t  **/
/** type : [os_mutex_t]  **/
typedef SemaphoreHandle_t  os_mutex_t;


/***********  queue type definition *******/
/** os_create_queue should return os_queue_t  **/
/** type : [os_queue_t]  **/
typedef QueueHandle_t  os_queue_t;



#ifndef CONFIG_USE_ONLY_OS_API

/***********  init os **********/
/** function : [os_init()]  **/
void os_init(void);

/***********  setting ticker device **********/
/** function : [os_set_tick_timer_dev()]  **/
void  os_set_tick_timer_dev(struct dev_desc_t *a_timer_dev);

/****setting tick callback (if application need to perform something each tick)
 *
 * function : [os_set_tick_callback()]
 */
void  os_set_heartbeat_dev(struct dev_desc_t *a_heartbeat_dev);


/***********  starting os **********/
/** function : [os_start()]  **/
void  os_start(void);

#endif //#ifndef CONFIG_USE_ONLY_OS_API


/***********  mutex creation **********/
/** os_create_mutex should return os_mutex_t  **/

/** function : [os_create_queue()]  **/
#define os_create_mutex()  xSemaphoreCreateMutex()

/** function : [os_create_queue()]  **/
//#define os_create_recursive_mutex()  xSemaphoreCreateRecursiveMutex()
#if defined(CONFIG_FREERTOS_USE_COUNTING_SEMAPHORES)
	#define os_create_recursive_mutex()  CRITICAL_ERROR("not implemented yet")
#endif

/** function : [os_create_semaphore()]  **/
#define os_create_semaphore(initial_value) \
			xSemaphoreCreateCounting(0xffffffff, initial_value)


/***********  mutex take **********/
/** on success will return OS_QUEUE_SEND_SUCCESS  **/

/** function : [os_mutex_take_with_timeout()]  **/
#define os_mutex_take_with_timeout(mutex , timeout)  \
								xSemaphoreTake(mutex, timeout)


//infinite wait
/** function : [os_mutex_take_infinite_wait()]  **/
#define os_mutex_take_infinite_wait(mutex  )   \
			xSemaphoreTake(mutex, ( TickType_t ) portMAX_DELAY)

/** function : [os_semaphore_take_with_timeout()]  **/
#define os_semaphore_take_with_timeout(sem , timeout)  \
								xSemaphoreTake(sem, timeout)

/** function : [os_semaphore_take_infinite_wait()]  **/
#define os_semaphore_take_infinite_wait(sem  )   \
			xSemaphoreTake(sem, ( TickType_t ) portMAX_DELAY)

/** function : [os_semaphore_try_wait()]  **/
#define os_semaphore_try_wait(sem)   xSemaphoreTake(sem, ( TickType_t ) 0)


/***********  mutex give **********/
/** on success will return OS_QUEUE_RECEIVE_SUCCESS  **/

/** function : [os_mutex_give()]  **/
#define os_mutex_give(mutex )  xSemaphoreGive(mutex)

/** function : [os_semaphore_give()]  **/
#define os_semaphore_give(sem)  xSemaphoreGive(sem)


/***********  semaphore get count **********/
#define os_semaphore_get_count(sem) uxSemaphoreGetCount(sem)


/***********  mutex delete **********/
#define os_delete_mutex(mutex) vSemaphoreDelete(mutex)

/***********  semaphore delete **********/
#define os_delete_semaphore(sem) vSemaphoreDelete(sem)



/***********  queue creation **********/
/** os_create_queue should return os_queue_t  **/

/** function : [os_create_queue()]  **/
#define os_create_queue(num_of_elements, size_of_elements) \
		xQueueCreate(num_of_elements, size_of_elements)




/***********  queue insertion **********/
/** on success will return OS_QUEUE_SEND_SUCCESS  **/

/** function : [os_queue_send_with_timeout()]  **/
#define os_queue_send_with_timeout(queue, pData, timeout) \
		xQueueSend(queue, pData, timeout)

// without wait
/** function : [os_queue_send_without_wait()]  **/
uint8_t os_queue_send_without_wait(os_queue_t queue,  void * pData);


//infinite wait
/** function : [os_queue_send_infinite_wait()]  **/
#define os_queue_send_infinite_wait(queue , pData) \
		xQueueSend(queue, ( void * ) pData, ( TickType_t ) portMAX_DELAY)



/***********  queue retrieval **********/
/** on success will return OS_QUEUE_RECEIVE_SUCCESS  **/

/** function : [os_queue_receive_with_timeout()]  **/
#define os_queue_receive_with_timeout(queue , pData , timeout) \
		xQueueReceive(queue, pData, ( TickType_t ) timeout )

//infinite wait
/** function : [os_queue_receive_with_timeout()]  **/
#define os_queue_receive_infinite_wait(queue , pData ) \
		xQueueReceive(queue, pData, ( TickType_t ) portMAX_DELAY )



/***********  task creation **********/
/** function : [os_create_task()]  **/
void *os_create_task_FreeRTOS(
		char *taskName, void (*taskFunction)(void *apParam),
		void *taskFunctionParam, size_t stack_size_bytes, uint8_t priority);

#define os_create_task(name, func, funcParam, stack_size_bytes, priority) \
	os_create_task_FreeRTOS(name, func, funcParam, stack_size_bytes, priority)



/***********  delay **********/
/** function : [os_delay_ms()]  **/
#define os_delay_ms   vTaskDelay




/*********** thread safe  malloc/free **********/
/** function : [os_safe_malloc()]  **/
#define os_safe_malloc   pvPortMalloc

/** function : [os_safe_malloc()]  **/
#define os_safe_free   vPortFree

/** function : [os_safe_realloc()]  **/
void *pvPortRealloc(void *p, size_t xWantedSize );
#define os_safe_realloc   pvPortRealloc

/********* stack debug ********/
#ifdef CONFIG_TEST_TASK_STACK

void os_stack_test_free_rtos(uint32_t *p_lowest_stack , const char *task_name );
#define os_stack_test()    \
{                          \
	static uint32_t lowest_stack = 0xffffffff;             \
	os_stack_test_free_rtos(&lowest_stack, __FUNCTION__); \
}

#define os_low_stack_trap(low_limit)   \
		if(low_limit > uxTaskGetStackHighWaterMark( NULL )) while(1);

#else

#define os_stack_test()
#define os_low_stack_trap(low_limit)

#endif


#define OS_WRAPPER_INCLUDED

#endif /* */
