/*
 *
 * file :   errors.c
 *
 *
 */



/********  includes *********************/
#include "errors_api.h"
#include "_project_typedefs.h"
#include "_project_defines.h"


/********  defines *********************/


/********  types  *********************/


/********  externals *********************/

/********  globals *********************/

/***********************************/
/********** ver_dev ********/


/********  local defs *********************/


void errors_api_check_if_malloc_succeed(void *pointer)
{
	if (NULL == pointer)
	{
		CRITICAL_ERROR("no heap memory");
	}
}
