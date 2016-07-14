/*
 * file : shell_prerequirements_check.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _shell_prerequirements_check_H
#define _shell_prerequirements_check_H

#include "_project.h"
#include "dev_management_api.h"
#include "os_wrapper.h"


/***************************/
/******** test defines *****/


#ifndef SHELL_CONFIG_MAX_RX_BUFFER_SIZE
	#define SHELL_CONFIG_MAX_RX_BUFFER_SIZE 	256
#endif

#ifndef SHELL_TASK_PRIORITY
    #error "SHELL_TASK_PRIORITY should be defined in project header files"
#endif

#ifndef SHELL_TASK_STACK_SIZE
    #error "SHELL_TASK_PRIORITY should be defined in project header files"
#endif

#ifndef CONFIG_SHELL_MAX_QUEUE_LEN
    #error "CONFIG_SHELL_MAX_QUEUE_LEN should be defined in project header files"
#endif

/***************************/




/***************************/
/*** test type existence (suffix should be unique)***/

typedef uint8_t 					_if_fail_here_then___uint8_t___is_not_defined_in_project_header_files_1SHELL1;
typedef uint16_t		 			_if_fail_here_then___uint16_t___is_not_defined_in_project_header_files_1SHELL1;
typedef uint32_t   					_if_fail_here_then___uint32_t___is_not_defined_in_project_header_files_1SHELL1;
typedef pdev_descriptor_t   			_if_fail_here_then___pdev_descriptor___is_not_defined_in_project_header_files_1SHELL1;
typedef dev_param_t   				_if_fail_here_then___dev_param_t___is_not_defined_in_project_header_files_1SHELL1;


typedef ioctl_get_data_buffer_t    	_if_fail_here_then___ioctl_get_data_buffer_t___is_not_defined_in_project_header_files_1SHELL1;

/***************************/


/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/

enum
{
#ifndef IOCTL_LAST_COMMON_IOCTL
	IOCTL_LAST_COMMON_IOCTL_not_declared_in_project_header_files_1SHELL1=sizeof((int)IOCTL_LAST_COMMON_IOCTL),
#endif
#ifndef DEV_CALLBACK_1_PARAMS
	DEV_CALLBACK_1_PARAM_not_declared_in_project_header_files_1SHELL1=sizeof((int)DEV_CALLBACK_1_PARAMS),
#endif


#ifndef os_stack_test
	os_stack_test___not_declared_in_project_header_files_1SHELL1=sizeof((int)os_stack_test),
#endif


	memcpy___not_declared_in_project_header_files_1SHELL1=sizeof((int)memcpy)

};

/***************************/




#endif /* */
