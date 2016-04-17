/*
 * file : gpio_prerequirements_check.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _gpio_nuc505_prerequirements_check_H
#define _gpio_nuc505_prerequirements_check_H


#include "_project.h"
#include "dev_managment_api.h" // for device manager defines and typedefs

/***************************/
/******** test defines *****/




/***************************/




/***************************/
/*** test type existence (suffix should be unique)***/

typedef uint8_t 					_if_fail_here_then___uint8_t___is_not_defined_in_project_header_files_1NUC505GPIO1;
typedef uint16_t		 			_if_fail_here_then___uint16_t___is_not_defined_in_project_header_files_1NUC505GPIO1;
typedef uint32_t   					_if_fail_here_then___uint32_t___is_not_defined_in_project_header_files_1NUC505GPIO1;
typedef pdev_descriptor   			_if_fail_here_then___pdev_descriptor___is_not_defined_in_project_header_files_1NUC505GPIO1;
typedef dev_param_t   				_if_fail_here_then___dev_param_t___is_not_defined_in_project_header_files_1NUC505GPIO1;


/***************************/


/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/

enum
{
#ifndef IOCTL_LAST_COMMON_IOCTL
	IOCTL_LAST_COMMON_IOCTL_not_declared_in_project_header_files_1NUC505GPIO1=sizeof((int)IOCTL_LAST_COMMON_IOCTL)
#endif

};

/***************************/


#endif /* */
