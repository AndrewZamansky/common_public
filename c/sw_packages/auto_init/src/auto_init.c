/*
 *
 *   file  :  DevManagment.cpp
 *
 */



/***************   includes    *******************/


#include "_auto_init_prerequirements_check.h" // should be after auto_init_config.h
#include "auto_init_api.h" //place first to test that header file is self-contained



/***************   defines    *******************/

/***************   typedefs    *******************/



/**********   external variables    **************/

extern uint32_t __auto_init_functions_section_start_on_RAM__;
extern uint32_t __auto_init_functions_section_end_on_RAM__;


/***********   global variables    **************/

/***********   local variables    **************/


/*
 * function : DEV_API_open_device()
 *
 *
 */
void auto_init_api(void)
{

	auto_init_func_t curr_init_function ;
	auto_init_func_t last_init_function ;

	curr_init_function = (auto_init_func_t)&__auto_init_functions_section_start_on_RAM__;
	last_init_function = (auto_init_func_t)&__auto_init_functions_section_end_on_RAM__;
	while(curr_init_function < last_init_function)
	{
		curr_init_function();
		curr_init_function++;
	}
}


