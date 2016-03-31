/*
 * file : semihosting_prerequirements_check.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _semihosting_prerequirements_check_H
#define _semihosting_prerequirements_check_H



/***************************/
/******** test defines *****/




/***************************/




/***************************/
/*** test type existence ***/

typedef uint8_t 					_if_fail_here_then___uint8_t___is_not_defined_in_semihosting_config_h;
typedef uint16_t		 			_if_fail_here_then___uint16_t___is_not_defined_in_semihosting_config_h;
typedef uint32_t   					_if_fail_here_then___uint32_t___is_not_defined_in_semihosting_config_h;
typedef pdev_descriptor   			_if_fail_here_then___pdev_descriptor___is_not_defined_in_semihosting_config_h;
typedef dev_param_t   				_if_fail_here_then___dev_param_t___is_not_defined_in_semihosting_config_h;


/***************************/


/**********************************/
/*** test variable/function declaration existence ***/

enum
{
#ifndef IOCTL_LAST_COMMON_IOCTL
	IOCTL_LAST_COMMON_IOCTL_not_declared_in_semihosting_config_h=sizeof((int)IOCTL_LAST_COMMON_IOCTL)
#endif

};

/***************************/


#endif /* */
