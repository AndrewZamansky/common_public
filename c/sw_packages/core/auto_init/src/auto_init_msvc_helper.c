/*
 *
 *   file  :  auto_init.c
 *
 */



/***************   includes    *******************/


#include "_project_typedefs.h"
#include "_project_defines.h"

#include "auto_init_api.h"



/***************   defines    *******************/

/***************   typedefs    *******************/



/**********   external variables    **************/


/***********   global variables    **************/

#pragma section("init_function_object_section$a", read)
#pragma section("init_function_object_section$c", read)

__declspec(allocate("init_function_object_section$a")) int *init_functions_section_start = (int *)0;
__declspec(allocate("init_function_object_section$c")) int *init_functions_section_end   = (int *)0;
