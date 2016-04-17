/*
 * file : wireless_uart_prerequirements_check.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _wireless_uart_prerequirements_check_H
#define _wireless_uart_prerequirements_check_H



/***************************/
/******** test defines *****/




/***************************/




/***************************/
/*** test type existence (suffix should be unique)***/
#ifndef WIRELESS_UART_CONFIG_MAX_RX_BUFFER_SIZE
	#define WIRELESS_UART_CONFIG_MAX_RX_BUFFER_SIZE 	256
#endif

typedef uint8_t 					_if_fail_here_then___uint8_t___is_not_defined_in_wireless_uart_config_h;
typedef uint16_t		 			_if_fail_here_then___uint16_t___is_not_defined_in_wireless_uart_config_h;
typedef uint32_t   					_if_fail_here_then___uint32_t___is_not_defined_in_wireless_uart_config_h;
typedef pdev_descriptor   			_if_fail_here_then___pdev_descriptor___is_not_defined_in_wireless_uart_config_h;
typedef dev_param_t   				_if_fail_here_then___dev_param_t___is_not_defined_in_wireless_uart_config_h;


typedef ioctl_get_data_buffer_t    	_if_fail_here_then___ioctl_get_data_buffer_t___is_not_defined_in_shell_config_h;

/***************************/


/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/

enum
{
#ifndef IOCTL_LAST_COMMON_IOCTL
	IOCTL_LAST_COMMON_IOCTL_not_declared_in_wireless_uart_config_h=sizeof((int)IOCTL_LAST_COMMON_IOCTL),
#endif
#ifndef DEV_CALLBACK_1_PARAMS
	DEV_CALLBACK_1_PARAMS_not_declared_in_wireless_uart_config_h=sizeof((int)DEV_CALLBACK_1_PARAMS),
#endif


#ifndef os_stack_test
	os_stack_test___not_declared_in_shell_config_h=sizeof((int)os_stack_test),
#endif

	memcpy___not_declared_in_wireless_uart_config_h=sizeof((int)memcpy)
};

/***************************/




#endif /* */
