/*
 * file : shell_prerequirements_check.h
 *
 *
 */

#ifndef _shell_prerequirements_check_H
#define _shell_prerequirements_check_H



/***************************/
/******** test defines *****/


#ifndef SHELL_TASK_PRIORITY
    #error "SHELL_TASK_PRIORITY should be defined in  _project_tasks_defines.h"
#endif

#ifndef SHELL_TASK_STACK_SIZE
    #error "SHELL_TASK_STACK_SIZE should be defined in  _project_tasks_defines.h"
#endif

#ifndef CONFIG_SHELL_MAX_QUEUE_LEN
    #error "CONFIG_SHELL_MAX_QUEUE_LEN should be defined in project header files"
#endif


/***************************/




/***************************/
/*** test type existence (suffix should be unique)***/

#ifndef os_queue_t
	typedef os_queue_t 		_if_fail_here_then___os_queue_t___is_not_declared_in_os_wrapper_h____as_required_by_flash_wrapper;
#endif


/***************************/


/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/




#ifndef os_queue_send_without_wait
enum{
	os_queue_send_without_wait___not_declared_in_os_wrapper_h__as_required_by_async_tx_wrapper = sizeof(os_queue_send_without_wait)
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




#endif /* */
