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
#include "global_prerequirements_check.h"


/***************************/
/******** test defines *****/





/***************************/




/***************************/
/*** test type existence (suffix should be unique)***/


/***************************/


/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/

#ifndef CRITICAL_ERROR
enum{
	CRITICAL_ERROR___not_declared_in__project_func_declarations_h__or__in__project_defines_h___as_required_by_memory_pool = sizeof(CRITICAL_ERROR)
};
#endif



/***************************/


#endif
