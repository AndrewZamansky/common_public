/*
 * file : global_prerequirements_check.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _global_prerequirements_check_H
#define _global_prerequirements_check_H



/***************************/
/******** test defines *****/

#ifndef VERSION_STR
    #error "VERSION_STR should be defined in project header files or in compile command line"
#endif


#ifndef NULL
	#error  "NULL is not defined in in project header files "
#endif


/***************************/




/***************************/
/*** test type existence (suffix should be unique)***/

typedef uint8_t 					_if_fail_here_then___uint8_t___is_not_defined_in_project_header_files ;
typedef uint16_t		 			_if_fail_here_then___uint16_t___is_not_defined_in_project_header_files ;
typedef uint32_t   					_if_fail_here_then___uint32_t___is_not_defined_in_project_header_files ;

/***************************/


/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/


/***************************/



#endif /* */
