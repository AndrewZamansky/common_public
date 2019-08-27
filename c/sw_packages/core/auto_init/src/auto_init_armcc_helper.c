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

extern const __attribute__((used))
	__attribute__((section(".constdata.autoinit_1")))
	int * const init_functions_section_start;
const __attribute__((used))
	__attribute__((section(".constdata.autoinit_1")))
	int * const init_functions_section_start = (int *)0;

extern const __attribute__((used))
	__attribute__((section(".constdata.autoinit_3")))
	int * const init_functions_section_end;
const __attribute__((used))
	__attribute__((section(".constdata.autoinit_3")))
	int * const init_functions_section_end   = (int *)0;
