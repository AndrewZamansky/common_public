/*
 *
 *   file  :  auto_init_without_section_bounds.c
 *
 */

/***************   includes    *******************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "auto_init_api.h"
#include "auto_init.h"


/* will create variable auto_init_ver_XXXXXXX that will be referenced from
 * library or application to test that API is matching between libraries
 * and application
*/
const uint8_t AUTO_INIT_API_VER_VARIABLE(AUTO_INIT_API_VERSION);

extern auto_init_struct_t
					AUTO_INIT_FUNCTION_PLACEMENT auto_init_dummy_auto_init;

static void _auto_init(int16_t struct_size)
{
	auto_init_struct_t const *p_curr_auto_init;
	auto_init_func_t init_function ;

	p_curr_auto_init = &auto_init_dummy_auto_init;
	while((int*)AUTO_INIT_MAGIC_NUMBER == p_curr_auto_init->magic_number)
	{
		init_function = p_curr_auto_init->auto_init_func ;
		init_function();
		p_curr_auto_init =
			(auto_init_struct_t *)( ((uint8_t*)p_curr_auto_init) + struct_size);
	}
}

static uint8_t auto_init_done = 0;

/*
 * function : auto_init_api()
 *
 *
 */
void auto_init_api(void)
{
	if (auto_init_done)
	{
		return;
	}
	_auto_init(   (uint8_t)sizeof(auto_init_struct_t)  );
	_auto_init( -((uint8_t)sizeof(auto_init_struct_t)) );
	auto_init_done = 1;
}

void dummy_auto_init(void){}

AUTO_INIT_FUNCTION(dummy_auto_init);
