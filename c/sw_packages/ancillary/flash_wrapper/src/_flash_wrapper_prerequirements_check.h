/*
 * file : _flash_wrapper_prerequirements_check.h
 *
 *
 */

#ifndef _flash_wrapper_prerequirements_check_H
#define _flash_wrapper_prerequirements_check_H


/***************************/
/******** test defines *****/

#ifndef FLASH_WRAPPER_TASK_PRIORITY
  #error "define FLASH_WRAPPER_TASK_PRIORITY  in  _project_tasks_defines.h"
#endif

#ifndef FLASH_WRAPPER_TASK_STACK_SIZE
  #error "define FLASH_WRAPPER_TASK_STACK_SIZE  in  _project_tasks_defines.h"
#endif


/***************************/




/***************************/
/*** test type existence (suffix should be unique)***/

#ifndef os_mutex_t
	typedef os_mutex_t 		_if_fail_here_then___os_mutex_t___is_not_declared_in_os_wrapper_h____as_required_by_flash_wrapper;
#endif


/***************************/


/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/

#ifndef os_mutex_take_infinite_wait
enum{
	os_mutex_take_infinite_wait___not_declared_in_os_wrapper_h___as_required_by_flash_wrapper = sizeof(os_mutex_take_infinite_wait)
};
#endif

#ifndef os_mutex_give
enum{
	os_mutex_give___not_declared_in_os_wrapper_h__as_required_by_flash_wrapper = sizeof(os_mutex_give)
};
#endif

#ifndef os_create_mutex
enum{
	os_create_mutex___not_declared_in_os_wrapper_h__as_required_by_flash_wrapper = sizeof(os_create_mutex)
};
#endif


#ifndef os_create_task
enum{
	os_create_task___not_declared_in_os_wrapper_h____flash_wrapper = sizeof(os_create_task)
};
#endif


#ifndef os_delay_ms
enum{
	os_delay_ms___not_declared_in_os_wrapper_h____flash_wrapper = sizeof(os_delay_ms)
};
#endif

#ifndef os_stack_test
enum{
	os_stack_test___not_declared_in_os_wrapper_h____flash_wrapper = sizeof(os_stack_test)
};
#endif


/***************************/


#endif /* */
