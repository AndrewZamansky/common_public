/*
 * file : auto_init_prerequirements_check.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _auto_init_prerequirements_check_H
#define _auto_init_prerequirements_check_H


#include "_project.h"
/***************************/
/******** test defines *****/



/***************************/




/***************************/
/*** test type existence (suffix should be unique)***/

typedef uint8_t _if_fail_here_then___uint8_t___is_not_defined_in_project_header_files_1AUTOINIT1;
typedef uint16_t _if_fail_here_then___uint16_t___is_not_defined_in_project_header_files_1AUTOINIT1;
typedef uint32_t _if_fail_here_then___uint32_t___is_not_defined_in_project_header_files_1AUTOINIT1;

/***************************/


/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/


enum
{
	strcmp_not_declared_in_project_header_files_1AUTOINIT1=sizeof((int)strcmp)
};




/***************************/


#endif /* */
