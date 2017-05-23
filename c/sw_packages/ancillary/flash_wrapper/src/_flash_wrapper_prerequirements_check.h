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
enum{
	os_mutex_t___not_declared_in_os_wrapper_h = sizeof(os_mutex_t)
};
#endif



/***************************/


/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/

#ifndef os_mutex_take_infinite_wait
enum{
	os_mutex_take_infinite_wait___not_declared_in_os_wrapper_h____flash_wrapper = sizeof(os_mutex_take_infinite_wait)
};
#endif

#ifndef os_mutex_give
enum{
	os_mutex_give___not_declared_in_os_wrapper_h____flash_wrapper = sizeof(os_mutex_give)
};
#endif

#ifndef os_create_mutex
enum{
	os_create_mutex___not_declared_in_os_wrapper_h____flash_wrapper = sizeof(os_create_mutex)
};
#endif


#ifndef os_create_task
enum{
	os_create_task___not_declared_in_os_wrapper_h____flash_wrapper = sizeof(os_create_task)
};
#endif


/***************************/


#endif /* */
