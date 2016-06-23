/*
 * file : timer_prerequirements_check.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _timer_prerequirements_check_H
#define _timer_prerequirements_check_H


#include "_project.h"
#include "dev_managment_api.h"

/***************************/
/******** test defines *****/

/***************************/




/***************************/
/*** test type existence (suffix should be unique)***/

typedef uint8_t 					_if_fail_here_then___uint8_t___is_not_defined_in_project_header_files_1TIMERWRP1;
typedef uint16_t		 			_if_fail_here_then___uint16_t___is_not_defined_in_project_header_files_1TIMERWRP1;
typedef uint32_t   					_if_fail_here_then___uint32_t___is_not_defined_in_project_header_files_1TIMERWRP1;
typedef pdev_descriptor_t   			_if_fail_here_then___pdev_descriptor___is_not_defined_in_project_header_files_1TIMERWRP1;
typedef dev_param_t   				_if_fail_here_then___dev_param_t___is_not_defined_in_project_header_files_1TIMERWRP1;


typedef ioctl_get_data_buffer_t    	_if_fail_here_then___ioctl_get_data_buffer_t___is_not_defined_in_project_header_files_1TIMERWRP1;

/***************************/


/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/

enum
{
#ifndef IOCTL_LAST_COMMON_IOCTL
	IOCTL_LAST_COMMON_IOCTL_not_declared_in_project_header_files_1TIMERWRP1=sizeof((int)IOCTL_LAST_COMMON_IOCTL),
#endif
#ifndef DEV_CALLBACK_1_PARAMS
	DEV_CALLBACK_1_PARAM_not_declared_in_project_header_files_1TIMERWRP1=sizeof((int)DEV_CALLBACK_1_PARAMS),
#endif


	memcpy___not_declared_in_project_header_files_1TIMERWRP1=sizeof((int)memcpy)


};

/***************************/




#endif /* */
