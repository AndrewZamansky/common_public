/*
 * file : _os_wrapper.h
 *
 *
 *
 */

#ifndef _os_wrapper_H
#define _os_wrapper_H

#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

#include "dev_management_api.h"

#include <pthread.h>
#include <mqueue.h>
#include <unistd.h>
#include <semaphore.h>

#define OS_MUTEX_TAKE_SUCCESS      0
#define OS_QUEUE_SEND_SUCCESS      0
#define OS_QUEUE_SEND_FAILED       1
#define OS_QUEUE_RECEIVE_SUCCESS   0
#define OS_QUEUE_RECEIVE_FAILED    1


#if 0
/*define maximal priority for interrupt that can call OS API */
 //TODO : 4 for priority bits . should be changed to generic model !!
#define OS_MAX_INTERRUPT_PRIORITY_FOR_API_CALLS  \
						(configMAX_SYSCALL_INTERRUPT_PRIORITY>>4)

#endif

/***********  mutex type definition *******/
/** os_create_mutex should return os_mutex_t  **/
/** type : [os_mutex_t]  **/
typedef pthread_mutex_t  *os_mutex_t;
typedef sem_t *os_semaphore_t;

struct os_wrapper_queue {
	mqd_t  mq;
	size_t element_size;
};
/***********  queue type definition *******/
/** os_create_queue should return os_queue_t  **/
/** type : [os_queue_t]  **/
typedef struct os_wrapper_queue  *os_queue_t;


#if 0


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
#endif


/***********  mutex creation **********/
/** os_create_mutex should return os_mutex_t  **/

/** function : [os_create_queue()]  **/
os_mutex_t os_create_mutex(void);

/** function : [os_create_queue()]  **/
os_mutex_t os_create_recursive_mutex(void);

/** function : [os_create_semaphore()]  **/
os_semaphore_t os_create_semaphore(uint32_t initial_value);


#if 0
/***********  mutex take **********/
/** on success will return OS_QUEUE_SEND_SUCCESS  **/

/** function : [os_mutex_take_with_timeout()]  **/
#define os_mutex_take_with_timeout(mutex , timeout)  xSemaphoreTake(mutex , timeout )


#endif

//infinite wait
/** function : [os_mutex_take_infinite_wait()]  **/
#define os_mutex_take_infinite_wait(mutex)   pthread_mutex_lock(mutex)

/** function : [os_semaphore_take_with_timeout()]  **/
uint8_t os_semaphore_take_with_timeout(os_semaphore_t sem, uint32_t timeout_ms);

/** function : [os_semaphore_take_infinite_wait()]  **/
uint8_t os_semaphore_take_infinite_wait(os_semaphore_t sem);


/** function : [os_semaphore_try_wait()]  **/
uint8_t os_semaphore_try_wait(os_semaphore_t sem);


/***********  mutex/semaphore give **********/
/** on success will return OS_QUEUE_RECEIVE_SUCCESS  **/

/** function : [os_mutex_give()]  **/
#define os_mutex_give(mutex)  pthread_mutex_unlock(mutex)

/** function : [os_semaphore_give()]  **/
uint8_t os_semaphore_give(os_semaphore_t sem);


/***********  semaphore get count **********/
uint32_t os_semaphore_get_count(os_semaphore_t sem);


/***********  mutex delete **********/
void os_delete_mutex(os_mutex_t mutex);

/***********  semaphore delete **********/
void os_delete_semaphore(os_semaphore_t sem);



/***********  queue receive **********/
//infinite wait
/**
 * os_queue_receive_with_timeout()
 *
 * retrun: OS_QUEUE_RECEIVE_SUCCESS if message received
 *      OS_QUEUE_RECEIVE_FAILED  if exit on timeout
 * */
uint8_t os_queue_receive_with_timeout(
				os_queue_t queue, void *pData, int timeout_msec);


//infinite wait
/** function : [os_queue_receive_infinite_wait()]  **/
uint8_t os_queue_receive_infinite_wait(os_queue_t queue, void *pData);





/***********  queue creation **********/
/** os_create_queue should return os_queue_t  **/

/** function : [os_create_queue()]  **/
os_queue_t os_create_queue(uint32_t num_of_elements, uint32_t size_of_elements);

#if 0
/***********  queue insertion **********/
/** on success will return OS_QUEUE_SEND_SUCCESS  **/

/** function : [os_queue_send_with_timeout()]  **/
#define os_queue_send_with_timeout(queue , pData , timeout) \
		xQueueSend( queue , pData , timeout )

#endif

// without wait
/** function : [os_queue_send_without_wait()]  **/
uint8_t os_queue_send_without_wait(os_queue_t queue ,  void const * pData  );


//infinite wait
/** function : [os_queue_send_infinite_wait()]  **/
uint8_t os_queue_send_infinite_wait(os_queue_t queue ,  void const * pData  );

#if 0

/***********  queue retrieval **********/
/** on success will return OS_QUEUE_RECEIVE_SUCCESS  **/

/** function : [os_queue_receive_with_timeout()]  **/
#define os_queue_receive_with_timeout(queue , pData , timeout) \
		xQueueReceive(queue, pData, ( TickType_t ) timeout )

//infinite wait
/** function : [os_queue_receive_with_timeout()]  **/
#define os_queue_receive_infinite_wait(queue , pData ) \
		xQueueReceive(queue, pData, ( TickType_t ) portMAX_DELAY )


#endif

/***********  task creation **********/
/** function : [os_create_task()]  **/
void *os_create_task(char *taskName , void (*taskFunction)(void *apParam),
		void *taskFunctionParam, uint16_t stack_size_bytes, uint8_t priority);

/** function : [os_create_detached_task()]  **/
void *os_create_detached_task(
		char *taskName, void (*taskFunction)(void *apParam),
		void *taskFunctionParam, uint16_t stack_size_bytes, uint8_t priority);


/***********  delay **********/
/** function : [os_delay_ms()]  **/
#define os_delay_ms(m)   usleep(1000 * m)




/*********** thread safe  malloc/free **********/
/** function : [os_safe_malloc()]  **/
#define os_safe_malloc   malloc

/** function : [os_safe_malloc()]  **/
#define os_safe_free   free

/** function : [os_safe_realloc()]  **/
#define os_safe_realloc   realloc

/********* stack debug ********/
#ifdef CONFIG_TEST_TASK_STACK

#define os_stack_test()
#define os_low_stack_trap(low_limit)

#else

#define os_stack_test()
#define os_low_stack_trap(low_limit)

#endif


#define OS_WRAPPER_INCLUDED

#endif
