/*
 *
 *   file  :  auto_init.c
 *
 */

/***************   includes    *******************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "auto_init_api.h"

#include "_auto_init_prerequirements_check.h"

/***************   defines    *******************/

/***************   typedefs    *******************/

/**********   external variables    **************/

/***********   global variables    **************/

/***********   local variables    **************/
#if defined(COMPILING_FOR_WINDOWS_HOST) && defined(CONFIG_MICROSOFT_COMPILER)
	extern int *init_functions_section_start ;
	extern int *init_functions_section_end ;
#else
	extern int *__start_auto_init_section ;
	extern int *__stop_auto_init_section ;
#endif

void auto_init_api(void)
{
	auto_init_struct_t *p_curr_auto_init;
	auto_init_struct_t *p_end_of_auto_init;


#if defined(COMPILING_FOR_WINDOWS_HOST) && defined(CONFIG_MICROSOFT_COMPILER)
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
}
