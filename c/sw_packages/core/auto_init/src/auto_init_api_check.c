/*
 *
 *   file  :  auto_init_api_check.c
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "auto_init_api.h"
#include "auto_init.h"

/* will create pointer to variable auto_init_API_ver_XXXXXXX.
 * during the linkage, if auto_init_ver_XXXXXXX will not be found then
 * it can mean that API and implementation is not compatible
*/
#define _VERSION_TEST_NAME(proj_name)  auto_init_API_test_##proj_name
#define VERSION_TEST_NAME(proj_name)  _VERSION_TEST_NAME(proj_name)

extern const uint8_t AUTO_INIT_API_VER_VARIABLE(AUTO_INIT_API_VERSION);
const uint8_t * const __attribute__((used))
	__attribute__((section(".const_variables_to_keep")))
	VERSION_TEST_NAME(AUTO_INIT_PROJ_NAME) =
				&AUTO_INIT_API_VER_VARIABLE(AUTO_INIT_API_VERSION);
