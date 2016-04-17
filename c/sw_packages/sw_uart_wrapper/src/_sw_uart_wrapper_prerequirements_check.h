/*
 * file : sw_uart_wrapper_prerequirements_check.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _sw_uart_wrapper_prerequirements_check_H
#define _sw_uart_wrapper_prerequirements_check_H

#include "_project.h"
#include "dev_managment_api.h"
#include "uart_api.h"
#include "os_wrapper.h"

/***************************/
/******** test defines *****/

#ifndef SW_UART_WRAPPER_TASK_PRIORITY
    #error "SW_UART_WRAPPER_TASK_PRIORITY should be defined in project header files"
#endif

#ifndef SW_UART_WRAPPER_TASK_STACK_SIZE
    #error "SW_UART_WRAPPER_TASK_STACK_SIZE should be defined in project header files"
#endif

#ifndef CONFIG_SW_UART_WRAPPER_MAX_QUEUE_LEN
    #error "CONFIG_SW_UART_WRAPPER_MAX_QUEUE_LEN should be defined in project header files"
#endif


#ifndef SW_UART_WRAPPER_CONFIG_NUM_OF_DYNAMIC_INSTANCES
	#define SW_UART_WRAPPER_CONFIG_NUM_OF_DYNAMIC_INSTANCES 	1
#endif

#ifndef SW_UART_WRAPPER_CONFIG_MAX_RX_BUFFER_SIZE
	#define SW_UART_WRAPPER_CONFIG_MAX_RX_BUFFER_SIZE 	256
#endif

#ifndef SW_UART_WRAPPER_CONFIG_MAX_TX_BUFFER_SIZE
	#define SW_UART_WRAPPER_CONFIG_MAX_TX_BUFFER_SIZE 	256
#endif


/***************************/




/***************************/
/*** test type existence (suffix should be unique)***/

typedef uint8_t 					_if_fail_here_then___uint8_t___is_not_defined_in_project_header_files_1SWURTWRP1;
typedef uint16_t		 			_if_fail_here_then___uint16_t___is_not_defined_in_project_header_files_1SWURTWRP1;
typedef uint32_t   					_if_fail_here_then___uint32_t___is_not_defined_in_project_header_files_1SWURTWRP1;
typedef pdev_descriptor   			_if_fail_here_then___pdev_descriptor___is_not_defined_in_project_header_files_1SWURTWRP1;
typedef dev_param_t   				_if_fail_here_then___dev_param_t___is_not_defined_in_project_header_files_1SWURTWRP1;

typedef ioctl_get_data_buffer_t    	_if_fail_here_then___ioctl_get_data_buffer_t___is_not_defined_in_project_header_files_1SWURTWRP1;

/***************************/


/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/

enum
{
#ifndef IOCTL_LAST_COMMON_IOCTL
	IOCTL_LAST_COMMON_IOCTL_not_declared_in_project_header_files_1SWURTWRP1=sizeof((int)IOCTL_LAST_COMMON_IOCTL),
#endif
#ifndef DEV_CALLBACK_1_PARAMS
	DEV_CALLBACK_1_PARAMS_not_declared_in_project_header_files_1SWURTWRP1=sizeof((int)DEV_CALLBACK_1_PARAMS),
#endif



#ifndef os_stack_test
	os_stack_test___not_declared_in_shell_config_h=sizeof((int)os_stack_test),
#endif

#ifdef SW_UART_WRAPPER_CONFIG_USE_MALLOC
	memcpy___not_declared_in_project_header_files_1SWURTWRP1=sizeof((int)memcpy),
	malloc___not_declared_in_project_header_files_1SWURTWRP1=sizeof((int)malloc),
	free___not_declared_in_project_header_files_1SWURTWRP1=sizeof((int)free),
#endif

#ifndef busy_delay
	busy_delay___declared_in_project_header_files_1SWURTWRP1=sizeof((int)busy_delay)
#endif


};

/***************************/



#endif /* */
