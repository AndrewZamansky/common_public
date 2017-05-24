/*
 * file : async_tx_wrapper_prerequirements_check.h
 *
 *
 *
 */

#ifndef _async_tx_wrapper_prerequirements_check_H
#define _async_tx_wrapper_prerequirements_check_H


/***************************/
/******** test defines *****/

#ifndef ASYNC_TX_WRAPPER_TASK_PRIORITY
    #error "ASYNC_TX_WRAPPER_TASK_PRIORITY should be defined in  _project_tasks_defines.h"
#endif

#ifndef ASYNC_TX_WRAPPER_TASK_STACK_SIZE
    #error "ASYNC_TX_WRAPPER_TASK_STACK_SIZE should be defined in  _project_tasks_defines.h"
#endif

#ifndef CONFIG_ASYNC_TX_WRAPPER_MAX_QUEUE_LEN
    #error "CONFIG_ASYNC_TX_WRAPPER_MAX_QUEUE_LEN should be defined in project header files"
#endif



/***************************/




/***************************/
/*** test type existence (suffix should be unique)***/
#ifndef os_queue_t
	typedef os_queue_t 		_if_fail_here_then___os_queue_t___is_not_declared_in_os_wrapper_h____as_required_by_async_tx_wrapper;
#endif


/***************************/


/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/



#ifndef os_queue_send_immediate
enum{
	os_queue_send_immediate___not_declared_in_os_wrapper_h__as_required_by_async_tx_wrapper = sizeof(os_queue_send_immediate)
};
#endif

#ifndef os_queue_receive_with_timeout
enum{
	os_queue_receive_with_timeout___not_declared_in_os_wrapper_h__as_required_by_async_tx_wrapper = sizeof(os_queue_receive_with_timeout)
};
#endif

#ifndef os_queue_send_infinite_wait
enum{
	os_queue_send_infinite_wait___not_declared_in_os_wrapper_h__as_required_by_async_tx_wrapper = sizeof(os_queue_send_infinite_wait)
};
#endif

#ifdef CONFIG_ASYNC_TX_WRAPPER_USE_MALLOC

	#ifndef os_safe_malloc
	enum{
		os_safe_malloc___not_declared_in_os_wrapper_h__as_required_by_async_tx_wrapper = sizeof(os_safe_malloc)
	};
	#endif

	#ifndef os_safe_free
	enum{
		os_safe_free___not_declared_in_os_wrapper_h__as_required_by_async_tx_wrapper = sizeof(os_safe_free)
	};
	#endif
#endif

#ifndef os_create_task
enum{
	os_create_task___not_declared_in_os_wrapper_h____as_required_by_async_tx_wrapper = sizeof(os_create_task)
};
#endif


#ifndef os_create_queue
enum{
	os_create_queue___not_declared_in_os_wrapper_h___as_required_by_async_tx_wrapper = sizeof(os_create_queue)
};
#endif

#ifndef os_queue_receive_infinite_wait
enum{
	os_queue_receive_infinite_wait___not_declared_in_os_wrapper_h___as_required_by_async_tx_wrapper = sizeof(os_queue_receive_infinite_wait)
};
#endif


#ifndef os_stack_test
enum{
	os_stack_test___not_declared_in_os_wrapper_h____as_required_by_async_tx_wrapper = sizeof(os_stack_test)
};
#endif


/***************************/



#endif
