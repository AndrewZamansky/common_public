/*
 * file : printf_prerequirements_check.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _printf_prerequirements_check_H
#define _printf_prerequirements_check_H



/***************************/
/******** test defines *****/



#ifndef PRINTF_CONFIG_USE_MINIMAL
	#define PRINTF_CONFIG_USE_MINIMAL 	0
#endif


/***************************/




/***************************/
/*** test type existence ***/

typedef uint8_t 					_if_fail_here_then___uint8_t___is_not_defined_in_printf_config_h;
typedef uint16_t		 			_if_fail_here_then___uint16_t___is_not_defined_in_printf_config_h;
typedef uint32_t   					_if_fail_here_then___uint32_t___is_not_defined_in_printf_config_h;
typedef pdev_descriptor   			_if_fail_here_then___pdev_descriptor___is_not_defined_in_printf_config_h;
typedef dev_param_t   				_if_fail_here_then___dev_param_t___is_not_defined_in_printf_config_h;


/***************************/


/**********************************/
/*** test declaration existence ***/



/***************************/



#endif /* */
