/*
 * file : sw_timer_wrapper_prerequirements_check.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _sw_timer_wrapper_prerequirements_check_H
#define _sw_timer_wrapper_prerequirements_check_H



/***************************/
/******** test defines *****/



#ifndef SW_TIMER_WRAPPER_CONFIG_NUM_OF_DYNAMIC_INSTANCES
	#define SW_TIMER_WRAPPER_CONFIG_NUM_OF_DYNAMIC_INSTANCES 	1
#endif


/***************************/




/***************************/
/*** test type existence (suffix should be unique)***/

typedef uint8_t 					_if_fail_here_then___uint8_t___is_not_defined_in_sw_timer_wrapper_config_h;
typedef uint16_t		 			_if_fail_here_then___uint16_t___is_not_defined_in_sw_timer_wrapper_config_h;
typedef uint32_t   					_if_fail_here_then___uint32_t___is_not_defined_in_sw_timer_wrapper_config_h;
typedef pdev_descriptor_t   			_if_fail_here_then___pdev_descriptor___is_not_defined_in_sw_timer_wrapper_config_h;
typedef dev_param_t   				_if_fail_here_then___dev_param_t___is_not_defined_in_sw_timer_wrapper_config_h;



/***************************/


/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/

enum
{
#ifndef IOCTL_LAST_COMMON_IOCTL
	IOCTL_LAST_COMMON_IOCTL_not_declared_in_sw_timer_wrapper_config_h=sizeof((int)IOCTL_LAST_COMMON_IOCTL),
#endif
#ifndef DEV_CALLBACK_1_PARAMS
	DEV_CALLBACK_1_PARAMS_not_declared_in_sw_timer_wrapper_config_h=sizeof((int)DEV_CALLBACK_1_PARAMS),
#endif

#ifndef os_stack_test
	os_stack_test___not_declared_in_shell_config_h=sizeof((int)os_stack_test),
#endif

#ifndef busy_delay
	busy_delay___declared_in_sw_timer_wrapper_config_h=sizeof((int)busy_delay)
#endif


};

/***************************/



#endif /* */
