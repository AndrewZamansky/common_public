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


/**	defines :	[OS_QUEUE_SEND_SUCCESS]  **/
#define OS_QUEUE_SEND_SUCCESS
/**	function :	[OS_QUEUE_RECEIVE_SUCCESS]  **/
#define OS_QUEUE_RECEIVE_SUCCESS


/*define maximal priority for interrupt that can call OS API */
#define OS_MAX_INTERRUPT_PRIORITY_FOR_API_CALLS


/**	*********  mutex type definition *****    **/
/** os_create_mutex should return os_mutex_t  **/
/**	type :	[os_mutex_t]  **/


/**	*********  queue type definition *****    **/
/** os_create_queue should return os_queue_t  **/
/**	type :	[os_queue_t]  **/


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
#define os_create_mutex()

/***********  mutex take **********/
/** on success will return OS_QUEUE_SEND_SUCCESS  **/

/**	function :	[os_mutex_take_with_timeout()]  **/
#define os_mutex_take_with_timeout(mutex , timeout)



//infinite wait
/**	function :	[os_mutex_take_infinite_wait()]  **/
#define os_mutex_take_infinite_wait(mutex  )



/***********  mutex give **********/
/** on success will return OS_QUEUE_RECEIVE_SUCCESS  **/

/**	function :	[os_mutex_give()]  **/
#define os_mutex_give(mutex )




/***********  queue receive **********/
//infinite wait
/**	function :	[os_queue_receive_with_timeout()]  **/
#define os_queue_receive_with_timeout(queue , pData ,timeout )

//infinite wait
/**	function :	[os_queue_receive_infinite_wait()]  **/
#define os_queue_receive_infinite_wait(queue , pData )


/**	*********  queue creation ********        **/
/** os_create_queue should return os_queue_t  **/

/**	function :	[os_create_queue()]  **/
#define os_create_queue(num_of_elements , size_of_elements)


/***********  queue insertion **********/
/** on success will return OS_QUEUE_SEND_SUCCESS  **/

/**	function :	[os_queue_send_with_timeout()]  **/
#define os_queue_send_with_timeout(queue , pData , timeout)

// without wait
/**	function :	[os_queue_send_immediate()]  **/
#define os_queue_send_immediate(queue , pData  )


//infinite wait
/**	function :	[os_queue_send_infinite_wait()]  **/
#define os_queue_send_infinite_wait(queue , pData  )


/***********  queue retrieval **********/
/** on success will return OS_QUEUE_RECEIVE_SUCCESS  **/

/**	function :	[os_queue_receive_with_timeout()]  **/
#define os_queue_receive_with_timeout(queue , pData , timeout)

//infinite wait
/**	function :	[os_queue_receive_with_timeout()]  **/
#define os_queue_receive_infinite_wait(queue , pData )


/***********  task creation **********/
/**	function :	[os_create_task()]  **/


#define os_create_task(...)


/***********  delay **********/
/**	function :	[os_delay_ms()]  **/
#define os_delay_ms(...)


/********* stack debug ********/

#define os_stack_test()
#define os_low_stack_trap(low_limit)


#define OS_WRAPPER_INCLUDED

#endif /* */
