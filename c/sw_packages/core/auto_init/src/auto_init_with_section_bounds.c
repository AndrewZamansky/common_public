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
extern int *init_functions_section_start ;
extern int *init_functions_section_end ;

extern int *__start_auto_init_section ;
extern int *__stop_auto_init_section ;

void auto_init_api(void)
{
	auto_init_struct_t *p_curr_auto_init;
	auto_init_struct_t *p_end_of_auto_init;


#if !defined(CONFIG_HEXAGON_COMPILER) && !defined(CONFIG_ANDROID_NDK) \
		&& !defined(CONFIG_XTENSA_GCC) && !defined(CONFIG_XTENSA_XCC)
	#pragma message( "change to __start_auto_init_section in " __FILE__ )
	p_curr_auto_init = (auto_init_struct_t *)&init_functions_section_start;
	p_end_of_auto_init = (auto_init_struct_t *)&init_functions_section_end;
#elif defined(CONFIG_XTENSA_XCC)
	p_curr_auto_init = NULL;
	p_end_of_auto_init = NULL;
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
			data_size = sizeof(int*);
		}
		p_curr_auto_init =
			(auto_init_struct_t *)( ((uint8_t*)p_curr_auto_init) + data_size);
	}
}