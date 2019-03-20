/* ****************************************
 *****************************************
	@file   core_init.c

	@date   :
	@project  :
	@description :
***************************************** */

/***************   includes    *******************/
#include "_project.h"

/***************   defines    *******************/




/***************   typedefs    *******************/



/**********   external variables    **************/



extern uint32_t Buttom_Of_Stacks;

/***********   loacal variables    **************/


/******************** default interrupt handlers ******************/




/******************-------------------*****************************/


/******************-------------------*****************************/


void fill_mem32_with_pattern(uint32_t *start , uint32_t *end , uint32_t pattern)
{
	// should be strong less (<) for not to overlap next stack area
	while (start < end)
	{
		*start++ = pattern;
	}
}


/*
 *  function : core_init()
 */
void core_init(uint32_t curr_stack)
{
	/*
	 * assume that jump to  fill_mem32_with_pattern will
	 * not take more than 32 registers in stack
	 */
	curr_stack = (curr_stack & 0xffffffffc) - (4*32);


#if (1==CONFIG_EXCEPTION_STACKS_DEBUG)
	fill_mem32_with_pattern(&Buttom_Of_Stacks,
					(uint32_t *)curr_stack , 0xb1b1b1b1);
#endif
}
