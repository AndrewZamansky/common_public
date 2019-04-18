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


#define OS_MUTEX_TAKE_SUCCESS    0
#define OS_QUEUE_SEND_SUCCESS    0
#define OS_QUEUE_RECEIVE_SUCCESS 0
#define OS_QUEUE_RECEIVE_FAILED  1


/*define maximal priority for interrupt that can call OS API */
#define OS_MAX_INTERRUPT_PRIORITY_FOR_API_CALLS


/**	*********  mutex type definition *****    **/
/** os_create_mutex should return os_mutex_t  **/
/**	type :	[os_mutex_t]  **/
typedef uint32_t *os_mutex_t;


/**	*********  queue type definition *****    **/
/** os_create_queue should return os_queue_t  **/
/**	type :	[os_queue_t]  **/
typedef uint32_t *os_queue_t;


/**	*********  init os ********        **/
/** 					    		   **/
/**	function :	[os_init()]  **/
#define os_init()

/**	*********  starting os ********        **/
/** 						    		   **/
/**	function :	[os_start()]  **/
#define os_start()



/**	*********  mutex creation ********        **/
/** os_create_mutex should return os_mutex_t  **/

/**	function :	[os_create_queue()]  **/
#define os_create_mutex()  NULL

/***********  mutex take **********/
/** on success will return OS_QUEUE_SEND_SUCCESS  **/

/**	function :	[os_mutex_take_with_timeout()]  **/
#define os_mutex_take_with_timeout(mutex , timeout)   OS_MUTEX_TAKE_SUCCESS



//infinite wait
/**	function :	[os_mutex_take_infinite_wait()]  **/
#define os_mutex_take_infinite_wait(mutex  )  OS_MUTEX_TAKE_SUCCESS



/***********  mutex give **********/
/** on success will return OS_QUEUE_RECEIVE_SUCCESS  **/

/**	function :	[os_mutex_give()]  **/
#define os_mutex_give(mutex )





/**	*********  queue creation ********        **/
/** os_create_queue should return os_queue_t  **/

/**	function :	[os_create_queue()]  **/
#define os_create_queue(num_of_elements , size_of_elements)   NULL


/***********  queue insertion **********/
/** on success will return OS_QUEUE_SEND_SUCCESS  **/

/**	function :	[os_queue_send_with_timeout()]  **/
#define os_queue_send_with_timeout(queue , pData , timeout)  \
													OS_QUEUE_SEND_SUCCESS

// without wait
/**	function :	[os_queue_send_without_wait()]  **/
#define os_queue_send_without_wait(queue , pData  )  \
													OS_QUEUE_SEND_SUCCESS


//infinite wait
/**	function :	[os_queue_send_infinite_wait()]  **/
#define os_queue_send_infinite_wait(queue , pData  )  \
													OS_QUEUE_SEND_SUCCESS


/***********  queue retrieval **********/
/** on success will return OS_QUEUE_RECEIVE_SUCCESS  **/

/**	function :	[os_queue_receive_with_timeout()]  **/
#define os_queue_receive_with_timeout(queue , pData , timeout)  \
													OS_QUEUE_RECEIVE_SUCCESS

//infinite wait
/**	function :	[os_queue_receive_with_timeout()]  **/
#define os_queue_receive_infinite_wait(queue , pData )  \
													OS_QUEUE_RECEIVE_SUCCESS


/***********  task creation **********/
/**	function :	[os_create_task()]  **/


#define os_create_task(...)


/***********  delay **********/
/**	function :	[os_delay_ms()]  **/
#define os_delay_ms(...)


/*********** thread safe  malloc/free **********/
/**	function :	[os_safe_malloc()]  **/
#define os_safe_malloc   malloc

/**	function :	[os_safe_malloc()]  **/
#define os_safe_free   free

/**	function :	[os_safe_realloc()]  **/
#define os_safe_realloc   realloc




/********* stack debug ********/

#define os_stack_test()
#define os_low_stack_trap(low_limit)


#define OS_WRAPPER_INCLUDED

#endif /* */
