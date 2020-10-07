/*
 *
 * file :   errors.c
 *
 *
 */

#include "errors_api.h"
#include "_project_typedefs.h"
#include "_project_defines.h"
#include "errors.h"

const uint8_t ERRORS_API_VER_VARIABLE(ERRORS_API_VERSION) = 0;

void errors_api_check_if_malloc_succeed(void *pointer)
{
	if (NULL == pointer)
	{
		CRITICAL_ERROR("no heap memory");
	}
}
