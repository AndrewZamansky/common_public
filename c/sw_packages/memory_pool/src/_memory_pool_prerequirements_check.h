/*
 * file : memory_pool_prerequirements_check.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _memory_pool_prerequirements_check_H
#define _memory_pool_prerequirements_check_H

#include "_project.h"


/***************************/
/******** test defines *****/





#ifndef NULL
	#error  "NULL is not defined in memory_pool_config.h "
#endif

/***************************/




/***************************/
/*** test type existence (suffix should be unique)***/

typedef uint8_t _if_fail_here_then___uint8_t___is_not_defined_in_project_header_files_1MEMPOOL1;
typedef uint16_t _if_fail_here_then___uint16_t___is_not_defined_in_project_header_files_1MEMPOOL1;
typedef uint32_t _if_fail_here_then___uint32_t___is_not_defined_in_project_header_files_1MEMPOOL1;

/***************************/


/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/




/***************************/


#endif /* */
