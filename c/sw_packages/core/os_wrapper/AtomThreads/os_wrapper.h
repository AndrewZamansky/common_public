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
#include "PRINTF_api.h"

#include "atom.h"
#include "atomtimer.h"

/** defines : [OS_QUEUE_SEND_SUCCESS]  **/
#define OS_QUEUE_SEND_SUCCESS     ATOM_OK
/** function : [OS_QUEUE_RECEIVE_SUCCESS]  **/
#define OS_QUEUE_RECEIVE_SUCCESS  ATOM_OK

/***********  queue type definition *******/
/** os_create_queue should return os_queue_t  **/
/** type : [os_queue_t]  **/
typedef ATOM_QUEUE* os_queue_t;


typedef struct
{
	uint16_t size;
	uint8_t *stackLocation;
}os_stack_info_t;



/***********  init os **********/
/** function : [os_init()]  **/
#define os_init() os_init_atomthreads()

void os_init_atomthreads(void);

/***********  starting os **********/
/** function : [os_start()]  **/
#define os_start()  atomOSStart()


/***********  queue creation **********/
/** os_create_queue should return os_queue_t  **/

/** function : [os_create_queue()]  **/
#define os_create_queue(num_of_elements, size_of_elements) \
		os_create_queue_atomthreads(num_of_elements, size_of_elements)

os_queue_t os_create_queue_atomthreads(
		uint8_t num_of_elements, uint8_t size_of_element);



/***********  queue insertion **********/
/** on success will return OS_QUEUE_SEND_SUCCESS  **/

/** function : [os_queue_send_with_timeout()]  **/
#define os_queue_send_with_timeout(queue, pData, timeout) \
					atomQueuePut(queue, timeout, (uint8_t*) pData)

// without wait
/** function : [os_queue_send_without_wait()]  **/
#define os_queue_send_without_wait(queue , pData  ) \
				atomQueuePut(queue, -1, (uint8_t*) pData)

//infinite wait
/** function : [os_queue_send_infinite_wait()]  **/
#define os_queue_send_infinite_wait(queue , pData) \
				atomQueuePut(queue, 0 , (uint8_t*)pData)



/***********  queue retrieval **********/
/** on success will return OS_QUEUE_RECEIVE_SUCCESS  **/

/** function : [os_queue_receive_with_timeout()]  **/
#define os_queue_receive_with_timeout(queue, pData, timeout) \
						atomQueueGet(queue, timeout, (uint8_t*)pData)

//infinite wait
/** function : [os_queue_receive_infinite_wait()]  **/
#define os_queue_receive_infinite_wait(queue , pData ) \
						atomQueueGet(queue, 0, (uint8_t*)pData)



/***********  task creation **********/
/** function : [os_create_task()]  **/


#define os_create_task(name , func, funcParam, stackSize , priority) \
		os_create_task_atomthreads(func, funcParam, stack_size_bytes , priority)

void *os_create_task_atomthreads(void *taskFunction,
		void *taskFunctionParam, size_t stack_size_bytes, uint8_t priority);


/***********  delay **********/
/** function : [os_delay_ms()]  **/

#define os_delay_ms   os_delay_ms_atomthreads

void os_delay_ms_atomthreads( uint32_t ms);

/********* stack debug ********/
#define os_stack_test()

#define OS_WRAPPER_INCLUDED

#endif /* */
