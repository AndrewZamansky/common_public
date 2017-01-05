/*
 *
 *   file  :  auto_init.c
 *
 */

/***************   includes    *******************/
#include "_auto_init_prerequirements_check.h"
#include "auto_init_api.h"

/***************   defines    *******************/

/***************   typedefs    *******************/

/**********   external variables    **************/

/***********   global variables    **************/

/***********   local variables    **************/
extern int *init_functions_section_start ;
extern int *init_functions_section_end ;


void auto_init_api(void)
{
	auto_init_struct_t *p_curr_auto_init;

	p_curr_auto_init = (auto_init_struct_t *)&init_functions_section_start;
	while(p_curr_auto_init < (auto_init_struct_t *)&init_functions_section_end)
	{
		uint8_t increment_value;
		if((int*)AUTO_INIT_MAGIC_NUMBER == p_curr_auto_init->magic_number)
		{
			auto_init_func_t init_function ;

			init_function = p_curr_auto_init->auto_init_func ;
			init_function();
			increment_value = sizeof(auto_init_struct_t);
		}
		else
		{
			increment_value = sizeof(int*);
		}
		p_curr_auto_init = (auto_init_struct_t *)( ((uint8_t*)p_curr_auto_init) + increment_value) ;
	}
}
