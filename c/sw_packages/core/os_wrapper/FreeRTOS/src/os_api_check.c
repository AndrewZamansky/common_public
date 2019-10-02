/*
 *
 *   file  :  os_api_check.c
 *
 */



/***************   includes    *******************/


#include "_project_typedefs.h"
#include "_project_defines.h"

#include "os_wrapper.h"
#include "freertos_internal.h"

/* will create pointer to variable os_API_ver_XXXXXXX.
 * during the linkage, if os_ver_XXXXXXX will not be found then
 * it can mean that API and implementation is not compatible
*/
#define _VERSION_TEST_NAME(proj_name)  os_API_test_##proj_name
#define VERSION_TEST_NAME(proj_name)  _VERSION_TEST_NAME(proj_name)

extern const uint8_t OS_API_VER_VARIABLE(OS_API_VERSION);
const uint8_t * const __attribute__((used))
	__attribute__((section(".const_variables_to_keep")))
	VERSION_TEST_NAME(OS_PROJ_NAME) =
				&OS_API_VER_VARIABLE(OS_API_VERSION);
