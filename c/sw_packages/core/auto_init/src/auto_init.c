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
extern auto_init_struct_t auto_init_dummy_auto_init;

static void _auto_init(int16_t struct_size)
{
	auto_init_struct_t *p_curr_auto_init;
	auto_init_func_t init_function ;

	p_curr_auto_init = &auto_init_dummy_auto_init;
	while(AUTO_INIT_MAGIC_NUMBER == p_curr_auto_init->magic_number)
	{
		init_function = p_curr_auto_init->auto_init_func ;
		init_function();
		p_curr_auto_init = (auto_init_struct_t *)( ((uint8_t*)p_curr_auto_init) + struct_size) ;
	}
}

/*
 * function : auto_init_api()
 *
 *
 */
void auto_init_api(void)
{
	_auto_init( (uint8_t)sizeof(auto_init_struct_t));
	_auto_init(- (uint8_t)sizeof(auto_init_struct_t));
}

void dummy_auto_init(void){}

AUTO_INIT_FUNCTION(dummy_auto_init)  ;

