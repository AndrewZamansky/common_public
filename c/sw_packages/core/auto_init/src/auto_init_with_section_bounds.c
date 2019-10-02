/*
 *
 *   file  :  auto_init_with_section_bounds.c
 *
 */

/***************   includes    *******************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "auto_init_api.h"
#include "auto_init.h"


#if defined(_MSC_VER) || defined(__ARMCC_VERSION)
	extern int *init_functions_section_start ;
	extern int *init_functions_section_end ;
#else
	extern int *__start_auto_init_section ;
	extern int *__stop_auto_init_section ;
#endif

static uint8_t auto_init_done = 0;

/* will create variable auto_init_ver_XXXXXXX that will be referenced from
 * library or application to test that API is matching between libraries
 * and application
*/
const uint8_t AUTO_INIT_API_VER_VARIABLE(AUTO_INIT_API_VERSION);

void auto_init_api(void)
{
	auto_init_struct_t *p_curr_auto_init;
	auto_init_struct_t *p_end_of_auto_init;

	if (auto_init_done) return;

#if defined(_MSC_VER) || defined(__ARMCC_VERSION)
    #pragma message( "change to __start_auto_init_section in " __FILE__ )
	p_curr_auto_init = (auto_init_struct_t *)&init_functions_section_start;
	p_end_of_auto_init = (auto_init_struct_t *)&init_functions_section_end;
#else
	p_curr_auto_init = (auto_init_struct_t *)&__start_auto_init_section;
	p_end_of_auto_init = (auto_init_struct_t *)&__stop_auto_init_section;
#endif

	while(p_curr_auto_init < p_end_of_auto_init)
	{
		uint8_t data_size;
		if((int*)AUTO_INIT_MAGIC_NUMBER == p_curr_auto_init->magic_number)
		{
			auto_init_func_t init_function ;

			init_function = p_curr_auto_init->auto_init_func ;
			init_function();
			data_size = sizeof(auto_init_struct_t);
		}
		else
		{
			/*for compilers that insert spaces between data (like MSVC)*/
			data_size = sizeof(int*);
		}
		p_curr_auto_init =
			(auto_init_struct_t *)( ((uint8_t*)p_curr_auto_init) + data_size);
	}
	auto_init_done = 1;
}
