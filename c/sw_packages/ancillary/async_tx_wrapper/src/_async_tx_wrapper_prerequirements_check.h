/*
 * file : async_tx_wrapper_prerequirements_check.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _async_tx_wrapper_prerequirements_check_H
#define _async_tx_wrapper_prerequirements_check_H

#include "_project.h"
#include "dev_management_api.h"
#include "uart_api.h"
#include "os_wrapper.h"
#include "global_prerequirements_check.h"

/***************************/
/******** test defines *****/

#ifndef ASYNC_TX_WRAPPER_TASK_PRIORITY
    #error "ASYNC_TX_WRAPPER_TASK_PRIORITY should be defined in project header files"
#endif

#ifndef ASYNC_TX_WRAPPER_TASK_STACK_SIZE
    #error "ASYNC_TX_WRAPPER_TASK_STACK_SIZE should be defined in project header files"
#endif

#ifndef CONFIG_ASYNC_TX_WRAPPER_MAX_QUEUE_LEN
    #error "CONFIG_ASYNC_TX_WRAPPER_MAX_QUEUE_LEN should be defined in project header files"
#endif



/***************************/




/***************************/
/*** test type existence (suffix should be unique)***/


/***************************/


/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/

enum
{


#ifndef os_stack_test
	os_stack_test___not_declared_in_project_header_files_as_required_by_async_tx_wrapper=sizeof((int)os_stack_test),
#endif

#ifdef CONFIG_ASYNC_TX_WRAPPER_USE_MALLOC
	memcpy___not_declared_in_project_header_files_as_required_by_async_tx_wrapper=sizeof((int)memcpy),
	malloc___not_declared_in_project_header_files_as_required_by_async_tx_wrapper=sizeof((int)malloc),
	free___not_declared_in_project_header_files_as_required_by_async_tx_wrapper=sizeof((int)free),
#endif

#ifndef busy_delay
	busy_delay___not_declared_in_project_header_files_as_required_by_async_tx_wrapper=sizeof((int)busy_delay)
#endif


};

/***************************/



#endif /* */
