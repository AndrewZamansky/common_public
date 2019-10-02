/*
 *
 *   file  :  errors_api_check.c
 *
 */



/***************   includes    *******************/


#include "_project_typedefs.h"
#include "_project_defines.h"

#include "errors_api.h"
#include "errors.h"

/* will create pointer to variable errors_API_ver_XXXXXXX.
 * during the linkage, if errors_ver_XXXXXXX will not be found then
 * it can mean that API and implementation is not compatible
*/
#define _VERSION_TEST_NAME(proj_name)  errors_API_test_##proj_name
#define VERSION_TEST_NAME(proj_name)  _VERSION_TEST_NAME(proj_name)

extern const uint8_t ERRORS_API_VER_VARIABLE(ERRORS_API_VERSION);
const uint8_t * const __attribute__((used))
	__attribute__((section(".const_variables_to_keep")))
	VERSION_TEST_NAME(ERRORS_PROJ_NAME) =
				&ERRORS_API_VER_VARIABLE(ERRORS_API_VERSION);
