/*
 * file : serial_number_prerequirements_check.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _serial_number_atmega_prerequirements_check_H
#define _serial_number_atmega_prerequirements_check_H



/***************************/
/******** test defines *****/




/***************************/




/***************************/
/*** test type existence (suffix should be unique)***/

typedef uint8_t 					_if_fail_here_then___uint8_t___is_not_defined_in_serial_number_config_h;
typedef uint16_t		 			_if_fail_here_then___uint16_t___is_not_defined_in_serial_number_config_h;
typedef uint32_t   					_if_fail_here_then___uint32_t___is_not_defined_in_serial_number_config_h;
typedef pdev_descriptor_t   			_if_fail_here_then___pdev_descriptor___is_not_defined_in_serial_number_config_h;
typedef dev_param_t   				_if_fail_here_then___dev_param_t___is_not_defined_in_serial_number_config_h;


/***************************/


/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/

enum
{

#ifndef hush32_function
	hush32_function___declared_in_crc_config_h=sizeof((int)hush32_function),
#endif
	dummy_crc_enum

};

/***************************/


#endif /* */
